// test_LuMP_disk_access.cxx
// test how fast the disk recording is for various writing schemes
//_HIST  DATE NAME PLACE INFO
// 2012 Feb 08  James M Anderson  --MPIfR  start



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
#include <string.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>






//const uint_fast64_t MAX_OUTPUT_SIZE_ = 40000000000LL; //40 GB
const uint_fast64_t MAX_OUTPUT_SIZE_ = 4000000000LL; //4 GB






void test_C_FILE_putc(const char* const restrict filename,
                      const int NUM_FILES,
                      const uint_fast64_t MAX_OUTPUT_SIZE)
{
    const int MAX_FILENAME_LENGTH = 2048;
    char new_filename[MAX_FILENAME_LENGTH];
    FILE* fp[NUM_FILES];
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        fp[f] = fopen(new_filename, "wb");
        if(fp[f] == NULL) {
            fprintf(stderr, "Error: cannot open '%s' for writing\n", new_filename);
            exit(2);
        }
    }
    uint_fast64_t total_written_size = 0;
    struct timespec start_time;
    struct timespec end_time;
    int retval = clock_gettime(CLOCK_MONOTONIC,&start_time);
    if((retval)) {
        perror("Error getting start time");
        exit(1);
    }
    while(total_written_size < MAX_OUTPUT_SIZE) {
        for(int i=0; i < 100000; i++) {
            for(int f=0; f < NUM_FILES; f++) {
                int retcode = putc(f,fp[f]);
                if(retcode < 0) {
                    fprintf(stderr, "Error writing to file %d\n", f);
                    exit(2);
                }
                total_written_size++;
            }
        }
        int retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
        if((retval)) {
            perror("Error getting end time");
            exit(1);
        }
        if(end_time.tv_sec - start_time.tv_sec > 60) {
            break;
        }
    }
    retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
    if((retval)) {
        perror("Error getting end time");
        exit(1);
    }
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        retcode = fclose(fp[f]);
        if((retcode)) {
            fprintf(stderr, "Error closing file '%s'\n", new_filename);
            exit(2);
        }
        retcode = remove(new_filename);
        if((retcode)) {
            fprintf(stderr, "Error removing file '%s'\n", new_filename);
            exit(2);
        }
    }
    double t = end_time.tv_nsec - start_time.tv_nsec;
    if(t >= 0.0) {
        t = (end_time.tv_sec - start_time.tv_sec) + t*1E-9;
    }
    else {
        t = (end_time.tv_sec - start_time.tv_sec -1) + (t+1E9)*1E-9;
    }
    double data_rate = total_written_size / t;
    printf("C_BUFFERED_PUTC: Files: %5d  Chunks [B] %6d  WriteSpeed [B/s] %10.3E\n",
           NUM_FILES, 1, data_rate);
    return;
}



void test_C_FILE_buffered(const char* const restrict filename,
                          const int NUM_FILES,
                          const size_t BYTES_PER_WRITE,
                          const uint_fast64_t MAX_OUTPUT_SIZE)
{
    char data[BYTES_PER_WRITE];
    for(size_t i=0; i < BYTES_PER_WRITE; i++) {
        data[i] = char(i);
    }
    const int MAX_FILENAME_LENGTH = 2048;
    char new_filename[MAX_FILENAME_LENGTH];
    FILE* fp[NUM_FILES];
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        fp[f] = fopen(new_filename, "wb");
        if(fp[f] == NULL) {
            fprintf(stderr, "Error: cannot open '%s' for writing\n", new_filename);
            exit(2);
        }
    }
    uint_fast64_t total_written_size = 0;
    struct timespec start_time;
    struct timespec end_time;
    int retval = clock_gettime(CLOCK_MONOTONIC,&start_time);
    if((retval)) {
        perror("Error getting start time");
        exit(1);
    }
    while(total_written_size < MAX_OUTPUT_SIZE) {
        for(int f=0; f < NUM_FILES; f++) {
            size_t retcode = fwrite(data, BYTES_PER_WRITE, 1, fp[f]);
            if(retcode != 1) {
                fprintf(stderr, "Error writing to file %d\n", f);
                exit(2);
            }
            total_written_size += BYTES_PER_WRITE;
        }
        int retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
        if((retval)) {
            perror("Error getting end time");
            exit(1);
        }
        if(end_time.tv_sec - start_time.tv_sec > 60) {
            break;
        }
    }
    retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
    if((retval)) {
        perror("Error getting end time");
        exit(1);
    }
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        retcode = fclose(fp[f]);
        if((retcode)) {
            fprintf(stderr, "Error closing file '%s'\n", new_filename);
            exit(2);
        }
        retcode = remove(new_filename);
        if((retcode)) {
            fprintf(stderr, "Error removing file '%s'\n", new_filename);
            exit(2);
        }
    }
    double t = end_time.tv_nsec - start_time.tv_nsec;
    if(t >= 0.0) {
        t = (end_time.tv_sec - start_time.tv_sec) + t*1E-9;
    }
    else {
        t = (end_time.tv_sec - start_time.tv_sec -1) + (t+1E9)*1E-9;
    }
    double data_rate = total_written_size / t;
    printf("C_BUFFERED_IO  : Files: %5d  Chunks [B] %6lu  WriteSpeed [B/s] %10.3E\n",
           NUM_FILES, BYTES_PER_WRITE, data_rate);
    return;
}


