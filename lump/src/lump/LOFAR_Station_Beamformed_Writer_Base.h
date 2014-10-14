// LOFAR_Station_Beamformed_Writer_Base.h
// Base class for Beamformed data writers
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 15  James M Anderson  --MPIfR  Start
// 2011 Mar 25  JMA  --update for revision 5.0 of LOFAR data output
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2011 Sep 28  JMA  --put thread conditions back in to lower CPU usage
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 12  JMA  --add new exit status code
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch,Source info
// 2013 Aug 03  JMA  ---- use defines to prettify mutex logging
// 2013 Aug 19  JMA  ---- record first sample information
// 2014 Aug 29  JMA  ---- add function for writing extra valid time series beamlet
//                        Info stuff

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



#ifndef LOFAR_Station_Beamformed_Writer_Base_H
#define LOFAR_Station_Beamformed_Writer_Base_H

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
#include "LOFAR_Station_Beamformed_Packet.h"
#include "LOFAR_Station_Beamformed_Info.h"
#include <pthread.h>
#include "MPIfR_logging.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {



#define LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Array \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_CHILD_FAILURE             , -9, "LOFAR_WRITER_RETVAL_CHILD_FAILURE") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_PROGRAMMER_ERROR          , -8, "LOFAR_WRITER_RETVAL_PROGRAMMER_ERROR") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_INTEGRATION_DUMP_FAIL     , -7, "LOFAR_WRITER_RETVAL_INTEGRATION_DUMP_FAIL") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_UNKNOWN_WORK              , -6, "LOFAR_WRITER_RETVAL_UNKNOWN_WORK") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_WORK_IS_ZERO              , -5, "LOFAR_WRITER_RETVAL_WORK_IS_ZERO") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_CLOSE_FAIL                , -4, "LOFAR_WRITER_RETVAL_CLOSE_FAIL") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_WRITE_FAIL                , -3, "LOFAR_WRITER_RETVAL_WRITE_FAIL") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_SOCKET_OPEN_FAIL          , -2, "LOFAR_WRITER_RETVAL_SOCKET_OPEN_FAIL") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL            , -1, "LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_GOOD                      ,  0, "LOFAR_WRITER_RETVAL_GOOD") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET       ,  1, "LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION ,  2, "LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION") \


enum LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Str(LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM e) throw();


#define LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_WRITER_BASE_WORK_WAIT                 ,  0, "LOFAR_WRITER_BASE_WORK_WAIT") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_INIT                 ,  1, "LOFAR_WRITER_BASE_WORK_INIT") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_TIME                 ,  2, "LOFAR_WRITER_BASE_WORK_TIME") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_WRITE_DATA           ,  3, "LOFAR_WRITER_BASE_WORK_WRITE_DATA") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA   ,  4, "LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_SHUTDOWN             ,  5, "LOFAR_WRITER_BASE_WORK_SHUTDOWN") \
        LOFAR_ENUM_VALS( LOFAR_WRITER_BASE_WORK_EXIT                 ,  6, "LOFAR_WRITER_BASE_WORK_EXIT") \




enum LOFAR_Station_Beamformed_Writer_Base_Work_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(LOFAR_Station_Beamformed_Writer_Base_Work_Enum e) throw();




#define LOFAR_Channelizer_Enum_Array \
    LOFAR_ENUM_VALS(     LOFAR_CHANNELIZER_NONE                      , 0, "LOFAR_CHANNELIZER_NONE") \
        LOFAR_ENUM_VALS( LOFAR_CHANNELIZER_FFT                       , 1, "LOFAR_CHANNELIZER_FFT") \
        LOFAR_ENUM_VALS( LOFAR_CHANNELIZER_POLYPHASE_FILTER          , 2, "LOFAR_CHANNELIZER_POLYPHASE_FILTER") \
        LOFAR_ENUM_VALS( LOFAR_CHANNELIZER_ENUM_MAX                  , 3, "LOFAR_CHANNELIZER_ENUM_MAX") \


