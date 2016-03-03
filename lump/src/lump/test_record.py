#!/usr/bin/env python
# test_record.py
# basing running of LOFAR beamformed data recording software
#_HIST  DATE NAME PLACE INFO
# 2011 May 14  James M Anderson  --MPIfR  start
# 2012 Apr 20  JMA  --update for modulal setup



# Copyright (c) 2011, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
import MPIfR_LOFAR_LuMP
import MPIfR_LOFAR_LuMP_Recorder_Common



################################################################################
# GLOBALS
################################################################################

PROGRAM_NAME = "test_record.py"
PROGRAM_VERSION = MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_VERSION_DATE_STRING
















################################################################################
def get_cpuinfo():
    NUM_CPU=0
    CACHE_SIZE=0
    fp=open("/proc/cpuinfo","r")
    for line in fp:
        if(line.startswith("processor\t:")):
            num=int(line[len("processor\t:"):])
            if(num>NUM_CPU):
                NUM_CPU=num+1
            print num
        elif(line.startswith("cache size\t:")):
            vals=line[len("cache size\t:"):].split()
            size=int(vals[0])
            if(len(vals)>1):
                if(vals[1].strip() == "KB"):
                    size *= 1024
                elif(vals[1].strip() == "MB"):
                    size *= 1024*1024
            if(CACHE_SIZE==0):
                CACHE_SIZE=size
            elif(size < CACHE_SIZE):
                CACHE_SIZE = size
    return NUM_CPU, CACHE_SIZE


################################################################################
def get_int_subbands(sarray,NUM_BEAMLETS):
    BOTTOM_SUBBAND = 0
    TOP_SUBBAND = NUM_LOFAR_STATION_SUBBANDS
    if(len(sarray) < 3):
        logging.error("Malformed subbands array argument --- not big enough")
        raise RuntimeError("bad subbands/beamlets")
    if((sarray[0] != '[') or (sarray[-1] != ']')):
        logging.error("Malformed subbands array argument --- does not start and end with square brackets")
        raise RuntimeError("bad subbands")
    try:
        array = sarray[1:-1].replace(' ','').strip().replace(',',' ').split
        outarray = []
        for a in array:
            if(a.find(':')>0):
                aa = map(int,a.replace(':',' ').split)
                if(len(aa) != 2):
                    logging.error("Malformed subband array argument --- range entry '%s' does not have 2 entries.")
                    raise RuntimeError("bad subbands")
                if((aa[0] < BOTTOM_SUBBAND) or (aa[0] >= TOP_SUBBAND)
                   or(aa[1] < BOTTOM_SUBBAND) or (aa[1] >= TOP_SUBBAND)):
                    logging.error("Specified subband range '%s' exceeds LOFAR range", a)
                    raise RuntimeError("bad subbands")
                aa = range(aa[0],aa[1])
                outarray = outarray + aa
            else:
                aa = int(a)
                if((aa < BOTTOM_SUBBAND) or (aa >= TOP_SUBBAND)):
                    logging.error("Specified subband '%s' exceeds LOFAR range", a)
                    raise RuntimeError("bad subbands")
                outarray.append(aa)
        if(len(outarray) > NUM_BEAMLETS):
            logging.error("Num subbands given is more than the total number of beamlets")
            raise RuntimeError("bad subbands")
    except:
        logging.error("Malformed subbands argument --- cannot parse")
        raise RuntimeError("bad subbands")
    return outarray

