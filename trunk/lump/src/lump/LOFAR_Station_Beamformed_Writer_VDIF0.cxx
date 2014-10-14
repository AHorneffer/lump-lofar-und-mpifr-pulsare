// LOFAR_Station_Beamformed_Writer_VDIF0.cxx
// writes out LOFAR VDIF frames
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 09  James M Anderson  --- start from LOFAR_Station_Beamformed_Writer_LuMP2.cxx
// 2014 Sep 19  JMA  --- try to detect whether or not the Python startup
//                       program has put a .XX extension on the filename_base
//                       provided by the user, as the .XX is not allowed by the
//                       VLBI filenaming standard.
// 2014 Sep 20  JMA  --- change to use DAS number for VDIF output
// 2014 Oct 14  JMA  --- use LuMP VDIF  writer version instead of version
//                       revision number in VDIF extended header



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



// Raw Data Format
// written in "wb" mode for all files
//
// Data file:  One data file is written out for all VDIF frames written
// by this writer.  For information on the VLBI Data Interchange Format (VDIF),
// see http://vlbi.org/vdif/.  This code was based on the specification in
// http://vlbi.org/vdif/docs/VDIF_specification_Release_1.1.1.pdf
// (document dated 2014 June, accessed 2014-09-04).  Because the raw LOFAR
// beamlet data taken using the 200 MHz sampling clock do not start at
// integer clock second marks for every second (there is a non-integer number of
// samples per second), the 200 MHz-sampled LOFAR bemaformed data cannot
// be put into a fully VDIF-1.1.1-compliant VDIF frame.  An attempt is made
// to make the LOFAR data come as close as possible, by having the
// "seconds_from_reference_epoch" value refer to the last second at which the
// LOFAR data started sampling at the integer clock tick mark, and the
// "data_frame_within_second" value counts frames from that "secondary reference
// second", so that the actual data time will for some frames be higher than
// the "seconds_from_reference_epoch", and the end-user must calculate the
// actual time, both sub-second and second, from the "data_frame_within_second",
// sampling rate, and "seconds_from_reference_epoch" (plus epoch).  See the
// LuMP VDIF specification document.  In order to fully fill out the VDIF data
// area of each VDIF frame, that is forced by the VDIF spec to be a multiple of
// 8 bytes in size, and to meet the requirement that all frames have the same
// size, even more powers of two seconds must be folded into the same
// data frame reference second, to make a constnt integer number of samples
// per frame, with as much as possible of the data area filled.  Thus, this
// writer will update the second marker only once every 8 seconds.
// The filename is
// <filename_base>_<station_code>_<scan_name>_<data start time>_<auxinfo1>_<auxinfo2>.vdif
// where the angle brackets (< and >) delimit the field names used to generate
// the file name and are not generated in the output file name.
// filename_base is the filename_base provided to the writer, and is supposed
// to be the experiment code as per VLBI file naming conventions.
// station_code is the decimal version of the LOFAR station code.
// scan_name is some text string provided as extra argument to the writer.
// data start time is in the VLBI (VEX) time format of the start of the data
// in the file, and is generated from stYYYYyDDDdHHhMMmSS,
// where the lowercase letters stay as they are, and
// YYYY is replaced by the year, DDD by the day of year, HH by the hour, MM by
// the minute, and SS by the second (truncated).
// auxinfo1 is generated from fdXXXX-<total # chans>-<bits/sample>-<# threads>
// where XXXX is the string XXXX, as the LOFAR data rate is not in general an
// integer number of megabits per second,
// total # chans is the total number of channels written out in the file
// (for LOFAR/LuMP this is 2 times the number of output beamlets),
// bits/sample is the number of bits for thereal part of the complex number
// (corresponding exactly to the LOFAR bit-size usage), and
// # threads is the total number of threads, which for LOFAR is the number
// of output beamlets.
// auxinfo2 is generated from wtXX, where wt is the character code meaning writer
// thread (the id variable), and XX is the 2 digit hexadecimal representation of
// the thread id.
// The data are written as complete VDIF frames.
//
//
// Information file:
// Filename: filename_base.info
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Beamlets file:
// Filename: filename_base.beamlets_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Subbands file:
// Filename: filename_base.subbands_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// RCUMODEs file:
// Filename: filename_base.rcumodes_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Right Ascensions file:
// Filename: filename_base.rightascensions_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Declinations file:
// Filename: filename_base.declinations_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Epochs file:
// Filename: filename_base.epochs_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// SourceNames file:
// Filename: filename_base.sourcenames_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
//
// Valid file.  A single file is written out to indicate which samples are
// valid (data came from the station, non-zero value) or invalid (no data
// available from the station, default value written to the data area, zero
// valued data written to the valid file).
// The filename is
// filename_base.valid
// The valid flags are written as
// individual bits to 8-bit bytes
// [0 1 2 3 4 5 6 7] [0 1 2 3 4 5 6 7] [0 1 2 3 4 5 6 7]
// So the valid signal for sample x is read as, assuming variables are unsigned
// byte = x >> 3
// bit  = x & 0x7
// signal = (valid_array[byte] >> bit) & 0x1
//
// Filename file:
// A file of name
// filename_base.file_lis
// is written.  It contains a single \n terminated string with the name of the
// filename_base.raw









// INCLUDES
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif
#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif
#ifndef __USE_ISOC99
#  define __USE_ISOC99 1
#endif
#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif
#ifndef __USE_MISC
#  define __USE_MISC 1
#endif
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <inttypes.h>
#include <limits>
#ifdef __cplusplus
#  include <cstddef>
#else
#  include <stddef.h>
#endif
#include <stdint.h>
// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#include <time.h>

#include "JMA_math.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_Writer_VDIF0.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>









namespace {
size_t check_for_LuMP_Python_extended_filename_base(const char* const f,
                                                    int32_t id)
{
    size_t l = strlen(f);
    if(l > 3) {
        if(  (f[l-3] == '.')
          && (isxdigit(f[l-2]))
          && (isxdigit(f[l-1])) ) {
            DEF_LOGGING_WARNING("It looks like the filename_base ('%s') provided to VDIF0 writer %" PRId32 " has been mangled by the LuMP Python control scripts (extension of the form .XX added) --- trying to remove this to be compliant with the VLBI filename conventions", f, id);
            l -= 3;
        }
    }
    return l;
}
}



// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {

namespace {
extern "C" void* VDIF_writer_thread_entry(void* pthis)
{
    LOFAR_Station_Beamformed_Writer_VDIF0* object = reinterpret_cast<LOFAR_Station_Beamformed_Writer_VDIF0*>(pthis);
    return object->start_VDIF_writer_thread();
}

static const char* const VDIF_FILENAME_EXTENSION = ".vdif";
static const char* const VDIF0_DEFAULT_SCAN_NAME = "VDIF0";


}









// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_VDIF0::
LOFAR_Station_Beamformed_Writer_VDIF0(const char* const restrict filename_base_,
                                      const char* const restrict station_name_,
                                      const uint32_t reference_time_,
                                      const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                      const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                      const uint_fast16_t NUM_Beamlets_,
                                      const uint_fast16_t NUM_Blocks_,
                                      const LOFAR_raw_data_type_enum DATA_TYPE_,
                                      const int32_t id_,
                                      const uint_fast32_t NUM_OUTPUT_CHANNELS_,
                                      const uint_fast16_t NUM_Output_Beamlets_,
                                      const uint_fast32_t* const restrict Physical_Beamlets_Array_,
                                      const uint_fast32_t* const restrict Physical_Subband_Array_,
                                      const uint_fast32_t* const restrict RCUMODE_Array_,
                                      const Real64_t* const restrict RightAscension_Array_,
                                      const Real64_t* const restrict Declination_Array_,
                                      const char* const * const restrict Epoch_Array_,
                                      const char* const * const restrict SourceName_Array_,
                                      const char* const scan_name_,
                                      const char* const DAS_name_)
    restrict throw()
            :
        LOFAR_Station_Beamformed_Writer_Base(filename_base_,
                                             station_name_,
                                             reference_time_,
                                             CLOCK_SPEED_IN_MHz_,
                                             PHYSICAL_BEAMLET_OFFSET_,
                                             NUM_Beamlets_,
                                             NUM_Blocks_,
                                             DATA_TYPE_,
                                             id_,
                                             NUM_OUTPUT_CHANNELS_,
                                             NUM_Output_Beamlets_,
                                             Physical_Beamlets_Array_,
                                             Physical_Subband_Array_,
                                             RCUMODE_Array_,
                                             RightAscension_Array_,
                                             Declination_Array_,
                                             Epoch_Array_,
                                             SourceName_Array_),
    fp_data(NULL),
    fp_file_list(NULL),
    num_raw_packet_commands_processed(0),
    initialized(0),
    status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
    valid_object(0),
    VDIF_FRAME_SIZE_BYTES(0),
    VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR(0),
    VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES(0),
    VDIF_frames_boss(0),
    VDIF_frames_VDIF_writer(0),
    current_VDIF_reference_second(0),
    current_VDIF_reference_epoch_unix_timestamp(0),
    num_time_sample_lines_in_boss_VDIF_storage(0),
    num_valid_time_sample_lines_in_boss_VDIF_storage(0),
    VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE(0),
    current_num_vdif_frames_since_reference_second_stride(0),
    VDIF_frames_initialization_stage(0),
    LuMP_data_aquisition_system(0),
    Scan_Name(0),
    VDIF_filename(0),
    VDIF_writer_thread_running(0),
    VDIF_writer_thread_ready(0),
    boss_has_VDIF_writer_thread_ready_lock(0),
    thread_has_VDIF_writer_thread_ready_lock(0),
    VDIF_writer_thread_work_code(LOFAR_WRITER_BASE_WORK_WAIT),
    VDIF_writer_thread_status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
    VDIF_writer_thread_write_command_count(0),
    VDIF_writer_thread_write_byte_count(0)
{
    // stuff for our own VDIF_writer_thread
    int retval = pthread_mutex_init(&VDIF_writer_thread_ready_mutex, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing VDIF_writer_thread mutex failed");
        DEF_LOGGING_CRITICAL("initializing VDIF_writer_thread %d mutex failed with %d\n", id, retval);
        exit(1);
    }
    retval = pthread_cond_init(&VDIF_writer_thread_go_to_work_condition, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing VDIF_writer_thread cond failed");
        DEF_LOGGING_CRITICAL("initializing VDIF_writer_thread %d cond failed with %d\n", id, retval);
        exit(1);
    }
    retval = pthread_create(&VDIF_writer_thread, NULL, VDIF_writer_thread_entry, this);    
    if((retval)) {
        DEF_LOGGING_PERROR("creating VDIF_writer_thread failed");
        DEF_LOGGING_CRITICAL("creating VDIF_writer_thread %d failed with %d\n", id, retval);
        exit(1);
    }

    copy_string_information(scan_name_);
    LuMP_data_aquisition_system = determine_LuMP_data_aquisition_system(DAS_name_);

    
    int_fast32_t retcode = wait_for_thread_sleeping();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to init properly\n", int(id));
        exit(1);
    }
    retcode = init_writer_processing();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to send init command\n", int(id));
        exit(1);
    }
    

    retval = int(wait_for_VDIF_writer_thread_startup());
    if((retval)) {
        DEF_LOGGING_PERROR("wait_for_VDIF_writer_thread_startup failed");
        DEF_LOGGING_CRITICAL("wait_for_VDIF_writer_thread_startup WRITER %d failed with %d\n", id, retval);
        exit(1);
    }
    retcode = wait_for_VDIF_writer_thread_sleeping();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to init properly\n", int(id));
        exit(1);
    }
    retcode = tell_VDIF_writer_thread_to_init();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to send init command to VDIF_writer_thread\n", int(id));
        exit(1);
    }

    return;
}

LOFAR_Station_Beamformed_Writer_VDIF0::
~LOFAR_Station_Beamformed_Writer_VDIF0()
{
    stop_thread();

    stop_VDIF_writer_thread();
    Writer_VDIF0_BOSS_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    pthread_mutex_destroy(&VDIF_writer_thread_ready_mutex);
    pthread_cond_destroy(&VDIF_writer_thread_go_to_work_condition);
    
    
    DEF_LOGGING_INFO("WRITER %d VDIF_writer_thread statistics", int(id));
    DEF_LOGGING_INFO("WRITER %" PRId32 " 0x%16.16" PRIXFAST64 " write commands processed", id, VDIF_writer_thread_write_command_count);
    DEF_LOGGING_INFO("WRITER %" PRId32 " 0x%16.16" PRIXFAST64 " VDIF frames written to output file", id, VDIF_writer_thread_write_frame_count);
    DEF_LOGGING_INFO("WRITER %" PRId32 " 0x%16.16" PRIXFAST64 " bytes written to output file", id, VDIF_writer_thread_write_byte_count);

    if(Scan_Name != VDIF0_DEFAULT_SCAN_NAME) {
        free(reinterpret_cast<void*>(const_cast<char*>(Scan_Name)));
    }
    Scan_Name=0;
    delete[] VDIF_frames_boss; VDIF_frames_boss=0;
    delete[] VDIF_frames_VDIF_writer; VDIF_frames_VDIF_writer=0;
    return;
}









