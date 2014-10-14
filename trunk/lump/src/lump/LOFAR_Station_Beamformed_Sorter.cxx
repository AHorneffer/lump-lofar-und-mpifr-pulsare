// LOFAR_Station_Beamformed_Sorter.cxx
// code for packet sorting
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 11  James M Anderson  --MPIfR  start
// 2011 Mar 24  JMA  --update for revision 5.0 of LOFAR data output
// 2012 Jan 18  JMA  --create a separate read thread
// 2012 Jan 24  JMA  --tweaking of read_thread stuff
// 2012 Jan 28  JMA  --move socket open and close to the read_thread
// 2013 Jan 05  JMA  --update thread initilization
// 2013 Jan 06  JMA  --allow multiple input types
// 2013 Jan 10  JMA  --stop checking system clock for file and stdin input
// 2013 Mar 07  JMA  ---- bugfix on Major flush die
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Mar 10  JMA  ---- fix packet pushing, count missing packets
// 2013 Mar 10  JMA  ---- add input type FILE_NO_SKIP to prevent seeking in data
// 2013 Aug 03  JMA  ---- use defines to prettify mutex logging
// 2013 Sep 21  JMA  ---- add option to pre-pad to start time if there are
//                        missing packets at the start
// 2014 Apr 05  JMA  ---- fix hardware big in LOFAR stations that produce
//                        timestamps of the form 0xFFFFFFFF on occasion
//                        by checking for this special case
// 2014 Apr 06  JMA  ---- clean up 64 bit times
// 2014 May 04  JMA  ---- fix FILE_NS offset length
// 2014 Sep 21  JMA  --- fix tracking of total hardware bug packets



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


#define LOFAR_Station_Beamformed_Sorter_FILE 1


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
#if(_POSIX_C_SOURCE >= 199309L)
// all ok
#else
#  error "clock_gettime may not be available"
#endif

#include "JMA_math.h"
#include "LOFAR_Station_Beamformed_Sorter.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_Writer_Signals.h"
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>






// set up an unnamed namespace area for internal stuff.
namespace {



// GLOBALS


// FUNCTIONS
int create_input_IP_socket(const char *port, bool use_UDP)
{
  int sk, receive_buffer_size;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family=AF_INET; // 2012 Jan 04: was: AF_UNSPEC;
  hints.ai_socktype = use_UDP ? SOCK_DGRAM : SOCK_STREAM;
  hints.ai_protocol = use_UDP ? IPPROTO_UDP : IPPROTO_TCP; // 2012 Jan 04: was: 0;//use_UDP ? IPPROTO_UDP : IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo* result;
  int retval = getaddrinfo(NULL, port, &hints, &result);
  if (retval != 0) {
      DEF_LOGGING_CRITICAL("getaddrinfo: %s\n", gai_strerror(retval));
      exit(1);
  }
  int sfd;
  struct addrinfo* rp;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
      sfd = socket(rp->ai_family, rp->ai_socktype,
                   rp->ai_protocol);
      if (sfd == -1)
          continue;
      
      if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
          break;                  /* Success */
      
      close(sfd);
  }
  
  if (rp == NULL) {               /* No address succeeded */
      DEF_LOGGING_CRITICAL("Could not bind\n");
      exit(1);
  }
  
  freeaddrinfo(result);           /* No longer needed */
  sk = sfd;

// 2012 Jan 04: comment out chaning receive buffer
  receive_buffer_size = 16 * 1024 * 1024;
  if (setsockopt(sk, SOL_SOCKET, SO_RCVBUF, &receive_buffer_size, sizeof receive_buffer_size) < 0) {
      DEF_LOGGING_PERROR("setsockopt SO_RCVBUF failed:");
      exit(1);
  }
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  if (setsockopt(sk, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) < 0) {
      DEF_LOGGING_PERROR("setsockopt SO_RCVTIMEO failed:");
      exit(1);
  }
  
  return sk;
}
}







// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {


namespace {
extern "C" void* read_thread_entry(void* pthis)
{
    RSP_beamformed_packet_sorter* object = reinterpret_cast<RSP_beamformed_packet_sorter*>(pthis);
    return object->start_thread();
}

}



// GLOBALS


// FUNCTIONS

RSP_beamformed_packet_sorter::
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
                             ) restrict
        : start_packet(0),
          end_packet(0),
          Last_Packet(0),
          Last_Packet_plus_one(1),
          Last_Processing_Sample(0),
          num_packets_processed(0),
          num_packets_processed_time_block(0),
          num_packets_dropped(0),
          num_packets_dropped_time_block(0),
          num_packets_missing(0),
          num_packets_missing_time_block(0),
          num_packets_duplicate(0),
          num_packets_duplicate_time_block(0),
          num_packets_out_of_order(0),
          num_packets_out_of_order_time_block(0),
          num_packets_overrun(0),
          num_packets_overrun_time_block(0),
          num_packets_hardware_bug(0),
          num_packets_hardware_bug_time_block(0),
          num_major_buffer_flush_skip(0),
          NUM_FIRST_PACKETS_TO_DROP(NUM_FIRST_PACKETS_TO_DROP_),
          used_flag_ring_buffer(NULL),
          used_flag_writing_buffer(NULL),
          used_flag_storage_buffer(NULL),
          packet_start_sample_ring_buffer(NULL),
          packet_index_ring_buffer(NULL),
          raw_packet_buffer(NULL),
          spare_packet_stack(NULL),
          packet_ring_buffer(NULL),
          packet_writing_buffer(NULL),
          packet_storage_buffer(NULL),
          read_thread_packet_buffer(NULL),
          boss_thread_packet_buffer(NULL),
          spare_packet_stack_void(NULL),
          MAX_NUM_PACKETS_OVER_TIME(MAX_NUM_PACKETS_OVER_TIME_),
          NUM_PACKETS_IN_RING_BUFFER(NUM_PACKETS_IN_RING_BUFFER_),
          NUM_PACKETS_IN_RING_BUFFER_MINUS_1(NUM_PACKETS_IN_RING_BUFFER_-1),
          NUM_PACKETS_IN_TWO_SECONDS_TIME(0),
          NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE(NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE_),
          NUM_PACKETS_IN_READ_THREAD_BUFFER(NUM_PACKETS_IN_READ_THREAD_BUFFER_),
          num_packets_boss_buffer(0),
          num_packets_writing_buffer(0),
          num_packets_storage_buffer(0),
          num_packets_read_thread_buffer(0),
          num_packets_packet_stack(0),
          MAX_NUM_PACKETS_PACKET_STACK(0),
          packet_start_sample_writing_buffer(0),
          packet_start_sample_storage_buffer(0),
          port(port_),
          socket_in(-1),
          time_samples_per_two_seconds(NUM_TIME_SAMPLES_PER_TWO_SECONDS(CLOCK_SPEED_IN_MHz_)),
          time_samples_per_even_second(NUM_TIME_SAMPLES_PER_EVEN_SECOND(CLOCK_SPEED_IN_MHz_)),
          CLOCK_SPEED_IN_MHz(CLOCK_SPEED_IN_MHz_),
          NUM_Beamlets(NUM_Beamlets_),
          SAMPLES_PER_PACKET(SAMPLES_PER_PACKET_),
          Packet_Sample_Index_Offset(0),
          backfill_to_start_time(backfill_to_start_time_),
          read_thread_running(0),
          boss_has_read_thread_lock(0),
          read_thread_has_read_thread_lock(0),
          packet_time_status(LOFAR_SORTER_PACKET_TIME_UNKNOWN),
          read_thread_status_code(LOFAR_SORTER_THREAD_STATUS_ZERO),
          boss_thread_work_code(LOFAR_SORTER_WORK_WAIT),
          DATA_TYPE(DATA_TYPE_),
          final_status(RSP_beamformed_packet_sorter_Initing)
    // uncomment when using a C++11 compiler
    //      read_thread_mutex(PTHREAD_MUTEX_INITIALIZER),
    //      read_thread_go_to_work_condition(PTHREAD_COND_INITIALIZER)
{
    int retval = pthread_mutex_init(&read_thread_mutex, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing read_thread_mutex failed");
        DEF_LOGGING_CRITICAL("initializing read_thread_mutex failed with %d\n", retval);
        exit(1);
    }
    retval = pthread_cond_init(&read_thread_go_to_work_condition, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing read_thread_go_to_work_condition cond failed");
        DEF_LOGGING_CRITICAL("initializing read_thread_go_to_work_condition cond failed with %d\n", retval);
        exit(1);
    }

    uint_fast16_t NUM_COMPLEX_VOLTAGE_POINTS_PER_PACKET =
        uint_fast16_t(NUM_Beamlets) * SAMPLES_PER_PACKET
        * NUM_RSP_BEAMLET_POLARIZATIONS;
    size_t complex_size = LOFAR_raw_data_type_enum_size(DATA_TYPE);
    PACKET_DATA_LENGTH_CHAR = complex_size * NUM_COMPLEX_VOLTAGE_POINTS_PER_PACKET;
    NUM_PACKETS_IN_TWO_SECONDS_TIME = NUM_TIME_SAMPLES_PER_TWO_SECONDS(CLOCK_SPEED_IN_MHz)/SAMPLES_PER_PACKET;

    // The read thread buffer must be at least 4096 in size
    if(NUM_PACKETS_IN_READ_THREAD_BUFFER < 4096) {
        DEF_LOGGING_CRITICAL("NUM_PACKETS_IN_READ_THREAD_BUFFER (%u) is not at least 4096\n", unsigned(NUM_PACKETS_IN_READ_THREAD_BUFFER));
        exit(1);
    }

    // Get the number of packets to hold in our ring buffer.
    // This must be at least 4, and it must be an exact power of 2
    if(NUM_PACKETS_IN_RING_BUFFER < 4) {
        NUM_PACKETS_IN_RING_BUFFER = 4;
    }
    {
        uint_fast32_t num = 0;
        uint_fast32_t t = NUM_PACKETS_IN_RING_BUFFER;
        while(t != 0x1) {
            t >>= 1;
            num += 1;
        }
        t <<= num;
        if(t != NUM_PACKETS_IN_RING_BUFFER) {
            t <<= 1;
            num += 1;
        }
        if(num > 31) {
            DEF_LOGGING_CRITICAL("NUM_PACKETS_IN_RING_BUFFER too large for uint32_t with %u bits\n", unsigned(num));
            exit(1);
        }
        NUM_PACKETS_IN_RING_BUFFER = t;
        uint64_t PACKET_NUMBER_MASK_64 = 0xFFFFFFFFFFFFFFFFUL;
        PACKET_NUMBER_MASK_64 >>= (64-num);
        PACKET_NUMBER_MASK_32 = uint_fast32_t(PACKET_NUMBER_MASK_64);
    }
    MAX_NUM_PACKETS_PACKET_STACK = NUM_PACKETS_IN_RING_BUFFER
        + 2*NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE
        + 2*NUM_PACKETS_IN_READ_THREAD_BUFFER
        + 2; // padding

    
    used_flag_ring_buffer = reinterpret_cast<uint8_t* restrict>(malloc(MAX_NUM_PACKETS_PACKET_STACK*sizeof(uint8_t)));
    if(used_flag_ring_buffer == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for used_flag_ring_buffer\n");
        exit(3);
    }
    memset(used_flag_ring_buffer,0,MAX_NUM_PACKETS_PACKET_STACK);
    num_packets_packet_stack = MAX_NUM_PACKETS_PACKET_STACK;
    used_flag_writing_buffer = used_flag_ring_buffer + NUM_PACKETS_IN_RING_BUFFER;
    used_flag_storage_buffer = used_flag_writing_buffer + NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE;

    
    packet_start_sample_ring_buffer = reinterpret_cast<uint_fast64_t* restrict>(malloc(NUM_PACKETS_IN_RING_BUFFER*sizeof(uint_fast64_t)));
    if(packet_start_sample_ring_buffer == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for packet sample start ring buffer\n");
        exit(3);
    }
    memset(packet_start_sample_ring_buffer,0,NUM_PACKETS_IN_RING_BUFFER*sizeof(uint_fast64_t));
    packet_index_ring_buffer = reinterpret_cast<uint_fast64_t* restrict>(malloc(NUM_PACKETS_IN_RING_BUFFER*sizeof(uint_fast64_t)));
    if(packet_index_ring_buffer == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for packet_index_ring_buffer\n");
        exit(3);
    }
    memset(packet_index_ring_buffer,0,NUM_PACKETS_IN_RING_BUFFER*sizeof(uint_fast64_t));
    raw_packet_buffer = reinterpret_cast<RSP_beamformed_packet*>(malloc(MAX_NUM_PACKETS_PACKET_STACK*sizeof(RSP_beamformed_packet)));
    if(raw_packet_buffer == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for raw_packet_buffer\n");
        exit(3);
    }
    spare_packet_stack_void = malloc(2*MAX_NUM_PACKETS_PACKET_STACK*sizeof(RSP_beamformed_packet*));
    if(spare_packet_stack_void == NULL) {
        DEF_LOGGING_CRITICAL("unable to allocate memory for spare_packet_stack_void\n");
        exit(3);
    }
    spare_packet_stack = reinterpret_cast<RSP_beamformed_packet* restrict* restrict >(reinterpret_cast<RSP_beamformed_packet**>(spare_packet_stack_void));
    packet_ring_buffer = spare_packet_stack + MAX_NUM_PACKETS_PACKET_STACK;
    packet_writing_buffer = packet_ring_buffer + NUM_PACKETS_IN_RING_BUFFER;
    packet_storage_buffer = packet_writing_buffer + NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE;
    read_thread_packet_buffer = packet_storage_buffer + NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE;
    boss_thread_packet_buffer = read_thread_packet_buffer + NUM_PACKETS_IN_READ_THREAD_BUFFER;
    for(uint32_t i=0; i < MAX_NUM_PACKETS_PACKET_STACK; i++) {
        spare_packet_stack[i] = &(raw_packet_buffer[i]);
    }

    // Set up a flag data area
    if(PACKET_DATA_LENGTH_CHAR > MAX_RSP_PACKET_SIZE_CHAR) {
        DEF_LOGGING_CRITICAL("computed packet size too big for jumbo packets!\n");
        exit(2);
    }
    {
        switch(DATA_TYPE) {
        case L_intComplex8_t:
            {
                Compare_Packet_Header.set_BM(2);
                intComplex8_t* restrict data = reinterpret_cast<intComplex8_t* restrict>(Compare_Packet_Header.data_start());
                for(uint16_t i=0; i < NUM_COMPLEX_VOLTAGE_POINTS_PER_PACKET; i++) {
                    data[i] = intComplex8_t::flag_vlaue();
                }
            }
            break;
        case L_intComplex16_t:
            {
                Compare_Packet_Header.set_BM(1);
                intComplex16_t* restrict data = reinterpret_cast<intComplex16_t* restrict>(Compare_Packet_Header.data_start());
                for(uint16_t i=0; i < NUM_COMPLEX_VOLTAGE_POINTS_PER_PACKET; i++) {
                    data[i] = intComplex16_t::flag_vlaue();
                }
            }
            break;
        case L_intComplex32_t:
            {
                Compare_Packet_Header.set_BM(0);
                intComplex32_t* restrict data = reinterpret_cast<intComplex32_t* restrict>(Compare_Packet_Header.data_start());
                for(uint16_t i=0; i < NUM_COMPLEX_VOLTAGE_POINTS_PER_PACKET; i++) {
                    data[i] = intComplex32_t::flag_vlaue();
                }
            }
            break;
        default:
            {
                DEF_LOGGING_CRITICAL("unknown DATA_TYPE %d\n",
                                     int(DATA_TYPE));
                exit(1);
            }
        }
    }
    {
        switch(CLOCK_SPEED_IN_MHz) {
        case 200:
            Compare_Packet_Header.set_200_MHz_clock();
            break;
        case 160:
            Compare_Packet_Header.set_160_MHz_clock();
            break;
#ifdef LOFAR_DEBUG
        case 0:
            Compare_Packet_Header.set_200_MHz_clock();
            break;
#endif // LOFAR_DEBUG
        default:
            {
                DEF_LOGGING_CRITICAL("unknown CLOCK_SPEED_IN_MHz %d\n",
                                     int(CLOCK_SPEED_IN_MHz));
                exit(1);
            }
        }
    }
    Compare_Packet_Header.set_nBeamlets(NUM_Beamlets);
    Compare_Packet_Header.set_nBlocks(SAMPLES_PER_PACKET);
    Compare_Packet_Header.set_Config(0);
    Compare_Packet_Header.set_Sta(0);
    Compare_Packet_Header.set_data_invalid();
    if(Compare_Packet_Header.packet_data_size() != PACKET_DATA_LENGTH_CHAR) {
        DEF_LOGGING_CRITICAL("Error setting up compare packet header\n");
        exit(1);
    }


    retval = pthread_create(&read_thread, NULL, read_thread_entry, this);    
    if((retval)) {
        DEF_LOGGING_PERROR("creating read_thread failed");
        DEF_LOGGING_CRITICAL("creating read_thread failed with %d\n", retval);
        exit(1);
    }
    retval = int(wait_for_read_thread_startup());
    if((retval)) {
        DEF_LOGGING_PERROR("wait_for_read_thread_startup thread failed");
        DEF_LOGGING_CRITICAL("wait_for_read_thread_startup thread failed with %d\n", retval);
        exit(1);
    }

    

    final_status = RSP_beamformed_packet_sorter_Inited;
    return;
}


