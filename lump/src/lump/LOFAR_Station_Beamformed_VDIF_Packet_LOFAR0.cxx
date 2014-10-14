// LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.cxx
// VDIF packets for LOFAR beamformed data, LOFAR class 0
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 08  James M Anderson  --- GFZ start
// 2014 Oct 14  JMA  --- use LuMP VDIF  writer version instead of version
//                       revision number in VDIF extended header


// Copyright (c) 2014, James M. Anderson <anderson@gfz-potsdam.de>

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
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.h"
#include "MPIfR_logging.h"
#include <string.h>


// set up a namespace area for stuff.
namespace {




// GLOBALS



// FUNCTIONS



}  // end namespace


namespace MPIfR {

namespace LOFAR {

namespace Station {









LOFAR_VDIF_Packet_LOFAR0::
LOFAR_VDIF_Packet_LOFAR0(const LOFAR_raw_data_type_enum DATA_TYPE,
                         const uint_fast16_t CLOCK_SPEED_IN_MHz,
                         const uint_fast32_t PHYSICAL_BEAMLET,
                         const uint_fast32_t PHYSICAL_SUBBAND,
                         const uint_fast32_t RCUMODE,
                         const uint_fast32_t station_ID_number,
                         const uint_fast32_t VDIF_writer_type
                         )
:
LOFAR_VDIF_Packet(calculate_LuMP_VDIF_frame_size(DATA_TYPE,CLOCK_SPEED_IN_MHz),false)
{
    //DEF_LOGGING_DEBUG("constructor: have %d %" PRIdFAST16 " %" PRIdFAST32 " %" PRIdFAST32 " %" PRIdFAST32 " %" PRIdFAST32, int(DATA_TYPE), CLOCK_SPEED_IN_MHz, PHYSICAL_BEAMLET, PHYSICAL_SUBBAND, RCUMODE, station_ID_number);
    // const uint32_t* const restrict dp = (const uint32_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(frame_pointer_const(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
    // for(uint_fast32_t i=0; i < 8; ++i) {
    //     DEF_LOGGING_DEBUG("START LOFAR VDIF packet word %" PRIdFAST32 " is 0x%8.8" PRIX32, i, dp[i]);
    // }
    // general VDIF setup
    set_data_valid();
    set_log_2_num_chan(1); // Write both X and Y polarizations into same thread
    set_data_type_complex();
    switch(DATA_TYPE) {
    case L_intComplex8_t:
        set_bits_per_sample(4);
        break;
    case L_intComplex16_t:
        set_bits_per_sample(8);
        break;
    case L_intComplex32_t:
        set_bits_per_sample(16);
        break;
    default:
        {
            DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                 int(DATA_TYPE));
            exit(1);
        }
    }
    set_thread_ID(uint32_t(PHYSICAL_BEAMLET));
    set_station_ID(uint32_t(station_ID_number));
   
    
    // LuMP specific things for VDIF
    set_LuMP_extended_user_data_version();
    set_sample_rate_unit_MHz();
    set_sample_rate_numerator_raw(uint32_t(CLOCK_SPEED_IN_MHz));
    set_LuMP_sync_pattern();
    set_LOFAR_subband(uint32_t(PHYSICAL_SUBBAND));
    set_sample_rate_denominator(LuMP_LOFAR_RAW_STATION_SAMPLES_IN_CHANNELIZER);
    set_LOFAR_beamlet_rcumode(uint32_t(RCUMODE));
    set_LuMP_software_major_version   (uint32_t((MPIfR_LOFAR_STATION_SOFTWARE_VERSION >> 8) & 0xF));
    set_LuMP_software_minor_version   (uint32_t((MPIfR_LOFAR_STATION_SOFTWARE_VERSION >> 4) & 0xF));
    set_LuMP_VDIF_writer_type(uint32_t(VDIF_writer_type & 0xF));
    // for(uint_fast32_t i=0; i < 8; ++i) {
    //     DEF_LOGGING_DEBUG("END LOFAR VDIF packet word %" PRIdFAST32 " is 0x%8.8" PRIX32, i, dp[i]);
    // }
    return;
}
    







size_t LOFAR_VDIF_Packet_LOFAR0::
calculate_LuMP_VDIF_frame_size(const LOFAR_raw_data_type_enum DATA_TYPE,
                               const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    switch(CLOCK_SPEED_IN_MHz) {
    case 200:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 5032;
        case L_intComplex16_t:
            return 5032;
        case L_intComplex32_t:
            return 5032;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
    case 160:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 6288;
        case L_intComplex16_t:
            return 8032;
        case L_intComplex32_t:
            return 8032;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#ifdef LOFAR_DEBUG
    case 0:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 5032;
        case L_intComplex16_t:
            return 5032;
        case L_intComplex32_t:
            return 5032;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#endif // LOFAR_DEBUG
    default:
        {
            DEF_LOGGING_CRITICAL("unknown CLOCK_SPEED_IN_MHz %d\n",
                                 int(CLOCK_SPEED_IN_MHz));
            exit(1);
        }
    }
    // should never reach here
    return 0;
}










uint_fast32_t LOFAR_VDIF_Packet_LOFAR0::
calculate_LuMP_VDIF_time_samples_per_frame(const LOFAR_raw_data_type_enum DATA_TYPE,
                                           const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    switch(CLOCK_SPEED_IN_MHz) {
    case 200:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 2500;
        case L_intComplex16_t:
            return 1250;
        case L_intComplex32_t:
            return 625;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
    case 160:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 2500;
        case L_intComplex16_t:
            return 2000;
        case L_intComplex32_t:
            return 1000;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#ifdef LOFAR_DEBUG
    case 0:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 2500;
        case L_intComplex16_t:
            return 1250;
        case L_intComplex32_t:
            return 625;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#endif // LOFAR_DEBUG
    default:
        {
            DEF_LOGGING_CRITICAL("unknown CLOCK_SPEED_IN_MHz %d\n",
                                 int(CLOCK_SPEED_IN_MHz));
            exit(1);
        }
    }
    // should never reach here
    return 0;
}




uint_fast32_t LOFAR_VDIF_Packet_LOFAR0::
calculate_LuMP_VDIF_num_frames_per_reference_second_stride(const LOFAR_raw_data_type_enum DATA_TYPE,
                                                           const uint_fast16_t CLOCK_SPEED_IN_MHz) throw()
{
    switch(CLOCK_SPEED_IN_MHz) {
    case 200:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 625;
        case L_intComplex16_t:
            return 1250;
        case L_intComplex32_t:
            return 2500;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
    case 160:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 500;
        case L_intComplex16_t:
            return 625;
        case L_intComplex32_t:
            return 1250;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#ifdef LOFAR_DEBUG
    case 0:
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            return 625;
        case L_intComplex16_t:
            return 1250;
        case L_intComplex32_t:
            return 2500;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
        break;
#endif // LOFAR_DEBUG
    default:
        {
            DEF_LOGGING_CRITICAL("unknown CLOCK_SPEED_IN_MHz %d\n",
                                 int(CLOCK_SPEED_IN_MHz));
            exit(1);
        }
    }
    // should never reach here
    return 0;
}




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR
