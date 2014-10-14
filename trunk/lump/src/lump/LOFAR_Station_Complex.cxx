// LOFAR_Station_Complex.cxx
// code for LOFAR Complex data types
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 23  James M Anderson  --MPIfR  Start
// 2014 Sep 04  JMA  ---- update for getting the max size of any supported type



// Copyright (c) 2011, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
#define LOFAR_Station_Complex_H_FILE
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
#include "LOFAR_Station_Complex.h"
#include <stdio.h>
#include <stdlib.h>
#include "JMA_math.h"
#include "MPIfR_Real16_t.h"
#include "MPIfR_logging.h"




// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {



// GLOBALS


// FUNCTIONS
size_t LOFAR_raw_data_type_enum_size(LOFAR_raw_data_type_enum type) throw()
{
    switch(type) {
    case L_int8_t:
        return sizeof(int8_t);
    case L_int16_t:
        return sizeof(int16_t);
    case L_int32_t:
        return sizeof(int32_t);
    case L_int64_t:
        return sizeof(int64_t);
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    case L_int128_t:
        return sizeof(int128_t);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT
    case L_Real16_t:
        return sizeof(MPIfR::DATA_TYPE::Real16_t);
    case L_Real32_t:
        return sizeof(Real32_t);
    case L_Real64_t:
        return sizeof(Real64_t);
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Real80_t:
        return sizeof(Real80_t);
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Real128_t:
        return sizeof(Real128_t);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Complex64_t:
        return sizeof(Complex64_t);
    case L_Complex128_t:
        return sizeof(Complex128_t);
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case L_Complex160_t:
        return sizeof(Complex160_t);
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_Complex256_t:
        return sizeof(Complex256_t);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case L_intComplex8_t:
        return sizeof(intComplex8_t);
    case L_intComplex16_t:
        return sizeof(intComplex16_t);
    case L_intComplex32_t:
        return sizeof(intComplex32_t);
    case L_intComplex64_t:
        return sizeof(intComplex64_t);
    case L_intComplex128_t:
        return sizeof(intComplex128_t);
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    case L_intComplex256_t:
        return sizeof(intComplex256_t);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
//     case :
//         return sizeof();
    case L_RAW_DATA_TYPE_MAX:
        // the maximum size of any of the supported data types
        return 32;
    default:
        DEF_LOGGING_CRITICAL("Unrecognized LOFAR data type %d for size function\n", int(type));
        exit(1);
    }
    return 0;
}





const char* const restrict LOFAR_raw_data_type_enum_Str(LOFAR_raw_data_type_enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_raw_data_type_enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_raw_data_type_enum";
    }
    return NULL;
}








}  // end namespace LOFAR
}  // end namespace MPIfR


