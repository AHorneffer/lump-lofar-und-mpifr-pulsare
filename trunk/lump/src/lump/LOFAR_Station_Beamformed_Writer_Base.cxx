// LOFAR_Station_Beamformed_Writer_Base.cxx
// code for the base writer class
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 16  James M Anderson  --MPIfR  start
// 2011 Aug 06  JMA  --updates for new LuMP pulsar format
// 2011 Sep 28  JMA  --put thread conditions back in to lower CPU usage
// 2012 Apr 19  JMA  --update for Info version 0002
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 05  JMA  --bug fixes for PFB0
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Aug 03  JMA  ---- use defines to prettify mutex logging
// 2013 Aug 09  JMA  ---- change info array data filenames to _lis extension
//                        from _raw
// 2013 Aug 19  JMA  ---- record first sample information
// 2014 Aug 29  JMA  ---- add function for writing extra valid time series beamlet
//                        Info stuff
// 2014 Sep 11  JMA  ---- fix error reporting for bad beamlet numbers



// Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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




// Information file:
// Filename: filename_base.info
// See LOFAR_Station_Beamformed_Info.h for a description of this binary
// header file.
//
// Beamlets file:
// Filename: filename_base.beamlets_lis
// One beamlets file is written out specifying the physical
// beamlet numbers of all beamlets written out by this writer.  The format is
// uint32_t beamlet[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// Subbands file:
// Filename: filename_base.subbands_lis
// One subbands file is written out specifying the physical
// subband numbers of all subbands written out by this writer.  The format is
// uint32_t subband[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// RCUMODEs file:
// Filename: filename_base.rcumodes_lis
// One RCUMODEs file is written out specifying the RCUMODE
// numbers of all beamlets written out by this writer.  The format is
// uint32_t rcumode[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// RightAscensions file:
// Filename: filename_base.rightascensions_lis
// One RightAscensions file is written out specifying the Right Ascension
// values (in units of radians) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// Real64_t rightascension[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// Declinations file:
// Filename: filename_base.declinations_lis
// One Declinations file is written out specifying the Declination
// values (in units of radians) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// Real64_t declination[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// Epochs file:
// Filename: filename_base.epochs_lis
// One Epochs file is written out specifying the Epoch
// values (string value) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// char epoch[beamlets_per_sample][]
// where beamlets_per_sample is obtained from the Information file above.
//
// SourceNames file:
// Filename: filename_base.sourcenames_lis
// One SourceNamess file is written out specifying the SourceName
// values (string value) of all beamlets written out by this writer.
// The format is
// char sourcename[beamlets_per_sample][]
// where beamlets_per_sample is obtained from the Information file above.







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
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include "LOFAR_Station_Swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <string.h>
#include <limits>






// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {


namespace {
extern "C" void* thread_entry(void* pthis)
{
    LOFAR_Station_Beamformed_Writer_Base* object = reinterpret_cast<LOFAR_Station_Beamformed_Writer_Base*>(pthis);
    return object->start_thread();
}

}





// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_Base::
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
                                     const char* const * const restrict SourceName_Array_)
restrict throw() : num_valid_samples(0),
    num_invalid_samples(0),
    num_packets_received(0),
    first_sample_received(0),
    last_sample_written_plus_1(0),
    num_packet_commands_processed(0),
    current_sample_offset(0),
    current_sample_stop_offset(0),
    current_data_packets(NULL),
    current_valid(0),
    reference_time(reference_time_),
    PHYSICAL_BEAMLET_OFFSET(PHYSICAL_BEAMLET_OFFSET_),
    thread_running(0),
    thread_ready(0),
    boss_has_thread_ready_lock(0),
    thread_has_thread_ready_lock(0),
    NUM_OUTPUT_CHANNELS(NUM_OUTPUT_CHANNELS_),
    id(id_),
    NUM_Output_Beamlets(NUM_Output_Beamlets_),
    Beamlets_Array(0),
    Beamlet_Offsets_Array(0),
    Physical_Subband_Array(0),
    RCUMODE_Array(0),
    Voltage_Size(0),
    CLOCK_SPEED_IN_MHz(CLOCK_SPEED_IN_MHz_),
    NUM_Beamlets(NUM_Beamlets_),
    NUM_Blocks(NUM_Blocks_),
    DATA_TYPE(DATA_TYPE_),
    work_code(LOFAR_WRITER_BASE_WORK_WAIT),
    // uncomment the following when finally using a C++11 compiler
    // thread_ready_mutex(PTHREAD_MUTEX_INITIALIZER),
    // go_to_work_condition(PTHREAD_COND_INITIALIZER),
    Info(filename_base_, "wbx")
{
    int retval = pthread_mutex_init(&thread_ready_mutex, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing thread mutex failed");
        DEF_LOGGING_CRITICAL("initializing thread %d mutex failed with %d\n", id, retval);
        exit(1);
    }
    retval = pthread_cond_init(&go_to_work_condition, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing thread cond failed");
        DEF_LOGGING_CRITICAL("initializing thread %d cond failed with %d\n", id, retval);
        exit(1);
    }

    
    {
        size_t len = strlen(filename_base_)+1;
        char* restrict temp = reinterpret_cast<char* restrict>(malloc(len));
        memcpy(temp, filename_base_, len);
        temp[len-1] = 0;
        filename_base = reinterpret_cast<const char* restrict>(temp);
    }

    NUM_COMPLEX_VOLTAGE_POINTS_PER_BEAMLET =
        uint_fast16_t(NUM_Blocks) * NUM_RSP_BEAMLET_POLARIZATIONS;
    Voltage_Size = uint_fast16_t(LOFAR_raw_data_type_enum_size(DATA_TYPE));

    DATA_BEAMLET_LENGTH_CHAR =
        uint_fast32_t(NUM_COMPLEX_VOLTAGE_POINTS_PER_BEAMLET) * Voltage_Size;
    DATA_BEAMLET_SINGLE_POL_LENGTH_CHAR = uint_fast32_t(NUM_Blocks) * Voltage_Size;


    LINES_PER_SECOND = NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz);
    SECONDS_PER_LINE = 1.0/LINES_PER_SECOND;

    reference_time_zero = make_reference_time_zero(reference_time);


    //thread_ready_mutex = PTHREAD_MUTEX_INITIALIZER;




    retval = pthread_create(&thread, NULL, thread_entry, this);    
    if((retval)) {
        DEF_LOGGING_PERROR("creating thread failed");
        DEF_LOGGING_CRITICAL("creating thread %d failed with %d\n", id, retval);
        exit(1);
    }
    retval = int(wait_for_thread_startup());
    if((retval)) {
        DEF_LOGGING_PERROR("wait_for_thread_startup thread failed");
        DEF_LOGGING_CRITICAL("wait_for_thread_startup thread %d failed with %d\n", id, retval);
        exit(1);
    }
   





     Beamlets_Array = reinterpret_cast<uint_fast32_t* restrict>(malloc(sizeof(uint_fast32_t)*NUM_Output_Beamlets));
    if(Beamlets_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for Beamlets_Array buffer\n");
        exit(3);
    }
    Beamlet_Offsets_Array = reinterpret_cast<uint_fast32_t* restrict>(malloc(sizeof(uint_fast32_t)*NUM_Output_Beamlets));
    if(Beamlet_Offsets_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for Beamlet_Offsets_Array buffer\n");
        exit(3);
    }
    Physical_Subband_Array = reinterpret_cast<uint_fast32_t* restrict>(malloc(sizeof(uint_fast32_t)*NUM_Output_Beamlets));
    if(Physical_Subband_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for Physical_Subband_Array buffer\n");
        exit(3);
    }
    RCUMODE_Array = reinterpret_cast<uint_fast32_t* restrict>(malloc(sizeof(uint_fast32_t)*NUM_Output_Beamlets));
    if(RCUMODE_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for RCUMODE_Array buffer\n");
        exit(3);
    }
    RightAscension_Array = reinterpret_cast<Real64_t* restrict>(malloc(sizeof(Real64_t)*NUM_Output_Beamlets));
    if(RightAscension_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for RightAscension_Array buffer\n");
        exit(3);
    }
    Declination_Array = reinterpret_cast<Real64_t* restrict>(malloc(sizeof(Real64_t)*NUM_Output_Beamlets));
    if(Declination_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for Declination_Array buffer\n");
        exit(3);
    }
    Epoch_Array = copy_string_array(NUM_Output_Beamlets, Epoch_Array_);
    if(Epoch_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for Epoch_Array buffer\n");
        exit(3);
    }
    SourceName_Array = copy_string_array(NUM_Output_Beamlets, SourceName_Array_);
    if(SourceName_Array == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for SourceName_Array buffer\n");
        exit(3);
    }
    bool common_RCUMODE = true;
    for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
        RCUMODE_Array[beamlet] = RCUMODE_Array_[beamlet];
        if(RCUMODE_Array[beamlet] != RCUMODE_Array[0]) {
            common_RCUMODE = false;
        }
        if(!LOFAR_RCUMODE_AND_CLOCK_MATCH(RCUMODE_Array[beamlet],CLOCK_SPEED_IN_MHz)) {
            DEF_LOGGING_CRITICAL("RCUMODE %d does not match clock speed in MHz setting %d", int(RCUMODE_Array[beamlet]), int(CLOCK_SPEED_IN_MHz));
            exit(2);
        }
        Beamlets_Array[beamlet] = Physical_Beamlets_Array_[beamlet] - PHYSICAL_BEAMLET_OFFSET_;
        if(Beamlets_Array[beamlet] >= NUM_Beamlets) {
            DEF_LOGGING_CRITICAL("physical beamlet %d, offset beamlet %d is beyond offset beamlet limit %d in writer %d\n", int(Physical_Beamlets_Array_[beamlet]), int(Beamlets_Array[beamlet]), int(NUM_Beamlets), int(id));
            exit(2);
        }
        Beamlet_Offsets_Array[beamlet] = Beamlets_Array[beamlet]*NUM_COMPLEX_VOLTAGE_POINTS_PER_BEAMLET;
        Physical_Subband_Array[beamlet] = Physical_Subband_Array_[beamlet];
        RightAscension_Array[beamlet] = RightAscension_Array_[beamlet];
        Declination_Array[beamlet] = Declination_Array_[beamlet];
    }

    // Set up Info as much as possible
    Info.set_station_name(station_name_);
    if(common_RCUMODE) {
        Info.set_RCUMODE(RCUMODE_Array[0]);
    }
    else {
        Info.set_RCUMODE(-1);
    }
    Info.set_time_reference_zero(int64_t(reference_time_zero));
    Info.set_seconds_per_sample(SECONDS_PER_LINE);
    Info.set_station_clock_frequency(CLOCK_SPEED_IN_MHz);
    Info.set_sampler_frequency(CLOCK_SPEED_IN_MHz*1.0E6);
    if(common_RCUMODE) {
        Info.set_Nyquist_offset(LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[0]));
    }
    else {
        Info.set_Nyquist_offset(-1.0);
    }
    Info.set_beamlets_per_sample(uint32_t(NUM_Output_Beamlets));
    Info.set_GRONINGEN_SAMPLE_COUNT_PER_BLOCK(uint32_t(NUM_Blocks));
    Info.set_station_DATA_TYPE(int32_t(DATA_TYPE));

    Info.write_beamlets_data(filename_base, Physical_Beamlets_Array_);
    Info.write_subbands_data(filename_base, Physical_Subband_Array);
    Info.write_rcumode_data(filename_base, RCUMODE_Array);
    Info.write_rightascension_data(filename_base, RightAscension_Array);
    Info.write_declination_data(filename_base, Declination_Array);
    Info.write_epoch_data(filename_base, Epoch_Array);
    Info.write_sourcename_data(filename_base, SourceName_Array);

    
    return;
}

    
LOFAR_Station_Beamformed_Writer_Base::
~LOFAR_Station_Beamformed_Writer_Base()
{
    stop_thread();
    WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    pthread_mutex_destroy(&thread_ready_mutex);
    pthread_cond_destroy(&go_to_work_condition);
    
    
    DEF_LOGGING_INFO("WRITER %d produced the following statistics:\n", int(id));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX valid samples\n", int(id), (unsigned long long)(num_valid_samples));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX invalid samples\n", int(id), (unsigned long long)(num_invalid_samples));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX packets\n", int(id), (unsigned long long)(num_packets_received));

    free(reinterpret_cast<void*>(Beamlets_Array)); Beamlets_Array=0;
    free(reinterpret_cast<void*>(Beamlet_Offsets_Array)); Beamlet_Offsets_Array=0;
    free(reinterpret_cast<void*>(Physical_Subband_Array)); Physical_Subband_Array=0;
    free(reinterpret_cast<void*>(RCUMODE_Array)); RCUMODE_Array=0;
    free(reinterpret_cast<void*>(RightAscension_Array)); RightAscension_Array=0;
    free(reinterpret_cast<void*>(Declination_Array)); Declination_Array=0;
    free(reinterpret_cast<void*>(Epoch_Array[0])); Epoch_Array[0]=0;
    char** restrict cp = const_cast<char** restrict>(Epoch_Array);
    char** cp2 = const_cast<char**>(cp);
    free(reinterpret_cast<void**>(cp2)); Epoch_Array=0;
    free(reinterpret_cast<void*>(SourceName_Array[0])); SourceName_Array[0]=0;
    cp = const_cast<char** restrict>(SourceName_Array);
    cp2 = const_cast<char**>(cp);
    free(reinterpret_cast<void**>(cp2)); SourceName_Array=0;
    
    return;
}





                      


    


