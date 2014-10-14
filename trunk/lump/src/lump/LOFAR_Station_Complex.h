// LOFAR_Station_Complex.h
// Integer complex functions for LOFAR station data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 10  James M Anderson  --MPIfR  start


// Copyright (c) 2011,2012,2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Complex_H
#define LOFAR_Station_Complex_H

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
#include "MPIfR_Real16_t.h"
#include "LOFAR_Station_Common.h"


#ifdef LOFAR_Station_Complex_H_FILE
#  define NUM(x) =x;
#  define NUM16(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xB,xC,xD,xE,xF) = {x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xB,xC,xD,xE,xF};
#else
#  define NUM(x)
#  define NUM16(x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xA,xB,xC,xD,xE,xF)
#endif


// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {



#define LOFAR_raw_data_type_enum_Array \
    LOFAR_ENUM_VALS(     L_int1_t           ,  0, "L_int1_t") \
        LOFAR_ENUM_VALS( L_int2_t           ,  1, "L_int2_t")   \
        LOFAR_ENUM_VALS( L_int3_t           ,  2, "L_int3_t")   \
        LOFAR_ENUM_VALS( L_int4_t           ,  3, "L_int4_t")   \
        LOFAR_ENUM_VALS( L_int5_t           ,  4, "L_int5_t")   \
        LOFAR_ENUM_VALS( L_int6_t           ,  5, "L_int6_t")   \
        LOFAR_ENUM_VALS( L_int7_t           ,  6, "L_int7_t")   \
        LOFAR_ENUM_VALS( L_int8_t           ,  7, "L_int8_t")   \
        LOFAR_ENUM_VALS( L_int10_t          ,  8, "L_int10_t")   \
        LOFAR_ENUM_VALS( L_int12_t          ,  9, "L_int12_t")   \
        LOFAR_ENUM_VALS( L_int16_t          , 10, "L_int16_t")   \
        LOFAR_ENUM_VALS( L_int32_t          , 11, "L_int32_t")   \
        LOFAR_ENUM_VALS( L_int64_t          , 12, "L_int64_t")   \
        LOFAR_ENUM_VALS( L_int128_t         , 13, "L_int128_t")   \
        LOFAR_ENUM_VALS( L_Real16_t         , 14, "L_Real16_t")   \
        LOFAR_ENUM_VALS( L_Real32_t         , 15, "L_Real32_t")   \
        LOFAR_ENUM_VALS( L_Real64_t         , 16, "L_Real64_t")   \
        LOFAR_ENUM_VALS( L_Real80_t         , 17, "L_Real80_t")   \
        LOFAR_ENUM_VALS( L_Real128_t        , 18, "L_Real128_t")   \
        LOFAR_ENUM_VALS( L_Complex32_t      , 19, "L_Complex32_t")   \
        LOFAR_ENUM_VALS( L_Complex64_t      , 20, "L_Complex64_t")   \
        LOFAR_ENUM_VALS( L_Complex128_t     , 21, "L_Complex128_t")   \
        LOFAR_ENUM_VALS( L_Complex160_t     , 22, "L_Complex160_t")   \
        LOFAR_ENUM_VALS( L_Complex256_t     , 23, "L_Complex256_t")   \
        LOFAR_ENUM_VALS( L_intComplex8_t    , 24, "L_intComplex8_t")   \
        LOFAR_ENUM_VALS( L_intComplex16_t   , 25, "L_intComplex16_t")   \
        LOFAR_ENUM_VALS( L_intComplex32_t   , 26, "L_intComplex32_t")   \
        LOFAR_ENUM_VALS( L_intComplex64_t   , 27, "L_intComplex64_t")   \
        LOFAR_ENUM_VALS( L_intComplex128_t  , 28, "L_intComplex128_t")   \
        LOFAR_ENUM_VALS( L_intComplex256_t  , 29, "L_intComplex256_t")   \
        LOFAR_ENUM_VALS( L_RAW_DATA_TYPE_MAX, 30, "L_RAW_DATA_TYPE_MAX")   \




enum LOFAR_raw_data_type_enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_raw_data_type_enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_raw_data_type_enum_Str(LOFAR_raw_data_type_enum e) throw();





size_t LOFAR_raw_data_type_enum_size(LOFAR_raw_data_type_enum type) throw();


// pre-declare int complex classes
class intComplex8_t;
class intComplex16_t;
class intComplex32_t;
class intComplex64_t;
class intComplex128_t;
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
class intComplex256_t;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT




// These are conversion constants to convert 4 bit *unsigned* integers
// to Real*_t types, as if the 4 bits were signed integers.  This
// is useful for the intComplex8_t type below, where the internal
// representation of the real and imaginary parts is stored in a
// single uint8_t.
extern const uint16_t int4_t_values_Real16_t_int[16]
NUM16(      0x0000, 0x3C00, 0x4000, 0x4200, 0x4400, 0x4500, 0x4600, 0x4700,
            0xC800, 0xC700, 0xC600, 0xC500, 0xC400, 0xC200, 0xC000, 0xBC00);
extern const MPIfR::DATA_TYPE::Real16_t * const restrict int4_t_values_Real16_t
NUM(reinterpret_cast<const MPIfR::DATA_TYPE::Real16_t* const restrict>(int4_t_values_Real16_t_int));
extern const Real32_t int4_t_values_Real32_t[16]
NUM16(      +0.0f, +1.0f, +2.0f, +3.0f, +4.0f, +5.0f, +6.0f, +7.0f,
            -8.0f, -7.0f, -6.0f, -5.0f, -4.0f, -3.0f, -2.0f, -1.0f);
extern const Real64_t int4_t_values_Real64_t[16]
NUM16(      +0.0, +1.0, +2.0, +3.0, +4.0, +5.0, +6.0, +7.0,
            -8.0, -7.0, -6.0, -5.0, -4.0, -3.0, -2.0, -1.0);
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
extern const Real80_t int4_t_values_Real80_t[16]
NUM16(      +0.0, +1.0, +2.0, +3.0, +4.0, +5.0, +6.0, +7.0,
            -8.0, -7.0, -6.0, -5.0, -4.0, -3.0, -2.0, -1.0);
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
extern const Real128_t int4_t_values_Real128_t[16]
NUM16(      +0.0, +1.0, +2.0, +3.0, +4.0, +5.0, +6.0, +7.0,
            -8.0, -7.0, -6.0, -5.0, -4.0, -3.0, -2.0, -1.0);
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT









//_CLASS  Complex32_t -- complex number using 2 Real16_t values
class Complex32_t {
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
    Complex32_t(const MPIfR::DATA_TYPE::Real16_t& restrict r, const MPIfR::DATA_TYPE::Real16_t& restrict i): re(r), im(i) {};
    MPIfR::DATA_TYPE::Real16_t real() const throw() {return re;}
    MPIfR::DATA_TYPE::Real16_t imag() const throw() {return im;}

    operator Complex64_t() const throw() {return Complex64_t(Real32_t(re),Real32_t(im));}
    operator Complex128_t() const throw() {return Complex128_t(Real64_t(re),Real64_t(im));}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(Real80_t(re),Real80_t(im));}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(Real128_t(re),Real128_t(im));}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static Complex32_t flag_vlaue() throw() {return Complex32_t(int4_t_values_Real16_t[0],int4_t_values_Real16_t[0]);}
    

protected:



private:
    MPIfR::DATA_TYPE::Real16_t re,im;

    
};








//_CLASS  intComplex8_t -- complex number using 4 bit integers for real,imag
class intComplex8_t {
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
    intComplex8_t(const int8_t& restrict r, const int8_t& restrict i) {val=(uint8_t(r)&0xF)+((uint8_t(i)&0xF)<<4);return;};
    explicit intComplex8_t(const intComplex16_t& restrict z) throw();
    explicit intComplex8_t(const intComplex32_t& restrict z) throw();
    explicit intComplex8_t(const intComplex64_t& restrict z) throw();
    explicit intComplex8_t(const intComplex128_t& restrict z) throw();

#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    explicit intComplex8_t(const intComplex256_t& restrict z) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT


    int8_t real() const throw() {return int8_t(int8_t(val<<4)>>4);}
    int8_t imag() const throw() {return int8_t(int8_t(val)>>4);}

