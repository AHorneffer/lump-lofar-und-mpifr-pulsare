// test_ascii_dump_PFB0.cxx
// test code to convert a beamlet to ASCII for PFB0 format
//_HIST  DATE NAME PLACE INFO
// 2014 Jul 27  James M Anderson  --- start from test_ascii_dump_power0.cxx
// 2014 Aug 29  JMA  ---- update for also doing FFT0, output valid info
// 2014 Sep 05  JMA  ---- add more output types allowed
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
#include "LOFAR_Station_Beamformed_Valid_Real32.h"


namespace MPIfR {
namespace LOGGING {
extern const char PROGRAM_NAME[] = "test_ascii_dump_PFB0";
extern const char PROGRAM_VERSION[] = "2014Sep21";
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
};};




// set up a namespace area for stuff.
namespace {




// GLOBALS


// FUNCTIONS



}  // end namespace






template<typename T> int process(const int argc, const char* const argv[])
{
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(argv[1], "rb");
    const uint_fast16_t beamlet(strtoul(argv[2],NULL,0));




    if((info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_PFB0_OUT))
      && (info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_FFT0_OUT))
      && (info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_PFB1_OUT))
      && (info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_FFT1_OUT))
      && (info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_FFT2_OUT))
       )
    {
        fprintf(stderr, "Error: wrong writer type '%u' in %s:%d\n",
                unsigned(info.get_WRITER_TYPE()), __FILE__, __LINE__);
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
    const size_t NUM_BEAMLETS = info.get_beamlets_per_sample();
    const size_t NUM_CHANNELS = info.get_NUM_CHANNELS();
    const size_t NUM_R = 2*2*NUM_CHANNELS*NUM_BEAMLETS;
    const size_t sample_line_size = sizeof(T)*NUM_R;
    T line[NUM_R];
    const size_t o = 2*2*NUM_CHANNELS*o_index;
    const size_t FSIZE=2048;
    char new_filename[FSIZE];
    char valid_filename[FSIZE];
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

    
    retval = snprintf(valid_filename, FSIZE, "%s.valid", argv[1]);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        fprintf(stderr, "Error creating valid file filename from base '%s'\n", argv[1]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Valid_Real32_Reader valid(valid_filename, num_integrations, 0);
    for(uint64_t sample=0; sample < num_integrations; sample++) {
        Real64_t time_offset = sample*seconds_per_output;
        if(fread(line,sample_line_size,1,fp_data) != 1) {
            fprintf(stderr, "Error reading from '%s' in %s:%d\n",
                    new_filename, __FILE__, __LINE__);
            exit(2);
        }
        Real32_t v;
        if(valid.read_N_points(sample, 1, &v)) {
            fprintf(stderr, "Error reading from valid file '%s' in %s:%d\n",
                    valid_filename, __FILE__, __LINE__);
            exit(2);
        }
        for(size_t c=0; c < NUM_CHANNELS; c++) {
            printf("%.14E %.14E  %4d  %+14.7E %+14.7E     %+14.7E %+14.7E    %+14.7E\n",
                   time_offset, time_since_1970, int(c),
                   Real64_t(line[o+2*c]), Real64_t(line[o+2*c+1]), Real64_t(line[o+2*c+2*NUM_CHANNELS]), Real64_t(line[o+2*c+2*NUM_CHANNELS+1]),
                   v);
        }
        // size_t pos = 0;
        // for(size_t b=0; b < NUM_BEAMLETS*2; b++) {
        //     for(size_t c=0; c < NUM_CHANNELS; c++) {
        //         printf("%.14E %.14E  %4d  %4d  %+14.7E %+14.7E\n",
        //                time_offset, time_since_1970, int(b), int(c),
        //                line[pos], line[pos+1]);
        //         pos += 2;
        //     }
        // }
    }
    fclose(fp_data);
    return 0;
}


    
    
int main(const int argc, const char* const argv[])
{
    if(argc != 3) {
        fprintf(stderr, "Error: correct usage is %s filename_base beamlet_nr\n", argv[0]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(argv[1], "rb");


    int32_t output_type = info.get_output_DATA_TYPE();
    switch (output_type) {
    case int32_t(MPIfR::LOFAR::L_Real16_t):
        process<MPIfR::DATA_TYPE::Real16_t>(argc, argv); break;
    case int32_t(MPIfR::LOFAR::L_Real32_t):
        process<Real32_t>(argc, argv); break;
    case int32_t(MPIfR::LOFAR::L_Real64_t):
        process<Real64_t>(argc, argv); break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    case int32_t(MPIfR::LOFAR::L_Real80_t):
        process<Real80_t>(argc, argv); break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    case int32_t(MPIfR::LOFAR::L_Real128_t):
        process<Real128_t>(argc, argv); break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    default:
        fprintf(stderr, "Error: bad output data type '%d' in %s:%d\n",
                int(info.get_output_DATA_TYPE()), __FILE__, __LINE__);
        exit(2);
    }        
    return 0;
}