################################################################################
def get_int_beamlets(sarray,PHYSICAL_BEAMLET_OFFSET, NUM_BEAMLETS):
    BOTTOM_BEAMLET = PHYSICAL_BEAMLET_OFFSET
    TOP_BEAMLET = PHYSICAL_BEAMLET_OFFSET + NUM_BEAMLETS
    if(len(sarray) < 3):
        logging.error("Malformed beamlets array argument --- not big enough")
        raise RuntimeError("bad beamlets")
    if((sarray[0] != '[') or (sarray[-1] != ']')):
        logging.error("Malformed beamlets array argument --- does not start and end with square brackets")
        raise RuntimeError("bad beamlets")
    try:
        array = sarray[1:-1].replace(' ','').strip().replace(',',' ').split
        outarray = []
        for a in array:
            if(a.find(':')>0):
                aa = map(int,a.replace(':',' ').split)
                if(len(aa) != 2):
                    logging.error("Malformed beamlets array argument --- range entry '%s' does not have 2 entries.")
                    raise RuntimeError("bad beamlets")
                if((aa[0] < BOTTOM_BEAMLET) or (aa[0] >= TOP_BEAMLET)
                   or(aa[1] < BOTTOM_BEAMLET) or (aa[1] >= TOP_BEAMLET)):
                    logging.error("Specified beamlet range '%s' exceeds allowed range", a)
                    raise RuntimeError("bad beamlets")
                aa = range(aa[0],aa[1])
                outarray = outarray + aa
            else:
                aa = int(a)
                if((aa < BOTTOM_BEAMLET) or (aa >= TOP_BEAMLET)):
                    logging.error("Specified beamlet '%s' exceeds allowed range", a)
                    raise RuntimeError("bad beamlets")
                outarray.append(aa)
        if(len(outarray) > NUM_BEAMLETS):
            logging.error("Num beamlets given is more than the total number of beamlets")
            raise RuntimeError("bad beamlets")
    except:
        logging.error("Malformed beamlets argument --- cannot parse")
        raise RuntimeError("bad beamlets")
    return outarray






