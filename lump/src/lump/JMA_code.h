// JMA_code.h 
// JMA's generic programming stuff
// 2000 Sep 17  James M Anderson  --NMT  Original Version
// 2007 Jan 08  JMA  --define __STDC_LIMIT_MACROS
// 2011 Mar 31  JMA  --MPIfR  make *_t typedefs for floating point
// 2012 Jan 30  JMA  --add __float128 stuff
// 2012 Feb 05  JMA  --update __float128 stuff and int128_t stuff
// 2012 Feb 25  JMA  --add declarations for alignment stuff for SIMD
//                     optimizations
// 2012 Feb 25  JMA  --update long double declarations
// 2013 Jul 13  JMA  ---- add malloc gcc function attribute
// 2014 Sep 10  JMA  ---- add gcc pointer data alignment hacks
// 2014 Sep 11  JMA  ---- include <inttypes.h> for printf/scanf stuff
// 2014 Sep 17  JMA  --- added format attribute macro
// 2014 Sep 19  JMA  --- fix checking of __GNUC__ to see that it is defined

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


#ifndef JMA_CODE_H
#define JMA_CODE_H



// MACRO TO MAKE UNIX F77 ROUTINES AVAILABLE
#define FTN_NAME(a)                               a##_



// JMA typedefs for standard type names of things
#ifdef __cplusplus
#  include <cstddef>
#else
#  include <stddef.h>
#endif
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include <inttypes.h>
// Exact types
typedef  int8_t                     Sint8;
typedef uint8_t                     Uint8;
typedef  int16_t                    Sint16;
typedef uint16_t                    Uint16;
typedef  int32_t                    Sint32;
typedef uint32_t                    Uint32;
typedef  int64_t                    Sint64;
typedef uint64_t                    Uint64;
#if ((defined __GNUC__) && ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))) \
    && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#  if ((defined(__LP64__) && !defined(__hppa__)) || defined(_WIN64)) 
typedef signed __int128             int128_t;
typedef unsigned __int128           uint128_t;
typedef int128_t                    int_fast128_t;
typedef uint128_t                   uint_fast128_t;
typedef int128_t                    int_least128_t;
typedef uint128_t                   uint_least128_t;
typedef int128_t                    Sint128;
typedef uint128_t                   Uint128;
#  define JMA_CODE_HAVE_NATIVE_128_BIT_INT
#  endif
#endif

typedef float                       Real32;
typedef double                      Real64;
typedef long double                 Real80;
typedef float                       Real32_t;
typedef double                      Real64_t;
#if defined(__i386__) || defined(__x86_64__)
// Note that for x86-64, the 80 bit long doubles are aligned on
// 16 byte boundries, so that the actual memory size taken up is
// 128 bits.
typedef long double                 Real80_t;
#  define JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#  define JMA_CODE_Real80_t_CODE l
#endif

#if ((defined __GNUC__) && ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))) \
    && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#  ifdef __cplusplus
extern "C" {
#  endif
#  include <quadmath.h>
typedef __float128                  Real128_t;
#  ifdef __cplusplus
}
#  endif
#  define JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  define JMA_CODE_HAVE_QUAD_FLOAT
#  define JMA_CODE_REAL128_T_CODE q
#endif

// non exact types

typedef ptrdiff_t                   Index_t;

// FORTRAN INTERFACE
// integer should be the size of a real, which is probably 32 bits
typedef int32_t                     Finteger;






// ATTRIBUTE SPECIFICATIONS
// These specifications are used to give hints to the compilers that
// the data variables have specific properties, for use in optimization
// and warning production.

// Alignment attributes
#if (defined __GNUC__) && (__GNUC__ >= 3)
// Indicate that a variable of type should be aligned to bytes boundaries
// For example, JMA_VARIABLE_ALIGNMENT(float,16) f; will allign variable
// f to a 16 byte boundary.
#  define JMA_VARIABLE_ALIGNMENT(type,bytes)  type __attribute__ ((aligned (bytes)))
// Indicate that it is ok if a variable of type type is not actually
// used by a function, and that no warning should be issued.
// Example: JMA_VARIABLE_UNUSED_OK(float) f;
#  define JMA_VARIABLE_UNUSED_OK(type) type  __attribute__ ((unused))
// GCC pure functions depend only on their arguments and global variables
// and have no side effects.  GCC pure functions may read data from memory
// through pointer arguments.
// Example:  int foo(int bar) JMA_FUNCTION_IS_PURE;
#  define JMA_FUNCTION_IS_PURE   __attribute__ ((pure))
// GCC const functions are like GCC pure functions, but more strict.
// No global memory may be accessed, no reading data through pointer
// arguments.
// Example:  int foo(int bar) JMA_FUNCTION_IS_CONST;
#  define JMA_FUNCTION_IS_CONST  __attribute__ ((const)) 
// GCC malloc functions return a pointer to memory that, when non-NULL
// cannot alias any other pointer, and that the memory pointed to by
// this pointer has undefined content.  This should be a malloc-like
// function.
// Example:  int* foo(int bar) JMA_FUNCTION_IS_MALLOC;
#  define JMA_FUNCTION_IS_MALLOC  __attribute__ ((malloc)) 
// GCC format functions should work like printf or scanf, and this attribute
// instruction tells the compiler to check the format string against the
// arguments to look for format errors.  f_pos is the argument position in the
// argument list (starting with 1, and remember that for member functions
// there is an implicit *this argument so increment up by one compared to what
// you see in the argument list), and arg_start is the starting argument
// position.
// Example:  int myprintf(int fd, char* fmt, int arg0, int arg2) JMA_FUNCTION_IS_FORMAT(printf,2,3);
#  define JMA_FUNCTION_IS_FORMAT(f_type,f_pos,arg_start)  __attribute__ ((format (f_type,f_pos,arg_start))) 
#else
#  define JMA_VARIABLE_ALIGNMENT(type,bytes) type
#  define JMA_VARIABLE_UNUSED_OK(type) type
#  define JMA_FUNCTION_IS_PURE
#  define JMA_FUNCTION_IS_CONST
#  define JMA_FUNCTION_IS_MALLOC
#  define JMA_FUNCTION_IS_FORMAT(f_type,f_pos,arg_start)
#endif
#if (defined __GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))
// GCC function
// void *__builtin_assume_aligned (const void *exp, size_t align, ...)
// tells the compiler that the software developer is guaranteeing that
// the pointer value provided by exp is aligned to a byte boundary
// indicated by align, and outputs a pointer that you can use, where the
// compiler knows that the data are aligned.  The third, optional, argument
// gives an offset from the specified alignement of the 0 element.
// Example:  int* restrict x_new = (int*) JMA_SPECIFY_POINTER_DATA_ALIGNMENT(x,16)
// Example:  int* restrict x_new = (int*) JMA_SPECIFY_POINTER_DATA_ALIGNMENT3(x,16,4)
#  define JMA_SPECIFY_POINTER_DATA_ALIGNMENT(var,bytes) __builtin_assume_aligned(var,bytes)
#  define JMA_SPECIFY_POINTER_DATA_ALIGNMENT3(var,bytes,offset) __builtin_assume_aligned(var,bytes,offset)
#else
#  define JMA_SPECIFY_POINTER_DATA_ALIGNMENT(var,bytes)
#  define JMA_SPECIFY_POINTER_DATA_ALIGNMENT3(var,bytes,offset)
#endif






#endif // JMA_CODE_H

