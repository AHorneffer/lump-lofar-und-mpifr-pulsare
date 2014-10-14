// MPIfR_Real16_t.cxx
// code to deal with conversion to a 16 bit real
//_HIST  DATE NAME PLACE INFO
// 2011 May 11  James M Anderson  --MPIfR  start
// 2012 Mar 10  JMA  --update for 128 bit integers, 128 bit reals



// Copyright (c) 2011, 2012, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



// INCLUDES
#include "MPIfR_Real16_t.h"


namespace {
union R32 {
    uint32_t u32;
    Real32_t r32;
};
union R64 {
    uint64_t u64;
    Real64_t r64;
};
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#  if (defined(__i386__) || defined(__x86_64__))
struct uint80_struct {
    uint64_t u64;
    uint16_t u16;
};
#  else
#    warning "Unknown extended double precision format, assuming Motorola"
struct uint80_struct {
    uint16_t u16_exponent;
    uint16_t u16_blank;
    uint64_t u64;
};
#  endif
union R80 {
    uint80_struct u80;
    Real80_t r80;
};
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
union R128 {
    uint128_t u128;
    Real128_t r128;
};
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
}



// set up a namespace area for stuff.
namespace MPIfR {

namespace DATA_TYPE {





// GLOBALS


// FUNCTIONS

Real16_t::
Real16_t(const Real32_t* const restrict fp) restrict throw()
{
    const uint_fast32_t u32 = *(reinterpret_cast<const uint32_t* const restrict>(fp));
    const uint_fast32_t u32_sign = u32 & UINT32_C(0x80000000);
    bits = uint16_t(u32_sign >> 16); // store the sign bit
    // deal with zero
    if((u32 & UINT32_C(0x7FFFFFFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast32_t u32_expo = u32 & UINT32_C(0x7F800000);
        const uint_fast32_t u32_mant = u32 & UINT32_C(0x007FFFFF);
        // deal with denormal in Real32_t
        if(u32_expo == 0) {
            // make this go to zero, so do nothing
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u32_expo == UINT32_C(0x7F800000)) {
            bits |= UINT16_C(0x7C00);
            if(u32_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                bits |= (u32_mant >> 13) | 0x1;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary16 format, then
            // unbiasing for the binary32 format            
            int_fast16_t R16_expo = int_fast16_t(u32_expo >> 23)
                + int_fast16_t(15-127);
            if(R16_expo > 0x1F) {
                // overflow, return +-Inf
                bits |= UINT16_C(0x7C00);
            }
            else if(R16_expo <= 0) {
                // undeflow
                if(R16_expo < -10) {
                    // too far gone, set to zero
                    // do nothing
                }
                else {
                    // add on the leading 1 bit
                    uint_fast16_t R16_mant((u32_mant | UINT32_C(0x00800000)) >> (13-R16_expo));
                    uint_fast16_t round(R16_mant & 0x1);
                    R16_mant >>= 1;
                    if((round)) {
                        ++R16_mant; // rounding up into exponent area is ok
                    }
                    bits |= R16_mant;
                }
            }
            else {
                R16_expo <<= 10;
                uint_fast16_t R16_mant(u32_mant >> 13);
                bits |= R16_expo | R16_mant;
                if((u32_mant & UINT32_C(0x00001000))) {
                    // need to round up, if it goes into exponent, this is ok
                    bits++;
                }
            }
        }
    }
    return;
}



Real16_t::
Real16_t(const Real64_t* const restrict fp) restrict throw()
{
    const uint_fast32_t u32((*(reinterpret_cast<const uint64_t* const restrict>(fp))) >> 32);
    const uint_fast32_t u32_sign = u32 & UINT32_C(0x80000000);
    bits = uint16_t(u32_sign >> 16); // store the sign bit
    // deal with zero
    if((u32 & UINT32_C(0x7FFFFFFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast32_t u32_expo = u32 & UINT32_C(0x7FF00000);
        const uint_fast32_t u32_mant = u32 & UINT32_C(0x000FFFFF);
        // deal with denormal in Real64_t
        if(u32_expo == 0) {
            // make this go to zero, so do nothing
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u32_expo == UINT32_C(0x7FF00000)) {
            bits |= UINT16_C(0x7C00);
            if(u32_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                bits |= (u32_mant >> 10) | 0x1;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary16 format, then
            // unbiasing for the binary64 format            
            int_fast16_t R16_expo = int_fast16_t(u32_expo >> 20)
                + int_fast16_t(15-1023);
            if(R16_expo > 0x1F) {
                // overflow, return +-Inf
                bits |= UINT16_C(0x7C00);
            }
            else if(R16_expo <= 0) {
                // undeflow
                if(R16_expo < -11) {
                    // too far gone, set to zero
                    // do nothing
                }
                else {
                    // add on the leading 1 bit
                    uint_fast16_t R16_mant((u32_mant | UINT32_C(0x00100000)) >> (10-R16_expo));
                    uint_fast16_t round(R16_mant & 0x1);
                    R16_mant >>= 1;
                    if((round)) {
                        ++R16_mant; // rounding up into exponent area is ok
                    }
                    bits |= R16_mant;
                }
            }
            else {
                R16_expo <<= 10;
                uint_fast16_t R16_mant(u32_mant >> 10);
                bits |= R16_expo | R16_mant;
                if((u32_mant & UINT32_C(0x00000200))) {
                    // need to round up, if it goes into exponent, this is ok
                    bits++;
                }
            }
        }
    }
    return;
}


#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
Real16_t::
Real16_t(const Real80_t* const restrict fp) restrict throw()
{
    const uint16_t* const restrict u16p = reinterpret_cast<const uint16_t* const restrict>(fp);
#  if (defined(__i386__) || defined(__x86_64__))
    // little endian, 80 bits
    const uint16_t u16_upper = u16p[4];
    const uint16_t u16_lower = u16p[3];
#  else
#    warning "Unknown extended double precision format, assuming Motorola"
    // big endian, 96 bits
    const uint16_t u16_upper = u16p[0];
    const uint16_t u16_lower = u16p[2];
#  endif
    const uint32_t u32 = uint_fast32_t(u16_upper) << 16 | u16_lower;
    const uint_fast32_t u32_sign = u32 & UINT32_C(0x80000000);
    bits = uint16_t(u32_sign >> 16); // store the sign bit
    // deal with zero
    if((u32 & UINT32_C(0x7FFFFFFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast32_t u32_expo = u32 & UINT32_C(0x7FFF0000);
        const uint_fast32_t u32_mant = u32 & UINT32_C(0x00007FFF);
        //const uint_fast32_t u32_one  = u32 & UINT32_C(0x00008000);
        // The 80 bit format has an explicit non-hidden bit mantissa position 63.
        // Since this will only be 0 when the number is denormal, and that
        // is too small to fit in a Real32_t, we can ignore it.
        // deal with denormal in Real32_t
        if(u32_expo == 0) {
            // make this go to zero, so do nothing
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u32_expo == UINT32_C(0x7FFF0000)) {
            bits |= UINT16_C(0x7C00);
            if(u32_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                bits |= (u32_mant >> 5) | 0x1;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary16 format, then
            // unbiasing for the binary64 format            
            int_fast16_t R16_expo = int_fast16_t(u32_expo >> 16)
                + int_fast16_t(15-16383);
            if(R16_expo > 0x1F) {
                // overflow, return +-Inf
                bits |= UINT16_C(0x7C00);
            }
            else if(R16_expo <= 0) {
                // undeflow
                if(R16_expo < -11) {
                    // too far gone, set to zero
                    // do nothing
                }
                else {
                    // add on the leading 1 bit
                    uint_fast16_t R16_mant((u32_mant | UINT32_C(0x00008000)) >> (10-R16_expo));
                    uint_fast16_t round(R16_mant & 0x1);
                    R16_mant >>= 1;
                    if((round)) {
                        ++R16_mant; // rounding up into exponent area is ok
                    }
                    bits |= R16_mant;
                }
            }
            else {
                R16_expo <<= 10;
                uint_fast16_t R16_mant(u32_mant >> 5);
                bits |= R16_expo | R16_mant;
                if((u32_mant & UINT32_C(0x00000010))) {
                    // need to round up, if it goes into exponent, this is ok
                    bits++;
                }
            }
        }
    }
    return;
}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
Real16_t::
Real16_t(const Real128_t* const restrict fp) restrict throw()
{
    const uint_fast32_t u32((*(reinterpret_cast<const uint128_t* const restrict>(fp))) >> 96);
    const uint_fast32_t u32_sign = u32 & UINT32_C(0x80000000);
    bits = uint16_t(u32_sign >> 16); // store the sign bit
    // deal with zero
    if((u32 & UINT32_C(0x7FFFFFFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast32_t u32_expo = u32 & UINT32_C(0x7FFF0000);
        const uint_fast32_t u32_mant = u32 & UINT32_C(0x0000FFFF);
        // deal with denormal in Real128_t
        if(u32_expo == 0) {
            // make this go to zero, so do nothing
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u32_expo == UINT32_C(0x7FFF0000)) {
            bits |= UINT16_C(0x7C00);
            if(u32_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                bits |= (u32_mant >> 6) | 0x1;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary16 format, then
            // unbiasing for the binary128 format            
            int_fast16_t R16_expo = int_fast16_t(u32_expo >> 16)
                + int_fast16_t(15-16383);
            if(R16_expo > 0x1F) {
                // overflow, return +-Inf
                bits |= UINT16_C(0x7C00);
            }
            else if(R16_expo <= 0) {
                // undeflow
                if(R16_expo < -11) {
                    // too far gone, set to zero
                    // do nothing
                }
                else {
                    // add on the leading 1 bit
                    uint_fast16_t R16_mant((u32_mant | UINT32_C(0x00010000)) >> (6-R16_expo));
                    uint_fast16_t round(R16_mant & 0x1);
                    R16_mant >>= 1;
                    if((round)) {
                        ++R16_mant; // rounding up into exponent area is ok
                    }
                    bits |= R16_mant;
                }
            }
            else {
                R16_expo <<= 10;
                uint_fast16_t R16_mant(u32_mant >> 6);
                bits |= R16_expo | R16_mant;
                if((u32_mant & UINT32_C(0x00000020))) {
                    // need to round up, if it goes into exponent, this is ok
                    bits++;
                }
            }
        }
    }
    return;
}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT


Real32_t Real16_t::
to_Real32_t() const restrict throw()
{
    uint_fast16_t u16_sign(bits & UINT16_C(0x8000));
    uint32_t u32 = uint_fast32_t(u16_sign) << 16;
    // deal with zero
    if((bits & UINT16_C(0x7FFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast16_t u16_expo = bits & UINT16_C(0x7C00);
              uint_fast16_t u16_mant = bits & UINT16_C(0x03FF);
        // deal with denormal
        if(u16_expo == 0) {
            // what is the exponent?
            // shift over until the leading bit is in the exponent area
            int_fast16_t expo = -1;
            do {
                expo++;
                u16_mant <<= 1;
            }
            while ((u16_mant & UINT16_C(0x0400)) == 0);
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(127-15) -expo;
            u32 |= (uint_fast32_t(R32_expo) << 23)
                | ((uint_fast32_t(u16_mant) & UINT16_C(0x03FF)) << 13);
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u16_expo == UINT16_C(0x7C00)) {
            u32 |= UINT32_C(0x7F800000);
            if(u16_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                u32 |= uint_fast32_t(u16_mant) << 13;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary32 format, then
            // unbiasing for the binary16 format            
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(127-15);
            u32 |= (uint_fast32_t(R32_expo) << 23)
                | (uint_fast32_t(u16_mant) << 13);
        }
    }
    union R32 u;
    u.u32=u32;
    return u.r32;
}

Real64_t Real16_t::
to_Real64_t() const restrict throw()
{
    uint_fast16_t u16_sign(bits & UINT16_C(0x8000));
    uint32_t u32 = uint_fast32_t(u16_sign) << 16;
    // deal with zero
    if((bits & UINT16_C(0x7FFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast16_t u16_expo = bits & UINT16_C(0x7C00);
              uint_fast16_t u16_mant = bits & UINT16_C(0x03FF);
        // deal with denormal
        if(u16_expo == 0) {
            // what is the exponent?
            // shift over until the leading bit is in the exponent area
            int_fast16_t expo = -1;
            do {
                expo++;
                u16_mant <<= 1;
            }
            while ((u16_mant & UINT16_C(0x0400)) == 0);
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(1023-15) -expo;
            u32 |= (uint_fast32_t(R32_expo) << 20)
                | ((uint_fast32_t(u16_mant) & UINT16_C(0x03FF)) << 10);
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u16_expo == UINT16_C(0x7C00)) {
            u32 |= UINT32_C(0x7FF00000);
            if(u16_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                u32 |= uint_fast32_t(u16_mant) << 10;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary64 format, then
            // unbiasing for the binary16 format            
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(1023-15);
            u32 |= (uint_fast32_t(R32_expo) << 20)
                | (uint_fast32_t(u16_mant) << 10);
        }
    }
    union R64 u;
    u.u64=uint64_t(u32)<<32;
    return u.r64;
}





#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
Real80_t Real16_t::
to_Real80_t() const restrict throw()
{
    uint_fast16_t u16_sign(bits & UINT16_C(0x8000));
    uint32_t u32 = uint_fast32_t(u16_sign) << 16;
    // deal with zero
    if((bits & UINT16_C(0x7FFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast16_t u16_expo = bits & UINT16_C(0x7C00);
              uint_fast16_t u16_mant = bits & UINT16_C(0x03FF);
        // deal with denormal
        if(u16_expo == 0) {
            // what is the exponent?
            // shift over until the leading bit is in the exponent area
            int_fast16_t expo = -1;
            do {
                expo++;
                u16_mant <<= 1;
            }
            while ((u16_mant & UINT16_C(0x0400)) == 0);
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(16383-15) -expo;
            u32 |= (uint_fast32_t(R32_expo) << 16)
                | ((uint_fast32_t(u16_mant) & UINT16_C(0x03FF)) << 5);
            // turn on the non-hidden bit
            u32 |= UINT32_C(0x8000);
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u16_expo == UINT16_C(0x7C00)) {
            u32 |= UINT32_C(0x7FFF0000);
            if(u16_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                u32 |= uint_fast32_t(u16_mant) << 5;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary80 format, then
            // unbiasing for the binary16 format            
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(16383-15);
            u32 |= (uint_fast32_t(R32_expo) << 16)
                | (uint_fast32_t(u16_mant) << 5);
            // turn on the non-hidden bit
            u32 |= UINT32_C(0x8000);
        }
    }
    uint16_t u16_upper = u32 >> 16;
    uint16_t u16_lower = u32;
    union R80 u;
#  if (defined(__i386__) || defined(__x86_64__))
    u.u80.u16 = u16_upper;
    u.u80.u64 = uint64_t(u16_lower) << 48;
#  else
#    warning "Unknown extended double precision format, assuming Motorola"
    u.u80.u16_exponent = u16_upper;
    u.u80.u16_blank = 0;
    u.u80.u64 = uint64_t(u16_lower) << 48;
#  endif
    return u.r80;
}
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
Real128_t Real16_t::
to_Real128_t() const restrict throw()
{
    uint_fast16_t u16_sign(bits & UINT16_C(0x8000));
    uint32_t u32 = uint_fast32_t(u16_sign) << 16;
    // deal with zero
    if((bits & UINT16_C(0x7FFF)) == 0) {
        // do nothing
    }
    else {
        const uint_fast16_t u16_expo = bits & UINT16_C(0x7C00);
              uint_fast16_t u16_mant = bits & UINT16_C(0x03FF);
        // deal with denormal
        if(u16_expo == 0) {
            // what is the exponent?
            // shift over until the leading bit is in the exponent area
            int_fast16_t expo = -1;
            do {
                expo++;
                u16_mant <<= 1;
            }
            while ((u16_mant & UINT16_C(0x0400)) == 0);
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(16383-15) -expo;
            u32 |= (uint_fast32_t(R32_expo) << 16)
                | ((uint_fast32_t(u16_mant) & UINT16_C(0x03FF)) << 6);
        }
        // deal with +-Inf or NaN, which have all exponent bits set
        else if(u16_expo == UINT16_C(0x7C00)) {
            u32 |= UINT32_C(0x7FFF0000);
            if(u16_mant == 0) {
                // +-Inf
                // do nothing more
            }
            else {
                // NaN, try to take as many of the bits as possible,
                // but make sure that the last bit is on, to keep it a NaN
                u32 |= uint_fast32_t(u16_mant) << 6;
            }
        }
        else {
            // normal number
            // convert the exponent, bias for the binary128 format, then
            // unbiasing for the binary16 format            
            int_fast16_t R32_expo = int_fast16_t(u16_expo >> 10)
                + int_fast16_t(16383-15);
            u32 |= (uint_fast32_t(R32_expo) << 16)
                | (uint_fast32_t(u16_mant) << 6);
        }
    }
    union R128 u;
    u.u128=uint128_t(u32)<<96;
    return u.r128;
}
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT




}  // end namespace DATA_TYPE

}  // end namespace MPIfR


// using namespace MPIfR::DATA_TYPE;



// void pp(const uint8_t* const p, const int NUM)
// {
//     for(int i=0; i < NUM; i++) {
//         printf("%2.2X ", p[i]);
//     }
//     printf("\n");
//     return;
// }

// void pr(const Real16_t& r16)
// {
//     Real32_t r32(r16);
//     Real64_t r64(r16);
//     Real80_t r80(r16);
//     Real128_t r128(r16);

//     printf("%E %E %LE %E\n", r32, r64, r80, double(r128-r64));
//     pp(reinterpret_cast<const uint8_t* const>(&r32),4);
//     pp(reinterpret_cast<const uint8_t* const>(&r64),8);
//     pp(reinterpret_cast<const uint8_t* const>(&r80),16);
//     pp(reinterpret_cast<const uint8_t* const>(&r128),16);
    
//     return;
// }


// int main(void)
// {
//     Real32_t r32 = 5.125f;
//     Real64_t r64 = 5.125;
//     Real80_t r80 = 5.125L;
//     Real128_t r128 = 5.125;
    
//     {Real16_t r16(r32); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r32),4); printf("\n");}
//     {Real16_t r16(r64); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r64),8); printf("\n");}
//     {Real16_t r16(r80); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r80),16); printf("\n");}
//     {Real16_t r16(r128); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r128),16); printf("\n");}
    
    
//    r32 = 0.03f;
//    r64 = 0.03;
//    r80 = 0.03L;
//    r128 = 0.03;
    
//     {Real16_t r16(r32); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r32),4); printf("\n");}
//     {Real16_t r16(r64); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r64),8); printf("\n");}
//     {Real16_t r16(r80); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r80),16); printf("\n");}
//     {Real16_t r16(r128); pr(r16); pp(reinterpret_cast<const uint8_t* const>(&r16),2); pp(reinterpret_cast<const uint8_t* const>(&r128),16); printf("\n");}
    
    
    
//     return 0;
// }




// using namespace MPIfR::DATA_TYPE;

// void pp(const uint16_t* const p)
// {
//     printf("    %4.4X,\n", p[0]);
//     return;
// }

// int main(void)
// {
//     for(uint_fast32_t u=0; u < 16; u++) {
//         int8_t i = int8_t(int8_t(u<<4)>>4);
//         Real32_t r(i);
//         Real16_t r16(r);
//         pp(reinterpret_cast<const uint16_t* const>(&r16));
//     }
//     return 0;
// }
