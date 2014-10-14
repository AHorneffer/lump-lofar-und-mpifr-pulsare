// MPIfR_Numerical_sampling_windows.cxx
// create the default window files for LuMP
//_HIST  DATE NAME PLACE INFO
// 2012 Mar 31  James M Anderson  --MPIfR  Start
// 2014 Aug 29  JMA  ---- comment out some debugging stuff



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



// INCLUDES
#define MPIFR_NUMERICAL_SAMPLING_WINDOWS_H_FILE 1
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
#include <stdint.h>
// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#include <time.h>

#include "JMA_math.h"
#include <stdio.h>
#include <stdlib.h>
#include "MPIfR_logging.h"
#include "MPIfR_Numerical_sampling_windows.h"



#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
typedef Real128_t MPIfR_Numerical_widest_Real_t;
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
#    define MPIfR_Numerical_widest_code q
#    define MPIfR_Numerical_widest_EPSILON FLT128_EPSILON
#    define MPIfR_Numerical_widest_MIN FLT128_MIN
#    define MPIfR_Numerical_widest_PI M_PIq
#  else
#    define MPIfR_Numerical_widest_code l
#    define MPIfR_Numerical_widest_EPSILON LDBL_EPSILON
#    define MPIfR_Numerical_widest_MIN LDBL_MIN
#    define MPIfR_Numerical_widest_PI M_PIl
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
typedef Real80_t MPIfR_Numerical_widest_Real_t;
#  define MPIfR_Numerical_widest_code l
#  define MPIfR_Numerical_widest_EPSILON LDBL_EPSILON
#  define MPIfR_Numerical_widest_MIN LDBL_MIN
#  define MPIfR_Numerical_widest_PI M_PIl
#else
typedef Real64_t MPIfR_Numerical_widest_Real_t;
#  define MPIfR_Numerical_widest_code
#  define MPIfR_Numerical_widest_EPSILON DBL_EPSILON
#  define MPIfR_Numerical_widest_MIN DBL_MIN
#  define MPIfR_Numerical_widest_PI M_PI
#endif


