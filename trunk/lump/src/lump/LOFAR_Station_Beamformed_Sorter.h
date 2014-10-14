// LOFAR_Station_Beamformed_Sorter.h
// class to handle the sorting of the packets
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 11  James M Anderson  --MPIfR  Start
// 2011 Mar 24  JMA  --update for revision 5.0 of LOFAR data output
// 2013 Jan 06  JMA  --allow multiple input types
// 2013 Jan 10  JMA  --stop checking system clock for file and stdin input
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Mar 10  JMA  ---- add input type FILE_NO_SKIP to prevent seeking in data
// 2013 Aug 03  JMA  ---- use defines to prettify mutex logging
// 2013 Sep 21  JMA  ---- add option to pre-pad to start time if there are
//                        missing packets at the start
// 2014 Apr 05  JMA  ---- fix hardware big in LOFAR stations that produce
//                        timestamps of the form 0xFFFFFFFF on occasion
//                        by checking for this special case
// 2014 Apr 06  JMA  ---- clean up 64 bit times

// Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_Sorter_H
#define LOFAR_Station_Beamformed_Sorter_H

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
#include "LOFAR_Station_Beamformed_Packet.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "MPIfR_logging.h"




// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



enum RSP_beamformed_packet_sorter_status {
    RSP_beamformed_packet_sorter_Programmer_Error = -4,
    RSP_beamformed_packet_sorter_Packet_Gap_Error = -3,
    RSP_beamformed_packet_sorter_Writer_Error = -2,
    RSP_beamformed_packet_sorter_Error = -1,
    RSP_beamformed_packet_sorter_OK = 0,
    RSP_beamformed_packet_sorter_Initing = 1,
    RSP_beamformed_packet_sorter_Inited = 2,
    RSP_beamformed_packet_sorter_Processing = 3
};







#define LOFAR_Station_Beamformed_Sorter_Work_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_WORK_WAIT                 ,  0, "LOFAR_SORTER_WORK_WAIT") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_WORK_READ                 ,  1, "LOFAR_SORTER_WORK_READ") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_WORK_EXIT                 ,  2, "LOFAR_SORTER_WORK_EXIT") 


enum LOFAR_Station_Beamformed_Sorter_Work_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Sorter_Work_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Sorter_Work_Enum_Str(LOFAR_Station_Beamformed_Sorter_Work_Enum e) throw();




#define LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_THREAD_STATUS_ZERO        ,  0, "LOFAR_SORTER_THREAD_STATUS_ZERO") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_THREAD_STATUS_WAIT        ,  1, "LOFAR_SORTER_THREAD_STATUS_WAIT") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_THREAD_STATUS_READ        ,  2, "LOFAR_SORTER_THREAD_STATUS_READ") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_THREAD_STATUS_EXIT        ,  3, "LOFAR_SORTER_THREAD_STATUS_EXIT") 


enum LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Str(LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum e) throw();


#define LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_PACKET_TIME_UNKNOWN     ,  0, "LOFAR_SORTER_PACKET_TIME_UNKNOWN") \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_PACKET_TIME_BEFORE      ,  1, "LOFAR_SORTER_PACKET_TIME_BEFORE") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_PACKET_TIME_IN_RANGE    ,  2, "LOFAR_SORTER_PACKET_TIME_IN_RANGE") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_PACKET_TIME_AFTER       ,  3, "LOFAR_SORTER_PACKET_TIME_AFTER") 


enum LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum_Str(LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum e) throw();




enum proto { UDP, TCP, File };

#define LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_SOCKET_TYPE_UDP     ,  0, "LOFAR_SORTER_SOCKET_TYPE_UDP") \
    LOFAR_ENUM_VALS(     LOFAR_SORTER_SOCKET_TYPE_TCP     ,  1, "LOFAR_SORTER_SOCKET_TYPE_TCP") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_SOCKET_TYPE_FILE    ,  2, "LOFAR_SORTER_SOCKET_TYPE_FILE") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_SOCKET_TYPE_STDIN   ,  3, "LOFAR_SORTER_SOCKET_TYPE_STDIN") \
        LOFAR_ENUM_VALS( LOFAR_SORTER_SOCKET_TYPE_FILE_NO_SKIP,  4, "LOFAR_SORTER_SOCKET_TYPE_FILE_NO_SKIP") 