RSP_beamformed_packet_sorter::
~RSP_beamformed_packet_sorter()
{
    stop_read_thread();
    //DEF_LOGGING_DEBUG("Destroying mutex and condition\n");
    while(boss_has_read_thread_lock) {
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    pthread_mutex_destroy(&read_thread_mutex);
    pthread_cond_destroy(&read_thread_go_to_work_condition);

    
    free(reinterpret_cast<void*>(used_flag_ring_buffer)); used_flag_ring_buffer=NULL;
    used_flag_writing_buffer = NULL;
    used_flag_storage_buffer = NULL;
    free(packet_start_sample_ring_buffer); packet_start_sample_ring_buffer=NULL;
    free(packet_index_ring_buffer); packet_index_ring_buffer=NULL;
    free(reinterpret_cast<void*>(raw_packet_buffer)); raw_packet_buffer=NULL;
    free(spare_packet_stack_void); spare_packet_stack_void=NULL;
    spare_packet_stack=0;
    packet_ring_buffer=0;
    packet_writing_buffer=0;
    packet_storage_buffer=0;
    read_thread_packet_buffer=0;
    boss_thread_packet_buffer=0;

    // close the socket
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
    if(socket_in >= 0) {
        close(socket_in); socket_in = -1;
    }
#else
    socket_in = -1;
#endif
    return;
}










int_fast32_t RSP_beamformed_packet_sorter::
write_data(const int_fast64_t start_time_,
           const int_fast64_t end_time_,  // Python notation, stop
                                          // at this second
           const int_fast64_t reference_time_,
           uint_fast16_t NUM_WRITERS_,
           LOFAR_Station_Beamformed_Writer_Base* const restrict * const restrict WRITERS_
           ) restrict throw()
{
    //DEF_LOGGING_DEBUG("in write_data\n");
    int_fast32_t return_code = 0;
    int_fast32_t read_return = 0;
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG == 0)
    start_time = start_time_;
    end_time = end_time_;
    reference_time = reference_time_;
#else
    start_time = 0;
    end_time = 0x7FFFFFFF;
    reference_time = 0;
#endif
    DEF_LOGGING_INFO("using start_time=0x%.8X end_time=0x%.8X (duration=%u seconds)", unsigned(start_time), unsigned(end_time), unsigned(end_time-start_time));
    reference_time_zero = 0;
    NUM_WRITERS = NUM_WRITERS_;
    WRITERS = WRITERS_;

    final_status = RSP_beamformed_packet_sorter_Initing;

    // Now set up the line counter information
    if(reference_time > start_time) {
        DEF_LOGGING_ERROR("reference time after start time\n");
        return_code = -20;
        goto write_data_exit;
    }
    set_reference_time_zero(reference_time);
    Last_Processing_Sample = time_sample_offset(end_time);
    //DEF_LOGGING_DEBUG("Last processing line will be %llX\n", (unsigned long long)Last_Processing_Sample);
    

    //DEF_LOGGING_DEBUG("write_data will wait for read_thread sleeping\n");
    read_return = wait_for_read_thread_sleeping();
    if((read_return)) {
        DEF_LOGGING_ERROR("read_thread not sleeping because %d\n", int(read_return));
        return_code = -21;
        goto write_data_exit;
    }

    //DEF_LOGGING_DEBUG("write_data will execute read_thread\n");
    read_return = execute_read_thread();
    if((read_return)) {
        DEF_LOGGING_ERROR("read_thread cannot execute because %d\n", int(read_return));
        return_code = -21;
        goto write_data_exit;
    }

    //DEF_LOGGING_DEBUG("write_data will wait for read_thread reading\n");
    read_return = wait_for_read_thread_reading();
    if((read_return)) {
        DEF_LOGGING_ERROR("read_thread not reading because %d\n", int(read_return));
        return_code = -25;
        goto write_data_exit;
    }


    

    //DEF_LOGGING_DEBUG("write_data will start process_packets\n");
    read_return = process_packets();
    if((read_return)) {
        DEF_LOGGING_ERROR("process_packets failed with  %d\n", int(read_return));
        return_code = -22;
        goto write_data_exit;
    }
    
    SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
    if(read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_EXIT) {
        return_code = -24;
        goto write_data_exit;
    }

    

    
    read_return = pause_read_thread();
    if((read_return)) {
        DEF_LOGGING_ERROR("could not pause read_thread because %d\n", int(read_return));
        return_code = -23;
        goto write_data_exit;
    }
    

    
write_data_exit:
    // in case soemthing goes wrong, make sure the read_thread can do things
    while(boss_has_read_thread_lock) {
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    
    print_final_statistics();
    close_WRITERS();

    start_time = 0;
    end_time = 0;
    reference_time = 0;
    reference_time_zero = 0;
    NUM_WRITERS = 0;
    WRITERS = 0;
    
    return return_code;
}









// int_fast32_t RSP_beamformed_packet_sorter::
// close_unexpectedly() restrict throw()
// {
//     int_fast32_t retval = flush_ring_buffer_to_storage();
//     retval |= flush_storage_to_writers();
//     close_WRITERS();
// TODO: JMA check on close_WRITERS
//     print_final_statistics();
//     done_flag = 1;
//     return retval;
// }







int_fast32_t RSP_beamformed_packet_sorter::
execute_read_thread() restrict throw()
{
    //DEF_LOGGING_DEBUG("execute_read_thread");
    int return_code = 0;
    //DEF_LOGGING_DEBUG("execute_read_thread results read_thread_running=%d boss_thread_work_code=%d read_thread_status_code=%d", int(read_thread_running), int(boss_thread_work_code), int(read_thread_status_code));
    if(read_thread_running == 1) {
    }
    else {
        return_code = -4;
        goto execute_read_thread_exit;
    }
    boss_thread_work_code=LOFAR_SORTER_WORK_READ;
    if(read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_READ) {
        // read thread is already reading, so return
        return_code = 0;
        goto execute_read_thread_exit;
    }
    else if(read_thread_status_code != LOFAR_SORTER_THREAD_STATUS_WAIT) {
        // something is wrong
        return_code = -4;
        goto execute_read_thread_exit;
    }

    SORTER_BOSS_BROADCAST_CONDITION(&read_thread_go_to_work_condition);
    
execute_read_thread_exit:
    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);

    return return_code;
}



int_fast32_t RSP_beamformed_packet_sorter::
pause_read_thread() restrict throw()
{
    //DEF_LOGGING_DEBUG("read_thread pausing:\n");
    // We already have the lock on the read_thread_mutex
    if(read_thread_running == 1) {
    }
    else {
        return -4;
    }
    boss_thread_work_code=LOFAR_SORTER_WORK_WAIT;
    if(read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_WAIT) {
        // read thread is already paused, so return
        return 0;
    }


    SORTER_BOSS_BROADCAST_CONDITION(&read_thread_go_to_work_condition);
    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);

    return wait_for_read_thread_sleeping();
}



int_fast32_t RSP_beamformed_packet_sorter::
stop_read_thread() restrict throw()
{
    //DEF_LOGGING_DEBUG("read_thread stopping:\n");
    SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
    if(read_thread_running != 2) {
        boss_thread_work_code=LOFAR_SORTER_WORK_EXIT;
        SORTER_BOSS_BROADCAST_CONDITION(&read_thread_go_to_work_condition);
    }
    else {
        // should already be stopping
    }
    while(boss_has_read_thread_lock) {
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    void* join_retval;
    int retval = pthread_join(read_thread, &join_retval);
    if((retval)) {
        DEF_LOGGING_PERROR("joining read_thread failed");
        DEF_LOGGING_CRITICAL("joining read_thread failed with %d\n", retval);
        exit(1);
    }
    if(join_retval != NULL) {
        DEF_LOGGING_WARNING("read_thread returned %p\n", join_retval);
    }
    //DEF_LOGGING_DEBUG("reader_thread stopped:\n");

    if(read_thread_running == 2) {
        return 0;
    }
    return -4;
}










int_fast32_t RSP_beamformed_packet_sorter::
wait_for_read_thread_startup() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_read_thread_startup thread\n");
    int_fast32_t return_code = 0;
    while(1) {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        //DEF_LOGGING_DEBUG("wait_for_thread_startup 2 thread thread_ready %d work %d\n",int(thread_ready), int(work_code));
        if(read_thread_running==2) {
            // thread died
            return_code = -1;
            break;
        }
        else if(read_thread_running == 0) {
            // wait for the thread to finish starting up
        }
        else if((read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_WAIT)) {
            // read_thread is ready to go
            return_code = 0;
            break;
        }
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    return return_code;
}

int_fast32_t RSP_beamformed_packet_sorter::
wait_for_read_thread_sleeping() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_read_thread_sleeping thread\n");
    while(1) {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        if(read_thread_running==2) {
            // thread died
            return -1;
        }
        else if(read_thread_running == 0) {
            // thread should already have started, wait_for_thread_startup
            // was called in constructor
            return -2;
        }
        else if(boss_thread_work_code != LOFAR_SORTER_WORK_WAIT) {
            // read_thread should have already been told to sleep
            return -3;
        }
        else if((read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_WAIT)) {
            return 0;
        }
        else if((read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_EXIT)) {
            return -4;
        }
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    // never reached
    return 0;
}







int_fast32_t RSP_beamformed_packet_sorter::
wait_for_read_thread_reading() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_read_thread_reading\n");
    while(1) {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        //DEF_LOGGING_DEBUG("wait_for_read_thread_reading have read_thread_running=%d boss_thread_work_code=%d read_thread_status_code=%d\n",int(read_thread_running), int(boss_thread_work_code), int(read_thread_status_code));
        if(read_thread_running==2) {
            // thread died
            return -1;
        }
        else if(read_thread_running == 0) {
            // thread should already have started, wait_for_thread_startup
            // was called in constructor
            return -2;
        }
        else if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
            // read_thread should have already been told to read
            return -3;
        }
        else if((read_thread_status_code == LOFAR_SORTER_THREAD_STATUS_READ)) {
            return 0;
        }
        else if((read_thread_status_code != LOFAR_SORTER_THREAD_STATUS_WAIT)) {
            return -4;
        }
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    // never reached
    return 0;
}







void* RSP_beamformed_packet_sorter::
start_thread() restrict
{
    //DEF_LOGGING_DEBUG("start_thread\n");
    SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
    if(read_thread_running == 0) {
        // ok, continue
    }
    else {
        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
        return (void*)(-1);
    }


//#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG == 0)
    socket_in = interpret_port_info();
// #elif (LOFAR_DEBUG < 2)
//     socket_in = open(port, O_RDONLY);
// #else
//     DEF_LOGGING_WARNING("Using stdin instead of port %s due to compile DEBUG settings\n", port);
//     socket_in = fileno(stdin);
// #endif
    if(socket_in < 0) {
        DEF_LOGGING_PERROR("Error opening file");
        DEF_LOGGING_CRITICAL("Error opening socket '%s'\n", port);
        exit(2);
    }
    port = NULL;

    

    
    read_thread_running = 1;
    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
    //DEF_LOGGING_DEBUG("start_thread running run_read_thread\n");
    void* retcode = run_read_thread();
    // This area should never be reached.    
    return retcode;
}

void* RSP_beamformed_packet_sorter::
run_read_thread() restrict
{
    //DEF_LOGGING_DEBUG("run_read_thread");
    //thread_cpu_affinity();
    SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
    while(1) {
        // inform the boss that we are ready for new data
        read_thread_status_code = LOFAR_SORTER_THREAD_STATUS_WAIT;
        // wait for an instruction
        while(boss_thread_work_code == LOFAR_SORTER_WORK_WAIT) {
            SORTER_THREAD_CONDITION_WAIT(&read_thread_go_to_work_condition,
                                 &read_thread_mutex);
        }
        if(boss_thread_work_code==LOFAR_SORTER_WORK_EXIT) {
            break;
        }
        if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
            DEF_LOGGING_CRITICAL("unknown boss_thread_work_code %d\n", int(boss_thread_work_code));
            exit(1);
        }
        read_thread_status_code = LOFAR_SORTER_THREAD_STATUS_READ;
        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
        // check the condition value and do some work
        int_fast32_t return_code = read_thread_process_packets();
        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
        if(boss_thread_work_code == LOFAR_SORTER_WORK_READ) {
            boss_thread_work_code = LOFAR_SORTER_WORK_WAIT;
        }
        if((return_code)) {
            break;
        }
    }
    read_thread_status_code=LOFAR_SORTER_THREAD_STATUS_EXIT;
    read_thread_running=2;
    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
    //DEF_LOGGING_INFO("read_thread exiting\n");
    pthread_exit(NULL);
    // never reached
    return NULL;
}












