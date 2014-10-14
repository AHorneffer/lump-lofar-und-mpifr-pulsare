// LOFAR_Station_Common.h
// COmmon defines, constants for LOFAR Station data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 11  James M Anderson  --MPIfR  start
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2013 Jan 12  JMA  --update for new output formats
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- fix DOFF max
// 2013 Aug 09  JMA  ---- add fielname copying function
// 2013 Aug 18  JMA  ---- add LuMP2 writer
// 2014 Apr 06  JMA  ---- improve 64 bit time_t handling
// 2014 Sep 01  JMA  ---- add FFT1, PFB0, P_FFT1, P_PFB1 types
// 2014 Sep 05  JMA  ---- add FFT2 type


// Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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





#ifndef LOFAR_Station_Common_H
#define LOFAR_Station_Common_H

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
#include "MPIfR_logging.h"


#ifdef LOFAR_Station_Common_H_FILE
#  define NUM(x) =x;
#else
#  define NUM(x)
#endif





// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



#define LOFAR_DATA_OUTPUT_FILE_FORMAT_Array \
    LOFAR_ENUM_VALS(     LOFAR_DOFF_STATION_IN_0  , 0, "LOFAR_DOFF_STATION_IN_0")       \
        LOFAR_ENUM_VALS( LOFAR_DOFF_RAW_OUT       , 1, "LOFAR_DOFF_RAW_OUT individual beamlets, separate polarizations, all beamlets, 1 channel")       \
        LOFAR_ENUM_VALS( LOFAR_DOFF_RAW0_OUT      , 2, "LOFAR_DOFF_RAW0_OUT selection of beamlets, beamlets in one file, 1 channel")       \
        LOFAR_ENUM_VALS( LOFAR_DOFF_RAW1_OUT      , 3, "LOFAR_DOFF_RAW1_OUT selection of beamlets, beamlets in separate files, separate polarizations, 1 channel")       \
        LOFAR_ENUM_VALS( LOFAR_DOFF_PulsarAris0   , 4, "LOFAR_DOFF_PulsarAris0")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_POWER0_OUT    , 5, "LOFAR_DOFF_POWER0_OUT selection of beamlets, all data in 1 file, 1 channel, power (not voltage)")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_LuMP0_OUT     , 6, "LOFAR_DOFF_LuMP0_OUT LuMP format, selection of beamlets, beamlets in separate files, 1 channel")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_FFT0_OUT      , 7, "LOFAR_DOFF_FFT0_OUT selection of beamlets, beamlets in one file, N FFT channels")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_PFB0_OUT      , 8, "LOFAR_DOFF_PFB0_OUT selection of beamlets, beamlets in one file, N PFB channels")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_POWER_FFT0_OUT, 9, "LOFAR_DOFF_POWER_FFT0_OUT selection of beamlets, beamlets in one file, N FFT channels, power (not voltage)")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_POWER_PFB0_OUT,10, "LOFAR_DOFF_POWER_PFB0_OUT selection of beamlets, beamlets in one file, N PFB channels, power (not voltage)")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_LuMP1_OUT     ,11, "LOFAR_DOFF_LuMP1_OUT LuMP format, selection of beamlets, beamlets in one file, 1 channel per beamlet")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_Packet0_OUT   ,12, "LOFAR_DOFF_Packet0_OUT raw LOFAR beamformed data packet format")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_VDIF0_OUT     ,13, "LOFAR_DOFF_VDIF0_OUT VDIF output format 0")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_LuMP2_OUT     ,14, "LOFAR_DOFF_LuMP2_OUT LuMP format, selection of beamlets, 1 channel per beamlet, stream to child processor")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_FFT1_OUT      ,15, "LOFAR_DOFF_FFT1_OUT selection of beamlets, beamlets in one file, N FFT channels, valid info scaled")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_PFB1_OUT      ,16, "LOFAR_DOFF_PFB1_OUT selection of beamlets, beamlets in one file, N PFB channels, valid info scaled")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_POWER_FFT1_OUT,17, "LOFAR_DOFF_POWER_FFT1_OUT selection of beamlets, beamlets in one file, N FFT channels, power (not voltage), valid info scaled")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_POWER_PFB1_OUT,18, "LOFAR_DOFF_POWER_PFB1_OUT selection of beamlets, beamlets in one file, N PFB channels, power (not voltage), valid info scaled")               \
        LOFAR_ENUM_VALS( LOFAR_DOFF_FFT2_OUT      ,19, "LOFAR_DOFF_FFT2_OUT selection of beamlets, beamlets in one file, N FFT channels, valid info scaled, valid transform out")               \
        LOFAR_ENUM_VALS( LOFAR_DATA_OUTPUT_FILE_FORMAT_MAX ,20, "LOFAR_DATA_OUTPUT_FILE_FORMAT_MAX")     \



