// MPIfR_Real16_t.h
// class to deal with conversion to a 16 bit real
//_HIST  DATE NAME PLACE INFO
// 2011 May 11  James M Anderson  --MPIfR  start
// 2011 Aug 03  JMA  --add mechanism to load data from integer bits, as
//                     byteswapping is difficult (cannot make a union with
//                     a uint16_t and this class)
// 2012 Mar 10  JMA  --update for 128 bit integers, 128 bit reals

// Copyright (c) 2011,2012,2014  James M. Anderson <anderson@mpifr-bonn.mpg.de>

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




// This class deals with a half-precision binary floating point format,
// from the IEEE 754-2008 16-bit base 2 format, officially referred to as
// binary16.  The class deals with conversion to and from single and double
// precision IEEE 754 base 2 floating point formats.

// See http://en.wikipedia.org/wiki/Half_precision_floating-point_format
//     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0205j/CIHGAECI.html

#ifndef MPIfR_Real16_t_H
#define MPIfR_Real16_t_H

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




// set up a namespace area for stuff.
namespace MPIfR {

namespace DATA_TYPE {



//_CLASS  Real16_t
class Real16_t {
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
    Real16_t() throw() {};
    Real16_t(const Real32_t& restrict f) throw() {bits = Real16_t(&f).bits;}
    Real16_t(const Real32_t* const restrict fp) throw();
    Real16_t(const Real64_t& restrict f) throw() {bits = Real16_t(&f).bits;}
    Real16_t(const Real64_t* const restrict fp) throw();


#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    explicit Real16_t(const Real80_t& restrict f) throw() {bits = Real16_t(&f).bits;}
    explicit Real16_t(const Real80_t* const restrict fp) throw();
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    explicit Real16_t(const Real128_t& restrict f) throw() {bits = Real16_t(&f).bits;}
    explicit Real16_t(const Real128_t* const restrict fp) throw();
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    Real16_t(const int_fast64_t& restrict i) throw() {Real64_t f(i); bits = Real16_t(&f).bits;}
    Real16_t(const int_fast64_t* const restrict ip) throw() {Real64_t f(*ip); bits = Real16_t(&f).bits;}
    Real16_t(const uint_fast64_t& restrict i) throw() {Real64_t f(i); bits = Real16_t(&f).bits;}
    Real16_t(const uint_fast64_t* const restrict ip) throw() {Real64_t f(*ip); bits = Real16_t(&f).bits;}
    explicit Real16_t(const int_fast128_t& restrict i) throw() {Real64_t f(i); bits = Real16_t(&f).bits;}
    explicit Real16_t(const int_fast128_t* const restrict ip) throw() {Real64_t f(*ip); bits = Real16_t(&f).bits;}
    explicit Real16_t(const uint_fast128_t& restrict i) throw() {Real64_t f(i); bits = Real16_t(&f).bits;}
    explicit Real16_t(const uint_fast128_t* const restrict ip) throw() {Real64_t f(*ip); bits = Real16_t(&f).bits;}


    Real32_t to_Real32_t() const throw();
    Real64_t to_Real64_t() const throw();
    operator Real32_t() const throw() {return to_Real32_t();}
    operator Real64_t() const throw() {return to_Real64_t();}
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    Real80_t to_Real80_t() const throw();
    operator Real80_t() const throw() {return to_Real80_t();}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    Real128_t to_Real128_t() const throw();
    operator Real128_t() const throw() {return to_Real128_t();}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT

    Real16_t& load_bits(uint16_t b) throw() {bits = b; return *this;}
    Real16_t& load_bits_byteswap(uint16_t b) throw() {bits = ((b>>8)|(b<<8)); return *this;}



protected:



private:
    uint16_t bits;


    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS



}  // end namespace DATA_TYPE

}  // end namespace MPIfR

#endif // MPIfR_Real16_t_H
