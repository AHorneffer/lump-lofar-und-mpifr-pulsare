// LOFAR_Station_Beamformed_Valid.h
// handling a vald bit file for LOFAR beamformed data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 31  James M Anderson  --MPIfR  start
// 2014 Sep 16  JMA  --- use buffer for writing instead of writing by char

// Copyright (c) 2011, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_Valid_H
#define LOFAR_Station_Beamformed_Valid_H

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




// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



//_CLASS  LOFAR_Station_Beamformed_Valid_Writer --maps bits into bytes output
class LOFAR_Station_Beamformed_Valid_Writer {
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
    LOFAR_Station_Beamformed_Valid_Writer(const char* const restrict filename) throw();
    ~LOFAR_Station_Beamformed_Valid_Writer() throw();

    int_fast32_t write_N_points(uint_fast32_t N, uint_fast32_t valid) throw();



protected:
    // The buffer goes first in order to have it aligned for fast memory
    // operations on the write to file stage.
    uint8_t write_buffer[BUFSIZ]; // Use system's BUFSIZ value, guaranted by
                                  // C to be at least 256

    uint_fast32_t write_buffer_count;
    FILE* fp;
    uint8_t bit_buffer;
    uint8_t bits_used;




private:

    inline int_fast32_t put_c_in_write_buffer(uint8_t c) restrict throw()
    {
        write_buffer[write_buffer_count++] = c;
        if(write_buffer_count != BUFSIZ) {
            return 0;
        }
        else {
            write_buffer_count = 0;
            if(fwrite(write_buffer, BUFSIZ, 1, fp) == 1) {
                return 0;
            }
        }
        return 1;
    }
    inline int_fast32_t flush_write_buffer(void) restrict throw()
    {
        if((write_buffer_count)) {
            if(fwrite(write_buffer, size_t(write_buffer_count), 1, fp) == 1) {
            }
            else {
                return 1;
            }
        }
        write_buffer_count = 0;
        return 0;
    }


    // prevent copying
    LOFAR_Station_Beamformed_Valid_Writer(const LOFAR_Station_Beamformed_Valid_Writer& a);
    LOFAR_Station_Beamformed_Valid_Writer& operator=(const LOFAR_Station_Beamformed_Valid_Writer& a);

   
};








//_CLASS  LOFAR_Station_Beamformed_Valid_Reader --maps bits into bytes output
class LOFAR_Station_Beamformed_Valid_Reader {
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
    LOFAR_Station_Beamformed_Valid_Reader(const char* restrict const filename,
                                          const uint_fast64_t NUM_SAMPLES_,
                                          const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE_) throw();
    ~LOFAR_Station_Beamformed_Valid_Reader() throw();

    int_fast32_t read_N_points(uint_fast64_t start_sample,
                               uint_fast32_t NUM_SAMPLES,
                               uint8_t* const restrict valid) throw();



protected:
    const uint_fast64_t MAX_SAMPLE_NUMBER;
    uint_fast64_t MAX_NUM_BYTES;
    const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE;
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    int fd;
    void* memory_map;
    const uint8_t* restrict valid_buffer;
#else
   FILE* fp;
#endif



private:


    // prevent copying
    LOFAR_Station_Beamformed_Valid_Reader(const LOFAR_Station_Beamformed_Valid_Reader& a);
    LOFAR_Station_Beamformed_Valid_Reader& operator=(const LOFAR_Station_Beamformed_Valid_Reader& a);

  
};



// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Valid_H