int_fast32_t RSP_beamformed_packet_sorter::
read_thread_process_packets() restrict throw()
// Return code
//  0  All ok
//             Errors
// -1  programmer error
// -2  error reading from input socket
// -3  clock error
// -4  local time is way after the commanded end time
// -5  local time is way too early for the commanded start time
// -6  expected packet size too large
// -7  packet contains wrong data format
// -8  read_thread buffer not empty on start
//             Warnings
// +1  First data after end time    
{
    //DEF_LOGGING_DEBUG("starting read area");
    int_fast32_t return_code = 0;
    uint_fast32_t num_packets_after_end = 0;
    struct timespec last_check_time = {0,0};
    bool store_packet = true;
    RSP_beamformed_packet* restrict packet(0);
    // lock mutex to check times and get packet size
    SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
    const ssize_t EXPECTED_PACKET_SIZE = Compare_Packet_Header.packet_size();
    {
        struct timespec local_start_time;
        int retval = clock_gettime(CLOCK_REALTIME,&local_start_time);
        if((retval)) {
            DEF_LOGGING_PERROR("Error getting local start time in read_thread_process_packets");
            return_code =  -3;
            goto read_thread_process_packets_exit;
        }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 1)
        if((port_proto == LOFAR_SORTER_SOCKET_TYPE_UDP)
          || (port_proto == LOFAR_SORTER_SOCKET_TYPE_TCP)) {
            if((local_start_time.tv_sec - end_time) > 86400) {
                DEF_LOGGING_ERROR("the clock on this computer says it is more than 24 hours after the commanded end time for RSP_beamformed_packet_sorter\n");
                return_code =  -4;
                goto read_thread_process_packets_exit;
            }
            else if(labs(start_time - local_start_time.tv_sec) > 86400) {
                DEF_LOGGING_ERROR("the clock on this computer says it is more than 24 hours before the commanded start time for RSP_beamformed_packet_sorter\n");
                return_code =  -5;
                goto read_thread_process_packets_exit;
            }
            else if((start_time - local_start_time.tv_sec) > 3600) {
                DEF_LOGGING_WARNING("the clock on this computer says it will be at least one hour before we can start recording data\n");
            }
        }
#else
#endif
        if(local_start_time.tv_sec >= end_time) {
            DEF_LOGGING_WARNING("the clock on this computer (0x%8.8X) says it is after the commanded end time (0x%8.8X) for RSP_beamformed_packet_sorter\n", unsigned(local_start_time.tv_sec), unsigned(end_time));
        }
    }

    if(num_packets_read_thread_buffer != 0) {
        DEF_LOGGING_ERROR("read_thread buffer still has %u packets \n", unsigned(num_packets_read_thread_buffer));
        return_code = -8;
        goto read_thread_process_packets_exit;
    }

    packet_time_status = LOFAR_SORTER_PACKET_TIME_UNKNOWN;

    
    // Ok, get the first few packets to drop
#ifdef LOFAR_DEBUG            
    DEF_LOGGING_DEBUG("Expected packet size is %d\n", int(EXPECTED_PACKET_SIZE));
#endif
    if(EXPECTED_PACKET_SIZE > ssize_t(MAX_RSP_PACKET_SIZE_CHAR)) {
        DEF_LOGGING_ERROR("expected packet size %d is too large for \n", int(EXPECTED_PACKET_SIZE));
        return_code =  -6;
        goto read_thread_process_packets_exit;
    }
    packet = pop_packet_stack();
    
    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);


    ssize_t size;
    {
        uint_fast64_t first_packets = NUM_FIRST_PACKETS_TO_DROP;
        while(first_packets) {
            errno = 0;
            size = read(socket_in, reinterpret_cast<void*>(packet), EXPECTED_PACKET_SIZE);
            SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
            
            if(size == EXPECTED_PACKET_SIZE) {
                // process packet
                ++num_packets_processed_time_block;
                ++num_packets_dropped_time_block;
                --first_packets;
                last_check_time.tv_sec = 0;
            }
            else if(size > 0) {
                // packet size wrong
                ++num_packets_processed_time_block;
                ++num_packets_dropped_time_block;
                DEF_LOGGING_WARNING("got bad packet size %d\n", int(size));
                last_check_time.tv_sec = 0;
            }
            else if(size < 0) {
                if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    struct timespec check_time;
                    int retval = clock_gettime(CLOCK_REALTIME,&check_time);
                    if((retval)) {
                        DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                        return_code =  -3;
                        goto read_thread_process_packets_exit;
                    }
                    if(check_time.tv_sec >= end_time) {
                        DEF_LOGGING_ERROR("No data packets arriving, and the current time is after the scan end time.  Bailing.\n");
                        return_code =  -2;
                        goto read_thread_process_packets_exit;
                    }
                    else if(start_time > check_time.tv_sec + 10) {
                        DEF_LOGGING_WARNING("No data packets arriving, but still have %d seconds before start time.\n", int(start_time - check_time.tv_sec));
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        struct timespec delay = {5,0}; // 5 seconds
                        nanosleep(&delay, NULL);

                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                    else if(start_time > check_time.tv_sec) {
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        errno=0;
                        retval = sched_yield();
                        if((retval)) {
                            DEF_LOGGING_PERROR("sched_yield");
                            DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                        }
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                    else {
                        // should be receiving packets.  Have we waited at least 2.0 seconds for data to flow?
                        retval = clock_gettime(CLOCK_MONOTONIC,&check_time);
                        if((retval)) {
                            DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                            return_code =  -3;
                            goto read_thread_process_packets_exit;
                        }
                        if(last_check_time.tv_sec == 0) {
                            // first time through
                            last_check_time = check_time;
                        }
                        else {
                            int_fast64_t ns_diff = int_fast64_t(check_time.tv_sec - last_check_time.tv_sec)* 1000000000L
                                + (check_time.tv_nsec - last_check_time.tv_nsec);
                            if(ns_diff > 2000000000L) {                        
                                DEF_LOGGING_ERROR("Read appears to have exceeded timeout length.\nMake sure the station is sending data, and try again.\n");
                                return_code =  -2;
                                goto read_thread_process_packets_exit;
                            }
                        }
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        errno=0;
                        retval = sched_yield();
                        if((retval)) {
                            DEF_LOGGING_PERROR("sched_yield");
                            DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                        }
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                }
                else if(errno == EINTR) {
                    // process error
                    DEF_LOGGING_PERROR("Read failed for input socket for RSP_beamformed_packet_sorter::process_packets()");
                    DEF_LOGGING_WARNING("Interrupt received??? Will try to continue.\n");
                }
                else {
                    // process error
                    DEF_LOGGING_PERROR("Read failed for input socket for RSP_beamformed_packet_sorter::process_packets()");
                    return_code =  -2;
                    goto read_thread_process_packets_exit;
                }
            }
            else {
                // socket closed on us???
                DEF_LOGGING_ERROR("input socket unexpectedly closed for RSP_beamformed_packet_sorter::process_packets()\n");
                return_code =  -2;
                goto read_thread_process_packets_exit;
            }
            if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
                DEF_LOGGING_INFO("read_thread commanded to stop reading\n");
                return_code = 0;
                goto read_thread_process_packets_exit;
            }
            
            SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
        }
    }

    SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
        
    // Now process packets until we get one in the right time range
    last_check_time.tv_sec = 0;
    num_packets_after_end = 0;
    packet_time_status = LOFAR_SORTER_PACKET_TIME_BEFORE;
    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
    
    {
        while(1) {
            errno = 0;
            size = read(socket_in, reinterpret_cast<void*>(packet), EXPECTED_PACKET_SIZE);
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 4)
#else
            DEF_LOGGING_DEBUG("Got %d bytes\n", int(size));
#endif
            SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
            
            if(size == EXPECTED_PACKET_SIZE) {
                // process packet
                ++num_packets_processed_time_block;
                packet->init();
                uint32_t last_packet_time = packet->uinteger_time();
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 4)
#else
                DEF_LOGGING_DEBUG("Packet time is %llX, start %llX, end %llX\n",
                                  (unsigned long long)(last_packet_time),
                                  (unsigned long long)(start_time),
                                  (unsigned long long)(end_time));
#endif
                if(last_packet_time < start_time) {
                    ++num_packets_dropped_time_block;
                    if(port_proto == LOFAR_SORTER_SOCKET_TYPE_FILE) {
                        // seek forward to approximately two seconds prior
                        // to the start time
                        int_fast64_t offset = start_time - last_packet_time;
                        if(offset > 2) {
                            // worry about packet loss.  Don't skip forward
                            // exactly to the start time distance away
                            // assuming perfect packet transmission.
                            int_fast64_t buffer = 2;
                            if(offset > 100) {
                                buffer = 50;
                            }
                            else if(offset > 10) {
                                buffer = 5;
                            }
                            DEF_LOGGING_INFO("Seeking ahead in file read from time %llX to %llX", (long long)(last_packet_time), (long long)(start_time-buffer));
                            offset -= buffer;
                            offset *= EXPECTED_PACKET_SIZE * NUM_TIME_SAMPLES_PER_EVEN_SECOND(CLOCK_SPEED_IN_MHz);
                            errno = 0;
                            off_t seek_set = lseek(socket_in, off_t(offset), SEEK_CUR);
                            if(seek_set == -1) {
                                DEF_LOGGING_PERROR("lseek SEEK_CUR failed:");
                                DEF_LOGGING_WARNING("could not seek forward %lld bytes in input file\n", (long long)(offset));
                            }
                            // reinstruct operating system to optimize for sequential access
                            int retval = posix_fadvise(socket_in, seek_set, 0, POSIX_FADV_SEQUENTIAL);
                            if((retval)) {
                                DEF_LOGGING_PERROR("posix_fadvise POSIX_FADV_SEQUENTIAL failed:");
                            }
                        }
                    } 
                    // get the next packet
                }
                else if(last_packet_time == LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON) {
                    // hardware bug has produced a timestamp indicating unknown
                    // time.  Ignore the packet for now, prior to starting up
                    // the ring buffer.
                    ++num_packets_dropped_time_block;
                    ++num_packets_hardware_bug_time_block;
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
#else
                    {
                        struct timespec local_time;
                        int retval = clock_gettime(CLOCK_REALTIME,&local_time);
                        if((retval)) {
                            DEF_LOGGING_PERROR("Error getting local time");
                        }
                        else {
                            DEF_LOGGING_DEBUG("Got LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON at current local time       0x%16.16llX        \n", (unsigned long long)(local_time.tv_sec));
                        }
                    }
#endif
                }
                else if(last_packet_time >= end_time) {
                    ++num_packets_dropped_time_block;
                    ++num_packets_after_end;
                    packet_time_status = LOFAR_SORTER_PACKET_TIME_AFTER;
                    if(num_packets_after_end >= MAX_NUM_PACKETS_OVER_TIME) {
                        DEF_LOGGING_WARNING("while waiting for packet timestamps in range, got too many (%u) timestamps after end time\n", unsigned(num_packets_after_end));
                        return_code =  +1;
                        goto read_thread_process_packets_exit;
                    }
                }
                else {
                    // time in correct range, start counting for real
                    packet_time_status = LOFAR_SORTER_PACKET_TIME_IN_RANGE;
                    DEF_LOGGING_INFO("Correct timerange found, starting recording\n");
                    break;
                }
                last_check_time.tv_sec = 0;
            }
            else if(size > 0) {
                // packet size wrong
                ++num_packets_processed_time_block;
                ++num_packets_dropped_time_block;
                DEF_LOGGING_WARNING("got bad packet size %d\n", int(size));
            }
            else if(size < 0) {
                if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    struct timespec check_time;
                    int retval = clock_gettime(CLOCK_REALTIME,&check_time);
                    if((retval)) {
                        DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                        return_code =  -3;
                        goto read_thread_process_packets_exit;
                    }
                    if(check_time.tv_sec >= end_time) {
                        DEF_LOGGING_ERROR("No data packets arriving, and the current time is after the scan end time.  Bailing.\n");
                        return_code =  -2;
                        goto read_thread_process_packets_exit;
                    }
                    else if(start_time > check_time.tv_sec) {
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        errno=0;
                        retval = sched_yield();
                        if((retval)) {
                            DEF_LOGGING_PERROR("sched_yield");
                            DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                        }
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                    else {
                        // should be receiving packets.  Have we waited at least 2.0 seconds for data to flow?
                        retval = clock_gettime(CLOCK_MONOTONIC,&check_time);
                        if((retval)) {
                            DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                            return_code =  -3;
                            goto read_thread_process_packets_exit;
                        }
                        if(last_check_time.tv_sec == 0) {
                            // first time through
                            last_check_time = check_time;
                        }
                        else {
                            int_fast64_t ns_diff = int_fast64_t(check_time.tv_sec - last_check_time.tv_sec)* 1000000000L
                                + (check_time.tv_nsec - last_check_time.tv_nsec);
                            if(ns_diff > 2000000000L) {                        
                                DEF_LOGGING_ERROR("Read appears to have exceeded timeout length.\nMake sure the station is sending data, and try again.\n");
                                return_code =  -2;
                                goto read_thread_process_packets_exit;
                            }
                        }
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        errno=0;
                        retval = sched_yield();
                        if((retval)) {
                            DEF_LOGGING_PERROR("sched_yield");
                            DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                        }
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                }
                else if(errno == EINTR) {
                    // process error
                    DEF_LOGGING_PERROR("Read failed for input socket for RSP_beamformed_packet_sorter::process_packets()");
                    DEF_LOGGING_WARNING("Interrupt received??? Will try to continue.\n");
                }
                else{
                    // process error
                    DEF_LOGGING_PERROR("Read failed for input socket for RSP_beamformed_packet_sorter::process_packets()");
                    return_code =  -2;
                    goto read_thread_process_packets_exit;
                }
            }
            else {
                // socket closed on us???
                DEF_LOGGING_ERROR("input socket unexpectedly closed for RSP_beamformed_packet_sorter::process_packets()\n");
                return_code =  -2;
                goto read_thread_process_packets_exit;
            }
            if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
                DEF_LOGGING_INFO("read_thread commanded to stop reading\n");
                return_code = 0;
                goto read_thread_process_packets_exit;
            }
            
            SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
        }
    }

    // Verify that the packet has the correct attributes
    // Eventually this should just be a direct comparison with
    // the Compare_Packet_Header, but until I implement all of the
    // input characteristics of the packet header, this must be an item
    // by item comparison
    {
        if(packet->nBeamlets() != NUM_Beamlets) {
            DEF_LOGGING_INFO("Packet has wrong number beamlets %d, expected %d", int(packet->nBeamlets()), int(NUM_Beamlets));
            report_bad_packet_config(packet);
            return_code =  -7;
            goto read_thread_process_packets_exit;
        }
        else if(packet->nBlocks() != SAMPLES_PER_PACKET) {
            DEF_LOGGING_INFO("Packet has wrong number of time samples %d", int(packet->nBlocks()));
            report_bad_packet_config(packet);
            return_code =  -7;
            goto read_thread_process_packets_exit;
        }
        else if(packet->BM() != Compare_Packet_Header.BM()) {
            DEF_LOGGING_INFO("Packet has wrong bit depth %d --- it should be %d", int(packet->BM()), int(Compare_Packet_Header.BM()));
            report_bad_packet_config(packet);
            return_code =  -7;
            goto read_thread_process_packets_exit;
        }
        else if(packet->using_200_MHz_clock() != Compare_Packet_Header.using_200_MHz_clock()) {
            DEF_LOGGING_INFO("Packet has wrong clock speed --- it should be %d", int(CLOCK_SPEED_IN_MHz));
            report_bad_packet_config(packet);
            return_code =  -7;
            goto read_thread_process_packets_exit;
        }
        // copy the complete header over to our compare area for following
        // comparisons
        memcpy(reinterpret_cast<void*>(&Compare_Packet_Header),
               reinterpret_cast<void*>(packet),
               packet->packet_header_size());
        // Set the valid flag to invalid for the Compare packet, as
        // this packet is used to fill in invalid or missing packets
        Compare_Packet_Header.set_data_invalid();
    }


    // Ok, now continue to read in the packets until we reach the end of
    // the observing time.  Note that the checks in the loop above waiting
    // for a valid timestamp packet guarantee that the code below and in the
    // other processing thread have a fully valid (no hardware bugs) header
    // to work with.
    last_check_time.tv_sec = 0;
    while(1) {
        if(store_packet) {
            if(num_packets_read_thread_buffer < NUM_PACKETS_IN_READ_THREAD_BUFFER) {
                // still have enough space to store this packet
                read_thread_packet_buffer[num_packets_read_thread_buffer++] =
                    packet;
                packet = pop_packet_stack();
            }
            else {
                if((port_proto == LOFAR_SORTER_SOCKET_TYPE_UDP)
                  || (port_proto == LOFAR_SORTER_SOCKET_TYPE_TCP)) {
                    // do not wait forever for the writers to catch up.
#ifdef LOFAR_DEBUG 
                    const int_fast16_t LOOP_MAX = 1000;
                    for(int_fast16_t loop=0; loop < LOOP_MAX; loop++) {
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        // especially when debugging on a fast machine, yielding
                        // alone may not be enough, so sleep
                        struct timespec delay = {0,30000000}; // 30 ms
                        nanosleep(&delay, NULL);
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);

                        if(num_packets_read_thread_buffer < NUM_PACKETS_IN_READ_THREAD_BUFFER) {
                            break;
                        }
                    }           
#else
                    const int_fast16_t LOOP_MAX = 10;
                    for(int_fast16_t loop=0; loop < LOOP_MAX; loop++) {
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);

                        //DEF_LOGGING_DEBUG("read_thread buffer full, yielding CPU\n");

                        if(loop != LOOP_MAX-1) {
                            errno=0;
                            int retval = sched_yield();
                            if((retval)) {
                                DEF_LOGGING_PERROR("sched_yield");
                                DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                            }
                        }
                        else {
                            // yielding alone may not be enough, so sleep for some time
                            struct timespec delay = {0,30000000}; // 30 ms
                            nanosleep(&delay, NULL);
                        }

                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);

                        if(num_packets_read_thread_buffer < NUM_PACKETS_IN_READ_THREAD_BUFFER) {
                            break;
                        }
                    }
#endif
                }
                else {
                    // we are not constrained by time while reading in the input
                    // wait forever if necessary
                    for(uint_fast64_t loop=0; ; loop++) {
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        if((loop & 0x1F) == 0x1F) {
                            DEF_LOGGING_WARNING("read_thread buffer full, yielding CPU, wait loop count=0x%llX", (unsigned long long)(loop));
                        }
                        struct timespec delay = {0,31250000}; // 31.25 ms
                        nanosleep(&delay, NULL);
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                        if(num_packets_read_thread_buffer < NUM_PACKETS_IN_READ_THREAD_BUFFER) {
                            break;
                        }
                        if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
                            DEF_LOGGING_INFO("read_thread commanded to stop reading\n");
                            return_code = 0;
                            goto read_thread_process_packets_exit;
                        }
                    }
                }
                if(num_packets_read_thread_buffer < NUM_PACKETS_IN_READ_THREAD_BUFFER) {
                    // now have enough space to store this packet
                    read_thread_packet_buffer[num_packets_read_thread_buffer++] =
                        packet;
                    packet = pop_packet_stack();
                }
                else {
                    num_packets_overrun_time_block++;
                }
            }
        }

        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);

        errno = 0;
        size = read(socket_in, reinterpret_cast<void*>(packet), EXPECTED_PACKET_SIZE);
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 4)
#else
        DEF_LOGGING_DEBUG("Got %d bytes\n", int(size));
