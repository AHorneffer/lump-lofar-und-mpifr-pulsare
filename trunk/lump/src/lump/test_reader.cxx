// test_reader.cxx
// try dumping out some test data
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 21  James M Anderson  --MPIfR  start



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



// INCLUDES
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "LOFAR_Station_Beamformed_Packet.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include "LOFAR_Station_Beamformed_Writer_Power0.h"
#include "LOFAR_Station_Beamformed_Writer_Raw.h"
#include "LOFAR_Station_Beamformed_Writer_Raw0.h"
#include "LOFAR_Station_Beamformed_Writer_Raw1.h"
#include "LOFAR_Station_Beamformed_Sorter.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>




// set up a namespace area for stuff.
namespace {



// GLOBALS


// FUNCTIONS



}  // end namespace


int create_input_IP_socket(const char *port, bool use_UDP)
{
  int		     sk, receive_buffer_size;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype = use_UDP ? SOCK_DGRAM : SOCK_STREAM;
  hints.ai_protocol = 0;//use_UDP ? IPPROTO_UDP : IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo* result;
  int retval = getaddrinfo(NULL, port, &hints, &result);
  if (retval != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
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
      fprintf(stderr, "Could not bind\n");
      exit(1);
  }
  
  freeaddrinfo(result);           /* No longer needed */
  sk = sfd;
  
  receive_buffer_size = 8 * 1024 * 1024;
  if (setsockopt(sk, SOL_SOCKET, SO_RCVBUF, &receive_buffer_size, sizeof receive_buffer_size) < 0) {
      perror("setsockopt failed:");
      exit(1);
  }
  
  return sk;
}










int main(int argc, char* argv[])
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    fprintf(stderr, "Warning: only tested on little-endian systems\n");
#endif
    if(argc != 6) {
        fprintf(stderr, "Error: correct usage is %s infile outfile0 outfile1 HexStartTime NUM_SECONDS\n", argv[0]);
        exit(2);
    }
    uint32_t start_time(strtol(argv[4],NULL,0));
    uint32_t NUM_seconds(strtol(argv[5],NULL,0));
    uint32_t END = start_time + NUM_seconds;

    
    uint16_t CLOCK_SPEED_IN_MHz = 0;





//     uint8_t Version = 0xAB;
//     uint8_t SourceInfo = 0xCD;
//     uint16_t Configuration = 0x1234;
//     uint16_t Station = 0x5678;
    uint8_t NUM_Beamlets = 61;
    uint8_t NUM_Blocks = 16;
    MPIfR::LOFAR::LOFAR_raw_data_type_enum DATA_TYPE = MPIfR::LOFAR::L_intComplex32_t;
    




    const int NUM_WRITERS = 2;
    uint_fast32_t Physical_Beamlets_Array0[NUM_Beamlets];
    uint_fast32_t Physical_Beamlets_Array1[NUM_Beamlets];
    for(uint_fast16_t beamlet=0; beamlet < NUM_Beamlets; beamlet++) {
        Physical_Beamlets_Array0[beamlet] = beamlet;
        Physical_Beamlets_Array1[beamlet] = beamlet+30;
    }
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Writer_Base* restrict * restrict WRITERS = new MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Writer_Base*[NUM_WRITERS];
//     WRITERS[0] = new MPIfR::LOFAR::Station::
//         LOFAR_Station_Beamformed_Writer_Raw0(argv[2],
//                                              time_t(start_time),
//                                              CLOCK_SPEED_IN_MHz,
//                                              0,
//                                              NUM_Beamlets,
//                                              NUM_Blocks,
//                                              DATA_TYPE,
//                                              0,
//                                              NUM_Beamlets,
//                                              Physical_Beamlets_Array0);
//     WRITERS[0] = new MPIfR::LOFAR::Station::
//         LOFAR_Station_Beamformed_Writer_Raw0(argv[2],
//                                              time_t(start_time),
//                                              CLOCK_SPEED_IN_MHz,
//                                              0,
//                                              NUM_Beamlets,
//                                              NUM_Blocks,
//                                              DATA_TYPE,
//                                              0,
//                                              30,
//                                              Physical_Beamlets_Array0);
//     WRITERS[1] = new MPIfR::LOFAR::Station::
//         LOFAR_Station_Beamformed_Writer_Raw0(argv[3],
//                                              time_t(start_time),
//                                              CLOCK_SPEED_IN_MHz,
//                                              0,
//                                              NUM_Beamlets,
//                                              NUM_Blocks,
//                                              DATA_TYPE,
//                                              1,
//                                              NUM_Beamlets-30,
//                                              Physical_Beamlets_Array1);
//     WRITERS[0] = new MPIfR::LOFAR::Station::
//         LOFAR_Station_Beamformed_Writer_Power0(argv[2],
//                                                time_t(start_time),
//                                                CLOCK_SPEED_IN_MHz,
//                                                0,
//                                                NUM_Beamlets,
//                                                NUM_Blocks,
//                                                DATA_TYPE,
//                                                0,
//                                                NUM_Beamlets,
//                                                Physical_Beamlets_Array0,
//                                                1.0,
//                                                1,
//                                                1.0,
//                                                MPIfR::LOFAR::L_Real64_t,
//                                                0);
    WRITERS[0] = new MPIfR::LOFAR::Station::
        LOFAR_Station_Beamformed_Writer_Power0(argv[2],
                                               time_t(start_time),
                                               CLOCK_SPEED_IN_MHz,
                                               0,
                                               NUM_Beamlets,
                                               NUM_Blocks,
                                               DATA_TYPE,
                                               0,
                                               30,
                                               Physical_Beamlets_Array0,
                                               1.0,
                                               1,
                                               1.0,
                                               MPIfR::LOFAR::L_Real64_t,
                                               0);
    WRITERS[1] = new MPIfR::LOFAR::Station::
        LOFAR_Station_Beamformed_Writer_Power0(argv[2],
                                               time_t(start_time),
                                               CLOCK_SPEED_IN_MHz,
                                               0,
                                               NUM_Beamlets,
                                               NUM_Blocks,
                                               DATA_TYPE,
                                               1,
                                               NUM_Beamlets-30,
                                               Physical_Beamlets_Array1,
                                               1.0,
                                               1,
                                               1.0,
                                               MPIfR::LOFAR::L_Real64_t,
                                               0);

    //fprintf(stderr, "writer made\n");

    int socket;
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG == 0)
    socket = create_input_IP_socket(argv[1], true);
#elif (LOFAR_DEBUG < 2)
    socket = open(argv[1], O_RDONLY);
#else
    socket = fileno(stdin);
#endif
    if(socket < 0) {
        perror("Error opening file");
        fprintf(stderr, "Error opening file '%s'\n", argv[1]);
        exit(2);
    }
    
    
//     uint32_t time_samples_per_second = MPIfR::LOFAR::Station::NUM_TIME_SAMPLES_PER_EVEN_SECOND(CLOCK_SPEED_IN_MHz);
//     uint32_t time_samples_per_two_seconds = MPIfR::LOFAR::Station::NUM_TIME_SAMPLES_PER_TWO_SECONDS(CLOCK_SPEED_IN_MHz);

    {
        MPIfR::LOFAR::Station::
            RSP_beamformed_packet_sorter sorter(socket,
                                                time_t(start_time),
                                                time_t(END),
                                                time_t(start_time),
                                                DATA_TYPE,
                                                NUM_Beamlets,
                                                NUM_Blocks,
                                                50, // ring buffer
                                                50,  // num packets to writers
                                                 0,  // num first drop
                                                30,  // num over time
                                                NUM_WRITERS,
                                                WRITERS,
                                                CLOCK_SPEED_IN_MHz);
        
        fprintf(stderr, "Sorter constructor status is %d\n", int(sorter.Final_Status()));
        int_fast32_t retval = sorter.process_packets();
        
        fprintf(stderr, "retval %d Final status is %d\n", int(retval), int(sorter.Final_Status()));
    }


#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 2)
    close(socket);
#else
#endif
    for(int i=0; i < NUM_WRITERS; i++) {
        delete WRITERS[i];
    }
    delete[] WRITERS; WRITERS=0;
    return 0;
}

