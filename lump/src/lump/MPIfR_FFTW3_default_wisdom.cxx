// MPIfR_FFTW3_default_wisdom.cxx
// create the default wisdom files for LuMP
//_HIST  DATE NAME PLACE INFO
// 2012 Jan 29  James M Anderson  --MPIfR  Start
// 2012 Mar 10  JMA  --move list of standard number channels to a configuration
//                     file
// 2013 Aug 03  JMA  ---- use defines to avoid __FILE__,__LINE__ visibility
// 2013 Sep 06  JMA  ---- use init and shutdown functions for MPIfR_FFTW3
// 2014 Sep 17  JMA  --- updated for 4 bit mode --- more beamlets needed
// 2014 Sep 19  JMA  --- fix number of beamlets to model bug



// Copyright (c) 2012, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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
#include <stdint.h>
// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#include <time.h>

#include "JMA_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "fftw3.h"
#include <pthread.h>
#include "MPIfR_FFTW3.h"
#include "MPIfR_logging.h"


namespace MPIfR {
namespace LOGGING {
extern const char PROGRAM_NAME[] = "MPIfR_FFTW3_default_wisdom";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
};};



namespace {
// &*$! GCC and GLIBC think ignoring return on strtol
// is a required warning.  Casting to (void) no longer
// works to get the compiler to shut up.
template<class T> inline T ignore_return(T i) {return i;}

// GLOBALS


// FUNCTIONS
int_fast32_t read_default_channel_list(const char* const restrict filename,
                                       int* const restrict NUM_LIST,
                                       int* restrict * const restrict channels)
{
    char* restrict text = NULL;
    int count = 0;
    char* ptr = NULL;
    char* endptr = NULL;
    int_fast32_t num_bytes = 0;
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("opening default number channels file '%s' failed\n", filename);
        exit(1);
    }
    int retval = fseek(fp, 0, SEEK_END);
    if((retval)) goto file_operation_error;
    num_bytes = ftell(fp);
    rewind(fp);

    text = (char* restrict)(malloc(num_bytes+1));
    if(!text) goto memory_error;

    if(fread(text, num_bytes, 1, fp) != 1) goto file_operation_error;

    text[num_bytes] = 0;

    // convert all non-numbers to spaces
    for(int_fast32_t i=0; i < num_bytes; i++) {
        if(!isdigit(text[i])) {
            text[i] = ' ';
        }
    }

    count = 0;
    ptr = text;
    endptr = NULL;

    ignore_return(strtol(ptr, &endptr, 10));
    while(endptr != ptr) {
        count++;
        ptr = endptr;
        ignore_return(strtol(ptr, &endptr, 10));
    }

    if(channels == 0) {
        DEF_LOGGING_ERROR("no valid number of channels list found\n");
        goto file_operation_error;
    }
    *NUM_LIST = count;

    *channels = (int* const restrict)(malloc(sizeof(int)*count));
    if((!*channels)) goto memory_error;

    ptr = text;
    endptr = NULL;
    count = 0;
    
    channels[0][count++] = int(strtol(ptr, &endptr, 10));
    while(endptr != ptr) {
        ptr = endptr;
        channels[0][count++] = int(strtol(ptr, &endptr, 10));
    }
    fclose(fp);
    free(text); text = NULL;
    return 0;


file_operation_error:
    DEF_LOGGING_CRITICAL("something wrong with default number channels file '%s'\n", filename);
    fclose(fp);
    free(text); text = NULL;
    exit(1);
    
memory_error:
    DEF_LOGGING_CRITICAL("memory allocation error\n");
    fclose(fp);
    exit(2);
    return -1;
}






}  // end namespace
















