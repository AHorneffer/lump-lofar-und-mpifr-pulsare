// LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.h
// VDIF packets for LOFAR beamformed data, LOFAR class 0
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 08  James M Anderson  --- GFZ start
// 2014 Sep 19  JMA  --- deal with copy and assignment
// 2014 Oct 14  JMA  --- use LuMP VDIF  writer version instead of version
//                       revision number in VDIF extended header

// Copyright (c) 2014 James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0_H
#define LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0_H

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
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include <stdio.h>
#include <stdlib.h>
#include "MPIfR_logging.h"
#include <exception>
#include "LOFAR_Station_Beamformed_VDIF_Packet.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {












//_CLASS  LOFAR_VDIF_Packet_LOFAR0
class LOFAR_VDIF_Packet_LOFAR0: public LOFAR_VDIF_Packet {
//_DESC  full description of class
// See 

//_FILE  files used and logical units used

//_LIMS  design limitations

//_BUGS  known bugs

//_CALL  list of calls

//_KEYS  

//_HIST  DATE NAME PLACE INFO

//_END


// NAMESPACE ISSUES    


public:
    static const uint8_t LuMP_EXTENDED_DATA_VERSION = 0x30;
    static const uint32_t LuMP_SYNC_PATTERN = UINT32_C(0xACABFEED);
    static const uint32_t LuMP_LOFAR_RAW_STATION_SAMPLES_IN_CHANNELIZER = 1024;


    LOFAR_VDIF_Packet_LOFAR0(const LOFAR_raw_data_type_enum DATA_TYPE,
                             const uint_fast16_t CLOCK_SPEED_IN_MHz,
                             const uint_fast32_t PHYSICAL_BEAMLET,
                             const uint_fast32_t PHYSICAL_SUBBAND,
                             const uint_fast32_t RCUMODE,
                             const uint_fast32_t station_ID_number,
                             const uint_fast32_t VDIF_writer_type
                             );
    // Default copy and assignment should be fine, so do not define
    // out own here.

    ~LOFAR_VDIF_Packet_LOFAR0() {return;}











    // Utility functions
    void set_LuMP_extended_user_data_version() restrict
    {
        return set_extended_user_data_version(LuMP_EXTENDED_DATA_VERSION);
    }


    
    bool sample_rate_unit_is_kHz() const restrict
    {
        uint32_t w = extended_user_data_0();
        return bool((w & UINT32_C(0x800000)) == 0);
    }
    bool sample_rate_unit_is_MHz() const restrict
    {
        uint32_t w = extended_user_data_0();
        return bool(w & UINT32_C(0x800000));
    }
    void set_sample_rate_unit_kHz() restrict
    {
        uint32_t w = extended_user_data_0();
        w = w & UINT32_C(0xFF7FFFFF);
        return set_extended_user_data_0(w);
    }
    void set_sample_rate_unit_MHz() restrict
    {
        uint32_t w = extended_user_data_0();
        w = w | UINT32_C(0x800000);
        return set_extended_user_data_0(w);
    }


    uint32_t sample_rate_numerator_raw() const restrict
    {
        uint32_t w = extended_user_data_0();
        return w & UINT32_C(0x7FFFFF);
    }
    void set_sample_rate_numerator_raw(const uint32_t r) restrict
    {
        uint32_t w = extended_user_data_0();
        w = (w & UINT32_C(0xFF800000)) | (r & UINT32_C(0x7FFFFF));
        return set_extended_user_data_0(w);
    }


    uint64_t sample_rate_numerator_full() const restrict
    {
        uint64_t r = extended_user_data_0();
        r = r * ( (sample_rate_unit_is_MHz()) ? UINT32_C(1000000):1000);
        return r;
    }
    void set_sample_rate_numerator_full(uint64_t r) restrict
    {
        r = r / ( (sample_rate_unit_is_MHz()) ? UINT32_C(1000000):1000);
        return set_sample_rate_numerator_raw(uint32_t(r));
    }

    

    void set_LuMP_sync_pattern() restrict
    {
        return set_extended_user_data_1(LuMP_SYNC_PATTERN);
    }


    uint32_t LOFAR_subband() const restrict
    {
        uint32_t w = extended_user_data_2();
        return (w >> 22);
    }
    void set_LOFAR_subband(const uint32_t s) restrict
    {
        uint32_t w = extended_user_data_2();
        w = (w & UINT32_C(0x3FFFFF)) | (s << 22);
        return set_extended_user_data_2(w);
    }