void* LOFAR_Station_Beamformed_Writer_Base::
run_thread() restrict
{
    //DEF_LOGGING_DEBUG("starting run_thread thread %d setting work condition to WAIT\n", id);
    //thread_cpu_affinity();
    WRITER_BASE_THREAD_LOCK_MUTEX(&thread_ready_mutex);
    while(1) {
        // inform the boss that we are ready for new data
        thread_ready = 1;
        //DEF_LOGGING_DEBUG("thread %d setting work condition to WAIT\n", id);
        // wait for an instruction
        while(work_code == LOFAR_WRITER_BASE_WORK_WAIT) {
            WRITER_BASE_THREAD_CONDITION_WAIT(&go_to_work_condition,
                                             &thread_ready_mutex);
        }
        LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_copy = work_code;
        //DEF_LOGGING_DEBUG("thread %d go_to_work %d\n", id, int(work_code));
        thread_ready = 0;
        //DEF_LOGGING_DEBUG("thread %d go_to_work %d\n", int(id), int(work_code));
        WRITER_BASE_THREAD_UNLOCK_MUTEX(&thread_ready_mutex);
        // check the condition value and do some work
        int_fast32_t return_code = do_work(work_copy);
        //DEF_LOGGING_DEBUG("thread %d got %d back from do_work\n", int(id), int(return_code));
        // Now that the work is done, tell the boss that the packets are free
        // to be re-used
        WRITER_BASE_THREAD_LOCK_MUTEX(&thread_ready_mutex);
        work_code = LOFAR_WRITER_BASE_WORK_WAIT;
        if((return_code)) {
            break;
        }
        if(work_copy==LOFAR_WRITER_BASE_WORK_EXIT) {
            break;
        }
    }
    thread_ready=0;
    thread_running=2;
    WRITER_BASE_THREAD_UNLOCK_MUTEX(&thread_ready_mutex);
    //DEF_LOGGING_DEBUG("Thread %d exiting\n", id);
    pthread_exit(NULL);
    // never reached
    return NULL;
}







