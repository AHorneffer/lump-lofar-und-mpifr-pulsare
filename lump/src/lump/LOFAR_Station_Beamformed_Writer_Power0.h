// LOFAR_Station_Beamformed_Writer_Power0.h
// Derived class for writing total power measurements
//_HIST  DATE NAME PLACE INFO
// 2011 May 11  James M Anderson  --MPIfR  Start
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2012 Jan 29  JMA  --move file opening and closing to thread
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info

// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_Writer_Power0_H
#define LOFAR_Station_Beamformed_Writer_Power0_H

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
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include <stdio.h>
#include <stdlib.h>
#include "LOFAR_Station_Beamformed_Valid_Real32.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





//_CLASS  LOFAR_Station_Beamformed_Writer_Power0
class LOFAR_Station_Beamformed_Writer_Power0 :
        public LOFAR_Station_Beamformed_Writer_Base {
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
    LOFAR_Station_Beamformed_Writer_Power0(const char* const restrict filename_base_,
                                           const char* const restrict station_name_,
                                           const uint32_t reference_time_,
                                           const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                           const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                           const uint_fast16_t NUM_Beamlets_,
                                           const uint_fast16_t NUM_Blocks_,
                                           const LOFAR_raw_data_type_enum DATA_TYPE_,
                                           const int32_t id_,
                                           const uint_fast32_t NUM_OUTPUT_CHANNELS_,
                                           const uint_fast16_t NUM_Output_Beamlets_,
                                           const uint_fast32_t* const restrict Physical_Beamlets_Array_,
                                           const uint_fast32_t* const restrict Physical_Subband_Array_,
                                           const uint_fast32_t* const restrict RCUMODE_Array_,
                                           const Real64_t* const restrict RightAscension_Array_,
                                           const Real64_t* const restrict Declination_Array_,
                                           const char* const * const restrict Epoch_Array_,
                                           const char* const * const restrict SourceName_Array_,
                                           const Real64_t desired_seconds_per_integration_,
                                           const uint8_t scale_by_inverse_num_samples_,
                                           const Real64_t extra_integration_multiplier,
                                           const LOFAR_raw_data_type_enum integration_DATA_TYPE,
                                           const uint8_t bounds_check_integration_data_type_) throw();
    
    virtual ~LOFAR_Station_Beamformed_Writer_Power0();


protected:
    FILE* restrict fp_data;
    
    uint_fast64_t num_raw_packet_commands_processed;
    uint_fast64_t num_integration_points_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    uint8_t*         restrict data_8_p;
    uint16_t*        restrict data_16_p;
    uint32_t*        restrict data_32_p;
    uint64_t*        restrict data_64_p;
    intComplex128_t* restrict data_128_p;
    LOFAR_Station_Beamformed_Valid_Real32_Writer* restrict valid_object;
    uint64_t pad_to_guarantee_64_bit_aligned;
    char data_buffer[MAX_RSP_PACKET_SIZE_CHAR];
    uint_fast32_t SAMPLES_PER_INTEGRATION;
    uint_fast32_t samples_in_this_integration;
    uint_fast32_t valid_samples_in_this_integration;
    uint_fast32_t apply_scaling;
    uint_fast32_t bounds_check_integration_data_type;
    Real32_t valid_multiplier;
    Real64_t scaling_multiplier;
    const Real64_t desired_seconds_per_integration;
    void* integration_storage;
    void* output_storage;
    uint_fast32_t INTEGRATION_BUFFER_LENGTH_CHAR;
    uint_fast32_t OUTPUT_INTEGRATION_BUFFER_LENGTH_CHAR;
    uint_fast32_t NUM_VALUES_IN_INTEGRATION_BUFFER;

    static const uint_fast16_t NUM_OUTPUT_FIELDS = 4;

    const uint8_t scale_by_inverse_num_samples;
    

    
    const LOFAR_raw_data_type_enum output_DATA_TYPE;

    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();

private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_POWER0_OUT);}




    int_fast32_t thread_constructor() restrict;
    int_fast32_t thread_destructor() restrict;
    int_fast32_t close_output_files() throw();
    int_fast32_t report_file_error(const char* const restrict msg) const throw();
    int_fast32_t write_header_init() throw();
    int_fast32_t write_header_start() throw();
    int_fast32_t write_header_end() throw();
    int_fast32_t write_standard_packets() throw();
    int_fast32_t write_partial_packet() throw();
    int_fast32_t add_packet_to_integration(const uint_fast16_t NUM_SAMPLES,
                                           const uint8_t packet_valid) throw();
    int_fast32_t dump_integration_to_output() throw();



    // prevent copying
    LOFAR_Station_Beamformed_Writer_Power0(const LOFAR_Station_Beamformed_Writer_Power0& a);
    LOFAR_Station_Beamformed_Writer_Power0& operator=(const LOFAR_Station_Beamformed_Writer_Power0& a);

    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Writer_Power0_H