int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
copy_string_information(const char* const scan_name_) throw()
{
    if(scan_name_ == NULL) {
        DEF_LOGGING_WARNING("no scan name provided for VDIF0 output: using '%s'", VDIF0_DEFAULT_SCAN_NAME);
        Scan_Name = VDIF0_DEFAULT_SCAN_NAME;
        return 0;
    }
    if(scan_name_[0] == 0) {
        DEF_LOGGING_WARNING("no scan name provided for VDIF0 output: using '%s'", VDIF0_DEFAULT_SCAN_NAME);
        Scan_Name = VDIF0_DEFAULT_SCAN_NAME;
        return 0;
    }
    size_t scan_name_size = strlen(scan_name_)+1;
    if(scan_name_size == 0) {
        DEF_LOGGING_CRITICAL("empty scan name string for VDIF0: using '%s'", VDIF0_DEFAULT_SCAN_NAME);
        Scan_Name = VDIF0_DEFAULT_SCAN_NAME;
        return 0;
    }
    char* temp = static_cast<char*>(malloc(scan_name_size));
    if(temp == NULL) {
        DEF_LOGGING_CRITICAL("could not malloc %llu bytes for scan_name_", (unsigned long long)(scan_name_size));
        exit(1);
    }
    strcpy(temp, scan_name_);
    Scan_Name = const_cast<const char*>(temp);
    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
thread_constructor() restrict
{
    if((check_input_parameters())) return -1;
    if((set_up_work_buffer_areas())) return -2;
    if((open_standard_files())) return -5;

    return write_header_init();
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
thread_destructor() restrict
{
    close_output_files();
    delete valid_object; valid_object=0;
    // don't free the output_storage here --- do it in the main destructor
    // after the VDIF_writer_thread is finished.

    DEF_LOGGING_INFO("WRITER %" PRId32 " processed 0x%16.16" PRIXFAST64 " raw packet commands\n", id, num_raw_packet_commands_processed);

    return 0;
}    










int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
check_input_parameters() restrict throw()
{
    if(NUM_OUTPUT_CHANNELS != 1) {
        DEF_LOGGING_CRITICAL("coded for 1 output channel, but have %u instead\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    if(NUM_RSP_BEAMLET_POLARIZATIONS != 2) {
        DEF_LOGGING_CRITICAL("coded for 2 polarizations, but have %d instead\n",
                int(NUM_RSP_BEAMLET_POLARIZATIONS));
        exit(1);
    }
    if(NUM_Output_Beamlets <= 0) {
        DEF_LOGGING_CRITICAL("Invalid number of output beamlets (%d) for this writer\n",
                (int)(NUM_Output_Beamlets));
        exit(1);
    }
    return 0;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
set_up_work_buffer_areas() restrict throw()
{
    // Calculate the sizes of the VDIF frames, and the size of a single
    // time sample line in the buffer.
    VDIF_FRAME_SIZE_BYTES =  LOFAR_VDIF_Packet_LOFAR0::calculate_LuMP_VDIF_frame_size(DATA_TYPE, CLOCK_SPEED_IN_MHz);
    VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES = LOFAR_VDIF_Packet_LOFAR0::calculate_LuMP_VDIF_time_samples_per_frame(DATA_TYPE, CLOCK_SPEED_IN_MHz);
    VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE = LOFAR_VDIF_Packet_LOFAR0::calculate_LuMP_VDIF_num_frames_per_reference_second_stride(DATA_TYPE, CLOCK_SPEED_IN_MHz);
    // We always write both polarization channels of each beamlet to the same
    // VDIF thread.
    VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR = LOFAR_raw_data_type_enum_size(DATA_TYPE) * 2;
    //
    // Get the station ID number, taking into account stations with
    // ring splitters (see the info class header for more information)
    
    const char* restrict station_number = Info.get_station_name()+3;
    int_fast32_t offset = 1000;
    if(!isdigit(*station_number)) {
        offset = 0;
        station_number++;
    }
    int_fast32_t station_id(std::atoi(station_number));
    if(station_id <= 0) {
        // Not possible in the LOFAR naming scheme --- something bad happend.
        DEF_LOGGING_ERROR("cannot parse station name '%s' to get LOFAR station ID number", Info.get_station_name());
        exit(2);
    }
    station_id += offset;
    DEF_LOGGING_DEBUG("have VDIF station_id=%" PRIdFAST32, station_id);
    uint_fast32_t VDIF_writer_type = 0;
    {
        char type_string[2];
        const char* s = LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(LOFAR_DATA_OUTPUT_FILE_FORMAT(WRITER_TYPE()));
        if(strlen(s) > 15) {
            type_string[0] = s[15];
        }
        else {
            type_string[0] = '0';
        }
        type_string[1] = 0;
        VDIF_writer_type = uint_fast32_t(strtoul(type_string, NULL, 16));
    }
    // Now, allocate space for the VDIF frames we will use to accumulate
    // the data before writing out to disk.  We allocate two sets of frames,
    // to allow one to be accumulating data while the other is busy being
    // written to disk.
    try {
        VDIF_frames_boss = new LOFAR_VDIF_Packet_LOFAR0* restrict[NUM_Beamlets];
        VDIF_frames_VDIF_writer = new LOFAR_VDIF_Packet_LOFAR0* restrict[NUM_Beamlets];
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            VDIF_frames_boss[b] = new LOFAR_VDIF_Packet_LOFAR0(DATA_TYPE,
                                                               CLOCK_SPEED_IN_MHz,
                                                               Beamlets_Array[b]+PHYSICAL_BEAMLET_OFFSET,
                                                               Physical_Subband_Array[b],
                                                               RCUMODE_Array[b],
                                                               uint_fast32_t(station_id),
                                                               VDIF_writer_type);
            VDIF_frames_VDIF_writer[b] = new LOFAR_VDIF_Packet_LOFAR0(DATA_TYPE,
                                                                      CLOCK_SPEED_IN_MHz,
                                                                      Beamlets_Array[b]+PHYSICAL_BEAMLET_OFFSET,
                                                                      Physical_Subband_Array[b],
                                                                      RCUMODE_Array[b],
                                                                      uint_fast32_t(station_id),
                                                                      VDIF_writer_type);
        }
    }
    catch(const std::bad_alloc& e) {
        DEF_LOGGING_ERROR("could not allocate memory with new because of '%s'", e.what());
        exit(1);
    }
    catch(const LOFAR_VDIF_Packet_exception_no_memory& e) {
        DEF_LOGGING_ERROR("VDIF found no memory '%s'", e.what());
        exit(1);
    }
    catch(const LOFAR_VDIF_Packet_exception_bad_frame_size& e) {
        DEF_LOGGING_ERROR("Frame size %zu seems to be bad for VDIF, '%s'", VDIF_FRAME_SIZE_BYTES, e.what());
        exit(1);
    }
    catch(const LOFAR_VDIF_Packet_exception_no_extended_user_data& e) {
        DEF_LOGGING_ERROR("Bad software development: missing extended header area for VDIF '%s'", e.what());
        exit(1);
    }
    catch(...) {
        DEF_LOGGING_CRITICAL("unknown exception happened --- barfing");
        exit(1);
    }
    VDIF_frames_initialization_stage=1;

    // indicate that we have no data in the current boss buffer
    num_time_sample_lines_in_boss_VDIF_storage = 0;

    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
open_standard_files() restrict throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating file filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    fp_file_list = fopen(new_filename,"wbx");
#else
    fp_file_list = fopen("/dev/null","wb");
#endif
    if(fp_file_list == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output file file '%s'\n",
                new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    
    retval = snprintf(new_filename, FSIZE, "%s.valid", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating valid filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    valid_object = new LOFAR_Station_Beamformed_Valid_Writer(new_filename);
#else
    valid_object = new LOFAR_Station_Beamformed_Valid_Writer("/dev/null");
#endif

    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
close_output_files() restrict throw()
{
    if((fp_data)) {
        int retval = fclose(fp_data); fp_data = NULL;
        if((retval)) {
            DEF_LOGGING_PERROR("closing raw data file failed");
            DEF_LOGGING_ERROR("closing '%s' data failed\n", filename_base);
            status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
        }
    }
    if((fp_file_list)) {
        DEF_LOGGING_ERROR("file list file was not properly closed in program");
        int retval = fclose(fp_file_list); fp_file_list = NULL;
        if((retval)) {
            DEF_LOGGING_PERROR("closing file list file failed");
            DEF_LOGGING_ERROR("closing '%s' file list file failed\n", filename_base);
            status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
        }
    }
    free(VDIF_filename); VDIF_filename=0;
            
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    // DEF_LOGGING_DEBUG("do_work %d (%s) for LOFAR_Station_Beamformed_Writer_VDIF0 WRITER %d\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id));
    switch(work_code_copy) {
    case LOFAR_WRITER_BASE_WORK_WAIT:
        status = LOFAR_WRITER_RETVAL_WORK_IS_ZERO;
        break;
    case LOFAR_WRITER_BASE_WORK_INIT:
        status = thread_constructor();
        break;
    case LOFAR_WRITER_BASE_WORK_TIME:
        status = write_header_start();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_DATA:
        status = write_standard_packets();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA:
        status = write_partial_packet();
        break;
    case LOFAR_WRITER_BASE_WORK_SHUTDOWN:
        status = write_header_end();
        break;
    case LOFAR_WRITER_BASE_WORK_EXIT:
        thread_destructor();
        break;
    default:
        status = LOFAR_WRITER_RETVAL_UNKNOWN_WORK;
        break;
    }
    // DEF_LOGGING_DEBUG("do_work %d (%s) for LOFAR_Station_Beamformed_Writer_VDIF0 WRITER %d has finished, status is %d\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id), int(status));
    return status;
}











int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
write_header_init() restrict throw()
{
    // set up Info area
    Info.set_WRITER_TYPE(uint16_t(WRITER_TYPE()));
    LOFAR_STATION_BEAMFORMED_WRITER_BASE_SET_INFO_VERSION_DATE();
    Info.set_processing_DATA_TYPE(int32_t(DATA_TYPE));
    Info.set_output_DATA_TYPE(int32_t(DATA_TYPE));
    Info.set_channelizer_TYPE(int32_t(LOFAR_CHANNELIZER_NONE));
    Info.set_NUM_CHANNELS(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_channelization(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_integration(1);
    Info.set_seconds_per_channelization_full(SECONDS_PER_LINE);
    Info.set_seconds_per_channelization_eff(SECONDS_PER_LINE);
    Info.set_samples_per_channelization_eff(NUM_OUTPUT_CHANNELS);
    Info.set_seconds_per_integration_full(SECONDS_PER_LINE);
    Info.set_seconds_per_integration_eff(SECONDS_PER_LINE);
    Info.set_samples_per_integration_eff(1);
    Info.set_seconds_per_output(SECONDS_PER_LINE);
    Info.set_integration_multiplier(1.0);


        
    if(status == int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)) {
        // Good
    }
    else {
        goto write_header_status_bad;
    }
    
    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_init");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
write_header_start() restrict throw()
{
    int_fast32_t retcode;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==0)) {
        // Good
    }
    else {
        goto write_header_status_bad;
    }

    // We have information about the first sample received from the LOFAR
    // station.  Use this and the LOFAR reference second to calculate how much we
    // need to backfil into the VDIF buffers.  This will also adjust the
    // first_sample_received value to account for back-filling the VDIF frame with
    // blank data.  If also dumps out invalid marks to the valid data buffer.
    retcode = initialize_VDIF_frame_data_areas();
    if((retcode)) {
        goto write_header_error;
    }
    // OK, now set the start time of the first sample in the first VDIF frame.
    Info.set_sample_start(int64_t(first_sample_received));
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    {
        Real64_t time_center_output_zero =
            first_sample_received * SECONDS_PER_LINE;
        Info.set_time_center_output_zero(time_center_output_zero);
    }

    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }


    {
        // open the output VDIF file now that we can construct the filename
        retcode = generate_VDIF_filename();
        if((retcode)) {
            DEF_LOGGING_ERROR("could not generate output VDIF filename for writer %" PRId32, id);
            exit(2);
        }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
        fp_data = fopen(VDIF_filename,"wbx");
#else
        fp_data = fopen("/dev/null","wb");
#endif
        if(fp_data == NULL) {
            DEF_LOGGING_CRITICAL("unable to open raw output data file '%s'\n",
                                 VDIF_filename);
            status=int32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
            exit(2);
        }
        int retval = fputs(strip_path(VDIF_filename),fp_file_list);
        if(retval<0) {
            DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                                 VDIF_filename);
            status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
            exit(2);
        }
        retval = fputc('\n', fp_file_list);
        if(retval<0) {
            DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                                 VDIF_filename);
            status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
            exit(2);
        }
        retval = fclose(fp_file_list);
        if(retval != 0) {
            DEF_LOGGING_PERROR("Error closing file");
            DEF_LOGGING_CRITICAL("Error closing file file from base '%s'\n", filename_base);
            exit(2);
        }
        fp_file_list = NULL;
    }


    initialized=1;
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_start");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
write_header_end() restrict throw()
{
    int_fast32_t VDIF_writer_return_code;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_header_status_bad;
    }

    VDIF_writer_return_code = tell_VDIF_writer_thread_to_end_data();
    if((VDIF_writer_return_code)) {
        goto VDIF_writer_status_bad;
    }

    {
        uint_fast64_t num_time_samples = VDIF_writer_thread_write_frame_count * VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES;
        Info.set_num_output_timeslots(uint64_t(num_time_samples));
    }


    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    initialized=2;
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_end");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
VDIF_writer_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_CHILD_FAILURE);
    return status;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
write_standard_packets() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing packet command number %llX line %llX\n", (unsigned long long)num_raw_packet_commands_processed, (unsigned long long)current_sample_offset);

    int_fast32_t return_val;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams

    switch(DATA_TYPE) {
    case L_intComplex8_t:
        return_val = write_block_to_VDIF_frames<uint16_t>(TWOS_COMPLEMENT_intComplex8_t_DOUBLE);
        break;
    case L_intComplex16_t:
        return_val = write_block_to_VDIF_frames<uint32_t>(TWOS_COMPLEMENT_intComplex16_t_DOUBLE);
        break;
    case L_intComplex32_t:
        return_val = write_block_to_VDIF_frames<uint64_t>(TWOS_COMPLEMENT_intComplex32_t_DOUBLE);
        break;
    default:
        {
            DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d\n",
                                 int(DATA_TYPE));
            exit(1);
        }
    }
    if((return_val)) {
        goto write_data_error;
    }
    // write to the valid stream
    for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
        if(valid_object->write_N_points(NUM_Blocks,current_valid[packet]) == 0){
        }
        else {
            goto write_data_error;
        }
        if((current_valid[packet])) {
            num_valid_samples += NUM_Blocks;
        }
        else {
            num_invalid_samples += NUM_Blocks;
        }
        last_sample_written_plus_1 += NUM_Blocks;
    }
    num_packets_received += current_num_packets;
    ++num_raw_packet_commands_processed;
    return status;
write_data_error:
    report_file_error("write_standard_packets");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    int_fast32_t return_val;
    const uint_fast16_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the data stream
    if(NUM_ACTUAL_BLOCKS > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("Error in write_partial_packet(), got %llX %llX sample positions\n",
                (unsigned long long)(current_sample_offset),
                (unsigned long long)(current_sample_stop_offset));
        exit(1);
    }
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        return_val = write_block_to_VDIF_frames_partial<uint16_t>(TWOS_COMPLEMENT_intComplex8_t_DOUBLE);
        break;
    case L_intComplex16_t:
        return_val = write_block_to_VDIF_frames_partial<uint32_t>(TWOS_COMPLEMENT_intComplex16_t_DOUBLE);
        break;
    case L_intComplex32_t:
        return_val = write_block_to_VDIF_frames_partial<uint64_t>(TWOS_COMPLEMENT_intComplex32_t_DOUBLE);
        break;
    default:
        {
            DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d\n",
                                 int(DATA_TYPE));
            exit(1);
        }
    }
    if((return_val)) {
        goto write_data_error;
    }
    // write to the valid stream
    if(valid_object->write_N_points(NUM_ACTUAL_BLOCKS,
                                    current_valid[0]) == 0){
    }
    else {
        goto write_data_error;
    }
    if((current_valid[0])) {
        num_valid_samples += NUM_ACTUAL_BLOCKS;
    }
    else {
        num_invalid_samples += NUM_ACTUAL_BLOCKS;
    }
    last_sample_written_plus_1 += NUM_ACTUAL_BLOCKS;
    ++num_packets_received;
    return status;
