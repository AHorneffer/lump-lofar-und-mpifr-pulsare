// LOFAR_Station_Beamformed_Writer_P_FFT1.cxx
// code for the FFT spectra power, case 1
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 03  James M Anderson  --GFZ  start from LOFAR_Station_Beamformed_Writer_P_FFT0.cxx
// 2014 Sep 04  JMA  ---- rename FFT_samples to FFT_blocks, round for num
//                        blocks per integration



// Copyright (c) 2012, 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



// FFT Format 0       Raw FFT
// written in "wb" mode for all files
//
// Data file:  One data file is written out for all beamlets written by
// this writer.  The filename is filename_base.raw   The data are
// written as 4 values of type integration_DATA_TYPE for each
// channel of each beamlet,
// for all beamlets of each interation point.  The 4 values written out,
// in order, the real-valued results of
// X_i X_i^\ast, Y_i Y_i^\ast, \Real(X_i Y_i^\ast), \Imag(X_i Y_i^\ast).
// where the X_i and Y_i refer to the X polarization complex sample of beamlet
// channel i.  This can also be written as
// X_R^2 + X_I^2, Y_R^2 + Y_I^2, X_R Y_R + X_I Y_I, X_I Y_R - X_R Y_I
// where X_R is the real component of X, and X_I is the imaginary component of
// X, and so on.
// Then the next integration point is written out.
// So, in C notation, the data are arranged as
// output_DATA_TYPE data[num_output_timeslots][beamlets_per_sample][NUM_CHANNELS][4]
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
// FFT window file:
// Filename: filename_base.fftwindow
// The actual FFT window weights applied for this measurement.  See
// MPIfR_Numerical_sampling_windows.cxx for a description of the available
// window functions.  This is an array of
// processing_DATA_TYPE fftwindow[samp]
// where samp is the raw sample dimension, and the array has a size of
// NUM_OUTPUT_CHANNELS.  Note that processing_DATA_TYPE here is the real form of
// the user specified processing type (so that if the user specifies
// L_Complex64_t, the processing type here is actually L_Real32_t).
//
// Valid file.  A single file is written out to indicate the fraction
// of FFT blocks making up the integration that had fully valid
// station data (data came from the station) as opposed to invalid or
// partially invalid data (no or only partial data available from the
// station).  This fraction is stored as a Real32_t, where the value
// 1.0f indicates that all samples were valid, 0.0f indicates that all
// samples were invalid, and 0.5f indicates that half of the samples
// were valid.
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
#include "LOFAR_Station_Beamformed_Writer_P_FFT1.h"
#include <string.h>



using namespace MPIfR::DATA_TYPE;



// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_P_FFT1::
LOFAR_Station_Beamformed_Writer_P_FFT1(const char* const restrict filename_base_,
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
                                       const LOFAR_raw_data_type_enum data_type_process_,
                                       const LOFAR_raw_data_type_enum data_type_out_,
                                       const MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window_type_,
                                       const Real64_t window_parameter_,
                                       const Real64_t multiplier_,
                                       const Real64_t desired_seconds_per_integration_,
                                       const uint8_t scale_by_inverse_num_samples_,
                                       const uint8_t bounds_check_output_data_type_
                                     )
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
    num_spectrum_points_processed(0),
    num_integration_points_processed(0),
    initialized(0),
    status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
    window_storage(NULL),
    fft_in_storage(NULL),
    fft_out_storage(NULL),
    integration_storage(NULL),
    output_storage(NULL),
    valid_object(0),
    WINDOW_PARAMETER(window_parameter_),
    SCALING_MULTIPLIER(multiplier_),
    samples_in_this_FFT(0),
    valid_samples_in_this_FFT(0),
    NUM_REALS_IN_FFT_BLOCK(NUM_OUTPUT_CHANNELS_*2), //2 reals per complex
    FFT_blocks_in_this_integration(0),
    valid_FFT_blocks_in_this_integration(0),
    bounds_check_output_data_type(bounds_check_output_data_type_),
    weighted_valid_FFT_blocks_in_this_integration_p(0),
    NUM_REALS_IN_FULL_INTEGRATION_BUFFER(uint_fast64_t(NUM_OUTPUT_FIELDS)
                                        * NUM_OUTPUT_CHANNELS_
                                        * NUM_Output_Beamlets_),
    desired_seconds_per_integration(desired_seconds_per_integration_),
    scale_by_inverse_num_samples(scale_by_inverse_num_samples_),
    NUM_PARALLEL_BEAMLETS(NUM_Output_Beamlets_ * NUM_RSP_BEAMLET_POLARIZATIONS),
    processing_DATA_TYPE_REAL(data_type_process_),
    processing_DATA_TYPE_COMPLEX(data_type_process_),
    output_DATA_TYPE_REAL(data_type_out_),
    window_TYPE(window_type_)
{
    if(NUM_RSP_BEAMLET_POLARIZATIONS != 2) {
        DEF_LOGGING_CRITICAL("WRITER %d requires NUM_RSP_BEAMLET_POLARIZATIONS==2 for the raw input voltages.\nContact your developer for more information.\n", int(id));
        exit(1);
    }
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


LOFAR_Station_Beamformed_Writer_P_FFT1::
~LOFAR_Station_Beamformed_Writer_P_FFT1()
{
    stop_thread();
    
    return;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
thread_constructor() restrict
{
    if((check_input_parameters())) return -1;
    if((set_up_work_buffer_areas())) return -2;
    if((set_up_FFT_window())) return -3;
    if((set_up_FFTW3())) return -4;
    if((open_standard_files())) return -5;
    
    return write_header_init();
}



int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
thread_destructor() restrict
{
    close_output_files();
    shut_down_FFTW3();
    // free FFT buffers
    free(window_storage);      window_storage = 0;
    free(fft_in_storage);      fft_in_storage = 0;
    free(fft_out_storage);     fft_out_storage = 0;
    // free integration buffers
    free(integration_storage); integration_storage = 0;
    // free output buffers
    free(output_storage);      output_storage = 0;

    delete valid_object;       valid_object=0;

    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX packet commands\n", int(id), (unsigned long long)(num_raw_packet_commands_processed));
    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX spectrum points\n", int(id), (unsigned long long)(num_spectrum_points_processed));
    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX integration points\n", int(id), (unsigned long long)(num_integration_points_processed));

    return 0;
}



int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
check_input_parameters() restrict throw()
{
    // deal with different input data types
    if(  (DATA_TYPE == L_intComplex8_t)
      || (DATA_TYPE == L_intComplex16_t)
      || (DATA_TYPE == L_intComplex32_t)
      || (DATA_TYPE == L_intComplex64_t)
      || (DATA_TYPE == L_intComplex128_t)
      || (DATA_TYPE == L_Complex32_t)
      || (DATA_TYPE == L_Complex64_t)
      || (DATA_TYPE == L_Complex128_t)
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
      || (DATA_TYPE == L_Complex160_t)
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
      || (DATA_TYPE == L_Complex256_t)
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
         ) {
        // these should be understood
    }
    else {
        DEF_LOGGING_CRITICAL("LOFAR input data type %d is not yet programmed.  Contact the software developer\n", int(DATA_TYPE));
        exit(1);
    }
    switch(processing_DATA_TYPE_REAL) {
    case L_Real16_t:
    case L_Complex32_t:
        DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
        exit(1);
        processing_DATA_TYPE_REAL = L_Real16_t;
        processing_DATA_TYPE_COMPLEX = L_Complex32_t;
        break;
    case L_Real32_t:
    case L_Complex64_t:
        processing_DATA_TYPE_REAL = L_Real32_t;
        processing_DATA_TYPE_COMPLEX = L_Complex64_t;
        break;
    case L_Real64_t:
    case L_Complex128_t:
        processing_DATA_TYPE_REAL = L_Real64_t;
        processing_DATA_TYPE_COMPLEX = L_Complex128_t;
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
    case L_Complex160_t:
        processing_DATA_TYPE_REAL = L_Real80_t;
        processing_DATA_TYPE_COMPLEX = L_Complex160_t;
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
    case L_Complex256_t:
        processing_DATA_TYPE_REAL = L_Real128_t;
        processing_DATA_TYPE_COMPLEX = L_Complex256_t;
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
        exit(2);
    }
    switch(output_DATA_TYPE_REAL) {
    case L_Real16_t:
    case L_Complex32_t:
        output_DATA_TYPE_REAL = L_Real16_t;
        break;
    case L_Real32_t:
    case L_Complex64_t:
        output_DATA_TYPE_REAL = L_Real32_t;
        break;
    case L_Real64_t:
    case L_Complex128_t:
        output_DATA_TYPE_REAL = L_Real64_t;
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
    case L_Complex160_t:
        output_DATA_TYPE_REAL = L_Real80_t;
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
    case L_Complex256_t:
        output_DATA_TYPE_REAL = L_Real128_t;
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("output data type %d (%s) not supported.  Contact your developer.\n", int(output_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(output_DATA_TYPE_REAL));
        exit(2);
    }
    if(NUM_RSP_BEAMLET_POLARIZATIONS != 2) {
        DEF_LOGGING_CRITICAL("coded for 2 polarizations, but have %d instead\n",
                int(NUM_RSP_BEAMLET_POLARIZATIONS));
        exit(1);
    }
    if(NUM_OUTPUT_CHANNELS < 4) {
        DEF_LOGGING_WARNING("told to use %u channels, but >= 4 is strongly suggested\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
    }
    if(NUM_OUTPUT_CHANNELS < 2) {
        DEF_LOGGING_CRITICAL("told to use %u channels, but >= 2 is required\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    if((NUM_OUTPUT_CHANNELS & 0x1) == 0x1) {
        DEF_LOGGING_CRITICAL("told to use %u channels, but this writer requires an even number of channels\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    // Check the number of samples per integration
    {
        const Real64_t FFT_BLOCKS_PER_SECOND = LINES_PER_SECOND / NUM_OUTPUT_CHANNELS;
        Real64_t num_FFT_blocks = desired_seconds_per_integration * FFT_BLOCKS_PER_SECOND;
        if(num_FFT_blocks < 0.0) {
            DEF_LOGGING_CRITICAL("cannot have negative number of samples per integration --- desired_seconds_per_integration was %E\n", desired_seconds_per_integration);
            exit(2);
        }
        if(num_FFT_blocks > Real64_t(0xFFFFFFFFU)) {
            DEF_LOGGING_CRITICAL("too many FFT blocks per integration to fit within 32 bit integer --- desired_seconds_per_integration was %E, LINES_PER_SECOND was %E, NUM_OUTPUT_CHANNELS was %d, result %E\n", desired_seconds_per_integration, LINES_PER_SECOND, int(NUM_OUTPUT_CHANNELS), num_FFT_blocks);
            exit(2);
        }
        FFT_BLOCKS_PER_INTEGRATION = uint_fast32_t(llrint(num_FFT_blocks));
        if(FFT_BLOCKS_PER_INTEGRATION == 0) {
            FFT_BLOCKS_PER_INTEGRATION++;
        }
    }
    
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
set_up_work_buffer_areas() restrict throw()
{
    // set up the buffer area sizes.  Start with the number of total elements
    const uint_fast64_t NUM_REALS_IN_FULL_FFT_BUFFER = uint_fast64_t(NUM_RSP_BEAMLET_POLARIZATIONS)
                                           * NUM_Output_Beamlets
                                           * NUM_REALS_IN_FFT_BLOCK;
    FFT_BUFFER_LENGTH_CHAR = NUM_REALS_IN_FULL_FFT_BUFFER;
    INTEGRATION_BUFFER_LENGTH_CHAR = NUM_REALS_IN_FULL_INTEGRATION_BUFFER;
    OUTPUT_BUFFER_LENGTH_CHAR = NUM_REALS_IN_FULL_INTEGRATION_BUFFER;
    FFT_BUFFER_LENGTH_CHAR *= LOFAR_raw_data_type_enum_size(processing_DATA_TYPE_REAL);
    INTEGRATION_BUFFER_LENGTH_CHAR *= LOFAR_raw_data_type_enum_size(processing_DATA_TYPE_REAL);
    OUTPUT_BUFFER_LENGTH_CHAR *= LOFAR_raw_data_type_enum_size(output_DATA_TYPE_REAL);
    uint_fast64_t VALID_WEIGHTING_WORKSPACE_LENGTH_CHAR(NUM_REALS_IN_FFT_BLOCK);
    VALID_WEIGHTING_WORKSPACE_LENGTH_CHAR *= LOFAR_raw_data_type_enum_size(processing_DATA_TYPE_REAL);
    // Now allocate memory for the work arrays
    int retval = posix_memalign(&fft_in_storage, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, size_t(FFT_BUFFER_LENGTH_CHAR + VALID_WEIGHTING_WORKSPACE_LENGTH_CHAR));
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for fft_in_storage with retval %d\n", (unsigned long long)(FFT_BUFFER_LENGTH_CHAR + VALID_WEIGHTING_WORKSPACE_LENGTH_CHAR), retval);
        return -1;
    }
    retval = posix_memalign(&fft_out_storage, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, size_t(FFT_BUFFER_LENGTH_CHAR));
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for fft_out_storage with retval %d\n", (unsigned long long)(FFT_BUFFER_LENGTH_CHAR), retval);
        return -1;
    }
    retval = posix_memalign(&integration_storage, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, size_t(INTEGRATION_BUFFER_LENGTH_CHAR));
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for integration_storage with retval %d\n", (unsigned long long)(INTEGRATION_BUFFER_LENGTH_CHAR), retval);
        return -1;
    }
    // The following call to zero the array is valid for all IEEE
    // floating point types and most integer types.
    memset(integration_storage,0,size_t(INTEGRATION_BUFFER_LENGTH_CHAR));
    if((processing_DATA_TYPE_REAL != output_DATA_TYPE_REAL)) {
        retval = posix_memalign(&output_storage, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, size_t(OUTPUT_BUFFER_LENGTH_CHAR));
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for output_storage with retval %d\n", (unsigned long long)(OUTPUT_BUFFER_LENGTH_CHAR), retval);
            return -1;
        }
    }
    // next, allocate the weighted valid FFT blocks counter
    size_t data_type_max_size = LOFAR_raw_data_type_enum_size(L_RAW_DATA_TYPE_MAX);
    retval = posix_memalign(&weighted_valid_FFT_blocks_in_this_integration_p, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, data_type_max_size);
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for weighted_valid_FFT_blocks_in_this_integration_p with retval %d\n", (unsigned long long)(data_type_max_size), retval);
        return -1;
    }
    memset(weighted_valid_FFT_blocks_in_this_integration_p, 0, data_type_max_size);
    
    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
set_up_FFT_window() restrict throw()
{
    // Get the FFT window
    int_fast32_t retval32=0;
    switch(processing_DATA_TYPE_REAL) {
    case L_Real16_t:
        retval32 = generate_sampling_window_Real16_t(window_TYPE,
                                                     WINDOW_PARAMETER,
                                                     SCALING_MULTIPLIER,
                                                     int_fast32_t(NUM_OUTPUT_CHANNELS),
                                                     true,
                                                     reinterpret_cast<MPIfR::DATA_TYPE::Real16_t**>(&window_storage));
        break;
    case L_Real32_t:
        retval32 = generate_sampling_window_Real32_t(window_TYPE,
                                                     WINDOW_PARAMETER,
                                                     SCALING_MULTIPLIER,
                                                     int_fast32_t(NUM_OUTPUT_CHANNELS),
                                                     true,
                                                     reinterpret_cast<Real32_t**>(&window_storage));
        break;
    case L_Real64_t:
        retval32 = generate_sampling_window_Real64_t(window_TYPE,
                                                     WINDOW_PARAMETER,
                                                     SCALING_MULTIPLIER,
                                                     int_fast32_t(NUM_OUTPUT_CHANNELS),
                                                     true,
                                                     reinterpret_cast<Real64_t**>(&window_storage));
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
        retval32 = generate_sampling_window_Real80_t(window_TYPE,
                                                     WINDOW_PARAMETER,
                                                     SCALING_MULTIPLIER,
                                                     int_fast32_t(NUM_OUTPUT_CHANNELS),
                                                     true,
                                                     reinterpret_cast<Real80_t**>(&window_storage));
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
        retval32 = generate_sampling_window_Real128_t(window_TYPE,
                                                      WINDOW_PARAMETER,
                                                      SCALING_MULTIPLIER,
                                                      int_fast32_t(NUM_OUTPUT_CHANNELS),
                                                      true,
                                                      reinterpret_cast<Real128_t**>(&window_storage));
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
        exit(2);
    }
    if((retval32)) {
        DEF_LOGGING_ERROR("FFT windowing function failed with %d\n", int(retval32));
        return -2;
    }
    // dump out the window function, as an array of a real-valued number
    // (cut out every other element that is used here for multiplication
    // of the imaginary portion of the complex numbers)
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.fftwindow", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating fftwindow filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_window = fopen(new_filename,"wbx");
#else
    FILE* fp_window = fopen("/dev/null","wb");
#endif
    if(fp_window == NULL) {
        DEF_LOGGING_CRITICAL("unable to open fftwindow file '%s'\n", new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    {
        const size_t VAR_SIZE = LOFAR_raw_data_type_enum_size(processing_DATA_TYPE_REAL);
        size_t pos = 0;
        for(uint_fast32_t c=0; c < NUM_OUTPUT_CHANNELS; c++, pos+=VAR_SIZE+VAR_SIZE) {
            if(fwrite(reinterpret_cast<char*>(window_storage)+pos,VAR_SIZE,1,fp_window)!=1) {
                DEF_LOGGING_CRITICAL("unable to write to fftwindow file '%s'\n", new_filename);
                status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
                exit(2);
            }
        }
    }
    retval = fclose(fp_window);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing fftwindow file '%s'\n", new_filename);
        exit(2);
    }

    // Multiply the odd samples of the window function by -1, so that
    // the FFT will put the 0 channel in the center of the output channel array.
    // change over so that the 0 frequency is in the middle.
    switch(processing_DATA_TYPE_REAL) {
    case L_Real16_t:
        {
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
        }
        break;
    case L_Real32_t:
        {
            Real32_t* const restrict w(reinterpret_cast<Real32_t* const restrict>(window_storage));
            for(uint_fast32_t n=0; n<NUM_REALS_IN_FFT_BLOCK; n+=4) {
                w[n+2] = -w[n+2];
                w[n+3] = -w[n+3];
            }
        }
        break;
    case L_Real64_t:
        {
            Real64_t* const restrict w(reinterpret_cast<Real64_t* const restrict>(window_storage));
            for(uint_fast32_t n=0; n<NUM_REALS_IN_FFT_BLOCK; n+=4) {
                w[n+2] = -w[n+2];
                w[n+3] = -w[n+3];
            }
        }
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
        {
            Real80_t* const restrict w(reinterpret_cast<Real80_t* const restrict>(window_storage));
            for(uint_fast32_t n=0; n<NUM_REALS_IN_FFT_BLOCK; n+=4) {
                w[n+2] = -w[n+2];
                w[n+3] = -w[n+3];
            }
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
        {
            Real128_t* const restrict w(reinterpret_cast<Real128_t* const restrict>(window_storage));
            for(uint_fast32_t n=0; n<NUM_REALS_IN_FFT_BLOCK; n+=4) {
                w[n+2] = -w[n+2];
                w[n+3] = -w[n+3];
            }
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
        exit(2);
    }
    return 0;
}



int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
set_up_FFTW3() restrict throw()
{
    // Start up the FFTW3 plan
    MPIfR::Numerical::FFTW3::load_standard_wisdom();
    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    int rank = 1;
    int n[1];
    n[0] = int(NUM_OUTPUT_CHANNELS);
    int idist = int(NUM_OUTPUT_CHANNELS);
    int odist = int(NUM_OUTPUT_CHANNELS);
    // odist is the distance from 0th element of 0th array to
    // 0th element of 1st array
    int istride = 1; // contiguous
    int ostride = 1; // contiguous
    int parallel = int(NUM_RSP_BEAMLET_POLARIZATIONS * NUM_Output_Beamlets);

    switch(processing_DATA_TYPE_REAL) {
    case L_Real16_t:
        DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
        exit(1);
        break;
    case L_Real32_t:
        plan_32 = fftwf_plan_many_dft(rank, n, parallel,
                                      reinterpret_cast<fftwf_complex*>(fft_in_storage),
                                      n, istride, idist,
                                      reinterpret_cast<fftwf_complex*>(fft_out_storage),
                                      n, ostride, odist,
                                      FFTW_FORWARD,
                                      FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        break;
    case L_Real64_t:
        plan_64 = fftw_plan_many_dft(rank, n, parallel,
                                     reinterpret_cast<fftw_complex*>(fft_in_storage),
                                     n, istride, idist,
                                     reinterpret_cast<fftw_complex*>(fft_out_storage),
                                     n, ostride, odist,
                                     FFTW_FORWARD,
                                     FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
        plan_80 = fftwl_plan_many_dft(rank, n, parallel,
                                      reinterpret_cast<fftwl_complex*>(fft_in_storage),
                                      n, istride, idist,
                                      reinterpret_cast<fftwl_complex*>(fft_out_storage),
                                      n, ostride, odist,
                                      FFTW_FORWARD,
                                      FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
        plan_128 = fftwq_plan_many_dft(rank, n, parallel,
                                       reinterpret_cast<fftwq_complex*>(fft_in_storage),
                                       n, istride, idist,
                                       reinterpret_cast<fftwq_complex*>(fft_out_storage),
                                       n, ostride, odist,
                                       FFTW_FORWARD,
                                       FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
        exit(2);
    }
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
open_standard_files() restrict throw()
{
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
        DEF_LOGGING_CRITICAL("unable to open filename file '%s'\n",
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
    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
close_output_files() restrict throw()
{
    if((fp_data)) {
        int retval = fclose(fp_data); fp_data = NULL;
        if((retval)) {
            DEF_LOGGING_PERROR("closing raw data file failed");
            DEF_LOGGING_ERROR("closing '%s' failed\n", filename_base);
            status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
        }
    }
            
    return status;
}



int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
shut_down_FFTW3() restrict throw()
{
    // Shut down the FFTW3 plan
    MPIfR::Numerical::FFTW3::store_current_wisdom();
    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    switch(processing_DATA_TYPE_REAL) {
    case L_Real16_t:
        DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
        exit(1);
        break;
    case L_Real32_t:
        fftwf_destroy_plan(plan_32);
        break;
    case L_Real64_t:
        fftw_destroy_plan(plan_64);
        break;
    case L_Real80_t:
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        fftwl_destroy_plan(plan_80);
        break;
#else
        DEF_LOGGING_CRITICAL("long double processing not supported on this platform\n");
        exit(2);
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real128_t:
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        fftwq_destroy_plan(plan_128);
        break;
#else
        DEF_LOGGING_CRITICAL("quad double processing not supported on this platform\n");
        exit(2);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
        exit(2);
    }
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("Error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
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











int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_header_init() restrict throw()
{
    Real64_t seconds_per_FFT(NUM_OUTPUT_CHANNELS*SECONDS_PER_LINE);
    Real64_t seconds_per_integration(seconds_per_FFT*FFT_BLOCKS_PER_INTEGRATION);

    // set up Info area
    Info.set_WRITER_TYPE(uint16_t(WRITER_TYPE()));
    LOFAR_STATION_BEAMFORMED_WRITER_BASE_SET_INFO_VERSION_DATE();
    Info.set_processing_DATA_TYPE(int32_t(processing_DATA_TYPE_REAL));
    Info.set_output_DATA_TYPE(int32_t(output_DATA_TYPE_REAL));
    Info.set_channelizer_TYPE(int32_t(LOFAR_CHANNELIZER_FFT));
    Info.set_FFT_window_parameter(WINDOW_PARAMETER);
    Info.set_FFT_window_TYPE(window_TYPE);
    Info.set_NUM_CHANNELS(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_channelization(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_integration(uint32_t(NUM_OUTPUT_CHANNELS*FFT_BLOCKS_PER_INTEGRATION));
    Info.set_seconds_per_channelization_full(seconds_per_FFT);
    Info.set_seconds_per_channelization_eff(seconds_per_FFT);
    Info.set_samples_per_channelization_eff(NUM_OUTPUT_CHANNELS);
    Info.set_seconds_per_integration_full(seconds_per_integration);
    Info.set_seconds_per_integration_eff(seconds_per_integration);
    Info.set_samples_per_integration_eff(uint_fast64_t(NUM_OUTPUT_CHANNELS)*FFT_BLOCKS_PER_INTEGRATION);
    Info.set_seconds_per_output(seconds_per_integration);
    if((scale_by_inverse_num_samples)) {
        Info.set_integration_multiplier(SCALING_MULTIPLIER/FFT_BLOCKS_PER_INTEGRATION);
    }
    else {
        Info.set_integration_multiplier(SCALING_MULTIPLIER);
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
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_header_start() restrict throw()
{
    Info.set_sample_start(int64_t(current_sample_offset));
    Real64_t seconds_per_FFT(NUM_OUTPUT_CHANNELS*SECONDS_PER_LINE);
    Real64_t seconds_per_integration(seconds_per_FFT*FFT_BLOCKS_PER_INTEGRATION);
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
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_header_end() restrict throw()
{
    int_fast32_t return_code = 0;
    return_code = write_integration_flush();
    if((return_code)) {
        goto write_header_error;
    }

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
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}











int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_standard_packets() restrict throw()
{
    //DEF_LOGGING_DEBUG("Thread %d writing packet command number %llX line %llX\n", int(id), (unsigned long long)num_raw_packet_commands_processed, (unsigned long long)current_sample_offset);
    int_fast32_t return_code = 0;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_complex_to_fft_in<intComplex8_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_complex_to_fft_in<intComplex8_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_complex_to_fft_in<intComplex8_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_complex_to_fft_in<intComplex8_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_complex_to_fft_in<intComplex8_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex16_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<int8_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<int8_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<int8_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<int8_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<int8_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex32_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<int16_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<int16_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<int16_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<int16_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<int16_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex64_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<int32_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<int32_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<int32_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<int32_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<int32_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex128_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<int64_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<int64_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<int64_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<int64_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<int64_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex32_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<Real16_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<Real16_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<Real16_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<Real16_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<Real16_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex64_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<Real32_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<Real32_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<Real32_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<Real32_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<Real32_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex128_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<Real64_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<Real64_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<Real64_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<Real64_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<Real64_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Complex160_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<Real80_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<Real80_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<Real80_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<Real80_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<Real80_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Complex256_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in<Real128_t, Real16_t>();
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in<Real128_t, Real32_t>();
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in<Real128_t, Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in<Real128_t, Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in<Real128_t, Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("input data type %d (%s) not supported.  Contact your developer.\n", int(DATA_TYPE), LOFAR_raw_data_type_enum_Str(DATA_TYPE));
        exit(2);
    }
    if((return_code)) {
        goto integration_data_error;
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
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    int_fast32_t return_code = 0;
    const uint_fast32_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    if(current_sample_stop_offset - current_sample_offset > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("Error in write_partial_packet(), got %llX %llX sample positions\n",
                (unsigned long long)(current_sample_offset),
                (unsigned long long)(current_sample_stop_offset));
        exit(1);
    }
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_complex_to_fft_in_partial<intComplex8_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_complex_to_fft_in_partial<intComplex8_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_complex_to_fft_in_partial<intComplex8_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_complex_to_fft_in_partial<intComplex8_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_complex_to_fft_in_partial<intComplex8_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex16_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<int8_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<int8_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<int8_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<int8_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<int8_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex32_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<int16_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<int16_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<int16_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<int16_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<int16_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex64_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<int32_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<int32_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<int32_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<int32_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<int32_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_intComplex128_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<int64_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<int64_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<int64_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<int64_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<int64_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex32_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<Real16_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<Real16_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<Real16_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<Real16_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<Real16_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex64_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<Real32_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<Real32_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<Real32_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<Real32_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<Real32_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
    case L_Complex128_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<Real64_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<Real64_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<Real64_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<Real64_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<Real64_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Complex160_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<Real80_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<Real80_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<Real80_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<Real80_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<Real80_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Complex256_t:
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_partial<Real128_t, Real16_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_partial<Real128_t, Real32_t>(NUM_ACTUAL_BLOCKS);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_partial<Real128_t, Real64_t>(NUM_ACTUAL_BLOCKS);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_partial<Real128_t, Real80_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_partial<Real128_t, Real128_t>(NUM_ACTUAL_BLOCKS);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        DEF_LOGGING_CRITICAL("input data type %d (%s) not supported.  Contact your developer.\n", int(DATA_TYPE), LOFAR_raw_data_type_enum_Str(DATA_TYPE));
        exit(2);
    }
    if((return_code)) {
        goto integration_data_error;
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
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_P_FFT1::
write_integration_flush() restrict throw()
{
    //DEF_LOGGING_DEBUG("Thread %d flushing integration buffer\n", int(id));
    int_fast32_t return_code = 0;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    {
        uint_fast32_t num_flushed = 0;
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = write_real_to_fft_in_flush<Real16_t>(&num_flushed);
            break;
        case L_Real32_t:
            return_code = write_real_to_fft_in_flush<Real32_t>(&num_flushed);
            break;
        case L_Real64_t:
            return_code = write_real_to_fft_in_flush<Real64_t>(&num_flushed);
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = write_real_to_fft_in_flush<Real80_t>(&num_flushed);
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = write_real_to_fft_in_flush<Real128_t>(&num_flushed);
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        if((return_code)) {
            goto flush_data_error;
        }
    }
    while(FFT_blocks_in_this_integration != 0) {
        valid_samples_in_this_FFT = 0;
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            //return_code = dump_integration_to_output<Real16_t>();
            break;
        case L_Real32_t:
            return_code = dump_integration_to_output<Real32_t>();
            break;
        case L_Real64_t:
            return_code = dump_integration_to_output<Real64_t>();
            break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            return_code = dump_integration_to_output<Real80_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            return_code = dump_integration_to_output<Real128_t>();
            break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        if((return_code)) {
            goto integration_data_error;
        }
    }

    return status;
flush_data_error:
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
integration_data_error:
    DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
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


