// LOFAR_Station_Beamformed_Info_0002.cxx
// code for the Info_0002 class
//_HIST  DATE NAME PLACE INFO
// 2012 Apr 13  James M Anderson  --MPIfR  start based on 0001
// 2012 May 12  JMA  --RCUMODE is now per beamlet



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
#include "LOFAR_Station_Beamformed_Info_0002.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "LOFAR_Station_Swap.h"






// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {






// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Info_0002::LOFAR_Station_Beamformed_Info_0002(FILE* fp)
    throw()
{
    if(fread(reinterpret_cast<void*>(this),sizeof(LOFAR_Station_Beamformed_Info_0002),1,fp)
       != 1) {
        DEF_LOGGING_ERROR("cannot read in LOFAR_Station_Beamformed_Info_0002\n");
        memset(reinterpret_cast<void*>(this),0,sizeof(LOFAR_Station_Beamformed_Info_0002));
        return;
    }
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    byteswap();
#endif
    return;
}


size_t LOFAR_Station_Beamformed_Info_0002::write_to_file(FILE* fp) throw()
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return fwrite(reinterpret_cast<void*>(this),
                  sizeof(LOFAR_Station_Beamformed_Info_0002),
                  1,
                  fp);
#else
    LOFAR_Station_Beamformed_Info_0002 temp = *this;
    temp.byteswap();
    return fwrite(reinterpret_cast<void*>(&temp),
                  sizeof(LOFAR_Station_Beamformed_Info_0002),
                  1,
                  fp);
#endif
}



void LOFAR_Station_Beamformed_Info_0002::byteswap() throw()
{
    int16_t* i16;
    int32_t* i32;
    int64_t* i64;

    i16 = reinterpret_cast<int16_t*>(&VERSION); *i16 = LOFAR_Station_Swap_int16_t(*i16);
    i16 = reinterpret_cast<int16_t*>(&WRITER_TYPE); *i16 = LOFAR_Station_Swap_int16_t(*i16);
    i16 = reinterpret_cast<int16_t*>(&RCUMODE); *i16 = LOFAR_Station_Swap_int16_t(*i16);
    i16 = reinterpret_cast<int16_t*>(&station_clock_frequency); *i16 = LOFAR_Station_Swap_int16_t(*i16);
    i64 = reinterpret_cast<int64_t*>(&time_reference_zero); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_sample); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&sampler_frequency); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&Nyquist_offset); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i32 = reinterpret_cast<int32_t*>(&beamlets_per_sample); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&GRONINGEN_SAMPLE_COUNT_PER_BLOCK); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&station_DATA_TYPE); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&processing_DATA_TYPE); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&output_DATA_TYPE); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&channelizer_TYPE); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i64 = reinterpret_cast<int64_t*>(&FFT_window_parameter); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i32 = reinterpret_cast<int32_t*>(&FFT_window_TYPE); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&NUM_CHANNELS); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&NUM_POLYPHASE_TAPS); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&PAD1); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&samples_per_channelization); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i32 = reinterpret_cast<int32_t*>(&samples_per_integration); *i32 = LOFAR_Station_Swap_int32_t(*i32);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_channelization_full); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_channelization_eff); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&samples_per_channelization_eff); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_integration_full); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_integration_eff); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&samples_per_integration_eff); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&seconds_per_output); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&integration_multiplier); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&sample_start); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&time_center_output_zero); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    i64 = reinterpret_cast<int64_t*>(&num_output_timeslots); *i64 = LOFAR_Station_Swap_int64_t(*i64);
    return;
}



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


