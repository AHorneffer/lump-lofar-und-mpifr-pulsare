// LOFAR_Station_Common.cxx
// common external variable declarations
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 16  James M Anderson  --MPIfR  Start
// 2013 Jul 07  JMA  ---- add LOFAR_RCUMODE_INPUT_CABLE_NAME
// 2013 Aug 09  JMA  ---- add fielname copying function




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
#define LOFAR_Station_Common_H_FILE 1
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
#include "LOFAR_Station_Common.h"
#include <string.h>

// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



// GLOBALS


// FUNCTIONS
const char* const restrict LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(LOFAR_DATA_OUTPUT_FILE_FORMAT e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_DATA_OUTPUT_FILE_FORMAT_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_DATA_OUTPUT_FILE_FORMAT";
    }
    return NULL;
}

const char* const restrict LOFAR_STATION_TYPE_ENUM_Str(LOFAR_STATION_TYPE_ENUM e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_STATION_TYPE_ENUM_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_STATION_TYPE_ENUM";
    }
    return NULL;
}

const char* const restrict LOFAR_ANTENNA_SET_Str(const char* const restrict name,
                                                 const int16_t RCUMODE
                                                 ) throw()
{
    static const char s0[] = "Unkown";
    static const char s1[] = "LBA_INNER";
    static const char s2[] = "LBA_OUTER";
    static const char s3[] = "HBA_DUAL";
//     static const char s4[] = "";
//     static const char s5[] = "";
//     static const char s6[] = "";
//     static const char s7[] = "";
//     static const char s8[] = "";
//     static const char s9[] = "";
    static const char se[] = "error";
    LOFAR_STATION_TYPE_ENUM station = LOFAR_Station_Type_From_Name(name);
    switch(station) {
    case LOFAR_STATION_TYPE_Core:
    case LOFAR_STATION_TYPE_Remote:
    case LOFAR_STATION_TYPE_International:
        switch(RCUMODE) {
        case 1:
        case 2:
            return s1;
        case 3:
        case 4:
            return s2;
        case 5:
        case 6:
        case 7:
            return s3;
        case 8:
            return s2;
        case 9:
            return s1;
        default:
            return s0;
        }
    default:
        return s0;
    }
    return se;
}


const char* const LOFAR_RCUMODE_INPUT_CABLE_NAME(const uint_fast32_t RCUMODE)
    throw()
{
    static const char s0[] = "Unkown";
    static const char s1[] = "LBL";
    static const char s2[] = "LBH";
    static const char s3[] = "HBA";
    static const char se[] = "error";
    switch(RCUMODE) {
    case 1:
    case 2:
        return s1;
    case 3:
    case 4:
        return s2;
    case 5:
    case 6:
    case 7:
        return s3;
    case 8:
        return s2;
    case 9:
        return s1;
    default:
        return s0;
    }
    return se;
}





LOFAR_STATION_TYPE_ENUM LOFAR_Station_Type_From_Name(const char* const restrict name) throw()
{
    if(strlen(name) != 7) {
        return LOFAR_STATION_TYPE_UNKNOWN;
    }
    else if(  (strcmp(name,"EfDE601")==0)
           || (strcmp(name,"UwDE602")==0)
           || (strcmp(name,"TbDE603")==0)
           || (strcmp(name,"PoDE604")==0)
           || (strcmp(name,"JuDE605")==0)
           || (strcmp(name,"NcFR606")==0)
           || (strcmp(name,"OnSE607")==0)
           || (strcmp(name,"CbUK608")==0)
           || (strcmp(name,"KjFI609")==0)
           || (strcmp(name,"NdDE609")==0)
            ) {
        return LOFAR_STATION_TYPE_International;
    }
    else if(strncmp(name,"  RS",4)==0) {
        return LOFAR_STATION_TYPE_Remote;
    }
    else if(strncmp(name,"  CS",4)==0) {
        return LOFAR_STATION_TYPE_Core;
    }
    return LOFAR_STATION_TYPE_UNKNOWN;
}

const char* const restrict LOFAR_FILTER_SET_Str(const int16_t RCUMODE) throw()
{
    static const char s0[] = "Unkown";
    static const char s1[] = "LBA_10_70";
    static const char s2[] = "LBA_30_70";
    static const char s3[] = "LBA_10_90";
    static const char s4[] = "LBA_30_90";
    static const char s5[] = "HBA_110_190";
    static const char s6[] = "HBA_170_230";
    static const char s7[] = "HBA_210_250";
    static const char s8[] = "LBA_10_90";
    static const char s9[] = "LBA_10_70";
    static const char se[] = "error";
    switch (RCUMODE) {
    case 1:
        return s1;
    case 2:
        return s2;
    case 3:
        return s3;
    case 4:
        return s4;
    case 5:
        return s5;
    case 6:
        return s6;
    case 7:
        return s7;
    case 8:
        return s8;
    case 9:
        return s9;
    default:
        return s0;
    }
    return se;
}

// Note that the caller is responsible for free'ing the returned pointer
char* copy_filename_without_extension(const char* const filename_orig,
                                      size_t extra_malloc_chars) throw()
{
    if(filename_orig == NULL) {
        return NULL;
    }
    size_t len = strlen(filename_orig) +1;
    char* new_filename = reinterpret_cast<char*>(malloc(len+extra_malloc_chars));
    if(new_filename == NULL) {
        return NULL;
    }
    strcpy(new_filename, filename_orig);
    // now cut off any extension
    char* cp = new_filename + len;
    bool dot_found = false;
    do {
        cp--;
        if(*cp == '.') {
            dot_found = true;
            break;
        }
        else if(*cp == '/') {
            break;
        }
    } while (cp > new_filename);
    if(dot_found) {
        *cp = 0;
    }
    return new_filename;
}

}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