enum LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Str(LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum e) throw();





//_CLASS  RSP_beamformed_packet_sorter
class RSP_beamformed_packet_sorter {
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
    // Constructor
    RSP_beamformed_packet_sorter(const char * const restrict port_,
                                 const LOFAR_raw_data_type_enum DATA_TYPE_,
                                 const uint_fast16_t NUM_Beamlets_,
                                 const uint_fast16_t SAMPLES_PER_PACKET_,
                                 uint_fast32_t NUM_PACKETS_IN_RING_BUFFER_,
                                 uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE_,
                                 uint_fast32_t NUM_PACKETS_IN_READ_THREAD_BUFFER_,
                                 uint_fast64_t NUM_FIRST_PACKETS_TO_DROP_,
                                 uint_fast32_t MAX_NUM_PACKETS_OVER_TIME_,
                                 
                                 uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                 bool backfill_to_start_time_
                                 );
    // Destructor
    ~RSP_beamformed_packet_sorter();


    // The function to call to have data written out
    int_fast32_t write_data(const int_fast64_t start_time_,
                            const int_fast64_t end_time_,  // Python notation, stop
                                                           // at this second
                            const int_fast64_t reference_time_,
                            uint_fast16_t NUM_WRITERS_,
                            LOFAR_Station_Beamformed_Writer_Base* const restrict * const restrict WRITERS_
                            ) throw();



    RSP_beamformed_packet_sorter_status Final_Status() const throw() {return final_status;}

    void* start_thread();



protected:



private:
    // RSP_beamformed_packet is currently N*16 byte aligned
    RSP_beamformed_packet Compare_Packet_Header;


    uint_fast64_t start_packet;
    uint_fast64_t end_packet;
    uint_fast64_t Last_Packet;             // Index of the last packet which
                                           // either contains partial good
                                           // samples or zero good samples
                                           // if the packet block start is 0
    uint_fast64_t Last_Packet_plus_one;    // Last_Packet+1;
    uint_fast64_t Last_Processing_Sample;  // The sample index of the first
                                           // sample of time end_time, which
                                           // is when we stop. (Python notation)
    uint_fast64_t num_packets_processed;
    uint_fast64_t num_packets_processed_time_block;
    uint_fast64_t num_packets_dropped;
    uint_fast64_t num_packets_dropped_time_block;
    uint_fast64_t num_packets_missing;
    uint_fast64_t num_packets_missing_time_block;
    uint_fast64_t num_packets_duplicate;
    uint_fast64_t num_packets_duplicate_time_block;
    uint_fast64_t num_packets_out_of_order;
    uint_fast64_t num_packets_out_of_order_time_block;
    uint_fast64_t num_packets_overrun;
    uint_fast64_t num_packets_overrun_time_block;
    uint_fast64_t num_packets_hardware_bug;
    uint_fast64_t num_packets_hardware_bug_time_block;

    static const uint_fast32_t LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON = 0xFFFFFFFF;
    
    uint_fast16_t num_major_buffer_flush_skip;
    static const uint_fast16_t MAX_NUM_MAJOR_BUFFER_FLUSH_SKIP = 10;

    uint_fast64_t NUM_FIRST_PACKETS_TO_DROP;
    
    uint8_t* restrict used_flag_ring_buffer;
    uint8_t* restrict used_flag_writing_buffer;
    uint8_t* restrict used_flag_storage_buffer;
    uint_fast64_t* restrict packet_start_sample_ring_buffer;
    uint_fast64_t* restrict packet_index_ring_buffer;
    RSP_beamformed_packet* raw_packet_buffer;
    RSP_beamformed_packet* restrict * restrict spare_packet_stack;
    RSP_beamformed_packet* restrict * restrict packet_ring_buffer;
    RSP_beamformed_packet* restrict * packet_writing_buffer;
    RSP_beamformed_packet* restrict * packet_storage_buffer;
    RSP_beamformed_packet* restrict * read_thread_packet_buffer;
    RSP_beamformed_packet* restrict * boss_thread_packet_buffer;
    void* spare_packet_stack_void;
    LOFAR_Station_Beamformed_Writer_Base* const restrict * restrict WRITERS;

    
    uint_fast32_t PACKET_NUMBER_MASK_32;
    static const uint_fast64_t PACKET_STATISTICS_REPORT_MASK = 0x80000;
                                                             // report every
                                                             // 524288 packets

