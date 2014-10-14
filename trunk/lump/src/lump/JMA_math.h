// JMA_math.h
// add in stuff for good math stuff under GCC
// 2000 Sep 17  James M Anderson  --NMT  Original Version
// 2005 Sep 20  JMA  --update some \pi related constants to more precision
// 2007 Dec 03  JMA  --MPIfR update for complex
// 2008 Sep 22  JMA  --fix M_PI stuff
// 2009 Jan 19  JMA  --chnage complex to full bit depth numbers
// 2010 Feb 08  JMA  --add tgmath stuff
// 2011 Mar 31  JMA  --MPIfR  make *_t typedefs for floating point
// 2012 Jan 30  JMA  --add __float128 stuff
// 2012 Feb 05  JMA  --update __float128 stuff and int128_t stuff
// 2012 Feb 25  JMA  --update long double declarations

// Copyright (c) 2012, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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


#ifndef JMA_MATH_H
#define JMA_MATH_H


// make sure we have some type things
#include "JMA_code.h"



// are we usign GCC ?
// if not, barf
#ifndef __GNUC__
# 	error "GCC required for compilation"
#elif (__GNUC__ < 2)
#	error "at least GCC 2 required for compilation"
#endif

// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif
#include <time.h>
#ifndef __USE_ISOC99
#  define __USE_ISOC99 1
#endif
#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif
#ifndef __USE_MISC
#  define __USE_MISC 1
#endif





// get the math functions
#ifdef __cplusplus
#  include <complex>
#  include <cmath>
#  include <math.h>
typedef std::complex<float>         Complex64;
typedef std::complex<double>        Complex128;
typedef std::complex<long double>   Complex160;
typedef std::complex<float>         Complex64_t;
typedef std::complex<double>        Complex128_t;
#  if defined(__i386__) || defined(__x86_64__)
// Note that for x86-64, the 80 bit long doubles are aligned on
// 16 byte boundries, so that the actual memory size taken up is
// 128 bits.
typedef std::complex<long double>   Complex160_t;
#  endif
#  if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
    && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#    include <quadmath.h>
typedef std::complex<__float128>    Complex256_t;
#  endif
#else
#  include <complex.h>
#  include <math.h>
#  include <tgmath.h>
typedef _Complex float              Complex64;
typedef _Complex double             Complex128;
typedef _Complex long double        Complex160;
typedef _Complex float              Complex64_t;
typedef _Complex double             Complex128_t;
#  define Complex64(r,i) ((float)(r) + ((float)(i))*I)
#  define Complex128(r,i) ((double)(r) + ((double)(i))*I)
#  define Complex160(r,i) ((long double)(r) + ((long double)(i))*I)
#  define Complex64_t(r,i) ((float)(r) + ((float)(i))*I)
#  define Complex128_t(r,i) ((double)(r) + ((double)(i))*I)
#  if defined(__i386__) || defined(__x86_64__)
// Note that for x86-64, the 80 bit long doubles are aligned on
// 16 byte boundries, so that the actual memory size taken up is
// 128 bits.
typedef _Complex long double        Complex160_t;
#  define Complex160_t(r,i) ((long double)(r) + ((long double)(i))*I)
#  endif
#  if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
    && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#    include <quadmath.h>
typedef __complex128                Complex256_t;
#    define Complex256_t(r,i) ((__float128)(r) + ((__float128)(i))*I)
#  endif
//#define real(x) creal(x)
//#define imag(x) cimag(x)
//#define abs(x) fabs(x)
//#define arg(x) carg(x)
#endif







// put anything else intersting here

/* restrict
   This is a really useful modifier, but it is not supported by
   all compilers.  Furthermore, the different ways to specify it
   (double * restrict dp0, double dp1[restrict]) are not available
   in the same release of a compiler.  If you are still using an old
   compiler, your performace is going to suck anyway, so this code
   will only give you restrict when it is fully available.
*/
#ifdef __GNUC__
#  ifdef restrict
/*   Someone else has already defined it.  Hope they got it right. */
#  elif !defined(__GNUG__) && (__STDC_VERSION__ >= 199901L)
/*   Restrict already available */
#  elif !defined(__GNUG__) && (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#    define restrict __restrict
#  elif (__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#    define restrict __restrict
#  else
#    define restrict
#  endif
#else
#  ifndef restrict
#    define restrict
#  endif
#endif






