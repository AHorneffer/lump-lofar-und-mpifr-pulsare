// LOFAR_Station_Beamformed_Writer_LuMP0.cxx
// writes out whole subband LuMP pulsar format data
//_HIST  DATE NAME PLACE INFO
// 2011 Aug 18  James M Anderson  --MPIfR  start
// 2011 Sep 27  JMA  --fix output
// 2012 Jan 28  JMA  --move file opening and closing to thread
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 12  JMA  --bugfixes
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Jul 07  JMA  ---- write out Telescope, RA, Dec, Source to LuMP file
// 2013 Jul 14  JMA  ---- fix endianness value in LuMP header for big-endian
//                        systems
// 2013 Jul 25  JMA  ---- provide information about the number of channels
//                        recorded for the case when channels are missing
//                        (READ_DATA_FROM_PIPE)
// 2013 Aug 09  JMA  ---- change info array data filenames to _lis extension
//                        from _raw
// 2013 Aug 19  JMA  ---- convert to first sample offset in Base.h class



// Copyright (c) 2011, 2012, 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
// Data files:  One LuMP data file per beamlet is written out.  The filenames are
// filename_base.%4.4X_raw
// where the %4.4X is replaced by the physical beamlet number in hex format.
// The data are written as one (complex) voltage value of type DATA_TYPE, 
// for each polarization,
// for each time sample.
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
// SourceNamess file:
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
// is written.  It contains a set of \n terminated strings which are the
// names of the files opened for each beamlet.







#define LOFAR_Station_Beamformed_Writer_LuMP0_H_FILE


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
#include "LOFAR_Station_Beamformed_Writer_LuMP0.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "MPIfR_Sexagesimal_Angle.h"






// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_LuMP0::
LOFAR_Station_Beamformed_Writer_LuMP0(const char* const restrict filename_base_,
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
                                      const char* const * const restrict SourceName_Array_)
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
        fp_data_void(NULL),
        num_raw_packet_commands_processed(0),
        NUM_FP_DATA(NUM_Output_Beamlets_),
        initialized(0),
        status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
        valid_object(0)
{
    int_fast32_t retcode = init_writer_processing();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to send init command\n", int(id));
        exit(1);
    }
    retcode = wait_for_thread_sleeping();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to init properly\n", int(id));
        exit(1);
    }
    return;
}

LOFAR_Station_Beamformed_Writer_LuMP0::
~LOFAR_Station_Beamformed_Writer_LuMP0()
{
    stop_thread();
    return;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
thread_constructor() restrict
{
    if(NUM_OUTPUT_CHANNELS != 1) {
        DEF_LOGGING_CRITICAL("coded for 1 output channel, but have %u instead\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating file filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out_file = fopen(new_filename,"wbx");
#else
    FILE* fp_out_file = fopen("/dev/null","wb");
#endif
    if(fp_out_file == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output file file '%s'\n",
                new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    
    fp_data_void = malloc(NUM_FP_DATA * sizeof(FILE*));
    if(fp_data_void == NULL) {
        DEF_LOGGING_CRITICAL("unable to malloc data file pointer memory\n");
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    fp_data = reinterpret_cast<FILE* restrict * restrict>(reinterpret_cast<FILE* *>(fp_data_void));

    for(uint_fast32_t f=0; f < NUM_FP_DATA; f++) {
        uint32_t beam(Beamlets_Array[f] + PHYSICAL_BEAMLET_OFFSET);
        retval = snprintf(new_filename, FSIZE, "%s.%4.4X_raw", filename_base, int(beam));
        if((retval < 0) || (size_t(retval) >= FSIZE)) {
            DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
            exit(2);
        }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
        fp_data[f] = fopen(new_filename,"wbx");
#else
        fp_data[f] = fopen("/dev/null","wb");
#endif
        if(fp_data[f] == NULL) {
            DEF_LOGGING_CRITICAL("unable to open raw output data file '%s'\n",
                    new_filename);
            status=int32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
            exit(2);
        }
        retval = fputs(strip_path(new_filename),fp_out_file);
        if(retval<0) {
            DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                    new_filename);
            status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
            exit(2);
        }
        retval = fputc('\n', fp_out_file);
        if(retval<0) {
            DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                    new_filename);
            status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
            exit(2);
        }
    }
    retval = fclose(fp_out_file);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing file file from base '%s'\n", filename_base);
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

    strncpy(LuMP_header,LuMP_HEADER_BASE,LuMP_HEADER_SIZE);
    LuMP_header[LuMP_HEADER_SIZE-1] = 0;
    
    

    return write_header_init();
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
thread_destructor() restrict
{
    close_output_files();
    free(fp_data_void); fp_data_void=NULL;
    fp_data=0;
    delete valid_object; valid_object=0;

    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX raw packet commands\n", int(id), (unsigned long long)(num_raw_packet_commands_processed));

    return 0;
}    









int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
close_output_files() restrict throw()
{
    if((fp_data)) {
        for(uint_fast32_t f=0; f < NUM_FP_DATA; f++) {
            if((fp_data[f])) {
                int retval = fclose(fp_data[f]); fp_data[f] = NULL;
                if((retval)) {
                    DEF_LOGGING_PERROR("closing raw data file failed");
                    DEF_LOGGING_ERROR("closing '%s' data %u failed\n", filename_base, (unsigned int)(f));
                    status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
                }
            }
        }
    }
            
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    //DEF_LOGGING_DEBUG("do_work %d for LOFAR_Station_Beamformed_Writer_LuMP0 WRITER %d\n", int(work_code_copy), int(id));
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
    return status;
}











int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
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

    char telescope_name[LuMP_STATION_NAME_STR_SIZE];

    // Set up LuMP header as much as possible
    {
        int i;
        long long l;
        double d;
        char s[80];
        const char* cp=0;
        size_t retval;
        struct tm time_s;
        struct tm* tmp;
        time_t tt(reference_time_zero);
        i = int(LuMP_HEADER_SIZE);
        write_into_header(LuMP_HEADER_BASE_HDR_SIZE_OFFSET,"%d",6,i);
        i = 0;
        write_into_header(LuMP_HEADER_BASE_OBS_ID_OFFSET,"%d",1,i);
        tmp = gmtime_r(&tt, &time_s);
        if(tmp == NULL) {
            goto write_header_error;
        }
        retval = strftime(s, 80, "%F-%T", &time_s);
        if(retval == 0) {
            goto write_header_error;
        }
        write_into_header(LuMP_HEADER_BASE_UTC_START_OFFSET,"%s",19,s);
        d = MJD(time_s.tm_year+1900, time_s.tm_mon+1, time_s.tm_mday,
                time_s.tm_hour, time_s.tm_min, time_s.tm_sec, 0.0);
        write_into_header(LuMP_HEADER_BASE_MJD_START_OFFSET,"%.12f",18,d);

        {
            const char* const LuMP_name = LuMP_Telescope_Name_Converter(Info.get_station_name());
            if(LuMP_name != NULL) {
                // Use this one
                strncpy(telescope_name, LuMP_name, LuMP_STATION_NAME_STR_SIZE);
            }
            else {
                strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
            }
            telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
        }
        write_into_header(LuMP_HEADER_BASE_TELESCOPE_OFFSET,"%s", 30, telescope_name);
        // pulsar people use MHz
        d = NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz) * 1E-6;
        write_into_header(LuMP_HEADER_BASE_BW_OFFSET,"%.16E",23,d);
        // pulsar people use \mu s
        d = 1.0/d;
        write_into_header(LuMP_HEADER_BASE_TSAMP_OFFSET,"%.16E",23,d);
        // number of bits per sample type (real part of complex)
        // is our Voltage_Size in bytes * 8 bits per byte / 2 (real) sizes per
        // complex.  8/2 = 4, *4 --> <<2
        i = int(Voltage_Size<<2);
        write_into_header(LuMP_HEADER_BASE_NBIT_OFFSET,"%d",3,i);
        i = int(NUM_OUTPUT_CHANNELS);
        // 10 digits is enough for 2^{31} channels, which would take
        // 3 hours of integration time to get enough samples
        write_into_header(LuMP_HEADER_BASE_NCHAN_OFFSET,"%d",10,i);
        // binary formats are
        // "INTEGER_BIN_FORM"
        // "IEEE_FLOAT"
        write_into_header(LuMP_HEADER_BASE_BINARY_FORMAT_OFFSET,"%s",16,"INTEGER_BIN_FORM");
        // pol ordering formats are
        // "DSPSR"
        // "JONES_MATRIX"
        write_into_header(LuMP_HEADER_BASE_POL_ORDERING_OFFSET,"%s",5,"DSPSR");
        // endian formats are
        // "BIG"
        // "LITTLE"
        write_into_header(LuMP_HEADER_BASE_ENDIAN_OFFSET,"%s",6,
#if __BYTE_ORDER == __LITTLE_ENDIAN
                          "LITTLE"
#else
                          "BIG"
#endif
                          );
        l = (long long)reference_time_zero;
        write_into_header(LuMP_HEADER_BASE_UTC_REFERENCE_TIME_T_OFFSET,"0x%llX",17,l);
        // State can be
        // //! Nyquist sampled voltages (real)
        // Nyquist,
        // //! In-phase and Quadrature sampled voltages (complex)
        // Analytic,
        // //! Square-law detected total power
        // Intensity,
        // //! Square-law detected nth power
        // NthPower,
        // //! Square-law detected, two polarizations
        // PPQQ,
        // //! PP, QQ, Re[PQ], Im[PQ]
        // Coherence,
        // //! Stokes I,Q,U,V
        // Stokes,
        // //! PseudoStokes S0,S2,S2,S3
        // PseudoStokes,
        // //! Stokes invariant interval
        // Invariant,
        // //! Other
        // Other,
        // //! Just PP
        // PP_State,
        // //! Just QQ
        // QQ_State,
        // //! Fourth moment of the electric field (covariance of Stokes parameters)
        // FourthMoment
        write_into_header(LuMP_HEADER_BASE_STATE_OFFSET,"%s",8,"Analytic");
        i = int(VERSION());
        write_into_header(LuMP_HEADER_BASE_LUMP_VERSION_OFFSET,"%X",4,i);
        d = 1.0;
        write_into_header(LuMP_HEADER_BASE_LUMP_SCALE_OFFSET,"%.16E",23,d);

        // Take care of LuMP1 information
        cp = "FALSE";
        write_into_header(LuMP_HEADER_BASE_READ_DATA_FROM_PIPE_OFFSET,"%s",6,cp);
        i = int(NUM_OUTPUT_CHANNELS);
        // Write out the actual number of channels written to the file,
        // as opposed to the effective number of channels (including gaps)
        // that is written into NCHAN.  For LuMP0, these are the same.
        // 10 digits is enough for 2^{31} channels, which would take
        // 3 hours of integration time to get enough samples.  
        write_into_header(LuMP_HEADER_BASE_NCHAN_RECORDED_OFFSET,"%d",10,i);
    }
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

    for(uint_fast32_t f=0; f < NUM_FP_DATA; f++) {
        rewind(fp_data[f]);
        size_t retval = fwrite(LuMP_header, LuMP_HEADER_SIZE, 1, fp_data[f]);
        if(retval != 1) {
            goto write_header_error;
        }
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

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
write_header_start() restrict throw()
{
    const char* filename_p = 0;
    Info.set_sample_start(int64_t(first_sample_received));
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    Real64_t time_center_output_zero =
        first_sample_received * SECONDS_PER_LINE;
    Info.set_time_center_output_zero(time_center_output_zero);
    Real64_t time_leading_edge_output_zero = time_center_output_zero
        - 0.5*SECONDS_PER_LINE;

    
    char telescope_name[LuMP_STATION_NAME_STR_SIZE];
    bool telescope_name_recognized = false;
    strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
    telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
    {
        const char* const LuMP_name = LuMP_Telescope_Name_Converter(Info.get_station_name());
        if(LuMP_name != NULL) {
            // Use this one
            strncpy(telescope_name, LuMP_name, LuMP_STATION_NAME_STR_SIZE);
            telescope_name_recognized = true;
        }
        else {
            strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
        }
        telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
    }

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==0)) {
        // Good
    }
    else {
        goto write_header_status_bad;
    }

    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    filename_p = filename_base + strlen(filename_base);
    while((*filename_p != '/') && (filename_p != filename_base))
    {
        --filename_p;
    }
    if(*filename_p == '/')
    {
        ++filename_p;
    }
    if(strlen(filename_p) > 40) {
        filename_p = "FILENAME_TOO_LONG";
    }

    for(uint_fast32_t f=0; f < NUM_FP_DATA; f++) {
        const size_t FSIZE = 2048;
        char new_filename[FSIZE];
        const size_t RADEC_SIZE = 40;
        char radec_str[RADEC_SIZE];
        int i;
        //long long l;
        double d;
        const char* cp=0;
        uint32_t beam(Beamlets_Array[f] + PHYSICAL_BEAMLET_OFFSET);
        int retcode = snprintf(new_filename, FSIZE, "%s.%4.4X_raw", filename_p, int(beam));
        if((retcode < 0) || (size_t(retcode) >= FSIZE)) {
            DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
            exit(2);
        }
        write_into_header(LuMP_HEADER_BASE_FILE_NAME_OFFSET,"%s",60,new_filename);
        i = int(Beamlets_Array[f] + PHYSICAL_BEAMLET_OFFSET);
        write_into_header(LuMP_HEADER_BASE_FILE_NUMBER_OFFSET,"%d",4,i);
        
        write_into_header(LuMP_HEADER_BASE_SOURCE_OFFSET,"%s", 33, SourceName_Array[f]);
        {
            using namespace MPIfR::Angles;
            Sexagesimal_Angle<Real64_t> RA(RightAscension_Array[f]);
            Sexagesimal_Type_Enum t_RA = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_HOURS);
            // For a Real64_t, the machine precision is equivalent to about
            // 45 picoarcseconds, so there is no point in going beyond
            // picoarcseconds for printing positions with our Real64_t angles.
            // Printing to a picoarcsecond should only require 22 characters.
            // This is in hours, so need 13 digits to get to picoarcseconds
            int_fast32_t sretval = RA.to_sexagesimal_string(t_RA, ':', ':', 0, 13, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make RA string from radian value %E", RA.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_RA_OFFSET,"%s", 37, radec_str);
            Sexagesimal_Angle<Real64_t> Dec(Declination_Array[f]);
            Sexagesimal_Type_Enum t_Dec = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
            sretval = Dec.to_sexagesimal_string(t_Dec, ':', ':', 0, 12, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make Dec string from radian value %E", Dec.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_DEC_OFFSET,"%s", 36, radec_str);
        }
        if(telescope_name_recognized) {
            const char* const input_name = LOFAR_RCUMODE_INPUT_CABLE_NAME(RCUMODE_Array[f]);
            telescope_name[5] = input_name[0];
            telescope_name[6] = input_name[1];
            telescope_name[7] = input_name[2];
            telescope_name[8] = 0;
        }
        write_into_header(LuMP_HEADER_BASE_TELESCOPE_OFFSET,"%s", 30, telescope_name);
        
        d = LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                           LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[f]),
                                           Physical_Subband_Array[f]);
        d *= 1E-6; // Pulsar people use MHz
        write_into_header(LuMP_HEADER_BASE_FREQ_OFFSET,"%.17E",24,d);
        d = time_center_output_zero; // This one is in seconds as it is my keyword.
        write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_CENTER_OFFSET,"%.17E",24,d);
        d = time_leading_edge_output_zero;//This one is in seconds as it is my keyword.
        write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_LEADING_OFFSET,"%.17E",24,d);
        cp = LOFAR_ANTENNA_SET_Str(Info.get_station_name(),int16_t(RCUMODE_Array[f]));
        write_into_header(LuMP_HEADER_BASE_ANTENNA_SET_OFFSET,"%s",15,cp);
        cp = LOFAR_FILTER_SET_Str(int16_t(RCUMODE_Array[f]));
        write_into_header(LuMP_HEADER_BASE_FILTER_SELECTION_OFFSET,"%s",15,cp);
        
        rewind(fp_data[f]);
        size_t retval = fwrite(LuMP_header, LuMP_HEADER_SIZE, 1, fp_data[f]);
        if(retval != 1) {
            goto write_header_error;
        }
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


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
write_header_end() restrict throw()
{
    const char* filename_p = 0;
    Info.set_num_output_timeslots(uint64_t(last_sample_written_plus_1-first_sample_received));
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    Real64_t time_center_output_zero =
        first_sample_received * SECONDS_PER_LINE;
    Info.set_time_center_output_zero(time_center_output_zero);
    Real64_t time_leading_edge_output_zero = time_center_output_zero
        - 0.5*SECONDS_PER_LINE;

    
    char telescope_name[LuMP_STATION_NAME_STR_SIZE];
    bool telescope_name_recognized = false;
    strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
    telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
    {
        const char* const LuMP_name = LuMP_Telescope_Name_Converter(Info.get_station_name());
        if(LuMP_name != NULL) {
            // Use this one
            strncpy(telescope_name, LuMP_name, LuMP_STATION_NAME_STR_SIZE);
            telescope_name_recognized = true;
        }
        else {
            strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
        }
        telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
    }

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_header_status_bad;
    }

    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    filename_p = filename_base + strlen(filename_base);
    while((*filename_p != '/') && (filename_p != filename_base))
    {
        --filename_p;
    }
    if(*filename_p == '/')
    {
        ++filename_p;
    }
    if(strlen(filename_p) > 40) {
        filename_p = "FILENAME_TOO_LONG";
    }

    for(uint_fast32_t f=0; f < NUM_FP_DATA; f++) {
        const size_t FSIZE = 2048;
        char new_filename[FSIZE];
        const size_t RADEC_SIZE = 40;
        char radec_str[RADEC_SIZE];
        int i;
        long long l;
        double d;
        const char* cp=0;
        uint32_t beam(Beamlets_Array[f] + PHYSICAL_BEAMLET_OFFSET);
        int retcode = snprintf(new_filename, FSIZE, "%s.%4.4X_raw", filename_p, int(beam));
        if((retcode < 0) || (size_t(retcode) >= FSIZE)) {
            DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
            exit(2);
        }
        write_into_header(LuMP_HEADER_BASE_FILE_NAME_OFFSET,"%s",60,new_filename);
        i = int(Beamlets_Array[f] + PHYSICAL_BEAMLET_OFFSET);
        write_into_header(LuMP_HEADER_BASE_FILE_NUMBER_OFFSET,"%d",4,i);
        
        write_into_header(LuMP_HEADER_BASE_SOURCE_OFFSET,"%s", 33, SourceName_Array[f]);
        {
            using namespace MPIfR::Angles;
            Sexagesimal_Angle<Real64_t> RA(RightAscension_Array[f]);
            Sexagesimal_Type_Enum t_RA = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_HOURS);
            // For a Real64_t, the machine precision is equivalent to about
            // 45 picoarcseconds, so there is no point in going beyond
            // picoarcseconds for printing positions with our Real64_t angles.
            // Printing to a picoarcsecond should only require 22 characters.
            // This is in hours, so need 13 digits to get to picoarcseconds
            int_fast32_t sretval = RA.to_sexagesimal_string(t_RA, ':', ':', 0, 13, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make RA string from radian value %E", RA.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_RA_OFFSET,"%s", 37, radec_str);
            Sexagesimal_Angle<Real64_t> Dec(Declination_Array[f]);
            Sexagesimal_Type_Enum t_Dec = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
            sretval = Dec.to_sexagesimal_string(t_Dec, ':', ':', 0, 12, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make Dec string from radian value %E", Dec.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_DEC_OFFSET,"%s", 36, radec_str);
        }
        if(telescope_name_recognized) {
            const char* const input_name = LOFAR_RCUMODE_INPUT_CABLE_NAME(RCUMODE_Array[f]);
            telescope_name[5] = input_name[0];
            telescope_name[6] = input_name[1];
            telescope_name[7] = input_name[2];
            telescope_name[8] = 0;
        }
        write_into_header(LuMP_HEADER_BASE_TELESCOPE_OFFSET,"%s", 30, telescope_name);
        
        d = LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                           LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[f]),
                                           Physical_Subband_Array[f]);
        d *= 1E-6; // Pulsar people use MHz
        write_into_header(LuMP_HEADER_BASE_FREQ_OFFSET,"%.17E",24,d);
        d = time_center_output_zero; // This one is in seconds as it is my keyword.
        write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_CENTER_OFFSET,"%.17E",24,d);
        d = time_leading_edge_output_zero;//This one is in seconds as it is my keyword.
        write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_LEADING_OFFSET,"%.17E",24,d);
        cp = LOFAR_ANTENNA_SET_Str(Info.get_station_name(),int16_t(RCUMODE_Array[f]));
        write_into_header(LuMP_HEADER_BASE_ANTENNA_SET_OFFSET,"%s",15,cp);
        cp = LOFAR_FILTER_SET_Str(int16_t(RCUMODE_Array[f]));
        write_into_header(LuMP_HEADER_BASE_FILTER_SELECTION_OFFSET,"%s",15,cp);

        retcode = fseeko(fp_data[f], 0, SEEK_END);
        if(retcode != 0) {
            DEF_LOGGING_PERROR("ErrorA seeking in file");
            DEF_LOGGING_CRITICAL("Error seeking in data file from base '%s'\n", filename_base);
            exit(2);
        }
        off_t off = ftello(fp_data[f]);
        l = (long long)(off);
        write_into_header(LuMP_HEADER_BASE_FILE_SIZE_OFFSET,"%16lld",16,l);
        
        rewind(fp_data[f]);
        size_t retval = fwrite(LuMP_header, LuMP_HEADER_SIZE, 1, fp_data[f]);
        if(retval != 1) {
            goto write_header_error;
        }
        retcode = fseeko(fp_data[f], 0, SEEK_END);
        if(retcode != 0) {
            DEF_LOGGING_PERROR("ErrorB seeking in file");
            DEF_LOGGING_CRITICAL("Error seeking in data file from base '%s'\n", filename_base);
            exit(2);
        }
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
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
write_standard_packets() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing packet command number %llX line %llX\n", (unsigned long long)num_raw_packet_commands_processed, (unsigned long long)current_sample_offset);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams
    switch(Voltage_Size) {
    case 1:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint8_t* const restrict data_8_p_in =
                reinterpret_cast<const uint8_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint8_t* restrict data_8_bp_in = data_8_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_8_bp_in, DATA_BEAMLET_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 2:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint16_t* const restrict data_16_p_in =
                reinterpret_cast<const uint16_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint16_t* restrict data_16_bp_in = data_16_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_16_bp_in, DATA_BEAMLET_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 4:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint32_t* const restrict data_32_p_in =
                reinterpret_cast<const uint32_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint32_t* restrict data_32_bp_in = data_32_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_32_bp_in, DATA_BEAMLET_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 8:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint64_t* const restrict data_64_p_in =
                reinterpret_cast<const uint64_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint64_t* restrict data_64_bp_in = data_64_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_64_bp_in, DATA_BEAMLET_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 16:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const intComplex128_t* const restrict data_128_p_in =
                reinterpret_cast<const intComplex128_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_128_bp_in, DATA_BEAMLET_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    // write to the valid stream
    last_sample_written_plus_1 = current_sample_offset;
    for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
        if(valid_object->write_N_points(NUM_Blocks,current_valid[packet]) == 0){
        }
        else {
            goto write_data_error;
        }

        if(num_raw_packet_commands_processed < NUM_FP_DATA) {
            // flush the corresponding buffer, so that we don't try to flush
            // all file buffers at the same time
            if(fflush(fp_data[num_raw_packet_commands_processed]) == 0) {
            }
            else {
                goto write_data_error;
            }
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




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP0::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    const uint_fast32_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    const uint_fast16_t THIS_BEAMLET_DUAL_POL_LENGTH_CHAR(uint_fast16_t(NUM_ACTUAL_BLOCKS)
                                                         * LOFAR_raw_data_type_enum_size(DATA_TYPE)
                                                         * NUM_RSP_BEAMLET_POLARIZATIONS);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams
    if(current_sample_stop_offset - current_sample_offset > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("Error in write_partial_packet(), got %llX %llX sample positions (difference %d is greater than allowed %d)\n",
                             (unsigned long long)(current_sample_offset),
                             (unsigned long long)(current_sample_stop_offset),
                             int(current_sample_stop_offset - current_sample_offset), int(NUM_Blocks));
        exit(1);
    }
    switch(Voltage_Size) {
    case 1:
        {
            const uint8_t* const restrict data_8_p_in =
                reinterpret_cast<const uint8_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint8_t* restrict data_8_bp_in = data_8_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_8_bp_in, THIS_BEAMLET_DUAL_POL_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 2:
        {
            const uint16_t* const restrict data_16_p_in =
                reinterpret_cast<const uint16_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint16_t* restrict data_16_bp_in = data_16_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_16_bp_in, THIS_BEAMLET_DUAL_POL_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 4:
        {
            const uint32_t* const restrict data_32_p_in =
                reinterpret_cast<const uint32_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint32_t* restrict data_32_bp_in = data_32_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_32_bp_in, THIS_BEAMLET_DUAL_POL_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 8:
        {
            const uint64_t* const restrict data_64_p_in =
                reinterpret_cast<const uint64_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const uint64_t* restrict data_64_bp_in = data_64_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_64_bp_in, THIS_BEAMLET_DUAL_POL_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    case 16:
        {
            const intComplex128_t* const restrict data_128_p_in =
                reinterpret_cast<const intComplex128_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++,fp++) {
                // copy over both polarizations, X then Y
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in + Beamlet_Offsets_Array[beamlet];
                // X&Y together
                if(fwrite(data_128_bp_in, THIS_BEAMLET_DUAL_POL_LENGTH_CHAR, 1,
                          fp_data[fp]) == 1) {
                }
                else {
                    goto write_data_error;
                }
            }
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    // write to the valid stream
    last_sample_written_plus_1 = current_sample_offset;
    if(valid_object->write_N_points(NUM_ACTUAL_BLOCKS,current_valid[0]) == 0){
    }
    else {
        goto write_data_error;
    }
    if(num_packet_commands_processed < NUM_FP_DATA) {
        // flush the corresponding buffer, so that we don't try to flush
        // all file buffers at the same time
        if(fflush(fp_data[num_packet_commands_processed]) == 0) {
        }
        else {
            goto write_data_error;
        }
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




void LOFAR_Station_Beamformed_Writer_LuMP0::
write_into_header(uint_fast32_t pos,
                  const char* const format,
                  uint_fast32_t MAX_SIZE)
    restrict throw()
{
    // clear any previous value
    memset(LuMP_header+pos, ' ', MAX_SIZE);

    size_t len = strlen(format);
    if(len <= MAX_SIZE) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("formatted string  '%s' more than %u characters, pos %d\n",
                format, int(MAX_SIZE), (unsigned int)(pos));
        exit(1);
    }
    if(pos+len < LuMP_HEADER_SIZE) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("formatted string  '%s' overflows LuMP_header at pos %u\n",
                format, (unsigned int)(pos));
        exit(1);
    }
    strncpy(LuMP_header+pos, format, len);
    return;
}



template <class T>
void LOFAR_Station_Beamformed_Writer_LuMP0::
write_into_header(uint_fast32_t pos,
                  const char* const format,
                  uint_fast32_t MAX_SIZE,
                  const T datap)
    restrict throw()
{
    static const uint_fast32_t BUF_SIZE= 80;
    char buf[BUF_SIZE];
    // now get the new information
    int retval = snprintf(buf, BUF_SIZE, format, datap);
    if((retval > 0) && (uint_fast32_t(retval) < BUF_SIZE)) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("Error printing to format '%s', pos %u\n",
                             format, (unsigned int)(pos));
        exit(1);
    }
    write_into_header(pos,buf,MAX_SIZE);
    return;
}


// optimized for valid values around year 2000, assumes Gregorian Calender,year>0
int_fast32_t JD(int_fast32_t year, int_fast32_t month, int_fast32_t day)
{
    int_fast32_t century = year/100;
    int_fast32_t fourcentury = year/400;

    if (month > 2) {
        month++;
    }
    else {
        year--;
        month+=13;
    }
    int_fast32_t jd = int_fast32_t(floor(365.25*year)+floor(30.6001*month))
        +day+1720995;
    jd += 2-century+fourcentury;
    return jd;
}

double JD(int_fast32_t year, int_fast32_t month, int_fast32_t day,
          int_fast32_t hour, int_fast32_t minute, int_fast32_t second,
          double sec_fraction)
{
    double jd = JD(year, month, day) - 0.5;
    jd += ((((second+sec_fraction)/60.0+minute)/60.0)+hour)/24.0;
    return jd;
}
double MJD(int_fast32_t year, int_fast32_t month, int_fast32_t day,
           int_fast32_t hour, int_fast32_t minute, int_fast32_t second,
           double sec_fraction)
{
    int_fast32_t jd = JD(year, month, day);
    double mjd = jd - 2400000.5;
    mjd += ((((second+sec_fraction)/60.0+minute)/60.0)+hour)/24.0;
    return mjd;
}

const char* const LuMP_Telescope_Name_Converter(const char* const station_name) throw()
{
    static const char station_name_list[][2][8] = {
        {"EfDE601", "DE601"},
        {"UwDE602", "DE602"},
        {"TbDE603", "DE603"},
        {"PoDE604", "DE604"},
        {"JuDE605", "DE605"},
        {"NcFR606", "FR606"},
        {"OnSE607", "SE607"},
        {"CbUK608", "UK608"},
        {"KjFI609", "FI609"},
	{"NdDE609", "DE609"},
        {"",""}
    };
    for(int i=0; ; i++) {
        if(station_name_list[i][0][0] == 0) {
            break;
        }
        if(strcmp(station_name_list[i][0], station_name) == 0) {
            return station_name_list[i][1];
        }
    }
    return NULL;        
}



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


