// LOFAR_Station_Beamformed_Valid_Real32.cxx
// handling a valid Real32 file for LOFAR beamformed data
//_HIST  DATE NAME PLACE INFO
// 2011 May 11  James M Anderson  --MPIfR  start



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
#include "LOFAR_Station_Beamformed_Valid_Real32.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "MPIfR_logging.h"



// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {




// GLOBALS


// FUNCTIONS





LOFAR_Station_Beamformed_Valid_Real32_Writer::
LOFAR_Station_Beamformed_Valid_Real32_Writer(const char* restrict const filename)
    throw()
        : fp(0)
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






LOFAR_Station_Beamformed_Valid_Real32_Writer::
~LOFAR_Station_Beamformed_Valid_Real32_Writer() throw()
{
    fclose(fp); fp = NULL;
    return;
}









LOFAR_Station_Beamformed_Valid_Real32_Reader::
LOFAR_Station_Beamformed_Valid_Real32_Reader(const char* restrict const filename,
                                             const uint_fast64_t NUM_SAMPLES_,
                                             const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE_)
    restrict throw()
            : MAX_SAMPLE_NUMBER(NUM_SAMPLES_),
              MAX_NUM_BYTES(NUM_SAMPLES_*sizeof(Real32_t)),
              SAMPLE_OFFSET_FROM_TIME_REFERENCE(SAMPLE_OFFSET_FROM_TIME_REFERENCE_),
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
              fd(0)
#else
              fp(NULL)
#endif
{
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    // 64 bit stuff
    // just memory map the file
    fd = open(filename, O_RDONLY|O_NOATIME);
    if(fd == -1) {
        DEF_LOGGING_PERROR("error opening valid bits file");
        DEF_LOGGING_CRITICAL("Error opening valid Real32 file '%s' for input\n", filename);
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
        DEF_LOGGING_PERROR("error memory mapping valid Real32 file");
        DEF_LOGGING_CRITICAL("Error memory mapping valid Real32 file '%s'\n", filename);
        exit(2);
    }
    valid_buffer = reinterpret_cast<const Real32_t* restrict>(memory_map);
#else
    // 32 bit pointer stuff?
    fp = fopen(filename, "rbx");
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("cannot open '%s' for input\n", filename);
        exit(2);
    }
#endif
    
}






LOFAR_Station_Beamformed_Valid_Real32_Reader::
~LOFAR_Station_Beamformed_Valid_Real32_Reader() throw()
{
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    // 64 bit stuff
    int retval = munmap(memory_map, size_t(MAX_NUM_BYTES));
    if((retval)) {
        DEF_LOGGING_PERROR("error unmemory mapping valid Real32 file");
        DEF_LOGGING_CRITICAL("Error unmemory mapping valid Real32 file\n");
        exit(2);
    }
    memory_map=NULL;
    valid_buffer=0;
    retval = close(fd);
    if((retval)) {
        DEF_LOGGING_PERROR("error closing valid Real32 file");
        DEF_LOGGING_CRITICAL("Error closing valid Real32 file\n");
        exit(2);
    }
    fd=0;
    
#else
    // 32 bit stuff?
    fclose(fp); fp=NULL;
#endif
}





int_fast32_t LOFAR_Station_Beamformed_Valid_Real32_Reader::
read_N_points(uint_fast64_t start_sample,
              uint_fast32_t NUM_SAMPLES,
              Real32_t* const restrict valid) restrict throw()
{
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    if((valid_buffer)) {}
    else {return -1;}
#else
    if((fp)) {}
    else {return -1;}
#endif
    start_sample -= SAMPLE_OFFSET_FROM_TIME_REFERENCE;
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    memcpy(reinterpret_cast<void*>(valid),
           reinterpret_cast<const void*>(valid_buffer+start_sample),
           NUM_SAMPLES*sizeof(Real32_t));
#  else
    int32_t* const restrict ip = reinterpret_cast<void*>(valid);
    int32_t* const restrict vip = reinterpret_cast<void*>(valid_buffer+start_sample);
    for(uint_fast32_t i=0; i < NUM_SAMPLES; i++) {
        ip[i] = LOFAR_Station_Swap_int32_t(vip[i]);
    }
#  endif
#else
    off_t offset = start_sample*sizeof(Real32_t);
    size_t bytes = NUM_SAMPLES*sizeof(Real32_t);
    int retval = fseeko(fp, offset, SEEK_SET);
    if(retval == 0) {}
    else {return -2;}
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    if(fread(reinterpret_cast<void*>(valid), bytes, 1, fp) == 1) {}
    else {return -3;}
#  else
    int32_t* const restrict ip = reinterpret_cast<void*>(valid);
    if(fread(reinterpret_cast<void*>(ip), bytes, 1, fp) == 1) {}
    else {return -3;}
    for(uint_fast32_t i=0; i < NUM_SAMPLES; i++) {
        ip[i] = LOFAR_Station_Swap_int32_t(ip[i]);
    }
#  endif
#endif
    return 0;
}












}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR



