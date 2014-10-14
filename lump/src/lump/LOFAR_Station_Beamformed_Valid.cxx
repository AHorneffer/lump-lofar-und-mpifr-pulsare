// LOFAR_Station_Beamformed_Valid.cxx
// handling a vald bit file for LOFAR beamformed data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 31  James M Anderson  --MPIfR  start
// 2013 Mar 16  JMA  ---- extra throw()
// 2014 Sep 16  JMA  --- add 64 bit version of popcount
// 2014 Sep 16  JMA  --- use buffer for writing instead of writing by char



// Copyright (c) 2011,2013,2014 James M. Anderson <anderson@gfz-potsdam.de>

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
#include "LOFAR_Station_Beamformed_Valid.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <sys/mman.h>
#include <unistd.h>
#include "MPIfR_logging.h"




// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {




// GLOBALS


// FUNCTIONS




namespace {

static const uint_fast16_t NUM_256 = 256;
static const uint8_t NUMBER_BITS_ON_ARRAY[NUM_256] = {0x0, 0x1, 0x1, 0x2, 0x1, 0x2, 0x2, 0x3, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x5, 0x6, 0x6, 0x7, 0x6, 0x7, 0x7, 0x8};



inline uint8_t count_number_bits_uint8(const uint8_t& u){return NUMBER_BITS_ON_ARRAY[u];}


uint_fast32_t NumberOfSetBits(uint32_t i) throw()
{
    i = i - ((i >> 1) & UINT32_C(0x55555555));
    i = (i & UINT32_C(0x33333333)) + ((i >> 2) & UINT32_C(0x33333333));
    return uint_fast32_t(((i + ((i >> 4) & UINT32_C(0xF0F0F0F))) * UINT32_C(0x1010101)) >> 24);
}

uint_fast64_t NumberOfSetBits(uint64_t i)
{
    i = i - ((i >> 1) & UINT64_C(0x5555555555555555));
    i = (i & UINT64_C(0x3333333333333333)) + ((i >> 2) & UINT64_C(0x3333333333333333));
    return uint_fast64_t((((i + (i >> 4)) & UINT64_C(0xF0F0F0F0F0F0F0F)) * UINT64_C(0x101010101010101)) >> 56);
}

} // end namespace





LOFAR_Station_Beamformed_Valid_Writer::
LOFAR_Station_Beamformed_Valid_Writer(const char* restrict const filename)
    throw()
        :
        write_buffer_count(0),
        fp(0),
        bit_buffer(0),
        bits_used(0)
{
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
    fp = fopen(filename, "wbx");
#else
    fp = fopen(filename, "wb");
#endif
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("cannot open '%s' for output\n", filename);
        exit(2);
    }
    return;
}






LOFAR_Station_Beamformed_Valid_Writer::
~LOFAR_Station_Beamformed_Valid_Writer() throw()
{
    if(bits_used == 0) {
    }
    else {
        write_N_points(8-bits_used,0);
    }
    flush_write_buffer();
    fclose(fp); fp = NULL;
    return;
}











int_fast32_t LOFAR_Station_Beamformed_Valid_Writer::
write_N_points(uint_fast32_t N, uint_fast32_t valid) restrict throw()
{
    uint8_t fill_char = ((valid)) ? uint8_t(0xFF) : uint8_t(0);
    int_fast32_t retval = 0;
    if(bits_used == 0) {
        uint_fast32_t N_BIG = N >> 3;
        bits_used = N & 0x7;
        for(uint_fast32_t i=0; i < N_BIG; i++) {
            retval = put_c_in_write_buffer(fill_char);
        }
        bit_buffer = fill_char;
    }
    else {
        // more complicated
        if(N+bits_used >= 8) {
            uint8_t N_shift = 8-bits_used;
            N -= N_shift;
            bit_buffer = (bit_buffer>>N_shift) | (fill_char<<bits_used);
            retval = put_c_in_write_buffer(bit_buffer);
            uint_fast32_t N_BIG = N >> 3;
            bits_used = N & 0x7;
            for(uint_fast32_t i=0; i < N_BIG; i++) {
                retval = put_c_in_write_buffer(fill_char);
            }
            bit_buffer = fill_char;
        }
        else {
            // not enough to fill up the buffer
            uint8_t N_shift = 8-N;
            bits_used += N;
            bit_buffer = (bit_buffer>>N) | (fill_char<<N_shift);
            retval = 0;
        }
    }
    return retval;
}

    