    uint_fast32_t MAX_NUM_PACKETS_OVER_TIME;

    
    uint_fast32_t NUM_PACKETS_IN_RING_BUFFER; // number of packets total to store
                                              // in ring buffer
    uint_fast32_t NUM_PACKETS_IN_RING_BUFFER_MINUS_1; // in packets
    uint_fast32_t NUM_PACKETS_IN_TWO_SECONDS_TIME;

    uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE;
    const uint_fast16_t NUM_PACKETS_IN_READ_THREAD_BUFFER;
    uint_fast16_t num_packets_boss_buffer;
    uint_fast16_t num_packets_writing_buffer;
    uint_fast16_t num_packets_storage_buffer;
    uint_fast32_t num_packets_read_thread_buffer;
    uint_fast32_t num_packets_packet_stack;
    uint_fast32_t MAX_NUM_PACKETS_PACKET_STACK;
    uint_fast64_t packet_start_sample_writing_buffer;
    uint_fast64_t packet_start_sample_storage_buffer;


    const char * restrict port;
    enum LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum port_proto;
    int socket_in;
    int_fast64_t start_time;
    int_fast64_t end_time;
    int_fast64_t reference_time;
    int_fast64_t reference_time_zero;
    uint_fast32_t time_samples_per_two_seconds;
    uint_fast32_t time_samples_per_even_second;

    uint_fast16_t NUM_WRITERS;
    uint_fast16_t CLOCK_SPEED_IN_MHz;
    uint_fast16_t PACKET_DATA_LENGTH_CHAR;// size of the data payload portion of
                                          // a packet


    const uint8_t NUM_Beamlets;
    const uint8_t SAMPLES_PER_PACKET;

    uint8_t Packet_Sample_Index_Offset;

    bool backfill_to_start_time;  // true means to backfill to the start time
                                  // with blank packets

    int_fast32_t read_thread_running;
    int_fast32_t boss_has_read_thread_lock;
    int_fast32_t read_thread_has_read_thread_lock;


    LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum packet_time_status;
    LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum read_thread_status_code;
    LOFAR_Station_Beamformed_Sorter_Work_Enum boss_thread_work_code;
    const LOFAR_raw_data_type_enum DATA_TYPE;
    RSP_beamformed_packet_sorter_status final_status;

    
    pthread_mutex_t read_thread_mutex;
    pthread_cond_t read_thread_go_to_work_condition;
    pthread_t read_thread;
    void* run_read_thread();  // thread function
    int_fast32_t wait_for_read_thread_startup() restrict throw();
    int_fast32_t wait_for_read_thread_sleeping() throw();
    int_fast32_t wait_for_read_thread_reading() throw();
    int_fast32_t execute_read_thread() restrict throw();  // tell thread to read
    int_fast32_t pause_read_thread() restrict throw();    // tell thread to pause
    int_fast32_t stop_read_thread() restrict throw();     // tell thread to die
    int_fast32_t read_thread_process_packets() restrict throw();


    



    // calculate the sample offset for time t
    uint_fast64_t time_sample_offset(const uint32_t& t) const throw()
    {
        int_fast64_t time_offset = t-reference_time_zero;
        uint_fast64_t count = uint_fast64_t(time_offset >> 1) * time_samples_per_two_seconds;
        if(time_offset & 0x1) {
            // odd number of seconds difference
            count += time_samples_per_even_second;
        }
        return count;
    }
    // set up the reference zero time, using the timestamp of the
    // first packet
    void set_reference_time_zero(const int_fast64_t& This_Time) throw()
    {reference_time_zero = make_reference_time_zero(This_Time); return;}


