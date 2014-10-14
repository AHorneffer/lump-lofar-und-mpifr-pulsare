// LOFAR_Station_Beamformed_VDIF_Packet.cxx
// VDIF packets for LOFAR beamformed data base class
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 08  James M Anderson  --- GFZ start


// Copyright (c) 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_VDIF_Packet.h"
#include "MPIfR_logging.h"
#include <string.h>


// set up a namespace area for stuff.
namespace {




// GLOBALS

static const uint_fast16_t VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE = 256;
const uint64_t VDIF_EPOCH_UNIX_TIMESTAMPS[VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE] = {
    0x386D4380, //        946684800   2000-01-01
    0x395D3480, //        962409600   2000-07-01
    0x3A4FC880, //        978307200   2001-01-01
    0x3B3E6800, //        993945600   2001-07-01
    0x3C30FC00, //        1009843200   2002-01-01
    0x3D1F9B80, //        1025481600   2002-07-01
    0x3E122F80, //        1041379200   2003-01-01
    0x3F00CF00, //        1057017600   2003-07-01
    0x3FF36300, //        1072915200   2004-01-01
    0x40E35400, //        1088640000   2004-07-01
    0x41D5E800, //        1104537600   2005-01-01
    0x42C48780, //        1120176000   2005-07-01
    0x43B71B80, //        1136073600   2006-01-01
    0x44A5BB00, //        1151712000   2006-07-01
    0x45984F00, //        1167609600   2007-01-01
    0x4686EE80, //        1183248000   2007-07-01
    0x47798280, //        1199145600   2008-01-01
    0x48697380, //        1214870400   2008-07-01
    0x495C0780, //        1230768000   2009-01-01
    0x4A4AA700, //        1246406400   2009-07-01
    0x4B3D3B00, //        1262304000   2010-01-01
    0x4C2BDA80, //        1277942400   2010-07-01
    0x4D1E6E80, //        1293840000   2011-01-01
    0x4E0D0E00, //        1309478400   2011-07-01
    0x4EFFA200, //        1325376000   2012-01-01
    0x4FEF9300, //        1341100800   2012-07-01
    0x50E22700, //        1356998400   2013-01-01
    0x51D0C680, //        1372636800   2013-07-01
    0x52C35A80, //        1388534400   2014-01-01
    0x53B1FA00, //        1404172800   2014-07-01
    0x54A48E00, //        1420070400   2015-01-01
    0x55932D80, //        1435708800   2015-07-01
    0x5685C180, //        1451606400   2016-01-01
    0x5775B280, //        1467331200   2016-07-01
    0x58684680, //        1483228800   2017-01-01
    0x5956E600, //        1498867200   2017-07-01
    0x5A497A00, //        1514764800   2018-01-01
    0x5B381980, //        1530403200   2018-07-01
    0x5C2AAD80, //        1546300800   2019-01-01
    0x5D194D00, //        1561939200   2019-07-01
    0x5E0BE100, //        1577836800   2020-01-01
    0x5EFBD200, //        1593561600   2020-07-01
    0x5FEE6600, //        1609459200   2021-01-01
    0x60DD0580, //        1625097600   2021-07-01
    0x61CF9980, //        1640995200   2022-01-01
    0x62BE3900, //        1656633600   2022-07-01
    0x63B0CD00, //        1672531200   2023-01-01
    0x649F6C80, //        1688169600   2023-07-01
    0x65920080, //        1704067200   2024-01-01
    0x6681F180, //        1719792000   2024-07-01
    0x67748580, //        1735689600   2025-01-01
    0x68632500, //        1751328000   2025-07-01
    0x6955B900, //        1767225600   2026-01-01
    0x6A445880, //        1782864000   2026-07-01
    0x6B36EC80, //        1798761600   2027-01-01
    0x6C258C00, //        1814400000   2027-07-01
    0x6D182000, //        1830297600   2028-01-01
    0x6E081100, //        1846022400   2028-07-01
    0x6EFAA500, //        1861920000   2029-01-01
    0x6FE94480, //        1877558400   2029-07-01
    0x70DBD880, //        1893456000   2030-01-01
    0x71CA7800, //        1909094400   2030-07-01
    0x72BD0C00, //        1924992000   2031-01-01
    0x73ABAB80, //        1940630400   2031-07-01
    0x749E3F80, //        1956528000   2032-01-01
    0x758E3080, //        1972252800   2032-07-01
    0x7680C480, //        1988150400   2033-01-01
    0x776F6400, //        2003788800   2033-07-01
    0x7861F800, //        2019686400   2034-01-01
    0x79509780, //        2035324800   2034-07-01
    0x7A432B80, //        2051222400   2035-01-01
    0x7B31CB00, //        2066860800   2035-07-01
    0x7C245F00, //        2082758400   2036-01-01
    0x7D145000, //        2098483200   2036-07-01
    0x7E06E400, //        2114380800   2037-01-01
    0x7EF58380, //        2130019200   2037-07-01
    0x7FE81780, //        2145916800   2038-01-01
    0x80D6B700, //        2161555200   2038-07-01
    0x81C94B00, //        2177452800   2039-01-01
    0x82B7EA80, //        2193091200   2039-07-01
    0x83AA7E80, //        2208988800   2040-01-01
    0x849A6F80, //        2224713600   2040-07-01
    0x858D0380, //        2240611200   2041-01-01
    0x867BA300, //        2256249600   2041-07-01
    0x876E3700, //        2272147200   2042-01-01
    0x885CD680, //        2287785600   2042-07-01
    0x894F6A80, //        2303683200   2043-01-01
    0x8A3E0A00, //        2319321600   2043-07-01
    0x8B309E00, //        2335219200   2044-01-01
    0x8C208F00, //        2350944000   2044-07-01
    0x8D132300, //        2366841600   2045-01-01
    0x8E01C280, //        2382480000   2045-07-01
    0x8EF45680, //        2398377600   2046-01-01
    0x8FE2F600, //        2414016000   2046-07-01
    0x90D58A00, //        2429913600   2047-01-01
    0x91C42980, //        2445552000   2047-07-01
    0x92B6BD80, //        2461449600   2048-01-01
    0x93A6AE80, //        2477174400   2048-07-01
    0x94994280, //        2493072000   2049-01-01
    0x9587E200, //        2508710400   2049-07-01
    0x967A7600, //        2524608000   2050-01-01
    0x97691580, //        2540246400   2050-07-01
    0x985BA980, //        2556144000   2051-01-01
    0x994A4900, //        2571782400   2051-07-01
    0x9A3CDD00, //        2587680000   2052-01-01
    0x9B2CCE00, //        2603404800   2052-07-01
    0x9C1F6200, //        2619302400   2053-01-01
    0x9D0E0180, //        2634940800   2053-07-01
    0x9E009580, //        2650838400   2054-01-01
    0x9EEF3500, //        2666476800   2054-07-01
    0x9FE1C900, //        2682374400   2055-01-01
    0xA0D06880, //        2698012800   2055-07-01
    0xA1C2FC80, //        2713910400   2056-01-01
    0xA2B2ED80, //        2729635200   2056-07-01
    0xA3A58180, //        2745532800   2057-01-01
    0xA4942100, //        2761171200   2057-07-01
    0xA586B500, //        2777068800   2058-01-01
    0xA6755480, //        2792707200   2058-07-01
    0xA767E880, //        2808604800   2059-01-01
    0xA8568800, //        2824243200   2059-07-01
    0xA9491C00, //        2840140800   2060-01-01
    0xAA390D00, //        2855865600   2060-07-01
    0xAB2BA100, //        2871763200   2061-01-01
    0xAC1A4080, //        2887401600   2061-07-01
    0xAD0CD480, //        2903299200   2062-01-01
    0xADFB7400, //        2918937600   2062-07-01
    0xAEEE0800, //        2934835200   2063-01-01
    0xAFDCA780, //        2950473600   2063-07-01
    0xB0CF3B80, //        2966371200   2064-01-01
    0xB1BF2C80, //        2982096000   2064-07-01
    0xB2B1C080, //        2997993600   2065-01-01
    0xB3A06000, //        3013632000   2065-07-01
    0xB492F400, //        3029529600   2066-01-01
    0xB5819380, //        3045168000   2066-07-01
    0xB6742780, //        3061065600   2067-01-01
    0xB762C700, //        3076704000   2067-07-01
    0xB8555B00, //        3092601600   2068-01-01
    0xB9454C00, //        3108326400   2068-07-01
    0xBA37E000, //        3124224000   2069-01-01
    0xBB267F80, //        3139862400   2069-07-01
    0xBC191380, //        3155760000   2070-01-01
    0xBD07B300, //        3171398400   2070-07-01
    0xBDFA4700, //        3187296000   2071-01-01
    0xBEE8E680, //        3202934400   2071-07-01
    0xBFDB7A80, //        3218832000   2072-01-01
    0xC0CB6B80, //        3234556800   2072-07-01
    0xC1BDFF80, //        3250454400   2073-01-01
    0xC2AC9F00, //        3266092800   2073-07-01
    0xC39F3300, //        3281990400   2074-01-01
    0xC48DD280, //        3297628800   2074-07-01
    0xC5806680, //        3313526400   2075-01-01
    0xC66F0600, //        3329164800   2075-07-01
    0xC7619A00, //        3345062400   2076-01-01
    0xC8518B00, //        3360787200   2076-07-01
    0xC9441F00, //        3376684800   2077-01-01
    0xCA32BE80, //        3392323200   2077-07-01
    0xCB255280, //        3408220800   2078-01-01
    0xCC13F200, //        3423859200   2078-07-01
    0xCD068600, //        3439756800   2079-01-01
    0xCDF52580, //        3455395200   2079-07-01
    0xCEE7B980, //        3471292800   2080-01-01
    0xCFD7AA80, //        3487017600   2080-07-01
    0xD0CA3E80, //        3502915200   2081-01-01
    0xD1B8DE00, //        3518553600   2081-07-01
    0xD2AB7200, //        3534451200   2082-01-01
    0xD39A1180, //        3550089600   2082-07-01
    0xD48CA580, //        3565987200   2083-01-01
    0xD57B4500, //        3581625600   2083-07-01
    0xD66DD900, //        3597523200   2084-01-01
    0xD75DCA00, //        3613248000   2084-07-01
    0xD8505E00, //        3629145600   2085-01-01
    0xD93EFD80, //        3644784000   2085-07-01
    0xDA319180, //        3660681600   2086-01-01
    0xDB203100, //        3676320000   2086-07-01
    0xDC12C500, //        3692217600   2087-01-01
    0xDD016480, //        3707856000   2087-07-01
    0xDDF3F880, //        3723753600   2088-01-01
    0xDEE3E980, //        3739478400   2088-07-01
    0xDFD67D80, //        3755376000   2089-01-01
    0xE0C51D00, //        3771014400   2089-07-01
    0xE1B7B100, //        3786912000   2090-01-01
    0xE2A65080, //        3802550400   2090-07-01
    0xE398E480, //        3818448000   2091-01-01
    0xE4878400, //        3834086400   2091-07-01
    0xE57A1800, //        3849984000   2092-01-01
    0xE66A0900, //        3865708800   2092-07-01
    0xE75C9D00, //        3881606400   2093-01-01
    0xE84B3C80, //        3897244800   2093-07-01
    0xE93DD080, //        3913142400   2094-01-01
    0xEA2C7000, //        3928780800   2094-07-01
    0xEB1F0400, //        3944678400   2095-01-01
    0xEC0DA380, //        3960316800   2095-07-01
    0xED003780, //        3976214400   2096-01-01
    0xEDF02880, //        3991939200   2096-07-01
    0xEEE2BC80, //        4007836800   2097-01-01
    0xEFD15C00, //        4023475200   2097-07-01
    0xF0C3F000, //        4039372800   2098-01-01
    0xF1B28F80, //        4055011200   2098-07-01
    0xF2A52380, //        4070908800   2099-01-01
    0xF393C300, //        4086547200   2099-07-01
    0xF4865700, //        4102444800   2100-01-01
    0xF574F680, //        4118083200   2100-07-01
    0xF6678A80, //        4133980800   2101-01-01
    0xF7562A00, //        4149619200   2101-07-01
    0xF848BE00, //        4165516800   2102-01-01
    0xF9375D80, //        4181155200   2102-07-01
    0xFA29F180, //        4197052800   2103-01-01
    0xFB189100, //        4212691200   2103-07-01
    0xFC0B2500, //        4228588800   2104-01-01
    0xFCFB1600, //        4244313600   2104-07-01
    0xFDEDAA00, //        4260211200   2105-01-01
    0xFEDC4980, //        4275849600   2105-07-01
    0xFFCEDD80, //        4291747200   2106-01-01
    0x100BD7D00, //        4307385600   2106-07-01
    0x101B01100, //        4323283200   2107-01-01
    0x1029EB080, //        4338921600   2107-07-01
    0x103914480, //        4354819200   2108-01-01
    0x104813580, //        4370544000   2108-07-01
    0x10573C980, //        4386441600   2109-01-01
    0x106626900, //        4402080000   2109-07-01
    0x10754FD00, //        4417977600   2110-01-01
    0x108439C80, //        4433616000   2110-07-01
    0x109363080, //        4449513600   2111-01-01
    0x10A24D000, //        4465152000   2111-07-01
    0x10B176400, //        4481049600   2112-01-01
    0x10C075500, //        4496774400   2112-07-01
    0x10CF9E900, //        4512672000   2113-01-01
    0x10DE88880, //        4528310400   2113-07-01
    0x10EDB1C80, //        4544208000   2114-01-01
    0x10FC9BC00, //        4559846400   2114-07-01
    0x110BC5000, //        4575744000   2115-01-01
    0x111AAEF80, //        4591382400   2115-07-01
    0x1129D8380, //        4607280000   2116-01-01
    0x1138D7480, //        4623004800   2116-07-01
    0x114800880, //        4638902400   2117-01-01
    0x1156EA800, //        4654540800   2117-07-01
    0x116613C00, //        4670438400   2118-01-01
    0x1174FDB80, //        4686076800   2118-07-01
    0x118426F80, //        4701974400   2119-01-01
    0x119310F00, //        4717612800   2119-07-01
    0x11A23A300, //        4733510400   2120-01-01
    0x11B139400, //        4749235200   2120-07-01
    0x11C062800, //        4765132800   2121-01-01
    0x11CF4C780, //        4780771200   2121-07-01
    0x11DE75B80, //        4796668800   2122-01-01
    0x11ED5FB00, //        4812307200   2122-07-01
    0x11FC88F00, //        4828204800   2123-01-01
    0x120B72E80, //        4843843200   2123-07-01
    0x121A9C280, //        4859740800   2124-01-01
    0x12299B380, //        4875465600   2124-07-01
    0x1238C4780, //        4891363200   2125-01-01
    0x1247AE700, //        4907001600   2125-07-01
    0x1256D7B00, //        4922899200   2126-01-01
    0x1265C1A80, //        4938537600   2126-07-01
    0x1274EAE80, //        4954435200   2127-01-01
    0x1283D4E00, //        4970073600   2127-07-01
};    


// FUNCTIONS



}  // end namespace


