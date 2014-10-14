// LOFAR_Station_Swap.h
// deal with byte-swapping data
//_HIST  DATE NAME PLACE INFO
// 2011 Aug 06  James M Anderson  --MPIfR  start

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





#ifndef LOFAR_Station_Swap_H
#define LOFAR_Station_Swap_H

// INCLUDES
#define __STDC_LIMIT_MACROS
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



// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





// GLOBALS
static const uint16_t LOFAR_Station_Swap_endian=0x100;


// FUNCTIONS

inline uint8_t LOFAR_Station_Swap_big_endian() throw() {return *((uint8_t *)&LOFAR_Station_Swap_endian);};


// byteswapping floats is tricky, as loading a byteswapped float into an FPU
// register can cause bits to get twiddled (e.g. automatic conversion of
// signalling NaNs to non-signalling NaNs), so until the bits are in their
// proper order, only load into integer registers.
inline int16_t LOFAR_Station_Swap_int16_t(int16_t i)
{
    // no __builtin_bswap16 in most gcc versions
    uint16_t u(i);
    return int16_t((u>>8)|(u<<8));
}
inline int32_t LOFAR_Station_Swap_int32_t(int32_t i)
{
#ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
    return __builtin_bswap32(i);
#  else
    uint32_t u(i);
    return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#  endif
#else
    uint32_t u(i);
    return int32_t((u>>24)|((u&0xFF0000)>>8)|((u&0xFF00)<<8)|(u<<24));
#endif
}
inline int64_t LOFAR_Station_Swap_int64_t(int64_t i)
{
#ifdef __GNUC__
#    if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 3))
    return __builtin_bswap64(i);
#  else
    uint64_t u(i);
    return int64_t(
                   ( (u & 0xFF00000000000000ULL) >> 56)
                  | ((u & 0x00FF000000000000ULL) >> 40)
                  | ((u & 0x0000FF0000000000ULL) >> 24)
                  | ((u & 0x000000FF00000000ULL) >> 8)
                  | ((u & 0x00000000FF000000ULL) << 8) 
                  | ((u & 0x0000000000FF0000ULL) << 24)
                  | ((u & 0x000000000000FF00ULL) << 40) 
                  | ((u & 0x00000000000000FFULL) << 56)
                   );
#  endif
#else
    uint64_t u(i);
    return int64_t(
                   ( (u & 0xFF00000000000000ULL) >> 56)
                  | ((u & 0x00FF000000000000ULL) >> 40)
                  | ((u & 0x0000FF0000000000ULL) >> 24)
                  | ((u & 0x000000FF00000000ULL) >> 8)
                  | ((u & 0x00000000FF000000ULL) << 8) 
                  | ((u & 0x0000000000FF0000ULL) << 24)
                  | ((u & 0x000000000000FF00ULL) << 40) 
                  | ((u & 0x00000000000000FFULL) << 56)
                   );
#endif
}


}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR



#endif // LOFAR_Station_Swap_H
