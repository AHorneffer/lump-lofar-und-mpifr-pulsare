// LOFAR_Station_Beamformed_VDIF_Packet.h
// VDIF packets for LOFAR beamformed data base class
//_HIST  DATE NAME PLACE INFO
// 2013 Mar 16  James M Anderson  --- MPIfR  start
// 2014 Apr 16  JMA  ---- change data_is_* to data_are_*
// 2014 Sep 07  JMA  ---- start real development of VDIF for LuMP
// 2014 Sep 19  JMA  --- fix checking of __GNUC__ to see that it is defined
// 2014 Sep 19  JMA  --- add gcc's bswap16 (gcc 4.8) and for completeness a
//                       bswap64
// 2014 Sep 19  JMA  --- allow copy and assignment

// Copyright (c) 2013, 2014 James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_VDIF_Packet_H
#define LOFAR_Station_Beamformed_VDIF_Packet_H

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








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {




class LOFAR_VDIF_Packet_exception_no_memory: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Could not allocate memory for LOFAR_VDIF_Packet";
    }
};
class LOFAR_VDIF_Packet_exception_bad_frame_size: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Told to use frame size for LOFAR_VDIF_Packet that is not a multiple of 8 bytes";
    }
};
class LOFAR_VDIF_Packet_exception_no_extended_user_data: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Requested to access extended user data, but this LOFAR_VDIF_Packet is in legacy mode";
    }
};



#define LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE 16
#if(LOFAR_VDIF_PACKET_1_1_1_MINIMUM_DATA_ALIGNMENT_GUARANTEE > MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT)
#  error "cannot have allocation alignment smaller than VDIF alignment"
#endif





//_CLASS  LOFAR_VDIF_Packet
class LOFAR_VDIF_Packet {
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

    static const int_fast8_t NORMAL_FRAME_HEADER_SIZE_BYTES = 32;
    static const int_fast8_t LEGACY_FRAME_HEADER_SIZE_BYTES = 16;
    static const int32_t     CURRENT_VDIF_OUTPUT_VERSION    = 0x0;

    

    // Use the following constructor when given an existing frame    
    LOFAR_VDIF_Packet(const size_t FRAME_SIZE_BYTES, bool legacy_mode_on=false);

    LOFAR_VDIF_Packet(const void* frame);
    // For the following, it is assumed that the memory is provided by
    // a malloc-compatible allocator.
    LOFAR_VDIF_Packet(void* frame, bool use_provided_pointer=true, bool delete_provided_pointer_on_destruction=false);

    LOFAR_VDIF_Packet(const LOFAR_VDIF_Packet& a);
    LOFAR_VDIF_Packet& operator=(const LOFAR_VDIF_Packet& a);


    ~LOFAR_VDIF_Packet();











    // Utility functions
    bool data_invalid() restrict const throw() {return bool(header[0] & UINT32_C(0x80000000));}
    void set_data_invalid() restrict throw() 
    {
        header[0] |= UINT32_C(0x80000000);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[0] = bswap4(header[0]);
#endif
    }
    void set_data_valid() restrict throw() 
    {
        header[0] &= UINT32_C(0x7FFFFFFF);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[0] = bswap4(header[0]);
#endif
    }

    bool legacy_mode_effective() restrict const throw() {return bool(header[0] & UINT32_C(0x40000000));}

    uint32_t seconds_from_reference_epoch() restrict const throw() {return header[0] & UINT32_C(0x40000000);}
    void set_seconds_from_reference_epoch(const uint32_t s) restrict throw() 
    {
        header[0] = (header[0] & UINT32_C(0xC0000000)) | (s & UINT32_C(0x3FFFFFFF));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[0] = bswap4(header[0]);
#endif
    }


    uint32_t reference_epoch() restrict const throw() {return (header[1] >> 24);}
    void set_reference_epoch(const uint32_t epoch) restrict throw()
    {
        header[1] = (header[1] & UINT32_C(0xFFFFFF)) | ((epoch << 24) & UINT32_C(0xFF000000));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[1] = bswap4(header[1]);
#endif
    }

      

    uint32_t data_frame_number_within_second() restrict const throw() {return (header[1] & UINT32_C(0xFFFFFF));}
    void set_data_frame_number_within_second(const uint32_t number) restrict throw()
    {
        header[1] = (header[1] & UINT32_C(0xFF000000)) | (number & UINT32_C(0xFFFFFF));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[1] = bswap4(header[1]);
#endif
    }


    uint32_t VDIF_version() restrict const throw() {return (header[2] >> 29);}
    void set_VDIF_version(const uint32_t version) restrict throw()
    {
        header[2] = (header[2] & UINT32_C(0x1FFFFFFF)) | ((version << 29) & UINT32_C(0xE0000000));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[2] = bswap4(header[2]);
#endif
    }


