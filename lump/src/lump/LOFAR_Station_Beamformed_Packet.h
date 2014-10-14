// LOFAR_Station_Beamformed_Packet.h
// What is in the beamformed UDP packet from a station
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 09  James M Anderson  --MPIfR  Start
// 2011 Mar 24  JMA  --update for revision 5.0 of LOFAR data output
// 2013 Jan 06  JMA  --update for 4/8 bit mode
// 2013 Mar 09  JMA  ---- enable setting clock mode
// 2013 Mar 16  JMA  ---- fix packet size code for multiple data sizes
// 2013 Jul 07  JMA  ---- bugfixes
// 2014 Apr 16  JMA  ---- change data_is_* to data_are_*
// 2014 Sep 19  JMA  --- fix checking of __GNUC__ to see that it is defined
// 2014 Sep 19  JMA  --- add gcc's bswap16 (gcc 4.8) and for completeness a
//                       bswap64

// Copyright (c) 2011, 2012, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_Packet_H
#define LOFAR_Station_Beamformed_Packet_H

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








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



//_CLASS  RSP_beamformed_packet
class RSP_beamformed_packet {
//_DESC  full description of class
// See RSP_CEP_ICD_v6_0.PDF
//  LOFAR-ASTRON-SDD-009

//_FILE  files used and logical units used

//_LIMS  design limitations

//_BUGS  known bugs

//_CALL  list of calls

//_KEYS  

//_HIST  DATE NAME PLACE INFO

//_END


// NAMESPACE ISSUES    


public:

    
    RSP_beamformed_packet() throw() {};
    void init() throw() {
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        Station_ID = bswap2(Station_ID);
        Timestamp = bswap4(Timestamp);
        BlockSequenceNumber = bswap4(BlockSequenceNumber);
#endif
    };
    // The following is valid for all bit modes, because of how the bit
    // modes are set up to provide multiple beamlet banks.
    uint_fast16_t block_data_size() const throw() {return uint_fast16_t(uint_fast16_t(NUM_Beamlets) << (3-BM()));}
    uint_fast16_t packet_data_size() const throw() {return uint_fast16_t((uint_fast16_t(NUM_Beamlets) << (3-BM()))*NUM_Blocks);}
    static uint_fast16_t packet_header_size() throw() {return uint_fast16_t(16);}
    uint_fast16_t packet_size() const throw() {return packet_data_size() + packet_header_size();}
    LOFAR_raw_data_type_enum data_type() const throw() {
        // Don't know how to interpret Configuration, so just guess
        switch(BM()) {
        case 0:
            return L_intComplex32_t;
        case 1:
            return L_intComplex16_t;
        case 2:
            return L_intComplex8_t;
        default:
            DEF_LOGGING_CRITICAL("LOFAR bit mode %X not supported.  Contact your developer.", BM());
            exit(1);
        }
        return L_intComplex32_t;
    }
    uint8_t Ver() const throw() {return Version;}
    uint8_t Sour_0() const throw() {return Source_Info_0;}
    uint8_t Sour_1() const throw() {return Source_Info_1;}
    // Take care of the bit mode when returning the number of beamlets.
    // Documentation is unclear whether the NOF_BEAMLETS field in the
    // header must be multiplied by the number of banks from the bit mode
    // setting, or whether this has already happened in the LCU.
    // uint_fast16_t nBeamlets() const throw() {return uint_fast16_t(NUM_Beamlets) << (Source_Info_1 & 0x2);}
    uint16_t nBeamlets() const throw() {return NUM_Beamlets;}
    uint8_t nBlocks() const throw() {return NUM_Blocks;}
    uint8_t Config() const throw() {return Configuration_ID;}
    uint16_t Sta() const throw() {return Station_ID;}
    time_t integer_time() const throw() {return time_t(Timestamp);}
    uint32_t uinteger_time() const throw() {return Timestamp;}
    uint32_t Block_Start() const throw() {return BlockSequenceNumber;}
    // Bit Mode:
    // 0  16 bit mode
    // 1   8 bit mode
    // 2   4 bit mode
    uint8_t BM() const throw() {return Source_Info_1 & uint8_t(0x3);}
    bool using_200_MHz_clock() const throw() {return bool(Source_Info_0&0x80);}
    bool data_are_valid() const throw() {return bool(!((Source_Info_0&0x40)));}
    bool data_are_invalid() const throw() {return bool((Source_Info_0&0x40));}
    uint8_t RSP_ID() const throw() {return Source_Info_0&uint8_t(0xF);}
    void convert_data_to_host_format() throw() {
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        swap_beamformed_data();
#endif
        return;}
    char* restrict data_start() throw() {return data_buffer;}
    const char* restrict data_start_const() const throw() {return data_buffer;}