#endif

        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);

        store_packet = false;
        if(size == EXPECTED_PACKET_SIZE) {
            // process packet
            ++num_packets_processed_time_block;
            packet->init();
            if(RSP_beamformed_packets_have_same_configuration(*packet,
                                                              Compare_Packet_Header)) {
                uint32_t packet_time = packet->uinteger_time();
                //DEF_LOGGING_DEBUG("time is %X\n", (unsigned int)(packet_time));
                if(packet_time < end_time) {
                    // Good packet to process
                    if(packet_time >= start_time) {
                        store_packet = true;
                    }
                    else {
                        // too early, skip packet
                        DEF_LOGGING_WARNING("Packet too early --- dropped\n");
                        ++num_packets_dropped_time_block;
                    }
                }
                else if(packet_time == LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON) {
                    ++num_packets_hardware_bug_time_block;
                    // Attempt to store the packet anyway.
                    store_packet = true;
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
#else
                    {
                        struct timespec local_time;
                        int retval = clock_gettime(CLOCK_REALTIME,&local_time);
                        if((retval)) {
                            DEF_LOGGING_PERROR("Error getting local time");
                        }
                        else {
                            DEF_LOGGING_DEBUG("Got LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON at current local time       0x%16.16llX        \n", (unsigned long long)(local_time.tv_sec));
                        }
                    }
#endif
                }
                else {
                    ++num_packets_after_end;
                    ++num_packets_dropped_time_block;
                    packet_time_status = LOFAR_SORTER_PACKET_TIME_AFTER;
                    if(num_packets_read_thread_buffer > 0) {
                        DEF_LOGGING_INFO("packet received after end_time, waiting for writers to catch up with processing");
                        // the processing threads have not caught up yet.
                        // pause to give the other threads time to catch up
                        SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                        if((port_proto == LOFAR_SORTER_SOCKET_TYPE_UDP)
                          || (port_proto == LOFAR_SORTER_SOCKET_TYPE_TCP)) {
                            // do not wait forever for the writers to catch up.
                            // especially when debugging on a fast machine, yielding
                            // alone may not be enough, so sleep
                            struct timespec delay = {0,30000000}; // 30 ms
                            nanosleep(&delay, NULL);
                        }
                        else {
                            // we are not constrained by time while reading in the input
                            // wait forever if necessary
                            for(uint_fast64_t loop=0; ; loop++) {
                                if((loop & 0x1F) == 0x1F) {
                                    DEF_LOGGING_WARNING("read_thread buffer not empty for packet after end_time, yielding CPU, wait loop count=0x%llX", (unsigned long long)(loop));
                                }
                                struct timespec delay = {0,31250000}; // 31.25 ms
                                nanosleep(&delay, NULL);
                                SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                                if(num_packets_read_thread_buffer == 0) {
                                    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                                    break;
                                }
                                if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
                                    DEF_LOGGING_INFO("read_thread commanded to stop reading\n");
                                    return_code = 0;
                                    goto read_thread_process_packets_exit;
                                }
                                SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                            }
                        }
                        SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                    }
                    else  {
                        // Let other threads work.  For LOFAR, 1 microsecond is
                        // enough to not miss any packets.
                        // especially when debugging on a fast machine, yielding
                        // alone may not be enough, so sleep
                        struct timespec delay = {0,1000}; // 1 \mu s
                        nanosleep(&delay, NULL);
                    }
                    if(num_packets_after_end >= MAX_NUM_PACKETS_OVER_TIME) {
                        DEF_LOGGING_WARNING("While waiting for packet timestamps in range, got limit of timestamps after end time\n");
                        return_code =  0;
                        goto read_thread_process_packets_exit;
                    }
                }
            }
            else {
                report_bad_packet_config(packet);
                ++num_packets_dropped_time_block;
                //return_code =  -7;
                //goto read_thread_process_packets_exit;
            }
            last_check_time.tv_sec = 0;
        }
        else if(size > 0) {
            // packet size wrong
            ++num_packets_processed_time_block;
            ++num_packets_dropped_time_block;
            DEF_LOGGING_ERROR("got bad packet size %d\n", int(size));
        }
        else if(size < 0) {
            if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                struct timespec check_time;
                int retval = clock_gettime(CLOCK_REALTIME,&check_time);
                if((retval)) {
                    DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                    return_code =  -3;
                    goto read_thread_process_packets_exit;
                }
                if(check_time.tv_sec >= end_time) {
                    DEF_LOGGING_ERROR("No data packets arriving, and the current time is after the scan end time.  Bailing.\n");
                    return_code =  0;
                    goto read_thread_process_packets_exit;
                }
                else {
                    // should be receiving packets.  Have we waited at least 2.0 seconds for data to flow?
                    retval = clock_gettime(CLOCK_MONOTONIC,&check_time);
                    if((retval)) {
                        DEF_LOGGING_PERROR("Error getting check_time in read_thread_process_packets");
                        return_code =  -3;
                        goto read_thread_process_packets_exit;
                    }
                    if(last_check_time.tv_sec == 0) {
                        // first time through
                        last_check_time = check_time;
                    }
                    else {
                        int_fast64_t ns_diff = int_fast64_t(check_time.tv_sec - last_check_time.tv_sec)* 1000000000L
                            + (check_time.tv_nsec - last_check_time.tv_nsec);
                        if(ns_diff > 2000000000L) {                        
                            DEF_LOGGING_ERROR("Read appears to have exceeded timeout length.\nMake sure the station is sending data, and try again.\n");
                            return_code =  -2;
                            goto read_thread_process_packets_exit;
                        }
                    }
                    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);
                    errno=0;
                    retval = sched_yield();
                    if((retval)) {
                        DEF_LOGGING_PERROR("sched_yield");
                        DEF_LOGGING_ERROR("read_thread sched_yield failed with %d\n", retval);
                    }
                    SORTER_THREAD_LOCK_MUTEX(&read_thread_mutex);
                }
            }
            else if(errno == EINTR) {
                DEF_LOGGING_PERROR("Read failed for input socket for RSP_beamformed_packet_sorter::process_packets()");
                DEF_LOGGING_WARNING("Interrupt received??? Will try to continue.\n");
            }
        }
        else {
            // socket closed on us???
            DEF_LOGGING_ERROR("input socket unexpectedly closed for RSP_beamformed_packet_sorter::process_packets()\n");
            return_code =  -2;
            goto read_thread_process_packets_exit;
        }
        if(boss_thread_work_code != LOFAR_SORTER_WORK_READ) {
            DEF_LOGGING_INFO("read_thread commanded to stop reading\n");
            return_code = 0;
            goto read_thread_process_packets_exit;
        }
    }

read_thread_process_packets_exit:
    SORTER_THREAD_UNLOCK_MUTEX(&read_thread_mutex);

    return return_code;
}
















