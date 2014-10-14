// test_MPIfR_Angles.cxx
// test MPIfR_Angle conversions
//_HIST  DATE NAME PLACE INFO
// 2013 Jul 13  James M Anderson  --- MPIfR start


// Copyright (c) 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
#include "MPIfR_Sexagesimal_Angle.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "MPIfR_logging.h"
#include "LOFAR_Station_Common.h"


namespace MPIfR {namespace LOGGING {
extern const char PROGRAM_NAME[] = "LOFAR_Station_Beamformed_Recorder";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
}}




using namespace MPIfR::Angles;

int main(void) {
    const size_t SIZE = 128;
    char str[SIZE];

    int_fast32_t retval;
    int compare;
    int num_failures = 0;

    printf("**********\nTesting string output conversion\n");
    Sexagesimal_Type_Enum t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_RADIANS | SEXAGESIMAL_TYPE_DECIMAL | SEXAGESIMAL_TYPE_ONE_DIGIT);
    Sexagesimal_Angle<Real64_t> a0(0.125);
    printf("a0(0.125) is %.10F\n", a0.radians());

    str[0] = 0;
    retval = a0.to_sexagesimal_string(t, 0, 0, 0, 3, SIZE, str);
    compare = strcmp(str, "+0.125");
    printf("test0 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    str[0] = 0;
    retval = a0.to_sexagesimal_string(t, 0, 0, 0, 2, SIZE, str);
    compare = strcmp(str, "+0.12");
    printf("test0 print1 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    
    Sexagesimal_Angle<Real64_t> a1(0.375);
    printf("a1(0.375) is %.10F\n", a1.radians());

    str[0] = 0;
    retval = a1.to_sexagesimal_string(t, 0, 0, 0, 3, SIZE, str);
    compare = strcmp(str, "+0.375");
    printf("test1 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    str[0] = 0;
    retval = a1.to_sexagesimal_string(t, 0, 0, 0, 2, SIZE, str);
    compare = strcmp(str, "+0.38");
    printf("test1 print1 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;


    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_MICROSECONDS);
    a1= Sexagesimal_Angle<Real64_t>(0.1,t);
    printf("a1(0.1,t) is %.10F\n", a1.radians());

    str[0] = 0;
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES|SEXAGESIMAL_TYPE_NO_PLUS);
    retval = a1.to_sexagesimal_string(t, ':', ':', 0, 7, SIZE, str);
    compare = strcmp(str, "00:00:00.0000001");
    printf("test2 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_MILLISECONDS|SEXAGESIMAL_TYPE_DECIMAL|SEXAGESIMAL_TYPE_ONE_DIGIT);
    str[0] = 0;
    retval = a1.to_sexagesimal_string(t, 0, 0, 0, 6, SIZE, str);
    compare = strcmp(str, "+0.000100");
    printf("test2 print1 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;


    printf("**********\nTesting from original string conversion\n");
    const char a2_s[] = "-09d05m42.567894s";
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
    Sexagesimal_Angle<Real64_t> a2(a2_s, t);
    printf("a2('%s',t) is %.10F\n", a2_s, a2.radians());

    str[0] = 0;
    retval = a2.to_sexagesimal_string(t, 'd', 'm', 's', 6, SIZE, str);
    compare = strcmp(str, a2_s);
    printf("test0 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;



    const char a3_s[] = "23:20:00.00";
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_HOURS);
    Sexagesimal_Angle<Real64_t> a3(a3_s, t);
    printf("a3('%s',t) is %.10F\n", a3_s, a3.radians());

    str[0] = 0;
    retval = a3.to_sexagesimal_string(t, ':', ':', 0, 2, SIZE, str);
    compare = strcmp(str, a3_s);
    printf("test1 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
    str[0] = 0;
    retval = a3.to_sexagesimal_string(t, ':', ':', 0, 0, SIZE, str);
    compare = strcmp(str, "+350:00:00");
    printf("test1 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;




    const char a4_s[] = "-1:15:00";
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES|SEXAGESIMAL_TYPE_ONE_DIGIT);
    Sexagesimal_Angle<Real64_t> a4(a4_s, t);
    printf("a4('%s',t) is %.10F\n", a4_s, a4.radians());

    str[0] = 0;
    retval = a4.to_sexagesimal_string(t, ':', ':', 0, 0, SIZE, str);
    compare = strcmp(str, a4_s);
    printf("test1 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES|SEXAGESIMAL_TYPE_THREE_DIGIT);
    str[0] = 0;
    retval = a4.to_sexagesimal_string(t, ':', ':', 0, 0, SIZE, str);
    compare = strcmp(str, "-001:15:00");
    printf("test1 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES|SEXAGESIMAL_TYPE_THREE_DIGIT|SEXAGESIMAL_TYPE_NO_ZERO_PAD);
    str[0] = 0;
    retval = a4.to_sexagesimal_string(t, ':', ':', 0, 0, SIZE, str);
    compare = strcmp(str, "-  1:15: 0");
    printf("test2 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;

    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES|SEXAGESIMAL_TYPE_DECIMAL);
    str[0] = 0;
    retval = a4.to_sexagesimal_string(t, 'd', 0, 0, 5, SIZE, str);
    compare = strcmp(str, "-01.25000d");
    printf("test3 print0 retval %d compare %d result '%s' \n", int(retval), compare, str);
    if(compare != 0) num_failures++;


    printf("**********\nTesting good string conversion\n");
    const char* endptr = NULL;
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
    strcpy(str, "-09d 5m42.567894s");
    compare = 0;
    errno = 0;
    get_Sexagesimal_Angle_from_string<Real64_t>(str, &endptr, t);
    if(endptr == str) {
        compare = 1;
        num_failures++;
    }
    printf("good string test 0 errno %d compare %d from string '%s'\n", errno, compare, str);
    
    


    printf("**********\nTesting bad string conversion\n");
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
    strcpy(str, "-09d005m42.567894s");
    compare = 0;
    errno = 0;
    get_Sexagesimal_Angle_from_string<Real64_t>(str, &endptr, t);
    if(endptr != str) {
        compare = 1;
        num_failures++;
    }
    printf("bad string test 0 errno %d compare %d from string '%s'\n", errno, compare, str);
    
    
    t = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
    strcpy(str, "-09d5m42.567894s");
    compare = 0;
    errno = 0;
    get_Sexagesimal_Angle_from_string<Real64_t>(str, &endptr, t);
    if(endptr != str) {
        compare = 1;
        num_failures++;
    }
    printf("bad string test 1 errno %d compare %d from string '%s'\n", errno, compare, str);
    
    


    printf("**********\nFound %d failures\n", num_failures);
    if((num_failures)) {
        return 1;
    }
    return 0;
}