// void LOFAR_Station_Beamformed_Writer_Base::
// thread_cpu_affinity() const restrict throw()
// {
//     // We can use at most CPU_SETSIZE processors.
//     // Processor 1 is used if we have at least 2 processors for the main
//     // boss thread.  Try to avoid usign that processor.  Processor 0
//     // handles the normal interrup processes (clock, etc.) so avoid that
//     // one too, but to a lesser extent.
//     DEF_LOGGING_INFO("We have %d processors on this computer\n", int(CPU_SETSIZE));
//     if(CPU_SETSIZE >= 2) {
//         int processor = int((id+2)%(CPU_SETSIZE));
//         if(processor == 0) {
//         }
//         else {
//             ++processor;
//         }
//         cpu_set_t mask;
//         CPU_ZERO(&mask);
//         CPU_SET(processor, &mask);
//         int retval = sched_setaffinity(0, sizeof(), &mask);
//         if((retval)) {
//             DEF_LOGGING_PERROR("Error setting processor affinity");
//             DEF_LOGGING_ERROR("Error %d setting processor affinity to %d\n", retval, processor);
//         }
//     }
//     return;
// }









int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
wait_for_thread_startup() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_thread_startup thread %d\n", int(id));
    int_fast32_t return_code = 0;
    while(1) {
        WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_thread_startup 2 thread %d thread_ready %d work %d\n", int(id),int(thread_ready), int(work_code));
        if(thread_running==2) {
            // thread died
            return_code = -1;
            break;
        }
        else if(thread_running == 0) {
            // wait for the thread to finish starting up
        }
        else if((thread_ready)) {
            return_code = 0;
            break;
        }
        WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    }
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return return_code;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
wait_for_thread_sleeping() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_thread_sleeping thread %d\n", int(id));
    while(1) {
        WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_thread_sleeping 2 thread %d thread_ready %d work %d\n", int(id),int(thread_ready), int(work_code));
        if(thread_running==2) {
            // thread died
            return -1;
        }
        else if(thread_running == 0) {
            // thread should already have started, wait_for_thread_startup
            // was called in constructor
            return -2;
        }
        else if(work_code != LOFAR_WRITER_BASE_WORK_WAIT) {
            // wait for the thread to finish
        }
        else if((thread_ready)) {
            return 0;
        }
        WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    }
    // never reached
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
send_data_packets(const uint_fast32_t NUM_PACKETS,
                  const uint8_t* const restrict valid,
                  const uint_fast64_t sample_offset,
                  const RSP_beamformed_packet* const restrict * const restrict data_packets) restrict throw()
{
    //DEF_LOGGING_DEBUG("send_data_packets with %d packets at %llX\n", int(NUM_PACKETS), (unsigned long long)(sample_offset));
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    work_code=LOFAR_WRITER_BASE_WORK_WRITE_DATA;
    current_num_packets=NUM_PACKETS;
    current_valid=valid;
    current_sample_offset=sample_offset;
    current_data_packets=data_packets;
    ++num_packet_commands_processed;

    WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
send_last_data_packet(const uint8_t* const restrict valid,
                      const uint_fast64_t sample_offset,
                      const uint_fast64_t sample_stop_offset,
                      const RSP_beamformed_packet* const restrict * const restrict data_packets) restrict throw()
{
    //DEF_LOGGING_DEBUG("send_last_data_packet at %llX %llX\n", (unsigned long long)(sample_offset), (unsigned long long)(sample_stop_offset));
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    work_code=LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA;
    current_valid=valid;
    current_sample_offset=sample_offset;
    current_sample_stop_offset=sample_stop_offset;
    current_data_packets=data_packets;
    ++num_packet_commands_processed;

    WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return 0;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
init_writer_processing() throw()
{
    //DEF_LOGGING_DEBUG("init_writer_processing\n");
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    work_code=LOFAR_WRITER_BASE_WORK_INIT;
    
    WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("init_writer_processing\n");
}






int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
time_block_finished() restrict throw()
{
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    work_code=LOFAR_WRITER_BASE_WORK_SHUTDOWN;
    
    WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
time_block_initialize(const uint64_t sample_start_offset) restrict throw()
{
    //DEF_LOGGING_DEBUG("time_block_initialize\n");
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    work_code=LOFAR_WRITER_BASE_WORK_TIME;
    current_sample_offset=sample_start_offset;
    first_sample_received=sample_start_offset;
    last_sample_written_plus_1=sample_start_offset;
    
    WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("time_block_initialize\n");
}



void* LOFAR_Station_Beamformed_Writer_Base::
start_thread() restrict
{
    //DEF_LOGGING_DEBUG("start_thread\n");
    WRITER_BASE_THREAD_LOCK_MUTEX(&thread_ready_mutex);
    if(thread_running == 0) {
        // ok, continue
    }
    else {
        WRITER_BASE_THREAD_UNLOCK_MUTEX(&thread_ready_mutex);
        return (void*)(-1);
    }
    thread_running = 1;
    WRITER_BASE_THREAD_UNLOCK_MUTEX(&thread_ready_mutex);
    //DEF_LOGGING_DEBUG("start_thread running thread\n");
    return run_thread();
}
    
    





int_fast32_t LOFAR_Station_Beamformed_Writer_Base::
stop_thread() restrict throw()
{
    DEF_LOGGING_INFO("WRITER %d stopping:\n", int(id));
    {
        int_fast32_t retval = wait_for_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_BASE_BOSS_LOCK_MUTEX(&thread_ready_mutex);
    if(thread_running != 2) {
        work_code=LOFAR_WRITER_BASE_WORK_EXIT;
        
        WRITER_BASE_BOSS_BROADCAST_CONDITION(&go_to_work_condition);
    }
    else {
        // continue
    }
    WRITER_BASE_BOSS_UNLOCK_MUTEX(&thread_ready_mutex);
    
    void* join_retval;
    int retval = pthread_join(thread, &join_retval);
    if((retval)) {
        DEF_LOGGING_PERROR("joining thread failed");
        DEF_LOGGING_CRITICAL("joining thread %d failed with %d\n", int(id), retval);
        exit(1);
    }
    if(join_retval != NULL) {
        DEF_LOGGING_WARNING("thread %d returned %p\n", int(id), join_retval);
    }
    DEF_LOGGING_INFO("WRITER %d thread stopped:\n", int(id));

    if(thread_running == 2) {
        return 0;
    }
    return -4;
}





void LOFAR_Station_Beamformed_Writer_Base::
set_info_version_date(const char* const date) restrict throw()
{
    const size_t DATE_SIZE = 16;
    char newdate[DATE_SIZE];
    newdate[0] = 0;
    const size_t l = strlen(date);
    if(l < 11) {
        // invalid format
        Info.set_version_date(newdate);
        return;
    }
    newdate[0] = date[7];
    newdate[1] = date[8];
    newdate[2] = date[9];
    newdate[3] = date[10];

    newdate[4] = date[0];
    newdate[5] = date[1];
    newdate[6] = date[2];

    newdate[7] = date[4];
    newdate[8] = date[5];

    newdate[9] = 0;

    if(newdate[7] == ' ') {
        newdate[7] = '0';
    }
    
    Info.set_version_date(newdate);
    return;
}






void LOFAR_Station_Beamformed_Writer_Base::
append_valid_information_beamlet_info_to_info_files(void) const restrict throw()
{
    uint32_t MAX_u = std::numeric_limits<uint32_t>::max();
    Real64_t my_nan = nan("");
    static const char None[] = "None";
    Info.append_to_beamlets_data(filename_base, MAX_u);
    Info.append_to_subbands_data(filename_base, MAX_u);
    Info.append_to_rcumode_data(filename_base, RCUMODE_Array[0]);
    Info.append_to_rightascension_data(filename_base, my_nan);
    Info.append_to_declination_data(filename_base, my_nan);
    Info.append_to_epoch_data(filename_base, None);
    Info.append_to_sourcename_data(filename_base, None);
    return;
}
    

















// HELPER FUNCTIONS
const char* const restrict LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Str(LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM";
    }
    return NULL;
}

const char* const restrict LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(LOFAR_Station_Beamformed_Writer_Base_Work_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Writer_Base_Work_Enum";
    }
    return NULL;
}

const char* const restrict LOFAR_Channelizer_Enum_Str(LOFAR_Channelizer_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Channelizer_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Channelizer_Enum";
    }
    return NULL;
}















}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