    void set_nBeamlets(uint8_t nBeamlets) throw() {NUM_Beamlets = nBeamlets; return;}
    void set_nBlocks(uint8_t nBlocks) throw() {NUM_Blocks = nBlocks; return;}
    void set_Ver(uint8_t Ver) throw() {Version = Ver; return;}
    void set_Sour_0(uint8_t Sour_0) throw() {Source_Info_0 = Sour_0; return;}
    void set_Sour_1(uint8_t Sour_1) throw() {Source_Info_1 = Sour_1; return;}
    void set_Config(uint8_t Config) throw() {Configuration_ID = Config; return;}
    void set_Sta(uint16_t Sta) throw() {Station_ID = Sta; return;}
    void set_Ts(uint32_t ts) throw() {Timestamp = ts; return;}
    void set_BSN(uint32_t BSN) throw() {BlockSequenceNumber = BSN; return;}
    void set_BM(uint8_t BM_) throw() {Source_Info_1 = (Source_Info_1& 0xFC) | (BM_ & 0x3); return;}
    void set_200_MHz_clock() throw() {Source_Info_0 |= uint8_t(0x80); return;}
    void set_160_MHz_clock() throw() {Source_Info_0 &= uint8_t(0x7F); return;}
    void set_data_valid() throw() {Source_Info_0 &= uint8_t(0xBF); return;}
    void set_data_invalid() throw() {Source_Info_0 |= uint8_t(0x40); return;}
    void set_RSP_ID(uint8_t RSP_ID_) throw() {Source_Info_0 = (Source_Info_0& 0xF0) | (RSP_ID_ & 0xF); return;}
    // Probably need to set bit mode prior to setting number of beamlets
    void set_nBeamlets_BM(uint_fast16_t nBeamlets, uint8_t BM_) throw() {NUM_Beamlets = nBeamlets >> BM_; set_BM(BM_); return;}
    
        
protected:
    


private:
    uint8_t Version;
    uint8_t Source_Info_0;
    uint8_t Source_Info_1;
    uint8_t Configuration_ID;
    uint16_t Station_ID;
    uint8_t NUM_Beamlets;
    uint8_t NUM_Blocks;
    uint32_t Timestamp;
    uint32_t BlockSequenceNumber;
    char data_buffer[MAX_RSP_PACKET_SIZE_CHAR];



    static const uint16_t endian=0x100;
    inline uint8_t big_endian() const throw() {return *((uint8_t *)&endian);};
    inline uint16_t bswap2(const uint16_t& u) const throw() {
#if (defined __GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        return __builtin_bswap16(u);
#else
        return (u>> 8) | (u << 8);
#endif
    };
    inline int16_t bswap2(const int16_t& i) const throw() {
        uint16_t u = (uint16_t)i; 
#if (defined __GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        return (int16_t)__builtin_bswap16(u);
#else
        return (int16_t)((u>> 8) | (u << 8));
#endif
    };
    inline uint32_t bswap4(uint32_t& u) const throw() {
#if (defined __GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
        return (uint32_t)(__builtin_bswap32((int32_t)u));
#else
        return (u>>24)|((u&UINT32_C(0xFF0000))>>8)|((u&UINT32_C(0xFF00))<<8)|(u<<24);
#endif
    };
    inline uint64_t bswap8(uint64_t& u) const throw() {
#if (defined __GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
        return __builtin_bswap64(u);
#else
        return ( ( (u                               ) >> 56)
               | ( (u & UINT64_C(0x00FF000000000000)) >> 40)
               | ( (u & UINT64_C(0x0000FF0000000000)) >> 24)
               | ( (u & UINT64_C(0x000000FF00000000)) >>  8)
               | ( (u & UINT64_C(0x00000000FF000000)) <<  8)
               | ( (u & UINT64_C(0x0000000000FF0000)) << 24)
               | ( (u & UINT64_C(0x000000000000FF00)) << 40)
               | ( (u                               ) << 56)
                 );
#endif
    };
    inline void swap_beamformed_data() throw() {
        switch(BM()) {
        case 0:
            // L_intComplex32_t;
            {
                uint32_t NUM_SAMPLES = uint32_t(NUM_Beamlets)*NUM_Blocks*NUM_RSP_BEAMLET_POLARIZATIONS*2;
                int16_t* const restrict data = reinterpret_cast<int16_t* const restrict>(data_buffer);
                for(uint16_t i=0; i < NUM_SAMPLES; i++) {
                    data[i] = bswap2(data[i]);
                }
            }
            break;
        case 1:
            // L_intComplex16_t;
            // Nothing to do
            break;
        case 2:
            // L_intComplex8_t;
            // Nothing to do
            break;
        default:
            DEF_LOGGING_CRITICAL("LOFAR bit mode %X not supported.  Contact your developer.", BM());
            exit(1);
        }
        return;
    }
            




    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS
inline bool RSP_beamformed_packets_have_same_configuration(const RSP_beamformed_packet& a, const RSP_beamformed_packet& b) throw() {
    // The data valid bit messes up a simple comparison.
    const uint64_t* const ap = reinterpret_cast<const uint64_t* const>(&a);
    const uint64_t* const bp = reinterpret_cast<const uint64_t* const>(&b);
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return ((ap[0] | 0x4000) == (bp[0] | 0x4000));
#else
    return ((ap[0] | UINT64_C(0x40000000000000)) == (bp[0] | UINT64_C(0x40000000000000)));
#endif
}




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Packet_H