// Radian to Degree conversions
// #define M_RAD2DEG 57.29577951308232087665
// #define M_DEG2RAD  0.0174532925199432976913
#ifndef M_RAD2DEG
#  define M_RAD2DEG 57.29577951308232087679815481410517033240547246656432154916024386120284714832155263244096899585111094418622338163286489328144826460124831503606826786
#endif
#ifndef M_DEG2RAD
#  define M_DEG2RAD  0.0174532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924838546922752810123984742
#endif
#ifndef M_RAD2DEGf
#  define M_RAD2DEGf 57.29577951308232087679815481410517033240547246656432154916024386120284714832155263244096899585111094418622338163286489328144826460124831503606826786f
#endif
#ifndef M_DEG2RADf
#  define M_DEG2RADf  0.0174532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924838546922752810123984742f
#endif
#ifndef M_RAD2DEGl
#  define M_RAD2DEGl 57.29577951308232087679815481410517033240547246656432154916024386120284714832155263244096899585111094418622338163286489328144826460124831503606826786L
#endif
#ifndef M_DEG2RADl
#  define M_DEG2RADl  0.0174532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924838546922752810123984742L
#endif

// C99 decided to drop M_PI and other things.  Define them here
#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534
#endif
#ifndef M_PIf
#  define M_PIf 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534f
#endif
#ifndef M_PIl
#  define M_PIl 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534L
#endif
#ifndef M_2PI
#  define M_2PI 6.283185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234135964296173026564613294187689
#endif
#ifndef M_2PIf
#  define M_2PIf 6.283185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234135964296173026564613294187689f
#endif
#ifndef M_2PIl
#  define M_2PIl 6.283185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234135964296173026564613294187689L
#endif
#ifndef M_PI_2
#  define M_PI_2 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775
#endif
#ifndef M_PI_2f
#  define M_PI_2f 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775f
#endif
#ifndef M_PI_2l
#  define M_PI_2l 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775L
#endif
#ifdef JMA_CODE_HAVE_QUAD_FLOAT
#  ifndef M_PIq
#    define M_PIq 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534Q
#  endif
#  ifndef M_2PIq
#    define M_2PIq 6.283185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234135964296173026564613294187689Q
#  endif
#  ifndef M_PI_2q
#    define M_PI_2q 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775Q
#  endif
#  ifndef M_RAD2DEGq
#    define M_RAD2DEGq 57.29577951308232087679815481410517033240547246656432154916024386120284714832155263244096899585111094418622338163286489328144826460124831503606826786Q
#  endif
#  ifndef M_DEG2RADq
#    define M_DEG2RADq  0.0174532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449250737905924838546922752810123984742Q
#  endif
#endif

#include <float.h>

#ifndef REAL32_T_MAX
#  define REAL32_T_MAX FLT_MAX
#endif
#ifndef REAL32_T_MIN
#  define REAL32_T_MIN FLT_MIN
#endif
#ifndef REAL32_T_EPSILON
#  define REAL32_T_EPSILON FLT_EPSILON
#endif

#ifndef REAL64_T_MAX
#  define REAL64_T_MAX DBL_MAX
#endif
#ifndef REAL64_T_MIN
#  define REAL64_T_MIN DBL_MIN
#endif
#ifndef REAL64_T_EPSILON
#  define REAL64_T_EPSILON DBL_EPSILON
#endif

#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#  ifndef REAL80_T_MAX
#    define REAL80_T_MAX FLT_MAX
#  endif
#  ifndef REAL80_T_MIN
#    define REAL80_T_MIN FLT_MIN
#  endif
#  ifndef REAL80_T_EPSILON
#    define REAL80_T_EPSILON FLT_EPSILON
#  endif
#endif

#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
      && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#    ifndef REAL128_T_MAX
#      define REAL128_T_MAX FLT128_MAX
#    endif
#    ifndef REAL128_T_MIN
#      define REAL128_T_MIN FLT128_MIN
#    endif
#    ifndef REAL128_T_EPSILON
#      define REAL128_T_EPSILON FLT128_EPSILON
#    endif
#  endif
#endif


#  ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
        && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
#      ifdef __cplusplus
extern "C" {
#      endif
inline __float128 isfinite(__float128 x) {return finiteq(x);}
#      ifdef __cplusplus
}
#      endif
#  endif
#endif
#ifdef __cplusplus
#  ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
        && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__))
// gcc does not have C++ versions of quad math functions yet
namespace std{
inline __float128 acos (__float128 x) throw() {return acosq(x);}
inline __float128 acosh (__float128 x) throw() {return acoshq(x);}
inline __float128 asin (__float128 x) throw() {return asinq(x);}
inline __float128 asinh (__float128 x) throw() {return asinhq(x);}
inline __float128 atan (__float128 x) throw() {return atanq(x);}
inline __float128 atanh (__float128 x) throw() {return atanhq(x);}
inline __float128 atan2 (__float128 y, __float128 x) throw() {return atan2q(y,x);}
inline __float128 cbrt (__float128 x) throw() {return cbrtq(x);}
inline __float128 ceil (__float128 x) throw() {return ceilq(x);}
inline __float128 copysign (__float128 y, __float128 x) throw() {return copysignq(y,x);}
inline __float128 cosh (__float128 x) throw() {return coshq(x);}
inline __float128 cos (__float128 x) throw() {return cosq(x);}
inline __float128 erf (__float128 x) throw() {return erfq(x);}
inline __float128 erfc (__float128 x) throw() {return erfcq(x);}
inline __float128 exp (__float128 x) throw() {return expq(x);}
inline __float128 expm1 (__float128 x) throw() {return expm1q(x);}
inline __float128 fabs (__float128 x) throw() {return fabsq(x);}
inline __float128 fdim (__float128 y, __float128 x) throw() {return fdimq(y,x);}
inline int isfinite (__float128 x) throw() {return finiteq(x);}
inline __float128 floor (__float128 x) throw() {return floorq(x);}
inline __float128 fma (__float128 a, __float128 b, __float128 c) throw() {return fmaq(a,b,c);}
inline __float128 fmax (__float128 y, __float128 x) throw() {return fmaxq(y,x);}
inline __float128 fmin (__float128 y, __float128 x) throw() {return fminq(y,x);}
inline __float128 fmod (__float128 y, __float128 x) throw() {return fmodq(y,x);}
inline __float128 frexp (__float128 y, int * x) throw() {return frexpq(y,x);}
inline __float128 hypot (__float128 y, __float128 x) throw() {return hypotq(y,x);}
inline int isinf (__float128 x) throw() {return isinfq(x);}
inline int ilogb (__float128 x) throw() {return ilogbq(x);}
inline int isnan (__float128 x) throw() {return isnanq(x);}
inline __float128 j0 (__float128 x) throw() {return j0q(x);}
inline __float128 j1 (__float128 x) throw() {return j1q(x);}
inline __float128 jn (int n, __float128 x) throw() {return jnq(n,x);}
inline __float128 ldexp (__float128 x, int n) throw() {return ldexpq(x,n);}
inline __float128 lgamma (__float128 x) throw() {return lgammaq(x);}
inline long long int llrint (__float128 x) throw() {return llrintq(x);}
inline long long int llround (__float128 x) throw() {return llroundq(x);}
inline __float128 log (__float128 x) throw() {return logq(x);}
inline __float128 log10 (__float128 x) throw() {return log10q(x);}
inline __float128 log2 (__float128 x) throw() {return log2q(x);}
inline __float128 log1p (__float128 x) throw() {return log1pq(x);}
inline long int lrint (__float128 x) throw() {return lrintq(x);}
inline long int lround (__float128 x) throw() {return lroundq(x);}
inline __float128 modf (__float128 y, __float128 * x) throw() {return modfq(y,x);}
inline __float128 nearbyint (__float128 x) throw() {return nearbyintq(x);}
inline __float128 nextafter (__float128 y, __float128 x) throw() {return nextafterq(y,x);}
inline __float128 pow (__float128 x, __float128 a) throw() {return powq(x,a);}
inline __float128 remainder (__float128 y, __float128 x) throw() {return remainderq(y,x);}
inline __float128 remquo (__float128 y, __float128 z, int * x) throw() {return remquoq(y,z,x);}
inline __float128 rint (__float128 x) throw() {return rintq(x);}
inline __float128 round (__float128 x) throw() {return roundq(x);}
inline __float128 scalbln (__float128 y, long int x) throw() {return scalblnq(y,x);}
inline __float128 scalbn (__float128 y, int x) throw() {return scalbnq(y,x);}
inline int signbit (__float128 x) throw() {return signbitq(x);}
inline void sincos (__float128 x, __float128 * s, __float128 * c) throw() {return sincosq(x,s,c);}
inline __float128 sinh (__float128 x) throw() {return sinhq(x);}
inline __float128 sin (__float128 x) throw() {return sinq(x);}
inline __float128 sqrt (__float128 x) throw() {return sqrtq(x);}
inline __float128 tan (__float128 x) throw() {return tanq(x);}
inline __float128 tanh (__float128 x) throw() {return tanhq(x);}
inline __float128 tgamma (__float128 x) throw() {return tgammaq(x);}
inline __float128 trunc (__float128 x) throw() {return truncq(x);}
inline __float128 y0 (__float128 x) throw() {return y0q(x);}
inline __float128 y1 (__float128 x) throw() {return y1q(x);}
inline __float128 yn (int n, __float128 x) throw() {return ynq(n,x);}
inline __float128 cabs (__complex128 z) throw() {return cabsq(z);}
inline __float128 carg (__complex128 z) throw() {return cargq(z);}
inline __float128 cimag (__complex128 z) throw() {return cimagq(z);}
inline __float128 creal (__complex128 z) throw() {return crealq(z);}
inline __complex128 cacos (__complex128 z) throw() {return cacosq(z);}
inline __complex128 cacosh (__complex128 z) throw() {return cacoshq(z);}
inline __complex128 casin (__complex128 z) throw() {return casinq(z);}
inline __complex128 casinh (__complex128 z) throw() {return casinhq(z);}
inline __complex128 catan (__complex128 z) throw() {return catanq(z);}
inline __complex128 catanh (__complex128 z) throw() {return catanhq(z);}
inline __complex128 ccos (__complex128 z) throw() {return ccosq(z);}
inline __complex128 ccosh (__complex128 z) throw() {return ccoshq(z);}
inline __complex128 cexp (__complex128 z) throw() {return cexpq(z);}
inline __complex128 cexpi (__float128 x) throw() {return cexpiq(x);}
inline __complex128 clog (__complex128 z) throw() {return clogq(z);}
inline __complex128 clog10 (__complex128 z) throw() {return clog10q(z);}
inline __complex128 conj (__complex128 z) throw() {return conjq(z);}
inline __complex128 cpow (__complex128 z, __complex128 w) throw() {return cpowq(z,w);}
inline __complex128 cproj (__complex128 z) throw() {return cprojq(z);}
inline __complex128 csin (__complex128 z) throw() {return csinq(z);}
inline __complex128 csinh (__complex128 z) throw() {return csinhq(z);}
inline __complex128 csqrt (__complex128 z) throw() {return csqrtq(z);}
inline __complex128 ctan (__complex128 z) throw() {return ctanq(z);}
inline __complex128 ctanh (__complex128 z) throw() {return ctanhq(z);}
}
#    endif
#  endif
#endif




#endif // JMA_MATH_H

