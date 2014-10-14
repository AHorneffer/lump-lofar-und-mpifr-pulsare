// test_ascii_dump_power0.cxx
// test code to convert a beamlet to ASCII for power00 format
//_HIST  DATE NAME PLACE INFO
// 2011 May 15  James M Anderson --MPIfR  start
// 2014 Sep 21  JMA  --- update to version 0x0230



// Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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




// see LOFAR_Station_Beamformed_Writer_Power0.cxx for format details



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
#include <string.h>
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Beamformed_Info.h"
#include "MPIfR_logging.h"


namespace MPIfR {
namespace LOGGING {
extern const char PROGRAM_NAME[] = "test_ascii_dump_power0";
extern const char PROGRAM_VERSION[] = "2014Sep21";
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
};};




// set up a namespace area for stuff.
namespace {




// GLOBALS


// FUNCTIONS



}  // end namespace






int main(const int argc, const char* const argv[])
{
    if(argc != 3) {
        fprintf(stderr, "Error: correct usage is %s filename_base beamlet_nr\n", argv[0]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(argv[1], "rb");
    const uint_fast16_t beamlet(strtoul(argv[2],NULL,0));




    if(info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_POWER0_OUT)) {
        fprintf(stderr, "Error: wrong writer type '%u' in %s:%d\n",
                unsigned(info.get_WRITER_TYPE()), __FILE__, __LINE__);
        exit(2);
    }
    if(info.get_NUM_CHANNELS() != 1) {
        fprintf(stderr, "Error: wrong number channels '%u' in %s:%d\n",
                unsigned(info.get_NUM_CHANNELS()), __FILE__, __LINE__);
        exit(2);
    }
    uint32_t o_index = 0;
    {
        uint_fast32_t* restrict beamlets = info.read_beamlets_data(argv[1]);
        if(beamlets == NULL) {
            fprintf(stderr, "Error reading beamlets data\n");
            exit(2);
        }
        uint32_t NUM_BEAMLETS = info.get_beamlets_per_sample();
        bool found = false;
        for(uint32_t b=0; b < NUM_BEAMLETS; b++) {
            if(beamlet == beamlets[b]) {
                found = true;
                o_index = b;
                break;
            }
        }
        if(!found) {
            fprintf(stderr, "Error: beamlet requested '%u' is not in data in %s:%d\n",
                    unsigned(beamlet), __FILE__, __LINE__);
            exit(2);
        }
        free(beamlets);
    }
    if(info.get_station_DATA_TYPE() != int32_t(MPIfR::LOFAR::L_intComplex32_t)) {
        fprintf(stderr, "Error: bad data type '%d' in %s:%d\n",
                int(info.get_station_DATA_TYPE()), __FILE__, __LINE__);
        exit(2);
    }
    if(info.get_output_DATA_TYPE() != int32_t(MPIfR::LOFAR::L_Real64_t)) {
        fprintf(stderr, "Error: bad integration data type '%d' in %s:%d\n",
                int(info.get_output_DATA_TYPE()), __FILE__, __LINE__);
        exit(2);
    }
    const size_t NUM_INT = 4*info.get_beamlets_per_sample();
    const size_t sample_line_size = sizeof(Real64_t)*NUM_INT;
    Real64_t line[NUM_INT];
    const size_t o = 4*o_index;
    const size_t FSIZE=2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", argv[1]);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        fprintf(stderr, "Error creating file file filename from base '%s'\n", argv[1]);
        exit(2);
    }
    FILE* fp_name = fopen(new_filename,"r");
    if(fp_name == NULL) {
        fprintf(stderr, "unable to open file file '%s' in %s:%d\n",
                new_filename, __FILE__, __LINE__);
        exit(2);
    }
    if(fgets(new_filename, FSIZE, fp_name) == NULL) {
        fprintf(stderr, "unable to read from file file '%s' in %s:%d\n",
                new_filename, __FILE__, __LINE__);
        exit(2);
    }
    fclose(fp_name);
    {
        size_t l = strlen(new_filename);
        if(l) {
            l--;
            if(new_filename[l] == '\n') {
                new_filename[l] = 0;
            }
        }
    }
    FILE* fp_data = fopen(new_filename,"rb");
    if(fp_data == NULL) {
        fprintf(stderr, "unable to open raw output data file '%s' in %s:%d\n",
                new_filename, __FILE__, __LINE__);
        exit(2);
    }
    const uint64_t num_integrations = info.get_num_output_timeslots();
    const Real64_t seconds_per_output = info.get_seconds_per_output();
    const Real64_t time_since_1970 = info.get_time_center_output_zero() + info.get_time_reference_zero();
    for(uint64_t sample=0; sample < num_integrations; sample++) {
        Real64_t time_offset = sample*seconds_per_output;
        if(fread(line,sample_line_size,1,fp_data) != 1) {
            fprintf(stderr, "Error reading from '%s' in %s:%d\n",
                    new_filename, __FILE__, __LINE__);
            exit(2);
        }
        printf("%.14E %.14E    %+.14E %+.14E %+.14E %+.14E\n",
               time_offset, time_since_1970,
               line[o], line[o+1], line[o+2], line[o+3]);
    }
    fclose(fp_data);
    return 0;
}


    
    