enum LOFAR_DATA_OUTPUT_FILE_FORMAT {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_DATA_OUTPUT_FILE_FORMAT_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(LOFAR_DATA_OUTPUT_FILE_FORMAT e) throw();





#define LOFAR_STATION_TYPE_ENUM_Array \
    LOFAR_ENUM_VALS(     LOFAR_STATION_TYPE_Core         , 0, "Core")       \
        LOFAR_ENUM_VALS( LOFAR_STATION_TYPE_Remote       , 1, "Remote") \
        LOFAR_ENUM_VALS( LOFAR_STATION_TYPE_International, 2, "International") \
        LOFAR_ENUM_VALS( LOFAR_STATION_TYPE_UNKNOWN      , 3, "LOFAR_STATION_TYPE_UNKNOWN") \



enum LOFAR_STATION_TYPE_ENUM {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_STATION_TYPE_ENUM_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_STATION_TYPE_ENUM_Str(LOFAR_STATION_TYPE_ENUM e) throw();












// GLOBALS


const uint_fast32_t NUM_TIME_SAMPLES_PER_EVEN_SECOND_160_MHz = 156250;
const uint_fast32_t NUM_TIME_SAMPLES_PER_TWO_SECONDS_160_MHz = 312500;
const uint_fast32_t NUM_TIME_SAMPLES_PER_EVEN_SECOND_200_MHz = 195313;
const uint_fast32_t NUM_TIME_SAMPLES_PER_TWO_SECONDS_200_MHz = 390625;
const uint_fast32_t NUM_TIME_SAMPLES_PER_EVEN_SECOND_TEST = 60;
const uint_fast32_t NUM_TIME_SAMPLES_PER_TWO_SECONDS_TEST = 120;

extern const Real64_t   NUM_TIME_SAMPLES_PER_SECOND_160_MHz_REAL64 NUM(156250.0);
extern const Real64_t   NUM_TIME_SAMPLES_PER_SECOND_200_MHz_REAL64 NUM(195312.5);
extern const Real64_t   NUM_TIME_SAMPLES_PER_SECOND_TEST_REAL64 NUM(60.0);


const uint_fast16_t MPIfR_LOFAR_STATION_FULL_NUM_SUBBANDS = 512;

const uint_fast16_t MAX_RSP_BEAMLETS_PER_SAMPLE_TIMESTEP = 256;
const uint_fast16_t MAX_RSP_PACKET_SIZE_CHAR = 9600;
const uint8_t  NUM_RSP_BEAMLET_POLARIZATIONS = 2;
const uint8_t  LOFAR_STATION_COMMON_PAD[3] = {0,0,0};







// FUNCTIONS

inline uint_fast32_t NUM_TIME_SAMPLES_PER_EVEN_SECOND(const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    if(CLOCK_SPEED_IN_MHz == 200) {
        return NUM_TIME_SAMPLES_PER_EVEN_SECOND_200_MHz;
    }
    else if(CLOCK_SPEED_IN_MHz == 160) {
        return NUM_TIME_SAMPLES_PER_EVEN_SECOND_160_MHz;
    }
#ifdef LOFAR_DEBUG
    else if(CLOCK_SPEED_IN_MHz == 0) {
        return NUM_TIME_SAMPLES_PER_EVEN_SECOND_TEST;
    }
#endif // LOFAR_DEBUG
    else {
        DEF_LOGGING_CRITICAL("unknown clock speed %d\n", int(CLOCK_SPEED_IN_MHz));
        exit(1);
    }
    return 0;
}
inline uint_fast32_t NUM_TIME_SAMPLES_PER_TWO_SECONDS(const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    if(CLOCK_SPEED_IN_MHz == 200) {
        return NUM_TIME_SAMPLES_PER_TWO_SECONDS_200_MHz;
    }
    else if(CLOCK_SPEED_IN_MHz == 160) {
        return NUM_TIME_SAMPLES_PER_TWO_SECONDS_160_MHz;
    }
#ifdef LOFAR_DEBUG
    else if(CLOCK_SPEED_IN_MHz == 0) {
        return NUM_TIME_SAMPLES_PER_TWO_SECONDS_TEST;
    }
#endif // LOFAR_DEBUG
    else {
        DEF_LOGGING_CRITICAL("unknown clock speed %d\n", int(CLOCK_SPEED_IN_MHz));
        exit(1);
    }
    return 0;
}
inline Real64_t NUM_TIME_SAMPLES_PER_SECOND_REAL64(const uint16_t CLOCK_SPEED_IN_MHz) throw()
{
    if(CLOCK_SPEED_IN_MHz == 200) {
        return NUM_TIME_SAMPLES_PER_SECOND_200_MHz_REAL64;
    }
    else if(CLOCK_SPEED_IN_MHz == 160) {
        return NUM_TIME_SAMPLES_PER_SECOND_160_MHz_REAL64;
    }
#ifdef LOFAR_DEBUG
    else if(CLOCK_SPEED_IN_MHz == 0) {
        return NUM_TIME_SAMPLES_PER_SECOND_TEST_REAL64;
    }
#endif // LOFAR_DEBUG
    else {
        DEF_LOGGING_CRITICAL("unknown clock speed %d\n", int(CLOCK_SPEED_IN_MHz));
        exit(1);
    }
    return 0.0;
}
inline bool Station_Samples_Overlap_Second_Boundary(const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    if(CLOCK_SPEED_IN_MHz == 200) {
        return true;
    }
    else if(CLOCK_SPEED_IN_MHz == 160) {
        return false;
    }
#ifdef LOFAR_DEBUG
    else if(CLOCK_SPEED_IN_MHz == 0) {
        return false;
    }
#endif // LOFAR_DEBUG
    else {
        DEF_LOGGING_CRITICAL("unknown clock speed %d\n", int(CLOCK_SPEED_IN_MHz));
        exit(1);
    }
    return false;
}


inline int_fast64_t make_reference_time_zero(const int_fast64_t& reference_time) {
    return reference_time & UINT64_C(0xFFFFFFFFFFFFFFFE);
}




extern const char* const restrict LOFAR_ANTENNA_SET_Str(const char* const restrict name,
                                                        const int16_t RCUMODE
                                                        ) throw();
extern LOFAR_STATION_TYPE_ENUM LOFAR_Station_Type_From_Name(const char* const restrict name) throw();

extern const char* const restrict LOFAR_FILTER_SET_Str(const int16_t RCUMODE) throw();

// Returns a 3 character string (plus end of string terminator) to indicate
// which input was used for the RCUMODE, LBL, LBH, or HBA
extern const char* const LOFAR_RCUMODE_INPUT_CABLE_NAME(const uint_fast32_t RCUMODE) throw();



inline Real64_t LOFAR_SUBBAND_FREQUENCY_CENTER(uint_fast16_t CLOCK_SPEED_IN_MHz,
                                               const Real64_t Nyquist_Offset,
                                               const uint_fast32_t subband)
    throw()
{
    Real64_t Delta_nu_subband = NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz);
    return Nyquist_Offset + Delta_nu_subband * subband;
}





// Returns the Nyquist sampling offset, in Hz
inline Real64_t LOFAR_NYQUIST_FREQUENCY_OFFSET(const uint_fast32_t RCUMODE) throw()
{
    switch(RCUMODE) {
    case 1://  1 LBL  10--80 MHz,  200 MHz clock
    case 2://  2 LBL  30--80 MHz,  200 MHz clock
    case 3://  3 LBH  10--80 MHz,  200 MHz clock
    case 4://  4 LBH  30--80 MHz,  200 MHz clock
        return 0.0;
    case 5://  5 HBA 110--190 MHz, 200 MHz clock
        return 100E6;
    case 6://  6 HBA 170--230 MHz, 160 MHz clock
        return 160E6;
    case 7://  7 HBA 210--260 MHz, 200 MHz clock
        return 200E6;
    case 8://  8 LBH  10--80 MHz,  160 MHz clock
    case 9://  9 LBL  30--80 MHz,  160 MHz clock
        return 0.0;
    default:
        DEF_LOGGING_CRITICAL("unknown RCUMODE %d\n", int(RCUMODE));
        exit(1);
    }
    return 0.0;
}


inline bool LOFAR_RCUMODE_AND_CLOCK_MATCH(const uint_fast32_t RCUMODE,
                                          uint_fast16_t CLOCK_SPEED_IN_MHz)
    throw()
{
    switch(RCUMODE) {
    case 1://  1 LBL  10--80 MHz,  200 MHz clock
    case 2://  2 LBL  30--80 MHz,  200 MHz clock
    case 3://  3 LBH  10--80 MHz,  200 MHz clock
    case 4://  4 LBH  30--80 MHz,  200 MHz clock
    case 5://  5 HBA 110--190 MHz, 200 MHz clock
    case 6://  6 HBA 170--230 MHz, 160 MHz clock
    case 7://  7 HBA 210--260 MHz, 200 MHz clock
    case 8://  8 LBH  10--80 MHz,  160 MHz clock
    case 9://  9 LBL  30--80 MHz,  160 MHz clock
        break;
    default:
        DEF_LOGGING_CRITICAL("unknown RCUMODE %d\n", int(RCUMODE));
        exit(1);
    }
    if(CLOCK_SPEED_IN_MHz == 200) {
        switch(RCUMODE) {
        case 1://  1 LBL  10--80 MHz,  200 MHz clock
        case 2://  2 LBL  30--80 MHz,  200 MHz clock
        case 3://  3 LBH  10--80 MHz,  200 MHz clock
        case 4://  4 LBH  30--80 MHz,  200 MHz clock
        case 5://  5 HBA 110--190 MHz, 200 MHz clock
        case 7://  7 HBA 210--260 MHz, 200 MHz clock
            return true;
        default:
            return false;
        }
    }
    else if(CLOCK_SPEED_IN_MHz == 160) {
        switch(RCUMODE) {
        case 6://  6 HBA 170--230 MHz, 160 MHz clock
        case 8://  8 LBH  10--80 MHz,  160 MHz clock
        case 9://  9 LBL  30--80 MHz,  160 MHz clock
            return true;
        default:
            return false;
        }
    }
#ifdef LOFAR_DEBUG
    else if(CLOCK_SPEED_IN_MHz == 0) {
        return true;
    }
#endif // LOFAR_DEBUG
    else {
        DEF_LOGGING_CRITICAL("unknown clock speed %d\n", int(CLOCK_SPEED_IN_MHz));
        exit(1);
    }
    return false;
}




// Note that the caller is responsible for free'ing the returned pointer
char* copy_filename_without_extension(const char* const filename_orig,
                                      size_t extra_malloc_chars=0) throw();




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR




#undef NUM
#endif // LOFAR_Station_Common_H