    //int_fast32_t close_unexpectedly() throw();
    int_fast32_t process_packets() restrict throw();
    
    int_fast32_t copy_initial_incoming_packet_to_ring_buffer(RSP_beamformed_packet * restrict packet) throw();
    int_fast32_t backfill_ring_buffer_with_initial_blank_packets(uint_fast64_t start_sample,
                                                                 uint_fast64_t NUM_BLANK_PACKETS) restrict throw();
    void close_WRITERS() throw();
    void report_bad_packet_config(const RSP_beamformed_packet * const restrict packet) const throw();
    int_fast32_t store_exact_packet(RSP_beamformed_packet * restrict packet,
                                    const uint_fast64_t packet_64_index,
                                    const uint_fast64_t packet_start_sample) throw();
    int_fast32_t process_regular_packet(RSP_beamformed_packet * restrict packet) throw();
    void print_final_statistics() throw();
    void print_intermediate_statistics() throw();
    int_fast32_t flush_ring_buffer_to_storage() throw();
    int_fast32_t flush_storage_to_writers() throw();
    int_fast32_t store_standard_packet(uint8_t valid,
                                       const uint_fast64_t start_sample,
                                       RSP_beamformed_packet* restrict data)
        throw();
    int_fast32_t send_WRITERS_partial_packet(uint8_t valid,
                                             const uint_fast64_t start_sample,
                                             const uint_fast64_t last_sample,
                                             RSP_beamformed_packet* restrict data)
        throw();
    int_fast32_t send_WRITERS_sample_start(const uint_fast64_t start_sample) throw();
    int_fast32_t try_sending_next_ring_packet_to_storage() throw();

    void print_16_hex_chars_to_string(uint_fast64_t x, char* const restrict p) const throw();
    void print_N_hex_chars_to_string(uint_fast64_t x, char* const restrict p, const int N) const throw();

    RSP_beamformed_packet* restrict pop_packet_stack() throw();
    void push_packet_stack(RSP_beamformed_packet* restrict p) throw();
    int_fast32_t check_for_interrupt() restrict throw();
    int interpret_port_info() restrict throw();




protected:
    // functions to help with debugging mutexes
#define SORTER_BOSS_LOCK_MUTEX(mutex) boss_lock_mutex(mutex,__FILE__,__LINE__)
    inline void boss_lock_mutex(pthread_mutex_t* restrict mutex_p,
                                const char* const restrict FILE,
                                const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("boss_lock_mutex %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        if((boss_has_read_thread_lock++)) {
            DEF_LOGGING_CRITICAL("boss locking mutex already has lock in %s:%d\n", FILE, LINE);
            exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss locking mutex failed");
            DEF_LOGGING_CRITICAL("boss locking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        //DEF_LOGGING_DEBUG("boss_lock_mutex got mutex %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        return;
    }
#define SORTER_THREAD_LOCK_MUTEX(mutex) thread_lock_mutex(mutex,__FILE__,__LINE__)
    inline void thread_lock_mutex(pthread_mutex_t* restrict mutex_p,
                                  const char* const restrict FILE,
                                  const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("thread_lock_mutex %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        if((read_thread_has_read_thread_lock++)) {
            DEF_LOGGING_CRITICAL("thread locking mutex already has lock in %s:%d\n", FILE, LINE);
            exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread locking mutex failed");
            DEF_LOGGING_CRITICAL("thread locking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        //DEF_LOGGING_DEBUG("thread_lock_mutex got mutex %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        return;
    }
#define SORTER_BOSS_UNLOCK_MUTEX(mutex) boss_unlock_mutex(mutex,__FILE__,__LINE__)
    inline void boss_unlock_mutex(pthread_mutex_t* restrict mutex_p,
                                  const char* const restrict FILE,
                                  const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("boss_unlock_mutex %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        if(!boss_has_read_thread_lock) {
            return;
        }
        if(--boss_has_read_thread_lock > 0) {
            DEF_LOGGING_CRITICAL("boss unlocking mutex too many locks in %s:%d\n", FILE, LINE);
            exit(1);
            return;
        }
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss unlocking mutex failed");
            DEF_LOGGING_CRITICAL("boss unlocking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        //DEF_LOGGING_DEBUG("boss_unlock_mutex mutex released %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        return;
    }
#define SORTER_THREAD_UNLOCK_MUTEX(mutex) thread_unlock_mutex(mutex,__FILE__,__LINE__)
    inline void thread_unlock_mutex(pthread_mutex_t* restrict mutex_p,
                                    const char* const restrict FILE,
                                    const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("thread_unlock_mutex %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        if(!read_thread_has_read_thread_lock) {
            return;
        }
        if(--read_thread_has_read_thread_lock > 0) {
            DEF_LOGGING_CRITICAL("thread unlocking mutex too many locks in %s:%d\n", FILE, LINE);
            exit(1);
            return;
        }
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread unlocking mutex failed");
            DEF_LOGGING_CRITICAL("thread unlocking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        //DEF_LOGGING_DEBUG("thread_unlock_mutex mutex released %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        return;
    }
#define SORTER_BOSS_BROADCAST_CONDITION(condition) boss_broadcast_condition(condition,__FILE__,__LINE__)
    inline void boss_broadcast_condition(pthread_cond_t* restrict condition_p,
                                         const char* const restrict FILE,
                                         const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("boss_broadcast_condition %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        if((boss_has_read_thread_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_broadcast_condition failed");
                DEF_LOGGING_CRITICAL("boss_broadcast_condition failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            //DEF_LOGGING_DEBUG("boss_broadcast_condition broadcasted %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
            return;
        }
        DEF_LOGGING_CRITICAL("boss_broadcast_condition called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define SORTER_THREAD_BROADCAST_CONDITION(condition) thread_broadcast_condition(condition,__FILE__,__LINE__)
    inline void thread_broadcast_condition(pthread_cond_t* restrict condition_p,
                                           const char* const restrict FILE,
                                           const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("thread_broadcast_condition %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        if((read_thread_has_read_thread_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_broadcast_condition failed");
                DEF_LOGGING_CRITICAL("thread_broadcast_condition failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            //DEF_LOGGING_DEBUG("thread_broadcast_condition broadcasted %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
            return;
        }
        DEF_LOGGING_CRITICAL("thread_broadcast_condition called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define SORTER_BOSS_CONDITION_WAIT(condition,mutex) boss_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void boss_condition_wait(pthread_cond_t* restrict condition_p,
                                    pthread_mutex_t* restrict mutex_p,
                                    const char* const restrict FILE,
                                    const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("boss_condition_wait %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
        if((boss_has_read_thread_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_condition_wait failed");
                DEF_LOGGING_CRITICAL("boss_condition_wait failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            //DEF_LOGGING_DEBUG("boss_condition_wait wait finished %d in %s:%d", int(boss_has_read_thread_lock), FILE, LINE);
            return;
        }
        DEF_LOGGING_CRITICAL("boss_condition_wait called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define SORTER_THREAD_CONDITION_WAIT(condition,mutex) thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void thread_condition_wait(pthread_cond_t* restrict condition_p,
                                      pthread_mutex_t* restrict mutex_p,
                                      const char* const restrict FILE,
                                      const int LINE) restrict throw() {
        //DEF_LOGGING_DEBUG("thread_condition_wait %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
        if((read_thread_has_read_thread_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_condition_wait failed");
                DEF_LOGGING_CRITICAL("thread_condition_wait failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            //DEF_LOGGING_DEBUG("thread_condition_wait wait finished %d in %s:%d", int(read_thread_has_read_thread_lock), FILE, LINE);
            return;
        }
        DEF_LOGGING_CRITICAL("thread_condition_wait called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
    
private:



    uint_fast64_t samples_to_packets(const uint_fast64_t& s) const throw() {
        uint_fast64_t so = s - Packet_Sample_Index_Offset;
        switch(SAMPLES_PER_PACKET) {
        case 16:
            return so >> 4;
        default:
            return so / SAMPLES_PER_PACKET;
        }
        return 0;
    }
    
    uint_fast64_t packets_to_samples(const uint_fast64_t& p) const throw() {
        // integer multiplication is usually fast enough to make
        // it faster to just do a multiplication than to check for
        // special cases.
        return (p * SAMPLES_PER_PACKET) + Packet_Sample_Index_Offset;
    }
    
   bool check_sample_slip(const uint_fast64_t& s) const throw() {
        switch(SAMPLES_PER_PACKET) {
        case 16:
            return ((s & 0xF) != Packet_Sample_Index_Offset);
        default:
            return ((s % SAMPLES_PER_PACKET) != Packet_Sample_Index_Offset);
        }
        return true;
    }


    // Try to guess the integer clock second of a packet based on an expected
    // packet position, and return the packet index for the packet
    uint_fast64_t calculate_expected_packet_64_index(const uint_fast64_t& expected_packet_index,
                                                     const uint32_t& this_block_start,
                                                     uint_fast64_t& packet_start_sample,
                                                     bool& packet_probably_valid) const restrict throw();
    
    
    
    // prevent copying
    RSP_beamformed_packet_sorter(const RSP_beamformed_packet_sorter& a);
    RSP_beamformed_packet_sorter& operator=(const RSP_beamformed_packet_sorter& a);
       
};



// CLASS FUNCTIONS


inline RSP_beamformed_packet* restrict RSP_beamformed_packet_sorter::
pop_packet_stack() restrict throw()
{
    // The caller is responsible for having the mutex lock
    if(num_packets_packet_stack > 0) {
        --num_packets_packet_stack;
        RSP_beamformed_packet* restrict p = spare_packet_stack[num_packets_packet_stack];
        spare_packet_stack[num_packets_packet_stack] = NULL;
        //DEF_LOGGING_DEBUG("popping packet stack, have %u left, popping %p\n", unsigned(num_packets_packet_stack), p);
        return p;
    }
    else {
        DEF_LOGGING_CRITICAL("no spare packets left\n");
        exit(1);
    }
    // never reached
    return NULL;
}


inline void RSP_beamformed_packet_sorter::
push_packet_stack(RSP_beamformed_packet* restrict p) restrict throw()
{
    // The caller is responsible for having the mutex lock
    //DEF_LOGGING_DEBUG("pushing packet stack, have %u now, pushing %p\n", unsigned(num_packets_packet_stack), p);
    if((p==NULL) || (p == &Compare_Packet_Header)) {
        // Don't push a NULL packet or the compare (invalid) packet
        return;
    }
    //DEF_LOGGING_DEBUG("trying to store %16.16p at position %d", reinterpret_cast<void*>(p), int(num_packets_packet_stack));
    if(num_packets_packet_stack < MAX_NUM_PACKETS_PACKET_STACK) {
        if(p != NULL) {
            spare_packet_stack[num_packets_packet_stack++] = p;
        }
        else {
            DEF_LOGGING_ERROR("NULL pointer given to push onto packet stack\n");
        }
    }
    else {
        DEF_LOGGING_CRITICAL("too many packets pushed onto packet stack\n");
        exit(1);
    }
    return;
}


#ifdef LOFAR_Station_Beamformed_Sorter_FILE
#  define NUM(x) =x;
#else
#  define NUM(x) ;
#endif
extern const char RSP_beamformed_packet_sorter_ca[17] NUM("0123456789ABCDEF");
#undef NUM


inline void RSP_beamformed_packet_sorter::
print_16_hex_chars_to_string(uint_fast64_t x, char* const restrict p) const restrict throw()
{
    for(int i=16; i != 0;) {
        uint_fast32_t c = uint_fast32_t(x) & 0xF;
        x >>= 4;
        p[--i] = RSP_beamformed_packet_sorter_ca[c];
    }
    return;
}
inline void RSP_beamformed_packet_sorter::
print_N_hex_chars_to_string(uint_fast64_t x, char* const restrict p, const int N) const restrict throw()
{
    for(int i=N; i != 0;) {
        uint_fast32_t c = uint_fast32_t(x) & 0xF;
        x >>= 4;
        p[--i] = RSP_beamformed_packet_sorter_ca[c];
    }
    return;
}




// HELPER FUNCTIONS



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Sorter_H
