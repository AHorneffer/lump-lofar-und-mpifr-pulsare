#!/usr/bin/env python
# LOFAR_Station_Beamformed_Recorder.py
# basing running of LOFAR beamformed data recording software
#_HIST  DATE NAME PLACE INFO
# 2011 May 14  James M Anderson  --MPIfR  start
# 2012 Jan 24  JMA  --update parameters for packet buffers
# 2012 Mar 18  JMA --update for new logging system in main recorder
# 2012 Apr 20  JMA  --update for modulal setup
# 2013 Jun 19  JMA  ---- changes preparing for LuMP v2.0
# 2013 Jul 15  JMA  ---- update for RA,Dec,Epoch,SourceName, string options
# 2013 Sep 21  JMA  ---- add option to pre-pad to start time if there are
#                        missing packets at the start
# 2014 May 04  JMA  ---- use convert_FILE_port_to_absolute_path to get file path
#                        before changing directories, and clear data directory
#                        after change



# Copyright (c) 2011, 2012, 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



################################################################################



import os
import time
import math
import sys
import argparse
import logging
import subprocess
import calendar
import socket
import MPIfR_LOFAR_LuMP
import MPIfR_LOFAR_LuMP_Recorder_Common



################################################################################
# GLOBALS
################################################################################

PROGRAM_NAME = "LOFAR_Station_Beamformed_Recorder.py"
PROGRAM_VERSION = MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_VERSION_DATE_STRING

HOSTNAME = socket.gethostname()











################################################################################
def check_options(options):
    if(options.read_thread_buffer_size == 0):
        options.read_thread_buffer_size = 16384
    if(options.read_thread_buffer_size < 4096):
        logging.error("read_thread_buffer_size must be at least 4096, but was given as %d", options.read_thread_buffer_size)
        raise RuntimeError("bad read_thread_buffer_size")

    NUM_BARE_WRITERS = len(options.writer_type)
    logging.debug("Have %d bare writers requested", NUM_BARE_WRITERS)
    if((len(options.filename_base) != NUM_BARE_WRITERS)
       or (len(options.physical_beamlet_array) != NUM_BARE_WRITERS)
       or (len(options.subband_array) != NUM_BARE_WRITERS)
       or (len(options.rcumode_array) != NUM_BARE_WRITERS)
       or (len(options.rightascension_array) != NUM_BARE_WRITERS)
       or (len(options.declination_array) != NUM_BARE_WRITERS)
       or (len(options.epoch_array) != NUM_BARE_WRITERS)
       or (len(options.sourcename_array) != NUM_BARE_WRITERS)
       or (len(options.data_type_process) != NUM_BARE_WRITERS)
       or (len(options.data_type_out) != NUM_BARE_WRITERS)
       or (len(options.num_output_channels) != NUM_BARE_WRITERS)
       or (len(options.num_polyphase_filter_taps) != NUM_BARE_WRITERS)
       or (len(options.window_function) != NUM_BARE_WRITERS)
       or (len(options.window_parameter) != NUM_BARE_WRITERS)
       or (len(options.integration_time) != NUM_BARE_WRITERS)
       or (len(options.scale_by_inverse_samples) != NUM_BARE_WRITERS)
       or (len(options.extra_scale_factor) != NUM_BARE_WRITERS)
       or (len(options.bounds_check_output) != NUM_BARE_WRITERS)
       or (len(options.extra_string_option_0) != NUM_BARE_WRITERS)
       or (len(options.extra_string_option_1) != NUM_BARE_WRITERS)
       or (len(options.extra_string_option_2) != NUM_BARE_WRITERS)
       or (len(options.extra_string_option_3) != NUM_BARE_WRITERS)
       or (len(options.extra_string_option_4) != NUM_BARE_WRITERS)):
        logging.error("writer-based arrays are not all the same size.  You must give each writer option for each writer call")
        raise RuntimeError("bad writer setup")
    options.station_name = MPIfR_LOFAR_LuMP_Recorder_Common.convert_station_name(options.station_name)
    return options, NUM_BARE_WRITERS





