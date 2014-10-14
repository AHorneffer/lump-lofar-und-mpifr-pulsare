// LOFAR_Station_Beamformed_Test_Writer.cxx
// try dumping out some test data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 17  James M Anderson  --MPIfR  start
// 2013 Jan 05  JMA  --bug fixes
// 2014 Apr 16  JMA  ---- fix implementation of spectral lines, adapt for
//                        multiple bit modes
// 2014 Apr 18  JMA  ---- rename to LOFAR_Station_Beamformed_Test_Writer.cxx


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



// INCLUDES
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "LOFAR_Station_Beamformed_Packet.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "MPIfR_logging.h"


namespace MPIfR {
namespace LOGGING {
extern const char PROGRAM_NAME[] = "LOFAR_Station_Beamformed_Test_Writer";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
};};




// set up a namespace area for stuff.
namespace {



void fill_spectral_lines(MPIfR::LOFAR::Station::RSP_beamformed_packet* const packet)
{
    const uint8_t NUM_Blocks = packet->nBlocks();
    if(NUM_Blocks == 16) {
        // OK
    }
    else {
        DEF_LOGGING_DEBUG("NUM_Blocks!=16 is not supported at this time");
    }
    const uint8_t NUM_Beamlets = packet->nBeamlets();
    if(NUM_Beamlets < 15) {
        DEF_LOGGING_CRITICAL("need at least 15 beamlets, only have %d", int(NUM_Beamlets));
        exit(1);
    }
    char* restrict data_start = packet->data_start();

    // for(uint_fast16_t beamlet=0;beamlet < NUM_Beamlets; beamlet++) {
    //     udata[beamlet*NUM_Blocks] = beamlet;
    //     for(uint8_t i=1; i < NUM_Blocks; i++) {
    //         udata[beamlet*NUM_Blocks+i] = beamlet;
    //     }
    // }


    
    if(packet->BM() == 0) {
        // 16 bit mode
        int16_t* restrict fdata = reinterpret_cast<int16_t* restrict>(data_start+8*2*NUM_Blocks);
        // do beamlet 2
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = i;    // X real
            fdata[4*i+1] = i;    // X imag
            fdata[4*i+2] = i;    // Y real
            fdata[4*i+3] = i;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // do beamlet 3
        int16_t p=1;
        int16_t q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 3 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = q;    // Y imag
            int16_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 4, go backwards
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 4 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = q;    // Y imag
            int16_t temp = q;
            q = -p;
            p = temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 5, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 1;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 1;    // Y real
            fdata[4*i+3] = 1;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 6, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 1;    // X real
            fdata[4*i+1] = 0;    // X imag
            fdata[4*i+2] = 1;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 7, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 1;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 8, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 9, zero
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 0;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 10, 90 degree offset
        p=1;
        q=0;
        int16_t last = -1;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 10 p=%2d q=%2d p2=%2d q2=%2d", int(p), int(q), int(q), int(last));
            fdata[4*i+0] = p;      // X real
            fdata[4*i+1] = q;      // X imag
            fdata[4*i+2] = q;      // Y real
            fdata[4*i+3] = last;   // Y imag
            last = q;
            q = p;
            p = -last;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 11, different frequencies for X and Y
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 11 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = -q;   // Y imag
            int16_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 12, intermediate frequency 1
        int p1[] = { 32767, 23170,     0,-23170,-32767,-23170,     0, 23170};
        int q1[] = {     0, 23170, 32767, 23170,     0,-23170,-32767,-23170};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = p1[i&0x7];    // X real
            fdata[4*i+1] = q1[i&0x7];    // X imag
            fdata[4*i+2] = p1[i&0x7];    // Y real
            fdata[4*i+3] = q1[i&0x7];    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 13, intermediate frequency 2
        int p2[] = { 32767, 30273, 23170, 12539,     0,-12539,-23170,-30273,-32767,-30273,-23170,-12539,     0, 12539, 23170, 30273};
        int q2[] = {     0, 12539, 23170, 30273, 32767, 30273, 23170, 12539,     0,-12539,-23170,-30273,-32767,-30273,-23170,-12539};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = p2[i&0xF];    // X real
            fdata[4*i+1] = q2[i&0xF];    // X imag
            fdata[4*i+2] = p2[i&0xF];    // Y real
            fdata[4*i+3] = q2[i&0xF];    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 14 count position
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = i;    // X real
            fdata[4*i+1] = i;    // X imag
            fdata[4*i+2] = i;    // Y real
            fdata[4*i+3] = i;    // Y imag
        }
        fdata += 4*NUM_Blocks;
    }
    else if(packet->BM() == 1) {
        // 8 bit mode
        int8_t* restrict fdata = reinterpret_cast<int8_t* restrict>(data_start+4*2*NUM_Blocks);
        // do beamlet 2
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = i;    // X real
            fdata[4*i+1] = i;    // X imag
            fdata[4*i+2] = i;    // Y real
            fdata[4*i+3] = i;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // do beamlet 3
        int8_t p=1;
        int8_t q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 3 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = q;    // Y imag
            int8_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 4, go backwards
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 4 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = q;    // Y imag
            int16_t temp = q;
            q = -p;
            p = temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 5, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 1;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 1;    // Y real
            fdata[4*i+3] = 1;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 6, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 1;    // X real
            fdata[4*i+1] = 0;    // X imag
            fdata[4*i+2] = 1;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 7, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 1;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 8, constant
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 1;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 9, zero
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = 0;    // X real
            fdata[4*i+1] = 0;    // X imag
            fdata[4*i+2] = 0;    // Y real
            fdata[4*i+3] = 0;    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 10, 90 degree offset
        p=1;
        q=0;
        int8_t last = -1;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 10 p=%2d q=%2d p2=%2d q2=%2d", int(p), int(q), int(q), int(last));
            fdata[4*i+0] = p;      // X real
            fdata[4*i+1] = q;      // X imag
            fdata[4*i+2] = q;      // Y real
            fdata[4*i+3] = last;   // Y imag
            last = q;
            q = p;
            p = -last;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 11, different frequencies for X and Y
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 11 p=%2d q=%2d", int(p), int(q));
            fdata[4*i+0] = p;    // X real
            fdata[4*i+1] = q;    // X imag
            fdata[4*i+2] = p;    // Y real
            fdata[4*i+3] = -q;   // Y imag
            int16_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 12, intermediate frequency 1
        int p1[] = {   127,    90,     0,   -90,  -127,   -90,     0,    90};
        int q1[] = {     0,    90,   127,    90,     0,   -90,  -127,   -90};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = p1[i&0x7];    // X real
            fdata[4*i+1] = q1[i&0x7];    // X imag
            fdata[4*i+2] = p1[i&0x7];    // Y real
            fdata[4*i+3] = q1[i&0x7];    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 13, intermediate frequency 2
        int p2[] = {   127,   117,    90,    49,     0,   -49,   -90,  -117,  -127,  -117,   -90,   -49,     0,    49,    90,   117};
        int q2[] = {     0,    49,    90,   117,   127,   117,    90,    49,     0,   -49,   -90,  -117,  -127,  -117,   -90,   -49};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = p2[i&0xF];    // X real
            fdata[4*i+1] = q2[i&0xF];    // X imag
            fdata[4*i+2] = p2[i&0xF];    // Y real
            fdata[4*i+3] = q2[i&0xF];    // Y imag
        }
        fdata += 4*NUM_Blocks;
        // Now for beamlet 14 count position
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[4*i+0] = i;    // X real
            fdata[4*i+1] = i;    // X imag
            fdata[4*i+2] = i;    // Y real
            fdata[4*i+3] = i;    // Y imag
        }
        fdata += 4*NUM_Blocks;
    }
    else if(packet->BM() == 2) {
        // 4 bit mode
        uint8_t* restrict fdata = reinterpret_cast<uint8_t* restrict>(data_start+2*2*NUM_Blocks);
        // do beamlet 2
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = i;    // X real
            fdata[2*i+1] = i;    // X imag
        }
        fdata += 2*NUM_Blocks;
        // do beamlet 3
        int8_t p=1;
        int8_t q=0;
        uint8_t val = 0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 3 p=%2d q=%2d", int(p), int(q));
            val = (p & 0xF) | ((q & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
            int8_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 4, go backwards
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 4 p=%2d q=%2d", int(p), int(q));
            val = (p & 0xF) | ((q & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
            int16_t temp = q;
            q = -p;
            p = temp;
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 5, constant
        val = 0x11;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 6, constant
        val = 0x1;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 7, constant
        val = 0x10;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 8, constant
        val = 0x1;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = 0;      // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 9, zero
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            fdata[2*i+0] = 0;      // X imag:real
            fdata[2*i+1] = 0;      // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 10, 90 degree offset
        p=1;
        q=0;
        int8_t last = -1;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 10 p=%2d q=%2d p2=%2d q2=%2d", int(p), int(q), int(q), int(last));
            val = (p & 0xF) | ((q & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            val = (q & 0xF) | ((last & 0xF) << 4);
            fdata[2*i+1] = val;    // Y imag:real
            last = q;
            q = p;
            p = -last;
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 11, different frequencies for X and Y
        p=1;
        q=0;
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            //DEF_LOGGING_DEBUG("beamlet 11 p=%2d q=%2d", int(p), int(q));
            val = (p & 0xF) | ((q & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            val = (p & 0xF) | ((-q & 0xF) << 4);
            fdata[2*i+1] = val;    // Y imag:real
            int16_t temp = q;
            q = p;
            p = -temp;
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 12, intermediate frequency 1
        int p1[] = {     7,     5,     0,    -5,    -7,    -5,     0,     5};
        int q1[] = {     0,     5,     7,     5,     0,    -5,    -7,    -5};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            val = (p1[i&0x7] & 0xF) | ((q1[i&0x7] & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 13, intermediate frequency 2
        int p2[] = {     7,     6,     5,     3,     0,    -3,    -5,    -6,    -7,    -6,    -5,    -3,     0,     3,     5,     6};
        int q2[] = {     0,     3,     5,     6,     7,     6,     5,     3,     0,    -3,    -5,    -6,    -7,    -6,    -5,    -3};
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            val = (p2[i&0xF] & 0xF) | ((q2[i&0xF] & 0xF) << 4);
            fdata[2*i+0] = val;    // X imag:real
            fdata[2*i+1] = val;    // Y imag:real
        }
        fdata += 2*NUM_Blocks;
        // Now for beamlet 14 count position
        for(uint8_t i=0; i < NUM_Blocks; i++) {
            val = (i & 0xF) | ((i & 0xF) << 4);
            fdata[2*i+0] = i;    // X real
            fdata[2*i+1] = i;    // X imag
        }
        fdata += 3*NUM_Blocks;
    }
    else {
        DEF_LOGGING_CRITICAL("unsupported bit depth %d", int(packet->BM()));
        exit(1);
    }
    return;
}
   













void write_packets(const char* filename, uint32_t start_second,
                   uint32_t NUM_seconds, int bit_depth)
{
    FILE* fp;
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
    fp = fopen(filename, "wb");
#else
    fp = stdout;
#endif
    if(fp == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
#else
    {
        int retval = setvbuf(fp, NULL, _IONBF, 0);
        if((retval)) {
            fprintf(stderr, "Error setting output buffering to none\n");
            exit(3);
        }
    }
#endif
    //time_t now = time(NULL);
    uint32_t second(start_second);
    uint32_t END = second + NUM_seconds;
    uint32_t HARDWARE_BUG_SECOND = second+3;
    //fprintf(stderr, "writer info 0x%X 0x%X\n", second, END);

    
    uint16_t CLOCK_SPEED_IN_MHz = 200;

    uint_fast32_t SAMPLES_PER_EVEN_SECOND = MPIfR::LOFAR::Station::NUM_TIME_SAMPLES_PER_EVEN_SECOND(CLOCK_SPEED_IN_MHz);
    uint_fast32_t SAMPLES_PER_TWO_SECONDS = MPIfR::LOFAR::Station::NUM_TIME_SAMPLES_PER_TWO_SECONDS(CLOCK_SPEED_IN_MHz);
    DEF_LOGGING_DEBUG("Have %u samples per even, %u samples per two", unsigned(SAMPLES_PER_EVEN_SECOND), unsigned(SAMPLES_PER_TWO_SECONDS));


    

    uint8_t Version = 0xAB;
    uint8_t SourceInfo = 0xCD;
    uint16_t Configuration = 0x1234;
    uint16_t Station = 0x5678;
    uint8_t NUM_Beamlets = 61;
    uint8_t NUM_Blocks = 16;
    uint8_t BM = 0;
    switch(bit_depth) {
    case 4: BM=2; NUM_Beamlets*=4; break;
    case 8: BM=1; NUM_Beamlets*=2; break;
    case 16:BM=0; NUM_Beamlets*=1; break;
    default: DEF_LOGGING_CRITICAL("bit depth %d is not supported", bit_depth);
        exit(2);
    }

    DEF_LOGGING_DEBUG("Have BM=%u, NUM_Beamlets=%u", BM, NUM_Beamlets);
    
    



    MPIfR::LOFAR::Station::RSP_beamformed_packet p;
    p.set_Ver(Version);
    p.set_Sour_0(SourceInfo);
    p.set_Sour_1(SourceInfo);
    p.set_Config(Configuration);
    p.set_Sta(Station);
    p.set_Config(Configuration);
    p.set_BM(BM);
    p.set_data_valid();
    p.set_nBeamlets(NUM_Beamlets);
    p.set_nBlocks(NUM_Blocks);


    
    MPIfR::LOFAR::Station::RSP_beamformed_packet p1;
    MPIfR::LOFAR::Station::RSP_beamformed_packet p2;
    MPIfR::LOFAR::Station::RSP_beamformed_packet p3;

    const uint16_t packet_size = p.packet_size();
    char* restrict data_start = p.data_start();
    uint64_t* restrict udata = reinterpret_cast<uint64_t* restrict>(data_start);

    uint64_t sample = 0;
    uint32_t block = 0;
    uint32_t block_since_even_second_start = (second & 0x1) ? SAMPLES_PER_EVEN_SECOND:0;
    uint64_t packet = 0;



    fill_spectral_lines(&p);
    

    while(second < END) {
        // set up the header information
        p.set_Ts(second);
        if(second == HARDWARE_BUG_SECOND) {
            p.set_Ts(0xFFFFFFFFU);
        }
        p.set_BSN(block);

        // copy important information into the data payload area
        udata[0] = sample;
        udata[1] = block;
        udata[2] = block_since_even_second_start;
        udata[3] = packet;
        udata[4] = second;

        if(packet >= 10000) {
            // standard write of packet
            size_t retval = fwrite(&p, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
        }
        // else if((packet > 5000) && (packet < 10000)) {
        //     // skip packet
        // }
        else if((packet == 0xA)) {
            // make packet out of order
            memcpy(&p1,&p,sizeof(p));
        }
        else if((packet == 0xB)) {
            // make packet out of order
            memcpy(&p2,&p,sizeof(p));
        }
        else if((packet == 0xC)) {
            // make packet out of order
            memcpy(&p3,&p,sizeof(p));
        }
        else if((packet == 0x13)) {
            size_t retval = fwrite(&p, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
            // write out of order packets
            retval = fwrite(&p1, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
            retval = fwrite(&p2, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
            retval = fwrite(&p3, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
        }
        else if(packet == 0x30) {
            // make packet invalid
            p.set_data_invalid();
            size_t retval = fwrite(&p, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
            p.set_data_valid();
        }
        else {
            // standard packet (packet <= 0x50)
            size_t retval = fwrite(&p, packet_size, 1, fp);
            if(retval != 1) {
                fprintf(stderr, "Error writing to '%s'\n", filename);
                exit(2);
            }
        }

        // update sample and block and second information
        sample += NUM_Blocks;
        packet++;
        block += NUM_Blocks;
        block_since_even_second_start += NUM_Blocks;
        if((second & 0x1)) {
            // odd second, we are looking for the two second block count
            if(block_since_even_second_start >= SAMPLES_PER_TWO_SECONDS) {
                //DEF_LOGGING_DEBUG("s=%8.8X, block=%u, be=%u", unsigned(second), unsigned(block), unsigned(block_since_even_second_start));
                ++second;
                block_since_even_second_start -= SAMPLES_PER_TWO_SECONDS;
                block = block_since_even_second_start;
                //DEF_LOGGING_DEBUG("new values s=%8.8X, block=%u, be=%u", unsigned(second), unsigned(block), unsigned(block_since_even_second_start));
            }
        }
        else {
            // even second, look for the even second count
            if(block >= SAMPLES_PER_EVEN_SECOND) {
                //DEF_LOGGING_DEBUG("s=%8.8X, block=%u, be=%u", unsigned(second), unsigned(block), unsigned(block_since_even_second_start));
                ++second;
                block -= SAMPLES_PER_EVEN_SECOND;
                //DEF_LOGGING_DEBUG("new values s=%8.8X, block=%u, be=%u", unsigned(second), unsigned(block), unsigned(block_since_even_second_start));
            }
        }
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
    fclose(fp);
#else
#endif
    return;
}



// GLOBALS


// FUNCTIONS



}  // end namespace



int main(int argc, char* argv[])
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    fprintf(stderr, "Warning: only tested on little-endian systems\n");
#endif
    if(argc != 5) {
        fprintf(stderr, "Error: correct usage is %s filename start_second NUM_seconds bit_depth\n", argv[0]);
        exit(2);
    }
    DEF_LOGGING_INFO("%s [version %s, build date %sT%s] starting\n", MPIfR::LOGGING::PROGRAM_NAME, MPIfR::LOGGING::PROGRAM_VERSION, MPIfR::LOGGING::PROGRAM_DATE, MPIfR::LOGGING::PROGRAM_TIME);
    
    uint32_t start_second(strtol(argv[2],NULL,0));
    uint32_t NUM_seconds(strtol(argv[3],NULL,0));
    int bit_depth(strtol(argv[4],NULL,0));
    //fprintf(stderr, "writer info 0x%X 0x%X\n", start_second, NUM_seconds);
    write_packets(argv[1], start_second, NUM_seconds, bit_depth);
    return 0;
}