int_fast32_t RSP_beamformed_packet_sorter::
process_packets() restrict throw()
// Return code
//  0  All ok
// -1  First data after end time
// -2  error reading from input socket
// -3  clock error
// -4  local time is way after the commanded end time
// -5  local time is way too early for the commanded start time
// -6  expected packet size too large
// -7  packet contains wrong data format
// -8  Too big of a packet gap to be able to deal with
// -9  Already done, cannot continue
// -10 error in initial ring buffer filling
// -11 signal handler error
// -1000 - N  Error writing out to output N
{
    final_status = RSP_beamformed_packet_sorter_Processing;
    int_fast32_t return_code = 0;
    bool initial_packet_data_examined = false;
    bool initial_ring_buffer_filling_done = false;
    uint_fast16_t max_packets_from_read_thread = 0;
    uint_fast64_t loop_counter = 0;


    if(num_packets_boss_buffer != 0) {
        DEF_LOGGING_ERROR("boss sorter buffer still has %u packets \n", unsigned(num_packets_boss_buffer));
        return_code = -8;
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        goto process_packets_exit;
    }
    

    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);

    while(1) {
        //DEF_LOGGING_DEBUG("boss check if packets waiting\n");
        // check if there are data waiting for us
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);

        if(num_packets_processed_time_block >= PACKET_STATISTICS_REPORT_MASK) {
            print_intermediate_statistics();
        }
        
        //DEF_LOGGING_DEBUG("read_thread_status_code is %d\n", int(read_thread_status_code));
        //DEF_LOGGING_DEBUG("boss found %u packets in read_thread\n", unsigned(num_packets_read_thread_buffer));
        if(num_packets_read_thread_buffer > 0) {
            // swap buffer pointers
            RSP_beamformed_packet* restrict * temp = boss_thread_packet_buffer;
            boss_thread_packet_buffer = read_thread_packet_buffer;
            read_thread_packet_buffer = temp;
//             memcpy(reinterpret_cast<void*>(boss_thread_packet_buffer),
//                    reinterpret_cast<void*>(read_thread_packet_buffer),
//                    num_packets_read_thread_buffer*sizeof(RSP_beamformed_packet*));
            num_packets_boss_buffer = num_packets_read_thread_buffer;
            num_packets_read_thread_buffer = 0;
        }
        else if(read_thread_status_code != LOFAR_SORTER_THREAD_STATUS_READ) {
            // the read thread has stopped reading
            DEF_LOGGING_INFO("read_thread status_code is %s, stopping process_packets\n", LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Str(read_thread_status_code));
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            break;
        }

        //DEF_LOGGING_DEBUG("boss found %u packets waiting\n", unsigned(num_packets_boss_buffer));

        // Give up the lock, so that the read_thread can work
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);

        // Are there packets to process?
        if(num_packets_boss_buffer > 0) {
            if(num_packets_boss_buffer > max_packets_from_read_thread) {
                max_packets_from_read_thread = num_packets_boss_buffer;
            }
            if(initial_packet_data_examined) {
            }
            else {
                // What is the initial block number for this second?
                uint_fast32_t initial_block_number = boss_thread_packet_buffer[0]->Block_Start();
                uint_fast64_t this_start_sample = time_sample_offset(boss_thread_packet_buffer[0]->uinteger_time()) + initial_block_number;
                Packet_Sample_Index_Offset = uint8_t(this_start_sample % SAMPLES_PER_PACKET);
                uint_fast64_t block_offset_from_start = uint_fast64_t(initial_block_number) / SAMPLES_PER_PACKET;
                bool need_to_pre_fill_with_blank_packets = false;
                if((boss_thread_packet_buffer[0]->uinteger_time() == start_time)
                  && (block_offset_from_start == 0)) {
                    // started on-time
                }
                else if(boss_thread_packet_buffer[0]->uinteger_time() > start_time) {
                    DEF_LOGGING_WARNING("data start second (0x%8.8X) is %u seconds after commanded start second (0x%8.8X)", unsigned(boss_thread_packet_buffer[0]->uinteger_time()), unsigned(boss_thread_packet_buffer[0]->uinteger_time()-start_time), unsigned(start_time));
                    if(backfill_to_start_time) {
                        DEF_LOGGING_WARNING("time gap too large to backfill with blank packets");
                    }
                }
                else {
                    if(block_offset_from_start >= NUM_PACKETS_IN_RING_BUFFER-1) {
                        DEF_LOGGING_WARNING("initial packet is %u packets after the initial packet this second --- time gap too large to backfill with blank packets", unsigned(block_offset_from_start));
                    }
                    else {
                        need_to_pre_fill_with_blank_packets = true;
                        this_start_sample -= block_offset_from_start * SAMPLES_PER_PACKET;
                    }
                }
                send_WRITERS_sample_start(this_start_sample);

                uint_fast64_t l_packet_64_index = samples_to_packets(Last_Processing_Sample);
                uint_fast64_t l_packet_sample = packets_to_samples(l_packet_64_index);
                if(l_packet_sample == Last_Processing_Sample) {
                    l_packet_64_index--;
                }
                Last_Packet = l_packet_64_index;
                Last_Packet_plus_one = Last_Packet+1;
                initial_packet_data_examined = true;

                if(need_to_pre_fill_with_blank_packets) {
                    if((return_code = backfill_ring_buffer_with_initial_blank_packets(this_start_sample, block_offset_from_start)) != 0) {
                        goto process_packets_exit;
                    }
                }
            }
            if(initial_ring_buffer_filling_done) {
                // skip internal check for initial filling
                for(uint_fast16_t p =0; p < num_packets_boss_buffer; p++) {
                    boss_thread_packet_buffer[p]->convert_data_to_host_format();

                    if((return_code = process_regular_packet(boss_thread_packet_buffer[p])) != 0) {
                        goto process_packets_exit;
                    }
                }
            }
            else {
                // do the check packet by packet to see if the initialization
                // still needs to be done
                for(uint_fast16_t p =0; p < num_packets_boss_buffer; p++) {
                    boss_thread_packet_buffer[p]->convert_data_to_host_format();

                    if(initial_ring_buffer_filling_done) {
                        if((return_code = process_regular_packet(boss_thread_packet_buffer[p])) != 0) {
                            goto process_packets_exit;
                        }
                    }
                    else {
                        int_fast32_t retval = copy_initial_incoming_packet_to_ring_buffer(boss_thread_packet_buffer[p]);
                        //DEF_LOGGING_DEBUG("got %d back\n", int(retval));
                        if(retval > 0) {
                            initial_ring_buffer_filling_done = true;
                            if((return_code = process_regular_packet(boss_thread_packet_buffer[p])) != 0) {
                                goto process_packets_exit;
                            }

                        }
                        else if(retval < 0) {
                            // some sort of error
                            return_code =  -10;
                            goto process_packets_exit;
                        }
                    }
                }
            }
            if(num_packets_boss_buffer < 10) {
                // only a few packets were processed.  Pause to give
                // the read_thread time to collect more
                //DEF_LOGGING_DEBUG("boss thread yielding\n");
                //struct timespec delay = {0,100000}; // 0.1 ms
                //nanosleep(&delay, NULL);
                errno=0;
                int retval = sched_yield();
                if((retval)) {
                    DEF_LOGGING_PERROR("sched_yield");
                    DEF_LOGGING_ERROR("boss sched_yield failed with %d\n", retval);
                }
                
            }
            num_packets_boss_buffer = 0;
            if((loop_counter++ & 0xF) == 0) {
                if(check_for_interrupt() != 0) {
                    DEF_LOGGING_WARNING("signal handler found interrupt\n");
                    return_code =  -11;
                    goto process_packets_exit;
                }
                //DEF_LOGGING_DEBUG("Found %llu packets in read_thread buffer this time around\n", (unsigned long long)(num_packets_boss_buffer));
            }
        }
        else {
            // no packets to process
            //DEF_LOGGING_DEBUG("boss thread yielding\n");
            //struct timespec delay = {0,100000}; // 0.1 ms
            //nanosleep(&delay, NULL);
            errno=0;
            int retval = sched_yield();
            if((retval)) {
                DEF_LOGGING_PERROR("sched_yield");
                DEF_LOGGING_ERROR("boss sched_yield failed with %d\n", retval);
            }
        }
    }

process_packets_exit:
    if(return_code == 1) {
        // return code of 1 is the magic code that says the end time
        // has been reached
        DEF_LOGGING_INFO("last packet processed by boss reader thread");
    }
    flush_ring_buffer_to_storage();
    flush_storage_to_writers();
    print_intermediate_statistics();
    DEF_LOGGING_INFO("Packet sorter had max %u packets at once from read_thread", unsigned(max_packets_from_read_thread));
    if(return_code < 0) {
        DEF_LOGGING_WARNING("process_packets finishing with return code %" PRIdFAST32 ", final_status %d", return_code, int(final_status));
        if(int_fast32_t(final_status) < 0) {}
        else {
            final_status = RSP_beamformed_packet_sorter_Error;
        }
    }
    else if(return_code > 1) {
        DEF_LOGGING_WARNING("process_packets finishing with %" PRIdFAST32, return_code);
        return_code = 0;
    }
    else {
        if(return_code == 1) {
            // return code of 1 is the magic code that says the end time
            // has been reached
            return_code = 0;
        }
        final_status = RSP_beamformed_packet_sorter_OK;
    }
    return return_code;
}







int_fast32_t RSP_beamformed_packet_sorter::
copy_initial_incoming_packet_to_ring_buffer(RSP_beamformed_packet * restrict packet) restrict throw()
{
    uint_fast64_t packet_start_sample =
        time_sample_offset(packet->uinteger_time())
        + packet->Block_Start();
    //uint_fast64_t packet_end_sample = packet_start_sample + SAMPLES_PER_PACKET;
    uint_fast64_t packet_64_index = samples_to_packets(packet_start_sample);
    uint_fast32_t packet_index = uint_fast32_t(packet_64_index) & PACKET_NUMBER_MASK_32;
    //DEF_LOGGING_DEBUG("copy_initial_incoming_packet_to_ring_buffer packet start %llX\n", (unsigned long long)packet_start_sample);
    //DEF_LOGGING_DEBUG("copy_initial_incoming_packet_to_ring_buffer packet_index  %X\n", (unsigned)packet_index);
    //DEF_LOGGING_DEBUG("copy_initial_incoming_packet_to_ring_buffer packet_64_index %llX Last_Packet %llX\n", (unsigned long long)packet_64_index, (unsigned long long)Last_Packet);
    //DEF_LOGGING_DEBUG("copy_initial_incoming_packet_to_ring_buffer start_packet %llX end_packet %llX\n", (unsigned long long)start_packet, (unsigned long long)end_packet);
#ifdef LOFAR_DEBUG    
    if(check_sample_slip(packet_start_sample)) {
        DEF_LOGGING_ERROR("packet shift not allowed\n");
        DEF_LOGGING_DEBUG("info 0x8.8X 0x%8.8X %d", unsigned(packet_start_sample), unsigned(packet->uinteger_time()), int(packet->Block_Start()));
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        push_packet_stack(packet);
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        return -1;
    }
#endif // LOFAR_DEBUG
    if(packet_64_index > Last_Packet) {
        if(packet->uinteger_time() == LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON) {
            // hardware bug affecting the timestamp.  Try to fix.
            // Note that we are guaranteed by the ??? function that at least one
            // valid timestamped packet has arrived prior to this packet, so
            // end_packet is valid.
            bool packet_probably_valid = false;
            packet_64_index = calculate_expected_packet_64_index(end_packet,
                                                                 packet->Block_Start(),
                                                                 packet_start_sample,
                                                                 packet_probably_valid);
            packet_index = uint_fast32_t(packet_64_index) & PACKET_NUMBER_MASK_32;
            if(!packet_probably_valid) {
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
#else
                DEF_LOGGING_WARNING("LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON could not have timestamp identified, start_packet 0x%llX end_packet 0x%llX Block_Start 0x%X Packet_Sample_Index_Offset 0x%X\n", (unsigned long long)start_packet, (unsigned long long)end_packet, unsigned(packet->Block_Start()), unsigned(Packet_Sample_Index_Offset));
#endif
                packet->set_data_invalid();
            }
        }
        else {
            // packet after end time
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            push_packet_stack(packet);
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            return +1;
        }
    }
    if((start_packet==0) && (end_packet==0)) {
        //DEF_LOGGING_DEBUG("first packet\n");
        // first packet, simple
        packet_ring_buffer[packet_index] = packet;
        used_flag_ring_buffer[packet_index] = 1;
        packet_start_sample_ring_buffer[packet_index] = packet_start_sample;
        packet_index_ring_buffer[packet_index] = packet_64_index;
        start_packet = packet_64_index;
        end_packet = packet_64_index+1;
        return 0;
    }
    else if(packet_64_index >= end_packet) {
        //DEF_LOGGING_DEBUG("case 1\n");
        if(packet_64_index - start_packet < NUM_PACKETS_IN_RING_BUFFER) {
            //DEF_LOGGING_DEBUG("case 2\n");
            if(!used_flag_ring_buffer[packet_index]) {
            }
            else {
                // duplicate packet
                ++num_packets_duplicate_time_block;
                
                // If the new packet is valid,
                // return the previous packet to the stack.
                SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                if(packet->data_are_valid()) {
                    push_packet_stack(packet_ring_buffer[packet_index]);
                }
                else {
                    push_packet_stack(packet);
                    packet = packet_ring_buffer[packet_index];
                }
                SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            }
            packet_ring_buffer[packet_index] = packet;
            used_flag_ring_buffer[packet_index] = 1;
            packet_start_sample_ring_buffer[packet_index] =packet_start_sample;
            packet_index_ring_buffer[packet_index] = packet_64_index;
            end_packet = packet_64_index+1;
            return 0;
            
        }
        else {
            //DEF_LOGGING_DEBUG("case 3\n");
            // too far in time after the start, fall back to caller
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            push_packet_stack(packet);
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            return +1;
        }
    }
    else if(packet_64_index <= start_packet) {
        //DEF_LOGGING_DEBUG("case 4\n");
        ++num_packets_out_of_order_time_block;
        if(end_packet - packet_64_index < NUM_PACKETS_IN_RING_BUFFER) {
            //DEF_LOGGING_DEBUG("case 5\n");
            if(!used_flag_ring_buffer[packet_index]) {
            }
            else {
                DEF_LOGGING_DEBUG("valid packet in initial ring filling prior to start packet should never happen --- programmer error");
                // duplicate packet
                ++num_packets_duplicate_time_block;

                // If the new packet is valid,
                // return the previous packet to the stack
                SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                if(packet->data_are_valid()) {
                    push_packet_stack(packet_ring_buffer[packet_index]);
                }
                else {
                    push_packet_stack(packet);
                    packet = packet_ring_buffer[packet_index];
                }
                SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            }
            packet_ring_buffer[packet_index] = packet;
            used_flag_ring_buffer[packet_index] = 1;
            packet_start_sample_ring_buffer[packet_index] =packet_start_sample;
            packet_index_ring_buffer[packet_index] = packet_64_index;
            if(packet_64_index < start_packet) {
                start_packet = packet_64_index;
            }
            return 0;
        }
        else {
            //DEF_LOGGING_DEBUG("packet dropped case 6\n");
            // too early, need to dump
            ++num_packets_dropped_time_block;

            // return the packet to the stack
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            push_packet_stack(packet);
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            return 0;
        }
    }
    else {
        ++num_packets_out_of_order_time_block;
        //DEF_LOGGING_DEBUG("case 7\n");
        if(!used_flag_ring_buffer[packet_index]) {
        }
        else {
            // duplicate packet
            ++num_packets_duplicate_time_block;
            
            // If the new packet is valid,
            // return the previous packet to the stack
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            if(packet->data_are_valid()) {
                push_packet_stack(packet_ring_buffer[packet_index]);
            }
            else {
                push_packet_stack(packet);
                packet = packet_ring_buffer[packet_index];
            }
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        }
        packet_ring_buffer[packet_index] = packet;
        used_flag_ring_buffer[packet_index] = 1;
        packet_start_sample_ring_buffer[packet_index] = packet_start_sample;
        packet_index_ring_buffer[packet_index] = packet_64_index;
        return 0;
    }
    return -10;
}







int_fast32_t RSP_beamformed_packet_sorter::
backfill_ring_buffer_with_initial_blank_packets(uint_fast64_t start_sample,
                                                uint_fast64_t NUM_BLANK_PACKETS) restrict throw()
{
    uint_fast64_t packet_start_sample = start_sample;
    uint_fast64_t packet_64_index = samples_to_packets(packet_start_sample);
    uint_fast32_t packet_index = uint_fast32_t(packet_64_index) & PACKET_NUMBER_MASK_32;
#ifdef LOFAR_DEBUG    
    if(check_sample_slip(packet_start_sample)) {
        DEF_LOGGING_ERROR("packet shift not allowed\n");
        DEF_LOGGING_DEBUG("info 0x%8.8X", unsigned(packet_start_sample));
        return -1;
    }
#endif // LOFAR_DEBUG
    if((start_packet != 0) || (end_packet != 0)) {
        DEF_LOGGING_ERROR("backfill_ring_buffer_with_initial_blank_packets should only be called prior to start of filling ring buffer with actual packet data");
        return -1;
    }
    if(NUM_BLANK_PACKETS >= NUM_PACKETS_IN_RING_BUFFER) {
        DEF_LOGGING_ERROR("too many blank packets (%llu) to backfill into ring buffer", (unsigned long long)(NUM_BLANK_PACKETS));
        return -1;
    }
    start_packet = packet_64_index;
    for(uint_fast32_t i=0; i < NUM_BLANK_PACKETS; i++) {
        uint_fast64_t this_packet_start_sample = packet_start_sample + i*SAMPLES_PER_PACKET;
        packet_64_index = samples_to_packets(this_packet_start_sample);
        packet_index = uint_fast32_t(packet_64_index) & PACKET_NUMBER_MASK_32;
        used_flag_ring_buffer[packet_index] = 0;
        packet_start_sample_ring_buffer[packet_index] = packet_start_sample;
        packet_index_ring_buffer[packet_index] = packet_64_index;
    }
    end_packet = packet_64_index+1;
    num_packets_missing_time_block += NUM_BLANK_PACKETS;
    return 0;
}




int_fast32_t RSP_beamformed_packet_sorter::
flush_storage_to_writers()
    restrict throw()
{
    //DEF_LOGGING_DEBUG("Flushing storage buffer\n");
    //DEF_LOGGING_DEBUG("Flushing storage stats writing used %d storage used %d\n", int(num_packets_writing_buffer), int(num_packets_storage_buffer));
    //DEF_LOGGING_DEBUG("sorter trying to send %d packets to writers\n", int(num_packets_storage_buffer));
    //DEF_LOGGING_DEBUG("Flushing storage buffer --- waiting for ready\n");
    int_fast32_t bad_flag = 0;
    // make sure the writers are done with the writing data area
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->wait_for_ready();
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    if((bad_flag)) {
        DEF_LOGGING_WARNING("got bad_flag=%d", int(bad_flag));
        return bad_flag;
    }
    {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        //DEF_LOGGING_DEBUG("Flushing storage buffer --- clear writing area\n");
        for(uint_fast32_t i=0; i < num_packets_writing_buffer; i++) {
            push_packet_stack(packet_writing_buffer[i]);
            packet_writing_buffer[i] = NULL;
            used_flag_writing_buffer[i]=0;
        }
        //DEF_LOGGING_DEBUG("Flushing storage buffer --- check invalid data\n");
        for(uint_fast32_t i=0; i < num_packets_storage_buffer; i++) {
            if((used_flag_storage_buffer[i])) {
                if(packet_storage_buffer[i]->data_are_invalid()) {
                    ++num_packets_dropped_time_block;
                    push_packet_stack(packet_storage_buffer[i]);
                    packet_storage_buffer[i] = &Compare_Packet_Header;
                    used_flag_storage_buffer[i] = 0;
                }
            }
        }
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    num_packets_writing_buffer = 0;
    packet_start_sample_writing_buffer = 0;
    //DEF_LOGGING_DEBUG("Flushing storage check stats writing used %d storage used %d\n", int(num_packets_writing_buffer), int(num_packets_storage_buffer));
    if(num_packets_storage_buffer == 0) {
        //DEF_LOGGING_DEBUG("Flushing storage buffer --- no storage to send, done\n");
        return 0;
    }
    //DEF_LOGGING_DEBUG("Flushing storage buffer --- write storage\n");
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->send_data_packets(num_packets_storage_buffer,
                                                            used_flag_storage_buffer,
                                                            packet_start_sample_storage_buffer,
                                                            packet_storage_buffer);
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    //DEF_LOGGING_DEBUG("Flushing storage buffer --- swap storage/write\n");
    // swap storage and writing areas
    {
        uint8_t* restrict used_flag_temp = used_flag_writing_buffer;
        RSP_beamformed_packet* restrict * restrict packet_temp = packet_writing_buffer;
        uint_fast16_t num_temp = num_packets_writing_buffer;
        uint_fast64_t packet_start_sample_temp = packet_start_sample_writing_buffer;
        used_flag_writing_buffer = used_flag_storage_buffer;
        packet_writing_buffer = packet_storage_buffer;
        num_packets_writing_buffer = num_packets_storage_buffer;
        packet_start_sample_writing_buffer = packet_start_sample_storage_buffer;
        used_flag_storage_buffer = used_flag_temp;
        packet_storage_buffer = packet_temp;
        num_packets_storage_buffer = num_temp;
        packet_start_sample_storage_buffer = packet_start_sample_temp;
    }
    //DEF_LOGGING_DEBUG("Flushing storage buffer --- done\n");
    return bad_flag;
}






int_fast32_t RSP_beamformed_packet_sorter::
store_standard_packet(uint8_t valid,
                      const uint_fast64_t start_sample,
                      RSP_beamformed_packet* restrict data)
    restrict throw()
{
    //DEF_LOGGING_DEBUG("Storing standard packet with start sample %llX\n", (unsigned long long)(start_sample));
    // store the information in our storage buffer
    if((num_packets_storage_buffer)) {
    }
    else {
        packet_start_sample_storage_buffer = start_sample;
    }
    used_flag_storage_buffer[num_packets_storage_buffer] = valid;
    packet_storage_buffer[num_packets_storage_buffer] = data;
    ++num_packets_storage_buffer;
    if(num_packets_storage_buffer == NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE) {
        return flush_storage_to_writers();
    }
    return 0;
}




int_fast32_t RSP_beamformed_packet_sorter::
send_WRITERS_partial_packet(uint8_t valid,
                            const uint_fast64_t start_sample,
                            const uint_fast64_t last_sample,
                            RSP_beamformed_packet* restrict data)
    restrict throw()
{
    //DEF_LOGGING_DEBUG("entering sending last data packet %llX %llX", (unsigned long long)(start_sample), (unsigned long long)(last_sample));
    //DEF_LOGGING_DEBUG("sorter trying to send partial packet to writers\n");
    // a partial packet only gets sent as the last possible packet.  So
    // just make sure that all other packets have been cleared.
    int_fast32_t bad_flag = flush_storage_to_writers();
    if((bad_flag)) {
        return bad_flag;
    }
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->wait_for_ready();
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    if((bad_flag)) {
        return bad_flag;
    }
    if(((valid)) && (data->data_are_valid())) {
        used_flag_storage_buffer[num_packets_storage_buffer] = valid;
        packet_storage_buffer[num_packets_storage_buffer] = data;
    }
    else {
        used_flag_storage_buffer[num_packets_storage_buffer] = 0;
        packet_storage_buffer[num_packets_storage_buffer] = &Compare_Packet_Header;
    }
    ++num_packets_storage_buffer;
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->send_last_data_packet(used_flag_storage_buffer,
                                                                start_sample,
                                                                last_sample,
                                                                packet_storage_buffer);
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    // now make sure the partial packet is cleared
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->wait_for_ready();
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    if((valid)) {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        push_packet_stack(data);
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }
    if((bad_flag)) {
        return bad_flag;
    }
    --num_packets_storage_buffer;
    used_flag_storage_buffer[num_packets_storage_buffer] = 0;
    packet_storage_buffer[num_packets_storage_buffer] = NULL;
    
    return bad_flag;
}


int_fast32_t RSP_beamformed_packet_sorter::
send_WRITERS_sample_start(const uint_fast64_t start_sample)
    restrict throw()
{
    int_fast32_t bad_flag = 0;
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->wait_for_ready();
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    if((bad_flag)) {
        return bad_flag;
    }
    for(uint_fast16_t w =0; w < NUM_WRITERS; w++) {
        int_fast32_t retval = WRITERS[w]->time_block_initialize(start_sample);
        if((retval)) {
            bad_flag = 2;
            DEF_LOGGING_ERROR("writer %d failed with message %d\n", int(w), int(retval));
            final_status = RSP_beamformed_packet_sorter_Writer_Error;
        }
    }
    return bad_flag;
}











int_fast32_t RSP_beamformed_packet_sorter::
flush_ring_buffer_to_storage() restrict throw()
{
    //DEF_LOGGING_DEBUG("Flushing ring buffer\n");
    //DEF_LOGGING_DEBUG("start_packet %llX end_packet %llX\n", (unsigned long long)(start_packet), (unsigned long long)(end_packet));
    uint_fast64_t END = end_packet;
    if(END > Last_Packet) {
        END = Last_Packet_plus_one;
    }
    int_fast16_t bad_flag = 0;    
    for(; start_packet < END; start_packet++) {
        uint_fast32_t index = uint_fast32_t(start_packet) & PACKET_NUMBER_MASK_32;
        //DEF_LOGGING_DEBUG("start_packet %llX valid %X\n", (unsigned long long)(start_packet), used_flag_ring_buffer[index]);
        //DEF_LOGGING_DEBUG("storage stats writing used %d storage used %d\n", int(num_packets_writing_buffer), int(num_packets_storage_buffer));
        if(start_packet == Last_Packet) {
            int_fast32_t retval=0;
            //DEF_LOGGING_DEBUG("flush ring buffer at partial\n");
            // storage buffer cleared by partial packet call
            if(used_flag_ring_buffer[index]) {
                //DEF_LOGGING_DEBUG("start %llX last %llX", (unsigned long long)(packet_start_sample_ring_buffer[index]), (unsigned long long)(Last_Processing_Sample));
                retval = send_WRITERS_partial_packet(used_flag_ring_buffer[index],
                                                     packet_start_sample_ring_buffer[index],
                                                     Last_Processing_Sample,
                                                     packet_ring_buffer[index]);
            }
            else {
                ++num_packets_missing_time_block;
                uint64_t start_sample = packets_to_samples(start_packet);
                //DEF_LOGGING_DEBUG("start %llX last %llX", (unsigned long long)(start_sample), (unsigned long long)(Last_Processing_Sample));
                retval = send_WRITERS_partial_packet(used_flag_ring_buffer[index],
                                                     start_sample,
                                                     Last_Processing_Sample,
                                                    &Compare_Packet_Header);
            }
            used_flag_ring_buffer[index] = 0;
            packet_ring_buffer[index] = 0;
            //DEF_LOGGING_DEBUG("finishing flush ring buffer at partial\n");
            if((retval)) {
                return retval;
            }
            if((bad_flag)) {
                return int_fast32_t(bad_flag);
            }
            return 1;  // special code to indicate that we are done processing
        }
        if(used_flag_ring_buffer[index]) {
            bad_flag |= store_standard_packet(used_flag_ring_buffer[index],
                                              packet_start_sample_ring_buffer[index],
                                              packet_ring_buffer[index]);
        }
        else {
            uint_fast64_t start_sample = packets_to_samples(start_packet);
            bad_flag |= store_standard_packet(used_flag_ring_buffer[index],
                                              start_sample,
                                              &Compare_Packet_Header);
            //DEF_LOGGING_DEBUG("Missing packet at 2\n");
            ++num_packets_missing_time_block;
        }
        used_flag_ring_buffer[index] = 0;
        packet_ring_buffer[index] = 0;
    }
    //DEF_LOGGING_DEBUG("finishing flush ring buffer\n");
    return int_fast32_t(bad_flag);
}





int_fast32_t RSP_beamformed_packet_sorter::
store_exact_packet(RSP_beamformed_packet * restrict packet,
                   const uint_fast64_t packet_64_index,
                   const uint_fast64_t packet_start_sample)
    restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing exact\n");
    if(packet_64_index > Last_Packet) {
        DEF_LOGGING_ERROR("told to store packet after end_time");
        end_packet = start_packet = packet_64_index;
        // drop the packet
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        push_packet_stack(packet);
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        return +2;
    }
    if(packet_64_index == Last_Packet) {
        // store exact packet only called when there are no packets in the
        // ring buffer
        // storage area cleared by partial packet call
        //DEF_LOGGING_DEBUG("packet %llX at store exact end %llX %llX\n", (unsigned long long)packet_64_index, (unsigned long long)(packet_start_sample), (unsigned long long)(Last_Processing_Sample));
        int_fast32_t retval = send_WRITERS_partial_packet(1,packet_start_sample, Last_Processing_Sample, packet);
        if((retval)) {
            DEF_LOGGING_DEBUG("send_WRITERS_partial_packet retval=%d", int(retval));
            return retval;
        }
        return 1; // special code to indicate that we are done writing
    }
    int_fast32_t return_code = store_standard_packet(1,packet_start_sample, packet);
    end_packet = start_packet = packet_64_index+1;
    //DEF_LOGGING_DEBUG("packet %llX at store exact end\n", (unsigned long long)packet_64_index);
    return return_code;
}





int_fast32_t RSP_beamformed_packet_sorter::
try_sending_next_ring_packet_to_storage() restrict throw()
{
    //DEF_LOGGING_DEBUG("trying to store next packet in ring buffer\n");
    uint_fast32_t index = uint_fast32_t(start_packet) & PACKET_NUMBER_MASK_32;
    if(start_packet == end_packet) {
        return 0;
    }
    if(!used_flag_ring_buffer[index]) {
        return 0;
    }
    if(start_packet == Last_Packet) {
        // storage area cleared by partial packet call
        //DEF_LOGGING_DEBUG("try_sending_next_ring_packet_to_storage partial");
        int_fast32_t retval = send_WRITERS_partial_packet(1,
                                                          packet_start_sample_ring_buffer[index],
                                                          Last_Processing_Sample,
                                                          packet_ring_buffer[index]);
        start_packet = end_packet = start_packet+1;
        used_flag_ring_buffer[index]=0;
        packet_ring_buffer[index] = 0;
        if((retval)) {
            return retval;
        }
        return 1; // special code to indicate that we are done writing
    }
    else {
        int_fast32_t retval = store_standard_packet(1,
                                                    packet_start_sample_ring_buffer[index],
                                                    packet_ring_buffer[index]);
        start_packet++;
        used_flag_ring_buffer[index] = 0;
        packet_ring_buffer[index] = 0;
        if((retval)) {
            return 4;
        }
    }
    // Should we try to send the next packet as well?
    if((start_packet & 0xF) == 0) {
        return try_sending_next_ring_packet_to_storage();
    }
    return 0;
}