write_data_error:
    report_file_error("write_partial_packet");
    close_output_files();
    status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}













///////////////////////////////////////////////////////////////////////////////
// boss functions to send signals to VDIF_writer_thread to do things
///////////////////////////////////////////////////////////////////////////////



int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
tell_VDIF_writer_thread_to_init() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_VDIF_writer_thread_to_init");
    {
        int_fast32_t retval = wait_for_VDIF_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    VDIF_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_INIT;
    
    Writer_VDIF0_BOSS_BROADCAST_CONDITION(&VDIF_writer_thread_go_to_work_condition);
    Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
tell_VDIF_writer_thread_to_send_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_VDIF_writer_thread_to_send_data");
    // calculate valid fraction
    Real64_t valid_fraction = num_valid_time_sample_lines_in_boss_VDIF_storage/(Real64_t)(VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES);
    //DEF_LOGGING_DEBUG("valid_fraction=%E from %" PRIdFAST64 " of %" PRIdFAST64 " time samples", valid_fraction, num_valid_time_sample_lines_in_boss_VDIF_storage, VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES);
    if(valid_fraction >= 0.5) {
        for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
            VDIF_frames_boss[b]->set_data_valid();
        }
    }
    else {
        for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
            VDIF_frames_boss[b]->set_data_invalid();
        }
    }
    {
        int_fast32_t retval = wait_for_VDIF_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    // Now swap the boss and writer frame pointers
    LOFAR_VDIF_Packet_LOFAR0* restrict * restrict temp=VDIF_frames_boss;
    VDIF_frames_boss = VDIF_frames_VDIF_writer;
    VDIF_frames_VDIF_writer = temp;
    // Tell the writer to go off and write.
    VDIF_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_WRITE_DATA;
    Writer_VDIF0_BOSS_BROADCAST_CONDITION(&VDIF_writer_thread_go_to_work_condition);
    Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);

    // Now increment the frames written, check the seconds, and update
    // the VDIF headers.
    ++current_num_vdif_frames_since_reference_second_stride;
    if(current_num_vdif_frames_since_reference_second_stride == VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE) {
        current_num_vdif_frames_since_reference_second_stride = 0;
        current_VDIF_reference_second += LOFAR_VDIF_Packet_LOFAR0::NUM_LOFAR_SECONDS_PER_VDIF_SECOND_STRIDE;
    }
    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
        VDIF_frames_boss[b]->set_seconds_from_reference_epoch(uint32_t(current_VDIF_reference_second));
        VDIF_frames_boss[b]->set_data_frame_number_within_second(uint32_t(current_num_vdif_frames_since_reference_second_stride));
    }
    // zero the boss counters
    num_time_sample_lines_in_boss_VDIF_storage = 0;
    num_valid_time_sample_lines_in_boss_VDIF_storage = 0;
    
    
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
tell_VDIF_writer_thread_to_send_partial_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_VDIF_writer_thread_to_send_partial_data");
    int_fast32_t retval = flush_VDIF_frame_data_areas();
    if((retval)) {
        return retval;
    }
    // Writing of the flushed frames is handled by a call to
    // tell_VDIF_writer_thread_to_send_data() within flush_VDIF_frame_data_areas()
    // so we don't need to handle anything extra here.
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
tell_VDIF_writer_thread_to_end_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_VDIF_writer_thread_to_end_data");
    {
        int_fast32_t retval = wait_for_VDIF_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    VDIF_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_SHUTDOWN;
    
    Writer_VDIF0_BOSS_BROADCAST_CONDITION(&VDIF_writer_thread_go_to_work_condition);
    Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
stop_VDIF_writer_thread() restrict throw()
{
    //DEF_LOGGING_INFO("WRITER %d stopping VDIF_writer_thread:\n", int(id));
    {
        int_fast32_t retval = wait_for_VDIF_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //Writer_VDIF0_BOSS_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    if(VDIF_writer_thread_running != 2) {
        VDIF_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_EXIT;
        
        Writer_VDIF0_BOSS_BROADCAST_CONDITION(&VDIF_writer_thread_go_to_work_condition);
    }
    else {
        // continue
    }
    Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    
    void* join_retval;
    int retval = pthread_join(VDIF_writer_thread, &join_retval);
    if((retval)) {
        DEF_LOGGING_PERROR("joining VDIF_writer_thread failed");
        DEF_LOGGING_CRITICAL("joining VDIF_writer_thread %d failed with %d\n", int(id), retval);
        exit(1);
    }
    if(join_retval != NULL) {
        DEF_LOGGING_WARNING("VDIF_writer_thread %d returned %p\n", int(id), join_retval);
    }
    DEF_LOGGING_INFO("WRITER %d VDIF_writer_thread stopped:\n", int(id));

    if(VDIF_writer_thread_running == 2) {
        return 0;
    }
    return -4;
}






///////////////////////////////////////////////////////////////////////////////
// VDIF_writer_thread functions for doing things
///////////////////////////////////////////////////////////////////////////////

void* LOFAR_Station_Beamformed_Writer_VDIF0::
start_VDIF_writer_thread() restrict
{
    //DEF_LOGGING_DEBUG("start_VDIF_writer_thread\n");
    Writer_VDIF0_THREAD_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    if(VDIF_writer_thread_running == 0) {
        // ok, continue
    }
    else {
        Writer_VDIF0_THREAD_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
        return (void*)(-1);
    }
    VDIF_writer_thread_running = 1;
    Writer_VDIF0_THREAD_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    //DEF_LOGGING_DEBUG("start_VDIF_writer_thread running thread\n");
    return run_VDIF_writer_thread();
}
    
    

void* LOFAR_Station_Beamformed_Writer_VDIF0::
run_VDIF_writer_thread() restrict
{
    //DEF_LOGGING_DEBUG("starting run_VDIF_writer_thread WRITER %d setting work condition to WAIT\n", id);
    Writer_VDIF0_THREAD_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    while(1) {
        // inform the boss that we are ready for new data
        VDIF_writer_thread_ready = 1;
        //DEF_LOGGING_DEBUG("VDIF_writer_thread %d setting work condition to WAIT\n", id);
        // wait for an instruction
        while(VDIF_writer_thread_work_code == LOFAR_WRITER_BASE_WORK_WAIT) {
            Writer_VDIF0_THREAD_CONDITION_WAIT(&VDIF_writer_thread_go_to_work_condition,
                                              &VDIF_writer_thread_ready_mutex);
        }
        LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_copy = VDIF_writer_thread_work_code;
        //DEF_LOGGING_DEBUG("VDIF_writer_thread %d go_to_work %d\n", id, int(work_code));
        VDIF_writer_thread_ready = 0;
        //DEF_LOGGING_DEBUG("VDIF_writer_thread %d go_to_work %d\n", int(id), int(work_code));
        Writer_VDIF0_THREAD_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
        // check the condition value and do some work
        int_fast32_t return_code = VDIF_writer_thread_do_work(work_copy);
        //DEF_LOGGING_DEBUG("VDIF_writer_thread %d got %d back from do_work\n", int(id), int(return_code));
        // Now that the work is done, tell the boss that the packets are free
        // to be re-used
        Writer_VDIF0_THREAD_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
        VDIF_writer_thread_work_code = LOFAR_WRITER_BASE_WORK_WAIT;
        if((return_code)) {
            break;
        }
        if(work_copy==LOFAR_WRITER_BASE_WORK_EXIT) {
            break;
        }
    }
    VDIF_writer_thread_ready=0;
    VDIF_writer_thread_running=2;
    Writer_VDIF0_THREAD_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    //DEF_LOGGING_DEBUG("VDIF_writer_thread %d exiting\n", id);
    pthread_exit(NULL);
    // never reached
    return NULL;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
wait_for_VDIF_writer_thread_startup() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_VDIF_writer_thread_startup WRITER %d\n", int(id));
    int_fast32_t return_code = 0;
    while(1) {
        Writer_VDIF0_BOSS_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_VDIF_writer_thread_startup 2 thread %d VDIF_writer_thread_ready %d work %d\n", int(id),int(VDIF_writer_thread_ready), int(VDIF_writer_thread_work_code));
        if(VDIF_writer_thread_running==2) {
            // thread died
            return_code = -1;
            break;
        }
        else if(VDIF_writer_thread_running == 0) {
            // wait for the thread to finish starting up
        }
        else if((VDIF_writer_thread_ready)) {
            return_code = 0;
            break;
        }
        Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    }
    Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    return return_code;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
wait_for_VDIF_writer_thread_sleeping() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_VDIF_writer_thread_sleeping in WRITER %d\n", int(id));
    while(1) {
        Writer_VDIF0_BOSS_LOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_VDIF_writer_thread_sleeping 2 WRITER %d VDIF_writer_thread_ready %d work %d (%s)\n", int(id),int(VDIF_writer_thread_ready), int(VDIF_writer_thread_work_code), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(VDIF_writer_thread_work_code));
        if(VDIF_writer_thread_running==2) {
            // thread died
            return -1;
        }
        else if(VDIF_writer_thread_running == 0) {
            // thread should already have started, wait_for_VDIF_writer_thread_startup
            // was called in constructor
            return -2;
        }
        else if(VDIF_writer_thread_work_code != LOFAR_WRITER_BASE_WORK_WAIT) {
            // wait for the thread to finish
        }
        else if((VDIF_writer_thread_ready)) {
            return 0;
        }
        Writer_VDIF0_BOSS_UNLOCK_MUTEX(&VDIF_writer_thread_ready_mutex);
    }
    // never reached
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
VDIF_writer_thread_do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    // DEF_LOGGING_DEBUG("VDIF_writer_thread_do_work got work code %d (%s) for LOFAR_Station_Beamformed_Writer_VDIF0 WRITER %d\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id));
    switch(work_code_copy) {
    case LOFAR_WRITER_BASE_WORK_WAIT:
        VDIF_writer_thread_status = LOFAR_WRITER_RETVAL_WORK_IS_ZERO;
        break;
    case LOFAR_WRITER_BASE_WORK_INIT:
        VDIF_writer_thread_status = LOFAR_WRITER_RETVAL_GOOD;
        break;
    case LOFAR_WRITER_BASE_WORK_TIME:
        VDIF_writer_thread_status = VDIF_writer_thread_constructor();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_DATA:
        VDIF_writer_thread_status = VDIF_writer_thread_write_buffer();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA:
        VDIF_writer_thread_status = VDIF_writer_thread_write_buffer();
        break;
    case LOFAR_WRITER_BASE_WORK_SHUTDOWN:
        VDIF_writer_thread_status = VDIF_writer_thread_stop_writing();
        break;
    case LOFAR_WRITER_BASE_WORK_EXIT:
        VDIF_writer_thread_destructor();
        break;
    default:
        VDIF_writer_thread_status = LOFAR_WRITER_RETVAL_UNKNOWN_WORK;
        break;
    }
    // DEF_LOGGING_DEBUG("VDIF_writer_thread_do_work finished work code %d (%s) for LOFAR_Station_Beamformed_Writer_VDIF0 WRITER %d with result %d (%s)\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id), int(VDIF_writer_thread_status), LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Str(LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM(VDIF_writer_thread_status)));
    return VDIF_writer_thread_status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
VDIF_writer_thread_constructor() restrict throw()
{
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
VDIF_writer_thread_destructor() restrict throw()
{
    return 0;
}    




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
VDIF_writer_thread_write_buffer() restrict throw()
{
    //DEF_LOGGING_DEBUG("VDIF_writer_thread_write_buffer Writing to child process");
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    if(VDIF_writer_thread_running != 1) {
        // should not be here
        DEF_LOGGING_ERROR("the VDIF_writer_thread is not running");
        goto write_data_running_bad;
    }
    
    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
        // header is always in network format, but we may need to byte swap
        // the data
        VDIF_frames_VDIF_writer[b]->convert_data_to_from_host_format();
        // now write
        if(fwrite(VDIF_frames_VDIF_writer[b]->frame_pointer_const(), VDIF_FRAME_SIZE_BYTES, 1, fp_data) != 1) {
            DEF_LOGGING_ERROR("write to VDIF datastream for writer %" PRId32 " failed for beamlet %" PRIdFAST16, id, b);
            goto write_data_error;
        }

        VDIF_writer_thread_write_byte_count += VDIF_FRAME_SIZE_BYTES;
        ++VDIF_writer_thread_write_frame_count;
    }
    ++VDIF_writer_thread_write_command_count;
    // // DEBUG print for beamlet 3
    // {
    //     const uint32_t* const restrict dp = (const uint32_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(VDIF_frames_VDIF_writer[3]->frame_pointer_const(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
    //     for(uint_fast32_t i=0; i < 16; ++i) {
    //         DEF_LOGGING_DEBUG("LOFAR VDIF b=3 packet word %2" PRIdFAST32 " is 0x%8.8" PRIX32, i, dp[i]);
    //     }
    // }
    // DEBUG print for beamlet 3
    // {
    //     const uint32_t* const restrict dp = (const uint32_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(VDIF_frames_VDIF_writer[3]->frame_pointer_const(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
    //     DEF_LOGGING_DEBUG("LOFAR VDIF b=3 packet word %d is 0x%8.8" PRIX32, 0, dp[0]);
    //     DEF_LOGGING_DEBUG("LOFAR VDIF b=3 packet word %d is 0x%8.8" PRIX32, 1, dp[1]);
    // }

    return int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
write_data_error:
    return int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
write_data_status_bad:
    return int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
write_data_running_bad:
    return int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
VDIF_writer_thread_stop_writing() restrict throw()
{
    //DEF_LOGGING_DEBUG("start VDIF_writer_thread_stop_writing");
    if(VDIF_writer_thread_running != 1) {
        // should not be here
        DEF_LOGGING_ERROR("the VDIF_writer_thread is not running");
        return int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    }
    return int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
}














///////////////////////////////////////////////////////////////////////////////
// VDIF helper functions
///////////////////////////////////////////////////////////////////////////////




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
check_for_allowed_VDIF_filename_characters(const char* restrict s,
                                           const bool allow_underscore) restrict const throw()
{
    while(*s) {
        int c = *s++;
        if( isalnum(c)
          || (c == '(')
          || (c == ')')
          || (c == '[')
          || (c == ']')
          || (c == '-')
          || (c == '&')
          || (c == '+')
          || ( (c == '_') && allow_underscore )
            ) {
            continue;
        }
        return c;
    }
    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
generate_VDIF_filename(void) restrict throw()
{
// The VDIF filename will be (ignoring a path specifier at the front)
// <filename_base>_<station_code>_<scan_name>_<data start time>_<auxinfo1>_<auxinfo2>.vdif
// where the angle brackets (< and >) delimit the field names used to generate
// the file name and are not generated in the output file name.
// filename_base is the filename_base provided to the writer, and is supposed
// to be the experiment code as per VLBI file naming conventions.
// station_code is the decimal version of the LOFAR station code.
// scan_name is some text string provided as extra argument to the writer.
// data start time is in the VLBI (VEX) time format of the start of the data
// in the file, and is generated from stYYYYyDDDdHHhMMmSS,
// where the lowercase letters stay as they are, and
// YYYY is replaced by the year, DDD by the day of year, HH by the hour, MM by
// the minute, and SS by the second (trucated).
// auxinfo1 is generated from fdXXXX-<total # chans>-<bits/sample>-<# threads>
// where XXXX is the string XXXX, as the LOFAR data rate is not in general an
// integer number of megabits per second,
// total # chans is the total number of channels written out in the file
// (for LOFAR/LuMP this is 2 times the number of output beamlets),
// bits/sample is the number of bits for thereal part of the complex number
// (corresponding exactly to the LOFAR bit-size usage), and
// # threads is the total number of threads, which for LOFAR is the number
// of output beamlets.
// auxinfo2 is generated from wtXX, where wt is the character code meaning writer
// thread (the id variable), and XX is the 2 digit hexadecimal representation of
// the thread id.
    
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    char new_filename_base[FSIZE];
    // Get the station ID number, taking into account stations with
    // ring splitters (see the info class header for more information)
    size_t filename_base_len(check_for_LuMP_Python_extended_filename_base(filename_base, id) +1);
    if(filename_base_len > FSIZE) {
        filename_base_len = FSIZE;
    }
    strncpy(new_filename_base, filename_base, filename_base_len);
    new_filename_base[filename_base_len-1] = 0;
    
    
    const char* restrict station_number = Info.get_station_name()+3;
    int_fast32_t offset = 1000;
    if(!isdigit(*station_number)) {
        offset = 0;
        station_number++;
    }
    int_fast32_t station_id(std::atoi(station_number));
    if(station_id <= 0) {
        // Not possible in the LOFAR naming scheme --- something bad happend.
        DEF_LOGGING_ERROR("cannot parse station name '%s' to get LOFAR station ID number", Info.get_station_name());
        exit(2);
    }
    station_id += offset;
    // construct the data start string
    static const size_t DATE_STRING_SIZE = 32;
    char date_str[DATE_STRING_SIZE];
    {
        int64_t time_reference_zero = Info.get_time_reference_zero();
        Real64_t partial_seconds = Info.get_time_center_output_zero();
        int_fast64_t offset = std::floor(partial_seconds);
        time_reference_zero += offset;
        // 2014-09-20 do not write millisecond portion accouring to VLBI standard
        // partial_seconds -= offset;
        // if(partial_seconds > 0.9994999) {
        //     // the time is written out to the millisecond.  This will wrap over.
        //     ++time_reference_zero;
        //     partial_seconds = 0.0;
        // }
        struct tm date;
        time_t my_time_reference_zero(time_reference_zero);
        gmtime_r(&my_time_reference_zero, &date);
        int retval = snprintf(date_str, DATE_STRING_SIZE,
                              "st%4.4dy%3.3dd%2.2dh%2.2dm%2.2ds",
                              date.tm_year+1900,
                              date.tm_yday+1,
                              date.tm_hour,
                              date.tm_min,
                              date.tm_sec);
        // int retval = snprintf(date_str, DATE_STRING_SIZE,
        //                       "st%4.4dy%3.3dd%2.2dh%2.2dm%06.3fs",
        //                       date.tm_year+1900,
        //                       date.tm_yday+1,
        //                       date.tm_hour,
        //                       date.tm_min,
        //                       date.tm_sec+partial_seconds);
        date_str[DATE_STRING_SIZE-1]=0;
        if(retval != 20) {
            DEF_LOGGING_ERROR("programmer error writing date");
            exit(1);
        }
    }
    int bits_per_sample = Voltage_Size * 4; // 8 bits per byte, dive by two
                                            // to get real portion only

    const int extension_size(strlen(VDIF_FILENAME_EXTENSION));
    int VDIF_length = snprintf(new_filename, FSIZE, "%s_%d_%s_%s_fdXXXX-%d-%d-%d_wt%2.2X", new_filename_base, int(station_id), Scan_Name, date_str, int(NUM_Output_Beamlets*2), bits_per_sample, int(NUM_Output_Beamlets), (unsigned int)(id));
    if((VDIF_length < 0) || (size_t(VDIF_length)+extension_size >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating VDIF filename from base '%s' scan name '%s'\n", filename_base, Scan_Name);
        exit(2);
    }
    // OK, find the position of the last path separator
    int last_path_sep_pos = -1;
    for(int i=0; i <= VDIF_length; i++) {
        if(new_filename[i] == '/') {
            last_path_sep_pos = i;
        }
    }
    int bad_char = check_for_allowed_VDIF_filename_characters(new_filename_base+last_path_sep_pos+1, false);
    if((bad_char)) {
        DEF_LOGGING_WARNING("character '%c' (hexadecimal value 0x%X) is not allowed in the filename base by the VLBI standards --- see http://vlbi.org", bad_char, bad_char);
    }
    bad_char = check_for_allowed_VDIF_filename_characters(Scan_Name, false);
    if((bad_char)) {
        DEF_LOGGING_WARNING("character '%c' (hexadecimal value 0x%X) is not allowed in the scan name by the VLBI standards --- see http://vlbi.org", bad_char, bad_char);
    }
    bad_char = check_for_allowed_VDIF_filename_characters(new_filename+last_path_sep_pos+1, true);
    if((bad_char)) {
        DEF_LOGGING_WARNING("character '%c' (hexadecimal value 0x%X) is not allowed in the VDIF filename by the VLBI standards --- see http://vlbi.org", bad_char, bad_char);
    }
    // now add on the extension (done here, since . is not allowed to be present in the filename by VLBI standards
    strcat(new_filename, VDIF_FILENAME_EXTENSION);
    VDIF_length += extension_size;
    int name_length = VDIF_length - last_path_sep_pos;
    if(name_length > 256) {
        DEF_LOGGING_WARNING("the filename portion of the path+filename for the output VDIF file is %d characters long, but the VLBI specification allows only 256 characters --- full path+filename is '%s'", name_length, new_filename);
    }

    free(VDIF_filename);
    VDIF_filename = static_cast<char*>(malloc(VDIF_length+1));
    if(VDIF_filename == NULL) {
        DEF_LOGGING_ERROR("unable to allocate memory for filename string of length %d", VDIF_length+1);
        exit(1);
    }
    strncpy(VDIF_filename, new_filename, VDIF_length+1);
    VDIF_filename[VDIF_length]=0;
    
    return 0;
}













int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
initialize_VDIF_frame_data_areas(void) restrict throw()
{
    // use Base's uint32_t reference_time_zero;
    // and uint_fast64_t first_sample_received;
    uint_fast64_t frames_since_reference_second = first_sample_received/VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES;
    uint_fast64_t samples_since_frame_start = first_sample_received%VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES;
    num_time_sample_lines_in_boss_VDIF_storage = uint_fast32_t(samples_since_frame_start);
    num_valid_time_sample_lines_in_boss_VDIF_storage=0;
    // DEF_LOGGING_DEBUG("have initial values frames_since_reference_second=0x%" PRIXFAST64 " samples_since_frame_start=0x%" PRIXFAST64 "", frames_since_reference_second, samples_since_frame_start);
    // Adjust first_sample_received to correct for the invalid data samples
    // we are about to back-fill into the first VDIF frame
    first_sample_received -= frames_since_reference_second;
    last_sample_written_plus_1 = first_sample_received;
    // Zero the data areas of the boss frames for the first
    // samples_since_frame_start samples, as they are bad.
    // Remember that VDIF uses an offset zero integer system.
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            uint16_t* const restrict dp = (uint16_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(VDIF_frames_boss[b]->data_pointer(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
            for(uint_fast64_t i=0; i < samples_since_frame_start; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex8_t_DOUBLE;
            }
        }
        break;
    case L_intComplex16_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            uint32_t* const restrict dp = (uint32_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(VDIF_frames_boss[b]->data_pointer(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
            for(uint_fast64_t i=0; i < samples_since_frame_start; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex16_t_DOUBLE;
            }
        }
        break;
    case L_intComplex32_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            uint64_t* const restrict dp = (uint64_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(VDIF_frames_boss[b]->data_pointer(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
            for(uint_fast64_t i=0; i < samples_since_frame_start; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex32_t_DOUBLE;
            }
        }
        break;
    default:
        {
            DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                 int(DATA_TYPE));
            exit(1);
        }
    }
    last_sample_written_plus_1 += samples_since_frame_start;
    num_invalid_samples += samples_since_frame_start;
    // Now tell the valid object that we have invalid data at the start.
    if(valid_object->write_N_points(samples_since_frame_start,0) == 0){
    }
    else {
        report_file_error("initialize_VDIF_frame_data_areas");
        close_output_files();
        status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
        return status;
    }
    // OK, now, for all of the VDIF frames, set the reference
    // epoch.
    uint_fast8_t VDIF_epoch_index = LOFAR_VDIF_Packet::get_VDIF_epoch_floor_index_from_UNIX_timestamp(reference_time_zero);
    uint64_t VDIF_epoch = LOFAR_VDIF_Packet::VDIF_epoch_to_UNIX_timestamp(uint8_t(VDIF_epoch_index));
    DEF_LOGGING_INFO("Setting VDIF reference epoch to timestamp 0x%8.8" PRIX64 ", LOFAR data reference time zero timestamp is 0x%8.8" PRIX32 " in writer %" PRId32, VDIF_epoch, reference_time_zero, id);
    for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
        VDIF_frames_boss[b]->set_reference_epoch(uint32_t(VDIF_epoch_index));
        VDIF_frames_VDIF_writer[b]->set_reference_epoch(uint32_t(VDIF_epoch_index));
    }
    // Now how many seconds are we since the VDIF epoch, and how many frames?
    uint_fast64_t frame_strides = frames_since_reference_second / VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE;
    current_num_vdif_frames_since_reference_second_stride =  frames_since_reference_second % VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE;
    current_VDIF_reference_epoch_unix_timestamp = LOFAR_VDIF_Packet::VDIF_epoch_to_UNIX_timestamp(uint8_t(VDIF_epoch_index));
    current_VDIF_reference_second = reference_time_zero
                                    - current_VDIF_reference_epoch_unix_timestamp
                                    + (frame_strides * LOFAR_VDIF_Packet_LOFAR0::NUM_LOFAR_SECONDS_PER_VDIF_SECOND_STRIDE);
    // set these values in the boss frame headers
    for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
        VDIF_frames_boss[b]->set_seconds_from_reference_epoch(uint32_t(current_VDIF_reference_second));
        VDIF_frames_boss[b]->set_data_frame_number_within_second(uint32_t(current_num_vdif_frames_since_reference_second_stride));
    }
    // set the DAS
    for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
        VDIF_frames_boss[b]->set_LOFAR_data_aquisition_system(uint32_t(LuMP_data_aquisition_system));
        VDIF_frames_VDIF_writer[b]->set_LOFAR_data_aquisition_system(uint32_t(LuMP_data_aquisition_system));
    }
    VDIF_frames_initialization_stage=2;
    return 0;
}




void LOFAR_Station_Beamformed_Writer_VDIF0::
add_LOFAR_station_software_info_to_VDIF_frame_data_areas(void) restrict throw()
{
    uint8_t station_configuration = current_data_packets[0]->Config();
    for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
        VDIF_frames_boss[b]->set_LOFAR_station_configuration(uint32_t(station_configuration));
        VDIF_frames_VDIF_writer[b]->set_LOFAR_station_configuration(uint32_t(station_configuration));
    }
    VDIF_frames_initialization_stage=3;
    return;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
flush_VDIF_frame_data_areas(void) restrict throw()
{
    if(num_time_sample_lines_in_boss_VDIF_storage == 0) {
        // we have no data in the current frames --- nothing to do
        return LOFAR_WRITER_RETVAL_GOOD;
    }
    // How many samples do we need to fill in?
    uint_fast64_t fill_samples = VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES - num_time_sample_lines_in_boss_VDIF_storage;
    // Zero the data areas of the boss frames for the last
    // fill_samples samples, as they are bad.
    // Remember that VDIF uses an offset zero integer system.
    const uint_fast32_t VDIF_BYTE_OFFSET = VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR * num_valid_time_sample_lines_in_boss_VDIF_storage;
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            char* VDIF_start_char = static_cast<char*>(VDIF_frames_boss[b]->data_pointer())+VDIF_BYTE_OFFSET;
            uint16_t* const restrict dp = reinterpret_cast<uint16_t* const restrict>(VDIF_start_char);
            for(uint_fast64_t i=0; i < fill_samples; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex8_t_DOUBLE;
            }
        }
        break;
    case L_intComplex16_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            char* VDIF_start_char = static_cast<char*>(VDIF_frames_boss[b]->data_pointer())+VDIF_BYTE_OFFSET;
            uint32_t* const restrict dp = reinterpret_cast<uint32_t* const restrict>(VDIF_start_char);
            for(uint_fast64_t i=0; i < fill_samples; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex16_t_DOUBLE;
            }
        }
        break;
    case L_intComplex32_t:
        for(uint_fast16_t b=0; b < NUM_Beamlets; b++) {
            char* VDIF_start_char = static_cast<char*>(VDIF_frames_boss[b]->data_pointer())+VDIF_BYTE_OFFSET;
            uint64_t* const restrict dp = reinterpret_cast<uint64_t* const restrict>(VDIF_start_char);
            for(uint_fast64_t i=0; i < fill_samples; ++i) {
                dp[i] = TWOS_COMPLEMENT_intComplex32_t_DOUBLE;
            }
        }
        break;
    default:
        {
            DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                 int(DATA_TYPE));
            exit(1);
        }
    }
    num_time_sample_lines_in_boss_VDIF_storage += fill_samples;
    {
        int_fast32_t retval = tell_VDIF_writer_thread_to_send_data();
        if((retval)) {
            DEF_LOGGING_ERROR("giving write buffer to VDIF_writer_thread failed with %d", int(retval));
            return LOFAR_WRITER_RETVAL_WRITE_FAIL;
        }
    }
    last_sample_written_plus_1 += fill_samples;
    num_invalid_samples += fill_samples;
    // Now tell the valid object that we have invalid data at the start.
    if(valid_object->write_N_points(fill_samples,0) == 0){
    }
    else {
        report_file_error("flush_VDIF_frame_data_areas");
        close_output_files();
        status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
        return status;
    }

    return 0;
}






uint_fast32_t LOFAR_Station_Beamformed_Writer_VDIF0::
determine_LuMP_data_aquisition_system(const char* const DASptr) restrict throw()
{
    if(DASptr != 0) {
        if(DASptr[0] != 0) {
            errno=0;
            char* endptr=NULL;
            uint_fast32_t DAS(strtoul(DASptr,&endptr,0));
            if((errno != 0) || (endptr == DASptr)) {
                DEF_LOGGING_CRITICAL("VDIF writer could not read provided number for LuMP data aquisition system --- allowed range is 0--255");
                exit(2);
            }
            if(DAS < 256) {
                DEF_LOGGING_INFO("VDIF0 writer using LuMP DAS 0x%2.2" PRIXFAST32 " from user input", DAS);
                return DAS;
            }
            DEF_LOGGING_WARNING("VDIF writer found illegal DAS vale 0x%" PRIXFAST32 " from user input --- trying to use hostname instead", DAS);
        }
    }
    // try to figure it out from the hostname
    char hostname[HOST_NAME_MAX+1];
#if (defined _BSD_SOURCE) || ((defined _XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)) || ((defined _POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L))
    {
        errno = 0;
        int retval = gethostname(hostname, HOST_NAME_MAX+1);
        hostname[HOST_NAME_MAX] = 0;
        if((retval)) {
            DEF_LOGGING_WARNING("gethostname call failed with code %d\n", errno);
            hostname[0] = 0;
        }
    }
#else
    {
        hostname[0] = 0;
    }
#endif
    size_t len = strlen(hostname);
    if(len > 2) {
        // see if this matches the MPIfR host naming conventions
        if((isxdigit(hostname[len-1])) && (isalpha(hostname[len-2]))) {
            DEF_LOGGING_INFO("VDIF0 writer found hostname='%s' apparently matches MPIfR LuMP naming conventions --- trying to determine DAS", hostname);
            // OK, try to get the values
            char hex[2];
            hex[0] = hostname[len-1];
            hex[1] = 0;
            uint_fast16_t lane(strtoul(hex,NULL,16));
            int group = hostname[len-2];
            if(islower(group)) {
                group -= 'a';
            }
            else {
                group -= 'A';
            }
            if((group >= 0) && (group < 16)) {
                uint_fast32_t DAS = (group << 4) | lane;
                DEF_LOGGING_INFO("VDIF0 writer using LuMP DAS 0x%2.2" PRIXFAST32 " from hostname", DAS);
                return DAS;
            }
        }
    }
    DEF_LOGGING_WARNING("VDIF0 writer could not determine LuMP DAS number from hostname='%s', using 0", hostname);
    return 0;
}












}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


