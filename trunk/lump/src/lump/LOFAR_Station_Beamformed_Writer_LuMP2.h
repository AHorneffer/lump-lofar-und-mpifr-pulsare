// LOFAR_Station_Beamformed_Writer_LuMP2.h
// writes out multiple subband LuMP pulsar format data
//_HIST  DATE NAME PLACE INFO
// 2013 Aug 18  James M Anderson  --- MPIfR  start from LuMP2 format
// 2013 Sep 05  JMA  ---- development fixes to get working and provide logfile
//                        for child process stdout/stderr

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



#ifndef LOFAR_Station_Beamformed_Writer_LuMP2_H
#define LOFAR_Station_Beamformed_Writer_LuMP2_H

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








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





//_CLASS  LOFAR_Station_Beamformed_Writer_LuMP2
class LOFAR_Station_Beamformed_Writer_LuMP2 :
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
    LOFAR_Station_Beamformed_Writer_LuMP2(const char* const restrict filename_base_,
                                          const char* const restrict station_name_,
                                          const uint32_t reference_time_,
                                          const uint32_t end_time_,
                                          const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                          const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                          const uint_fast16_t NUM_Beamlets_,
                                          const uint_fast16_t NUM_Blocks_,
                                          const uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE_,
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
                                          const char* const child_process_command_line_,
                                          const char* const child_process_channel_count_multiplier_,
                                          const char* const child_process_sample_blocksize_,
                                          const char* const child_process_logfile_) throw();

    virtual ~LOFAR_Station_Beamformed_Writer_LuMP2();

    void* start_pipe_writer_thread();

    static const uint16_t LuMP_HEADER_SIZE = 4096;

protected:
    const uint32_t end_time;
    FILE* restrict fp_data;
    
    uint_fast64_t num_raw_packet_commands_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    LOFAR_Station_Beamformed_Valid_Writer* restrict valid_object;

    uint64_t pad_to_guarantee_64_bit_aligned;
    char LuMP_header[LuMP_HEADER_SIZE];

    // Stuff about the child processing process
    uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE;
    uint_fast64_t CHILD_PROCESS_SAMPLE_BLOCKSIZE;
    uint_fast32_t CHILD_CHANNEL_COUNT_MULTIPLIER;
    char* child_process_command_line;
    char* child_process_logfile;

    // Stuff for handling the output
    bool output_beamlets_are_contiguous;
    Real64_t LuMP2_BANDWIDTH_IN_HZ;
    Real64_t LuMP2_CENTER_FREQUENCY_HZ;
    uint_fast16_t NUM_OUTPUT_BEAMLETS;
    uint_fast32_t* restrict beamlet_index;  
    uint_fast64_t OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR;
    uint_fast64_t OUTPUT_BUFFER_LOFAR_BLOCK_LENGTH_CHAR;
    uint_fast64_t OUTPUT_BUFFER_NUM_SAMPLE_LINES;
    uint_fast64_t OUTPUT_BUFFER_TOTAL_LENGTH_CHAR;
    void* output_storage;
    void* output_storage_boss;
    void* output_storage_pipe_writer;
    uint_fast64_t num_bytes_in_boss_storage_buffer;
    uint_fast64_t num_bytes_in_pipe_writer_storage_buffer;

    // Stuff for dealing with the pipe_writer thread
    int_fast32_t pipe_writer_thread_running;
    int_fast32_t pipe_writer_thread_ready;
    int_fast32_t boss_has_pipe_writer_thread_ready_lock;
    int_fast32_t thread_has_pipe_writer_thread_ready_lock;
    LOFAR_Station_Beamformed_Writer_Base_Work_Enum pipe_writer_thread_work_code;
    int_fast32_t pipe_writer_thread_status;

    pthread_mutex_t pipe_writer_thread_ready_mutex;
    pthread_cond_t pipe_writer_thread_go_to_work_condition;
    pthread_t pipe_writer_thread;

    uint_fast64_t pipe_writer_thread_write_command_count;
    uint_fast64_t pipe_writer_thread_write_byte_count;
    uint_fast64_t pipe_writer_thread_write_sample_count;

    // Stuff for dealing with the child thread
    pid_t child_process_id;
    int child_fd;             // file descriptor to write to child stdin

    void* run_pipe_writer_thread();
    int_fast32_t pipe_writer_thread_do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();
    int_fast32_t init_pipe_writer_thread_processing() throw();
    int_fast32_t wait_for_pipe_writer_thread_sleeping() throw();
    int_fast32_t wait_for_pipe_writer_thread_ready() throw() {return wait_for_pipe_writer_thread_sleeping();}


    int_fast32_t tell_pipe_writer_thread_to_init() restrict throw();
    int_fast32_t tell_pipe_writer_thread_to_start_child() restrict throw();
    int_fast32_t tell_pipe_writer_thread_to_send_data() restrict throw();
    int_fast32_t tell_pipe_writer_thread_to_send_partial_data() restrict throw();
    int_fast32_t tell_pipe_writer_thread_to_end_data() restrict throw();

    int_fast32_t stop_pipe_writer_thread() restrict throw();

    // functions to help with debugging mutexes
#define WRITER_LuMP2_BOSS_LOCK_MUTEX(mutex) boss_lock_pipe_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void boss_lock_pipe_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                   const char* const restrict FILE,
                                                   const int LINE) restrict throw() {
        if((boss_has_pipe_writer_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("boss already has pipe_writer_thread_lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss locking pipe_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        boss_has_pipe_writer_thread_ready_lock = 1;
        return;
    }
#define WRITER_LuMP2_THREAD_LOCK_MUTEX(mutex) thread_lock_pipe_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void thread_lock_pipe_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                     const char* const restrict FILE,
                                                     const int LINE) restrict throw() {
        if((thread_has_pipe_writer_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("thread already has pipe_writer_thread_lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread locking pipe_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        thread_has_pipe_writer_thread_ready_lock=1;
        return;
    }
#define WRITER_LuMP2_BOSS_UNLOCK_MUTEX(mutex) boss_unlock_pipe_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void boss_unlock_pipe_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                     const char* const restrict FILE,
                                                     const int LINE) restrict throw() {
        if(!boss_has_pipe_writer_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("boss does not have pipe_writer_thread_lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        boss_has_pipe_writer_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss unlocking pipe_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define WRITER_LuMP2_THREAD_UNLOCK_MUTEX(mutex) thread_unlock_pipe_writer_thread_mutex(mutex,__FILE__,__LINE__)
    inline void thread_unlock_pipe_writer_thread_mutex(pthread_mutex_t* restrict mutex_p,
                                                       const char* const restrict FILE,
                                                       const int LINE) restrict throw() {
        if(!thread_has_pipe_writer_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("thread does not have pipe_writer_thread_lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        thread_has_pipe_writer_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread unlocking pipe_writer_thread_mutex failed");
            DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define WRITER_LuMP2_BOSS_BROADCAST_CONDITION(condition) boss_broadcast_pipe_writer_thread_condition(condition,__FILE__,__LINE__)
    inline void boss_broadcast_pipe_writer_thread_condition(pthread_cond_t* restrict condition_p,
                                                            const char* const restrict FILE,
                                                            const int LINE) restrict throw() {
        if((boss_has_pipe_writer_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_broadcast_pipe_writer_thread_condition failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_LuMP2_THREAD_BROADCAST_CONDITION(condition) thread_broadcast_pipe_writer_thread_condition(condition,__FILE__,__LINE__)
    inline void thread_broadcast_pipe_writer_thread_condition(pthread_cond_t* restrict condition_p,
                                                              const char* const restrict FILE,
                                                              const int LINE) restrict throw() {
        if((thread_has_pipe_writer_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_broadcast_pipe_writer_thread_condition failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_LuMP2_BOSS_CONDITION_WAIT(condition,mutex) boss_pipe_writer_thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void boss_pipe_writer_thread_condition_wait(pthread_cond_t* restrict condition_p,
                                                       pthread_mutex_t* restrict mutex_p,
                                                       const char* const restrict FILE,
                                                       const int LINE) restrict throw() {
        if((boss_has_pipe_writer_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_pipe_writer_thread_condition_wait failed");
                DEF_LOGGING_CRITICAL("failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("do not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_LuMP2_THREAD_CONDITION_WAIT(condition,mutex) thread_pipe_writer_thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void thread_pipe_writer_thread_condition_wait(pthread_cond_t* restrict condition_p,
                                                         pthread_mutex_t* restrict mutex_p,
                                                         const char* const restrict FILE,
                                                         const int LINE) restrict throw() {
        if((thread_has_pipe_writer_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_pipe_writer_thread_condition_wait failed");
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

    void write_into_header(uint_fast32_t pos, const char* const format,
                           uint_fast32_t MAX_SIZE) throw();
    template <class T> void write_into_header(uint_fast32_t pos,
                                              const char* const format,
                                              uint_fast32_t MAX_SIZE,
                                              const T datap) throw();


    
    template<class Tproc> int_fast32_t copy_block_to_output(uint_fast32_t multiplier) restrict throw()
    {
        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(reinterpret_cast<char*>(output_storage_boss)+num_bytes_in_boss_storage_buffer);
        uint_fast32_t COPY_COUNT = multiplier * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;

        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tproc* const restrict data_in =
                reinterpret_cast<const Tproc* const restrict>(current_data_packets[packet]->data_start_const());
            if(multiplier==1) {
                // optimize for the simple case
                for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                    const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet];
                    Tproc* restrict data_bp = data_out + beamlet_index[beamlet];
                    for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                        data_bp[0] = *data_bp_in++;
                        data_bp[1] = *data_bp_in++;
                        data_bp += NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS;
                    }
                }
            }
            else {
                // generic case
                for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                    const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet] * multiplier;
                    Tproc* restrict data_bp = data_out + beamlet_index[beamlet];
                    for(uint_fast16_t sample = 0; sample < NUM_Blocks; sample++) {
                        for(uint_fast32_t c=0; c < COPY_COUNT; c++) {
                            data_bp[c] = *data_bp_in++;
                        }
                        data_bp += NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS;
                    }
                }
            }
            data_out += (NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS)*uint_fast32_t(NUM_Blocks);
            num_bytes_in_boss_storage_buffer += OUTPUT_BUFFER_LOFAR_BLOCK_LENGTH_CHAR;
            if(num_bytes_in_boss_storage_buffer == OUTPUT_BUFFER_TOTAL_LENGTH_CHAR) {
                int_fast32_t retval = tell_pipe_writer_thread_to_send_data();
                if((retval)) {
                    DEF_LOGGING_ERROR("giving write buffer to pipe_writer_thread failed with %d", int(retval));
                    return LOFAR_WRITER_RETVAL_WRITE_FAIL;
                }
                data_out = reinterpret_cast<Tproc* restrict>(output_storage_boss);
            }
        }
    
        return 0;
    }
    
    template<class Tproc> int_fast32_t copy_block_to_output_partial(uint_fast32_t multiplier) restrict throw()
    {
        const uint_fast16_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                             - current_sample_offset);

        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(reinterpret_cast<char*>(output_storage_boss)+num_bytes_in_boss_storage_buffer);
        uint_fast32_t COPY_COUNT = multiplier * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;

        const Tproc* const restrict data_in =
            reinterpret_cast<const Tproc* const restrict>(current_data_packets[0]->data_start_const());
        if(multiplier==1) {
            // optimize for the simple case
            for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet];
                Tproc* restrict data_bp = data_out + beamlet_index[beamlet];
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    data_bp[0] = *data_bp_in++;
                    data_bp[1] = *data_bp_in++;
                    data_bp += NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS;
                }
            }
        }
        else {
            // generic case
            for(uint_fast32_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const Tproc* restrict data_bp_in = data_in + Beamlet_Offsets_Array[beamlet] * multiplier;
                Tproc* restrict data_bp = data_out + beamlet_index[beamlet];
                for(uint_fast16_t sample = 0; sample < NUM_ACTUAL_BLOCKS; sample++) {
                    for(uint_fast32_t c=0; c < COPY_COUNT; c++) {
                        data_bp[c] = *data_bp_in++;
                    }
                    data_bp += NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS;
                }
            }
        }
        data_out += (NUM_OUTPUT_BEAMLETS+NUM_OUTPUT_BEAMLETS)*uint_fast32_t(NUM_ACTUAL_BLOCKS);
        num_bytes_in_boss_storage_buffer += OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR * NUM_ACTUAL_BLOCKS;

        int_fast32_t retval = tell_pipe_writer_thread_to_send_partial_data();
        if((retval)) {
            DEF_LOGGING_ERROR("giving write buffer to pipe_writer_thread failed with %d", int(retval));
            return LOFAR_WRITER_RETVAL_WRITE_FAIL;
        }
    
        return 0;
    }


    uint_fast64_t expected_sample_count(void) const throw();



private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_LuMP2_OUT);}

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
    LOFAR_Station_Beamformed_Writer_LuMP2(const LOFAR_Station_Beamformed_Writer_LuMP2& a);
    LOFAR_Station_Beamformed_Writer_LuMP2& operator=(const LOFAR_Station_Beamformed_Writer_LuMP2& a);

    // Stuff for boss to tell pipe_writer thread to do something
    int_fast32_t wait_for_pipe_writer_thread_startup() restrict throw();
    int_fast32_t pipe_writer_thread_constructor() restrict throw();
    int_fast32_t pipe_writer_thread_destructor() restrict throw();
    int_fast32_t pipe_writer_thread_write_buffer() throw();
    int_fast32_t pipe_writer_thread_stop_writing() throw();

    int_fast32_t
    copy_child_process_string_information(const char* const child_process_command_line_,
                                          const char* const child_process_channel_count_multiplier_,
                                          const char* const child_process_sample_blocksize_,
                                          const char* const child_process_logfile_) throw();
    // After return from tokenize_command_line, the caller is responsible
    // for freeing memory pointed to by *memory_p and *argv_p.
    int_fast32_t tokenize_command_line(char** command_p, char*** argv_p, char** memory_p) throw();
    int open_child_logfile() throw();
    pid_t pipe_writer_popen(char *command, char** argv, int child_stdout_fd, int *writefd) throw();

    
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
#endif // LOFAR_Station_Beamformed_Writer_LuMP2_H