int_fast32_t RSP_beamformed_packet_sorter::
process_regular_packet(RSP_beamformed_packet * restrict packet) restrict throw()
{
    //DEF_LOGGING_DEBUG("New packet has time %X time_sample_offset %llX block start %X\n",unsigned(packet->uinteger_time()),time_sample_offset(packet->uinteger_time()),unsigned(packet->Block_Start()));
    
    uint_fast64_t packet_start_sample =
        time_sample_offset(packet->uinteger_time()) + packet->Block_Start();
    //uint_fast64_t packet_end_sample = packet_start_sample + SAMPLES_PER_PACKET;
    uint_fast64_t packet_64_index = samples_to_packets(packet_start_sample);
    //DEF_LOGGING_DEBUG("start_packet %llX end_packet %llX packet_64_index %llX at process_regular_packet start  ", (unsigned long long)start_packet, (unsigned long long)end_packet, (unsigned long long)packet_64_index);
    //DEF_LOGGING_DEBUG("    packet time %X block %.8X", unsigned(packet->uinteger_time()), unsigned(packet->Block_Start()));
//     for(uint_fast32_t i=0; i < NUM_PACKETS_IN_RING_BUFFER; i++) {
//         int c = '0';
//         if(i==(uint_fast32_t(start_packet) & PACKET_NUMBER_MASK_32)) c+=2;
//         if(i==(uint_fast32_t(end_packet) & PACKET_NUMBER_MASK_32)) c+=4;
//         fputc(c+used_flag_ring_buffer[i],stderr);
//     }
//     fputc('\n',stderr);
    //DEF_LOGGING_DEBUG("packet_64_index %llX Last_Packet %llX at process regular start\n", (unsigned long long)packet_64_index, (unsigned long long)Last_Packet);

    //DEF_LOGGING_DEBUG("packet start %llX end %llX\n", (unsigned long long)packet_start_sample, (unsigned long long)packet_end_sample);
#ifdef LOFAR_DEBUG
    if(packet->uinteger_time() != LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON) {
        if(check_sample_slip(packet_start_sample)) {
            DEF_LOGGING_ERROR("packet shift not allowed\n");
            DEF_LOGGING_DEBUG("info 0x%8.8X 0x%8.8X %d", unsigned(packet_start_sample), unsigned(packet->uinteger_time()), int(packet->Block_Start()));
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            push_packet_stack(packet);
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            return -1;
        }
    }
#endif // LOFAR_DEBUG
    if(packet_64_index > Last_Packet) {
        if(packet->uinteger_time() == LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON) {
            // hardware bug affecting the timestamp.  Try to fix.
            bool packet_probably_valid = false;
            //DEF_LOGGING_DEBUG("JMA start_packet=0x%X end_packet=0x%X", unsigned(start_packet), unsigned(end_packet));
            packet_64_index = calculate_expected_packet_64_index(end_packet,
                                                                 packet->Block_Start(),
                                                                 packet_start_sample,
                                                                 packet_probably_valid);
#ifdef LOFAR_DEBUG
            if(check_sample_slip(packet_start_sample)) {
                DEF_LOGGING_ERROR("packet shift not allowed\n");
                DEF_LOGGING_DEBUG("info 0x%8.8X 0x%8.8X %d", unsigned(packet_start_sample), unsigned(packet->uinteger_time()), int(packet->Block_Start()));
                SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                push_packet_stack(packet);
                SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                return -1;
            }
#endif // LOFAR_DEBUG
            if(!packet_probably_valid) {
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
#else
                DEF_LOGGING_WARNING("LOFAR_HARDWARE_BUG_00_TIMESTAMP_UNKNWON could not have timestamp identified, start_packet 0x%llX end_packet 0x%llX Block_Start 0x%X Packet_Sample_Index_Offset 0x%X\n", (unsigned long long)start_packet, (unsigned long long)end_packet, unsigned(packet->Block_Start()), unsigned(Packet_Sample_Index_Offset));
#endif
                packet->set_data_invalid();
                if(start_packet == end_packet) {
                    // No data in the ring buffer.  Flush out an invalid packet
                    // to make sure we don't miss several seconds of recording.
                    // First, drop the packet.
                    SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                    push_packet_stack(packet);
                    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                    {
                        uint_fast64_t start_sample = packets_to_samples(start_packet);
                        ++num_packets_missing_time_block;
                        if(start_packet == Last_Packet) {
                            // storage area cleared by partial packet call
                            int_fast32_t retval = send_WRITERS_partial_packet(0,
                                                                              start_sample,
                                                                              Last_Processing_Sample,
                                                                             &Compare_Packet_Header);
                            if((retval)) {
                                DEF_LOGGING_WARNING("got retval %d", int(retval));
                                return retval;
                            }
                            return 1; // special code to indiate that we are done.
                        }
                        int_fast32_t bad_flag = store_standard_packet(0,
                                                                      start_sample,
                                                                     &Compare_Packet_Header);
                        if((bad_flag)) {
                            DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                            return bad_flag;
                        }
                    }
                    ++end_packet;
                    start_packet = end_packet;
                }
            }
        }
        else {        
            // packet after end_time.  Are we really done?
            // First, drop the packet, then check what to do.
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            push_packet_stack(packet);
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
            if(start_packet >= Last_Packet) {
                return 1; // special code to indicate that we are done
            }
            return 0;
        }
    }


    if((start_packet == end_packet) && (packet_64_index == end_packet)) {
        return store_exact_packet(packet,packet_64_index,packet_start_sample);
    }
    int_fast32_t bad_flag = try_sending_next_ring_packet_to_storage();
    if((bad_flag)) {
        DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        push_packet_stack(packet);
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        return bad_flag;
    }
    //DEF_LOGGING_DEBUG("start_packet %llX end_packet %llX packet_64_index %llX after try storage\n", (unsigned long long)start_packet, (unsigned long long)end_packet, (unsigned long long)packet_64_index);
    if((start_packet == end_packet) && (packet_64_index == end_packet)) {
        return store_exact_packet(packet,packet_64_index,packet_start_sample);
    }

    if(packet_64_index < end_packet) {
        ++num_packets_out_of_order_time_block;
    }
    
    uint_fast32_t packet_index = uint_fast32_t(packet_64_index) & PACKET_NUMBER_MASK_32;
    if(packet_64_index >= end_packet) {
        // should we get rid of some of the existing data first?
        if(packet_64_index - start_packet >= NUM_PACKETS_IN_RING_BUFFER) {
            //DEF_LOGGING_WARNING("flush of packet ring buffer");
            //DEF_LOGGING_DEBUG("packet_index is    %X", unsigned(packet_index));
            //DEF_LOGGING_DEBUG("packet_64_index is %llX", unsigned(packet_64_index));
            //DEF_LOGGING_DEBUG("start_packet is    %llX", unsigned(start_packet));
            //DEF_LOGGING_DEBUG("end_packet is      %llX", unsigned(end_packet));
            //DEF_LOGGING_DEBUG("Must flush part of ring buffer, packet difference start is %llX, continue after flush\n", (unsigned long long)(packet_64_index - start_packet));
            //DEF_LOGGING_DEBUG("Must flush part of ring buffer, packet difference from end   is %llX, continue after flush\n", (unsigned long long)(packet_64_index - end_packet));
            //DEF_LOGGING_DEBUG("Must flush part of ring buffer, packet_64_index is %llX, continue after flush\n", (unsigned long long)(packet_64_index));
            //DEF_LOGGING_DEBUG("packet_index is %X", unsigned(packet_index));
            // if we miss more than 2 seconds' worth of data, bail
            if(packet_64_index - end_packet > NUM_PACKETS_IN_TWO_SECONDS_TIME) {
                num_major_buffer_flush_skip++;
                DEF_LOGGING_WARNING("Major flush of data number %d, packet difference is %llX, die after flush\n", int(num_major_buffer_flush_skip), (unsigned long long)(packet_64_index - end_packet));
                DEF_LOGGING_WARNING("Major flush of data, packet timestamp is %llX\n", (unsigned long long)(packet->uinteger_time()));
                {
                    struct timespec local_time;
                    int retval = clock_gettime(CLOCK_REALTIME,&local_time);
                    if((retval)) {
                        DEF_LOGGING_PERROR("Error getting local time");
                    }
                    else {
                        DEF_LOGGING_WARNING("Major flush of data, local timestamp is %llX (%lld.%09ld)\n", (unsigned long long)(local_time.tv_sec), (long long)(local_time.tv_sec), (long)(local_time.tv_nsec));
                    }
                }
                // return the packet to the stack
                SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                push_packet_stack(packet);
                SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                if(num_major_buffer_flush_skip <= MAX_NUM_MAJOR_BUFFER_FLUSH_SKIP)
                {
                    ++num_packets_dropped_time_block;
                    return 0;
                }
                else {
                    // far too much data has been lost, die and have the
                    // user try again
                    // clear out the ring buffer
                    flush_ring_buffer_to_storage();
                    flush_storage_to_writers();
                    final_status = RSP_beamformed_packet_sorter_Packet_Gap_Error;
                    return -2;
                }
            }
            else if(packet_64_index - end_packet >= NUM_PACKETS_IN_RING_BUFFER) {
                num_major_buffer_flush_skip++;
                DEF_LOGGING_WARNING("Minor flush of data number %d, packet difference is %llX, continue after flush\n", int(num_major_buffer_flush_skip), (unsigned long long)(packet_64_index - end_packet));
                DEF_LOGGING_WARNING("Minor flush of data, packet timestamp is %llX\n", (unsigned long long)(packet->uinteger_time()));
                {
                    struct timespec local_time;
                    int retval = clock_gettime(CLOCK_REALTIME,&local_time);
                    if((retval)) {
                        DEF_LOGGING_PERROR("Error getting local time");
                    }
                    else {
                        DEF_LOGGING_WARNING("Minor flush of data, local timestamp is %llX (%lld.%09ld)\n", (unsigned long long)(local_time.tv_sec), (long long)(local_time.tv_sec), (long)(local_time.tv_nsec));
                    }
                }
                if(num_major_buffer_flush_skip <= MAX_NUM_MAJOR_BUFFER_FLUSH_SKIP)
                {
                    ++num_packets_dropped_time_block;
                    // return the packet to the stack
                    SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                    push_packet_stack(packet);
                    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                    
                    return 0;
                }
                // difference is too big for the buffer, so blank the buffer
                bad_flag = flush_ring_buffer_to_storage();
                if((bad_flag)) {
                    DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                    SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                    push_packet_stack(packet);
                    SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                    return bad_flag;
                }
                memset(used_flag_ring_buffer,0,NUM_PACKETS_IN_RING_BUFFER);
                // write out bad data lines until we can fit the new packet in
                uint_fast64_t END = packet_64_index - NUM_PACKETS_IN_RING_BUFFER;
                if(END > Last_Packet) {
                    END = Last_Packet;
                }
                for(start_packet=end_packet;
                    start_packet <= END; start_packet++) {
                    uint_fast64_t start_sample = packets_to_samples(start_packet);
                    ++num_packets_missing_time_block;
                    if(start_packet == Last_Packet) {
                        // storage area cleared by partial packet call
                        int_fast32_t retval = send_WRITERS_partial_packet(0,
                                                                          start_sample,
                                                                          Last_Processing_Sample,
                                                                         &Compare_Packet_Header);
                        if((retval)) {
                            DEF_LOGGING_WARNING("got retval %d", int(retval));
                            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                            push_packet_stack(packet);
                            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                            return retval;
                        }
                        if((bad_flag)) {
                            DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                            push_packet_stack(packet);
                            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                            return bad_flag;
                        }
                        return 1; // special code to indiate that we are done.
                    }
                    bad_flag = store_standard_packet(0,
                                                     start_sample,
                                                     &Compare_Packet_Header);
                    if((bad_flag)) {
                        DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                        push_packet_stack(packet);
                        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                        return bad_flag;
                    }
                }
            }
            else {
                // flush packet by packet until we can fit the new one in
                uint_fast64_t END = packet_64_index - NUM_PACKETS_IN_RING_BUFFER;
                //DEF_LOGGING_DEBUG("flushing by single packets, will flush up to packet %llX", (unsigned long long)(END));
                if(END > Last_Packet) {
                    END = Last_Packet;
                }
                for(;start_packet <= END; start_packet++) {
                    uint_fast32_t index = uint_fast32_t(start_packet) & PACKET_NUMBER_MASK_32;
                    if(start_packet == Last_Packet) {
                        int_fast32_t retval;
                        if((used_flag_ring_buffer[index])) {
                            retval = send_WRITERS_partial_packet(used_flag_ring_buffer[index],
                                                                 packet_start_sample_ring_buffer[index],
                                                                 Last_Processing_Sample,
                                                                 packet_ring_buffer[index]);
                        }
                        else {
                            uint_fast64_t start_sample = packets_to_samples(start_packet);
                            ++num_packets_missing_time_block;
                            retval = send_WRITERS_partial_packet(used_flag_ring_buffer[index],
                                                                 start_sample,
                                                                 Last_Processing_Sample,
                                                                 &Compare_Packet_Header);
                        }
                        used_flag_ring_buffer[index] = 0;
                        packet_ring_buffer[index] = 0;
                        if((retval)) {
                            DEF_LOGGING_WARNING("got retval %d", int(retval));
                            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                            push_packet_stack(packet);
                            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                            return int_fast32_t(retval);
                        }
                        if((bad_flag)) {
                            DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                            push_packet_stack(packet);
                            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                            return bad_flag;
                        }
                        return 1; // special code to indiate that we are done
                    }
                    if(used_flag_ring_buffer[index]) {
                        bad_flag = store_standard_packet(used_flag_ring_buffer[index],
                                                         packet_start_sample_ring_buffer[index],
                                                         packet_ring_buffer[index]);
                    }
                    else {
                        uint_fast64_t start_sample = packets_to_samples(start_packet);
                        bad_flag = store_standard_packet(used_flag_ring_buffer[index],
                                                         start_sample,
                                                        &Compare_Packet_Header);
                        ++num_packets_missing_time_block;
                    }
                    if((bad_flag)) {
                        DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
                        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
                        push_packet_stack(packet);
                        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
                        return bad_flag;
                    }
                    used_flag_ring_buffer[index] = 0;
                    packet_ring_buffer[index] = 0;
                } // for start to END of packet
            }
        } // if we have to force writing out packets to fit new in ring buffer
        end_packet = packet_64_index+1;
    } // if new packet >= end_packet
    else if(packet_64_index < start_packet) {
        // before our current minimum, so just drop the packet
        //DEF_LOGGING_DEBUG("Dropping packet before start_packet\n");
        ++num_packets_dropped_time_block;
        
        // return the packet to the stack
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        push_packet_stack(packet);
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);

        if((bad_flag)) {
            DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
        }
        return bad_flag;
    }
    else {
        // new packet is somewhere in the middle of the buffer
        // does it overlap an existing packet?
        if((used_flag_ring_buffer[packet_index])) {
            ++num_packets_duplicate_time_block;

            // If the new packet is valid, 
            // return the previous packet to the stack
            SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
            if(packet->data_are_valid()) {
                push_packet_stack(packet_ring_buffer[packet_index]);
            }
            else {
                push_packet_stack(packet);
                packet = packet_ring_buffer[packet_index];
            }
            SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
        }
    }
    // copy the new packet in
    packet_ring_buffer[packet_index] = packet;
    used_flag_ring_buffer[packet_index] = 1;
    packet_start_sample_ring_buffer[packet_index] = packet_start_sample;
    packet_index_ring_buffer[packet_index] = packet_64_index;
    if((bad_flag)) {
        DEF_LOGGING_WARNING("got bad_flag %d", int(bad_flag));
    }
    return bad_flag;
}