void test_C_FILE_unbuffered(const char* const restrict filename,
                            const int NUM_FILES,
                            const size_t BYTES_PER_WRITE,
                            const uint_fast64_t MAX_OUTPUT_SIZE)
{
    if(BYTES_PER_WRITE%4096 != 0) {
        fprintf(stderr, "Error: block size %lld not a 4096 multiple\n", (long long)(BYTES_PER_WRITE));
        exit(1);
    }
    char data[BYTES_PER_WRITE];
    for(size_t i=0; i < BYTES_PER_WRITE; i++) {
        data[i] = char(i);
    }
    const int MAX_FILENAME_LENGTH = 2048;
    char new_filename[MAX_FILENAME_LENGTH];
    FILE* fp[NUM_FILES];
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        fp[f] = fopen(new_filename, "wb");
        if(fp[f] == NULL) {
            fprintf(stderr, "Error: cannot open '%s' for writing\n", new_filename);
            exit(2);
        }
        retcode = setvbuf(fp[f], NULL, _IONBF, 0);
        if((retcode)) {
            fprintf(stderr, "Error: cannot make stream unbuffered\n");
            exit(1);
        }
    }
    uint_fast64_t total_written_size = 0;
    struct timespec start_time;
    struct timespec end_time;
    int retval = clock_gettime(CLOCK_MONOTONIC,&start_time);
    if((retval)) {
        perror("Error getting start time");
        exit(1);
    }
    while(total_written_size < MAX_OUTPUT_SIZE) {
        for(int f=0; f < NUM_FILES; f++) {
            size_t retcode = fwrite(data, BYTES_PER_WRITE, 1, fp[f]);
            if(retcode != 1) {
                fprintf(stderr, "Error writing to file %d\n", f);
                exit(2);
            }
            total_written_size += BYTES_PER_WRITE;
        }
        int retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
        if((retval)) {
            perror("Error getting end time");
            exit(1);
        }
        if(end_time.tv_sec - start_time.tv_sec > 60) {
            break;
        }
    }
    retval = clock_gettime(CLOCK_MONOTONIC,&end_time);
    if((retval)) {
        perror("Error getting end time");
        exit(1);
    }
    for(int f=0; f < NUM_FILES; f++) {
        int retcode = snprintf(new_filename, MAX_FILENAME_LENGTH, "%s.%d",
                               filename, f);
        if((retcode <= 0) || (retcode > MAX_FILENAME_LENGTH)) {
            fprintf(stderr, "Error: cannot write new filename to string\n");
            exit(2);
        }
        retcode = fclose(fp[f]);
        if((retcode)) {
            fprintf(stderr, "Error closing file '%s'\n", new_filename);
            exit(2);
        }
        retcode = remove(new_filename);
        if((retcode)) {
            fprintf(stderr, "Error removing file '%s'\n", new_filename);
            exit(2);
        }
    }
    double t = end_time.tv_nsec - start_time.tv_nsec;
    if(t >= 0.0) {
        t = (end_time.tv_sec - start_time.tv_sec) + t*1E-9;
    }
    else {
        t = (end_time.tv_sec - start_time.tv_sec -1) + (t+1E9)*1E-9;
    }
    double data_rate = total_written_size / t;
    printf("C_UNBUFFERED_IO: Files: %5d  Chunks [B] %6lu  WriteSpeed [B/s] %10.3E\n",
           NUM_FILES, BYTES_PER_WRITE, data_rate);
    return;
}
       















int main(int argc, char* argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Error: correct usage is %s scratch_file\n", argv[0]);
        exit(2);
    }

    test_C_FILE_putc(argv[1], 1, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 1, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 16, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 128, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 256, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 512, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 1024, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 2048, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 1, 8192, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 1, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 1, 8192, MAX_OUTPUT_SIZE_);
    
    test_C_FILE_putc(argv[1], 8, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 1, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 16, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 128, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 256, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 512, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 1024, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 2048, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 8, 8192, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 8, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 8, 8192, MAX_OUTPUT_SIZE_);
    
    test_C_FILE_putc(argv[1], 61, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 1, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 16, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 128, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 256, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 512, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 1024, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 2048, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 61, 8192, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 61, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 61, 8192, MAX_OUTPUT_SIZE_);
    
    test_C_FILE_putc(argv[1], 122, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 1, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 16, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 128, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 256, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 512, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 1024, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 2048, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_buffered(argv[1], 122, 8192, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 122, 4096, MAX_OUTPUT_SIZE_);
    test_C_FILE_unbuffered(argv[1], 122, 8192, MAX_OUTPUT_SIZE_);
    

    return 0;
}