enum LOFAR_Channelizer_Enum {
#define LOFAR_ENUM_VALS(Enum, Val, String) Enum = Val,
    LOFAR_Channelizer_Enum_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict LOFAR_Channelizer_Enum_Str(LOFAR_Channelizer_Enum e) throw();





//_CLASS  LOFAR_Station_Beamformed_Writer_Base
class LOFAR_Station_Beamformed_Writer_Base {
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
    LOFAR_Station_Beamformed_Writer_Base(const char* const restrict filename_base_,
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

    virtual ~LOFAR_Station_Beamformed_Writer_Base();

    int_fast32_t send_data_packets(const uint_fast32_t NUM_PACKETS,
                                   const uint8_t* const restrict valid,
                                   const uint_fast64_t sample_offset,
                                   const RSP_beamformed_packet* const restrict * const restrict data_packets) throw();
    int_fast32_t send_last_data_packet(const uint8_t* const restrict valid,
                                       const uint_fast64_t sample_offset,
                                       const uint_fast64_t sample_stop_offset,
                                       const RSP_beamformed_packet* const restrict * const restrict data_packets) throw();

    int_fast32_t time_block_finished() throw();
    int_fast32_t time_block_initialize(const uint_fast64_t sample_start_offset) throw();
    int_fast32_t wait_for_ready() throw() {return wait_for_thread_sleeping();}

    void* start_thread();
    



                                                                

protected:
    uint_fast64_t num_valid_samples;
    uint_fast64_t num_invalid_samples;
    uint_fast64_t num_packets_received;
    uint_fast64_t first_sample_received;
    uint_fast64_t last_sample_written_plus_1;
    uint_fast64_t num_packet_commands_processed;
    Real64_t LINES_PER_SECOND;
    Real64_t SECONDS_PER_LINE;

    uint_fast64_t current_sample_offset;
    uint_fast64_t current_sample_stop_offset;
    const RSP_beamformed_packet* const restrict * restrict current_data_packets;
    const uint8_t* restrict current_valid;

    const char* restrict filename_base;

    
    const uint32_t reference_time;
    uint32_t reference_time_zero;
    uint_fast32_t PHYSICAL_BEAMLET_OFFSET;
    uint_fast32_t DATA_BEAMLET_LENGTH_CHAR;  // units of char size
                                             // NUM_Blocks*NUM_POL*Voltage_Size
    uint_fast32_t DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR;  // units of char size
                                             // NUM_Blocks*1*Voltage_Size
    
    int_fast32_t thread_running;
    int_fast32_t thread_ready;
    int_fast32_t boss_has_thread_ready_lock;
    int_fast32_t thread_has_thread_ready_lock;

    uint_fast32_t current_num_packets;

    const uint_fast32_t NUM_OUTPUT_CHANNELS;

    int32_t id;

    uint_fast16_t NUM_Output_Beamlets;
    uint_fast32_t* restrict Beamlets_Array;
    uint_fast32_t* restrict Beamlet_Offsets_Array;
    uint_fast32_t* restrict Physical_Subband_Array;
    uint_fast32_t* restrict RCUMODE_Array;
    Real64_t* restrict RightAscension_Array;
    Real64_t* restrict Declination_Array;
    char* restrict * restrict Epoch_Array;
    char* restrict * restrict SourceName_Array;

    uint_fast16_t Voltage_Size;  // bytes per complex voltage datum, single pol
    
    uint_fast16_t NUM_COMPLEX_VOLTAGE_POINTS_PER_BEAMLET; // NUM_Blocks*NUM_POL
    const uint_fast16_t CLOCK_SPEED_IN_MHz;
    const uint_fast16_t NUM_Beamlets;
    const uint_fast16_t NUM_Blocks;
    const LOFAR_raw_data_type_enum DATA_TYPE;
    LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code;

    
    pthread_mutex_t thread_ready_mutex;
    pthread_cond_t go_to_work_condition;
    pthread_t thread;




    void* run_thread();
    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw() = 0;
    int_fast32_t init_writer_processing() throw();
    int_fast32_t wait_for_thread_sleeping() throw();
    //void thread_cpu_affinity() const throw();
    int_fast32_t stop_thread() restrict throw();

    LOFAR_Station_Beamformed_Info Info;






protected:
    // In the function below, date should be provided as
    // the preprocessor defined __DATE__
    // which has a format like "Feb  6 2001"
    void set_info_version_date(const char* const date) throw();
#define LOFAR_STATION_BEAMFORMED_WRITER_BASE_SET_INFO_VERSION_DATE() set_info_version_date(__DATE__)
    // functions to help with debugging mutexes
#define WRITER_BASE_BOSS_LOCK_MUTEX(mutex) boss_lock_mutex(mutex,__FILE__,__LINE__)
    inline void boss_lock_mutex(pthread_mutex_t* restrict mutex_p,
                                const char* const restrict FILE,
                                const int LINE) restrict throw() {
        if((boss_has_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("boss already has lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss locking mutex failed");
            DEF_LOGGING_CRITICAL("boss locking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        boss_has_thread_ready_lock = 1;
        return;
    }
#define WRITER_BASE_THREAD_LOCK_MUTEX(mutex) thread_lock_mutex(mutex,__FILE__,__LINE__)
    inline void thread_lock_mutex(pthread_mutex_t* restrict mutex_p,
                                  const char* const restrict FILE,
                                  const int LINE) restrict throw() {
        if((thread_has_thread_ready_lock)) {
            //DEF_LOGGING_CRITICAL("thread already has lock at %s:%d",FILE,LINE);
            //exit(1);
            return;
        }
        int retval = pthread_mutex_lock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread locking mutex failed");
            DEF_LOGGING_CRITICAL("thread locking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        thread_has_thread_ready_lock=1;
        return;
    }
#define WRITER_BASE_BOSS_UNLOCK_MUTEX(mutex) boss_unlock_mutex(mutex,__FILE__,__LINE__)
    inline void boss_unlock_mutex(pthread_mutex_t* restrict mutex_p,
                                  const char* const restrict FILE,
                                  const int LINE) restrict throw() {
        if(!boss_has_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("boss does not have lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        boss_has_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("boss unlocking mutex failed");
            DEF_LOGGING_CRITICAL("boss unlocking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define WRITER_BASE_THREAD_UNLOCK_MUTEX(mutex) thread_unlock_mutex(mutex,__FILE__,__LINE__)
    inline void thread_unlock_mutex(pthread_mutex_t* restrict mutex_p,
                                    const char* const restrict FILE,
                                    const int LINE) restrict throw() {
        if(!thread_has_thread_ready_lock) {
            DEF_LOGGING_CRITICAL("thread does not have lock at %s:%d",FILE,LINE);
            exit(1);
            return;
        }
        thread_has_thread_ready_lock=0;
        int retval = pthread_mutex_unlock(mutex_p);
        if((retval)) {
            DEF_LOGGING_PERROR("thread unlocking mutex failed");
            DEF_LOGGING_CRITICAL("thread unlocking mutex failed with %d in %s:%d\n", retval, FILE, LINE);
            exit(1);
        }
        return;
    }
#define WRITER_BASE_BOSS_BROADCAST_CONDITION(condition) boss_broadcast_condition(condition,__FILE__,__LINE__)
    inline void boss_broadcast_condition(pthread_cond_t* restrict condition_p,
                                         const char* const restrict FILE,
                                         const int LINE) restrict throw() {
        if((boss_has_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_broadcast_condition failed");
                DEF_LOGGING_CRITICAL("boss_broadcast_condition failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("boss_broadcast_condition called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_BASE_THREAD_BROADCAST_CONDITION(condition) thread_broadcast_condition(condition,__FILE__,__LINE__)
    inline void thread_broadcast_condition(pthread_cond_t* restrict condition_p,
                                           const char* const restrict FILE,
                                           const int LINE) restrict throw() {
        if((thread_has_thread_ready_lock)) {
            int retval = pthread_cond_broadcast(condition_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_broadcast_condition failed");
                DEF_LOGGING_CRITICAL("thread_broadcast_condition failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("thread_broadcast_condition called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_BASE_BOSS_CONDITION_WAIT(condition,mutex) boss_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void boss_condition_wait(pthread_cond_t* restrict condition_p,
                                    pthread_mutex_t* restrict mutex_p,
                                    const char* const restrict FILE,
                                    const int LINE) restrict throw() {
        if((boss_has_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("boss_condition_wait failed");
                DEF_LOGGING_CRITICAL("boss_condition_wait failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("boss_condition_wait called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }
#define WRITER_BASE_THREAD_CONDITION_WAIT(condition,mutex) thread_condition_wait(condition,mutex,__FILE__,__LINE__)
    inline void thread_condition_wait(pthread_cond_t* restrict condition_p,
                                      pthread_mutex_t* restrict mutex_p,
                                      const char* const restrict FILE,
                                      const int LINE) restrict throw() {
        if((thread_has_thread_ready_lock)) {
            int retval = pthread_cond_wait(condition_p, mutex_p);
            if((retval)) {
                DEF_LOGGING_PERROR("thread_condition_wait failed");
                DEF_LOGGING_CRITICAL("thread_condition_wait failed with %d in %s:%d\n", retval, FILE, LINE);
                exit(1);
            }
            return;
        }
        DEF_LOGGING_CRITICAL("thread_condition_wait called but does not have lock at %s:%d\n", FILE, LINE);
        exit(1);
        return;
    }



    void append_valid_information_beamlet_info_to_info_files(void) const restrict throw();
    

private:


    // prevent copying
    LOFAR_Station_Beamformed_Writer_Base(const LOFAR_Station_Beamformed_Writer_Base& a);
    LOFAR_Station_Beamformed_Writer_Base& operator=(const LOFAR_Station_Beamformed_Writer_Base& a);
    

    int_fast32_t wait_for_thread_startup() restrict throw();


    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Writer_Base_H
