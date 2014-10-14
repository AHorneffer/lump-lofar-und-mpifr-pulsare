// LOFAR_Station_Beamformed_Writer_Raw1.cxx
// code for the derived writer class for raw data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 16  James M Anderson  --MPIfR  start
// 2011 Mar 25  JMA  --update for revision 5.0 of LOFAR data output
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2012 Jan 29  JMA  --move file opening and closing to thread
// 2012 Mar 18  JMA  --change to MPIfR::LOGGING error messages
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 12  JMA  --bugfixes
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Aug 09  JMA  ---- change info array data filenames to _lis extension
//                        from _raw
// 2013 Aug 19  JMA  ---- use first sample information to correct output samples





// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

// Copyright 2014,  James M. Anderson <anderson@gfz-potsdam.de>

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
// Data files:  Two data files per beamlet are written out, one for
// each polarization.  The filenames are
// filename_base.%4.4X%c_raw
// where the %4.4X is replaced by the physical beamlet number in hex format,
// and the %c is replaced by 'X' for the X polarizatin and 'Y' for the Y
// polarization.
// The data are written as one (complex) voltage value of type DATA_TYPE
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
// names of the files opened for each beamlet,
// first for the X polarization, then for the Y polarization









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
#include "LOFAR_Station_Beamformed_Writer_Raw1.h"
#include <errno.h>
#include "MPIfR_logging.h"







// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_Raw1::
LOFAR_Station_Beamformed_Writer_Raw1(const char* const restrict filename_base_,
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
        NUM_FP_DATA(NUM_Beamlets_*NUM_RSP_BEAMLET_POLARIZATIONS),
        initialized(0),
        status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
        data_8_p(reinterpret_cast<uint8_t* restrict>(data_buffer)),
        data_16_p(reinterpret_cast<uint16_t* restrict>(data_buffer)),
        data_32_p(reinterpret_cast<uint32_t* restrict>(data_buffer)),
        data_64_p(reinterpret_cast<uint64_t* restrict>(data_buffer)),
        data_128_p(reinterpret_cast<intComplex128_t* restrict>(data_buffer)),
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


LOFAR_Station_Beamformed_Writer_Raw1::
~LOFAR_Station_Beamformed_Writer_Raw1()
{
    stop_thread();
    
    return;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
thread_constructor() restrict
{
    if(NUM_OUTPUT_CHANNELS != 1) {
        DEF_LOGGING_CRITICAL("Error: coded for 1 output channel, but have %u instead\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL(" cannot create file filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out_file = fopen(new_filename,"wbx");
#else
    FILE* fp_out_file = fopen("/dev/null","wb");
#endif
    if(fp_out_file == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output file file '%s'\n", new_filename);
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
        uint32_t beam(Beamlets_Array[f>>1] + PHYSICAL_BEAMLET_OFFSET);
        char p = ((f&0x1)) ? 'Y' : 'X';
        retval = snprintf(new_filename, FSIZE, "%s.%4.4X%c_raw", filename_base, int(beam), p);
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
            DEF_LOGGING_CRITICAL("unable to open raw output data file '%s'\n",new_filename);
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

    return write_header_init();
}



int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
thread_destructor() restrict
{
    close_output_files();
    free(fp_data_void); fp_data_void=NULL;
    fp_data=0;
    delete valid_object; valid_object=0;

    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX raw packet commands\n", int(id), (unsigned long long)(num_raw_packet_commands_processed));

    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
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


int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    //DEF_LOGGING_DEBUG("do_work %d for LOFAR_Station_Beamformed_Writer_LuMP0\n", int(work_code_copy));
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











int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
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

int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
write_header_start() restrict throw()
{
    Info.set_sample_start(int64_t(current_sample_offset));
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    Real64_t time_center_output_zero =
        current_sample_offset * SECONDS_PER_LINE;
    Info.set_time_center_output_zero(time_center_output_zero);

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


int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
write_header_end() restrict throw()
{
    Info.set_num_output_timeslots(uint64_t(last_sample_written_plus_1-first_sample_received));

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_header_status_bad;
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
}





int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint8_t* restrict data_8_bp_in = data_8_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_Blocks; block++) {
                    data_8_p[block] = *data_8_bp_in++;
                    data_8_p[NUM_Blocks+block] = *data_8_bp_in++;
                }
                // X
                if(fwrite(data_8_p, DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_8_p[NUM_Blocks]), DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint16_t* restrict data_16_bp_in = data_16_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_Blocks; block++) {
                    data_16_p[block] = *data_16_bp_in++;
                    data_16_p[NUM_Blocks+block] = *data_16_bp_in++;
                }
                // X
                if(fwrite(data_16_p, DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_16_p[NUM_Blocks]), DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint32_t* restrict data_32_bp_in = data_32_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_Blocks; block++) {
                    data_32_p[block] = *data_32_bp_in++;
                    data_32_p[NUM_Blocks+block] = *data_32_bp_in++;
                }
                // X
                if(fwrite(data_32_p, DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_32_p[NUM_Blocks]), DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint64_t* restrict data_64_bp_in = data_64_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_Blocks; block++) {
                    data_64_p[block] = *data_64_bp_in++;
                    data_64_p[NUM_Blocks+block] = *data_64_bp_in++;
                }
                // X
                if(fwrite(data_64_p, DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_64_p[NUM_Blocks]), DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_Blocks; block++) {
                    data_128_p[block] = *data_128_bp_in++;
                    data_128_p[NUM_Blocks+block] = *data_128_bp_in++;
                }
                // X
                if(fwrite(data_128_p, DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_128_p[NUM_Blocks]), DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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




int_fast32_t LOFAR_Station_Beamformed_Writer_Raw1::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    const uint_fast32_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    const uint_fast16_t THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR(uint_fast16_t(NUM_ACTUAL_BLOCKS) * LOFAR_raw_data_type_enum_size(DATA_TYPE));
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams
    if(current_sample_stop_offset - current_sample_offset > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("in write_partial_packet(), got %llX %llX sample positions\n",
                (unsigned long long)(current_sample_offset),
                (unsigned long long)(current_sample_stop_offset));
        exit(1);
    }
    switch(Voltage_Size) {
    case 1:
        {
            const uint8_t* const restrict data_8_p_in =
                reinterpret_cast<const uint8_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint8_t* restrict data_8_bp_in = data_8_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_ACTUAL_BLOCKS; block++) {
                    data_8_p[block] = *data_8_bp_in++;
                    data_8_p[NUM_Blocks+block] = *data_8_bp_in++;
                }
                // X
                if(fwrite(data_8_p, THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_8_p[NUM_Blocks]), THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint16_t* restrict data_16_bp_in = data_16_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_ACTUAL_BLOCKS; block++) {
                    data_16_p[block] = *data_16_bp_in++;
                    data_16_p[NUM_Blocks+block] = *data_16_bp_in++;
                }
                // X
                if(fwrite(data_16_p, THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_16_p[NUM_Blocks]), THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint32_t* restrict data_32_bp_in = data_32_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_ACTUAL_BLOCKS; block++) {
                    data_32_p[block] = *data_32_bp_in++;
                    data_32_p[NUM_Blocks+block] = *data_32_bp_in++;
                }
                // X
                if(fwrite(data_32_p, THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_32_p[NUM_Blocks]), THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const uint64_t* restrict data_64_bp_in = data_64_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_ACTUAL_BLOCKS; block++) {
                    data_64_p[block] = *data_64_bp_in++;
                    data_64_p[NUM_Blocks+block] = *data_64_bp_in++;
                }
                // X
                if(fwrite(data_64_p, THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_64_p[NUM_Blocks]), THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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
            for(uint_fast32_t beamlet=0,fp=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over polarizations, X to low, Y to high
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in + Beamlet_Offsets_Array[beamlet];
                for(uint_fast16_t block=0; block < NUM_ACTUAL_BLOCKS; block++) {
                    data_128_p[block] = *data_128_bp_in++;
                    data_128_p[NUM_Blocks+block] = *data_128_bp_in++;
                }
                // X
                if(fwrite(data_128_p, THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
                }
                else {
                    goto write_data_error;
                }
                // Y
                if(fwrite(&(data_128_p[NUM_Blocks]), THIS_BEAMLET_SINGLE_POL_LENGTH_CHAR, 1,
                          fp_data[fp++]) == 1) {
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






}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


