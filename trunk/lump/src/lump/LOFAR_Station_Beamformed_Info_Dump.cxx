// LOFAR_Station_Beamformed_Info_Dump.cxx
// program to dump out INFO file as ASCII text
//_HIST  DATE NAME PLACE INFO
// 2011 Sep 11  James M Anderson  --MPIfR  start



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
#include "LOFAR_Station_Beamformed_Info.h"
#include <stdio.h>
#include <stdlib.h>
#include "MPIfR_logging.h"


namespace MPIfR { namespace LOGGING {
extern const char PROGRAM_NAME[] = "LOFAR_Station_Beamformed_Info_Dump";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
}}

// set up a namespace area for stuff.
namespace {




// GLOBALS


// FUNCTIONS



}  // end namespace





int main(int argc, char* argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Error: correct usage is %s filename_base\n", argv[0]);
        exit(2);
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(argv[1], "rb");
    info.print_as_text(stdout, NULL);
    return 0;
}
    
