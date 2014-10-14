// MPIfR_FFTW3.h
// FFTW3 routine handling with multiple, *SINGLE* threads.
//_HIST  DATE NAME PLACE INFO
// 2011 Jan 29  James M Anderson  --MPIfR  start
// 2013 Sep 06  JMA  ---- add init and shutdown functions

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





#ifndef MPIfR_FFTW3_H
#define MPIfR_FFTW3_H

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
#include <stdio.h>
#include <stdlib.h>
#include "fftw3.h"
#include <pthread.h>
#include "MPIfR_logging.h"




#ifdef MPIfR_FFTW3_H_FILE
#  define NUM(x) =x;
#  define EXTERNNUM
#else
#  define NUM(x)
#  define EXTERNNUM extern
#endif






// set up a namespace area for stuff.
namespace MPIfR {

namespace Numerical {

namespace FFTW3 {


// GLOBALS

#define MPIfR_Numerical_FFTW3_Storage_Directory "FFTW3/"

#define MPIfR_Numerical_FFTW3_LuMP_Channel_Number_File "MPIfR_Numerical_Channelization_List.txt"


EXTERNNUM pthread_mutex_t MPIfR_Numerical_FFTW3_access_mutex NUM(PTHREAD_MUTEX_INITIALIZER);

// FUNCTIONS
inline void MPIfR_Numerical_FFTW3_lock_access_mutex(const char* const restrict FILE,
                                                    const int LINE) throw()
{
    int retval = pthread_mutex_lock(&MPIfR_Numerical_FFTW3_access_mutex);
    if((retval)) {
        DEF_LOGGING_PERROR("MPIfR_Numerical_FFTW3 locking mutex failed");
        DEF_LOGGING_CRITICAL("MPIfR_Numerical_FFTW3 locking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
        exit(1);
    }
    return;
}
#define MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF() MPIfR::Numerical::FFTW3::MPIfR_Numerical_FFTW3_lock_access_mutex(__FILE__,__LINE__)
inline void MPIfR_Numerical_FFTW3_unlock_access_mutex(const char* const restrict FILE,
                                                      const int LINE) throw()
{
    int retval = pthread_mutex_unlock(&MPIfR_Numerical_FFTW3_access_mutex);
    if((retval)) {
        DEF_LOGGING_PERROR("MPIfR_Numerical_FFTW3 unlocking mutex failed");
        DEF_LOGGING_CRITICAL("MPIfR_Numerical_FFTW3 unlocking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
        exit(1);
    }
    return;
}
#define MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF() MPIfR::Numerical::FFTW3::MPIfR_Numerical_FFTW3_unlock_access_mutex(__FILE__,__LINE__)


void load_standard_wisdom() throw();
void store_current_wisdom() throw();
int_fast32_t read_channel_list(int* const restrict NUM_LIST,
                               int* restrict * const restrict channels);
int_fast32_t init_MPIfR_FFTW3(void) throw();
int_fast32_t shutdown_MPIfR_FFTW3(void) throw();





}  // end namespace FFTW3
}  // end namespace Numerical
}  // end namespace MPIfR




#undef NUM
#undef EXTERNNUM
#endif // MPIfR_FFTW3_H