void RSP_beamformed_packet_sorter::
close_WRITERS() restrict throw()
{
    //DEF_LOGGING_DEBUG("Closing writers\n");
    for(uint16_t w =0; w < NUM_WRITERS; w++) {
        WRITERS[w]->time_block_finished();
    }
    return;
}

void RSP_beamformed_packet_sorter::
report_bad_packet_config(const RSP_beamformed_packet * const restrict packet) restrict const throw()
{
    DEF_LOGGING_ERROR("Packet has wrong configuration %2.2X %2.2X %2.2X %2.2X %4.4X %2.2X %2.2X\n",
            packet->Ver(),
            packet->Sour_0(),
            packet->Sour_1(),
            packet->Config(),
            packet->Sta(),
            packet->nBeamlets(),
            packet->nBlocks());
    return;
}


void RSP_beamformed_packet_sorter::
print_final_statistics() restrict throw()
{
    num_packets_processed += num_packets_processed_time_block;
    num_packets_dropped += num_packets_dropped_time_block;
    num_packets_missing += num_packets_missing_time_block;
    num_packets_duplicate += num_packets_duplicate_time_block;
    num_packets_out_of_order += num_packets_out_of_order_time_block;
    num_packets_overrun += num_packets_overrun_time_block;
    num_packets_hardware_bug += num_packets_hardware_bug_time_block;
    
    // Final statistics
    DEF_LOGGING_INFO("Final packet statistics for sorter\n");
    DEF_LOGGING_INFO("Num packets processed    0x%16.16llX\n", (unsigned long long)num_packets_processed);
    DEF_LOGGING_INFO("Num packets dropped      0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_dropped, double(num_packets_dropped)/double(num_packets_processed));
    DEF_LOGGING_INFO("Num packets missing      0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_missing, double(num_packets_missing)/double(num_packets_processed));
    DEF_LOGGING_INFO("Num packets duplicate    0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_duplicate, double(num_packets_duplicate)/double(num_packets_processed));
    DEF_LOGGING_INFO("Num packets out of order 0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_out_of_order, double(num_packets_out_of_order)/double(num_packets_processed));
    DEF_LOGGING_INFO("Num packets overrun      0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_overrun, double(num_packets_overrun)/double(num_packets_processed));
    DEF_LOGGING_INFO("Num packets hardware bug 0x%16.16llX  %10.1E\n", (unsigned long long)num_packets_hardware_bug, double(num_packets_hardware_bug)/double(num_packets_processed));
    {
        struct timespec local_time;
        int retval = clock_gettime(CLOCK_REALTIME,&local_time);
        if((retval)) {
            DEF_LOGGING_PERROR("Error getting local time");
        }
        else {
            DEF_LOGGING_INFO("Current local time       0x%16.16llX        \n", (unsigned long long)(local_time.tv_sec));
        }
    }
    
    return;
}


void RSP_beamformed_packet_sorter::
print_intermediate_statistics() restrict throw()
{
    static const int SIZE = 164;
//    static const char message[SIZE] = "Intermediate packet statistics: 0x0123456789ABCDEF proc 0x0123456789ABCDEF drop 0x0123456789ABCDEF miss 0x0123456789ABCDEF dupl 0x0123456789ABCDEF ooor 0x0123456789ABCDEF over X 0x0123456789ABCDEF time\n";
    static const char message[SIZE] = "Intermediate packet statistics: 0x01234567 proc 0x01234567 drop 0x01234567 miss 0x01234567 dupl 0x01234567 ooor 0x01234567 over 0x01234567 hbug X 0x01234567 time\n";
    bool need_to_unlock = false;
    char mes[SIZE];
    memcpy(mes,message,SIZE);

    {
        struct timespec local_time;
        int retval = clock_gettime(CLOCK_REALTIME,&local_time);
        if((retval)) {
            DEF_LOGGING_PERROR("Error getting local time");
        }
        else {
            print_N_hex_chars_to_string(uint_fast64_t(local_time.tv_sec),mes+148,8);
        }
    }

    if(boss_has_read_thread_lock==0) {
        SORTER_BOSS_LOCK_MUTEX(&read_thread_mutex);
        need_to_unlock = true;
    }

    char A=' ';
    switch(packet_time_status) {
    case LOFAR_SORTER_PACKET_TIME_BEFORE:   A = 'B'; break;
    case LOFAR_SORTER_PACKET_TIME_IN_RANGE: A = 'I'; break;
    case LOFAR_SORTER_PACKET_TIME_AFTER:    A = 'A'; break;
    default:
        A = 'U';
    }
    mes[144] = A;
    
    print_N_hex_chars_to_string(num_packets_processed_time_block,mes+34,8);
    print_N_hex_chars_to_string(num_packets_dropped_time_block,mes+50,8);
    print_N_hex_chars_to_string(num_packets_missing_time_block,mes+66,8);
    print_N_hex_chars_to_string(num_packets_duplicate_time_block,mes+82,8);
    print_N_hex_chars_to_string(num_packets_out_of_order_time_block,mes+98,8);
    print_N_hex_chars_to_string(num_packets_overrun_time_block,mes+114,8);
    print_N_hex_chars_to_string(num_packets_hardware_bug_time_block,mes+130,8);
    

    num_packets_processed += num_packets_processed_time_block;
    num_packets_dropped += num_packets_dropped_time_block;
    num_packets_missing += num_packets_missing_time_block;
    num_packets_duplicate += num_packets_duplicate_time_block;
    num_packets_out_of_order += num_packets_out_of_order_time_block;
    num_packets_overrun += num_packets_overrun_time_block;
    num_packets_hardware_bug += num_packets_hardware_bug_time_block;
    num_packets_processed_time_block =
        num_packets_dropped_time_block =
        num_packets_missing_time_block =
        num_packets_duplicate_time_block =
        num_packets_out_of_order_time_block =
        num_packets_overrun_time_block =
        num_packets_hardware_bug_time_block = 0;

    if(need_to_unlock) {
        SORTER_BOSS_UNLOCK_MUTEX(&read_thread_mutex);
    }

    DEF_LOGGING_INFO(mes);


    return;
}



// Try to guess the integer clock second of a packet based on an expected
// packet position, and return the packet index for the packet
uint_fast64_t RSP_beamformed_packet_sorter::
calculate_expected_packet_64_index(const uint_fast64_t& expected_packet_index,
                                   const uint32_t& this_block_start,
                                   uint_fast64_t& packet_start_sample,
                                   bool& packet_probably_valid) const throw() {
    uint_fast64_t return_value = 0;
    packet_probably_valid = true;

    //DEF_LOGGING_DEBUG("JMA expected_packet_index=0x%X, this_block_start=%d this_block_start=0x%X", unsigned(expected_packet_index), unsigned(this_block_start), unsigned(this_block_start));
        
    uint_fast64_t expected_sample = packets_to_samples(expected_packet_index);
    int_fast64_t expected_time_offset_2 = expected_sample / time_samples_per_two_seconds;
    int_fast64_t remainder_samples_time_offset_2 = expected_sample % time_samples_per_two_seconds;
    
    int_fast64_t expected_time = expected_time_offset_2 * 2 + reference_time_zero;
    //DEF_LOGGING_DEBUG("JMA expected_sample=%d 0x%X, expected_time_offset_2=%d, remainder_samples_time_offset_2=%d time_samples_per_even_second=%d", int(expected_sample), int(expected_sample), int(expected_time_offset_2), int(remainder_samples_time_offset_2), int(time_samples_per_even_second));
    if(remainder_samples_time_offset_2 >= int_fast64_t(time_samples_per_even_second)) {
        remainder_samples_time_offset_2 -= int_fast64_t(time_samples_per_even_second);
        ++expected_time;
        
    }
    int_fast64_t remainder_samples_time_offset = remainder_samples_time_offset_2;
    int_fast64_t predicted_time = expected_time;

    //DEF_LOGGING_DEBUG("JMA expected_sample=%d 0x%X expected_time=0x%X", unsigned(expected_sample), unsigned(expected_sample), unsigned(expected_time));
    //DEF_LOGGING_DEBUG("JMA Packet_Sample_Index_Offset=%u 0x%X time_samples_per_two_seconds=%d", unsigned(Packet_Sample_Index_Offset), unsigned(Packet_Sample_Index_Offset), int(time_samples_per_two_seconds));

    
    int_fast32_t half_samples_per_second = time_samples_per_even_second >> 1;

    if(remainder_samples_time_offset == int_fast64_t(this_block_start)) {
        // found the correct time
        return_value = expected_packet_index;
        packet_start_sample = expected_sample;
    }
    else {
        if(remainder_samples_time_offset < int_fast64_t(this_block_start)) {
            // Actual block start is higher.  Is this block later than expected,
            // or earlier?
            int_fast32_t diff = int_fast64_t(this_block_start) - remainder_samples_time_offset;
            if(diff >= half_samples_per_second) {
                // The actual block is probably from the previous second.  Back up.
                --predicted_time;
            }
        }
        else {
            // Actual block start is lower.  Is this block later than expected,
            // or earlier
            int_fast32_t diff = remainder_samples_time_offset - int_fast64_t(this_block_start);
            if(diff >= half_samples_per_second) {
                // The actual block is probably from the next second.  Go forward.
                ++predicted_time;
            }
        }
        uint_fast64_t t_packet_start_sample =
            time_sample_offset(predicted_time) + this_block_start;
        packet_start_sample = t_packet_start_sample;
        return_value = samples_to_packets(t_packet_start_sample);
    }
    if(check_sample_slip(packet_start_sample)) {
        packet_probably_valid = false;
    }
    //DEF_LOGGING_DEBUG("JMA expected_time=0x%X packet_start_sample=0x%X packet=0x%X packet_probably_valid=%d", unsigned(expected_time), unsigned(packet_start_sample), unsigned(return_value), int(packet_probably_valid));
    return return_value;
}






// void RSP_beamformed_packet_sorter::
// print_16_hex_chars_to_string(uint_fast64_t x, char* const restrict p) const restrict throw()
// {
//     static const char ca[17] = "0123456789ABCDEF";
//     for(int i=16; i != 0;) {
//         uint_fast32_t c = uint_fast32_t(x) & 0xF;
//         x >>= 4;
//         p[--i] = ca[c];
//     }
//     return;
// }



int_fast32_t RSP_beamformed_packet_sorter::
check_for_interrupt() restrict throw()
{
    int_fast32_t retcode = 0;
    pthread_mutex_lock(&(MPIfR::LOFAR::Station::Signals::user_signal_found_mutex));
    switch(MPIfR::LOFAR::Station::Signals::user_signal_found) {
    case MPIfR::LOFAR::Station::Signals::USER_SIGNAL_NOTHING_HAPPENING:
        // do nothing
        break;
    case 0:
        // do nothing
        break;
    case SIGTERM:
    case SIGINT:
        retcode = -1;
        break;
    default:
        DEF_LOGGING_WARNING("Unrecognized signal %d\n", int(MPIfR::LOFAR::Station::Signals::user_signal_found));
        retcode = -1;
    }
    pthread_mutex_unlock(&(MPIfR::LOFAR::Station::Signals::user_signal_found_mutex));
    return retcode;        
}












const char* const restrict LOFAR_Station_Beamformed_Sorter_Work_Enum_Str(LOFAR_Station_Beamformed_Sorter_Work_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Sorter_Work_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Sorter_Work_Enum";
    }
    return NULL;
}

const char* const restrict LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Str(LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Sorter_Thread_Status_Enum";
    }
    return NULL;
}

const char* const restrict LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum_Str(LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Sorter_Packet_Time_Enum";
    }
    return NULL;
}

const char* const restrict LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Str(LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String) case Enum: return String;
        LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Array;
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum";
    }
    return NULL;
}



int RSP_beamformed_packet_sorter::
interpret_port_info() restrict throw()
{
    // based on the create_socket function in udp-copy.c received from
    // John Romein at ASTRON
    port_proto = LOFAR_SORTER_SOCKET_TYPE_UDP;
    int offset = 0;

    if (strncmp(port, "udp:", 4) == 0 || strncmp(port, "UDP:", 4) == 0) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_UDP;
        offset = 4;
    }
    else if (strncmp(port, "tcp:", 4) == 0 || strncmp(port, "TCP:", 4) == 0) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_TCP;
        offset = 4;
    }
    else if (strncmp(port, "file:", 5) == 0 || strncmp(port, "FILE:", 5) == 0) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_FILE;
        offset = 5;
    }
    else if (strncmp(port, "file_ns:", 8) == 0 || strncmp(port, "FILE_NS:", 8) == 0) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_FILE_NO_SKIP;
        offset = 8;
    }
    else if (strchr(port, ':') != NULL) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_UDP;
        offset = 1;
    }
    else if (strcmp(port, "-") == 0) {
        port_proto = LOFAR_SORTER_SOCKET_TYPE_STDIN;
    }
    else {
        // let getaddrinfo figure out the service name
        port_proto = LOFAR_SORTER_SOCKET_TYPE_UDP;
    }


    DEF_LOGGING_DEBUG("Trying to open input socket from '%s' '%s' as type %s", port, port+offset, LOFAR_Station_Beamformed_Sorter_Socket_Type_Enum_Str(port_proto));
    switch (port_proto) {
    case LOFAR_SORTER_SOCKET_TYPE_UDP  :return create_input_IP_socket(port+offset, true);
    case LOFAR_SORTER_SOCKET_TYPE_TCP  : return create_input_IP_socket(port+offset, false);

    case LOFAR_SORTER_SOCKET_TYPE_FILE : 
    case LOFAR_SORTER_SOCKET_TYPE_FILE_NO_SKIP : {
        int p_i = open(port+offset, O_RDONLY);
        {
            errno = 0;
            int retval = posix_fadvise(p_i, 0, 0, POSIX_FADV_SEQUENTIAL);
            if((retval)) {
                DEF_LOGGING_PERROR("posix_fadvise POSIX_FADV_SEQUENTIAL failed:");
            }
        }
        return p_i;
    }
    case LOFAR_SORTER_SOCKET_TYPE_STDIN : return fileno(stdin);
    }
    return -1;
}







}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR



