// MPIfR_Sexagesimal_Angle.h
// Handle sexagesimal coordinate stuff
//_HIST  DATE NAME PLACE INFO
// 2013 Jul 08  James M Anderson  --- MPIfR start
// 2014 Sep 17  JMA  --- fix formatting bugs in debug printing

// Copyright (c) 2013, 2014 James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef MPIfR_Sexagesimal_Angle_H
#define MPIfR_Sexagesimal_Angle_H

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
#include <ctype.h>
#include <errno.h>
#include "MPIfR_logging.h"




// set up a namespace area for stuff.
namespace MPIfR {

namespace Angles {


enum Sexagesimal_Type_Enum {
    SEXAGESIMAL_TYPE_DEGREES      = 0x0001,
    SEXAGESIMAL_TYPE_HOURS        = 0x0002,
    SEXAGESIMAL_TYPE_RADIANS      = 0x0004,
    SEXAGESIMAL_TYPE_MINUTES      = 0x0008,
    SEXAGESIMAL_TYPE_SECONDS      = 0x0010,
    SEXAGESIMAL_TYPE_MILLISECONDS = 0x0020,
    SEXAGESIMAL_TYPE_MICROSECONDS = 0x0040,
    SEXAGESIMAL_TYPE_NO_PLUS      = 0x0080,
    SEXAGESIMAL_TYPE_ONE_DIGIT    = 0x0100,
    SEXAGESIMAL_TYPE_THREE_DIGIT  = 0x0200,
    SEXAGESIMAL_TYPE_DECIMAL      = 0x0400,
    SEXAGESIMAL_TYPE_EXPONENTIAL  = 0x0800,
    SEXAGESIMAL_TYPE_MINUTES_STOP = 0x1000,  // degrees and minutes, no seconds
    SEXAGESIMAL_TYPE_TRUNCATE     = 0x2000,  // always round toward zero
    SEXAGESIMAL_TYPE_NO_ZERO_PAD  = 0x4000,
    //
    // Special Bitmasks
    SEXAGESIMAL_TYPE_UNITS_BITMASK= 0x007F
};
        





//_CLASS  Sexagesimal_Angle
template<class T> class Sexagesimal_Angle {
//_DESC  full description of class

//_FILE  files used and logical units used

//_LIMS  design limitations

//_BUGS  known bugs

//_CALL  list of calls

//_KEYS  

//_HIST  DATE NAME PLACE INFO

//_END


// NAMESPACE ISSUES    


public:
    Sexagesimal_Angle() {return;};
    Sexagesimal_Angle(T angle_) : angle(angle_) {return;}; // assume radians
    Sexagesimal_Angle(T angle_, Sexagesimal_Type_Enum t) throw();
    Sexagesimal_Angle(const char* str, Sexagesimal_Type_Enum t) throw();
    

    int_fast32_t to_sexagesimal_string(Sexagesimal_Type_Enum t,
                                       const char separator_0,
                                       const char separator_1,
                                       const char separator_2,
                                       const int_fast32_t precision,
                                       const size_t STR_SIZE,
                                       char* const restrict output_str)
        const throw();

    T radians() const throw() {return angle;}

    Sexagesimal_Angle<T>& operator=(T a) throw() {angle=a; return *this;}

protected:


private:
    T angle;  // in radians

};

// HELPER functions

bool check_Sexagesimal_Type_Enum_valid(const Sexagesimal_Type_Enum t) throw();



template<class T> T get_M_RAD2DEG() throw()
{
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
    return T(M_RAD2DEGq);
#endif
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    return T(M_RAD2DEGl);
#endif
    return T(M_RAD2DEG);
}
template<> inline Real32_t get_M_RAD2DEG<Real32_t>() throw()
{
    return M_RAD2DEGf;
}
template<> inline Real64_t get_M_RAD2DEG<Real64_t>() throw()
{
    return M_RAD2DEG;
}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
template<> inline Real80_t get_M_RAD2DEG<Real80_t>() throw()
{
    return M_RAD2DEGl;
}
#endif
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
template<> inline Real128_t get_M_RAD2DEG<Real128_t>() throw()
{
    return M_RAD2DEGq;
}
#endif

template<class T> char get_snprintf_length_modifier() throw()
{
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
    return 'Q';
#endif
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    return 'L';
#endif
    return 0;
}
template<> inline char get_snprintf_length_modifier<Real32_t>() throw()
{
    return 0;
}
template<> inline char get_snprintf_length_modifier<Real64_t>() throw()
{
    return 0;
}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
template<> inline char get_snprintf_length_modifier<Real80_t>() throw()
{
    return 'L';
}
#endif
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
template<> inline char get_snprintf_length_modifier<Real128_t>() throw()
{
    return 'Q';
}
#endif



template<class T> int Sexagesimal_Angle_snprintf(char* s, size_t size, const char* format,
                                     int width, int precision, T value) throw()
{
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
    return quadmath_snprintf(s, size, format, width, precision, Real128_t(value));
#endif
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    return snprintf(s, size, format, width, precision, Real80_t(value));
#endif
    return snprintf(s, size, format, width, precision, Real64_t(value));
}
template<> inline int Sexagesimal_Angle_snprintf<Real32_t>(char* s, size_t size, const char* format,
                                               int width, int precision, Real32_t value) throw()
{
    return snprintf(s, size, format, width, precision, value);
}
template<> inline int Sexagesimal_Angle_snprintf<Real64_t>(char* s, size_t size, const char* format,
                                               int width, int precision, Real64_t value) throw()
{
    return snprintf(s, size, format, width, precision, value);
}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
template<> inline int Sexagesimal_Angle_snprintf<Real80_t>(char* s, size_t size, const char* format,
                                               int width, int precision, Real80_t value) throw()
{
    return snprintf(s, size, format, width, precision, value);
}
#endif
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
template<> inline int Sexagesimal_Angle_snprintf<Real128_t>(char* s, size_t size, const char* format,
                                                int width, int precision, Real128_t value) throw()
{
    return quadmath_snprintf(s, size, format, width, precision, value);
}
#endif


template<class T> T Sexagesimal_Angle_strtod(const char* nptr, char** endptr) throw()
{
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
    return T(strtoflt128(nptr, endptr));
#endif
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    return T(strtold(nptr, endptr));
#endif
    return T(strtod(nptr, endptr));
}
template<> inline Real32_t Sexagesimal_Angle_strtod<Real32_t>(const char* nptr, char** endptr) throw()
{
    return strtof(nptr, endptr);
}
template<> inline Real64_t Sexagesimal_Angle_strtod<Real64_t>(const char* nptr, char** endptr) throw()
{
    return strtod(nptr, endptr);
}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
template<> inline Real80_t Sexagesimal_Angle_strtod<Real80_t>(const char* nptr, char** endptr) throw()
{
    return strtold(nptr, endptr);
}
#endif
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
template<> inline Real128_t Sexagesimal_Angle_strtod<Real128_t>(const char* nptr, char** endptr) throw()
{
    return strtoflt128(nptr, endptr);
}
#endif





template<class T> Sexagesimal_Angle<T> get_Sexagesimal_Angle_from_string(const char* const s,
                                                 const char** endptr,
                                                 const Sexagesimal_Type_Enum t) throw()
{
    //DEF_LOGGING_DEBUG("converting string '%s' type 0x%X", s, int(t));
    if(endptr != NULL) {
        *endptr = s;
    }
    Sexagesimal_Angle<T> default_Sexagesimal_Angle(0.0f);
    if(!check_Sexagesimal_Type_Enum_valid(t)) {
        DEF_LOGGING_ERROR("invalid Sexagesimal_Type_Enum 0x%X", (unsigned int)(t));
        errno = -1;
        return default_Sexagesimal_Angle;
    }
    const char* cp = s;
    while(isspace(*cp)) {
        cp++;
    }
    char sign = '+';
    if(*cp == '+') {
        cp++;
    }
    else if(*cp == '-') {
        sign = '-';
        cp++;
    }
    while(isspace(*cp)) {
        cp++;
    }
    if(!isdigit(*cp)) {
        DEF_LOGGING_ERROR("bad number format");
        errno = -2;
        return default_Sexagesimal_Angle;
    }
    if(  ((t & SEXAGESIMAL_TYPE_DECIMAL))
      || ((t & SEXAGESIMAL_TYPE_EXPONENTIAL)) ) {
        char* new_endptr = NULL;
        errno = 0;
        T val = Sexagesimal_Angle_strtod<T>(cp, &new_endptr);
        if(new_endptr == cp) {
            DEF_LOGGING_PERROR("Sexagesimal_Angle_strtod conversion error");
            DEF_LOGGING_ERROR("could not convert string to floating point");
            errno = -3;
            return default_Sexagesimal_Angle;
        }
        if(sign == '-') {
            val = -val;
        }
        if((t & SEXAGESIMAL_TYPE_RADIANS)) {
            // do nothing
        }
        else {
            // degree-based units
            val /= get_M_RAD2DEG<T>();
            switch(t & SEXAGESIMAL_TYPE_UNITS_BITMASK) {
            case SEXAGESIMAL_TYPE_HOURS:
                val *= 15.0;
                break;
            case SEXAGESIMAL_TYPE_DEGREES:
                break;
            case SEXAGESIMAL_TYPE_MINUTES:
                val /= 60.0;
                break;
            case SEXAGESIMAL_TYPE_SECONDS:
                val /= 3600.0;
                break;
            case SEXAGESIMAL_TYPE_MILLISECONDS:
                val /= 3600.0 * 1000.0;
                break;
            case SEXAGESIMAL_TYPE_MICROSECONDS:
                val /= 3600.0 * 1000000.0;
                break;
            default:
                DEF_LOGGING_ERROR("invalid Sexagesimal_Type_Enum");
                errno = -1;
                return default_Sexagesimal_Angle;
            }
        }
        // is there a units character after this?
        if( ((*new_endptr)) && !isspace(*new_endptr) ) {
            new_endptr++;
        }
        *endptr = new_endptr;
        return Sexagesimal_Angle<T>(val);
    }
    else {
        // sexagesimal number.  Get the degrees portion
        char* new_endptr = NULL;
        errno = 0;
        long degrees = strtol(cp, &new_endptr, 10);
        if(new_endptr == cp) {
            DEF_LOGGING_PERROR("Sexagesimal_Angle_strtod conversion error");
            DEF_LOGGING_ERROR("could not convert string to long");
            errno = -4;
            return default_Sexagesimal_Angle;
        }
        //DEF_LOGGING_DEBUG("degrees portion %ld", degrees);
        if((*new_endptr)) {
            new_endptr++;
        }
        cp = new_endptr;
        // get the minutes portion
        // Note that this must be two characters long.
        if( (cp[0] == 0) || (cp[1] == 0)
          || !(isdigit(cp[0]) || isspace(cp[0]))
          || !isdigit(cp[1]) || isdigit(cp[2])
          || (isdigit(cp[0]) && ( (cp[0] == '7') || (cp[0] == '8') || (cp[0] == '9') ) ) ) {
            DEF_LOGGING_ERROR("bad minutes format");
            errno = -5;
            return default_Sexagesimal_Angle;
        }
        errno = 0;
        long minutes = strtol(cp, &new_endptr, 10);
        if(new_endptr == cp) {
            DEF_LOGGING_PERROR("Sexagesimal_Angle_strtod conversion error");
            DEF_LOGGING_ERROR("could not convert string to long");
            errno = -6;
            return default_Sexagesimal_Angle;
        }
        //DEF_LOGGING_DEBUG("minutes portion %ld", minutes);
        if((t & SEXAGESIMAL_TYPE_MINUTES_STOP)) {
            // only minutes to be read.  Was there a minutes unit?
            if( ((*new_endptr)) && !isspace(*new_endptr) ) {
                new_endptr++;
            }
            *endptr = new_endptr;
            T val = T(degrees) + minutes / T(60.0);
            if((t & SEXAGESIMAL_TYPE_HOURS)) {
                val *= 15.0;
            }
            val /= get_M_RAD2DEG<T>();
            if(sign == '-') {
                val = -val;
            }
            return Sexagesimal_Angle<T>(val);
        }
        else {
            // Need seconds portion too.
            if((*new_endptr)) {
                new_endptr++;
            }
            cp = new_endptr;
            // get the seconds portion
            // Note that the integer portion must be two characters long.
            if( (cp[0] == 0) || (cp[1] == 0)
              || !(isdigit(cp[0]) || isspace(cp[0]))
              || !isdigit(cp[1]) || isdigit(cp[2])
              || (isdigit(cp[0]) && ( (cp[0] == '7') || (cp[0] == '8') || (cp[0] == '9') ) ) ) {
                DEF_LOGGING_ERROR("bad seconds format");
                errno = -7;
                return default_Sexagesimal_Angle;
            }
            errno = 0;
            T seconds = Sexagesimal_Angle_strtod<T>(cp, &new_endptr);
            if(new_endptr == cp) {
                DEF_LOGGING_PERROR("Sexagesimal_Angle_strtod conversion error");
                DEF_LOGGING_ERROR("could not convert string to floating point");
                errno = -8;
                return default_Sexagesimal_Angle;
            }
            //DEF_LOGGING_DEBUG("seconds portion %F", Real64_t(seconds));
            // Seconds read.  Was there a seconds unit?
            if( ((*new_endptr)) && !isspace(*new_endptr) ) {
                new_endptr++;
            }
            *endptr = new_endptr;
            T val = T(degrees) + T(minutes) / T(60.0) + seconds / T(3600.0);
            if((t & SEXAGESIMAL_TYPE_HOURS)) {
                val *= 15.0;
            }
            val /= get_M_RAD2DEG<T>();
            if(sign == '-') {
                val = -val;
            }
            return Sexagesimal_Angle<T>(val);
        }
    }
    DEF_LOGGING_ERROR("programmer error");
    errno = -9;
    return default_Sexagesimal_Angle;
}


template<class T> int
get_RA_Dec_from_Sexagesimal_Angles(const char* str,
                                   Sexagesimal_Type_Enum t_RA,
                                   Sexagesimal_Type_Enum t_Dec,
                                   Sexagesimal_Angle<T>* RA,
                                   Sexagesimal_Angle<T>* Dec) throw()
{
    const char* endptr = NULL;
    errno = 0;
    Sexagesimal_Angle<T> temp = get_Sexagesimal_Angle_from_string<T>(str, &endptr, t_RA);
    if(endptr == str) {
        DEF_LOGGING_ERROR("unable to convert RA from string '%s' to Sexagesimal_Angle with type 0x%X", str, int(t_RA));
        return errno;
    }
    *RA = temp;
    const char* str_Dec = endptr;
    errno = 0;
    temp = get_Sexagesimal_Angle_from_string<T>(str_Dec, &endptr, t_Dec);
    if(endptr == str) {
        DEF_LOGGING_ERROR("unable to convert Dec from string '%s' to Sexagesimal_Angle with RA type 0x%X, Dec type 0x%X", str, int(t_RA), int(t_Dec));
        return errno;
    }
    *Dec = temp;
    return 0;
}





// CLASS FUNCTIONS

template<class T>
Sexagesimal_Angle<T>::Sexagesimal_Angle(T angle_, Sexagesimal_Type_Enum t) throw()
  : angle(angle_)
{
    if((t & SEXAGESIMAL_TYPE_RADIANS)) {
        // do nothing
    }
    else {
        // degree-based
        angle /= get_M_RAD2DEG<T>();
        switch(t & SEXAGESIMAL_TYPE_UNITS_BITMASK) {
        case SEXAGESIMAL_TYPE_RADIANS:
            break;
        case SEXAGESIMAL_TYPE_HOURS:
            angle *= 15.0;
            break;
        case SEXAGESIMAL_TYPE_DEGREES:
            break;
        case SEXAGESIMAL_TYPE_MINUTES:
            angle /= 60.0;
            break;
        case SEXAGESIMAL_TYPE_SECONDS:
            angle /= 3600.0;
            break;
        case SEXAGESIMAL_TYPE_MILLISECONDS:
            angle /= 3600.0 * 1000.0;
            break;
        case SEXAGESIMAL_TYPE_MICROSECONDS:
            angle /= 3600.0 * 1000000.0;
            break;
        default:
            DEF_LOGGING_ERROR("invalid Sexagesimal_Type_Enum");
            exit(1);
        }
        return;
    }
}

template<class T>
Sexagesimal_Angle<T>::Sexagesimal_Angle(const char* str, Sexagesimal_Type_Enum t) throw()
{
    const char* endptr = NULL;
    errno = 0;
    angle = get_Sexagesimal_Angle_from_string<T>(str, &endptr, t).radians();
    if(endptr == str) {
        DEF_LOGGING_ERROR("unable to convert string '%s' to Sexagesimal_Angle with type 0x%X", str, int(t));
        exit(1);
    }
    return;
}

template<class T> int_fast32_t
Sexagesimal_Angle<T>::to_sexagesimal_string(Sexagesimal_Type_Enum t,
                                       const char separator_0,
                                       const char separator_1,
                                       const char separator_2,
                                       const int_fast32_t precision,
                                       const size_t STR_SIZE,
                                       char* const restrict output_str)
        const throw()
{
    static const T ln_10 = 2.3025850929940459; // log(10.0) to sufficient digits
    static const size_t FORMAT_SIZE = 8;
    char format_str[FORMAT_SIZE];
    
    if(!check_Sexagesimal_Type_Enum_valid(t)) {
        DEF_LOGGING_ERROR("invalid Sexagesimal_Type_Enum 0x%X", (unsigned int)(t));
        return -1;
    }
    if(precision < 0) {
        DEF_LOGGING_ERROR("invalid precision %d", int(precision));
        return -2;
    }
    // Calculate how many characters we need for the string
    size_t min_str_len = 0;
    T a(angle);
    char sign = '+';
    if(std::signbit(a)) {
        sign = '-';
        a = std::fabs(a);
    }
    if((sign == '-') || (! ((t & SEXAGESIMAL_TYPE_NO_PLUS)) )) {
        // have a sign byte
        min_str_len++;
    }
    if((t & SEXAGESIMAL_TYPE_RADIANS)) {
        // no conversion to degrees
    }
    else {
        // degree-based units
        a *= get_M_RAD2DEG<T>();
        switch(t & SEXAGESIMAL_TYPE_UNITS_BITMASK) {
        case SEXAGESIMAL_TYPE_HOURS:
            a /= 15.0;
            break;
        case SEXAGESIMAL_TYPE_DEGREES:
            break;
        case SEXAGESIMAL_TYPE_MINUTES:
            a *= 60.0;
            break;
        case SEXAGESIMAL_TYPE_SECONDS:
            a *= 3600.0;
            break;
        case SEXAGESIMAL_TYPE_MILLISECONDS:
            a *= 3600.0 * 1000.0;
            break;
        case SEXAGESIMAL_TYPE_MICROSECONDS:
            a *= 3600.0 * 1000000.0;
            break;
        default:
            DEF_LOGGING_ERROR("invalid Sexagesimal_Type_Enum");
        }
    }
    int expected_front_digits = 2;
    if((t & SEXAGESIMAL_TYPE_ONE_DIGIT)) {
        expected_front_digits = 1;
    }
    else if((t & SEXAGESIMAL_TYPE_THREE_DIGIT)) {
        expected_front_digits = 3;
    }
    min_str_len += expected_front_digits;
    if((t & SEXAGESIMAL_TYPE_DECIMAL)) {
        if(separator_0 != 0) {
            min_str_len++;
        }
    }
    else if((t & SEXAGESIMAL_TYPE_EXPONENTIAL)) {
        min_str_len += 6; // size for maximum E+4932 part for quad precision
        if(separator_0 != 0) {
            min_str_len++;
        }
    }
    else if((t & SEXAGESIMAL_TYPE_MINUTES_STOP)) {
        min_str_len += 3;
        if(separator_1 != 0) {
            min_str_len++;
        }
    }
    else {
        min_str_len += 6;
        if(separator_2 != 0) {
            min_str_len++;
        }
    }
    if(precision > 0) {
        min_str_len++; // period
        min_str_len += precision;
    }
    min_str_len++; // 0 at end of string
    if(min_str_len > STR_SIZE) {
        DEF_LOGGING_ERROR("expected string size %zu greater than provided string size %zu", min_str_len, STR_SIZE);
        return -3;
    }

    // Ok, actually do the conversion
    size_t current_string_pos = 0;
    if( ((t & SEXAGESIMAL_TYPE_NO_PLUS))
      || ((t & SEXAGESIMAL_TYPE_HOURS)) ){
        if(sign == '-') {
            output_str[current_string_pos++] = sign;
        }
    }
    else {
        output_str[current_string_pos++] = sign;
    }
    // get the standard float format specifier
    if((t & SEXAGESIMAL_TYPE_NO_ZERO_PAD)) {
        strcpy(format_str, "%*.*F");
        {
            char float_length = get_snprintf_length_modifier<T>();
            if((float_length)) {
                format_str[4] = float_length;
                format_str[5] = 'F';
                format_str[6] = 0;
            }
        }
    }
    else {
        strcpy(format_str, "%0*.*F");
        {
            char float_length = get_snprintf_length_modifier<T>();
            if((float_length)) {
                format_str[5] = float_length;
                format_str[6] = 'F';
                format_str[7] = 0;
            }
        }
    }


    
    if((t & SEXAGESIMAL_TYPE_DECIMAL)) {
        T num_digits = (a > 0.0) ? std::log(a) / ln_10 : 1;
        int num_digits_i = int(num_digits);
        if((t & SEXAGESIMAL_TYPE_ONE_DIGIT)) {
            num_digits_i -= 1;
        }
        else if((t & SEXAGESIMAL_TYPE_THREE_DIGIT)) {
            num_digits_i -= 3;
        }
        else {
            num_digits_i -= 2;
        }
        if(num_digits_i > 0) {
            min_str_len += num_digits_i;
            if(min_str_len > STR_SIZE) {
                DEF_LOGGING_ERROR("expected string size %zu greater than provided string size %zu", min_str_len, STR_SIZE);
                return -3;
            }
        }
        if((t & SEXAGESIMAL_TYPE_TRUNCATE)) {
            T precision_multiplier(1.0f);
            for(int i=0; i < precision; i++) {
                precision_multiplier *= T(10.0f);
            }
            T a_large = a * precision_multiplier;
            T a_trunc = std::floor(a_large);
            a = a_trunc / precision_multiplier;
        }
        int full_width = expected_front_digits + precision;
        if(precision > 0) {
            full_width++;
        }
        int num_written = Sexagesimal_Angle_snprintf<T>(output_str+current_string_pos,
                                            STR_SIZE-current_string_pos,
                                            format_str,
                                            full_width,
                                            precision,
                                            a
                                            );
        if(num_written < 0) {
            // format error
            DEF_LOGGING_ERROR("error in processing format string '%s' with arguments %d and %" PRIdFAST32, format_str, full_width, precision);
            return -5;
        }
        else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
            // needed more space to write out than was available
            DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
            return -4;
        }
        current_string_pos += num_written;
        if(separator_0 != 0) {
            if(current_string_pos >= STR_SIZE-1) {
                DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
                return -6;
            }
            output_str[current_string_pos++] = separator_0;
        }
    }
    else if((t & SEXAGESIMAL_TYPE_EXPONENTIAL)) {
        // get the exponential float format specifier    
        strcpy(format_str, "%*.*E");
        {
            char float_length = get_snprintf_length_modifier<T>();
            if((float_length)) {
                format_str[4] = float_length;
                format_str[5] = 'E';
                format_str[6] = 0;
            }
        }
        int num_written = Sexagesimal_Angle_snprintf<T>(output_str+current_string_pos,
                                            STR_SIZE-current_string_pos,
                                            format_str,
                                            1,
                                            precision,
                                            a
                                            );
        if(num_written < 0) {
            // format error
            DEF_LOGGING_ERROR("error in processing format string '%s' with arguments %d and %" PRIdFAST32, format_str, 1, precision);
            return -5;
        }
        else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
            // needed more space to write out than was available
            DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
            return -4;
        }
        current_string_pos += num_written;
        if(separator_0 != 0) {
            if(current_string_pos >= STR_SIZE-1) {
                DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
                return -6;
            }
            output_str[current_string_pos++] = separator_0;
        }
    }
    else {
        // actual sexagesimal structure
        T ad = a;
        T degrees_float = std::floor(ad);
        int degrees = int(degrees_float);
        T am = (ad - degrees) * 60.0;
        int minutes = int(std::floor(am));
        T as = (am - minutes) * 60.0;
        if((t & SEXAGESIMAL_TYPE_MINUTES_STOP)) {
            // am is limited to [0--60) above.  If we need to truncate, them
            // minutes is correct.  If not truncating, then worry about
            // rounding
            if(! ((t & SEXAGESIMAL_TYPE_TRUNCATE)) ) {
                minutes = int(lrint(am));
                if(minutes == 60) {
                    minutes = 0;
                    degrees++;
                }
            }
        }
        else {
            // also printing out seconds.  What happens for truncation or
            // rounding?
            if((t & SEXAGESIMAL_TYPE_TRUNCATE)) {
                T precision_multiplier(1.0f);
                for(int i=0; i < precision; i++) {
                    precision_multiplier *= T(10.0f);
                }
                T as_large = as * precision_multiplier;
                T as_trunc = std::floor(as_large);
                as = as_trunc / precision_multiplier;
            }
            else if(as > 59.0f) {
                // Rounding.  Figure out whether or not seconds get
                // rounded to 60.  Because of rounding in floating point
                // operations, we cannot use the multiplication and division
                // be the precision_multiplier above combined with
                // lrint to check.  We need to do a test print.
                int num_written = Sexagesimal_Angle_snprintf<T>(output_str+current_string_pos,
                                                    STR_SIZE-current_string_pos,
                                                    format_str,
                                                    1,
                                                    precision,
                                                    as
                                                    );
                if(num_written < 0) {
                    // format error
                    DEF_LOGGING_ERROR("error in processing format string '%s' with arguments %d and %" PRIdFAST32, format_str, 1, precision);
                    return -5;
                }
                else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
                    // needed more space to write out than was available
                    DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
                    return -4;
                }
                if(output_str[current_string_pos] == '6') {
                    // got rounded up to 60.
                    as = 0.0f;
                    minutes++;
                    if(minutes == 60) {
                        degrees++;
                    }
                }
            }
        }
        // OK, print degrees
        int full_width = 2;
        if((t & SEXAGESIMAL_TYPE_ONE_DIGIT)) {
            full_width = 1;
        }
        else if((t & SEXAGESIMAL_TYPE_THREE_DIGIT)) {
            full_width = 3;
        }
        char int_format_str[8];
        if((t & SEXAGESIMAL_TYPE_NO_ZERO_PAD)) {
            strcpy(int_format_str, "%*d");
        }
        else {
            strcpy(int_format_str, "%0*d");
        }
        // DEGREES
        int num_written = snprintf(output_str+current_string_pos,
                                   STR_SIZE-current_string_pos,
                                   int_format_str,
                                   full_width,
                                   degrees
                                   );
        if(num_written < 0) {
            // format error
            DEF_LOGGING_ERROR("error in processing format string '%s' with argument %d", int_format_str, full_width);
            return -7;
        }
        else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
            // needed more space to write out than was available
            DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
            return -4;
        }
        current_string_pos += num_written;
        if(current_string_pos >= STR_SIZE-1) {
            DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
            return -6;
        }
        output_str[current_string_pos++] = separator_0;
        
        // MINUTES
        full_width = 2;
        num_written = snprintf(output_str+current_string_pos,
                               STR_SIZE-current_string_pos,
                               int_format_str,
                               full_width,
                               minutes
                               );
        if(num_written < 0) {
            // format error
            DEF_LOGGING_ERROR("error in processing format string '%s' with argument %d", int_format_str, full_width);
            return -7;
        }
        else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
            // needed more space to write out than was available
            DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
            return -4;
        }
        current_string_pos += num_written;
        if((t & SEXAGESIMAL_TYPE_MINUTES_STOP)) {
            // it is ok for the separator to be the end of string character
            if(separator_1 != 0) {
                if(current_string_pos >= STR_SIZE-1) {
                    DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
                    return -6;
                }
                output_str[current_string_pos++] = separator_1;
            }
        }
        else {
            if(current_string_pos >= STR_SIZE-1) {
                DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
                return -6;
            }
            output_str[current_string_pos++] = separator_1;
        
            // SECONDS
            full_width = 2 + precision;
            if(precision > 0) {
                full_width++;
            }
            num_written = Sexagesimal_Angle_snprintf<T>(output_str+current_string_pos,
                                            STR_SIZE-current_string_pos,
                                            format_str,
                                            full_width,
                                            precision,
                                            as
                                            );
            if(num_written < 0) {
                // format error
                DEF_LOGGING_ERROR("error in processing format string '%s' with arguments %d and %" PRIdFAST32, format_str, full_width, precision);
                return -5;
            }
            else if(unsigned(num_written) >= STR_SIZE-1-current_string_pos) {
                // needed more space to write out than was available
                DEF_LOGGING_ERROR("actual string size %zu greater than provided string size %zu", current_string_pos+num_written+1, STR_SIZE);
                return -4;
            }
            current_string_pos += num_written;
            if(separator_2 != 0) {
                if(current_string_pos >= STR_SIZE-1) {
                    DEF_LOGGING_ERROR("no room for separator char, provided string size %zu", STR_SIZE);
                    return -6;
                }
                output_str[current_string_pos++] = separator_2;
            }
        }
    }
    // End the string
    if(current_string_pos >= STR_SIZE-1) {
        DEF_LOGGING_ERROR("no room for end of string char, actual string size %zu", STR_SIZE);
        return -8;
    }
    output_str[current_string_pos++] = 0;
    return 0;
}
 



}  // end namespace Angles
}  // end namespace MPIfR

#undef NUM
#endif // MPIfR_Sexagesimal_Angle_H