    operator Complex32_t() const throw()
    {return Complex32_t(int4_t_values_Real16_t[real_internal()],
                        int4_t_values_Real16_t[imag_internal()]);}
    operator Complex64_t() const throw()
    {return Complex64_t(int4_t_values_Real32_t[real_internal()],
                        int4_t_values_Real32_t[imag_internal()]);}
    operator Complex128_t() const throw()
    {return Complex128_t(int4_t_values_Real64_t[real_internal()],
                         int4_t_values_Real64_t[imag_internal()]);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw()
    {return Complex160_t(int4_t_values_Real80_t[real_internal()],
                         int4_t_values_Real80_t[imag_internal()]);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw()
    {return Complex256_t(int4_t_values_Real128_t[real_internal()],
                         int4_t_values_Real128_t[imag_internal()]);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT


    static intComplex8_t flag_vlaue() throw() {return intComplex8_t(0,0);}
    

protected:



private:
    uint8_t val;

    uint_fast32_t real_internal() const throw() {return (val&0xF);}
    uint_fast32_t imag_internal() const throw() {return (val>>4);}
    
};





//_CLASS  intComplex16_t -- complex numberr using 8 bit integers for real,imag
class intComplex16_t {
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
    intComplex16_t(const int8_t& restrict r, const int8_t& restrict i): re(r), im(i) {};
    intComplex16_t(const intComplex8_t& restrict z): re(z.real()), im(z.imag()) {};
    explicit intComplex16_t(const intComplex32_t& restrict z) throw();
    explicit intComplex16_t(const intComplex64_t& restrict z) throw();
    explicit intComplex16_t(const intComplex128_t& restrict z) throw();
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    explicit intComplex16_t(const intComplex256_t& restrict z) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

    int8_t real() const throw() {return re;}
    int8_t imag() const throw() {return im;}

    operator Complex32_t() const throw()
    {Real64_t r(re), i(im); return Complex32_t(MPIfR::DATA_TYPE::Real16_t(r),MPIfR::DATA_TYPE::Real16_t(i));}
    operator Complex64_t() const throw() {return Complex64_t(re,im);}
    operator Complex128_t() const throw() {return Complex128_t(re,im);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static intComplex16_t flag_vlaue() throw() {return intComplex16_t(0,0);}
    

protected:



private:
    int8_t re, im;

    
};







//_CLASS  intComplex32_t -- complex numberr using 16 bit integers for real,imag
class intComplex32_t {
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
    intComplex32_t(const int16_t& restrict r, const int16_t& restrict i): re(r), im(i) {};
    intComplex32_t(const intComplex8_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex32_t(const intComplex16_t& restrict z): re(z.real()), im(z.imag()) {};
    explicit intComplex32_t(const intComplex64_t& restrict z) throw();
    explicit intComplex32_t(const intComplex128_t& restrict z) throw();
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    explicit intComplex32_t(const intComplex256_t& restrict z) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

    int16_t real() const throw() {return re;}
    int16_t imag() const throw() {return im;}

    operator Complex32_t() const throw()
    {Real64_t r(re), i(im); return Complex32_t(MPIfR::DATA_TYPE::Real16_t(r),MPIfR::DATA_TYPE::Real16_t(i));}
    operator Complex64_t() const throw() {return Complex64_t(re,im);}
    operator Complex128_t() const throw() {return Complex128_t(re,im);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static intComplex32_t flag_vlaue() throw() {return intComplex32_t(0,0);}
    

protected:



private:
    int16_t re, im;

    
};







//_CLASS  intComplex64_t -- complex numberr using 32 bit integers for real,imag
class intComplex64_t {
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
    intComplex64_t(const int32_t& restrict r, const int32_t& restrict i): re(r), im(i) {};
    intComplex64_t(const intComplex8_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex64_t(const intComplex16_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex64_t(const intComplex32_t& restrict z): re(z.real()), im(z.imag()) {};
    explicit intComplex64_t(const intComplex128_t& restrict z) throw();
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    explicit intComplex64_t(const intComplex256_t& restrict z) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

    int32_t real() const throw() {return re;}
    int32_t imag() const throw() {return im;}

    operator Complex32_t() const throw()
    {Real64_t r(re), i(im); return Complex32_t(MPIfR::DATA_TYPE::Real16_t(r),MPIfR::DATA_TYPE::Real16_t(i));}
    operator Complex64_t() const throw() {return Complex64_t(re,im);}
    operator Complex128_t() const throw() {return Complex128_t(re,im);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static intComplex64_t flag_vlaue() throw() {return intComplex64_t(0,0);}
    

protected:



private:
    int32_t re, im;

    
};




//_CLASS  intComplex128_t -- complex numberr using 64 bit integers for real,imag
class intComplex128_t {
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
    intComplex128_t(const int64_t& restrict r, const int64_t& restrict i): re(r), im(i) {};
    intComplex128_t(const intComplex8_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex128_t(const intComplex16_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex128_t(const intComplex32_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex128_t(const intComplex64_t& restrict z): re(z.real()), im(z.imag()) {};
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
    explicit intComplex128_t(const intComplex256_t& restrict z) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

    int64_t real() const throw() {return re;}
    int64_t imag() const throw() {return im;}

    operator Complex32_t() const throw()
    {Real64_t r(re), i(im); return Complex32_t(MPIfR::DATA_TYPE::Real16_t(r),MPIfR::DATA_TYPE::Real16_t(i));}
    operator Complex64_t() const throw() {return Complex64_t(re,im);}
    operator Complex128_t() const throw() {return Complex128_t(re,im);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static intComplex128_t flag_vlaue() throw() {return intComplex128_t(0,0);}
    

protected:



private:
    int64_t re, im;

    
};




#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
//_CLASS  intComplex256_t -- complex numberr using 64 bit integers for real,imag
class intComplex256_t {
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
    intComplex256_t(const int64_t& restrict r, const int64_t& restrict i): re(r), im(i) {};
    intComplex256_t(const intComplex8_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex256_t(const intComplex16_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex256_t(const intComplex32_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex256_t(const intComplex64_t& restrict z): re(z.real()), im(z.imag()) {};
    intComplex256_t(const intComplex128_t& restrict z): re(z.real()), im(z.imag()) {};

    int64_t real() const throw() {return re;}
    int64_t imag() const throw() {return im;}

    operator Complex32_t() const throw()
    {Real64_t r(re), i(im); return Complex32_t(MPIfR::DATA_TYPE::Real16_t(r),MPIfR::DATA_TYPE::Real16_t(i));}
    operator Complex64_t() const throw() {return Complex64_t(re,im);}
    operator Complex128_t() const throw() {return Complex128_t(re,im);}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    operator Complex160_t() const throw() {return Complex160_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    operator Complex256_t() const throw() {return Complex256_t(re,im);}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    static intComplex256_t flag_vlaue() throw() {return intComplex256_t(0,0);}
    

protected:



private:
    int128_t re, im;

    
};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT







inline intComplex8_t::intComplex8_t(const intComplex16_t& restrict z) throw() {val=(uint8_t(z.real())&0xF)+((uint8_t(z.imag())&0xF)<<4);return;};
inline intComplex8_t::intComplex8_t(const intComplex32_t& restrict z) throw() {val=(uint8_t(z.real())&0xF)+((uint8_t(z.imag())&0xF)<<4);return;};
inline intComplex8_t::intComplex8_t(const intComplex64_t& restrict z) throw() {val=(uint8_t(z.real())&0xF)+((uint8_t(z.imag())&0xF)<<4);return;};
inline intComplex8_t::intComplex8_t(const intComplex128_t& restrict z) throw() {val=(uint8_t(z.real())&0xF)+((uint8_t(z.imag())&0xF)<<4);return;};
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
inline intComplex8_t::intComplex8_t(const intComplex256_t& restrict z) throw() {val=(uint8_t(z.real())&0xF)+((uint8_t(z.imag())&0xF)<<4);return;};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

inline intComplex16_t::intComplex16_t(const intComplex32_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
inline intComplex16_t::intComplex16_t(const intComplex64_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
inline intComplex16_t::intComplex16_t(const intComplex128_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
inline intComplex16_t::intComplex16_t(const intComplex256_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

inline intComplex32_t::intComplex32_t(const intComplex64_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
inline intComplex32_t::intComplex32_t(const intComplex128_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
inline intComplex32_t::intComplex32_t(const intComplex256_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

inline intComplex64_t::intComplex64_t(const intComplex128_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
inline intComplex64_t::intComplex64_t(const intComplex256_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT

#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_INT
inline intComplex128_t::intComplex128_t(const intComplex256_t& restrict z) throw(): re(z.real()), im(z.imag()) {};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_INT



// CLASS FUNCTIONS



// HELPER FUNCTIONS






//_CLASS  intComplex32_t_array -- handling for contiguous intComplex32_t subbands
class intComplex32_t_array {
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
    // Give the start and end positions in terms of Beamlets
    intComplex32_t_array(const uint_fast16_t start, const uint_fast16_t end) :
            p(NULL),
            start_pos(start*sizeof(intComplex32_t)*Station::NUM_RSP_BEAMLET_POLARIZATIONS),
            end_pos(end*sizeof(intComplex32_t)*Station::NUM_RSP_BEAMLET_POLARIZATIONS),
            size((end-start)*sizeof(intComplex32_t)*Station::NUM_RSP_BEAMLET_POLARIZATIONS)
    {};
    void copy_timestep(const char* restrict cp) throw() {p = cp; return;}
    const char* restrict data_pos() const throw() {return p;} // in char
    const uint_fast16_t data_size() const throw() {return size;} // in char
        
    
        

protected:



private:
    const char* restrict p;
    const uint_fast16_t start_pos;      // in char
    const uint_fast16_t end_pos;        // in char
    const uint_fast16_t size;           // in char


    // prevent copying
    intComplex32_t_array(const intComplex32_t_array& a);
    intComplex32_t_array& operator=(const intComplex32_t_array& a);
    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS











//_CLASS  intComplex32_t_selection -- handling for random intComplex32_t subbands
class intComplex32_t_selection {
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
    
        

protected:



private:
    intComplex32_t array[Station::MAX_RSP_BEAMLETS_PER_SAMPLE_TIMESTEP];

    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS










}  // end namespace LOFAR
}  // end namespace MPIfR

#undef NUM
#undef NUM16
#endif // LOFAR_Station_Complex_H
