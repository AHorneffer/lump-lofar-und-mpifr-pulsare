// MPIfR_Sexagesimal_Angle.cxx
// Handle sexagesimal coordinate stuff
//_HIST  DATE NAME PLACE INFO
// 2013 Jul 08  James M Anderson  --- MPIfR start


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






// set up a namespace area for stuff.
namespace MPIfR {

namespace Angles {




   





bool check_Sexagesimal_Type_Enum_valid(const Sexagesimal_Type_Enum t) throw()
{
    int count_units = 0;
    if((t & SEXAGESIMAL_TYPE_DEGREES))      count_units++;
    if((t & SEXAGESIMAL_TYPE_HOURS))        count_units++;
    if((t & SEXAGESIMAL_TYPE_RADIANS))      count_units++;
    if((t & SEXAGESIMAL_TYPE_MINUTES))      count_units++;
    if((t & SEXAGESIMAL_TYPE_SECONDS))      count_units++;
    if((t & SEXAGESIMAL_TYPE_MILLISECONDS)) count_units++;
    if((t & SEXAGESIMAL_TYPE_MICROSECONDS)) count_units++;
    if(count_units != 1) {
        DEF_LOGGING_DEBUG("found %d units", count_units);
        return false;
    }
    int count_digits = 0;
    if((t & SEXAGESIMAL_TYPE_ONE_DIGIT))   count_digits++;
    if((t & SEXAGESIMAL_TYPE_THREE_DIGIT)) count_digits++;
    if(count_digits > 1) {
        DEF_LOGGING_DEBUG("found %d digits", count_digits);
        return false;
    }
    int count_depth = 0;
    if((t & SEXAGESIMAL_TYPE_DECIMAL))      count_depth++;
    if((t & SEXAGESIMAL_TYPE_EXPONENTIAL))  count_depth++;
    if((t & SEXAGESIMAL_TYPE_MINUTES_STOP)) count_depth++;
    if(count_depth > 1) {
        DEF_LOGGING_DEBUG("found %d depth", count_depth);
        return false;
    }
    if( ((t & SEXAGESIMAL_TYPE_RADIANS))
      || ((t & SEXAGESIMAL_TYPE_MINUTES))
      || ((t & SEXAGESIMAL_TYPE_SECONDS))
      || ((t & SEXAGESIMAL_TYPE_MILLISECONDS))
      || ((t & SEXAGESIMAL_TYPE_MICROSECONDS)) ) {
        if( ((t & SEXAGESIMAL_TYPE_DECIMAL))
          || ((t & SEXAGESIMAL_TYPE_EXPONENTIAL)) ) {
            // ok, only decimal and exponential are valid types for these units
        }
        else {
            DEF_LOGGING_DEBUG("bad decimal/exponential");
            return false;
        }
    }
    if( ((t & SEXAGESIMAL_TYPE_HOURS))
      && ((t & SEXAGESIMAL_TYPE_THREE_DIGIT)) ) {
        DEF_LOGGING_DEBUG("bad hours");
        return false;
    }
    if( ((t & SEXAGESIMAL_TYPE_EXPONENTIAL))
      && ( !((t & SEXAGESIMAL_TYPE_ONE_DIGIT))
         || ((t & SEXAGESIMAL_TYPE_THREE_DIGIT))
         || ((t & SEXAGESIMAL_TYPE_TRUNCATE)) ) ) {
        DEF_LOGGING_DEBUG("bad exponential");
        return false;
    }
    return true;
}

    
    
    

                      

}  // end namespace Angles
}  // end namespace MPIfR