################################################################################
def check_beamlet_validity(options):
    NUM_BARE_WRITERS = len(options.writer_type)
    #recorder_group = LuMP_Computer_Information.LuMP_Recorder_Group_Dict[options.recorder_group]
    # convert input strings to raw integer arrays
    NUM_TOTAL_BEAMLETS = options.beamlets_per_lane * MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES
    lane_needed = None
    for w in range(NUM_BARE_WRITERS):
        beamlet_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_int_beamlets(options.physical_beamlet_array[w],NUM_TOTAL_BEAMLETS)
        options.physical_beamlet_array[w] = beamlet_array
        subband_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_int_subbands(options.subband_array[w],NUM_TOTAL_BEAMLETS)
        options.subband_array[w] = subband_array
        rcumode_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_int_rcumodes(options.rcumode_array[w],NUM_TOTAL_BEAMLETS)
        options.rcumode_array[w] = rcumode_array
        rightascension_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_double_rightascensions(options.rightascension_array[w],NUM_TOTAL_BEAMLETS)
        options.rightascension_array[w] = rightascension_array
        declination_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_double_declinations(options.declination_array[w],NUM_TOTAL_BEAMLETS)
        options.declination_array[w] = declination_array
        epoch_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_string_epochs(options.epoch_array[w],NUM_TOTAL_BEAMLETS)
        options.epoch_array[w] = epoch_array
        sourcename_array = MPIfR_LOFAR_LuMP_Recorder_Common.get_string_sourcenames(options.sourcename_array[w],NUM_TOTAL_BEAMLETS)
        options.sourcename_array[w] = sourcename_array
        if((len(beamlet_array) != len(subband_array))
           or (len(beamlet_array) != len(rcumode_array)) 
           or (len(beamlet_array) != len(rightascension_array)) 
           or (len(beamlet_array) != len(declination_array)) 
           or (len(beamlet_array) != len(epoch_array)) 
           or (len(beamlet_array) != len(sourcename_array)) ):
            logging.error("sizes of physical_beamlet_array, subband_array, rcumode_array, rightascension_array, declination_array, epoch_array, and sourcename_array for writer %d do not match", w)
            logging.error("sizes are %d %d %d %d %d %d %d",
                          len(beamlet_array), len(subband_array),
                          len(rcumode_array), len(rightascension_array),
                          len(declination_array), len(epoch_array),
                          len(sourcename_array))
            raise RuntimeError("bad writer setup")
        # now check that our recording computer setup can handle this
        need_lane = [False]*MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES
        for beamlet in beamlet_array:
            lane = beamlet / options.beamlets_per_lane
            if((lane < 0) or (lane >= MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES)):
                logging.error("physical beamlet %d is in lane %d (%d bemlets per lane), which is not a valid LOFAR lane", beamlet, lane, options.beamlets_per_lane)
                raise RuntimeError("lane out of range")
            need_lane[lane] = True
        for lane in range(MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES):
            # if(need_lane[lane]):
            #     if(LuMP_Computer_Information.LuMP_Recorder_Group_Dict[options.recorder_group]["recorder_lane%d"%lane] is None):
            #         logging.error("physical beamlets requested for writer %d corespond to non-existent recording computer for recorder_group '%s'", w, options.recorder_group)
            #         raise RuntimeError("bad writer setup")
            if(lane_needed is None):
                if(need_lane[lane]):
                    lane_needed = lane
            elif(need_lane[lane]):
                logging.error("recording beamlets from multiple RSP lanes is not supported by this software --- you must create multiple instances of the recorder, one for each lane")
                raise RuntimeError("multiple RSP lanes required")
    if(lane_needed is None):
        logging.error("no RSP lane detected --- nothing to do")
        raise RuntimeError("no RSP lane detected")
    return options, lane_needed

############################################################################
def bool_to_digit_str(b):
    if(b):
        return "1"
    return "0"





















