// LOFAR_Station_Beamformed_Writer_Power0.cxx
// code for the derived writer class for total power measurements
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 16  James M Anderson  --MPIfR  start
// 2011 Mar 25  JMA  --update for revision 5.0 of LOFAR data output
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2012 Jan 29  JMA  --move file opening and closing to thread
// 2012 Feb 05  JMA  --strip path from filename_base for output file
// 2012 Mar 18  JMA  --change to MPIfR::LOGGING error messages
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Aug 09  JMA  ---- change info array data filenames to _lis extension
//                        from _raw




// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



// Power Format 0
// written in "wb" mode for all files
//
// Data file:  One data file is written out for all beamlets written by
// this writer.  The filename is filename_base.raw The data are
// written as 4 values of type output_DATA_TYPE for each beamlet,
// for all beamlets of each interation point.  The 4 values written out are,
// in order, the real-valued results of
// X_i X_i^\ast, Y_i Y_i^\ast, \Real(X_i Y_i^\ast), \Imag(X_i Y_i^\ast).
// where the X_i and Y_i refer to the X polarization complex sample of beamlet
// i.  This can also be written as
// X_R^2 + X_I^2, Y_R^2 + Y_I^2, X_R Y_R + X_I Y_I, X_I Y_R - X_R Y_I
// where X_R is the real component of X, and X_I is the imaginary component of
// X, and so on.
// Then the next integration point is written out.
// So, in C notation, the data are arranged as
// output_DATA_TYPE data[num_output_timeslots][beamlets_per_sample][4]
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
// Valid file.  A single file is written out to indicate the fraction of
// samples for each integration point that were valid
// (data came from the station) as opposed to invalid (no data
// available from the station).  This fraction is stored as a Real32_t, where
// the value 1.0f indicates that all samples were valid, 0.0f indicates that
// all samples were invalid, and 0.5f indicates that half of the samples were
// valid.
// The filename is
// filename_base.valid
//
// Filename file:
// A file of name
// filename_base.file_lis
// is written.  It contains a \n terminated string which is the
// name of the file opened









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
#include "LOFAR_Station_Beamformed_Writer_Power0.h"
#include <errno.h>
#include <string.h>
#include "MPIfR_logging.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_Power0::
LOFAR_Station_Beamformed_Writer_Power0(const char* const restrict filename_base_,
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
                                       const Real64_t desired_seconds_per_integration_,
                                       const uint8_t scale_by_inverse_num_samples_,
                                       const Real64_t extra_integration_multiplier,
                                       const LOFAR_raw_data_type_enum integration_DATA_TYPE,
                                       const uint8_t bounds_check_integration_data_type_)
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
        num_raw_packet_commands_processed(0),
        num_integration_points_processed(0),
        initialized(0),
        status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
        data_8_p(reinterpret_cast<uint8_t* restrict>(data_buffer)),
        data_16_p(reinterpret_cast<uint16_t* restrict>(data_buffer)),
        data_32_p(reinterpret_cast<uint32_t* restrict>(data_buffer)),
        data_64_p(reinterpret_cast<uint64_t* restrict>(data_buffer)),
        data_128_p(reinterpret_cast<intComplex128_t* restrict>(data_buffer)),
        valid_object(0),
        SAMPLES_PER_INTEGRATION(0),
        samples_in_this_integration(0),
        valid_samples_in_this_integration(0),
        apply_scaling(0),
        bounds_check_integration_data_type(bounds_check_integration_data_type_),
        valid_multiplier(1.0f),
        scaling_multiplier(extra_integration_multiplier),
        desired_seconds_per_integration(desired_seconds_per_integration_),
        integration_storage(0),
        output_storage(0),
        INTEGRATION_BUFFER_LENGTH_CHAR(0),
        OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR(0),
        NUM_VALUES_IN_INTEGRATION_BUFFER(0),
        scale_by_inverse_num_samples(scale_by_inverse_num_samples_),
        output_DATA_TYPE(integration_DATA_TYPE)
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


LOFAR_Station_Beamformed_Writer_Power0::
~LOFAR_Station_Beamformed_Writer_Power0()
{
    stop_thread();
    
    return;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
thread_constructor() restrict
{
    if(NUM_RSP_BEAMLET_POLARIZATIONS != 2) {
        DEF_LOGGING_CRITICAL("coded for 2 polarizations, but have %d instead\n",
                int(NUM_RSP_BEAMLET_POLARIZATIONS));
        exit(1);
    }
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

    retval = snprintf(new_filename, FSIZE, "%s.raw", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    fp_data = fopen(new_filename,"wbx");
#else
    fp_data = fopen("/dev/null","wb");
#endif
    if(fp_data == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output data file '%s'\n",
                new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
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
    valid_object = new LOFAR_Station_Beamformed_Valid_Real32_Writer(new_filename);
#else
    valid_object = new LOFAR_Station_Beamformed_Valid_Real32_Writer("/dev/null");
#endif

    // how many samples per integration?
    {
        Real64_t num_samp = desired_seconds_per_integration * LINES_PER_SECOND;
        if(num_samp < 0.0) {
            DEF_LOGGING_CRITICAL("cannot have negative number of samples per integration --- desired_seconds_per_integration was %E\n", desired_seconds_per_integration);
            exit(2);
        }
        if(num_samp > Real64_t(0xFFFFFFFFU)) {
            DEF_LOGGING_CRITICAL("too many samples per integration to fit within 32 bit integer --- desired_seconds_per_integration was %E, LINES_PER_SECOND was %E, result %E\n", desired_seconds_per_integration, LINES_PER_SECOND, num_samp);
            exit(2);
        }
        SAMPLES_PER_INTEGRATION = uint_fast32_t(num_samp);
        if(SAMPLES_PER_INTEGRATION == 0) {
            SAMPLES_PER_INTEGRATION++;
        }
    }    
    if((scale_by_inverse_num_samples)) {
        scaling_multiplier *= 1.0/SAMPLES_PER_INTEGRATION;
    }
    valid_multiplier = Real32_t(1.0/SAMPLES_PER_INTEGRATION);
    if(scaling_multiplier != 1.0) {
        apply_scaling = 1;
    }
    if(scaling_multiplier <= 0.0) {
        DEF_LOGGING_CRITICAL("final scaling multiplier is non-positive! %E\n", scaling_multiplier);
        exit(2);
    }

    // deal with different input data types
    if(  (DATA_TYPE == L_intComplex8_t)
      || (DATA_TYPE == L_intComplex16_t)
      || (DATA_TYPE == L_intComplex32_t)
      || (DATA_TYPE == L_intComplex64_t)
      || (DATA_TYPE == L_intComplex128_t)
      || (DATA_TYPE == L_Complex32_t)
      || (DATA_TYPE == L_Complex64_t)
      || (DATA_TYPE == L_Complex128_t)
         ) {
        // these should be understood
    }
    else {
        DEF_LOGGING_CRITICAL("LOFAR input data type %d is not yet programmed.  Contact the software developer\n", int(DATA_TYPE));
        exit(1);
    }
    // deal with different output data types
    if(  (output_DATA_TYPE == L_int8_t)
      || (output_DATA_TYPE == L_int16_t)
      || (output_DATA_TYPE == L_int32_t)
      || (output_DATA_TYPE == L_int64_t)
      || (output_DATA_TYPE == L_Real16_t)
      || (output_DATA_TYPE == L_Real32_t)
      || (output_DATA_TYPE == L_Real64_t)
         ) {
        // these should be understood
    }
    else {
        DEF_LOGGING_CRITICAL("LOFAR output data type %d is not yet programmed.  Contact the software developer\n", int(output_DATA_TYPE));
        exit(1);
    }
    {
        // allocate buffer memory
        // for speed performance reasons on my test sytems (32 bit core2 duo,
        // 64 bit xeon), the integration temp storage is made using
        // Real64_t values.  But the switch structure in the code could
        // allow one to use different types for each input type.  (The
        // code was originally written to use int_fast64_t values for
        // integer complex up through intComplex32_t.)
        size_t temp_point_size = 0;
        size_t output_point_size = LOFAR_raw_data_type_enum_size(output_DATA_TYPE);
        switch(DATA_TYPE) {
        case L_intComplex8_t:
        case L_intComplex16_t:
        case L_intComplex32_t:
        case L_intComplex64_t:
        case L_intComplex128_t:
        case L_Complex32_t:
        case L_Complex64_t:
        case L_Complex128_t:
            temp_point_size = sizeof(Real64_t);
            break;
        default:
            DEF_LOGGING_CRITICAL("LOFAR input data type %d is not yet programmed.  Contact the software developer\n", int(DATA_TYPE));
            exit(1);
        }
        NUM_VALUES_IN_INTEGRATION_BUFFER = NUM_Output_Beamlets * NUM_OUTPUT_FIELDS;
        INTEGRATION_BUFFER_LENGTH_CHAR = uint_fast32_t(temp_point_size * NUM_VALUES_IN_INTEGRATION_BUFFER);
        OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR = uint_fast32_t(output_point_size * NUM_VALUES_IN_INTEGRATION_BUFFER);
        integration_storage = malloc(INTEGRATION_BUFFER_LENGTH_CHAR);
        if(integration_storage == NULL) {
            DEF_LOGGING_CRITICAL("unable to malloc %llu bytes for integration_storage\n", (unsigned long long)INTEGRATION_BUFFER_LENGTH_CHAR);
            exit(3);
        }
        memset(integration_storage,0,INTEGRATION_BUFFER_LENGTH_CHAR);
        output_storage = malloc(OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR);
        if(output_storage == NULL) {
            DEF_LOGGING_CRITICAL("unable to malloc %llu bytes for output_storage\n", (unsigned long long)OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR);
            exit(3);
        }
        memset(output_storage,0,OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR);
    }

    return write_header_init();
}



int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
thread_destructor() restrict
{
    close_output_files();
    free(integration_storage); integration_storage = 0;
    free(output_storage); output_storage = 0;
    delete valid_object; valid_object=0;

    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX packet commands\n", int(id), (unsigned long long)(num_raw_packet_commands_processed));
    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX integration points\n", int(id), (unsigned long long)(num_integration_points_processed));

    return 0;
}








int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
close_output_files() restrict throw()
{
    if((fp_data)) {
        int retval = fclose(fp_data); fp_data = NULL;
        if((retval)) {
            DEF_LOGGING_PERROR("closing raw data file failed");
            DEF_LOGGING_ERROR("Error: closing '%s' failed\n", filename_base);
            status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
        }
    }
            
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
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











int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
write_header_init() restrict throw()
{
    Real64_t seconds_per_integration(SAMPLES_PER_INTEGRATION*SECONDS_PER_LINE);

    // set up Info area
    Info.set_WRITER_TYPE(uint16_t(WRITER_TYPE()));
    LOFAR_STATION_BEAMFORMED_WRITER_BASE_SET_INFO_VERSION_DATE();
    Info.set_processing_DATA_TYPE(int32_t(L_Real64_t));
    Info.set_output_DATA_TYPE(int32_t(output_DATA_TYPE));
    Info.set_channelizer_TYPE(int32_t(LOFAR_CHANNELIZER_NONE));
    Info.set_NUM_CHANNELS(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_channelization(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_integration(uint32_t(SAMPLES_PER_INTEGRATION));
    Info.set_seconds_per_channelization_full(SECONDS_PER_LINE);
    Info.set_seconds_per_channelization_eff(SECONDS_PER_LINE);
    Info.set_samples_per_channelization_eff(NUM_OUTPUT_CHANNELS);
    Info.set_seconds_per_integration_full(seconds_per_integration);
    Info.set_seconds_per_integration_eff(seconds_per_integration);
    Info.set_samples_per_integration_eff(SAMPLES_PER_INTEGRATION);
    Info.set_seconds_per_output(seconds_per_integration);
    Info.set_integration_multiplier(scaling_multiplier);

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

int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
write_header_start() restrict throw()
{
    Info.set_sample_start(int64_t(current_sample_offset));
    Real64_t seconds_per_integration(SAMPLES_PER_INTEGRATION*SECONDS_PER_LINE);
    Real64_t sample_start(current_sample_offset);
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    // + seconds_per_integration / 2
    // - seconds_per_sample / 2
    Real64_t time_center_output_zero =
        sample_start * SECONDS_PER_LINE
        + seconds_per_integration * 0.5
        - SECONDS_PER_LINE * 0.5;
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


int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
write_header_end() restrict throw()
{
    Info.set_num_output_timeslots(uint64_t(num_integration_points_processed));

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





int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
write_standard_packets() restrict throw()
{
    //DEF_LOGGING_DEBUG("Thread %d writing packet command number %llX line %llX\n", int(id), (unsigned long long)num_raw_packet_commands_processed, (unsigned long long)current_sample_offset);
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
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint8_t* restrict data_8_bp_in = data_8_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint8_t* restrict data_8_bp = data_8_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_8_bp[0] = *data_8_bp_in++;
                    data_8_bp[1] = *data_8_bp_in++;
                    data_8_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_Blocks, current_valid[packet]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 2:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint16_t* const restrict data_16_p_in =
                reinterpret_cast<const uint16_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint16_t* restrict data_16_bp_in = data_16_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint16_t* restrict data_16_bp = data_16_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_16_bp[0] = *data_16_bp_in++;
                    data_16_bp[1] = *data_16_bp_in++;
                    data_16_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_Blocks, current_valid[packet]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 4:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint32_t* const restrict data_32_p_in =
                reinterpret_cast<const uint32_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint32_t* restrict data_32_bp_in = data_32_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint32_t* restrict data_32_bp = data_32_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_32_bp[0] = *data_32_bp_in++;
                    data_32_bp[1] = *data_32_bp_in++;
                    data_32_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_Blocks, current_valid[packet]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 8:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const uint64_t* const restrict data_64_p_in =
                reinterpret_cast<const uint64_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint64_t* restrict data_64_bp_in = data_64_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint64_t* restrict data_64_bp = data_64_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_64_bp[0] = *data_64_bp_in++;
                    data_64_bp[1] = *data_64_bp_in++;
                    data_64_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_Blocks, current_valid[packet]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 16:
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const intComplex128_t* const restrict data_128_p_in =
                reinterpret_cast<const intComplex128_t* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in
                    + Beamlet_Offsets_Array[beamlet];
                intComplex128_t* restrict data_128_bp = data_128_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_128_bp[0] = *data_128_bp_in++;
                    data_128_bp[1] = *data_128_bp_in++;
                    data_128_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_Blocks, current_valid[packet]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    last_sample_written_plus_1 = current_sample_offset;
    for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
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
integration_data_error:
    report_file_error("write_standard_packets");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    const uint_fast32_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams
    if(current_sample_stop_offset - current_sample_offset > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("Error in write_partial_packet(), got %llX %llX sample positions\n",
                (unsigned long long)(current_sample_offset),
                (unsigned long long)(current_sample_stop_offset));
        exit(1);
    }
    switch(Voltage_Size) {
    case 1:
        {
            const uint8_t* const restrict data_8_p_in =
                reinterpret_cast<const uint8_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint8_t* restrict data_8_bp_in = data_8_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint8_t* restrict data_8_bp = data_8_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_8_bp[0] = *data_8_bp_in++;
                    data_8_bp[1] = *data_8_bp_in++;
                    data_8_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_ACTUAL_BLOCKS, current_valid[0]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 2:
        {
            const uint16_t* const restrict data_16_p_in =
                reinterpret_cast<const uint16_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint16_t* restrict data_16_bp_in = data_16_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint16_t* restrict data_16_bp = data_16_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_16_bp[0] = *data_16_bp_in++;
                    data_16_bp[1] = *data_16_bp_in++;
                    data_16_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_ACTUAL_BLOCKS, current_valid[0]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 4:
        {
            const uint32_t* const restrict data_32_p_in =
                reinterpret_cast<const uint32_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint32_t* restrict data_32_bp_in = data_32_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint32_t* restrict data_32_bp = data_32_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_32_bp[0] = *data_32_bp_in++;
                    data_32_bp[1] = *data_32_bp_in++;
                    data_32_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_ACTUAL_BLOCKS, current_valid[0]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 8:
        {
            const uint64_t* const restrict data_64_p_in =
                reinterpret_cast<const uint64_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const uint64_t* restrict data_64_bp_in = data_64_p_in
                    + Beamlet_Offsets_Array[beamlet];
                uint64_t* restrict data_64_bp = data_64_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_64_bp[0] = *data_64_bp_in++;
                    data_64_bp[1] = *data_64_bp_in++;
                    data_64_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_ACTUAL_BLOCKS, current_valid[0]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    case 16:
        {
            const intComplex128_t* const restrict data_128_p_in =
                reinterpret_cast<const intComplex128_t* const restrict>(current_data_packets[0]->data_start_const());
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const intComplex128_t* restrict data_128_bp_in = data_128_p_in
                    + Beamlet_Offsets_Array[beamlet];
                intComplex128_t* restrict data_128_bp = data_128_p + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_128_bp[0] = *data_128_bp_in++;
                    data_128_bp[1] = *data_128_bp_in++;
                    data_128_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(add_packet_to_integration(NUM_ACTUAL_BLOCKS, current_valid[0]) == 0) {
            }
            else {
                goto integration_data_error;
            }
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    last_sample_written_plus_1 = current_sample_offset;
    if((current_valid[0])) {
        num_valid_samples += NUM_ACTUAL_BLOCKS;
    }
    else {
        num_invalid_samples += NUM_ACTUAL_BLOCKS;
    }
    last_sample_written_plus_1 += NUM_ACTUAL_BLOCKS;
    ++num_packets_received;
    ++num_raw_packet_commands_processed;
    return status;
integration_data_error:
    report_file_error("write_partial_packet");
    close_output_files();
    status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}


















int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
add_packet_to_integration(const uint_fast16_t NUM_SAMPLES,
                          const uint8_t packet_valid) restrict throw()
{
    // deal with the different input data types
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        {
            const intComplex8_t* restrict data_in =
                reinterpret_cast<const intComplex8_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_intComplex16_t:
        {
            const intComplex16_t* restrict data_in =
                reinterpret_cast<const intComplex16_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += int_fast32_t(Xr*Xr) + Xi*Xi;
                        *data_out++ += int_fast32_t(Yr*Yr) + Yi*Yi;
                        *data_out++ += int_fast32_t(Xr*Yr) + Xi*Yi;
                        *data_out++ += int_fast32_t(Xi*Yr) - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_intComplex32_t:
        {
            const intComplex32_t* restrict data_in =
                reinterpret_cast<const intComplex32_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
//                     int_fast64_t* restrict data_out =
//                         reinterpret_cast<int_fast64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
//                         int_fast64_t Xr = data_in->real();
//                         int_fast64_t Xi = data_in->imag();
//                         ++data_in;
//                         int_fast64_t Yr = data_in->real();
//                         int_fast64_t Yi = data_in->imag();
//                         ++data_in;
//                         *data_out++ += Xr*Xr + Xi*Xi;
//                         *data_out++ += Yr*Yr + Yi*Yi;
//                         *data_out++ += Xr*Yr + Xi*Yi;
//                         *data_out++ += Xi*Yr - Xr*Yi;
//                         int_fast32_t Xr = data_in->real();
//                         int_fast32_t Xi = data_in->imag();
//                         ++data_in;
//                         int_fast32_t Yr = data_in->real();
//                         int_fast32_t Yi = data_in->imag();
//                         ++data_in;
//                         *data_out++ += uint_fast32_t(Xr*Xr) + uint_fast32_t(Xi*Xi);
//                         *data_out++ += uint_fast32_t(Yr*Yr) + uint_fast32_t(Yi*Yi);
//                         *data_out++ += int_fast64_t(Xr*Yr) + Xi*Yi;
//                         *data_out++ += int_fast64_t(Xi*Yr) - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_intComplex64_t:
        {
            const intComplex64_t* restrict data_in =
                reinterpret_cast<const intComplex64_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_intComplex128_t:
        {
            const intComplex128_t* restrict data_in =
                reinterpret_cast<const intComplex128_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_Complex32_t:
        {
            const Complex32_t* restrict data_in =
                reinterpret_cast<const Complex32_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real().to_Real64_t();
                        Real64_t Xi = data_in->imag().to_Real64_t();
                        ++data_in;
                        Real64_t Yr = data_in->real().to_Real64_t();
                        Real64_t Yi = data_in->imag().to_Real64_t();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_Complex64_t:
        {
            const Complex64_t* restrict data_in =
                reinterpret_cast<const Complex64_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    case L_Complex128_t:
        {
            const Complex128_t* restrict data_in =
                reinterpret_cast<const Complex128_t* restrict>(data_buffer);
            for(uint_fast16_t s=0; s < NUM_SAMPLES; s++) {
                if((packet_valid)) {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(integration_storage);
                    for(uint_fast16_t b=0; b < NUM_Output_Beamlets; b++) {
                        Real64_t Xr = data_in->real();
                        Real64_t Xi = data_in->imag();
                        ++data_in;
                        Real64_t Yr = data_in->real();
                        Real64_t Yi = data_in->imag();
                        ++data_in;
                        *data_out++ += Xr*Xr + Xi*Xi;
                        *data_out++ += Yr*Yr + Yi*Yi;
                        *data_out++ += Xr*Yr + Xi*Yi;
                        *data_out++ += Xi*Yr - Xr*Yi;
                    } // for beamlets
                    valid_samples_in_this_integration++;
                }
                samples_in_this_integration++;
                if(samples_in_this_integration == SAMPLES_PER_INTEGRATION) {
                    int_fast32_t retval = dump_integration_to_output();
                    if(retval == 0) {
                    }
                    else {
                        goto dump_integration_error;
                    }
                }
            } // for samples
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("LOFAR input data type %d is not yet programmed.  Contact the software developer\n", int(DATA_TYPE));
        exit(1);
    }
    return status;
dump_integration_error:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_INTEGRATION_DUMP_FAIL);
    return status;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_Power0::
dump_integration_to_output() restrict throw()
{
    static const Real64_t Real64_INT8_MIN(INT8_MIN);
    static const Real64_t Real64_INT8_MAX(INT8_MAX);
    static const Real64_t Real64_INT16_MIN(INT16_MIN);
    static const Real64_t Real64_INT16_MAX(INT16_MAX);
    static const Real64_t Real64_INT32_MIN(INT32_MIN);
    static const Real64_t Real64_INT32_MAX(INT32_MAX);
    static const Real64_t Real64_INT64_MIN(INT64_MIN);
    static const Real64_t Real64_INT64_MAX(INT64_MAX);
    
    uint_fast32_t this_apply_scale = apply_scaling;
    Real64_t scale = scaling_multiplier;
    Real32_t fraction_valid(Real64_t(valid_samples_in_this_integration)/Real64_t(SAMPLES_PER_INTEGRATION));
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    memset(output_storage,0,OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR);
    if(valid_samples_in_this_integration != SAMPLES_PER_INTEGRATION) {
        this_apply_scale = 1;
        if(valid_samples_in_this_integration != 0) {
            scale *= Real64_t(SAMPLES_PER_INTEGRATION)/Real64_t(valid_samples_in_this_integration);
        }
        else {
            scale = 0.0;
            this_apply_scale = 0;
        }
    }
    switch(DATA_TYPE) {
    case L_intComplex8_t:
    case L_intComplex16_t:
    case L_intComplex32_t:
    case L_intComplex64_t:
    case L_intComplex128_t:
    case L_Complex32_t:
    case L_Complex64_t:
    case L_Complex128_t:
        {
            // integration storage is of type Real64_t
            const Real64_t* restrict data_in =
                reinterpret_cast<const Real64_t* restrict>(integration_storage);
            switch(output_DATA_TYPE) {
            case L_int8_t:
                {
                    int8_t* restrict data_out =
                        reinterpret_cast<int8_t* restrict>(output_storage);
                    if((bounds_check_integration_data_type)) {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value(*data_in++ * scale);
                                if(value < Real64_INT8_MIN) *data_out++ = int8_t(INT8_MIN);
                                else if(value > Real64_INT8_MAX) *data_out++ = int8_t(INT8_MAX);
                                else *data_out++ = int8_t(lrint(value));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value = *data_in++;
                                if(value < Real64_INT8_MIN) *data_out++ = int8_t(INT8_MIN);
                                else if(value > Real64_INT8_MAX) *data_out++ = int8_t(INT8_MAX);
                                else *data_out++ = int8_t(lrint(value));
                            }
                        }
                    }
                    else {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int8_t(lrint(*data_in++ * scale));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int8_t(lrint(*data_in++));
                            }
                        }
                    }
                }
                break;
            case L_int16_t:
                {
                    int16_t* restrict data_out =
                        reinterpret_cast<int16_t* restrict>(output_storage);
                    if((bounds_check_integration_data_type)) {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value(*data_in++ * scale);
                                if(value < Real64_INT16_MIN) *data_out++ = int16_t(INT16_MIN);
                                else if(value > Real64_INT16_MAX) *data_out++ = int16_t(INT16_MAX);
                                else *data_out++ = int16_t(lrint(value));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value = *data_in++;
                                if(value < Real64_INT16_MIN) *data_out++ = int16_t(INT16_MIN);
                                else if(value > Real64_INT16_MAX) *data_out++ = int16_t(INT16_MAX);
                                else *data_out++ = int16_t(lrint(value));
                            }
                        }
                    }
                    else {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int16_t(lrint(*data_in++ * scale));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int16_t(lrint(*data_in++));
                            }
                        }
                    }
                }
                break;
            case L_int32_t:
                {
                    int32_t* restrict data_out =
                        reinterpret_cast<int32_t* restrict>(output_storage);
                    if((bounds_check_integration_data_type)) {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value(*data_in++ * scale);
                                if(value < Real64_INT32_MIN) *data_out++ = int32_t(INT32_MIN);
                                else if(value > Real64_INT32_MAX) *data_out++ = int32_t(INT32_MAX);
                                else *data_out++ = int32_t(lrint(value));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value = *data_in++;
                                if(value < Real64_INT32_MIN) *data_out++ = int32_t(INT32_MIN);
                                else if(value > Real64_INT32_MAX) *data_out++ = int32_t(INT32_MAX);
                                else *data_out++ = int32_t(lrint(value));
                            }
                        }
                    }
                    else {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int32_t(lrint(*data_in++ * scale));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int32_t(lrint(*data_in++));
                            }
                        }
                    }
                }
                break;
            case L_int64_t:
                {
                    int64_t* restrict data_out =
                        reinterpret_cast<int64_t* restrict>(output_storage);
                    if((bounds_check_integration_data_type)) {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value(*data_in++ * scale);
                                if(value < Real64_INT64_MIN) *data_out++ = int64_t(INT64_MIN);
                                else if(value > Real64_INT64_MAX) *data_out++ = int64_t(INT64_MAX);
                                else *data_out++ = int64_t(llrint(value));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                Real64_t value = *data_in++;
                                if(value < Real64_INT64_MIN) *data_out++ = int64_t(INT64_MIN);
                                else if(value > Real64_INT64_MAX) *data_out++ = int64_t(INT64_MAX);
                                else *data_out++ = int64_t(llrint(value));
                            }
                        }
                    }
                    else {
                        if((this_apply_scale)) {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int64_t(llrint(*data_in++ * scale));
                            }
                        }
                        else {
                            for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                                *data_out++ = int64_t(llrint(*data_in++));
                            }
                        }
                    }
                }
                break;
            case L_Real16_t:
                {
                    MPIfR::DATA_TYPE::Real16_t* restrict data_out =
                        reinterpret_cast<MPIfR::DATA_TYPE::Real16_t* restrict>(output_storage);
                    if((this_apply_scale)) {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = MPIfR::DATA_TYPE::Real16_t(*data_in++ * scale);
                        }
                    }
                    else {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = MPIfR::DATA_TYPE::Real16_t(*data_in++);
                        }
                    }
                }
                break;
            case L_Real32_t:
                {
                    Real32_t* restrict data_out =
                        reinterpret_cast<Real32_t* restrict>(output_storage);
                    if((this_apply_scale)) {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = Real32_t(*data_in++ * scale);
                        }
                    }
                    else {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = Real32_t(*data_in++);
                        }
                    }
                }
                break;
            case L_Real64_t:
                {
                    Real64_t* restrict data_out =
                        reinterpret_cast<Real64_t* restrict>(output_storage);
                    if((this_apply_scale)) {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = Real64_t(*data_in++ * scale);
                        }
                    }
                    else {
                        for(uint_fast32_t i=0; i < NUM_VALUES_IN_INTEGRATION_BUFFER; i++) {
                            *data_out++ = Real64_t(*data_in++);
                        }
                    }
                }
                break;
            default:
                DEF_LOGGING_CRITICAL("LOFAR output data type %d is not yet programmed.  Contact the software developer\n", int(output_DATA_TYPE));
                exit(1);
            }
        }
        break;
    default:
        DEF_LOGGING_CRITICAL("LOFAR input data type %d is not yet programmed.  Contact the software developer\n", int(DATA_TYPE));
        exit(1);
    }

    if(fwrite(output_storage, OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR, 1,
              fp_data) == 1) {
    }
    else {
        goto write_data_error;
    }
    if(valid_object->write_valid(fraction_valid) == 0){
    }
    else {
        goto write_data_error;
    }
    
    
    samples_in_this_integration=0;
    valid_samples_in_this_integration=0;
    memset(integration_storage,0,INTEGRATION_BUFFER_LENGTH_CHAR);

    num_integration_points_processed++;
    return status;
write_data_error:
    report_file_error("dump_integration_to_output");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}







}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


