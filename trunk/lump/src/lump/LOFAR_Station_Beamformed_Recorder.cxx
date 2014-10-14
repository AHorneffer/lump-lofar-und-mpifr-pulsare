// LOFAR_Station_Beamformed_Recorder.cxx
// base recording program for the LOFAR single-station recording
//_HIST  DATE NAME PLACE INFO
// 2011 May 13  James M Anderson  --MPIfR  start from test_reader.cxx
// 2011 Sep 17  JMA  --updates for LuMP format and required changes to system
// 2012 Jan 04  JMA  --modify socket opening to fully specify UDP access better
// 2012 Jan 28  JMA  --mode socket open and close to Sorter read_thread
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 05  JMA  --add PFB0 format, already added FFT0 at some point
// 2013 Jan 13  JMA  --add Power FFT0 writer
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Aug 30  JMA  ---- add LuMP2 format
// 2013 Sep 05  JMA  ---- add extra parameter to LuMP2 call for child logfile
// 2013 Sep 21  JMA  ---- add option to pre-pad to start time if there are
//                        missing packets at the start
// 2014 Jul 13  JMA  ---- bgfixes to get correct writers called in switch
// 2014 Sep 01  JMA  ---- add FFT1 writer type
// 2014 Sep 05  JMA  ---- add FFT2, PFB1, P_FFT1, P_PFB1 types
// 2014 Sep 16  JMA  --- add VDIF0 type



// Copyright (c) 2011, 2012, 2013, 2014 James M. Anderson <anderson@gfz-potsdam.de>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



// INCLUDES
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "LOFAR_Station_Beamformed_Packet.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include "LOFAR_Station_Beamformed_Writer_FFT0.h"
#include "LOFAR_Station_Beamformed_Writer_FFT1.h"
#include "LOFAR_Station_Beamformed_Writer_FFT2.h"
#include "LOFAR_Station_Beamformed_Writer_LuMP0.h"
#include "LOFAR_Station_Beamformed_Writer_LuMP1.h"
#include "LOFAR_Station_Beamformed_Writer_LuMP2.h"
#include "LOFAR_Station_Beamformed_Writer_PFB0.h"
#include "LOFAR_Station_Beamformed_Writer_PFB1.h"
#include "LOFAR_Station_Beamformed_Writer_P_FFT0.h"
#include "LOFAR_Station_Beamformed_Writer_P_FFT1.h"
#include "LOFAR_Station_Beamformed_Writer_P_PFB0.h"
#include "LOFAR_Station_Beamformed_Writer_P_PFB1.h"
#include "LOFAR_Station_Beamformed_Writer_Packet0.h"
#include "LOFAR_Station_Beamformed_Writer_Power0.h"
#include "LOFAR_Station_Beamformed_Writer_Raw.h"
#include "LOFAR_Station_Beamformed_Writer_Raw0.h"
#include "LOFAR_Station_Beamformed_Writer_Raw1.h"
#include "LOFAR_Station_Beamformed_Writer_Signals.h"
#include "LOFAR_Station_Beamformed_Writer_VDIF0.h"
#include "LOFAR_Station_Beamformed_Sorter.h"
#include "MPIfR_Numerical_sampling_windows.h"
#include "MPIfR_logging.h"
#include "MPIfR_FFTW3.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>





namespace MPIfR {namespace LOGGING {
extern const char PROGRAM_NAME[] = "LOFAR_Station_Beamformed_Recorder";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
}}




// set up a namespace area for stuff.
namespace {



// GLOBALS


// FUNCTIONS
int_fast32_t convert_string_to_char_p_array(const char* const s,
                                            const uint_fast16_t NUM_BEAMLETS,
                                            char* * const restrict array)
{
    // assumes that the caller has pre-allocated memory for the array of char
    // pointers, as well as for the base character memory itself
    const char* ss(s);
    const char* se=NULL;
    const char* s_first=NULL;
    const char* s_last=NULL;
    bool empty_string_warned = false;
    if(ss == NULL) {
        DEF_LOGGING_ERROR("NULL string to convert");
        goto bad_physical_beamlets_string;
    }
    // the first character needs to be a '['
    if(*ss == '[') {}
    else {
        DEF_LOGGING_ERROR("missing [\n");
        goto bad_physical_beamlets_string;
    }
    for(uint_fast32_t b=0; b < NUM_BEAMLETS; b++) {
        if(*ss == 0) {
            DEF_LOGGING_ERROR("end of input string seen at string conversion at beamlet %d\n", int(b));
            goto bad_physical_beamlets_string;
        }
        ss++;
        se = ss;
        while((*se != ',') && (*se != ']') && (*se != 0)) {se++;}
        if(se == ss) {
            if(empty_string_warned) {}
            else {
                empty_string_warned = true;
                DEF_LOGGING_WARNING("zero length string seen at string conversion at beamlet %d\n", int(b));
            }
        }
        s_first = ss;
        while(*s_first == ' ') {s_first++;}
        s_last = se;
        while(s_last[-1] == ' ') {s_last--;}
        if(s_last - s_first <= 1) {
            if(empty_string_warned) {}
            else {
                empty_string_warned = true;
                DEF_LOGGING_WARNING("zero length string seen at string conversion at beamlet %d\n", int(b));
            }
        }
        size_t len = s_last-s_first;
        strncpy(array[b],s_first,len);
        array[b][len] = 0;
        len++;
        if(b < NUM_BEAMLETS-1) {
            array[b+1] = array[b] + len;
        }
        ss=se;
        if((b==NUM_BEAMLETS-1) && (*ss != ']')) {
            DEF_LOGGING_ERROR("missing ] at end of beamlet %d\n",int(b));
            goto bad_physical_beamlets_string;
        }
    }
    return 0;

bad_physical_beamlets_string:
    DEF_LOGGING_CRITICAL("bad beamlets data character string '%s'\n", s);
    exit(2);
    return -1;
}

int_fast32_t convert_string_to_int_array(const char* const s,
                                         const uint_fast16_t NUM_BEAMLETS,
                                         uint_fast32_t* const restrict array)
{
    // assumes that the caller has pre-allocated memory for the array
    const char* ss(s);
    char* se=NULL;
    if(ss == NULL) {
        DEF_LOGGING_ERROR("NULL string to convert");
        goto bad_physical_beamlets_string;
    }
    // the first character needs to be a '['
    if(*ss++ == '[') {}
    else {
        DEF_LOGGING_ERROR("missing [\n");
        goto bad_physical_beamlets_string;
    }
    for(uint_fast32_t b=0; b < NUM_BEAMLETS; b++) {
        errno = 0;
        array[b] = uint_fast32_t(strtoul(ss,&se,0));
        if((errno)) {
            DEF_LOGGING_PERROR("integer conversion failure");
            DEF_LOGGING_ERROR("at integer conversion at beamlet %d\n", int(b));
            goto bad_physical_beamlets_string;
        }
        if(se == ss) {
            DEF_LOGGING_ERROR("in integer conversion at beamlet %d\n", int(b));
            goto bad_physical_beamlets_string;
        }
        ss=se;
        while(((*ss)) && !isdigit(*ss) && (*ss != ']')) {
            ss++;
        }
        if((b==NUM_BEAMLETS-1) && (*ss != ']')) {
            DEF_LOGGING_ERROR("missing ] at end of beamlet %d\n",int(b));
            goto bad_physical_beamlets_string;
        }
    }
    return 0;

bad_physical_beamlets_string:
    DEF_LOGGING_CRITICAL("bad beamlets integer data string '%s'\n", s);
    exit(2);
    return -1;
}

int_fast32_t convert_string_to_double_array(const char* const s,
                                            const uint_fast16_t NUM_BEAMLETS,
                                            Real64_t* const restrict array)
{
    // assumes that the caller has pre-allocated memory for the array
    const char* ss(s);
    char* se=NULL;
    if(ss == NULL) {
        DEF_LOGGING_ERROR("NULL string to convert");
        goto bad_physical_beamlets_string;
    }
    // the first character needs to be a '['
    if(*ss++ == '[') {}
    else {
        DEF_LOGGING_ERROR("missing [\n");
        goto bad_physical_beamlets_string;
    }
    for(uint_fast32_t b=0; b < NUM_BEAMLETS; b++) {
        errno = 0;
        array[b] = strtod(ss,&se);
        if((errno)) {
            DEF_LOGGING_PERROR("double conversion failure");
            DEF_LOGGING_ERROR("at double conversion at beamlet %d\n", int(b));
            goto bad_physical_beamlets_string;
        }
        if(se == ss) {
            DEF_LOGGING_ERROR("in double conversion at beamlet %d\n", int(b));
            goto bad_physical_beamlets_string;
        }
        ss=se;
        while(((*ss)) && !(isdigit(*ss) || (*ss == '+') || (*ss == '-') || (*ss == ']'))) {
            ss++;
        }
        if((b==NUM_BEAMLETS-1) && (*ss != ']')) {
            DEF_LOGGING_ERROR("missing ] at end of beamlet %d\n",int(b));
            goto bad_physical_beamlets_string;
        }
    }
    return 0;

bad_physical_beamlets_string:
    DEF_LOGGING_CRITICAL("bad beamlets data double string '%s'\n", s);
    exit(2);
    return -1;
}


int_fast32_t init(void) throw()
{
    int_fast32_t return_code = MPIfR::Numerical::FFTW3::init_MPIfR_FFTW3();
    if((return_code)) {
        DEF_LOGGING_ERROR("init_MPIfR_FFTW3 returned %d", int(return_code));
        exit(1);
    }
    return 0;
}

int_fast32_t shutdown(void) throw()
{
    int_fast32_t return_code = MPIfR::Numerical::FFTW3::shutdown_MPIfR_FFTW3();
    if((return_code)) {
        DEF_LOGGING_ERROR("shutdown_MPIfR_FFTW3 returned %d", int(return_code));
        exit(1);
    }
    return 0;
}





}  // end namespace












