// LOFAR_Station_Beamformed_Writer_VDIF0.h
// writes out LOFAR VDIF frames
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 09  James M Anderson  --- start from LOFAR_Station_Beamformed_Writer_LuMP2.h
// 2014 Sep 20  JMA  --- need separate function to add LOFAR station
//                       information to VDIF headers
// 2014 Sep 20  JMA  --- change to use DAS number for VDIF output

// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_Writer_VDIF0_H
#define LOFAR_Station_Beamformed_Writer_VDIF0_H

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
#include <unistd.h>
#include <fcntl.h>
#include "LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





//_CLASS  LOFAR_Station_Beamformed_Writer_VDIF0
class LOFAR_Station_Beamformed_Writer_VDIF0 :
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
    LOFAR_Station_Beamformed_Writer_VDIF0(const char* const restrict filename_base_,
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
                                          const char* const scan_name_,
                                          const char* const DAS_name_) throw();

    virtual ~LOFAR_Station_Beamformed_Writer_VDIF0();

    void* start_VDIF_writer_thread();


protected:
    FILE* restrict fp_data;
    FILE* restrict fp_file_list;
    
    uint_fast64_t num_raw_packet_commands_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    LOFAR_Station_Beamformed_Valid_Writer* restrict valid_object;

    // Stuff for handling the VDIF frames
    size_t VDIF_FRAME_SIZE_BYTES;
    uint_fast32_t VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR;
    uint_fast32_t VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES;
    LOFAR_VDIF_Packet_LOFAR0* restrict * restrict VDIF_frames_boss;
    LOFAR_VDIF_Packet_LOFAR0* restrict * restrict VDIF_frames_VDIF_writer;
    uint_fast64_t current_VDIF_reference_second;
    uint_fast64_t current_VDIF_reference_epoch_unix_timestamp;
    uint_fast32_t num_time_sample_lines_in_boss_VDIF_storage;
    uint_fast32_t num_valid_time_sample_lines_in_boss_VDIF_storage;
    uint_fast32_t VDIF_NUM_TOTAL_FRAMES_PER_REFERENCE_SECOND_STRIDE;
    uint_fast32_t current_num_vdif_frames_since_reference_second_stride;
    int_fast32_t VDIF_frames_initialization_stage;
    uint_fast32_t LuMP_data_aquisition_system;

    // Things VDIF needs to keep around
    const char* Scan_Name;
    char* VDIF_filename;


    // Stuff for dealing with the VDIF_writer thread
    int_fast32_t VDIF_writer_thread_running;
    int_fast32_t VDIF_writer_thread_ready;
    int_fast32_t boss_has_VDIF_writer_thread_ready_lock;
    int_fast32_t thread_has_VDIF_writer_thread_ready_lock;
    LOFAR_Station_Beamformed_Writer_Base_Work_Enum VDIF_writer_thread_work_code;
    int_fast32_t VDIF_writer_thread_status;

    pthread_mutex_t VDIF_writer_thread_ready_mutex;
    pthread_cond_t VDIF_writer_thread_go_to_work_condition;
    pthread_t VDIF_writer_thread;

    uint_fast64_t VDIF_writer_thread_write_command_count;
    uint_fast64_t VDIF_writer_thread_write_frame_count;
    uint_fast64_t VDIF_writer_thread_write_byte_count;

    void* run_VDIF_writer_thread();
    int_fast32_t VDIF_writer_thread_do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();
    int_fast32_t init_VDIF_writer_thread_processing() throw();
    int_fast32_t wait_for_VDIF_writer_thread_sleeping() throw();
    int_fast32_t wait_for_VDIF_writer_thread_ready() throw() {return wait_for_VDIF_writer_thread_sleeping();}


    int_fast32_t tell_VDIF_writer_thread_to_init() restrict throw();
    int_fast32_t tell_VDIF_writer_thread_to_start() restrict throw();
    int_fast32_t tell_VDIF_writer_thread_to_send_data() restrict throw();
    int_fast32_t tell_VDIF_writer_thread_to_send_partial_data() restrict throw();
    int_fast32_t tell_VDIF_writer_thread_to_end_data() restrict throw();

    int_fast32_t stop_VDIF_writer_thread() restrict throw();

    // functions to help with debugging mutexes