############################################################################
def main():
    p = argparse.ArgumentParser(description="test Python program to run LOFAR_Station_Beamformed_Recorder on a single LOFAR recording computer.", epilog="See the accompanying manual for more information.")
    # General setup
    p.add_argument("start_date", type=str,help="*REQUIRED*  The date and time to begin recording, as a UTC ISO date string of the format YYYY-MM-DDTHH:MM:SS", required=True)
    p.add_argument("--duration",type=int,help="*REQUIRED*  Duration of measurment to listen to the station, in seconds", required=True)
    p.add_argument("--port", "-p",type=int,help="*REQUIRED*  port to listen to", required=True)
    p.add_argument("--data_type_in", default=26,type=int,help="data type of station beamforemd data, see LOFAR_Station_Complex.h.  Defaults to %(default)d")
    p.add_argument("--clock_speed",default=200,type=int,choices=[200,160],help="Clock speed of station, in MHz.  Defaults to %(default)d")
    p.add_argument("--num_beamlets",default=61,type=int,help="number of beamlets sent to this computer in the beamformed data.  Defaults to %(default)d")
    p.add_argument("--physical_beamlet_offset",type=int,help="*REQUIRED*  beamlet number on the station corresponding to the 0th beamlet received by this computer", required=True)
    p.add_argument("--station_name",default="Ef",type=str,choices=["Ef", "Uw", "Tb", "Po", "Ju", "Nc", "On", "Cb", "Nd", "DE601","DE602","DE603","DE604","DE605","FR606","SE607","UK608","DE609"],help="LOFAR name of the station dumping data to this writer.  Defaults to %(default)s")
    # setup for specific writers
    p.add_argument("--writer_type",action='append',type=int,help="The enum code of the writer type to use.  See LOFAR_Station_Common.h  ", required=True)
    p.add_argument("--filename_base",action='append',type=str,help="*REQUIRED*  The base filename to give to the data", required=True)
    p.add_argument("--RCUMODE", action='append',type=int, choices=[-1,1,2,3,4,5,6,7,8,9],help="*REQUIRED* the RCUMODE being used.", required=True)
    p.add_argument("--physical_beamlet_array", action='append',type=str, help="*REQUIRED*  Python-like array of the physical beamlets to use for this writer.  A combination of indiidual physical beamlets and Python ranges may be used, such as '[0,1,4,7,10:31,60:62]'.  Ranges must have both start and end specified.  Note that the notation here is a Python notation for the ranges (start, start+1,start+2,...,end-1), which is different from the ASTRON LOFAR station software.", required=true)
    p.add_argument("--subband_array", action='append',type=str, help="*REQUIRED*  Python array of the subbands corresponding to the beamlets.  A combination of indiidual physical subbands and Python ranges may be used, such as '[100,101,104,107,110:131,160:162]'.  Ranges must have both start and end specified.  Note that the notation here is a Python notation for the ranges (start, start+1,start+2,...,end-1), which is different from the ASTRON LOFAR station software.  The PHYSICAL_BEAMLET_ARRAY and SUBBAND_ARRAY should match beamlet to subband at the same index.", required=True)
    p.add_argument("--data_type_process", action='append',type=int,help="*REQUIRED*  Data type for internal processing",required=True)
    p.add_argument("--data_type_out", action='append',type=int,help="*REQUIRED*  Data type for output to disk",required=True)
    p.add_argument("--channelizer_type", action='append',type=int,help="*REQUIRED*  Type of channelizer to use",required=True)
    p.add_argument("--num_output_channels", action='append',type=int,help="*REQUIRED*  Number of output channels to make",required=True)
    p.add_argument("--channelizer_shift", action='append',type=float,help="*REQUIRED*  How far, relative to a normal FFT setup (N samples for complex data to N channels, or 2N samples for real-valued data to N channels) should the channelizer shift each channelization timestep.  Usually this will be 1.0.",required=True)
    p.add_argument("--integration_time",action='append',type=float,help="*REQUIRED*  The integration time, in seconds, for averaging the total power data.  If not averaging, just give 1.0.", required=True)
    p.add_argument("--scale_by_inverse_samples",action='append',type=int,choices=[0,1],help="*REQUIRED*  Should the total power values should be scaled by the number of samples per integration?  0 No, or 1 Yes.", required=True)
    p.add_argument("--extra_scale_factor",action='append',type=float,help="*REQUIRED*  Extra scaling factor for total power.  Use 1.0 for default.", required=True)
    p.add_argument("--bounds_check_output",action='append',type=int,choices=[0,1],help="*REQUIRED*  Should the software do bounds checking when converting data types?  0 No, or 1 Yes.", required=True)
    # Information for the user
    p.add_argument("--verbose", "-v", action="store_true", help="write commands to screen as well as executing")
    p.add_argument("--echo_only", action="store_true", help="Only show the commands that would be processed, do not actually run them")
    p.add_argument("-V", "--version", action='version', version='%(prog)s '+"%s"%PROGRAM_VERSION, help="Print the version number of this software and exit.")
    # Things that should not need to be changed
    #p.add_argument("--num_writers",default=1,type=int,help="The numbr of writers to divide the writing among.  In the future, this will be optimized.  Defaults to %(default)d.")
    p.add_argument("--ring_buffer_size",default=0,type=int,help="Size of the packet reader ring buffer, in packets.  If 0, this program will compute an optimal size based on the cache size of the CPU.  Defaults to %(default)d.")
    p.add_argument("--num_packets_to_writers",default=0,type=int,help="Numbr of packets to deliver to the writers at once.  If 0, this program will compute an optimal size based on the cache size of the CPU.  Defaults to %(default)d.")
    p.add_argument("--num_first_packets_drop",default=0,type=int,help="The number of initial packets to drop, assuming that there may be problems with packets stored in the system somewhere.  Defaults to %(default)d.")
    p.add_argument("--num_over_time",default=30,type=int,help="The number of packets past the official stop time to read in case there are missing packets in the UDP packet stream.  Defaults to %(default)d.")
    p.add_argument("--num_samples_per_block",default=16,type=int,help="number of samples per block (packet).  Defaults to %(default)d")
    p.add_argument("--packet_size",default=7824,type=int,help="size of a packet, in bytes.  Defaults to %(default)d.")
    options = p.parse_args()
    setup_logging(PROGRAM_NAME,'.')
    if(options.duration < 1):
        logging.error("duration must be at least 1 second")
        raise RuntimeError("duration bad")
    NUM_BARE_WRITERS = len(options.writer_type)
    if((len(options.filename_base) != NUM_BARE_WRITERS)
       or (len(options.RCUMODE) != NUM_BARE_WRITERS)
       or (len(options.physical_beamlet_array) != NUM_BARE_WRITERS)
       or (len(options.subband_array) != NUM_BARE_WRITERS)
       or (len(options.data_type_process) != NUM_BARE_WRITERS)
       or (len(options.data_type_out) != NUM_BARE_WRITERS)
       or (len(options.channelizer_type) != NUM_BARE_WRITERS)
       or (len(options.num_output_channels) != NUM_BARE_WRITERS)
       or (len(options.channelizer_shift) != NUM_BARE_WRITERS)
       or (len(options.integration_time) != NUM_BARE_WRITERS)
       or (len(options.scale_by_inverse_samples) != NUM_BARE_WRITERS)
       or (len(options.extra_scale_factor) != NUM_BARE_WRITERS)
       or (len(options.bounds_check_output) != NUM_BARE_WRITERS)):
        logging.error("writer-based arrays are not all the same size.  You must give each writer option for each writer call")
        raise RuntimeError("bad writer setup")
    options.station_name = convert_station_name(options.station_name)
    NUM_CPU,CACHE_SIZE=get_cpuinfo()
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
    if(ring_packets_size<1):
        ring_packets_size=1
    if(writer_packets_size<1):
        writer_packets_size=1
    logging.info("Optimal buffer sizes in packets for your CPU:\n    ring buffer %d writer dump %d\n",ring_packets_size,writer_packets_size)
    if(options.ring_buffer_size == 0):
        options.ring_buffer_size = ring_packets_size
    if(options.num_packets_to_writers == 0):
        options.num_packets_to_writers = ring_packets_size
    start_time = int(time.time() + 1.0)
    end_time = start_time + options.duration
    for w in range(options.num_writers):
        beamlet_array = get_int_beamlets(options.physical_beamlet_array[w],
                                         options.physical_beamlet_offset,
                                         options.num_beamlets)
        options.physical_beamlet_array[w] = beamlet_array
        subband_array = get_int_subbands(options.subband_array[w],
                                         options.num_beamlets)
        options.subband_array[w] = subband_array
        if(len(beamlet_array) != len(subband_array)):
            logging.error("sizes of physical beamlets and subbands arrays for writer %d do not match", w)
            sys.exit(2)
    NUM_WRITERS = NUM_BARE_WRITERS
    if(NUM_CPU >= 2):
        NUM_WRITER_CPU = NUM_CPU-1
        if((NUM_BARE_WRITERS == 1)
           and (options.writer_type in [3,5])):
            # spread the writing across all available writer CPUs
            NUM_WRITERS = NUM_WRITER_CPU
            NUM_BEAMLETS_OUT = len(options.physical_beamlet_array[0])
            if(NUM_BEAMLETS_OUT > NUM_BARE_WRITERS):
                ba = []
                sa = []
                NUM_PER_WRITER = NUM_BEAMLETS_OUT/NUM_WRITER_CPU
                start=-NUM_PER_WRITER
                for i in range(NUM_WRITER_CPU):
                    start += NUM_PER_WRITER
                    ba.append(options.physical_beamlet_array[0][start:start+NUM_PER_WRITER])
                    sa.append(options.subband_array[0][start:start+NUM_PER_WRITER])
                ba[-1] = options.physical_beamlet_array[0][start:]
                sa[-1] = options.subband_array[0][start:]
                options.writer_type = options.writer_type*NUM_WRITER_CPU
                options.filename_base = options.filename_base*NUM_WRITER_CPU
                options.RCUMODE = options.RCUMODE*NUM_WRITER_CPU
                options.physical_beamlet_array = ba
                options.subband_array = sa
                options.data_type_process = options.data_type_process*NUM_WRITER_CPU
                options.data_type_out = options.data_type_out*NUM_WRITER_CPU
                options.channelizer_type = options.channelizer_type*NUM_WRITER_CPU
                options.num_output_channels = options.num_output_channels*NUM_WRITER_CPU
                options.channelizer_shift = options.channelizer_shift*NUM_WRITER_CPU
                options.integration_time = options.integration_time*NUM_WRITER_CPU
                options.scale_by_inverse_samples = options.scale_by_inverse_samples*NUM_WRITER_CPU
                options.extra_scale_factor = options.extra_scale_factor*NUM_WRITER_CPU
                options.bounds_check_output = options.bounds_check_output*NUM_WRITER_CPU

    command = ["LOFAR_Station_Beamformed_Recorder"]
    command.append("0x%X"%start_time)                    #  1 start time_t
    command.append("0x%X"%end_time)                      #  2 end time_t                           
    command.append(str(options.port))                    #  3 port number                          
    command.append(str(options.data_type_in))            #  4 LOFAR data type from station         
    command.append(str(options.clock_speed))             #  5 clock speed in MHz                   
    command.append(str(options.num_beamlets))            #  6 num beamlets in packet               
    command.append(str(options.num_samples_per_block))   #  7 samples per packet                   
    command.append(str(options.physical_beamlet_offset)) #  8 beamlet offset for this recorder     
    command.append(str(options.ring_buffer_size))        #  9 packet ring buffer size, in packets  
    command.append(str(options.num_packets_to_writers))  # 10 number packets to give to writers    
    command.append(str(options.num_first_packets_drop))  # 11 drop first N packets                 
    command.append(str(options.num_over_time))           # 12 wait for N packets after end time    
    command.append(str(NUM_WRITERS))                     # 13 total number of writers              
    command.append(options.station_name)                 # 14 name of the station
    for w in range(options.num_writers):
        Nyquist_Offset = None
        if(options.clock_speed == 200):
            if((options.RCUMODE[w] >=1) and (options.RCUMODE[w] <= 4)):
                Nyquist_Offset = 0.0 # Hz
            elif(options.RCUMODE[w] == 5):
                Nyquist_Offset = 100.0E6
            elif(options.RCUMODE[w] == 7):
                Nyquist_Offset = 200.0E6
            elif(options.RCUMODE[w] == -1):
                Nyquist_Offset = -1E15
            else:
                logging.error("RCUMODE and CLOCK do not match")
                raise RuntimeError("bad RCUMODE and CLOCK")
        elif(options.clock_speed[w] == 160):
            if(options.RCUMODE[w] == 6):
                Nyquist_Offset = 160.0E6
            elif(options.RCUMODE[w] == 8):
                Nyquist_Offset = 0.0E6
            elif(options.RCUMODE[w] == 9):
                Nyquist_Offset = 0.0E6
            elif(options.RCUMODE[w] == -1):
                Nyquist_Offset = -1E15
            else:
                logging.error("RCUMODE and CLOCK do not match")
                raise RuntimeError("bad RCUMODE and CLOCK")
        else:
            logging.error("RCUMODE and CLOCK do not match")
            raise RuntimeError("bad RCUMODE and CLOCK")
        beamlet_string=""
        subband_string = ""
        THIS_NUM_BEAMLETS = len(options.physical_beamlet_array[w])
        for i in range(THIS_NUM_BEAMLETS):
            beamlet_string += "%d,"%(options.physical_beamlet_array[w][i])
            subband_string += "%d,"%(options.subband_array[w][i])
        beamlet_string = '['+beamlet_string[:-1]+']'
        subband_string = '['+subband_string[:-1]+']'
        command.append(str(options.writer_type[w]))            #  0 writer type                        
        command.append("%s.%2.2X"%(options.filename_base[w],w))#  1 base filename                      
        command.append(str(options.RCUMODE[w]))                #  2 RCUMODE
        command.append("%.16E"%Nyquist_Offset)                 #  3 Nyquist offset (LO) offset in Hz   
        command.append(str(THIS_NUM_BEAMLETS))                 #  4 number beamlets in this writer     
        command.append(str(beamlet_string))                    #  5 physical beamlet array             
        command.append(str(subband_string))                    #  6 subband array                      
        command.append(str(options.data_type_process[w]))      #  7 processing data type               
        command.append(str(options.data_type_out[w]))          #  8 output data type                   
        command.append(str(options.channelizer_type[w]))       #  9 channelizer_type                   
        command.append(str(options.num_output_channels[w]))    # 10 number of output channels per sb   
        command.append("%.16E"%options.channelizer_shift[w])   # 11 how far relative to a standard     
                                                               #    FFT channelizer shift in samples   
                                                               #    does the channelizer shift each    
                                                               #    channelization                     
        command.append("%.16E"%options.integration_time[w])    # 12 integration time, in s             
        command.append(str(options.scale_by_inverse_samples[w]))#13 bool for scaling by samples        
        command.append("%.16E"%options.extra_scale_factor[w])  # 14 extra scaling factor               
        command.append(str(options.bounds_check_output[w]))    # 15 bool for bounds checking           
    logging.info("Running command '%s'\n",command)
    if(not options.echo_only):
        try:
            retcode = subprocess.call(command)
            logging.info("command returned %d", retcode)
            if(retcode != 0):
                logging.error("command had an error %d", retcode)
                raise RuntimeError("command failed")
        except OSError, e:
            logging.error("exception found trying to run command")
            raise e
    logging.info("command finished ok")









if __name__ == "__main__":
    main()