############################################################################
def main():
    p = argparse.ArgumentParser(description="Python program to run LOFAR_Station_Beamformed_Recorder on a single LOFAR recording computer.", epilog="See the accompanying manual for more information.")
    # General setup
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_general_setup(p)
    # CPU setup
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_recorder_cpu_specification(p)
    # setup for specific writers
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_writer_setup("LuMP0",
                                                                         "L_intComplex32_t",
                                                                         "L_intComplex32_t",
                                                                         1,
                                                                         1,
                                                                         "0",
                                                                         1.0,
                                                                         p,
                                                                         True,
                                                                         'append')
    # Information for the user
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_user_info(p, PROGRAM_VERSION)

    # Things that should not need to be changed
    #p.add_argument("--num_writers",default=1,type=int,help="The numbr of writers to divide the writing among.  In the future, this will be optimized.  Defaults to %(default)d.")
    p.add_argument("--read_thread_buffer_size",default=0,type=int,help="Size of the packet read thread buffer, in packets.  If 0, this program will compute an optimal size based on some factors.  The minimum value when a specific value is provided, is 4096.  Defaults to %(default)d.")
    p.add_argument("--ring_buffer_size",default=0,type=int,help="Size of the packet reader ring buffer, in packets.  If 0, this program will compute an optimal size based on the cache size of the CPU.  Defaults to %(default)d.")
    p.add_argument("--num_packets_to_writers",default=0,type=int,help="Numbr of packets to deliver to the writers at once.  If 0, this program will compute an optimal size based on the cache size of the CPU.  Defaults to %(default)d.")
    p.add_argument("--num_first_packets_drop",default=0,type=int,help="The number of initial packets to drop, assuming that there may be problems with packets stored in the system somewhere.  Defaults to %(default)d.")
    p.add_argument("--num_over_time",default=30,type=int,help="The number of packets past the official stop time to read in case there are missing packets in the UDP packet stream.  Defaults to %(default)d.")
    p.add_argument("--num_samples_per_block",default=16,type=int,help="number of samples per block (packet).  Defaults to %(default)d")
    p.add_argument("--packet_size",default=7824,type=int,help="size of a packet, in bytes.  Defaults to %(default)d.")
    if((len(sys.argv) == 2) and (sys.argv[1] == "--stdin")):
        argv = MPIfR_LOFAR_LuMP_Recorder_Common.read_arguments_from_stdin()
        options = p.parse_args(argv)
    else:
        options = p.parse_args()
    options = MPIfR_LOFAR_LuMP_Recorder_Common.convert_FILE_port_to_absolute_path(options)
    MPIfR_LOFAR_LuMP_Recorder_Common.make_and_change_to_data_directory(options.datadir)
    options.datadir = "."
    MPIfR_LOFAR_LuMP_Recorder_Common.setup_logging(PROGRAM_NAME,options)
    
    options = MPIfR_LOFAR_LuMP_Recorder_Common.check_general_setup_options(options)
    options = MPIfR_LOFAR_LuMP_Recorder_Common.check_recorder_cpu_specification(options)
    options, NUM_BARE_WRITERS = check_options(options)

    options, lane_needed = check_beamlet_validity(options)


    NUM_CPU = options.recorder_num_cores
    CACHE_SIZE = options.recorder_cache_size
    logging.info("Found %d cores with %d cache", NUM_CPU,CACHE_SIZE)
    if(NUM_CPU < 2):
        logging.warning("only %d CPUs detected --- performance will be bad",NUM_CPU)
    packets_per_cache = CACHE_SIZE/options.packet_size
    # only take up 2/3 of the cache for the sorter thread
    packet_store = 2*packets_per_cache/3
    ring_packets_size=0
    writer_packets_size=0
    if(packet_store > 128):
        ring_packets_size = 64
        writer_packets_size = (packet_store-ring_packets_size)/3
    else:
        ring_packets_size = packet_store/2
        writer_packets_size = ring_packets_size/2
    if(ring_packets_size < 512):
        ring_packets_size = 512
    writer_packets_size=512
    if(ring_packets_size<1.5):
        ring_packets_size=1
    else:
        # nearest power of 2
        ring_packets_size = 2**(int(math.log(ring_packets_size)/math.log(2)+0.5))
    if(writer_packets_size<1.5):
        writer_packets_size=1
    else:
        # nearest power of 2
        writer_packets_size = 2**(int(math.log(writer_packets_size)/math.log(2)+0.5))
    logging.info("Optimal buffer sizes in packets for your CPU:\n    ring buffer %d writer dump %d",ring_packets_size,writer_packets_size)
    if(options.ring_buffer_size == 0):
        options.ring_buffer_size = ring_packets_size
    if(options.num_packets_to_writers == 0):
        options.num_packets_to_writers = writer_packets_size
    start_time = MPIfR_LOFAR_LuMP_Recorder_Common.convert_ISO_date_string(options.start_date)
    end_time = start_time + options.duration


    NUM_WRITERS = NUM_BARE_WRITERS
    #logging.debug("point-1 have %d", NUM_CPU)
    if(NUM_CPU >= 2):
        NUM_WRITER_CPU = NUM_CPU-1
        #logging.debug("point0 have %d", NUM_WRITER_CPU)
        #logging.debug("point0 have %d %d", NUM_BARE_WRITERS,MPIfR_LOFAR_LuMP_Recorder_Common.get_writer_type(options.writer_type[0]))
        if((NUM_BARE_WRITERS == 1)
           and (MPIfR_LOFAR_LuMP_Recorder_Common.get_writer_type_atat(options.writer_type[0]) == "multi_thread")):
            #logging.debug("point1")
            # spread the writing across all available writer CPUs
            NUM_BEAMLETS_OUT = len(options.physical_beamlet_array[0])
            if(NUM_BEAMLETS_OUT < NUM_WRITER_CPU):
                NUM_WRITER_CPU = NUM_BEAMLETS_OUT
            NUM_WRITERS = NUM_WRITER_CPU
            if(NUM_BEAMLETS_OUT > NUM_BARE_WRITERS):
                ba = []
                sa = []
                rcua = []
                raa  = []
                deca = []
                epoa = []
                soua = []
                NUM_PER_WRITER = int(math.ceil(float(NUM_BEAMLETS_OUT)/NUM_WRITER_CPU))
                start=0
                for i in range(NUM_WRITER_CPU-1):
                    ba.append(options.physical_beamlet_array[0][start:start+NUM_PER_WRITER])
                    sa.append(options.subband_array[0][start:start+NUM_PER_WRITER])
                    rcua.append(options.rcumode_array[0][start:start+NUM_PER_WRITER])
                    raa.append(options.rightascension_array[0][start:start+NUM_PER_WRITER])
                    deca.append(options.declination_array[0][start:start+NUM_PER_WRITER])
                    epoa.append(options.epoch_array[0][start:start+NUM_PER_WRITER])
                    soua.append(options.sourcename_array[0][start:start+NUM_PER_WRITER])
                    start += NUM_PER_WRITER
                ba.append(options.physical_beamlet_array[0][start:])
                sa.append(options.subband_array[0][start:])
                rcua.append(options.rcumode_array[0][start:])
                raa.append(options.rightascension_array[0][start:])
                deca.append(options.declination_array[0][start:])
                epoa.append(options.epoch_array[0][start:])
                soua.append(options.sourcename_array[0][start:])
                options.writer_type = options.writer_type*NUM_WRITER_CPU
                options.filename_base = options.filename_base*NUM_WRITER_CPU
                options.physical_beamlet_array = ba
                options.subband_array          = sa
                options.rcumode_array          = rcua
                options.rightascension_array   = raa
                options.declination_array      = deca
                options.epoch_array            = epoa
                options.sourcename_array       = soua
                options.data_type_process = options.data_type_process*NUM_WRITER_CPU
                options.data_type_out = options.data_type_out*NUM_WRITER_CPU
                options.num_output_channels = options.num_output_channels*NUM_WRITER_CPU
                #options.channelizer_type = options.channelizer_type*NUM_WRITER_CPU
                options.num_polyphase_filter_taps = options.num_polyphase_filter_taps*NUM_WRITER_CPU
                options.window_function = options.window_function*NUM_WRITER_CPU
                options.window_parameter = options.window_parameter*NUM_WRITER_CPU
                options.integration_time = options.integration_time*NUM_WRITER_CPU
                options.scale_by_inverse_samples = options.scale_by_inverse_samples*NUM_WRITER_CPU
                options.extra_scale_factor = options.extra_scale_factor*NUM_WRITER_CPU
                options.bounds_check_output = options.bounds_check_output*NUM_WRITER_CPU
                options.extra_string_option_0 = options.extra_string_option_0*NUM_WRITER_CPU
                options.extra_string_option_1 = options.extra_string_option_1*NUM_WRITER_CPU
                options.extra_string_option_2 = options.extra_string_option_2*NUM_WRITER_CPU
                options.extra_string_option_3 = options.extra_string_option_3*NUM_WRITER_CPU
                options.extra_string_option_4 = options.extra_string_option_4*NUM_WRITER_CPU

    command = ["LOFAR_Station_Beamformed_Recorder"]
    command.append("%s"%options.main_recorder_logfile)   #  1 logfile
    command.append("0x%X"%start_time)                    #  2 start time_t
    command.append("0x%X"%end_time)                      #  3 end time_t                           
    command.append(str(options.port))                    #  4 port number                          
    command.append(str(MPIfR_LOFAR_LuMP_Recorder_Common.get_data_type(options.data_type_in)))
    #                                                    #  5 LOFAR data type from station         
    command.append(str(options.clock_speed))             #  6 clock speed in MHz                   
    command.append(str(options.beamlets_per_lane))       #  7 num beamlets in packet               
    command.append(str(options.num_samples_per_block))   #  8 samples per packet                   
    command.append(str(options.beamlets_per_lane * lane_needed))
                                                         #  9 beamlet offset for this recorder     
    command.append(str(options.read_thread_buffer_size)) # 10 read thread buffer size, in packets  
    command.append(str(options.ring_buffer_size))        # 11 packet ring buffer size, in packets  
    command.append(str(options.num_packets_to_writers))  # 12 number packets to give to writers    
    command.append(str(options.num_first_packets_drop))  # 13 drop first N packets                 
    command.append(str(options.num_over_time))           # 14 wait for N packets after end time    
    command.append(str(NUM_WRITERS))                     # 15 total number of writers              
    command.append(options.station_name)                 # 16 name of the station
    command.append(bool_to_digit_str(options.backfill_to_start_time))
                                                         # 17 backfill_to_start_time
    for w in range(NUM_WRITERS):
        logging.info("setting up information for writer %d, type %d=%s", w, MPIfR_LOFAR_LuMP_Recorder_Common.get_writer_type(options.writer_type[w]), MPIfR_LOFAR_LuMP_Recorder_Common.get_writer_type_string(options.writer_type[w]))
        beamlet_string=""
        subband_string = ""
        rcumode_string = ""
        ra_string = ""
        dec_string = ""
        epoch_string = ""
        source_string = ""
        THIS_NUM_BEAMLETS = len(options.physical_beamlet_array[w])
        for i in range(THIS_NUM_BEAMLETS):
            beamlet_string += "%d,"%(options.physical_beamlet_array[w][i])
            subband_string += "%d,"%(options.subband_array[w][i])
            rcumode_string += "%d,"%(options.rcumode_array[w][i])
            ra_string += "%.16E,"%(options.rightascension_array[w][i])
            dec_string += "%.16E,"%(options.declination_array[w][i])
            epoch_string += "%s,"%(options.epoch_array[w][i])
            source_string += "%s,"%(options.sourcename_array[w][i])
        beamlet_string = '['+beamlet_string[:-1]+']'
        subband_string = '['+subband_string[:-1]+']'
        rcumode_string = '['+rcumode_string[:-1]+']'
        ra_string = '['+ra_string[:-1]+']'
        dec_string = '['+dec_string[:-1]+']'
        epoch_string = '['+epoch_string[:-1]+']'
        source_string = '['+source_string[:-1]+']'
        command.append("%d"%(MPIfR_LOFAR_LuMP_Recorder_Common.get_writer_type(options.writer_type[w])))
        #                                                      #  0 writer type
        command.append("%s.%2.2X"%(options.filename_base[w],w))#  1 base filename                      
        command.append(str(THIS_NUM_BEAMLETS))                 #  2 number beamlets in this writer     
        command.append(str(beamlet_string))                    #  3 physical beamlet array             
        command.append(str(subband_string))                    #  4 subband array                      
        command.append(str(rcumode_string))                    #  5 rcumode array                      
        command.append(str(ra_string))                         #  6 rightascension array                      
        command.append(str(dec_string))                        #  7 declination array                      
        command.append(str(epoch_string))                      #  8 epoch array 
        command.append(str(source_string))                     #  9 sourcename array                      
        command.append("%d"%(MPIfR_LOFAR_LuMP_Recorder_Common.get_data_type(options.data_type_process[w])))
        #                                                      # 10 processing data type               
        command.append("%d"%(MPIfR_LOFAR_LuMP_Recorder_Common.get_data_type(options.data_type_out[w])))
        #                                                      # 11 output data type                   
        command.append(str(options.num_output_channels[w]))    # 12 number of output channels per sb   
        command.append(str(options.num_polyphase_filter_taps[w]))
        #                                                      # 13 number of polyphase filter taps
        command.append("%d"%(MPIfR_LOFAR_LuMP_Recorder_Common.get_window_type(options.window_function[w])))
        #                                                      # 14 the type of windowing function to use
        command.append("%.16E"%options.window_parameter[w])    # 15 parameter option for the windowing function
        command.append("%.16E"%1.0)                            # 16 how far relative to a standard     
                                                               #    FFT channelizer shift in samples   
                                                               #    does the channelizer shift each    
                                                               #    channelization                     
        command.append("%.16E"%options.integration_time[w])    # 17 integration time, in s             
        command.append(str(options.scale_by_inverse_samples[w]))
        #                                                      # 18 bool for scaling by samples        
        command.append("%.16E"%options.extra_scale_factor[w])  # 19 extra scaling factor               
        command.append(str(options.bounds_check_output[w]))    # 20 bool for bounds checking    
        command.append(str(options.extra_string_option_0[w]))  # 21 extra string option 0
        command.append(str(options.extra_string_option_1[w]))  # 22 extra string option 1
        command.append(str(options.extra_string_option_2[w]))  # 23 extra string option 2
        command.append(str(options.extra_string_option_3[w]))  # 24 extra string option 3
        command.append(str(options.extra_string_option_4[w]))  # 25 extra string option 4
    logging.info("Running command %s",command)
    for i,arg in enumerate(command):
        logging.debug("command arg %d is '%s'", i,arg)
        # if(type(arg) == type(1)):
        #     logging.error("arg is int")
        #     raise RuntimeError("int arg")
        # if('\0' in arg):
        #     logging.error("bad string value --- contains 0")
        #     raise RuntimeError("bad string")
    if(not options.echo_only):
        try:
            # p = subprocess.Popen(command,shell=False,
            #                      stdout=subprocess.PIPE,
            #                      stderr=subprocess.STDOUT)
            # p = subprocess.Popen(command,shell=False)
            # os.nice(10)
            # p2 = subprocess.Popen(["tee", "-a", "%s.log"%command[0]],
            #                       stdin=p.stdout)
            
##             while(True):
##                 while(True):
##                     s = p.stderr.readline()
##                     if(s):
##                         if(s[-1] == '\n'):
##                             logging.info("READER: %s"%s[:-1])
##                         else:
##                             logging.info("READER: %s"%s)
##                     else:
##                         break
##                 while(True):
##                     s = p.stdout.readline()
##                     if(s):
##                         if(s[-1] == '\n'):
##                             logging.info("reader: %s"%s[:-1])
##                         else:
##                             logging.info("reader: %s"%s)
##                     else:
##                         break
##                 if(p.poll()):
##                     break
##             retcode = p.returncode
            # retcode = os.waitpid(p.pid,0)[1]
            retcode = subprocess.call(command,shell=False)
            logging.info("command %s returned %d", command[0], retcode)
            if(retcode != 0):
                logging.error("command %s had an error %d", command[0], retcode)
                raise RuntimeError("command failed")
        except OSError, e:
            logging.error("exception found trying to run command %s", command[0])
            raise e
    logging.info("command %s finished ok", command[0])









if __name__ == "__main__":
    main()
