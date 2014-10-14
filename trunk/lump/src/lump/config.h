// config.h
// config stuff
//_HIST  DATE NAME PLACE INFO
// 2012 Apr 09  James M Anderson  --MPIfR  start
// 2013 Aug 03  JMA  ---- changing to new version date
// 2014 Aug 29  JMA  ---- update program date
// 2014 Sep 09  JMA  ---- update version string and number
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





#ifndef CONFIG_H
#define CONFIG_H

#define MPIfR_LOFAR_LuMP_VERSION_DATE_STRING "2014Oct14"
const unsigned int MPIfR_LOFAR_STATION_SOFTWARE_VERSION = 0x0230;


// _REENTRANT not needed for current linux systems
// #ifndef _REENTRANT
// #  define _REENTRANT        // to get threads-safe versions of standard libraries
// #endif
#include <pthread.h>

#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
#endif



#define MPIfR_LOFAR_LuMP_ROOT "LUMP_ROOT"
#define MPIfR_LOFAR_LIBDATA_DIRECTORY "libdata/"
#define MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY ".LuMP/"
#define MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY "LuMP/"

// This is to aid with x86* special instructions (SSE, SSE2, ...)
#define MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT 16





#endif // CONFIG_H
