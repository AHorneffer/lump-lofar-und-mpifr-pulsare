// LOFAR_Station_Beamformed_Writer_LuMP1.h
// writes out multiple subband LuMP pulsar format data
//_HIST  DATE NAME PLACE INFO
// 2013 Feb 23  James M Anderson  --- MPIfR  start from LuMP0 format
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Aug 19  JMA  ---- convert to first sample offset in Base.h class

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



#ifndef LOFAR_Station_Beamformed_Writer_LuMP1_H
#define LOFAR_Station_Beamformed_Writer_LuMP1_H

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
#include "LOFAR_Station_Beamformed_Valid.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





//_CLASS  LOFAR_Station_Beamformed_Writer_LuMP1
class LOFAR_Station_Beamformed_Writer_LuMP1 :
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
    LOFAR_Station_Beamformed_Writer_LuMP1(const char* const restrict filename_base_,
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
                                          const char* const * const restrict SourceName_Array_) throw();

    virtual ~LOFAR_Station_Beamformed_Writer_LuMP1();


    static const uint16_t LuMP_HEADER_SIZE = 4096;

protected:
    FILE* restrict fp_data;
    
    uint_fast64_t num_raw_packet_commands_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    LOFAR_Station_Beamformed_Valid_Writer* restrict valid_object;

    uint64_t pad_to_guarantee_64_bit_aligned;
    char LuMP_header[LuMP_HEADER_SIZE];

    uint_fast16_t OUTPUT_BUFFER_LENGTH_CHAR;
    void* output_storage;

    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();

    void write_into_header(uint_fast32_t pos, const char* const format,
                           uint_fast32_t MAX_SIZE) throw();
    template <class T> void write_into_header(uint_fast32_t pos,
                                              const char* const format,
                                              uint_fast32_t MAX_SIZE,
                                              const T datap) throw();


    
    template<class Tproc> int_fast32_t copy_block_to_output() restrict throw()
    {
        Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(output_storage);

        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tproc* const restrict data_in =
                reinterpret_cast<const Tproc* const restrict>(current_data_packets[packet]->data_start_const());
            for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                // copy over both polarizations, X then Y, for all sample lines
                const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet];
                Tproc* restrict data_bp = data_out + (beamlet<<1);
                for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                    data_bp[0] = *data_bp_in++;
                    data_bp[1] = *data_bp_in++;
                    data_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
                }
            }
            if(fwrite(data_out, OUTPUT_BUFFER_LENGTH_CHAR, 1,
                      fp_data) == 1) {
            }
            else {
                return LOFAR_WRITER_RETVAL_WRITE_FAIL;
            }
        }
    
        return 0;
    }
    
    template<class Tproc> int_fast32_t copy_block_to_output_partial() restrict throw()
    {
        const uint_fast16_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                             - current_sample_offset);
        const uint_fast16_t THIS_OUTPUT_BUFFER_LENGTH_CHAR =
            uint_fast64_t(NUM_RSP_BEAMLET_POLARIZATIONS)
            * NUM_Output_Beamlets * NUM_ACTUAL_BLOCKS
            * Voltage_Size;
        
        Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(output_storage);

        const Tproc* const restrict data_in =
            reinterpret_cast<const Tproc* const restrict>(current_data_packets[0]->data_start_const());
        for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
            // copy over both polarizations, X then Y, for all sample lines
            const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet];
            Tproc* restrict data_bp = data_out + (beamlet<<1);
            for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                data_bp[0] = *data_bp_in++;
                data_bp[1] = *data_bp_in++;
                data_bp += NUM_Output_Beamlets+NUM_Output_Beamlets;
            }
        }
        if(fwrite(data_out, THIS_OUTPUT_BUFFER_LENGTH_CHAR, 1,
                  fp_data) == 1) {
        }
        else {
            return LOFAR_WRITER_RETVAL_WRITE_FAIL;
        }
    
        return 0;
    }




private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_LuMP1_OUT);}

    static const int LuMP_STATION_NAME_STR_SIZE = 32;



    int_fast32_t check_input_parameters() restrict throw();
    int_fast32_t set_up_work_buffer_areas() restrict throw();
    int_fast32_t open_standard_files() restrict throw();

    int_fast32_t thread_constructor() restrict;
    int_fast32_t thread_destructor() restrict;
    int_fast32_t close_output_files() throw();
    int_fast32_t report_file_error(const char* const restrict msg) const throw();
    int_fast32_t write_header_init() throw();
    int_fast32_t write_header_start() throw();
    int_fast32_t write_header_end() throw();
    int_fast32_t write_standard_packets() throw();
    int_fast32_t write_partial_packet() throw();


    // prevent copying
    LOFAR_Station_Beamformed_Writer_LuMP1(const LOFAR_Station_Beamformed_Writer_LuMP1& a);
    LOFAR_Station_Beamformed_Writer_LuMP1& operator=(const LOFAR_Station_Beamformed_Writer_LuMP1& a);

    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS
int_fast32_t JD(int_fast32_t year, int_fast32_t month, int_fast32_t day);
double JD(int_fast32_t year, int_fast32_t month, int_fast32_t day,
          int_fast32_t hour, int_fast32_t minute, int_fast32_t second,
          double sec_fraction);
double MJD(int_fast32_t year, int_fast32_t month, int_fast32_t day,
           int_fast32_t hour, int_fast32_t minute, int_fast32_t second,
           double sec_fraction);




// GLOBALS



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#undef NUM
#endif // LOFAR_Station_Beamformed_Writer_LuMP1_H