LOFAR_Station_Beamformed_Valid_Reader::
LOFAR_Station_Beamformed_Valid_Reader(const char* restrict const filename,
                                      const uint_fast64_t NUM_SAMPLES_,
                                      const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE_)
    restrict throw()
            : MAX_SAMPLE_NUMBER(NUM_SAMPLES_),
              SAMPLE_OFFSET_FROM_TIME_REFERENCE(SAMPLE_OFFSET_FROM_TIME_REFERENCE_),
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
              fd(0)
#else
              fp(NULL)
#endif
{
    MAX_NUM_BYTES = MAX_SAMPLE_NUMBER>>3;
    if((MAX_SAMPLE_NUMBER&0x7)) {
        MAX_NUM_BYTES++;
    }
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    // 64 bit stuff
    // just memory map the file
    fd = open(filename, O_RDONLY|O_NOATIME);
    if(fd == -1) {
        DEF_LOGGING_PERROR("error opening valid bits file");
        DEF_LOGGING_CRITICAL("Error opening valid bits file '%s' for input\n", filename);
        exit(2);
    }
    {
        struct stat fs;
        int retval = fstat(fd, &fs);
        if(retval < 0) {
            DEF_LOGGING_PERROR("error getting file size");
            DEF_LOGGING_CRITICAL("Error with file '%s'\n", filename);
            exit(2);
        }
        if(uint_fast64_t(fs.st_size) < MAX_NUM_BYTES) {
            DEF_LOGGING_CRITICAL("Error with file '%s', only has %llu bytes, need %llu\n", filename, (unsigned long long)(fs.st_size), (unsigned long long)(MAX_NUM_BYTES));
            exit(2);
        }
    }
    
    memory_map = mmap(NULL, size_t(MAX_NUM_BYTES), PROT_READ,
                      MAP_SHARED|MAP_NORESERVE,
                      fd, 0);
    if(memory_map == MAP_FAILED) {
        DEF_LOGGING_PERROR("error memory mapping valid bits file");
        DEF_LOGGING_CRITICAL("Error memory mapping valid bits file '%s'\n", filename);
        exit(2);
    }
    valid_buffer = reinterpret_cast<const uint8_t* restrict>(memory_map);
#else
    // 32 bit stuff?
    fp = fopen(filename, "rbx");
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("cannot open '%s' for input\n", filename);
        exit(2);
    }
#endif
    
}






LOFAR_Station_Beamformed_Valid_Reader::
~LOFAR_Station_Beamformed_Valid_Reader() throw()
{
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    // 64 bit stuff
    int retval = munmap(memory_map, size_t(MAX_NUM_BYTES));
    if((retval)) {
        DEF_LOGGING_PERROR("error unmemory mapping valid bits file");
        DEF_LOGGING_CRITICAL("Error unmemory mapping valid bits file\n");
        exit(2);
    }
    memory_map=NULL;
    valid_buffer=0;
    retval = close(fd);
    if((retval)) {
        DEF_LOGGING_PERROR("error closing valid bits file");
        DEF_LOGGING_CRITICAL("Error closing valid bits file\n");
        exit(2);
    }
    fd=0;
    
#else
    // 32 bit stuff?
    fclose(fp); fp=NULL;
#endif
}


int_fast32_t LOFAR_Station_Beamformed_Valid_Reader::
read_N_points(uint_fast64_t start_sample,
              uint_fast32_t NUM_SAMPLES,
              uint8_t* const restrict valid) restrict throw()
{
    start_sample -= SAMPLE_OFFSET_FROM_TIME_REFERENCE;

    uint_fast64_t start_byte = start_sample >> 3;
    uint_fast32_t bit_pos = start_sample & 0x7;
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    uint8_t v_byte = valid_buffer[start_byte];
#else
    {
        off_t offset = start_byte;
        int retval = fseeko(fp, offset, SEEK_SET);
        if(retval == 0) {}
        else {return -2;}
    }
    int retval = getc(fp);
    if(retval != EOF) {}
    else {return -3;}
    uint8_t v_byte = uint8_t(retval);
#endif
    v_byte >>= bit_pos;
    for(uint_fast32_t i=0; i < NUM_SAMPLES; i++) {
        if(bit_pos == 8) {
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
            v_byte = valid_buffer[++start_byte];
#else
            retval = getc(fp);
            if(retval != EOF) {}
            else {return -3;}
            v_byte = uint8_t(retval);
#endif
            bit_pos = 0;
        }
        bit_pos++;
        valid[i] = v_byte & 0x1;
        v_byte >>= 1;
    }
    return 0;
}







}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR



