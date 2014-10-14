//LOFAR_Station_Beamformed_LuMP1_Expander.h
// writes out multiple subband LuMP pulsar format data
//_HIST  DATE NAME PLACE INFO
// 2013 Feb 24  James M Anderson  --- MPIfR  start from test_ascii_dump_raw0.cxx
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp

// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_LuMP1_Expander_H
#define LOFAR_Station_Beamformed_LuMP1_Expander_H

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
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include <stdio.h>
#include <stdlib.h>








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#undef NUM
#endif // LOFAR_Station_Beamformed_LuMP1_Expander