    uint32_t log_2_num_chan() restrict const throw() {return ((header[2] & UINT32_C(0x1F000000)) >> 24);}
    void set_log_2_num_chan(const uint32_t log_2_channels) restrict throw()
    {
        header[2] = (header[2] & UINT32_C(0xE0FFFFFF)) | ((log_2_channels & 0x1F) << 24);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[2] = bswap4(header[2]);
#endif
    }


    size_t frame_length_BYTES() restrict const throw() {return FRAME_BUFFER_SIZE_BYTES;}
    size_t frame_data_area_length_BYTES() restrict const throw() {return DATA_AREA_SIZE_BYTES;}
    

    uint32_t data_type() restrict const throw() {return (header[3] >> 31);}
    bool data_type_is_real() restrict const throw() {return (data_type() == 0);}
    bool data_type_is_complex() restrict const throw() {return (data_type() == 1);}
    void set_data_type(const uint32_t t) restrict throw()
    {
        header[3] = (header[3] & UINT32_C(0x7FFFFFFF)) | (t << 31);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[3] = bswap4(header[3]);
#endif
    }
    void set_data_type_real() restrict throw() {return set_data_type(0);}
    void set_data_type_complex() restrict throw() {return set_data_type(1);}


    uint32_t bits_per_sample_m1() restrict const throw() {return ((header[3] & UINT32_C(0x7C000000)) >> 26);}
    uint32_t bits_per_sample() restrict const throw() {return bits_per_sample_m1() +1;}
    void set_bits_per_sample_m1(const uint32_t b) restrict throw()
    {
        header[3] = (header[3] & UINT32_C(0x83FFFFFF)) | ((b & 0x1F) << 26);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[3] = bswap4(header[3]);
#endif
    }
    void set_bits_per_sample(const uint32_t b) restrict throw()
    {
        return set_bits_per_sample_m1(b-1);
    }


    uint32_t thread_ID() restrict const throw() {return ((header[3] & UINT32_C(0x3FF0000)) >> 16);}
    void set_thread_ID(const uint32_t id) restrict throw()
    {
        header[3] = (header[3] & UINT32_C(0xFC00FFFF)) | ((id & 0xFFFF) << 16);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[3] = bswap4(header[3]);
#endif
    }


    uint32_t station_ID() restrict const throw() {return (header[3] & UINT32_C(0xFFFF));}
    void station_ID(char* const station) restrict const throw()
    {
        char* s = static_cast<char*>(frame_buffer);
        s += 24;
        station[0] = s[0];
        station[1] = s[1];
        station[2] = 0;
        return;
    }
    void set_station_ID(const uint32_t id) restrict throw()
    {
        header[3] = (header[3] & UINT32_C(0xFFFF0000)) | (id & 0xFFFF);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[3] = bswap4(header[3]);
#endif
    }
    void set_station_ID(const char* const station) restrict throw()
    {
        char* s = static_cast<char*>(frame_buffer);
        s += 24;
        s[0] = station[0];
        s[1] = station[1];
        return;
    }


    uint32_t extended_user_data_version() restrict const throw()
    {
        if(legacy_mode_effective()) {
            throw(LOFAR_VDIF_Packet_exception_no_extended_user_data());
        }
        return (header[4] >> 24);
    }
    void set_extended_user_data_version(const uint32_t version) restrict throw()
    {
        header[4] = (header[4] & UINT32_C(0xFFFFFF)) | ((version << 24) & UINT32_C(0xFF000000));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[4] = bswap4(header[4]);
#endif
    }


    uint32_t extended_user_data_0() restrict const throw()
    {
        if(legacy_mode_effective()) {
            throw(LOFAR_VDIF_Packet_exception_no_extended_user_data());
        }
        return (header[4] & UINT32_C(0xFFFFFF));
    }
    void set_extended_user_data_0(const uint32_t u) restrict throw()
    {
        header[4] = (header[4] & UINT32_C(0xFF000000)) | (u & UINT32_C(0xFFFFFF));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[4] = bswap4(header[4]);
#endif
    }

    
uint32_t extended_user_data_1() restrict const throw()
    {
        if(legacy_mode_effective()) {
            throw(LOFAR_VDIF_Packet_exception_no_extended_user_data());
        }
        return header[5];
    }
    void set_extended_user_data_1(const uint32_t u) restrict throw()
    {
        header[5] = u;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[5] = bswap4(header[5]);
#endif
    }


    uint32_t extended_user_data_2() restrict const throw()
    {
        if(legacy_mode_effective()) {
            throw(LOFAR_VDIF_Packet_exception_no_extended_user_data());
        }
        return header[6];
    }
    void set_extended_user_data_2(const uint32_t u) restrict throw()
    {
        header[6] = u;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[6] = bswap4(header[6]);
#endif
    }


    uint32_t extended_user_data_3() restrict const throw()
    {
        if(legacy_mode_effective()) {
            throw(LOFAR_VDIF_Packet_exception_no_extended_user_data());
        }
        return header[7];
    }
    void set_extended_user_data_3(const uint32_t u) restrict throw()
    {
        header[7] = u;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[7] = bswap4(header[7]);
#endif
    }


    const void* frame_pointer_const() restrict const throw() {return frame_buffer;}
    void* frame_pointer() restrict throw() {return frame_buffer;}
    const void* data_pointer_const() restrict const throw() {return data_buffer;}
    void* data_pointer() restrict throw() {return data_buffer;}

        

    

    static uint64_t get_VDIF_epoch_floor_from_UNIX_timestamp(const uint64_t t) throw();
    static uint_fast8_t get_VDIF_epoch_floor_index_from_UNIX_timestamp(const uint64_t t) throw();

    static uint64_t VDIF_epoch_to_UNIX_timestamp(const uint8_t epoch) throw();
    
    
        
protected:


    

private:

    static const int_fast8_t MAX_HEADER_32_BIT_WORDS = 8;

    
    void* frame_buffer;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t* restrict header;
#else
    uint32_t header[MAX_HEADER_32_BIT_WORDS];
    uint32_t* restrict real_header;
#endif
    void* data_buffer;
    size_t FRAME_BUFFER_SIZE_BYTES;
    size_t DATA_AREA_SIZE_BYTES;


    bool delete_frame_buffer_on_destruction;




    void set_legacy_mode_on() restrict const throw()
    {
        header[0] |= UINT32_C(0x40000000);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[0] = bswap4(header[0]);
#endif
    }
    void set_legacy_mode_off() restrict const throw()
    {
        header[0] &= UINT32_C(0xBFFFFFFF);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[0] = bswap4(header[0]);
#endif
    }
    void set_frame_length_BYTES(size_t bytes) restrict const throw()
    {
        header[2] = (header[2] & UINT32_C(0xFF000000)) | ((bytes >> 3) &  UINT32_C(0xFFFFFF));
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        real_header[2] = bswap4(header[2]);
#endif
    }
    static size_t get_frame_size_in_BYTES_from_void_ptr(const void* vp) throw()
    {
        const uint32_t* up = static_cast<const uint32_t*>(vp);
        uint32_t u = up[2];
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        u = bswap4(u);
#endif
        return size_t((u & UINT32_C(0xFFFFFF)) << 3);
    }
    size_t get_frame_size_in_BYTES_from_header() throw()
    {
        return size_t((header[2] & UINT32_C(0xFFFFFF)) << 3);
    }
    void init_from_allocated_memory(void) restrict throw()
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        FRAME_BUFFER_SIZE_BYTES = get_frame_size_in_BYTES_from_header();
        if(!legacy_mode_effective()) {
            DATA_AREA_SIZE_BYTES = FRAME_BUFFER_SIZE_BYTES - NORMAL_FRAME_HEADER_SIZE_BYTES;
        }
        else {
            DATA_AREA_SIZE_BYTES = FRAME_BUFFER_SIZE_BYTES - LEGACY_FRAME_HEADER_SIZE_BYTES;
        }
#else
        uint32_t* restrict up = reinterpret_cast<uint32_t* restrict>(frame_buffer);
        uint_fast16_t i;
        for(i=0; i < 4; i++) {
            header[i] = bswap4(up[i]);
        }
        FRAME_BUFFER_SIZE_BYTES = get_frame_size_in_BYTES_from_header();
        if(!legacy_mode_effective()) {
            for(;i < MAX_HEADER_32_BIT_WORDS; i++) {
                header[i] = bswap4(up[i]);
            }
            DATA_AREA_SIZE_BYTES = FRAME_BUFFER_SIZE_BYTES - NORMAL_FRAME_HEADER_SIZE_BYTES;
        }
        else {
            DATA_AREA_SIZE_BYTES = FRAME_BUFFER_SIZE_BYTES - LEGACY_FRAME_HEADER_SIZE_BYTES;
        }
#endif
    }
    void setup_header_pointers(void) restrict throw()
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header = reinterpret_cast<uint32_t* restrict>(frame_buffer);
#else
        real_header = reinterpret_cast<uint32_t* restrict>(frame_buffer);
#endif
        return;
    }
    void setup_internal_pointers(void) restrict throw()
    {
        data_buffer = static_cast<void*>(static_cast<char*>(frame_buffer) + (FRAME_BUFFER_SIZE_BYTES-DATA_AREA_SIZE_BYTES));
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header = reinterpret_cast<uint32_t* restrict>(frame_buffer);
#else
        real_header = reinterpret_cast<uint32_t* restrict>(frame_buffer);
#endif
        return;
    }
       






protected:

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
        return __builtin_bswap32(u);
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



private:

    void deallocate_memory() throw()
    {
        if(delete_frame_buffer_on_destruction) {
            free(frame_buffer);
        }
        frame_buffer=0;
        data_buffer=0;
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header=0;
#else
        real_header=0;
#endif
        return;
    }


    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS





}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_VDIF_Packet_H