#define Writer_VDIF0_BOSS_LOCK_MUTEX(mutex) boss_lock_VDIF_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void boss_lock_VDIF_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                   const char* const restrict FILE,
                                                   const int LINE) restrict throw() {
        if((boss_has_VDIF_writer_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("boss already has VDIF_writer_thread_lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss locking VDIF_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        boss_has_VDIF_writer_thread_ready_lock = 1;
        return;
    }
#define Writer_VDIF0_THREAD_LOCK_MUTEX(mutex) thread_lock_VDIF_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void thread_lock_VDIF_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                     const char* const restrict FILE,
                                                     const int LINE) restrict throw() {
        if((thread_has_VDIF_writer_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("thread already has VDIF_writer_thread_lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread locking VDIF_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        thread_has_VDIF_writer_thread_ready_lock=1;
        return;
    }
#define Writer_VDIF0_BOSS_UNLOCK_MUTEX(mutex) boss_unlock_VDIF_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void boss_unlock_VDIF_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                     const char* const restrict FILE,
                                                     const int LINE) restrict throw() {
        if(!boss_has_VDIF_writer_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("boss does not have VDIF_writer_thread_lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        boss_has_VDIF_writer_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss unlocking VDIF_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define Writer_VDIF0_THREAD_UNLOCK_MUTEX(mutex) thread_unlock_VDIF_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void thread_unlock_VDIF_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                       const char* const restrict FILE,
                                                       const int LINE) restrict throw() {
        if(!thread_has_VDIF_writer_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("thread does not have VDIF_writer_thread_lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        thread_has_VDIF_writer_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread unlocking VDIF_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define Writer_VDIF0_BOSS_BROADCAST_CONDITION(condition) boss_broadcast_VDIF_writer_thread_condition(condition,__FILE__,__LINE__)
    inline void boss_broadcast_VDIF_writer_thread_condition(pthread_cond_t* restrict condition_p,
                                                            const char* const restrict FILE,
                                                            const int LINE) restrict throw() {
        if((boss_has_VDIF_writer_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_broadcast_VDIF_writer_thread_condition failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define Writer_VDIF0_THREAD_BROADCAST_CONDITION(condition) thread_broadcast_VDIF_writer_thread_condition(condition,__FILE__,__LINE__)
    inline void thread_broadcast_VDIF_writer_thread_condition(pthread_cond_t* restrict condition_p,
                                                              const char* const restrict FILE,
                                                              const int LINE) restrict throw() {
        if((thread_has_VDIF_writer_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_broadcast_VDIF_writer_thread_condition failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define Writer_VDIF0_BOSS_CONDITION_WAIT(condition,mutex) boss_VDIF_writer_thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void boss_VDIF_writer_thread_condition_wait(pthread_cond_t* restrict condition_p,
                                                       pthread_mutex_t* restrict mutex_p,
                                                       const char* const restrict FILE,
                                                       const int LINE) restrict throw() {
        if((boss_has_VDIF_writer_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_VDIF_writer_thread_condition_wait failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define Writer_VDIF0_THREAD_CONDITION_WAIT(condition,mutex) thread_VDIF_writer_thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void thread_VDIF_writer_thread_condition_wait(pthread_cond_t* restrict condition_p,
                                                         pthread_mutex_t* restrict mutex_p,
                                                         const char* const restrict FILE,
                                                         const int LINE) restrict throw() {
        if((thread_has_VDIF_writer_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_VDIF_writer_thread_condition_wait failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }

    

    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();








    template<typename Tproc> int_fast32_t write_block_to_VDIF_frames(const Tproc MASK_PETTERN) restrict throw()
    {
        if(VDIF_frames_initialization_stage >= 3) {}
        else {
            add_LOFAR_station_software_info_to_VDIF_frame_data_areas();
        }
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tproc* const restrict data_in = reinterpret_cast<const Tproc* const restrict>(current_data_packets[packet]->data_start_const());
            uint_fast32_t sample_offset = 0;
            while(sample_offset < NUM_Blocks) {
                const uint_fast32_t VDIF_BYTE_OFFSET = VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR * num_time_sample_lines_in_boss_VDIF_storage;
                uint_fast32_t samples_to_copy = NUM_Blocks - sample_offset;
                if(num_time_sample_lines_in_boss_VDIF_storage+samples_to_copy > VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES) {
                    samples_to_copy = VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES - num_time_sample_lines_in_boss_VDIF_storage;
                }
                for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                    char* VDIF_start_char = static_cast<char*>(VDIF_frames_boss[beamlet]->data_pointer())+VDIF_BYTE_OFFSET;
                    Tproc* restrict data_out = reinterpret_cast<Tproc* const restrict>(VDIF_start_char);
                    const Tproc* restrict data_in_beamlet = data_in
                                                            + sample_offset
                                                            + (Beamlet_Offsets_Array[beamlet]>>1);
                    for(uint_fast16_t s = 0; s < samples_to_copy; ++s) {
                        // VDIF uses an offset integer format with all bits
                        // zero being the lowest value.  As LOFAR uses
                        // two's complement integers, to get the the VDIF
                        // pattern, xor by the provided mask.
                        data_out[s] = data_in_beamlet[s] ^ MASK_PETTERN;
                    }
                }
                sample_offset += samples_to_copy;
                // Now increment the VDIF sample and valid information
                num_time_sample_lines_in_boss_VDIF_storage += samples_to_copy;
                if((current_valid[packet])) {
                    num_valid_time_sample_lines_in_boss_VDIF_storage += samples_to_copy;
                }
                if(num_time_sample_lines_in_boss_VDIF_storage == VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES) {
                    int_fast32_t retval = tell_VDIF_writer_thread_to_send_data();
                    if((retval)) {
                        DEF_LOGGING_ERROR("giving write buffer to VDIF_writer_thread failed with %d", int(retval));
                        return LOFAR_WRITER_RETVAL_WRITE_FAIL;
                    }
                }
            } // while(sample_offset < NUM_Blocks)
        } // for packet < current_num_packets
        return 0;
    }



    template<typename Tproc> int_fast32_t write_block_to_VDIF_frames_partial(const Tproc MASK_PETTERN) restrict throw()
    {
        if(VDIF_frames_initialization_stage >= 3) {}
        else {
            add_LOFAR_station_software_info_to_VDIF_frame_data_areas();
        }
        const uint_fast16_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                             - current_sample_offset);

        const Tproc* const restrict data_in = reinterpret_cast<const Tproc* const restrict>(current_data_packets[0]->data_start_const());
        uint_fast32_t sample_offset = 0;
        while(sample_offset < NUM_ACTUAL_BLOCKS) {
            const uint_fast32_t VDIF_BYTE_OFFSET = VDIF_BUFFER_TIME_SAMPLE_LINE_LENGTH_CHAR * num_time_sample_lines_in_boss_VDIF_storage;
            uint_fast32_t samples_to_copy = NUM_ACTUAL_BLOCKS - sample_offset;
            if(num_time_sample_lines_in_boss_VDIF_storage+samples_to_copy > VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES) {
                samples_to_copy = VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES - num_time_sample_lines_in_boss_VDIF_storage;
            }
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                char* VDIF_start_char = static_cast<char*>(VDIF_frames_boss[beamlet]->data_pointer())+VDIF_BYTE_OFFSET;
                Tproc* restrict data_out = reinterpret_cast<Tproc* const restrict>(VDIF_start_char);
                const Tproc* restrict data_in_beamlet = data_in
                                                        + sample_offset
                                                        + (Beamlet_Offsets_Array[beamlet]>>1);
                for(uint_fast16_t s = 0; s < samples_to_copy; ++s) {
                    // VDIF uses an offset integer format with all bits
                    // zero being the lowest value.  As LOFAR uses
                    // two's complement integers, to get the the VDIF
                    // pattern, xor by the provided mask.
                    data_out[s] = data_in_beamlet[s] ^ MASK_PETTERN;
                }
            }
            sample_offset += samples_to_copy;
            // Now increment the VDIF sample and valid information
            num_time_sample_lines_in_boss_VDIF_storage += samples_to_copy;
            if((current_valid[0])) {
                num_valid_time_sample_lines_in_boss_VDIF_storage += samples_to_copy;
            }
            if(num_time_sample_lines_in_boss_VDIF_storage == VDIF_BUFFER_NUM_TOTAL_TIME_SAMPLE_LINES) {
                int_fast32_t retval = tell_VDIF_writer_thread_to_send_data();
                if((retval)) {
                    DEF_LOGGING_ERROR("giving write buffer to VDIF_writer_thread failed with %d", int(retval));
                    return LOFAR_WRITER_RETVAL_WRITE_FAIL;
                }
            }
        } // while(sample_offset < NUM_Blocks)
        // now force writing of last partial frame
        {
            int_fast32_t retval = tell_VDIF_writer_thread_to_send_partial_data();
            if((retval)) {
                DEF_LOGGING_ERROR("giving write buffer to VDIF_writer_thread failed with %" PRIdFAST32, retval);
                return LOFAR_WRITER_RETVAL_WRITE_FAIL;
            }
        }
        return 0;
    }






private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_VDIF0_OUT);}

    static const int LuMP_STATION_NAME_STR_SIZE = 32;
    static const uint8_t TWOS_COMPLEMENT_intComplex8_t = 0x88;
    static const uint16_t TWOS_COMPLEMENT_intComplex8_t_DOUBLE = 0x8888U;
    static const uint16_t TWOS_COMPLEMENT_intComplex16_t = 0x8080U;
    static const uint32_t TWOS_COMPLEMENT_intComplex16_t_DOUBLE = UINT32_C(0x80808080);
    static const uint32_t TWOS_COMPLEMENT_intComplex32_t = UINT32_C(0x80008000);
    static const uint64_t TWOS_COMPLEMENT_intComplex32_t_DOUBLE = UINT64_C(0x8000800080008000);



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


    int_fast32_t copy_string_information(const char* const scan_name_) restrict throw();
    int_fast32_t generate_VDIF_filename(void) restrict throw();
    int_fast32_t check_for_allowed_VDIF_filename_characters(const char* restrict s,
                                                            const bool allow_underscore) restrict const throw();
    int_fast32_t initialize_VDIF_frame_data_areas(void) restrict throw();
    void add_LOFAR_station_software_info_to_VDIF_frame_data_areas(void) restrict throw();
    int_fast32_t flush_VDIF_frame_data_areas(void) restrict throw();
    uint_fast32_t determine_LuMP_data_aquisition_system(const char* const DASptr) restrict throw();



    // prevent copying
    LOFAR_Station_Beamformed_Writer_VDIF0(const LOFAR_Station_Beamformed_Writer_VDIF0& a);
    LOFAR_Station_Beamformed_Writer_VDIF0& operator=(const LOFAR_Station_Beamformed_Writer_VDIF0& a);

    // Stuff for boss to tell VDIF_writer thread to do something
    int_fast32_t wait_for_VDIF_writer_thread_startup() restrict throw();
    int_fast32_t VDIF_writer_thread_constructor() restrict throw();
    int_fast32_t VDIF_writer_thread_destructor() restrict throw();
    int_fast32_t VDIF_writer_thread_write_buffer() throw();
    int_fast32_t VDIF_writer_thread_stop_writing() throw();

};


// CLASS FUNCTIONS







// GLOBALS



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#undef NUM
#endif // LOFAR_Station_Beamformed_Writer_VDIF0_H
