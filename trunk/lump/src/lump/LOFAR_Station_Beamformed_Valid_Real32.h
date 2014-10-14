// LOFAR_Station_Beamformed_Valid_Real32.h
// handling a valid Real32 file for LOFAR beamformed data
//_HIST  DATE NAME PLACE INFO
// 2011 May 11  James M Anderson  --MPIfR  start

// Copyright (c) 2011, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_Valid_Real32_H
#define LOFAR_Station_Beamformed_Valid_Real32_H

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




// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



//_CLASS  LOFAR_Station_Beamformed_Valid_Real32_Writer --maps bits into bytes output
class LOFAR_Station_Beamformed_Valid_Real32_Writer {
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
    LOFAR_Station_Beamformed_Valid_Real32_Writer(const char* const restrict filename) throw();
    ~LOFAR_Station_Beamformed_Valid_Real32_Writer() throw();

    int_fast32_t write_valid(const Real32_t& valid) restrict throw()
    {
        if((fp)) {}
        else {return -1;}
#if __BYTE_ORDER == __LITTLE_ENDIAN
        if(fwrite(&valid, sizeof(valid), 1, fp) == 1) {
            return 0;
        }
#else
        int32_t* const restrict ip = reinterpret_cast<void*>(&valid);
        int32_t i = LOFAR_Station_Swap_int32_t(*ip);
        if(fwrite(&i, sizeof(valid), 1, fp) == 1) {
            return 0;
        }
#endif
        return -2;
    }




protected:
    FILE* fp;



private:


    // prevent copying
    LOFAR_Station_Beamformed_Valid_Real32_Writer(const LOFAR_Station_Beamformed_Valid_Real32_Writer& a);
    LOFAR_Station_Beamformed_Valid_Real32_Writer& operator=(const LOFAR_Station_Beamformed_Valid_Real32_Writer& a);

   
};








//_CLASS  LOFAR_Station_Beamformed_Valid_Real32_Reader --maps bits into bytes output
class LOFAR_Station_Beamformed_Valid_Real32_Reader {
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
    LOFAR_Station_Beamformed_Valid_Real32_Reader(const char* restrict const filename,
                                                 const uint_fast64_t NUM_SAMPLES_,
                                                 const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE_) throw();
    ~LOFAR_Station_Beamformed_Valid_Real32_Reader() throw();

    int_fast32_t read_N_points(uint_fast64_t start_sample,
                               uint_fast32_t NUM_SAMPLES,
                               Real32_t* const restrict valid) throw();



protected:
    const uint_fast64_t MAX_SAMPLE_NUMBER;
    const uint_fast64_t MAX_NUM_BYTES;
    const uint_fast64_t SAMPLE_OFFSET_FROM_TIME_REFERENCE;
#if defined(__LP64__) || defined(_LP64) || defined(__64BIT__)
    int fd;
    void* memory_map;
    const Real32_t* restrict valid_buffer;
#else
   FILE* fp;
#endif



private:


    // prevent copying
    LOFAR_Station_Beamformed_Valid_Real32_Reader(const LOFAR_Station_Beamformed_Valid_Real32_Reader& a);
    LOFAR_Station_Beamformed_Valid_Real32_Reader& operator=(const LOFAR_Station_Beamformed_Valid_Real32_Reader& a);

  
};



// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Valid_Real32_H
