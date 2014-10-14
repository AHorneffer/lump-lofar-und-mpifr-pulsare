// MPIfR_Numerical_sampling_windows.h
// create sampling windows
//_HIST  DATE NAME PLACE INFO
// 2012 Apr 01  James M Anderson  --MPIfR  start

// Copyright (c) 2012, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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





#ifndef MPIFR_NUMERICAL_SAMPLING_WINDOWS_H
#define MPIFR_NUMERICAL_SAMPLING_WINDOWS_H

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
#include "MPIfR_Real16_t.h"




#ifdef MPIFR_NUMERICAL_SAMPLING_WINDOWS_H_FILE
#  define NUM(x) =x;
#else
#  define NUM(x)
#endif






// set up a namespace area for stuff.
namespace MPIfR {

namespace Numerical {



// ENUMERATIONS

//See http://en.wikipedia.org/wiki/Window_function

#define MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Array \
    MPIfR_ENUM_VALS(     MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Rectangular     , 0, "Rectangular")       \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hann            , 1, "Hann") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hamming         , 2, "Hamming") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Tukey           , 3, "Tukey") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Cosine          , 4, "Cosine") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Lanczos         , 5, "Lanczos") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Barlett0        , 6, "Barlett0") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_BarlettN0       , 7, "BarlettN0") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Gaussian        , 8, "Gaussian") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Bartlett_Hann   , 9, "Bartlett_Hann") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman        ,10, "Blackman") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Kaiser          ,11, "Kaiser") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Nuttall         ,12, "Nuttall") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Harris ,13, "Blackman-Harris") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Nuttall,14, "Blackman-Nuttall") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Flat_Top        ,15, "Flat-Top") \
        MPIfR_ENUM_VALS( MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_UNKNOWN         ,16, "MPIfR_NUMERICAL_TYPE_UNKNOWN") 



enum MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM {
#define MPIfR_ENUM_VALS(Enum, Val, String) Enum = Val,
    MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Array
#undef MPIfR_ENUM_VALS
};
const char* const restrict MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Str(MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM e) throw();





// GLOBALS



// FUNCTIONS


int_fast32_t generate_sampling_window_Real16_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               MPIfR::DATA_TYPE::Real16_t* * const data_pp);
int_fast32_t generate_sampling_window_Real32_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real32_t* * const data_pp);
int_fast32_t generate_sampling_window_Real64_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real64_t* * const data_pp);
#if defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
int_fast32_t generate_sampling_window_Real80_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real80_t* * const data_pp);
#endif
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
int_fast32_t generate_sampling_window_Real128_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                                const Real64_t window_parameter,
                                                const Real64_t scaling_muiltiplier,
                                                const int_fast32_t N,
                                                const bool complex_samples,
                                                Real128_t* * const data_pp);
#endif



// Polyphase filtering
int_fast32_t
generate_polyphase_sampling_window_Real16_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            MPIfR::DATA_TYPE::Real16_t* * const data_pp);
int_fast32_t
generate_polyphase_sampling_window_Real32_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real32_t* * const data_pp);
int_fast32_t
generate_polyphase_sampling_window_Real64_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real64_t* * const data_pp);
#if defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
int_fast32_t
generate_polyphase_sampling_window_Real80_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real80_t* * const data_pp);
#endif
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
int_fast32_t
generate_polyphase_sampling_window_Real128_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                             const Real64_t window_parameter,
                                             const Real64_t scaling_muiltiplier,
                                             const int_fast32_t N, // size of block
                                             const int_fast32_t P, // num taps
                                             const bool complex_samples,
                                             Real128_t* * const data_pp);
#endif








}  // end namespace Numerical
}  // end namespace MPIfR




#undef NUM
#endif // MPIFR_NUMERICAL_SAMPLING_WINDOWS_H