void do_32(const int MAX_SIZE,
           const int MAX_PARALLEL,
           const int LIST_SIZE,
           const int * const restrict CHANNELS)
{
    fftwf_complex* in, *out;
    fftwf_plan p;

    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    in = reinterpret_cast<fftwf_complex*>(fftwf_malloc(sizeof(fftwf_complex)*MAX_SIZE*MAX_PARALLEL));
    out = reinterpret_cast<fftwf_complex*>(fftwf_malloc(sizeof(fftwf_complex)*MAX_SIZE*MAX_PARALLEL));

    if((in == NULL) || (out == NULL)) {
        DEF_LOGGING_CRITICAL("cannot malloc memory for %d 32 bit elements\n", MAX_SIZE);
        exit(1);
    }

    // do channels individually
    for(int s = 0; s < LIST_SIZE; s++) {
        DEF_LOGGING_INFO("Generating 32 bit transform for %d elements\n", CHANNELS[s]);
        p = fftwf_plan_dft_1d(CHANNELS[s], in, out, FFTW_FORWARD,
                              FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        fftwf_destroy_plan(p);
    }
    // generate all parallel components
    for(int pa=1; pa <= MAX_PARALLEL; pa++) {
        for(int s = 0; s < LIST_SIZE; s++) {
            DEF_LOGGING_INFO("Generating 32 bit transform for %dx%d elements\n", CHANNELS[s], pa);
            int rank = 1;
            int n[1];
            n[0] = CHANNELS[s];
            int idist = CHANNELS[s];
            int odist = CHANNELS[s]; // distance from 0th element of 0th array to
                              // 0th element of 1st array
            int istride = 1; // contiguous
            int ostride = 1; // contiguous
            
            p = fftwf_plan_many_dft(rank, n, pa,
                                    in, n, istride, idist,
                                    out, n, ostride, odist,
                                    FFTW_FORWARD,
                                    FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
            fftwf_destroy_plan(p);
        }
    }


    fftwf_free(in);
    fftwf_free(out);  
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}


void do_64(const int MAX_SIZE,
           const int MAX_PARALLEL,
           const int LIST_SIZE,
           const int * const restrict CHANNELS)
{
    fftw_complex* in, *out;
    fftw_plan p;

    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    in = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex)*MAX_SIZE*MAX_PARALLEL));
    out = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex)*MAX_SIZE*MAX_PARALLEL));

    if((in == NULL) || (out == NULL)) {
        DEF_LOGGING_CRITICAL("cannot malloc memory for %d 32 bit elements\n", MAX_SIZE);
        exit(1);
    }

    // generate individual channels
    for(int s = 0; s < LIST_SIZE; s++) {
        DEF_LOGGING_INFO("Generating 64 bit transform for %d elements\n", CHANNELS[s]);
        p = fftw_plan_dft_1d(CHANNELS[s], in, out, FFTW_FORWARD,
                              FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        fftw_destroy_plan(p);
    }
    // generate all parallel components
    for(int pa=1; pa <= MAX_PARALLEL; pa++) {
        for(int s = 0; s < LIST_SIZE; s++) {
            DEF_LOGGING_INFO("Generating 64 bit transform for %dx%d elements\n", CHANNELS[s], pa);
            int rank = 1;
            int n[1];
            n[0] = CHANNELS[s];
            int idist = CHANNELS[s];
            int odist = CHANNELS[s]; // distance from 0th element of 0th array to
                              // 0th element of 1st array
            int istride = 1; // contiguous
            int ostride = 1; // contiguous
            
            p = fftw_plan_many_dft(rank, n, pa,
                                    in, n, istride, idist,
                                    out, n, ostride, odist,
                                    FFTW_FORWARD,
                                    FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
            fftw_destroy_plan(p);
        }
    }


    fftw_free(in);
    fftw_free(out);  
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}


#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
void do_80(const int MAX_SIZE,
           const int MAX_PARALLEL,
           const int LIST_SIZE,
           const int * const restrict CHANNELS)
{
    fftwl_complex* in, *out;
    fftwl_plan p;

    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    in = reinterpret_cast<fftwl_complex*>(fftwl_malloc(sizeof(fftwl_complex)*MAX_SIZE*MAX_PARALLEL));
    out = reinterpret_cast<fftwl_complex*>(fftwl_malloc(sizeof(fftwl_complex)*MAX_SIZE*MAX_PARALLEL));

    if((in == NULL) || (out == NULL)) {
        DEF_LOGGING_CRITICAL("cannot malloc memory for %d 32 bit elements\n", MAX_SIZE);
        exit(1);
    }

    // generate individual channels
    for(int s = 0; s < LIST_SIZE; s++) {
        DEF_LOGGING_INFO("Generating 80 bit transform for %d elements\n", CHANNELS[s]);
        p = fftwl_plan_dft_1d(CHANNELS[s], in, out, FFTW_FORWARD,
                              FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        fftwl_destroy_plan(p);
    }
    // generate all parallel components
    for(int pa=1; pa <= MAX_PARALLEL; pa++) {
        for(int s = 0; s < LIST_SIZE; s++) {
            DEF_LOGGING_INFO("Generating 80 bit transform for %dx%d elements\n", CHANNELS[s], pa);
            int rank = 1;
            int n[1];
            n[0] = CHANNELS[s];
            int idist = CHANNELS[s];
            int odist = CHANNELS[s]; // distance from 0th element of 0th array to
                              // 0th element of 1st array
            int istride = 1; // contiguous
            int ostride = 1; // contiguous
            
            p = fftwl_plan_many_dft(rank, n, pa,
                                    in, n, istride, idist,
                                    out, n, ostride, odist,
                                    FFTW_FORWARD,
                                    FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
            fftwl_destroy_plan(p);
        }
    }


    fftwl_free(in);
    fftwl_free(out);  
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT


#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
void do_128(const int MAX_SIZE,
            const int MAX_PARALLEL,
            const int LIST_SIZE,
            const int * const restrict CHANNELS)
{
    fftwq_complex* in, *out;
    fftwq_plan p;

    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    in = reinterpret_cast<fftwq_complex*>(fftwq_malloc(sizeof(fftwq_complex)*MAX_SIZE*MAX_PARALLEL));
    out = reinterpret_cast<fftwq_complex*>(fftwq_malloc(sizeof(fftwq_complex)*MAX_SIZE*MAX_PARALLEL));

    if((in == NULL) || (out == NULL)) {
        DEF_LOGGING_CRITICAL("cannot malloc memory for %d 32 bit elements\n", MAX_SIZE);
        exit(1);
    }

    // generate individual channels
    for(int s = 0; s < LIST_SIZE; s++) {
        DEF_LOGGING_INFO("Generating 128 bit transform for %d elements\n", CHANNELS[s]);
        p = fftwq_plan_dft_1d(CHANNELS[s], in, out, FFTW_FORWARD,
                              FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
        fftwq_destroy_plan(p);
    }
    // generate all parallel components
    for(int pa=1; pa <= MAX_PARALLEL; pa++) {
        for(int s = 0; s < LIST_SIZE; s++) {
            DEF_LOGGING_INFO("Generating 128 bit transform for %dx%d elements\n", CHANNELS[s], pa);
            int rank = 1;
            int n[1];
            n[0] = CHANNELS[s];
            int idist = CHANNELS[s];
            int odist = CHANNELS[s]; // distance from 0th element of 0th array to
                              // 0th element of 1st array
            int istride = 1; // contiguous
            int ostride = 1; // contiguous
            
            p = fftwq_plan_many_dft(rank, n, pa,
                                    in, n, istride, idist,
                                    out, n, ostride, odist,
                                    FFTW_FORWARD,
                                    FFTW_EXHAUSTIVE|FFTW_DESTROY_INPUT);
            fftwq_destroy_plan(p);
        }
    }


    fftwq_free(in);
    fftwq_free(out);  
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT













int main(int argc, char* argv[])
{
    if(argc != 3) {
        DEF_LOGGING_CRITICAL("correct usage is %s channelization_file process_flag\n    where channelization_file is the filename of a text file containing\n    a list of integer channel numbers to calculate wisdom for, and\n    process_flag is a hexadecimal number whose bits indicate which bit sizes to process\n    so 0x1 indicates 16 bit half-precision floats, 0x2 indicates 32 bit floats,\n    0x4 indicates 64 bit doubles, 0x8 indicates 80 bit long doubles,\n    and 0x10 indicates 128 bit quad floats, and\n    you may or the bit pattern as desired.\n    (16 bit half-precision floats are not yet fully implemented.)\n", argv[0]);
        exit(2);
    }
    MPIfR::LOGGING::LOGGING_BLANK_.reset_display(stdout,MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_INFO);
    MPIfR::LOGGING::LOGGING_BLANK_.reset_disk("MPIfR_FFTW3_default_wisdom.log",MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET);
    DEF_LOGGING_INFO("%s [version %s, build date %sT%s] starting\n", MPIfR::LOGGING::PROGRAM_NAME, MPIfR::LOGGING::PROGRAM_VERSION, MPIfR::LOGGING::PROGRAM_DATE, MPIfR::LOGGING::PROGRAM_TIME);

    {
        int_fast32_t return_code = MPIfR::Numerical::FFTW3::init_MPIfR_FFTW3();
        if((return_code)) {
            DEF_LOGGING_ERROR("init_MPIfR_FFTW3 returned %d", int(return_code));
            exit(1);
        }
    }

    
    const int BITMASK = strtol(argv[2], NULL, 16);
    if((BITMASK & 0x1E) == 0) {
        DEF_LOGGING_CRITICAL("invalid process_flag --- no bits set for available data types\n");
        exit(2);
    }
    int NUM_LIST;
    int* restrict channels;
    int_fast32_t retval = read_default_channel_list(argv[1],
                                                   &NUM_LIST,
                                                   &channels);
    if((retval)) {
        DEF_LOGGING_CRITICAL("could not load default channelization list\n");
        exit(2);
    }
    int MAX_SIZE = channels[0];
    for(int i=0; i < NUM_LIST; i++) {
        if(MAX_SIZE < channels[i]) {
            MAX_SIZE = channels[i];
        }
    }
    
    // the current LOFAR system can generate up to 61 beamlets per
    // RSP lane, with 2 polarizations, multiplied by 4 for the 4 bit mode
    const int MAX_PARALLEL = 61*2*4;


    if((BITMASK & 0x1)) {
        DEF_LOGGING_WARNING("16 bit half-precision floats not yet implemented");
    }
    if((BITMASK & 0x2)) {
        do_32(MAX_SIZE, MAX_PARALLEL, NUM_LIST, channels);
    }
    if((BITMASK & 0x4)) {
        do_64(MAX_SIZE, MAX_PARALLEL, NUM_LIST, channels);
    }
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    if((BITMASK & 0x8)) {
        do_80(MAX_SIZE, MAX_PARALLEL, NUM_LIST, channels);
    }
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    if((BITMASK & 0x10)) {
        do_128(MAX_SIZE, MAX_PARALLEL, NUM_LIST, channels);
    }
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    MPIfR::Numerical::FFTW3::store_current_wisdom();
    free(channels); channels = NULL;
    {
        int_fast32_t return_code = MPIfR::Numerical::FFTW3::shutdown_MPIfR_FFTW3();
        if((return_code)) {
            DEF_LOGGING_ERROR("shutdown_MPIfR_FFTW3 returned %d", int(return_code));
            exit(1);
        }
    }

    return 0;
}