int main(int argc, char* argv[])
{
    fprintf(stderr, "%s starting\n", MPIfR::LOGGING::PROGRAM_NAME);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    fprintf(stderr, "Warning: only tested on little-endian systems\n");
#endif
    if(argc < 18) {
        fprintf(stderr, "Error: this program ('%s') should only be run from the LOFAR_Station_Beamformed_Recorder.py script\n", argv[0]);
        exit(2);
    }

    int argpos = 1;
    char* endptr;

    
    const char* const logging_filename = argv[argpos++];
    MPIfR::LOGGING::LOGGING_BLANK_.reset_display(stdout,MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_INFO);
    MPIfR::LOGGING::LOGGING_BLANK_.reset_disk(logging_filename,MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET);
    DEF_LOGGING_INFO("%s [version %s, build date %sT%s] starting\n", MPIfR::LOGGING::PROGRAM_NAME, MPIfR::LOGGING::PROGRAM_VERSION, MPIfR::LOGGING::PROGRAM_DATE, MPIfR::LOGGING::PROGRAM_TIME);

    {
        int_fast32_t return_code = init();
        if((return_code)) {
            DEF_LOGGING_ERROR("init returned %d", int(return_code));
            exit(1);
        }
    }

    {
        for(int a=0; a < argc; a++) {
            DEF_LOGGING_DEBUG("argument %5d is '%s'", a, argv[a]);
        }
    }
    

    // get the main information
    const uint32_t start_time(strtoul(argv[argpos++],NULL,0));
    const uint32_t end_time(strtoul(argv[argpos++],NULL,0));
    if(end_time <= start_time) {
        DEF_LOGGING_CRITICAL("end not after start time.\n");
        exit(2);
    }
    const char* const port_p = argv[argpos++];
    errno=0;
    endptr=NULL;
    const int_fast32_t data_type_in_int(strtol(argv[argpos++],&endptr,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for beamformed data type protocol.\n");
        exit(2);
    }
    if((data_type_in_int < MPIfR::LOFAR::L_intComplex8_t)
      || (data_type_in_int > MPIfR::LOFAR::L_intComplex32_t)) {
        DEF_LOGGING_CRITICAL("not aware that the LOFAR station supports this beamformed data type protocol (%d).\n", int(data_type_in_int));
        exit(2);
    }
    const MPIfR::LOFAR::LOFAR_raw_data_type_enum data_type_in(static_cast<MPIfR::LOFAR::LOFAR_raw_data_type_enum>(data_type_in_int));
    const uint_fast16_t CLOCK_SPEED_IN_MHz(strtoul(argv[argpos++],NULL,0));
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG == 0)
    if(CLOCK_SPEED_IN_MHz == 0) {
        DEF_LOGGING_CRITICAL("bad value for CLOCK_SPEED_IN_MHz (%d)\n", int(CLOCK_SPEED_IN_MHz));
        exit(2);
    }
#endif
    const uint_fast16_t NUM_Beamlets(strtoul(argv[argpos++],NULL,0));
    if(NUM_Beamlets == 0) {
        DEF_LOGGING_CRITICAL("bad value for NUM_Beamlets (%d)\n", int(NUM_Beamlets));
        exit(2);
    }
    const uint_fast16_t SAMPLES_PER_PACKET(strtoul(argv[argpos++],NULL,0));
    if(SAMPLES_PER_PACKET == 0) {
        DEF_LOGGING_CRITICAL("bad value for SAMPLES_PER_PACKET (%d)\n", int(SAMPLES_PER_PACKET));
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast32_t PHYSICAL_BEAMLET_OFFSET(strtoul(argv[argpos++],&endptr,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for PHYSICAL_BEAMLET_OFFSET.\n");
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast32_t NUM_PACKETS_IN_READ_THREAD_BUFFER(strtoul(argv[argpos++],NULL,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for NUM_PACKETS_IN_READ_THREAD_BUFFER.\n");
        exit(2);
    }
    if(NUM_PACKETS_IN_READ_THREAD_BUFFER == 0) {
        DEF_LOGGING_CRITICAL("bad value for NUM_PACKETS_IN_READ_THREAD_BUFFER (%d)\n", int(NUM_PACKETS_IN_READ_THREAD_BUFFER));
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast32_t NUM_PACKETS_IN_RING_BUFFER(strtoul(argv[argpos++],NULL,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for NUM_PACKETS_IN_RING_BUFFER.\n");
        exit(2);
    }
    if(NUM_PACKETS_IN_RING_BUFFER == 0) {
        DEF_LOGGING_CRITICAL("bad value for NUM_PACKETS_IN_RING_BUFFER (%d)\n", int(NUM_PACKETS_IN_RING_BUFFER));
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE(strtoul(argv[argpos++],NULL,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE.\n");
        exit(2);
    }
    if(NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE == 0) {
        DEF_LOGGING_CRITICAL("bad value for NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE (%d)\n", int(NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE));
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast64_t NUM_FIRST_PACKETS_TO_DROP(strtoull(argv[argpos++],&endptr,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for NUM_FIRST_PACKETS_TO_DROP.\n");
        exit(2);
    }
    errno=0;
    endptr=NULL;
    const uint_fast32_t MAX_NUM_PACKETS_OVER_TIME(strtoul(argv[argpos++],&endptr,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for MAX_NUM_PACKETS_OVER_TIME.\n");
        exit(2);
    }
    const uint_fast32_t NUM_WRITERS(strtoul(argv[argpos++],NULL,0));
    if(NUM_WRITERS == 0) {
        DEF_LOGGING_CRITICAL("bad value for NUM_WRITERS (%d)\n", int(NUM_WRITERS));
        exit(2);
    }
    const char* const STATION_NAME = argv[argpos++];
    errno=0;
    endptr=NULL;
    const uint8_t BACKFILL_TO_START_TIME(strtoul(argv[argpos++],&endptr,0));
    if((errno != 0) || (endptr == argv[argpos-1])) {
        DEF_LOGGING_CRITICAL("invalid number for BACKFILL_TO_START_TIME");
        exit(2);
    }

    DEF_LOGGING_INFO("starting sorter object\n");
    MPIfR::LOFAR::Station::
        RSP_beamformed_packet_sorter sorter(port_p,
                                            data_type_in,
                                            NUM_Beamlets,
                                            SAMPLES_PER_PACKET,
                                            NUM_PACKETS_IN_RING_BUFFER,
                                            NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE,
                                            NUM_PACKETS_IN_READ_THREAD_BUFFER,
                                            NUM_FIRST_PACKETS_TO_DROP,
                                            MAX_NUM_PACKETS_OVER_TIME,
                                            CLOCK_SPEED_IN_MHz,
                                            ((BACKFILL_TO_START_TIME==0)?false:true));



    
    
    DEF_LOGGING_INFO("starting writer objects\n");
    // Now read in the information for the writers
    if(argc != int(argpos+26*NUM_WRITERS)) {
        DEF_LOGGING_CRITICAL("wrong number of arguments.  This program ('%s') should only be run from the LOFAR_Station_Beamformed_Recorder.py script\n", argv[0]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Writer_Base* restrict * restrict WRITERS = new MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Writer_Base*[NUM_WRITERS];
    for(uint_fast32_t w=0; w < NUM_WRITERS; w++) {
        errno=0;
        endptr=NULL;
        const int_fast32_t writer_type_int(strtol(argv[argpos++],&endptr,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for writer_type_int.\n");
            exit(2);
        }
        if((writer_type_int < MPIfR::LOFAR::Station::LOFAR_DOFF_RAW_OUT)
          || (writer_type_int >= MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT_MAX)) {
            DEF_LOGGING_CRITICAL("invalid writer type %d.\n", int(writer_type_int));
            exit(2);
        }
        const MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT writer_type(static_cast<MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT>(writer_type_int));
        const char* const filename_base = argv[argpos++];
        const uint_fast16_t NUM_Output_Beamlets(strtoul(argv[argpos++],NULL,0));
        if(NUM_Output_Beamlets == 0) {
            DEF_LOGGING_CRITICAL("bad value for NUM_Output_Beamlets (%d) for WRITER %d\n", int(NUM_Output_Beamlets), int(w));
            exit(2);
        }
        uint_fast32_t* Physical_Beamlets_Array = new uint_fast32_t[NUM_Output_Beamlets];
        convert_string_to_int_array(argv[argpos++],
                                    NUM_Output_Beamlets,
                                    Physical_Beamlets_Array);
        uint_fast32_t* Subband_Array = new uint_fast32_t[NUM_Output_Beamlets];
        convert_string_to_int_array(argv[argpos++],
                                    NUM_Output_Beamlets,
                                    Subband_Array);
        uint_fast32_t* RCUMODE_Array = new uint_fast32_t[NUM_Output_Beamlets];
        convert_string_to_int_array(argv[argpos++],
                                    NUM_Output_Beamlets,
                                    RCUMODE_Array);
        Real64_t* RightAscension_Array = new Real64_t[NUM_Output_Beamlets];
        convert_string_to_double_array(argv[argpos++],
                                       NUM_Output_Beamlets,
                                       RightAscension_Array);
        Real64_t* Declination_Array = new Real64_t[NUM_Output_Beamlets];
        convert_string_to_double_array(argv[argpos++],
                                       NUM_Output_Beamlets,
                                       Declination_Array);
        char** Epoch_Array = new char*[NUM_Output_Beamlets];
        char* Epoch_Array_Data = new char[strlen(argv[argpos])+1];
        Epoch_Array[0] = Epoch_Array_Data;
        convert_string_to_char_p_array(argv[argpos++],
                                       NUM_Output_Beamlets,
                                       Epoch_Array);
        char** SourceName_Array = new char*[NUM_Output_Beamlets];
        char* SourceName_Array_Data = new char[strlen(argv[argpos])+1];
        SourceName_Array[0] = SourceName_Array_Data;
        convert_string_to_char_p_array(argv[argpos++],
                                       NUM_Output_Beamlets,
                                       SourceName_Array);
        errno=0;
        endptr=NULL;
        const int_fast32_t data_type_process_int(strtol(argv[argpos++],NULL,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for data_type_process_int.\n");
            exit(2);
        }
        if((data_type_process_int < MPIfR::LOFAR::L_int1_t)
          || (data_type_process_int >= MPIfR::LOFAR::L_RAW_DATA_TYPE_MAX)) {
            DEF_LOGGING_CRITICAL("not aware that the LOFAR software supports this data type protocol (%d).\n", int(data_type_process_int));
            exit(2);
        }
        const MPIfR::LOFAR::LOFAR_raw_data_type_enum data_type_process(static_cast<MPIfR::LOFAR::LOFAR_raw_data_type_enum>(data_type_process_int));
        errno=0;
        endptr=NULL;
        const int_fast32_t data_type_out_int(strtol(argv[argpos++],NULL,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for data_type_out_int.\n");
            exit(2);
        }
        if((data_type_out_int < MPIfR::LOFAR::L_int1_t)
          || (data_type_out_int >= MPIfR::LOFAR::L_RAW_DATA_TYPE_MAX)) {
            DEF_LOGGING_CRITICAL("not aware that the LOFAR software supports this data type protocol (%d).\n", int(data_type_out_int));
            exit(2);
        }
        const MPIfR::LOFAR::LOFAR_raw_data_type_enum data_type_out(static_cast<MPIfR::LOFAR::LOFAR_raw_data_type_enum>(data_type_out_int));
        const uint_fast32_t NUM_CHANNELS(strtoul(argv[argpos++],NULL,0));
        if(NUM_CHANNELS == 0) {
            DEF_LOGGING_CRITICAL("bad value for NUM_CHANNELS (%d)\n", int(NUM_CHANNELS));
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const uint_fast16_t NUM_TAPS(strtoul(argv[argpos++],&endptr,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for NUM_TAPS.\n");
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const int_fast32_t window_function_int(strtol(argv[argpos++],&endptr,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for window_function_int.\n");
            exit(2);
        }
        if((window_function_int < MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Rectangular)
          || (window_function_int >= MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_UNKNOWN)) {
            DEF_LOGGING_CRITICAL("not aware that the LOFAR software supports this windowing function protocol (%d).\n", int(window_function_int));
            exit(2);
        }
        const MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window_function(static_cast<MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM>(window_function_int));
        errno=0;
        endptr=NULL;
        const Real64_t WINDOW_PARAMETER(strtod(argv[argpos++],&endptr));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for WINDOW_PARAMETER.\n");
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const Real64_t CHANNELIZER_SHIFT(strtod(argv[argpos++],&endptr));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for CHANNELIZER_SHIFT.\n");
            exit(2);
        }
        if(CHANNELIZER_SHIFT <= 0.0) {
            DEF_LOGGING_CRITICAL("bad value for CHANNELIZER_SHIFT (%E)\n", CHANNELIZER_SHIFT);
            exit(2);
        }
        else if(CHANNELIZER_SHIFT != 1.0) {
            DEF_LOGGING_CRITICAL("CHANNELIZER_SHIFT (%E) not supported at this time\n", CHANNELIZER_SHIFT);
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const Real64_t INTEGRATION_TIME(strtod(argv[argpos++],&endptr));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for INTEGRATION_TIME.\n");
            exit(2);
        }
        if(INTEGRATION_TIME <= 0.0) {
            DEF_LOGGING_CRITICAL("bad value for INTEGRATION_TIME (%E)\n", INTEGRATION_TIME);
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const uint8_t SCALE_BY_INVERSE_NUM_SAMPLES(strtoul(argv[argpos++],&endptr,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for SCALE_BY_INVERSE_NUM_SAMPLES.\n");
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const Real64_t EXTRA_SCALE_FACTOR(strtod(argv[argpos++],&endptr));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for EXTRA_SCALE_FACTOR.\n");
            exit(2);
        }
        if(EXTRA_SCALE_FACTOR <= 0.0) {
            DEF_LOGGING_CRITICAL("bad value for EXTRA_SCALE_FACTOR (%E)\n", EXTRA_SCALE_FACTOR);
            exit(2);
        }
        errno=0;
        endptr=NULL;
        const uint8_t BOUNDS_CHECK_CONVERSION(strtoul(argv[argpos++],&endptr,0));
        if((errno != 0) || (endptr == argv[argpos-1])) {
            DEF_LOGGING_CRITICAL("invalid number for BOUNDS_CHECK_CONVERSION.\n");
            exit(2);
        }
        const char* const Generic_String_0 = argv[argpos++];
        const char* const Generic_String_1 = argv[argpos++];
        const char* const Generic_String_2 = argv[argpos++];
        const char* const Generic_String_3 = argv[argpos++];
        const char* const Generic_String_4 = argv[argpos++];
        DEF_LOGGING_DEBUG("have generic strings 0 '%s' 1 '%s' 2 '%s' 3 '%s' 4 '%s'",
                          Generic_String_0, Generic_String_1,
                          Generic_String_2, Generic_String_3,
                          Generic_String_4);
        
        
        switch(writer_type) {
        case MPIfR::LOFAR::Station::LOFAR_DOFF_RAW_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_Raw(filename_base,
                                                    STATION_NAME,
                                                    start_time,
                                                    CLOCK_SPEED_IN_MHz,
                                                    PHYSICAL_BEAMLET_OFFSET,
                                                    NUM_Beamlets,
                                                    SAMPLES_PER_PACKET,
                                                    data_type_in,
                                                    int32_t(w),
                                                    NUM_CHANNELS,
                                                    NUM_Output_Beamlets,
                                                    Physical_Beamlets_Array,
                                                    Subband_Array,
                                                    RCUMODE_Array,
                                                    RightAscension_Array,
                                                    Declination_Array,
                                                    Epoch_Array,
                                                    SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_RAW0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_Raw0(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_RAW1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_Raw1(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_POWER0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_Power0(filename_base,
                                                       STATION_NAME,
                                                       start_time,
                                                       CLOCK_SPEED_IN_MHz,
                                                       PHYSICAL_BEAMLET_OFFSET,
                                                       NUM_Beamlets,
                                                       SAMPLES_PER_PACKET,
                                                       data_type_in,
                                                       int32_t(w),
                                                       NUM_CHANNELS,
                                                       NUM_Output_Beamlets,
                                                       Physical_Beamlets_Array,
                                                       Subband_Array,
                                                       RCUMODE_Array,
                                                       RightAscension_Array,
                                                       Declination_Array,
                                                       Epoch_Array,
                                                       SourceName_Array,
                                                       INTEGRATION_TIME,
                                                       SCALE_BY_INVERSE_NUM_SAMPLES,
                                                       EXTRA_SCALE_FACTOR,
                                                       data_type_out,
                                                       BOUNDS_CHECK_CONVERSION);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_LuMP0(filename_base,
                                                      STATION_NAME,
                                                      start_time,
                                                      CLOCK_SPEED_IN_MHz,
                                                      PHYSICAL_BEAMLET_OFFSET,
                                                      NUM_Beamlets,
                                                      SAMPLES_PER_PACKET,
                                                      data_type_in,
                                                      int32_t(w),
                                                      NUM_CHANNELS,
                                                      NUM_Output_Beamlets,
                                                      Physical_Beamlets_Array,
                                                      Subband_Array,
                                                      RCUMODE_Array,
                                                      RightAscension_Array,
                                                      Declination_Array,
                                                      Epoch_Array,
                                                      SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_FFT0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_FFT0(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array,
                                                     data_type_process,
                                                     data_type_out,
                                                     window_function,
                                                     WINDOW_PARAMETER,
                                                     EXTRA_SCALE_FACTOR);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_PFB0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_PFB0(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_TAPS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array,
                                                     data_type_process,
                                                     data_type_out,
                                                     window_function,
                                                     WINDOW_PARAMETER,
                                                     EXTRA_SCALE_FACTOR);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_POWER_FFT0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_P_FFT0(filename_base,
                                                       STATION_NAME,
                                                       start_time,
                                                       CLOCK_SPEED_IN_MHz,
                                                       PHYSICAL_BEAMLET_OFFSET,
                                                       NUM_Beamlets,
                                                       SAMPLES_PER_PACKET,
                                                       data_type_in,
                                                       int32_t(w),
                                                       NUM_CHANNELS,
                                                       NUM_Output_Beamlets,
                                                       Physical_Beamlets_Array,
                                                       Subband_Array,
                                                       RCUMODE_Array,
                                                       RightAscension_Array,
                                                       Declination_Array,
                                                       Epoch_Array,
                                                       SourceName_Array,
                                                       data_type_process,
                                                       data_type_out,
                                                       window_function,
                                                       WINDOW_PARAMETER,
                                                       EXTRA_SCALE_FACTOR,
                                                       INTEGRATION_TIME,
                                                       SCALE_BY_INVERSE_NUM_SAMPLES,
                                                       BOUNDS_CHECK_CONVERSION);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_POWER_PFB0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_P_PFB0(filename_base,
                                                       STATION_NAME,
                                                       start_time,
                                                       CLOCK_SPEED_IN_MHz,
                                                       PHYSICAL_BEAMLET_OFFSET,
                                                       NUM_Beamlets,
                                                       SAMPLES_PER_PACKET,
                                                       data_type_in,
                                                       int32_t(w),
                                                       NUM_CHANNELS,
                                                       NUM_TAPS,
                                                       NUM_Output_Beamlets,
                                                       Physical_Beamlets_Array,
                                                       Subband_Array,
                                                       RCUMODE_Array,
                                                       RightAscension_Array,
                                                       Declination_Array,
                                                       Epoch_Array,
                                                       SourceName_Array,
                                                       data_type_process,
                                                       data_type_out,
                                                       window_function,
                                                       WINDOW_PARAMETER,
                                                       EXTRA_SCALE_FACTOR,
                                                       INTEGRATION_TIME,
                                                       SCALE_BY_INVERSE_NUM_SAMPLES,
                                                       BOUNDS_CHECK_CONVERSION);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_LuMP1(filename_base,
                                                      STATION_NAME,
                                                      start_time,
                                                      CLOCK_SPEED_IN_MHz,
                                                      PHYSICAL_BEAMLET_OFFSET,
                                                      NUM_Beamlets,
                                                      SAMPLES_PER_PACKET,
                                                      data_type_in,
                                                      int32_t(w),
                                                      NUM_CHANNELS,
                                                      NUM_Output_Beamlets,
                                                      Physical_Beamlets_Array,
                                                      Subband_Array,
                                                      RCUMODE_Array,
                                                      RightAscension_Array,
                                                      Declination_Array,
                                                      Epoch_Array,
                                                      SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_Packet0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_Packet0(filename_base,
                                                        STATION_NAME,
                                                        start_time,
                                                        CLOCK_SPEED_IN_MHz,
                                                        PHYSICAL_BEAMLET_OFFSET,
                                                        NUM_Beamlets,
                                                        SAMPLES_PER_PACKET,
                                                        data_type_in,
                                                        int32_t(w),
                                                        NUM_CHANNELS,
                                                        NUM_Output_Beamlets,
                                                        Physical_Beamlets_Array,
                                                        Subband_Array,
                                                        RCUMODE_Array,
                                                        RightAscension_Array,
                                                        Declination_Array,
                                                        Epoch_Array,
                                                        SourceName_Array);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_VDIF0_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_VDIF0(filename_base,
                                                      STATION_NAME,
                                                      start_time,
                                                      CLOCK_SPEED_IN_MHz,
                                                      PHYSICAL_BEAMLET_OFFSET,
                                                      NUM_Beamlets,
                                                      SAMPLES_PER_PACKET,
                                                      data_type_in,
                                                      int32_t(w),
                                                      NUM_CHANNELS,
                                                      NUM_Output_Beamlets,
                                                      Physical_Beamlets_Array,
                                                      Subband_Array,
                                                      RCUMODE_Array,
                                                      RightAscension_Array,
                                                      Declination_Array,
                                                      Epoch_Array,
                                                      SourceName_Array,
                                                      Generic_String_0,
                                                      Generic_String_1);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP2_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_LuMP2(filename_base,
                                                      STATION_NAME,
                                                      start_time,
                                                      end_time,
                                                      CLOCK_SPEED_IN_MHz,
                                                      PHYSICAL_BEAMLET_OFFSET,
                                                      NUM_Beamlets,
                                                      SAMPLES_PER_PACKET,
                                                      NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE,
                                                      data_type_in,
                                                      int32_t(w),
                                                      NUM_CHANNELS,
                                                      NUM_Output_Beamlets,
                                                      Physical_Beamlets_Array,
                                                      Subband_Array,
                                                      RCUMODE_Array,
                                                      RightAscension_Array,
                                                      Declination_Array,
                                                      Epoch_Array,
                                                      SourceName_Array,
                                                      Generic_String_0,
                                                      Generic_String_1,
                                                      Generic_String_2,
                                                      Generic_String_3);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_FFT1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_FFT1(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array,
                                                     data_type_process,
                                                     data_type_out,
                                                     window_function,
                                                     WINDOW_PARAMETER,
                                                     EXTRA_SCALE_FACTOR);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_PFB1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_PFB1(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_TAPS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array,
                                                     data_type_process,
                                                     data_type_out,
                                                     window_function,
                                                     WINDOW_PARAMETER,
                                                     EXTRA_SCALE_FACTOR);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_POWER_FFT1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_P_FFT1(filename_base,
                                                       STATION_NAME,
                                                       start_time,
                                                       CLOCK_SPEED_IN_MHz,
                                                       PHYSICAL_BEAMLET_OFFSET,
                                                       NUM_Beamlets,
                                                       SAMPLES_PER_PACKET,
                                                       data_type_in,
                                                       int32_t(w),
                                                       NUM_CHANNELS,
                                                       NUM_Output_Beamlets,
                                                       Physical_Beamlets_Array,
                                                       Subband_Array,
                                                       RCUMODE_Array,
                                                       RightAscension_Array,
                                                       Declination_Array,
                                                       Epoch_Array,
                                                       SourceName_Array,
                                                       data_type_process,
                                                       data_type_out,
                                                       window_function,
                                                       WINDOW_PARAMETER,
                                                       EXTRA_SCALE_FACTOR,
                                                       INTEGRATION_TIME,
                                                       SCALE_BY_INVERSE_NUM_SAMPLES,
                                                       BOUNDS_CHECK_CONVERSION);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_POWER_PFB1_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_P_PFB1(filename_base,
                                                       STATION_NAME,
                                                       start_time,
                                                       CLOCK_SPEED_IN_MHz,
                                                       PHYSICAL_BEAMLET_OFFSET,
                                                       NUM_Beamlets,
                                                       SAMPLES_PER_PACKET,
                                                       data_type_in,
                                                       int32_t(w),
                                                       NUM_CHANNELS,
                                                       NUM_TAPS,
                                                       NUM_Output_Beamlets,
                                                       Physical_Beamlets_Array,
                                                       Subband_Array,
                                                       RCUMODE_Array,
                                                       RightAscension_Array,
                                                       Declination_Array,
                                                       Epoch_Array,
                                                       SourceName_Array,
                                                       data_type_process,
                                                       data_type_out,
                                                       window_function,
                                                       WINDOW_PARAMETER,
                                                       EXTRA_SCALE_FACTOR,
                                                       INTEGRATION_TIME,
                                                       SCALE_BY_INVERSE_NUM_SAMPLES,
                                                       BOUNDS_CHECK_CONVERSION);
            break;
        case MPIfR::LOFAR::Station::LOFAR_DOFF_FFT2_OUT:
            WRITERS[w] = new MPIfR::LOFAR::Station::
                LOFAR_Station_Beamformed_Writer_FFT2(filename_base,
                                                     STATION_NAME,
                                                     start_time,
                                                     CLOCK_SPEED_IN_MHz,
                                                     PHYSICAL_BEAMLET_OFFSET,
                                                     NUM_Beamlets,
                                                     SAMPLES_PER_PACKET,
                                                     data_type_in,
                                                     int32_t(w),
                                                     NUM_CHANNELS,
                                                     NUM_Output_Beamlets,
                                                     Physical_Beamlets_Array,
                                                     Subband_Array,
                                                     RCUMODE_Array,
                                                     RightAscension_Array,
                                                     Declination_Array,
                                                     Epoch_Array,
                                                     SourceName_Array,
                                                     data_type_process,
                                                     data_type_out,
                                                     window_function,
                                                     WINDOW_PARAMETER,
                                                     EXTRA_SCALE_FACTOR,
                                                     Generic_String_0);
            break;
        default:
            DEF_LOGGING_CRITICAL("writer type %d not yet supported. Contact the developer\n", int(writer_type));
            exit(2);
        }
        delete[] SourceName_Array_Data;
        delete[] SourceName_Array;
        delete[] Epoch_Array_Data;
        delete[] Epoch_Array;
        delete[] Declination_Array;
        delete[] RightAscension_Array;
        delete[] RCUMODE_Array;
        delete[] Subband_Array;
        delete[] Physical_Beamlets_Array;
    }

    //DEF_LOGGING_DEBUG("writer made\n");
    int_fast32_t sig_retval = MPIfR::LOFAR::Station::Signals::setup_user_signal(NULL);
    if(sig_retval != 0) {
        DEF_LOGGING_CRITICAL("setting up signal handler failed\n");
        exit(1);
    }



    DEF_LOGGING_INFO("starting packet reading\n");
    int_fast32_t retval = sorter.write_data(start_time,
                                            end_time,
                                            start_time,
                                            NUM_WRITERS,
                                            WRITERS
                                            );
        
    DEF_LOGGING_INFO("retval %d Final status is %d\n", int(retval), int(sorter.Final_Status()));


    DEF_LOGGING_INFO("deleting writers\n");
    for(uint_fast32_t w=0; w < NUM_WRITERS; w++) {
        delete WRITERS[w];
    }
    delete[] WRITERS; WRITERS=0;
    {
        int_fast32_t return_code = shutdown();
        if((return_code)) {
            DEF_LOGGING_ERROR("shutdown returned %d", int(return_code));
            exit(1);
        }
    }
    MPIfR::LOFAR::Station::Signals::shutdown_user_signal();
    DEF_LOGGING_INFO("finishing\n");
    return int(retval);
}