namespace MPIfR {

namespace LOFAR {

namespace Station {







LOFAR_VDIF_Packet::
LOFAR_VDIF_Packet(const size_t FRAME_SIZE_BYTES, bool legacy_mode_on)
    :
frame_buffer(0),
#if __BYTE_ORDER == __LITTLE_ENDIAN
    header(0),
#else
    real_header(0),
#endif
    data_buffer(0),
    FRAME_BUFFER_SIZE_BYTES(FRAME_SIZE_BYTES),
    DATA_AREA_SIZE_BYTES((legacy_mode_on) ?
                         FRAME_SIZE_BYTES-LEGACY_FRAME_HEADER_SIZE_BYTES
                         : FRAME_SIZE_BYTES-NORMAL_FRAME_HEADER_SIZE_BYTES),
    delete_frame_buffer_on_destruction(true)
{
    if(legacy_mode_on)
    {
        if(FRAME_BUFFER_SIZE_BYTES<=LEGACY_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    }
    else {
        if(FRAME_BUFFER_SIZE_BYTES<=NORMAL_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    }
    if((FRAME_BUFFER_SIZE_BYTES & 0x7)) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if(FRAME_BUFFER_SIZE_BYTES > UINT32_C(0x7FFFFF8)) {
        // 0xFFFFFF << 3 == 0x7FFFFF8
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    
    int retval = posix_memalign(&frame_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, FRAME_BUFFER_SIZE_BYTES);
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for frame_buffer with retval %d\n", (unsigned long long)(FRAME_BUFFER_SIZE_BYTES), retval);
        throw(LOFAR_VDIF_Packet_exception_no_memory());
    }

    // Zero the header area
    memset(frame_buffer, 0, FRAME_BUFFER_SIZE_BYTES-DATA_AREA_SIZE_BYTES);
    // Zero the last 8 bytes of the memory, to enforce the requirement that
    // it is filled with 0 bits.
    memset(static_cast<void*>(static_cast<char*>(frame_buffer)+8), 0, 8);

    setup_internal_pointers();

    // Set general paramters that we currently know.  Start with the frame size
    // in units of 8 bytes
    set_frame_length_BYTES(FRAME_BUFFER_SIZE_BYTES);
    if(legacy_mode_on) {
        set_legacy_mode_on();
    }
    set_VDIF_version(CURRENT_VDIF_OUTPUT_VERSION);
    
    return;
}


LOFAR_VDIF_Packet::
LOFAR_VDIF_Packet(const void* frame)
    :
        frame_buffer(0),
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header(0),
#else
        real_header(0),
#endif
        data_buffer(0),
        FRAME_BUFFER_SIZE_BYTES(get_frame_size_in_BYTES_from_void_ptr(frame)),
        delete_frame_buffer_on_destruction(true)
{
    if(FRAME_BUFFER_SIZE_BYTES<=LEGACY_FRAME_HEADER_SIZE_BYTES) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if((FRAME_BUFFER_SIZE_BYTES & 0x7)) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if(FRAME_BUFFER_SIZE_BYTES > UINT32_C(0x7FFFFF8)) {
        // 0xFFFFFF << 3 == 0x7FFFFF8
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    
    int retval = posix_memalign(&frame_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, FRAME_BUFFER_SIZE_BYTES);
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for frame_buffer with retval %d\n", (unsigned long long)(FRAME_BUFFER_SIZE_BYTES), retval);
        throw(LOFAR_VDIF_Packet_exception_no_memory());
    }
    memcpy(frame_buffer, frame, FRAME_BUFFER_SIZE_BYTES);

    setup_internal_pointers();

    init_from_allocated_memory();
    
    if(!legacy_mode_effective()) {
        if(FRAME_BUFFER_SIZE_BYTES<=NORMAL_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    }

    return;
}



LOFAR_VDIF_Packet::
LOFAR_VDIF_Packet(void* frame, bool use_provided_pointer, bool delete_provided_pointer_on_destruction)
    :
        frame_buffer(frame),
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header(0),
#else
        real_header(0),
#endif
        data_buffer(0),
        FRAME_BUFFER_SIZE_BYTES(get_frame_size_in_BYTES_from_void_ptr(frame)),
        delete_frame_buffer_on_destruction(delete_provided_pointer_on_destruction)
{
    if(FRAME_BUFFER_SIZE_BYTES<=LEGACY_FRAME_HEADER_SIZE_BYTES) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if((FRAME_BUFFER_SIZE_BYTES & 0x7)) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if(FRAME_BUFFER_SIZE_BYTES > UINT32_C(0x7FFFFF8)) {
        // 0xFFFFFF << 3 == 0x7FFFFF8
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    
    if(!use_provided_pointer) {
        int retval = posix_memalign(&frame_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, FRAME_BUFFER_SIZE_BYTES);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for frame_buffer with retval %d\n", (unsigned long long)(FRAME_BUFFER_SIZE_BYTES), retval);
            throw(LOFAR_VDIF_Packet_exception_no_memory());
        }
        memcpy(frame_buffer, frame, FRAME_BUFFER_SIZE_BYTES);
    }

    setup_internal_pointers();

    init_from_allocated_memory();
    
    if(!legacy_mode_effective()) {
        if(FRAME_BUFFER_SIZE_BYTES<=NORMAL_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    }

    return;
}


// copy constructor
LOFAR_VDIF_Packet::
LOFAR_VDIF_Packet(const LOFAR_VDIF_Packet& a)
    :
        frame_buffer(0),
#if __BYTE_ORDER == __LITTLE_ENDIAN
        header(0),
#else
        real_header(0),
#endif
        data_buffer(0),
        FRAME_BUFFER_SIZE_BYTES(a.frame_length_BYTES()),
        DATA_AREA_SIZE_BYTES(a.frame_data_area_length_BYTES()),
        delete_frame_buffer_on_destruction(true)
{
    if(FRAME_BUFFER_SIZE_BYTES<=LEGACY_FRAME_HEADER_SIZE_BYTES) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if((FRAME_BUFFER_SIZE_BYTES & 0x7)) {
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    if(FRAME_BUFFER_SIZE_BYTES > UINT32_C(0x7FFFFF8)) {
        // 0xFFFFFF << 3 == 0x7FFFFF8
        throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
    }
    
    int retval = posix_memalign(&frame_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, FRAME_BUFFER_SIZE_BYTES);
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for frame_buffer with retval %d\n", (unsigned long long)(FRAME_BUFFER_SIZE_BYTES), retval);
        throw(LOFAR_VDIF_Packet_exception_no_memory());
    }
    memcpy(frame_buffer, a.frame_pointer_const(), FRAME_BUFFER_SIZE_BYTES);

    setup_internal_pointers();

    init_from_allocated_memory();
    
    if(!legacy_mode_effective()) {
        if(FRAME_BUFFER_SIZE_BYTES<=NORMAL_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    }

    return;
}

// assignment operator
LOFAR_VDIF_Packet& LOFAR_VDIF_Packet::
operator=(const LOFAR_VDIF_Packet& a)
{
    if(this != &a) {
        deallocate_memory();
        FRAME_BUFFER_SIZE_BYTES = a.frame_length_BYTES();
        DATA_AREA_SIZE_BYTES = a.frame_data_area_length_BYTES();
        delete_frame_buffer_on_destruction = true;

        if(FRAME_BUFFER_SIZE_BYTES<=LEGACY_FRAME_HEADER_SIZE_BYTES) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
        if((FRAME_BUFFER_SIZE_BYTES & 0x7)) {
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
        if(FRAME_BUFFER_SIZE_BYTES > UINT32_C(0x7FFFFF8)) {
            // 0xFFFFFF << 3 == 0x7FFFFF8
            throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
        }
    
        int retval = posix_memalign(&frame_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, FRAME_BUFFER_SIZE_BYTES);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for frame_buffer with retval %d\n", (unsigned long long)(FRAME_BUFFER_SIZE_BYTES), retval);
            throw(LOFAR_VDIF_Packet_exception_no_memory());
        }
        memcpy(frame_buffer, a.frame_pointer_const(), FRAME_BUFFER_SIZE_BYTES);

        setup_internal_pointers();

        init_from_allocated_memory();
    
        if(!legacy_mode_effective()) {
            if(FRAME_BUFFER_SIZE_BYTES<=NORMAL_FRAME_HEADER_SIZE_BYTES) {
                throw(LOFAR_VDIF_Packet_exception_bad_frame_size());
            }
        }
    }
    return *this;
}







LOFAR_VDIF_Packet::
~LOFAR_VDIF_Packet()
{
    deallocate_memory();
    return;
}










uint64_t LOFAR_VDIF_Packet::
VDIF_epoch_to_UNIX_timestamp(const uint8_t epoch) throw()
{
    return VDIF_EPOCH_UNIX_TIMESTAMPS[epoch];
}


uint64_t LOFAR_VDIF_Packet::
get_VDIF_epoch_floor_from_UNIX_timestamp(const uint64_t t) throw()
{
    return VDIF_EPOCH_UNIX_TIMESTAMPS[get_VDIF_epoch_floor_index_from_UNIX_timestamp(t)];
}



uint_fast8_t LOFAR_VDIF_Packet::
get_VDIF_epoch_floor_index_from_UNIX_timestamp(const uint64_t t) throw()
{
    static bool is_inited = false;
    static uint_fast16_t last_index = 0;

    if(is_inited) {
        uint_fast16_t index = last_index+1;
        for(; index < VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE; ++index) {
            if(t < VDIF_EPOCH_UNIX_TIMESTAMPS[index]) {
                --index;
                break;
            }
        }
        for(; index > 0; --index) {
            if(t >= VDIF_EPOCH_UNIX_TIMESTAMPS[index]) {
                break;
            }
        }
        last_index = index;
        return uint_fast8_t(index);
    }
    else {
        // bisection finder.
        // Note that VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE is a power of 2
        uint_fast16_t index = (VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE>>1) - 1;
        uint_fast16_t Delta = VDIF_EPOCH_UNIX_TIMESTAMPS_SIZE>>2;
        while(Delta > 0) {
            
            if(t >= VDIF_EPOCH_UNIX_TIMESTAMPS[index+Delta]) {
                index = index+Delta;
            }
            else if(t < VDIF_EPOCH_UNIX_TIMESTAMPS[index]) {
                index -= Delta;
            }
            Delta = Delta>>1;
        }
        last_index = index;
        is_inited = true;
        return uint_fast8_t(index);
    }
    return uint_fast8_t(last_index);        
}





}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR
