// test_ascii_dump_power0_mean.cxx
// test code to convert a beamlet to ASCII from power0 format, mean and stddev
//_HIST  DATE NAME PLACE INFO
// 2012 Jan 25  James M Anderson --MPIfR  start
// 2014 Sep 21  JMA  --- update to version 0x0230



// Copyright (c) 2012, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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
extern const char PROGRAM_NAME[] = "test_ascii_dump_power0_mean";
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
    if(argc != 2) {
        fprintf(stderr, "Error: correct usage is %s filename_base\n", argv[0]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(argv[1], "rb");




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
    const size_t NUM_POL = 4;
    const size_t NUM_INT = NUM_POL*info.get_beamlets_per_sample();
    const size_t sample_line_size = sizeof(Real64_t)*NUM_INT;
    Real64_t line[NUM_INT];
    const size_t FSIZE=2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", argv[1]);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        fprintf(stderr, "Error creating raw filename from base '%s'\n", argv[1]);
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
    if(num_integrations < 2) {
        fprintf(stderr, "Error: only found %u integration timeslots --- cannot make mean and standard deviation\n", unsigned(num_integrations));
        exit(2);
    }

    
    uint_fast32_t* restrict subbands = info.read_subbands_data(argv[1]);
    if(subbands == NULL) {
        fprintf(stderr, "Error reading subbands data\n");
        exit(2);
    }
    
    uint32_t NUM_BEAMLETS = info.get_beamlets_per_sample();
    for(uint32_t b=0; b < NUM_BEAMLETS; b++) {
        const Real64_t frequency = info.get_Nyquist_offset()
            + info.get_sampler_frequency()/1024*subbands[b];
        printf("%E", frequency);
        const size_t o = NUM_POL*b;
        rewind(fp_data);
        Real64_t sum[NUM_POL];
        Real64_t sumsqr[NUM_POL];
        for(uint64_t p=0; p < NUM_POL; p++) {
            sum[p] = sumsqr[p] = 0.0;
        }
        for(uint64_t sample=0; sample < num_integrations; sample++) {
            if(fread(line,sample_line_size,1,fp_data) != 1) {
                fprintf(stderr, "Error reading from '%s' in %s:%d\n",
                        new_filename, __FILE__, __LINE__);
                exit(2);
            }
            for(uint64_t p=0; p < NUM_POL; p++) {
                sum[p] += line[o+p];
                sumsqr[p] += line[o+p]*line[o+p];
            }
        }
        Real64_t mean[NUM_POL];
        Real64_t stddev[NUM_POL];
        for(uint64_t p=0; p < NUM_POL; p++) {
            mean[p] = sum[p] / num_integrations;
            stddev[p] = (sumsqr[p] - sum[p]*sum[p]/num_integrations)/(num_integrations-1);
            if(stddev[p] > 0.0) {
                stddev[p] = sqrt(stddev[p]);
            }
            else {
                stddev[p] = 0.0;
            }
            printf(" %E %.2E", mean[p], stddev[p]);
        }
        fputc('\n', stdout);
    }

    free(subbands);
    fclose(fp_data);
    return 0;
}


    
    