    uint32_t sample_rate_denominator_m1() const restrict
    {
        uint32_t w = extended_user_data_2();
        return (w & UINT32_C(0x3FFFFF));
    }
    uint32_t sample_rate_denominator() const restrict
    {
        return sample_rate_denominator_m1() +1;
    }
    void set_sample_rate_denominator_m1(const uint32_t d) restrict
    {
        uint32_t w = extended_user_data_2();
        w = (w & UINT32_C(0xFFC00000)) | (d & UINT32_C(0x3FFFFF));
        return set_extended_user_data_2(w);
    }
    void set_sample_rate_denominator(const uint32_t d) restrict
    {
        return set_sample_rate_denominator_m1(d-1);
    }


    uint32_t LOFAR_data_aquisition_system() const restrict
    {
        uint32_t w = extended_user_data_3();
        return (w >> 24);
    }
    void set_LOFAR_data_aquisition_system(const uint32_t v) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFFFFFF)) | (v << 24);
        return set_extended_user_data_3(w);
    }


    uint32_t LOFAR_station_configuration() const restrict
    {
        uint32_t w = extended_user_data_3();
        return ((w >> 16) & UINT32_C(0xFF));
    }
    void set_LOFAR_station_configuration(const uint32_t c) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFF00FFFF)) | ((c & UINT32_C(0xFF)) << 16);
        return set_extended_user_data_3(w);
    }


    uint32_t LOFAR_beamlet_rcumode() const restrict
    {
        uint32_t w = extended_user_data_3();
        return ((w >> 12) & UINT32_C(0xF));
    }
    void set_LOFAR_beamlet_rcumode(const uint32_t r) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFFFF0FFF)) | ((r & UINT32_C(0xF)) << 12);
        return set_extended_user_data_3(w);
    }


    uint32_t LuMP_software_major_version() const restrict
    {
        uint32_t w = extended_user_data_3();
        return ((w >> 8) & UINT32_C(0xF));
    }
    void set_LuMP_software_major_version(const uint32_t v) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFFFFF0FF)) | ((v & UINT32_C(0xF)) << 8);
        return set_extended_user_data_3(w);
    }


    uint32_t LuMP_software_minor_version() const restrict
    {
        uint32_t w = extended_user_data_3();
        return ((w >> 4) & UINT32_C(0xF));
    }
    void set_LuMP_software_minor_version(const uint32_t v) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFFFFFF0F)) | ((v & UINT32_C(0xF)) << 4);
        return set_extended_user_data_3(w);
    }


    uint32_t LuMP_VDIF_writer_type() const restrict
    {
        uint32_t w = extended_user_data_3();
        return (w & UINT32_C(0xF));
    }
    void set_LuMP_VDIF_writer_type(const uint32_t v) restrict
    {
        uint32_t w = extended_user_data_3();
        w = (w & UINT32_C(0xFFFFFFF0)) | (v & UINT32_C(0xF));
        return set_extended_user_data_3(w);
    }


    
    void convert_data_to_from_host_format() throw() {
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        byte_swap_data_area();
#endif
        return;
    }
   

    static size_t calculate_LuMP_VDIF_frame_size(const LOFAR_raw_data_type_enum DATA_TYPE,
                                                 const uint_fast16_t CLOCK_SPEED_IN_MHz) throw();
    
    static uint_fast32_t calculate_LuMP_VDIF_time_samples_per_frame(const LOFAR_raw_data_type_enum DATA_TYPE,
                                                 const uint_fast16_t CLOCK_SPEED_IN_MHz) throw();
    static uint_fast32_t calculate_LuMP_VDIF_num_frames_per_reference_second_stride(const LOFAR_raw_data_type_enum DATA_TYPE,
                                                                                  const uint_fast16_t CLOCK_SPEED_IN_MHz) throw();
    

    // Roll over the VDIF second counter by the below number of seconds,
    // once every number of seconds below.  The VDIF frame number is referenced
    // back to the second, and in order to fill frames and have a constant
    // integer number of frames per "second" epoch, and a constant number of
    // samples per frame, we have to go through these hoops.
    static const uint_fast64_t NUM_LOFAR_SECONDS_PER_VDIF_SECOND_STRIDE = 8;
    
        
protected:



private:


    inline void byte_swap_data_area() throw() {
        // only works for the supported LOFAR data types
        switch(bits_per_sample()) {
        case 8:
            // L_intComplex8_t --- nothing to do
            break;
        case 16:
            // L_intComplex16_t --- nothing to do
        case 32:
            // L_intComplex32_t;
            {
                uint_fast32_t NUM = frame_data_area_length_BYTES()/2;
                uint16_t* const restrict dp = (uint16_t* const restrict)JMA_SPECIFY_POINTER_DATA_ALIGNMENT(data_pointer(),LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE);
                for(uint_fast32_t i=0; i < NUM; ++i) {
                    dp[i] = bswap2(dp[i]);
                }
            }
            break;
        default:
            DEF_LOGGING_CRITICAL("LOFAR VDIF bit mode %d bits not supported.  Contact your developer.", bits_per_sample());
            exit(1);
        }
        return;
    }
    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS





}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0_H