namespace MPIfR {

namespace Numerical {



namespace {
inline MPIfR_Numerical_widest_Real_t sinc(MPIfR_Numerical_widest_Real_t x)
{
    MPIfR_Numerical_widest_Real_t pi_x = MPIfR_Numerical_widest_PI * x;
    if(std::isfinite(x)) {
        if(x != 0.0) {
            return std::sin(pi_x)/pi_x;
        }
        return 1.0;
    }
    else if(std::isinf(x)) {
        return (x>0.0) ?
            +0.0
            : -0.0;
    }
    else {
        // must be a NaN
        return x;
    }
#ifdef NAN
    return NAN;
#else
    return 0.0;
#endif
}



MPIfR_Numerical_widest_Real_t jma_CF1_ik(MPIfR_Numerical_widest_Real_t x)
{
    MPIfR_Numerical_widest_Real_t C, D, f, a, b, delta;
    static const MPIfR_Numerical_widest_Real_t tolerance = 2.0 * MPIfR_Numerical_widest_EPSILON;
    static MPIfR_Numerical_widest_Real_t tiny = 0.0;
    static int initted = 0;
    if((initted)) {
    }
    else {
        tiny = std::sqrt(MPIfR_Numerical_widest_MIN);
    }
    C = f = tiny;
    D = 0.0;
    for(unsigned long k = 1; k < 1000; k++) {
        a = 1.0;
        b = 2.0 * k / x;
        C = b + a / C;
        D = b + a * D;
        if (C == 0.0) { C = tiny; }
        if (D == 0.0) { D = tiny; }
        D = 1.0 / D;
        delta = C * D;
        f *= delta;
        if (std::fabs(delta - 1.0) <= tolerance) 
        { 
            //printf("jma_CF1_ik stop at %lu\n", k);
            break;
        }
    }
    return f;    
}

void jma_temme_ik(MPIfR_Numerical_widest_Real_t x,
                  MPIfR_Numerical_widest_Real_t* Ku,
                  MPIfR_Numerical_widest_Real_t* Ku1)
{
    MPIfR_Numerical_widest_Real_t f, h, p, q, coef, sum, sum1;
    MPIfR_Numerical_widest_Real_t a, b, c, d, sigma, gamma1, gamma2;

    static const MPIfR_Numerical_widest_Real_t tolerance = MPIfR_Numerical_widest_EPSILON;

    MPIfR_Numerical_widest_Real_t gp = std::tgamma(1.0)-1.0;
    MPIfR_Numerical_widest_Real_t gm = gp;
    a = std::log(x*0.5);
    b = 1.0;
    sigma = 0.0;
    c = 1.0;
    d = 1.0;
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
    gamma1 = 0.577215664901532860606512090082402431042159335939923598805Q;
#  else
    gamma1 = 0.577215664901532860606512090082402431042159335939923598805L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    gamma1 = 0.577215664901532860606512090082402431042159335939923598805L;
#else
    gamma1 = 0.577215664901532860606512090082402431042159335939923598805;
#endif
    gamma2 = (2.0 + gp + gm) * c * 0.5;
    p = (gp + 1.0) / (2.0 * b);
    q = (1.0 + gm) * b / 2.0;
    f = (std::cosh(sigma) * gamma1 + d * (-a) * gamma2) / c;
    h = p;
    coef = 1.0;
    sum = coef * f;
    sum1 = coef * h;

    for(unsigned long k = 1; k < 1000; k++) {
        f = (k * f + p + q) / (k*k);
        p /= k;
        q /= k;
        h = p - k * f;
        coef *= x * x / (4.0 * k);
        sum += coef * f;
        sum1 += coef * h;
        if (std::fabs(coef * f) < std::fabs(sum) * tolerance) 
        { 
            //printf("jma_temme_ik stop at %lu\n", k);
            break; 
        }
    }
    *Ku = sum;
    *Ku1 = 2.0 * sum1 / x;
    return;
}
void jma_CF2_ik(MPIfR_Numerical_widest_Real_t x,
                MPIfR_Numerical_widest_Real_t* Ku,
                MPIfR_Numerical_widest_Real_t* Ku1)
{
    MPIfR_Numerical_widest_Real_t S, C, Q, D, f, a, b, q, delta, current, prev;

    static const MPIfR_Numerical_widest_Real_t tolerance = MPIfR_Numerical_widest_EPSILON;

    a = -0.25;
    b = 2.0
        * (x + 1.0); // b1
    D = 1.0 / b;     // D1 = 1 / b1
    f = delta = D;                                // f1 = delta1 = D1, coincidence
    prev = 0.0;      // q0
    current = 1.0;   // q1
    Q = C = -a;                                   // Q1 = C1 because q1 = 1
    S = 1.0 + Q * delta;// S1
    
    for (unsigned long k = 2; k < 1000; k++)     // starting from 2
    {
        // continued fraction f = z1 / z0
        a -= 2.0 * (k - 1.0);
        b += 2.0;
        D = 1.0 / (b + a * D);
        delta *= b * D - 1.0;
        f += delta;

        // series summation S = 1 + \sum_{n=1}^{\infty} C_n * z_n / z_0
        q = (prev - (b - 2.0) * current) / a;
        prev = current;
        current = q;                        // forward recurrence for q
        C *= -a / k;
        Q += C * q;
        S += Q * delta;

        // S converges slower than f
        if (std::fabs(Q * delta) < std::fabs(S) * tolerance) 
        { 
            //printf("jma_CF2_ik stop at %lu\n", k);
            break; 
        }
    }
    *Ku = std::sqrt(MPIfR_Numerical_widest_PI / (2.0 * x)) * std::exp(-x) / S;
    *Ku1 = *Ku * (0.5 + x + (-0.25) * f) / x;
    return;
}



MPIfR_Numerical_widest_Real_t jma_bessel_i0(MPIfR_Numerical_widest_Real_t x)
{
    MPIfR_Numerical_widest_Real_t Iv, Ku, Ku1, fv;
    MPIfR_Numerical_widest_Real_t W;
    if(x == 0.0) {
        return 1.0;
    }
    W = 1.0/x;
    if(x<= 2.0) {
        jma_temme_ik(x, &Ku, &Ku1);
    }
    else {
        jma_CF2_ik(x, &Ku, &Ku1);
    }
    fv = jma_CF1_ik(x);
    Iv = W / (Ku * fv + Ku1);
    return Iv;
}





MPIfR_Numerical_widest_Real_t get_actual_window_parameter(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window, const Real64_t window_parameter) throw()
{
    MPIfR_Numerical_widest_Real_t parameter(window_parameter);
    switch(window) {
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Tukey:
        if(window_parameter <= 0.0) {
            parameter = 0.5;
        }
        else if(window_parameter >= 1.0) {
            parameter = 1.0;
        }
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Gaussian:
        if(window_parameter <= 0.0) {
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
            parameter = 0.4Q;
#  else
            parameter = 0.4L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
            parameter = 0.4L;
#else
            parameter = 0.4;
#endif
        }
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman:
        if((window_parameter <= 0.0) || (window_parameter >= 1.0)) {
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
            parameter = 0.16Q;
#  else
            parameter = 0.16L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
            parameter = 0.16L;
#else
            parameter = 0.16;
#endif
        }
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Kaiser:
        if(window_parameter <= 0.0) {
            parameter = 3.0;
        }
        parameter *= MPIfR_Numerical_widest_PI;
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Rectangular:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hann:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hamming:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Cosine:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Lanczos:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Barlett0:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_BarlettN0:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Bartlett_Hann:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Nuttall:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Harris:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Nuttall:
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Flat_Top:
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_UNKNOWN:
    default:
        DEF_LOGGING_CRITICAL("Unknown MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM %d\n", int(window));
        exit(1);
    }
    return parameter;
}




MPIfR_Numerical_widest_Real_t get_window_value(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const MPIfR_Numerical_widest_Real_t parameter,
                                               const int_fast64_t N,
                                               const int_fast64_t n) throw()
{
    // For a window of type window, size N, at position n, return the window
    // weight value value
    static const MPIfR_Numerical_widest_Real_t M_2PI_ = 2.0 * MPIfR_Numerical_widest_PI;
    MPIfR_Numerical_widest_Real_t value = 0.0;
    MPIfR_Numerical_widest_Real_t fraction = MPIfR_Numerical_widest_Real_t(n)/(N-1);
    MPIfR_Numerical_widest_Real_t a0, a1, a2, a3, a4;
    switch(window) {
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Rectangular:
        value = 1.0;
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hann:
        value = MPIfR_Numerical_widest_Real_t(0.5)
            * (1.0 - std::cos(M_2PI_*fraction));
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Hamming:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        value = 0.54Q - 0.46Q * std::cos(M_2PI_*fraction);
#  else
        value = 0.54L - 0.46L * std::cos(M_2PI_*fraction);
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        value = 0.54L - 0.46L * std::cos(M_2PI_*fraction);
#else
        value = 0.54 - 0.46 * std::cos(M_2PI_*fraction);
#endif
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Tukey:
        a0 = parameter * (N-1);
        if(n<=a0) {
            value = n/a0-1.0;
            value = MPIfR_Numerical_widest_Real_t(0.5)
                + 0.5
                * std::cos(MPIfR_Numerical_widest_PI * value);
        }
        else if(N-1-n<=a0) {
            value = (n-(N-1))/a0+1.0;
        }
        else {
            value = 1.0;
            value = MPIfR_Numerical_widest_Real_t(0.5)
                + 0.5
                * std::cos(MPIfR_Numerical_widest_PI * value); 
        }
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Cosine:
        value = std::sin(MPIfR_Numerical_widest_PI * n/(N-1));
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Lanczos:
        value = sinc(MPIfR_Numerical_widest_Real_t(2.0) *n/(N-1)-1.0);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Barlett0:
        a0 = MPIfR_Numerical_widest_Real_t(0.5)
            * (N-1);
        value = (a0-std::fabs(n-a0))/a0;
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_BarlettN0:
        a0 = MPIfR_Numerical_widest_Real_t(0.5)
            * (N+1);
        value = (a0-std::fabs(n+1-a0))/a0;
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Gaussian:
        a0 = MPIfR_Numerical_widest_Real_t(0.5)
            * (N-1);
        a0 = (n-a0)/(parameter*a0);
        a0 = -0.5 * a0 * a0;
        value = std::exp(a0);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Bartlett_Hann:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        value = 0.62Q - 0.48Q * std::fabs(fraction-0.5)
            - 0.38Q * std::cos(M_2PI_*fraction);
#  else
        value = 0.62L - 0.48L * std::fabs(fraction-0.5)
            - 0.38L * std::cos(M_2PI_*fraction);
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        value = 0.62L - 0.48L * std::fabs(fraction-0.5)
            - 0.38L * std::cos(M_2PI_*fraction);
#else
        value = 0.62 - 0.48 * std::fabs(fraction-0.5)
            - 0.38 * std::cos(M_2PI_*fraction);
#endif
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman:
        a0 = 0.5 * (1.0-parameter);
        a1 = 0.5;
        a2 = 0.5 * parameter;
        value = a0 - a1*std::cos(M_2PI_*fraction)
            + a2*std::cos(M_2PI_*2.0*fraction);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Kaiser:
        a0 = 2.0*fraction-1.0;
        a1 = 1.0-a0*a0;
        a2 = std::sqrt(a1) * parameter;
        value = jma_bessel_i0(a2)/jma_bessel_i0(parameter);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Nuttall:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        a0 = 0.355768Q;
        a1 = 0.487396Q;
        a2 = 0.144232Q;
        a3 = 0.012604Q;
#  else
        a0 = 0.355768L;
        a1 = 0.487396L;
        a2 = 0.144232L;
        a3 = 0.012604L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        a0 = 0.355768L;
        a1 = 0.487396L;
        a2 = 0.144232L;
        a3 = 0.012604L;
#else
        a0 = 0.355768;
        a1 = 0.487396;
        a2 = 0.144232;
        a3 = 0.012604;
#endif
        value = a0
            - a1*std::cos(M_2PI_*fraction)
            + a2*std::cos(2.0*M_2PI_*fraction)
            - a3*std::cos(3.0*M_2PI_*fraction);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Harris:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        a0 = 0.35875Q;
        a1 = 0.48829Q;
        a2 = 0.14128Q;
        a3 = 0.01168Q;
#  else
        a0 = 0.35875L;
        a1 = 0.48829L;
        a2 = 0.14128L;
        a3 = 0.01168L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        a0 = 0.35875L;
        a1 = 0.48829L;
        a2 = 0.14128L;
        a3 = 0.01168L;
#else
        a0 = 0.35875;
        a1 = 0.48829;
        a2 = 0.14128;
        a3 = 0.01168;
#endif
        value = a0
            - a1*std::cos(M_2PI_*fraction)
            + a2*std::cos(2.0*M_2PI_*fraction)
            - a3*std::cos(3.0*M_2PI_*fraction);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Blackman_Nuttall:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        a0 = 0.3635819Q;
        a1 = 0.4891775Q;
        a2 = 0.1365995Q;
        a3 = 0.0106411Q;
#  else
        a0 = 0.3635819L;
        a1 = 0.4891775L;
        a2 = 0.1365995L;
        a3 = 0.0106411L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        a0 = 0.3635819L;
        a1 = 0.4891775L;
        a2 = 0.1365995L;
        a3 = 0.0106411L;
#else
        a0 = 0.3635819;
        a1 = 0.4891775;
        a2 = 0.1365995;
        a3 = 0.0106411;
#endif
        value = a0
            - a1*std::cos(M_2PI_*fraction)
            + a2*std::cos(2.0*M_2PI_*fraction)
            - a3*std::cos(3.0*M_2PI_*fraction);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_Flat_Top:
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
#  ifdef JMA_CODE_HAVE_QUAD_FLOAT
        a0 = 1.0Q;
        a1 = 1.93Q;
        a2 = 1.29Q;
        a3 = 0.388Q;
        a4 = 0.032Q;
#  else
        a0 = 1.0L;
        a1 = 1.93L;
        a2 = 1.29L;
        a3 = 0.388L;
        a4 = 0.032L;
#  endif
#elif defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        a0 = 1.0L;
        a1 = 1.93L;
        a2 = 1.29L;
        a3 = 0.388L;
        a4 = 0.032L;
#else
        a0 = 1.0;
        a1 = 1.93;
        a2 = 1.29;
        a3 = 0.388;
        a4 = 0.032;
#endif
        value = a0
            - a1*std::cos(M_2PI_*fraction)
            + a2*std::cos(2.0*M_2PI_*fraction)
            - a3*std::cos(3.0*M_2PI_*fraction)
            + a4*std::cos(4.0*M_2PI_*fraction);
        break;
    case MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_UNKNOWN:
    default:
        DEF_LOGGING_CRITICAL("Unknown MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM %d\n", int(window));
        exit(1);
    }
    return value;
}





MPIfR_Numerical_widest_Real_t get_polyphase_inpulse_value(const int_fast64_t N,
                                                          const int_fast64_t P,
                                                          const int_fast64_t np) throw()
{
    // For a polyphase filter, with block size N and P taps, and position np
    // along the full input data strip, calculte the impulse response multiplier
    // for the polyphase filter.  See _A Comparison of FFT and Polyphase
    // Channelizers_, S. Faint and W. Read, 2003, equation 14, page 10.
    // For case with even number of NP samples
    //
    // (np - NP/2 + 0.5) / N
    // = np/N - P/2 + 1/(2N)
    // = (np+0.5)/N - P/2
    //
    // For odd NP
    // (np-NP/2)/N
    // = np/N - P/2
    static const MPIfR_Numerical_widest_Real_t half(0.5);
    MPIfR_Numerical_widest_Real_t fraction = MPIfR_Numerical_widest_Real_t(1.0)/N;
    MPIfR_Numerical_widest_Real_t x;
    if(((N&0x1)==0) || ((P&0x1)==0)) {
        // NP is even
        x = (np + half) * fraction - P * half;
    }
    else {
        // NP odd
        x = np * fraction - P * half;
    }
    MPIfR_Numerical_widest_Real_t value = sinc(x) * fraction;
    return value;
}







// int main(void)
// {
//     char buf[128];
//     __float128 z = 6.28Q;
//     __float128 i1 = jma_bessel_i0(z);
//     int n = quadmath_snprintf(buf, sizeof(buf), "%.30QE", i1);
//     if((size_t)n < sizeof(buf)) {
//         printf("__float128 result 1.0 is %s\n", buf);
//     }
//     else {
//         printf("__float128 result could not be printed\n");
//     }
//     return 0;
// }




}  // end namespace



using namespace MPIfR::DATA_TYPE;











int_fast32_t generate_sampling_window_Real16_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real16_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples %d too small\n", int(N));
        return -1;
    }
    {
        size_t bytes = size_t(N)*sizeof(Real16_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast32_t n=0, m=0; n<N; n++) {
        MPIfR_Numerical_widest_Real_t value = get_window_value(window, parameter, N, n);
        Real16_t val(value);
        sum += value;
        sum_native += Real32_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][n] = val;
        }
    } // for n=0 to N
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast32_t m=0; m<2*N; m++) {
            data_pp[0][m] = Real16_t(Real32_t(data_pp[0][m])*scale_native);
        }
    }
    else {
        for(int_fast32_t n=0; n<N; n++) {
            data_pp[0][n] = Real16_t(Real32_t(data_pp[0][n])*scale_native);
        }
    }
    return 0;
}
int_fast32_t generate_sampling_window_Real32_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real32_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples %d too small\n", int(N));
        return -1;
    }
    {
        size_t bytes = size_t(N)*sizeof(Real32_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast32_t n=0, m=0; n<N; n++) {
        MPIfR_Numerical_widest_Real_t value = get_window_value(window, parameter, N, n);
        Real32_t val(value);
        sum += value;
        sum_native += val;
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][n] = val;
        }
    } // for n=0 to N
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast32_t m=0; m<2*N; m++) {
            data_pp[0][m] = Real32_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast32_t n=0; n<N; n++) {
            data_pp[0][n] = Real32_t(data_pp[0][n]*scale_native);
        }
    }
    return 0;
}
int_fast32_t generate_sampling_window_Real64_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real64_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples %d too small\n", int(N));
        return -1;
    }
    {
        size_t bytes = size_t(N)*sizeof(Real64_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast32_t n=0, m=0; n<N; n++) {
        MPIfR_Numerical_widest_Real_t value = get_window_value(window, parameter, N, n);
        Real64_t val(value);
        sum += value;
        sum_native += val;
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][n] = val;
        }
    } // for n=0 to N
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast32_t m=0; m<2*N; m++) {
            data_pp[0][m] = Real64_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast32_t n=0; n<N; n++) {
            data_pp[0][n] = Real64_t(data_pp[0][n]*scale_native);
        }
    }
    return 0;
}
#if defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
int_fast32_t generate_sampling_window_Real80_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                               const Real64_t window_parameter,
                                               const Real64_t scaling_muiltiplier,
                                               const int_fast32_t N,
                                               const bool complex_samples,
                                               Real80_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples %d too small\n", int(N));
        return -1;
    }
    {
        size_t bytes = size_t(N)*sizeof(Real80_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast32_t n=0, m=0; n<N; n++) {
        MPIfR_Numerical_widest_Real_t value = get_window_value(window, parameter, N, n);
        Real80_t val(value);
        sum += value;
        sum_native += val;
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][n] = val;
        }
    } // for n=0 to N
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast32_t m=0; m<2*N; m++) {
            data_pp[0][m] = Real80_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast32_t n=0; n<N; n++) {
            data_pp[0][n] = Real80_t(data_pp[0][n]*scale_native);
        }
    }
    return 0;
}
#endif
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
int_fast32_t generate_sampling_window_Real128_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                                const Real64_t window_parameter,
                                                const Real64_t scaling_muiltiplier,
                                                const int_fast32_t N,
                                                const bool complex_samples,
                                                Real128_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples %d too small\n", int(N));
        return -1;
    }
    {
        size_t bytes = size_t(N)*sizeof(Real128_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast32_t n=0, m=0; n<N; n++) {
        MPIfR_Numerical_widest_Real_t value = get_window_value(window, parameter, N, n);
        Real128_t val(value);
        sum += value;
        sum_native += val;
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][n] = val;
        }
    } // for n=0 to N
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast32_t m=0; m<2*N; m++) {
            data_pp[0][m] = Real128_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast32_t n=0; n<N; n++) {
            data_pp[0][n] = Real128_t(data_pp[0][n]*scale_native);
        }
    }
    return 0;
}
#endif











int_fast32_t
generate_polyphase_sampling_window_Real16_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real16_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples per block %d too small\n", int(N));
        return -1;
    }
    if(P<2) {
        DEF_LOGGING_ERROR("Number of taps %d too small\n", int(P));
        return -2;
    }
    const int_fast64_t NP = int_fast64_t(N)*P;
    {
        size_t bytes = size_t(N)*size_t(P)*sizeof(Real16_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast64_t np=0, m=0; np<NP; np++) {
        MPIfR_Numerical_widest_Real_t value =
            get_window_value(window, parameter, NP, np);
        MPIfR_Numerical_widest_Real_t poly_impulse =
            get_polyphase_inpulse_value(N, P, np);
        value *= poly_impulse;
        Real16_t val(value);
        sum += value;
        sum_native += Real32_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][np] = val;
        }
    } // for np=0 to NP
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Note that N is used here, not NP,
    // as the FFT is done over a block size of N.
    // Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast64_t m=0; m<2*NP; m++) {
            data_pp[0][m] = Real16_t(Real32_t(data_pp[0][m])*scale_native);
        }
    }
    else {
        for(int_fast64_t np=0; np<NP; np++) {
            data_pp[0][np] = Real16_t(Real32_t(data_pp[0][np])*scale_native);
        }
    }
    return 0;
}
int_fast32_t
generate_polyphase_sampling_window_Real32_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real32_t* * const data_pp)
{
    // DEF_LOGGING_DEBUG("JMA using window type '%s' with parameters %E %E", MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Str(window), window_parameter, scaling_muiltiplier);
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples per block %d too small\n", int(N));
        return -1;
    }
    if(P<2) {
        DEF_LOGGING_ERROR("Number of taps %d too small\n", int(P));
        return -2;
    }
    const int_fast64_t NP = int_fast64_t(N)*P;
    {
        size_t bytes = size_t(N)*size_t(P)*sizeof(Real32_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast64_t np=0, m=0; np<NP; np++) {
        MPIfR_Numerical_widest_Real_t value =
            get_window_value(window, parameter, NP, np);
        MPIfR_Numerical_widest_Real_t poly_impulse =
            get_polyphase_inpulse_value(N, P, np);
        //DEF_LOGGING_DEBUG("JMA window function position=%3d block=%2d value=%E impulse=%E", int(np), int(np/16), double(value), double(poly_impulse));
        value *= poly_impulse;
        Real32_t val(value);
        sum += value;
        sum_native += Real32_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][np] = val;
        }
    } // for np=0 to NP
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Note that N is used here, not NP,
    // as the FFT is done over a block size of N.
    // Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    // MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    // DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    // DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast64_t m=0; m<2*NP; m++) {
            data_pp[0][m] = Real32_t(data_pp[0][m]*scale_native);
            //data_pp[0][m] = Real32_t(1.0f);   DEBUG condition
        }
    }
    else {
        for(int_fast64_t np=0; np<NP; np++) {
            data_pp[0][np] = Real32_t(data_pp[0][np]*scale_native);
            //data_pp[0][np] = Real32_t(1.0f);   DEBUG condition
        }
    }
    return 0;
}
int_fast32_t
generate_polyphase_sampling_window_Real64_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real64_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples per block %d too small\n", int(N));
        return -1;
    }
    if(P<2) {
        DEF_LOGGING_ERROR("Number of taps %d too small\n", int(P));
        return -2;
    }
    const int_fast64_t NP = int_fast64_t(N)*P;
    {
        size_t bytes = size_t(N)*size_t(P)*sizeof(Real64_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast64_t np=0, m=0; np<NP; np++) {
        MPIfR_Numerical_widest_Real_t value =
            get_window_value(window, parameter, NP, np);
        MPIfR_Numerical_widest_Real_t poly_impulse =
            get_polyphase_inpulse_value(N, P, np);
        value *= poly_impulse;
        Real64_t val(value);
        sum += value;
        sum_native += Real64_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][np] = val;
        }
    } // for np=0 to NP
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Note that N is used here, not NP,
    // as the FFT is done over a block size of N.
    // Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast64_t m=0; m<2*NP; m++) {
            data_pp[0][m] = Real64_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast64_t np=0; np<NP; np++) {
            data_pp[0][np] = Real64_t(data_pp[0][np]*scale_native);
        }
    }
    return 0;
}
#if defined JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
int_fast32_t
generate_polyphase_sampling_window_Real80_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                            const Real64_t window_parameter,
                                            const Real64_t scaling_muiltiplier,
                                            const int_fast32_t N, // size of block
                                            const int_fast32_t P, // num taps
                                            const bool complex_samples,
                                            Real80_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples per block %d too small\n", int(N));
        return -1;
    }
    if(P<2) {
        DEF_LOGGING_ERROR("Number of taps %d too small\n", int(P));
        return -2;
    }
    const int_fast64_t NP = int_fast64_t(N)*P;
    {
        size_t bytes = size_t(N)*size_t(P)*sizeof(Real80_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast64_t np=0, m=0; np<NP; np++) {
        MPIfR_Numerical_widest_Real_t value =
            get_window_value(window, parameter, NP, np);
        MPIfR_Numerical_widest_Real_t poly_impulse =
            get_polyphase_inpulse_value(N, P, np);
        value *= poly_impulse;
        Real80_t val(value);
        sum += value;
        sum_native += Real80_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][np] = val;
        }
    } // for np=0 to NP
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Note that N is used here, not NP,
    // as the FFT is done over a block size of N.
    // Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast64_t m=0; m<2*NP; m++) {
            data_pp[0][m] = Real80_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast64_t np=0; np<NP; np++) {
            data_pp[0][np] = Real80_t(data_pp[0][np]*scale_native);
        }
    }
    return 0;
}
#endif
#if defined JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
int_fast32_t
generate_polyphase_sampling_window_Real128_t(const MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window,
                                             const Real64_t window_parameter,
                                             const Real64_t scaling_muiltiplier,
                                             const int_fast32_t N, // size of block
                                             const int_fast32_t P, // num taps
                                             const bool complex_samples,
                                             Real128_t* * const data_pp)
{
    if(N<2) {
        DEF_LOGGING_ERROR("Number of samples per block %d too small\n", int(N));
        return -1;
    }
    if(P<2) {
        DEF_LOGGING_ERROR("Number of taps %d too small\n", int(P));
        return -2;
    }
    const int_fast64_t NP = int_fast64_t(N)*P;
    {
        size_t bytes = size_t(N)*size_t(P)*sizeof(Real128_t);
        if((complex_samples)) {
            bytes *= 2;
        }
        int retval = posix_memalign(reinterpret_cast<void**>(data_pp), MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, bytes);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for window with retval %d\n", (unsigned long long)(bytes), retval);
            return -2;
        }

    }
    MPIfR_Numerical_widest_Real_t parameter = get_actual_window_parameter(window,window_parameter);
    MPIfR_Numerical_widest_Real_t sum = 0.0;
    MPIfR_Numerical_widest_Real_t sum_native = 0.0;
    for(int_fast64_t np=0, m=0; np<NP; np++) {
        MPIfR_Numerical_widest_Real_t value =
            get_window_value(window, parameter, NP, np);
        MPIfR_Numerical_widest_Real_t poly_impulse =
            get_polyphase_inpulse_value(N, P, np);
        value *= poly_impulse;
        Real128_t val(value);
        sum += value;
        sum_native += Real128_t(val);
        if(complex_samples) {
            data_pp[0][m++] = val;
            data_pp[0][m++] = val;
        }
        else {
            data_pp[0][np] = val;
        }
    } // for np=0 to NP
    // The average weight should be 1.0/N exactly, in order to
    // not change the amplitude level of the individual frequencies
    // aftr coming out of FFTW3.  Note that N is used here, not NP,
    // as the FFT is done over a block size of N.
    // Find the scaling factor that makes this
    // come true.  So 1/N = mean_wt = scale (sum/N), or scale = 1/sum
    // Combine this with the scaling_muiltiplier parameter input to the function,
    // and scale = scaling_muiltiplier/sum
    MPIfR_Numerical_widest_Real_t scale = scaling_muiltiplier/sum;
    MPIfR_Numerical_widest_Real_t scale_native = scaling_muiltiplier/sum_native;
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor is %E\n", int(window), int(N), double(scale_native));
    DEF_LOGGING_DEBUG("For windowing function %d, length %d, scaling_factor_widest is %E\n", int(window), int(N), double(scale));
    if(complex_samples) {
        for(int_fast64_t m=0; m<2*NP; m++) {
            data_pp[0][m] = Real128_t(data_pp[0][m]*scale_native);
        }
    }
    else {
        for(int_fast64_t np=0; np<NP; np++) {
            data_pp[0][np] = Real128_t(data_pp[0][np]*scale_native);
        }
    }
    return 0;
}
#endif










    

const char* const restrict MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Str(MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM e) throw()
{
    switch(e) {
#define MPIfR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Array
#undef MPIfR_ENUM_VALS
    default:
        return "Unknown MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM";
    }
    return NULL;
}





}  // end namespace Numerical
}  // end namespace MPIfR




