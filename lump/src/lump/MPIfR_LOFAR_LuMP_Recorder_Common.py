#!/usr/bin/env python
# MPIfR_LOFAR_LuMP_Recorder_Common.py
# comon stuff for basic recorder soption parsing
#_HIST  DATE NAME PLACE INFO
# 2012 Apr 20  James M Anderson  --MPIfR  start
# 2013 Jun 19  JMA  ---- changes preparing for LuMP v2.0
# 2013 Jul 14  JMA  ---- changes for RA/Dec/Epoch/Source names to be passed in
# 2013 Jul 24  JMA  ---- more changes for LuMP v2.0
# 2013 Aug 16  JMA  ---- allow LuMP1 to be both single and multi-threaded
# 2013 Aug 30  JMA  ---- add LuMP2 format
# 2013 Sep 21  JMA  ---- add option to pre-pad to start time if there are
#                        missing packets at the start
# 2014 May 04  JMA  ---- add function convert_FILE_port_to_absolute_path
# 2014 Sep 01  JMA  ---- add FFT1, PFB0, P_FFT1, P_PFB1 types
# 2014 Sep 05  JMA  ---- add FFT2 type



# Copyright (c) 2012, 2013, 2014 James M. Anderson <anderson@gfz-potsdam.de>

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
import datetime
import math
import sys
import argparse
import logging
import subprocess
import MPIfR_LOFAR_LuMP
#import LuMP_Computer_Information
import calendar
import errno




################################################################################
# GLOBALS
################################################################################


FULL_C_RECORDER_PROGRAM_CALL = "LOFAR_Station_Beamformed_Recorder"
FULL_PYTHON_RECORDER_PROGRAM_CALL = FULL_C_RECORDER_PROGRAM_CALL+".py"
FULL_RECORDER_PROGRAM_DEFAULT_LOGFILE = FULL_C_RECORDER_PROGRAM_CALL+".log"




################################################################################
def convert_station_name(name):
    """convert from a short to the long name"""
    if((name == "Ef") or (name == "DE601") or (name == "EfDE601")):
        return "EfDE601"
    elif((name == "Uw") or (name == "DE602") or (name == "UwDE602")):
        return "UwDE602"
    elif((name == "Tb") or (name == "DE603") or (name == "TbDE603")):
        return "TbDE603"
    elif((name == "Po") or (name == "DE604") or (name == "PoDE604")):
        return "PoDE604"
    elif((name == "Ju") or (name == "DE605") or (name == "JuDE605")):
        return "JuDE605"
    elif((name == "Nc") or (name == "FR606") or (name == "NcFR606")):
        return "NcFR606"
    elif((name == "On") or (name == "SE607") or (name == "OnSE607")):
        return "OnSE607"
    elif((name == "Cb") or (name == "UK608") or (name == "CbUK608")):
        return "CbUK608"
    elif((name == "Kj") or (name == "FI609") or (name == "KjFI609")):
        return "KjFI609"
    elif((name == "Nd") or (name == "DE609") or (name == "NdDE609")):
        return "NdDE609"
    return "Unknown"





################################################################################
class LuMP_Colored_Formatter(logging.Formatter):
    """see http://stackoverflow.com/questions/384076/how-can-i-color-python-logging-output"""
    def __init__(self, msg):
        color_msg = "\033[%(levelcolor)sm" + msg + "\033[00m"
        logging.Formatter.__init__(self, color_msg)

    def format(self, record):
        levelno = record.levelno
        if(levelno >= 50):
            color_str = "7;31" # CRITICAL: red inverse
        elif(levelno >= 40):
            color_str = "7;31" # ERROR:    red inverse
        elif(levelno >= 30):
            color_str = "1;31" # WARNING:  red bold
        elif(levelno >= 20):
            color_str = "00"   # INFO:     normal
        elif(levelno >= 10):
            color_str = "1;34" # DEBUG:    blue bold
        else:
            color_str = "7;34m"# NOTSET:   blue inverse
        record.levelcolor = color_str
        return logging.Formatter.format(self, record)
        
    
    


################################################################################
def setup_logging(program_name,options):
    filename_ = program_name + ".log"
    warnfile = program_name + ".warn.log"
    full_format = "%(asctime)s:%(levelname)-8s:%(filename)s:%(funcName)s:%(lineno)d: %(message)s"
    short_format = "%(levelname)-8s: %(message)s"
    console_level = logging.INFO
    if(options.verbose):
        console_level = logging.NOTSET
    # remove all existing handlers
    for handler in logging.root.handlers:
        logging.root.removeHandler(handler)
    logging.root.setLevel(logging.NOTSET)
    # logging.basicConfig(level=logging.NOTSET,
    #                     format="%(asctime)s:%(levelname)-8s(%(levelno)02d):%(filename)s:%(funcName)s:%(lineno)d: %(message)s",
    #                     filename=filename_,
    #                     filemode='a')
    # define a Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(console_level)
    # set a format which is simpler for console use
    ###formatter = logging.Formatter(short_format)
    color_formatter = LuMP_Colored_Formatter(short_format)
    # tell the handler to use this format
    ###console.setFormatter(formatter)
    console.setFormatter(color_formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(console)
    # Add a file handler to capture all messages
    a = logging.FileHandler(filename_,mode='a',delay=False)
    a.setLevel(logging.NOTSET)
    aformatter = logging.Formatter(full_format)
    a.setFormatter(aformatter)
    logging.getLogger('').addHandler(a)
    # Define a Handler to keep track of warnings and errors to print at the
    # end of the run
##     repeater = logging.StreamHandler(GMVA_Warning_Logger())
##     repeater.setLevel(logging.WARNING)
##     rformatter = logging.Formatter("GMVA:%(levelname)-8s:%(asctime)s:%(filename)s:%(funcName)s:%(lineno)d: %(message)s")
##     repeater.setFormatter(rformatter)
##     logging.getLogger('').addHandler(repeater)
    w = logging.FileHandler(warnfile,mode='a',delay=False)
    w.setLevel(logging.WARNING)
    wformatter = logging.Formatter(full_format)
    w.setFormatter(wformatter)
    logging.getLogger('').addHandler(w)
    return


    

############################################################################
def read_arguments_from_stdin():
    argv = []
    while(True):
        l = sys.stdin.readline()
        if(l[0] == chr(23)):
            # done
            break
        if(l[-1] == '\n'):
            l = l[:-1]
        argv.append(l)
    return argv



################################################################################
def make_default_start_date():
    """make a time 1 and a bit seconds in the future"""
    t = math.ceil(time.time()) + 2
    tt = time.gmtime(t)
    s = "%04d-%02d-%02dT%02d:%02d:%02dZ"%tt[0:6]
    return s




################################################################################
def convert_FILE_port_to_absolute_path(options):
    """convert the FILE form of the port parameter to an absolute path

    port should normally be a number, indicating the port number to listen to.
    But, when this is actually a filename, convert a relative path to an absolute path, in case the working directory gets changed
    """
    # check for known FILE leads
    path_offset = -1
    if((options.port.startswith("file:")) or (options.port.startswith("FILE:"))):
        path_offset = 5
    elif((options.port.startswith("file_ns:")) or (options.port.startswith("FILE_NS:"))):
        path_offset = 8
    if(path_offset < 0):
        return options
    new_path = options.port[0:path_offset] + os.path.abspath(options.port[path_offset:])
    logging.info("old port FILE path was '%s', now using '%s'", options.port, new_path)
    options.port = new_path
    return options
    
          
          
        
    


################################################################################
def change_to_data_directory(path):
    """chdir so that the current working directory is path"""
    try:
        os.chdir(path)
    except OSError as e:
        sys.stderr.write("could not chdir into directory '%s' with errno=%d in call to make_and_change_to_data_directory"%(path,e.errno))
        raise
    return

################################################################################
def make_and_change_to_data_directory(path):
    """make a working directory for writing the data, and chdir into it

Note that no logging commands are issue here, because it is assumed that
there is no logging active until after the data directory is created and
we are inside of it.  So only writing to stderr is performed.

From stackexchange.com:
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise
"""
    if(len(path) == 0):
        sys.stderr.write("path has zero length in call to make_and_change_to_data_directory")
        raise RuntimeError("bad path")
    if(path[-1] == os.sep):
        # strip trailing '/'
        path = path[:-1]
    if(path == '.'):
        # write to current directory --- do nothing
        return
    try:
        os.makedirs(path)
    except OSError as e:
        if e.errno == errno.EEXIST and os.path.isdir(path):
            #sys.stderr.write("LuMP data directory '%s' already exists in call to make_and_change_to_data_directory"%(path))
            pass
        else:
            sys.stderr.write("could not create new directory '%s' with errno=%d in call to make_and_change_to_data_directory"%(path,e.errno))
            raise
    return change_to_data_directory(path)







################################################################################
def get_enum_match(message,s):
    ss = s
    if(type(ss)==type(1)):
        ss = "%d"%ss
    lines = message.split('\n')
    for l in lines:
        pos = l.find('=')
        if(pos < 0):
            continue
        val = l[0:pos].strip()
        enum = l[pos+1:].strip()
        pos = enum.find(' ')
        if(pos > 0):
            enum = enum[:pos]
        if((ss==val) or (ss==enum)):
            return int(val)
    logging.error("Cannot find '%s' in description '%s'", ss, message)
    return None

################################################################################
def get_enum_match_name(message,s):
    ss = s
    if(type(ss)==type(1)):
        ss = "%d"%ss
    lines = message.split('\n')
    for l in lines:
        pos = l.find('=')
        if(pos < 0):
            continue
        val = l[0:pos].strip()
        enum = l[pos+1:].strip()
        pos = enum.find(' ')
        if(pos > 0):
            enum = enum[:pos]
        if((ss==val) or (ss==enum)):
            return enum
    logging.error("Cannot find '%s' in description '%s'", ss, message)
    return None

################################################################################
def get_enum_atat_info(message,s):
    ss = s
    if(type(ss)==type(1)):
        ss = "%d"%ss
    lines = message.split('\n')
    for l in lines:
        pos = l.find('=')
        if(pos < 0):
            continue
        val = l[0:pos].strip()
        enum = l[pos+1:].strip()
        pos = enum.find(' ')
        if(pos > 0):
            enum = enum[:pos]
        if((ss==val) or (ss==enum)):
            #match found, now find the @@ marker
            pos = l.find("@@")
            if(pos < 0):
                logging.error("Cannot find @@ marker in '%s'", l)
                return None
            val = l[pos+2:].split()[0]
            val = val.replace(',', '')
            return val
    logging.error("Cannot find '%s' in description '%s'", ss, message)
    return None




WRITER_OPTIONS_MESSAGE = """1=RAW (raw voltages, separate files for each beamlet, for ALL beamlets from the RSP board, for each polarization) @@single_thread,

2=RAW0 (raw voltages, one data file with ALL beamlets from the RSP board, all polarizations) @@single_thread,

3=RAW1 (raw voltages, separate files for each beamlet, for selected beamlets from the RSP boardfor each polarization) @@multi_thread,

5=POWER0 (power measuremnts integrated over time, one data file containing the selected beamlets and full polarization information) @@multi_thread,

6=LuMP0 (raw voltage data for selected beamlets in the LuMP output format, full polarization information) @@multi_thread,

7=FFT0 (channelized voltage data using an FFT, single data file for selected beamlets, full polarization) @@multi_thread,

8=PFB0 (channelized voltage data using a polyphase filterbank, single data file for selected beamlets, full polarization) @@multi_thread,

9=POWER_FFT0 (channelized power measurements integrated over time using an FFT, single data file for selected beamlets, full polarization) @@multi_thread,

10=POWER_PFB0 (channelized power measurements integrated over time using a polyphase filterbank, single data file for selected beamlets, full polarization) @@multi_thread,

11=LuMP1  (raw voltage data for selected beamlets in the LuMP output format, full polarization information, single output file for all selected beamlets) @@single_thread,

11=LuMP1-multi  (raw voltage data for selected beamlets in the LuMP output format, full polarization information, multiple output files for selected beamlets) @@multi_thread,

12=Packet0  (writes raw LOFAR beamformed data packets to disk) @@single_thread,

13=VDIF0  (raw voltage data in the VDIF 2 format, a single data file is written for all selected beamlets, with different threads for different beamlets) @@single_thread,

14=LuMP2  (raw voltage data for selected beamlets in the LuMP output format, full polarization information, writes to a child process such as dspsr for output) @@single_thread,

15=FFT1 (channelized voltage data using an FFT, single data file for selected beamlets, full polarization, valid info scaled) @@multi_thread,

16=PFB1 (channelized voltage data using a polyphase filterbank, single data file for selected beamlets, full polarization, valid info scaled) @@multi_thread,

17=POWER_FFT1 (channelized power measurements integrated over time using an FFT, single data file for selected beamlets, full polarization, valid info scaled) @@multi_thread,

18=POWER_PFB1 (channelized power measurements integrated over time using a polyphase filterbank, single data file for selected beamlets, full polarization, valid info scaled) @@multi_thread,

19=FFT2 (channelized voltage data using an FFT, single data file for selected beamlets, full polarization, valid info scaled, valid transform written) @@multi_thread,
"""

################################################################################
def get_writer_type(s):
    r = get_enum_match(WRITER_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported writer type '%s'", s)
        raise RuntimeError("Unsupported writer type")
    return r
################################################################################
def get_writer_type_string(s):
    r = get_enum_match_name(WRITER_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported writer type '%s'", s)
        raise RuntimeError("Unsupported writer type")
    return r
################################################################################
def get_writer_type_atat(s):
    r = get_enum_atat_info(WRITER_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported writer type '%s'", s)
        raise RuntimeError("Unsupported writer type")
    return r



DATA_TYPE_OPTIONS_MESSAGE = """
 7=L_int8_t          (8 bit integer),
10=L_int16_t         (16 bit integer),
11=L_int32_t         (32 bit integer),
12=L_int64_t         (64 bit integer),
14=L_Real16_t        (half precision floating point),
15=L_Real32_t        (single precision floating point),
16=L_Real64_t        (double precision floating point),
17=L_Real80_t        (80 bit extended precision precision floating point),
18=L_Real128_t       (quad precision floating point),
19=L_Complex32_t     (half precision complex floating point, two L_Real16_t values),
20=L_Complex64_t     (single precision complex floating point, two L_Real32_t values),
21=L_Complex128_t    (double precision complex floating point, two L_Real64_t values),
22=L_Complex160_t    (extended precision complex floating point, two L_Real80_t values),
23=L_Complex256_t    (quad precision complex floating point, two L_Real128_t values),
24=L_intComplex8_t   (complex integer, two L_int4_t values,  LOFAR 4 bit mode),
25=L_intComplex16_t  (complex integer, two L_int8_t values,  LOFAR 8 bit mode),
26=L_intComplex32_t  (complex integer, two L_int16_t values, LOFAR 16 bit mode),
27=L_intComplex64_t  (complex integer, two L_int32_t values),
27=L_intComplex128_t (complex integer, two L_int64_t values)
"""
################################################################################
def get_data_type(s):
    r = get_enum_match(DATA_TYPE_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported data type '%s'", s)
        raise RuntimeError("Unsupported data type")
    return r
################################################################################
def get_data_type_string(s):
    r = get_enum_match_name(DATA_TYPE_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported data type '%s'", s)
        raise RuntimeError("Unsupported data type")
    return r


WINDOW_FUNCTION_OPTIONS_MESSAGE = """
 0=Rectangular
 1=Hann
 2=Hamming
 3=Tukey
 4=Cosine
 5=Lanczos
 6=Barlett0
 7=BarlettN0
 8=Gaussian
 9=Bartlett_Hann
10=Blackman
11=Kaiser
12=Nuttall
13=Blackman_Harris
14=Blackman_Nuttall
15=Flat_Top
"""
################################################################################
def get_window_type(s):
    r = get_enum_match(WINDOW_FUNCTION_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported window function type '%s'", s)
        raise RuntimeError("Unsupported window function type")
    return r
################################################################################
def get_window_type_string(s):
    r = get_enum_match_name(WINDOW_FUNCTION_OPTIONS_MESSAGE,s)
    if(r is None):
        logging.critical("Unsupported window function type '%s'", s)
        raise RuntimeError("Unsupported window function type")
    return r




################################################################################
def get_default_recorder_groups_message():
    msg = "*REQUIRED*  The recorder group to use.  Available default options are: "
    for g in LuMP_Computer_Information.LuMP_Recorder_Group_Dict.keys():
        msg += "%s, "%g
    msg = msg[:-2] + ". Additional recording groups may be available if provided through the --add_recorder_group option."
    return msg




################################################################################
def get_cpuinfo():
    NUM_CPU=0
    CACHE_SIZE=0
    SIBLINGS=None
    CPU_CORES=None
    fp=open("/proc/cpuinfo","r")
    for line in fp:
        if(line.startswith("processor\t:")):
            num=int(line[len("processor\t:"):])
            if(num>=NUM_CPU):
                NUM_CPU=num+1
        elif(line.startswith("cache size\t:")):
            vals=line[len("cache size\t:"):].split()
            size=int(vals[0])
            if(len(vals)>1):
                if((vals[1].strip() == "KB") or (vals[1].strip() == "kB")):
                    size *= 1024
                elif(vals[1].strip() == "MB"):
                    size *= 1024*1024
            if(CACHE_SIZE==0):
                CACHE_SIZE=size
            elif(size < CACHE_SIZE):
                CACHE_SIZE = size
        elif(line.startswith("siblings\t:")):
            SIBLINGS=int(line[len("siblings\t:"):])
        elif(line.startswith("cpu cores\t:")):
            CPU_CORES=int(line[len("cpu cores\t:"):])
    logging.debug("Found %d cores with %d B cache on this machine", NUM_CPU, CACHE_SIZE)
    if((SIBLINGS is not None)and(CPU_CORES is not None)):
        # Intel Hyperthreading is BS
        ratio = SIBLINGS/CPU_CORES
        NUM_CPU /= ratio
        logging.debug("Foung Intel Hyperthreading active with %d pseudo cores per real core.  Have %d real cores", ratio, NUM_CPU)
    return NUM_CPU, CACHE_SIZE



################################################################################
def get_raminfo():
    RAM_SIZE = 0.0
    fp=open("/proc/meminfo","r")
    for line in fp:
        if(line.startswith("MemTotal:")):
            vals=line[len("MemTotal:"):].split()
            size=float(vals[0])
            if(len(vals)>1):
                if((vals[1].strip() == "KB") or (vals[1].strip() == "kB")):
                    size *= 1024
                elif(vals[1].strip() == "MB"):
                    size *= 1024*1024
                elif(vals[1].strip() == "GB"):
                    size *= 1024*1024*1024
            RAM_SIZE = size
            break
    prefix = 0
    size_report = RAM_SIZE
    while(size_report >= 1024.0):
        prefix += 1
        size_report /= 1024.0
    prefix_letter = " kMGTPEZY"[prefix]
    logging.debug("Found %.3G %cB (%d B) of RAM", size_report, prefix_letter, int(RAM_SIZE))
    return RAM_SIZE
    


    

################################################################################
def get_int_array(sarray,MAX_ELEMENTS, BOTTOM_VALUE, TOP_VALUE):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    MAX_NEST = 10
    if(len(sarray) < 1):
        logging.error("Malformed array argument --- not big enough")
        raise RuntimeError("bad array")
    array = sarray
    array = array.replace(' ','').strip().replace(',',' ').replace('[',' [ ').replace(']',' ] ').replace('*',' * ').split()
    outarray = [[]]*MAX_NEST
    bracket_level = 0
    bracket_ready = False
    bracket_multiplier_found = False
    for a in array:
        #print("processing element '%s'"%a)
        #print outarray
        if(a == '*'):
            if(not bracket_ready):
                logging.error("array multiplier found but bracket not finished by ']' symbol")
                raise RuntimeError("bad array")
            elif(bracket_multiplier_found):
                logging.error("double array multiplier symbol found")
                raise RuntimeError("bad array")
            bracket_multiplier_found = True
        elif(a == '['):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            bracket_level += 1
            if(bracket_level >= MAX_NEST):
                logging.error("nested bracket level exceeds max allowed nest level of %d", MAX_NEST)
                raise RuntimeError("bad array")
            outarray[bracket_level] = []
        elif(a == ']'):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_level -= 1
            bracket_ready = True
            if(bracket_level < 0):
                logging.error("mismatched brackets in array, ']' found with no matching '['")
                raise RuntimeError("bad array")
        elif(a.find(':')>0):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            aa = map(int,a.replace(':',' ').split())
            if(len(aa) != 2):
                logging.error("Malformed array argument --- range entry '%s' does not have 2 entries.")
                raise RuntimeError("bad array")
            if((aa[0] < BOTTOM_VALUE) or (aa[0] > TOP_VALUE)
               or(aa[1] < BOTTOM_VALUE) or (aa[1] > TOP_VALUE)):
                logging.error("Specified array range '%s' exceeds LOFAR range", a)
                raise RuntimeError("bad array")
            aa = range(aa[0],aa[1])
            outarray[bracket_level] = outarray[bracket_level] + aa
        else:
            if(bracket_ready and not bracket_multiplier_found):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            aa = int(a)
            if(bracket_multiplier_found):
                if(aa <= 0):
                    logging.error("bracket multiplier (%d) is non-positive", aa)
                    raise RuntimeError("bad array")
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]*aa
                bracket_multiplier_found = False
                bracket_ready = False
            else:
                if((aa < BOTTOM_VALUE) or (aa >= TOP_VALUE)):
                    logging.error("Specified array value '%d' exceeds LOFAR range %d <= value < %d", aa, BOTTOM_VALUE, TOP_VALUE)
                    raise RuntimeError("bad array")
                outarray[bracket_level].append(aa)
    else:
        if(bracket_multiplier_found):
            logging.error("array multiplier symbol requires integer multiplier")
            raise RuntimeError("bad array")
        elif(bracket_ready):
            outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_ready = False
        if(bracket_level != 0):
            logging.error("no closing ']' for bracket array")
            raise RuntimeError("bad array")
    if(len(outarray[0]) > MAX_ELEMENTS):
        logging.error("Num array elements given is more than the max total number of elements")
        raise RuntimeError("bad array")
    return outarray[0]


################################################################################
def get_double_array(sarray,MAX_ELEMENTS, BOTTOM_VALUE, TOP_VALUE):
    #logging.debug("get_double_array input '%s'", sarray)
    MAX_NEST = 10
    if(len(sarray) < 1):
        logging.error("Malformed array argument --- not big enough")
        raise RuntimeError("bad array")
    array = sarray
    array = array.replace(' ','').strip().replace(',',' ').replace('[',' [ ').replace(']',' ] ').replace('*',' * ').split()
    #logging.debug("get_double_array split input '%s'", array)
    outarray = [[]]*MAX_NEST
    bracket_level = 0
    bracket_ready = False
    bracket_multiplier_found = False
    for a in array:
        #logging.debug("processing element '%s'",a)
        #logging.debug("%s",outarray)
        if(a == '*'):
            if(not bracket_ready):
                logging.error("array multiplier found but bracket not finished by ']' symbol")
                raise RuntimeError("bad array")
            elif(bracket_multiplier_found):
                logging.error("double array multiplier symbol found")
                raise RuntimeError("bad array")
            bracket_multiplier_found = True
        elif(a == '['):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            bracket_level += 1
            if(bracket_level >= MAX_NEST):
                logging.error("nested bracket level exceeds max allowed nest level of %d", MAX_NEST)
                raise RuntimeError("bad array")
            outarray[bracket_level] = []
        elif(a == ']'):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_level -= 1
            bracket_ready = True
            if(bracket_level < 0):
                logging.error("mismatched brackets in array, ']' found with no matching '['")
                raise RuntimeError("bad array")
        else:
            if(bracket_ready and not bracket_multiplier_found):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            if(bracket_multiplier_found):
                aa = int(a)
                if(aa <= 0):
                    logging.error("bracket multiplier (%d) is non-positive", aa)
                    raise RuntimeError("bad array")
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]*aa
                bracket_multiplier_found = False
                bracket_ready = False
            else:
                aa = float(a)
                if((aa < BOTTOM_VALUE) or (aa >= TOP_VALUE)):
                    logging.error("Specified array value '%f' exceeds LOFAR range %f <= value < %f", aa, BOTTOM_VALUE, TOP_VALUE)
                    raise RuntimeError("bad array")
                outarray[bracket_level].append(aa)
    else:
        if(bracket_multiplier_found):
            logging.error("array multiplier symbol requires integer multiplier")
            raise RuntimeError("bad array")
        elif(bracket_ready):
            outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_ready = False
        if(bracket_level != 0):
            logging.error("no closing ']' for bracket array")
            raise RuntimeError("bad array")
    if(len(outarray[0]) > MAX_ELEMENTS):
        logging.error("Num array elements given is more than the max total number of elements")
        raise RuntimeError("bad array")
    return outarray[0]


################################################################################
def get_string_array(sarray,MAX_ELEMENTS):
    MAX_NEST = 10
    if(len(sarray) < 1):
        logging.error("Malformed array argument --- not big enough")
        raise RuntimeError("bad array")
    array = sarray
    array = array.strip().replace(',','\0').replace('[','\0[\0').replace(']','\0]\0').replace('*','\0*\0').split('\0')
    #logging.debug("Epoch parse array is '%s'", array)
    outarray = [[]]*MAX_NEST
    bracket_level = 0
    bracket_ready = False
    bracket_multiplier_found = False
    for a in array:
        #print("processing element '%s'"%a)
        #print outarray
        if(len(a) == 0):
            continue
        elif(a == '*'):
            if(not bracket_ready):
                logging.error("array multiplier found but bracket not finished by ']' symbol")
                raise RuntimeError("bad array")
            elif(bracket_multiplier_found):
                logging.error("double array multiplier symbol found")
                raise RuntimeError("bad array")
            bracket_multiplier_found = True
        elif(a == '['):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            bracket_level += 1
            if(bracket_level >= MAX_NEST):
                logging.error("nested bracket level exceeds max allowed nest level of %d", MAX_NEST)
                raise RuntimeError("bad array")
            outarray[bracket_level] = []
        elif(a == ']'):
            if(bracket_multiplier_found):
                logging.error("array multiplier symbol requires integer multiplier")
                raise RuntimeError("bad array")
            elif(bracket_ready):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_level -= 1
            bracket_ready = True
            if(bracket_level < 0):
                logging.error("mismatched brackets in array, ']' found with no matching '['")
                raise RuntimeError("bad array")
        else:
            if(bracket_ready and not bracket_multiplier_found):
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
                bracket_ready = False
            if(bracket_multiplier_found):
                aa = int(a)
                if(aa <= 0):
                    logging.error("bracket multiplier (%d) is non-positive", aa)
                    raise RuntimeError("bad array")
                outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]*aa
                bracket_multiplier_found = False
                bracket_ready = False
            else:
                aa = a.strip()
                # remove string quotations, if found
                if(len(aa)>1):
                    if( ((aa[0] == "'") and (aa[-1] == "'"))
                        or ((aa[0] == '"') and (aa[-1] == '"')) ):
                        aa = aa[1:-1]
                outarray[bracket_level].append(aa)
    else:
        if(bracket_multiplier_found):
            logging.error("array multiplier symbol requires integer multiplier")
            raise RuntimeError("bad array")
        elif(bracket_ready):
            outarray[bracket_level] = outarray[bracket_level] + outarray[bracket_level+1]
            bracket_ready = False
        if(bracket_level != 0):
            logging.error("no closing ']' for bracket array")
            raise RuntimeError("bad array")
    if(len(outarray[0]) > MAX_ELEMENTS):
        logging.error("Num array elements given is more than the max total number of elements")
        raise RuntimeError("bad array")
    return outarray[0]


################################################################################
def get_int_subbands(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    BOTTOM_SUBBAND = 0
    TOP_SUBBAND = MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_SUBBANDS
    try:
        outarray = get_int_array(sarray,NUM_BEAMLETS,BOTTOM_SUBBAND,TOP_SUBBAND)
    except:
        logging.error("bad subband array '%s'", sarray)
        raise RuntimeError("bad subbands")
    return outarray

################################################################################
def get_int_beamlets(sarray, NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    BOTTOM_BEAMLET = 0
    TOP_BEAMLET = NUM_BEAMLETS
    try:
        outarray = get_int_array(sarray,NUM_BEAMLETS,BOTTOM_BEAMLET,TOP_BEAMLET)
    except:
        logging.error("bad beamlets array '%s'", sarray)
        raise RuntimeError("bad beamlets")
    return outarray

################################################################################
def get_int_rcumodes(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    BOTTOM_RCUMODE = 1
    TOP_RCUMODE = 10
    try:
        outarray = get_int_array(sarray,NUM_BEAMLETS,BOTTOM_RCUMODE,TOP_RCUMODE)
    except:
        logging.error("bad rcumodes array '%s'", sarray)
        raise RuntimeError("bad rcumodes")
    return outarray

################################################################################
def get_double_rightascensions(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    # Although RA runs from 0 to 2\pi radians, other coordinate systems
    # (different Epoch values) may have -pi to +pi
    BOTTOM_RA = -math.pi
    TOP_RA = 2.0*math.pi
    try:
        outarray = get_double_array(sarray,NUM_BEAMLETS,BOTTOM_RA,TOP_RA)
    except:
        logging.error("bad rightascensions array '%s'", sarray)
        raise RuntimeError("bad rightascensions")
    return outarray

################################################################################
def get_double_declinations(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    BOTTOM_DEC = -math.pi/2.0
    TOP_DEC = math.pi/2.0*(1.0+sys.float_info.epsilon)
    try:
        outarray = get_double_array(sarray,NUM_BEAMLETS,BOTTOM_DEC,TOP_DEC)
    except:
        logging.error("bad declinations array '%s'", sarray)
        raise RuntimeError("bad declinations")
    return outarray

################################################################################
def get_string_epochs(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    try:
        outarray = get_string_array(sarray,NUM_BEAMLETS)
    except:
        logging.error("bad epochs array '%s'", sarray)
        raise RuntimeError("bad epochs")
    return outarray

################################################################################
def get_string_sourcenames(sarray,NUM_BEAMLETS):
    # NUM_BEAMLETS is the number of beamlets per station, not per lane
    try:
        outarray = get_string_array(sarray,NUM_BEAMLETS)
    except:
        logging.error("bad sourcenames array '%s'", sarray)
        raise RuntimeError("bad sourcenames")
    return outarray

################################################################################
def check_LOFAR_epochs(epoch_array):
    LOFAR_EPOCHS = ["J2000", "HADEC", "AZELGEO", "SUN", "MERCURY", "VENUS",
                    "MOON", "MARS", "JUPITER", "SATURN", "URANUS", "NEPTUNE",
                    "PLUTO"]
    bad_epochs = []
    for epoch in epoch_array:
        if(epoch not in LOFAR_EPOCHS):
            if(epoch not in bad_epochs):
                bad_epochs.append(epoch)
                logging.warning("epoch '%s' is not known.  If you are sure this is a valid LOFAR epoch, please contact the developer", epoch)
    return

################################################################################
def check_LOFAR_directions(rightascension_array, declination_array, epoch_array):
    if(len(rightascension_array) != len(declination_array)):
        logging.error("size of rightascension_array (%d) is not the same as the size of the declination_array (%d)", len(rightascension_array), len(declination_array))
        raise RuntimeError("array size mismatch")
    if(len(rightascension_array) != len(epoch_array)):
        logging.error("size of rightascension_array (%d) is not the same as the size of the epoch_array (%d)", len(rightascension_array), len(epoch_array))
        raise RuntimeError("array size mismatch")
    for i in range(len(rightascension_array)):
        epoch = epoch_array[i];
        ra = rightascension_array[i]
        dec = declination_array[i]
        if(epoch == "J2000"):
            if((ra < 0.0) or (ra >= 2.0*math.pi)):
                logging.warning("right ascension value %E out of range for epoch %s", ra, epoch)
            if((dec < -math.pi/2.0) or (dec > math.pi/2.0)):
                logging.warning("declination value %E out of range for epoch %s", ra, epoch)
        elif(epoch == "HADEC"):
            if((ra < -math.pi) or (ra > math.pi)):
                logging.warning("right ascension value %E out of range for epoch %s", ra, epoch)
            if((dec < math.pi/2.0) or (dec > math.pi/2.0)):
                logging.warning("declination value %E out of range for epoch %s", ra, epoch)
        elif(epoch == "AZELGEO"):
            if((ra < -math.pi) or (ra > 2.0*math.pi)):
                logging.warning("right ascension value %E out of range for epoch %s", ra, epoch)
            if((dec < 0.0) or (dec > math.pi/2.0)):
                logging.warning("declination value %E out of range for epoch %s", ra, epoch)
    return
    
################################################################################
def check_beamlet_values(BEAMLETS_PER_LANE, beamlet_array, subband_array, rcumode_array):
    NUM_TOTAL_BEAMLETS = BEAMLETS_PER_LANE * MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES
    if(len(beamlet_array) != len(subband_array)):
        logging.error("size of beamlet_array (%d) is not the same as the size of the subband_array (%d)", len(beamlet_array), len(subband_array))
        raise RuntimeError("array size mismatch")
    if(len(beamlet_array) != len(rcumode_array)):
        logging.error("size of beamlet_array (%d) is not the same as the size of the rcumode_array (%d)", len(beamlet_array), len(rcumode_array))
        raise RuntimeError("array size mismatch")
    for i in range(len(beamlet_array)):
        beamlet = beamlet_array[i]
        subband = subband_array[i]
        rcumode = rcumode_array[i]
        if((beamlet < 0) or (beamlet >= NUM_TOTAL_BEAMLETS)):
            logging.error("beamlet value %d is out of expected range 0--%d.  If the beamlet value is correct, contact your developer", beamlet, NUM_TOTAL_BEAMLETS)
            raise RuntimeError("beamlet out of range")
        if((subband < 0) or (subband > MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_SUBBANDS)):
            logging.error("subband value %d is out of expected range 0--%d.  If the subband value is correct, contact your developer", subband, MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_SUBBANDS)
            raise RuntimeError("subband out of range")
        if((rcumode < 1) or (rcumode > 9)):
            logging.error("rcumode value %d is out of expected range 1--9.  If the rcumode value is correct, contact your developer", rcumode)
            raise RuntimeError("rcumode out of range")
    return
    


################################################################################
def convert_ISO_date_string(s):
    """ convert YYYY-MM-DDTHH:MM:SSZ string to a time_t value for UTC
YYYY-MM-DDTHH:MM:SSZ

Allows a special case of a hexidecimal number (0X???) to be directly specified.

    """
    if(len(s) > 2):
        if((s[0:2] == "0x") or (s[0:2] == "0X")):
            t = int(s,16)
            return t
    if((len(s) != 20) or (s[4] != '-') or (s[7] != '-') or (s[10] != 'T')
       or (s[13] != ':') or (s[16] != ':') or (s[19] != 'Z')) :
        logging.error("Bad ISO date format.  Need 'YYYY-MM-DDTHH:MM:SSZ' but got '%s'", s)
        raise RuntimeError("Bad ISO date")
    try:
        year = int(s[0:4])
        month = int(s[5:7])
        day = int(s[8:10])
        hour = int(s[11:13])
        minute = int(s[14:16])
        second = int(s[17:19])
    except ValueError, e:
        logging.error("Unable to interpret ISO date format.  Need 'YYYY-MM-DDTHH:MM:SSZ' but got '%s'", s)
        raise e
    tt = (year, month, day, hour, minute, second, 0, 0, 0)
    t = calendar.timegm(tt)
    return t











############################################################################
def set_option_parsing_general_setup(p):
    p.add_argument("--start_date", type=str,help="*OPTIONAL*  The date and time to begin recording, as a UTC ISO date string of the format YYYY-MM-DDTHH:MM:SSZ or as a hexadecimal number representing the integer Unix timestamp in seconds since the reference epoch 1970-01-01 00:00:00 +0000 (UTC).")
    p.add_argument("--duration",type=int,help="*REQUIRED*  Duration of measurment to listen to the station, in seconds.  Note that this is the duration from the start_date, so if recording begins late, the actual recorded duration will be smaller.", required=True)
    p.add_argument("--port", type=str,help="*REQUIRED*  Port number to listen to for incoming data from a LOFAR station.  Normally, this should be a decimal number.  When reading from a file (raw UDP dump), this should be the filename, including any necessary path, of the input file, preceded by FILE:.  For example, if your filename is ./MYDIR/somedir/myfile.raw then you would specify this as --port=FILE:./MYDIR/somedir/myfile.raw in the argument list.  If the port specification starts with UDP: then UDP network data are read from the port number following the UDP: key.  TCP: specifies that the TCP protocol is to be used.  The value - specifies that the program should read from stdin.  By default, the program will use UDP access.", required=True)
    p.add_argument("--station_name", type=str,help="*REQUIRED*  Name of the LOFAR station to record data from.  This should be of the type DE601, Ef, or EfDE601", required=True)
    p.add_argument("--datadir", type=str, help="*REQUIRED*  Name of the directory of the main data recording area into which this recording will be written.  For example, suppose that the main data recording area is '/media/scratch/observer', your name is 'Astronomer', and you are observing on 2010 Dec 25.  You want all of the data to be recorded to your own specific directory area, to not be confused with other people's data, and you want to sort things by the date of observation.  Then you would set --datadir to '/media/scratch/observer/Astronomer/20101225'.  A relative path name may be specified.  The datadir '.' may also be specified.", required=True)
    p.add_argument("--data_type_in", default="26",type=str,help="*OPTIONAL*  data type of station beamforemd data.  Defaults to %(default)s.  Available options are: " + DATA_TYPE_OPTIONS_MESSAGE)
    p.add_argument("--clock_speed",default=200,type=int,choices=[200,160,0],help="Clock speed of station, in MHz.  Defaults to %(default)d")
    p.add_argument("--beamlets_per_lane",default=61,type=int,help="*OPTIONAL*  number of beamlets per RSP lane sent out by the station.  Defaults to %(default)d")
    p.add_argument("--samples_per_packet",default=16,type=int,help="*OPTIONAL*  number of samples per packet sent out by the station.  Defaults to %(default)d")
    p.add_argument("--backfill_to_start_time", action="store_true", help="*OPTIONAL*  instruct the packet sorter to backfill with blank packets to the start time if there are missing packets at the start of recording.")
    p.add_argument("--main_recorder_logfile",default=FULL_RECORDER_PROGRAM_DEFAULT_LOGFILE,type=str,help="Name of the logfile to write out for the main recorder program.  Defaults to %(default)s")
    return p




############################################################################
def set_option_parsing_recorder_group(p):
    """Sets up information for using recorder groups"""
    p.add_argument("--recorder_group",type=str,help=get_default_recorder_groups_message(), required=True)
    p.add_argument("--add_recorder_computers", action='append', help="*OPTIONAL* filename of additional LuMP recording computer information to use")
    p.add_argument("--add_stations", action='append', help="*OPTIONAL* filename of additional LuMP station information to use")
    p.add_argument("--add_recorder_group", action='append', help="*OPTIONAL* filename of additional LuMP recorder group information to use")
    return p





############################################################################
def set_option_parsing_recorder_cpu_specification(p):
    """specify information about a single recorder (RAM, num cores to use, cache size) directly"""
    p.add_argument("--recorder_num_cores", type=int, default=0, help="*OPTIONAL*  number of CPU cores to use for LuMP recording.  If 0 is specified, the software will attempt to determine the number of cores available on the recording computer and use all of them.  Also note that this only specifies the number of cores used by the LuMP recording software itself --- CPU utilization by downstream software that may be started by LuMP (dspsr, for example) must be specified in the option arguments to that software separately.  Defaults to %(default)d")
    p.add_argument("--recorder_cache_size", type=int, default=0, help="*OPTIONAL*  size of the CPU cache, in bytes.  This is the size of the full cache per CPU (typically L3 cache), as reported by the 'cache size' listing in /proc/cpuinfo.  If specified as 0, LuMP will attempt to determine this information automatically.  Defaults to %(default)d")
    p.add_argument("--recorder_ram_size", type=float, default=0.0, help="*OPTIONAL*  size of the RAM available for LuMP to use, in bytes.  If specified as 0, LuMP will attempt to determine the amount of RAM available on the computer and assume it can use all of that.  Default %(default).0f")
    return p



############################################################################
def set_option_parsing_writer_setup(default_writer,
                                    default_processing,
                                    default_output,
                                    NUM_CHAN,
                                    NUM_TAP,
                                    window_function,
                                    integration_time,
                                    p,
                                    all_required=False,
                                    all_action='store'):
    REQUIREMENT_STRING = "*OPTIONAL*  "
    if(all_required):
        REQUIREMENT_STRING = "*REQUIRED*  "
    if(all_required is False):
        d=default_writer
        if(all_action=='append'):
            d=[d]
        p.add_argument("--writer_type",action=all_action,default=d,help=REQUIREMENT_STRING+"The enum code of the writer type to use.  Defaults to %(default)s.  Available options are: "+WRITER_OPTIONS_MESSAGE, required=all_required)
    else:
        p.add_argument("--writer_type",action=all_action,help=REQUIREMENT_STRING+"The enum code of the writer type to use.  Available options are: "+WRITER_OPTIONS_MESSAGE, required=all_required)
    p.add_argument("--filename_base",action=all_action, help="*REQUIRED*  base string from which the output file names will be generated.  Note that this base filename will be extended by a 2 digit hexadecimal number indicating the writer ID number used to write out the data ( filename_base=\"%%s.%%2.2X\"%%(filename_base,ID) ).", required=True)
    p.add_argument("--physical_beamlet_array",action=all_action, help="*REQUIRED*  Python-like array of the physical beamlets to use for this writer.  A combination of individual physical beamlets and Python ranges may be used, such as '[0,1,4,7,10:31,60:62]'.  Ranges must have both start and end specified as start:end.  Note that the notation here is a Python notation for the ranges (start, start+1,start+2,...,end-1), which is different from the ASTRON LOFAR station software.", required=True)
    p.add_argument("--subband_array",action=all_action, help="*REQUIRED*  Python array of the subbands corresponding to the beamlets.  A combination of individual physical subbands and Python ranges may be used, such as '[100,101,104,107,110:131,160:162]'.  Ranges must have both start and end specified as start:end.  Note that the notation here is a Python notation for the ranges (start, start+1,start+2,...,end-1), which is different from the ASTRON LOFAR station software.  Python-style array multipliers may be used to repeat subband selections, such as when multiple pointing directions use the same observing frequencies.  For example, '[0:2]*3' is equivalent to '[0,1,0,1,0,1]'.  The PHYSICAL_BEAMLET_ARRAY and SUBBAND_ARRAY should match beamlet to subband at the same index.", required=True)
    p.add_argument("--rcumode_array",action=all_action, help="*REQUIRED*  Python array of the RCUMODEs corresponding to the beamlets.  Python-style array multipliers are allowed, simplifying the standard case where all beamlets have the same RCUMODE.  For example, '[5]*244' yields an array of RCUMODE values that is 244 elements long, all with RCUMODE==5.  Alternatively, individual RCUMODE values may be specified in the standard Python array syntax, such as '[5,5,5,5,6,6,7,7]'.  Ranges may also be specified as start:end.  Note that the notation here is a Python notation for the ranges (start, start+1,start+2,...,end-1), which is different from the ASTRON LOFAR station software.  The PHYSICAL_BEAMLET_ARRAY and RCUMODE_ARRAY should match beamlet to RCUMODE at the same index.", required=True)
    p.add_argument("--rightascension_array",action=all_action, help="*REQUIRED*  Python array of the right ascensions (or other coordinate if the Epoch is not J2000) corresponding to the beamlets.  *Note that the right ascension is to be provided in units of radians, as it is specified to the LOFAR beamctl program.*  Python-style array multipliers are allowed, simplifying the standard case where all beamlets have the same pointing direction.  For example, '[1.23456789]*244' yields an array of right ascension values that is 244 elements long, all with rightascension==1.23456789.  Alternatively, individual right ascension values may be specified in the standard Python array syntax, such as '[1,1,1,1,2,2,3,3]'.  The PHYSICAL_BEAMLET_ARRAY and RIGHTASCENSION_ARRAY should match beamlet to right ascension at the same index.", required=True)
    p.add_argument("--declination_array",action=all_action, help="*REQUIRED*  Python array of the declinations (or other coordinate if the Epoch is not J2000) corresponding to the beamlets.  *Note that the declination is to be provided in units of radians, as it is specified to the LOFAR beamctl program.*  Python-style array multipliers are allowed, simplifying the standard case where all beamlets have the same pointing direction.  For example, '[1.23456789]*244' yields an array of declination values that is 244 elements long, all with declination==1.23456789.  Alternatively, individual declination values may be specified in the standard Python array syntax, such as '[0,0,0,0,0.5,0.5,1.0,1.0]'.  The PHYSICAL_BEAMLET_ARRAY and DECLINATION_ARRAY should match beamlet to declination at the same index.", required=True)
    p.add_argument("--epoch_array",action=all_action, help="*REQUIRED*  Python array of the epochs (or other coordinate system identifiers) corresponding to the beamlets.  Python-style array multipliers are allowed, simplifying the standard case where all beamlets have the same pointing epoch.  For example, '[J2000]*244' yields an array of epoch values that is 244 elements long, all with epoch==J2000.  Note that the epoch values do not require string quotation marks.  Alternatively, individual epoch values may be specified in the standard Python array syntax, such as '[J2000,J2000,HADEC, AZELGEO, SUN,MOON]'.  The PHYSICAL_BEAMLET_ARRAY and EPOCH_ARRAY should match beamlet to epoch at the same index.", required=True)
    p.add_argument("--sourcename_array",action=all_action, help="*REQUIRED*  Python array of the source names corresponding to the beamlets.  Python-style array multipliers are allowed, simplifying the standard case where all beamlets have the same source name.  For example, '[Cas A]*244' yields an array of epoch values that is 244 elements long, all with sourcename==Cas A.  Note that the source name values do not require string quotation marks.  Alternatively, individual source name values may be specified in the standard Python array syntax, such as '[Cas A, Cas A, Cyg A, Cyg A, Hydra A]'.  Note that leading and trailing whitespace will be removed.  The PHYSICAL_BEAMLET_ARRAY and SOURCENMAE_ARRAY should match beamlet to source name at the same index.", required=True)
    if(all_required is False):
        d=default_processing
        if(all_action=='append'):
            d=[d]
        p.add_argument("--data_type_process",action=all_action,default=d,type=str,help="*OPTIONAL*  Data type for internal processing.  Defaults to %(default)s.  Available options are: " + DATA_TYPE_OPTIONS_MESSAGE, required=all_required)
        d=default_output
        if(all_action=='append'):
            d=[d]
        p.add_argument("--data_type_out",action=all_action, default=d,type=str,help=REQUIREMENT_STRING+"Data type for output to disk.  Defaults to %(default)s.  Available options are: " + DATA_TYPE_OPTIONS_MESSAGE, required=all_required)
    else:
        p.add_argument("--data_type_process",action=all_action,help="*OPTIONAL*  Data type for internal processing.  Available options are: " + DATA_TYPE_OPTIONS_MESSAGE, required=all_required)
        p.add_argument("--data_type_out",action=all_action,help=REQUIREMENT_STRING+"Data type for output to disk.  Available options are: " + DATA_TYPE_OPTIONS_MESSAGE, required=all_required)
    if(NUM_CHAN is not None):
        if(all_required is False):
            d=NUM_CHAN
            if(all_action=='append'):
                d=[d]
            p.add_argument("--num_output_channels",action=all_action, default=d,type=int,help=REQUIREMENT_STRING+"Number of output channels to make per subband.  Default is %(default)d")
        else:
            p.add_argument("--num_output_channels",action=all_action,type=int,help=REQUIREMENT_STRING+"Number of output channels to make per subband", required=all_required)
    if(NUM_TAP is not None):
        if(all_required is False):
            d=NUM_TAP
            if(all_action=='append'):
                d=[d]
            p.add_argument("--num_polyphase_filter_taps",action=all_action, default=d,type=int,help=REQUIREMENT_STRING+"Number of polyphase filter taps to use.  Default is %(default)d")
        else:
            p.add_argument("--num_polyphase_filter_taps",action=all_action,type=int,help=REQUIREMENT_STRING+"Number of polyphase filter taps to use.", required=all_required)
    if(window_function is not None):
        if(all_required is False):
            d=window_function
            if(all_action=='append'):
                d=[d]
            p.add_argument("--window_function",action=all_action,default=d,type=str,help=REQUIREMENT_STRING+"Type of window function to use.  Defaults to %(default)s.  Available options are: "+WINDOW_FUNCTION_OPTIONS_MESSAGE)
            d = 0.0
            if(all_action=='append'):
                d=[d]
            p.add_argument("--window_parameter",action=all_action, default=d,type=float,help=REQUIREMENT_STRING+"Extra parameter for specific window functions. Defaults to %(default)E.")
        else:
            p.add_argument("--window_function",action=all_action,help=REQUIREMENT_STRING+"Type of window function to use.  Available options are: "+WINDOW_FUNCTION_OPTIONS_MESSAGE, required=all_required)
            p.add_argument("--window_parameter",action=all_action,type=float,help=REQUIREMENT_STRING+"Extra parameter for specific window functions.", required=all_required)
    #p.add_argument("--channelizer_shift", default=1.0,type=float,help="*OPTIONAL*  How far, relative to a normal FFT setup (N samples for complex data to N channels, or 2N samples for real-valued data to N channels) should the channelizer shift each channelization timestep.  Usually this will be 1.0.")
    if(integration_time is not None):
        if(all_required is False):
            d=integration_time
            if(all_action=='append'):
                d=[d]
            p.add_argument("--integration_time",action=all_action,default=d,type=float,help=REQUIREMENT_STRING+"The integration time, in seconds, for averaging the total power data.  Default=%(default).2E")
            d=1
            if(all_action=='append'):
                d=[d]
            p.add_argument("--scale_by_inverse_samples",action=all_action,default=d,type=int,choices=[0,1],help=REQUIREMENT_STRING+"Should the total power values should be scaled by the number of samples per integration?  0 No, or 1 Yes.")
        else:
            p.add_argument("--integration_time",action=all_action,type=float,help=REQUIREMENT_STRING+"The integration time, in seconds, for averaging the total power data.", required=all_required)
            p.add_argument("--scale_by_inverse_samples",action=all_action,type=int,choices=[0,1],help=REQUIREMENT_STRING+"Should the total power values should be scaled by the number of samples per integration?  0 No, or 1 Yes.", required=all_required)
    if(all_required is False):
        d=1.0
        if(all_action=='append'):
            d=[d]
        p.add_argument("--extra_scale_factor",action=all_action,default=d,type=float,help=REQUIREMENT_STRING+"Extra scaling factor for total power.  Default is %(default).2f")
        d=1
        if(all_action=='append'):
            d=[d]
        p.add_argument("--bounds_check_output",action=all_action,default=d,type=int,choices=[0,1],help=REQUIREMENT_STRING+"Should the software do bounds checking when converting data types?  0 No, or 1 Yes.  This is most useful only for integer output types, whre the default is a simple truncation of bits.")
    else:
        p.add_argument("--extra_scale_factor",action=all_action,type=float,help=REQUIREMENT_STRING+"Extra scaling factor for total power.", required=all_required)
        p.add_argument("--bounds_check_output",action=all_action,type=int,choices=[0,1],help=REQUIREMENT_STRING+"Should the software do bounds checking when converting data types?  0 No, or 1 Yes.  This is most useful only for integer output types, where the default is a simple truncation of bits.", required=all_required)
    for i in range(5):
        option_name = "--extra_string_option_%d"%i
        if(all_required is False):
            d=""
            if(all_action=='append'):
                d=[d]
            p.add_argument(option_name,action=all_action,default=d,help=REQUIREMENT_STRING+"Extra string option for controlling some writers.  Default is '%(default)s'")
        else:
            p.add_argument(option_name,action=all_action,help=REQUIREMENT_STRING+"Extra string option for controlling some writers.", required=all_required)
    return p



############################################################################
def set_option_parsing_user_info(p, PROGRAM_VERSION):
    #p.add_argument("--separate_windows", action="store_true", help="open up separate xterm windows for each RSP lane execution to show progress, instead of having all information from all RSP lanes come out to the current terminal")
    p.add_argument("--verbose", "-v", action="store_true", help="write commands to screen as well as executing")
    p.add_argument("--echo_only", action="store_true", help="Only show the commands that would be processed, do not actually run them")
    p.add_argument("--stdin", action="store_true", help="Read the arguments to the program from stdin instead of from the command line.  If this option is present, it must be the only option on the command line provided, and all regular options must be passed in via stdin.")
    p.add_argument("--version", "-V", action='version', version='%(prog)s '+"%s"%PROGRAM_VERSION, help="Print the version number of this software and exit.")
    return p



############################################################################
def check_array_values(BEAMLETS_PER_LANE,
                       beamlets, subbands, rcumodes, ras, decs, epochs, names):
    NB = len(beamlets)
    NS = len(subbands)
    NR = len(rcumodes)
    NA = len(ras)
    ND = len(decs)
    NE = len(epochs)
    NN = len(names)
    if((NB != NS) or (NB != NR) or (NB != NA) or (NB != ND) or (NB != NE)
       or (NB != NN)):
        logging.error("sizes of array parameters do not match.  len(physical_beamlet_array)=%d, len(subband_array)=%d, len(rcumode_array)=%d, len(rightascension_array)=%d, len(declination_array)=%d, len(epoch_array)=%d, len(sourcename_array)=%d", NB, NS, NR, NA, ND, NE, NN)
        raise RuntimeError("array size mismatch")
    #logging.debug("Have RA,DEc,Epoch,Name=%s, %s, %s, %s", ras, decs, epochs, names)
    check_LOFAR_epochs(epochs)
    check_LOFAR_directions(ras,
                           decs,
                           epochs)
    check_beamlet_values(BEAMLETS_PER_LANE,
                         beamlets,
                         subbands,
                         rcumodes)
    return
   

############################################################################
def check_general_setup_options(options):
    if(convert_station_name(options.station_name) == "Unknown"):
        logging.warning("station name '%s' is not recognized", options.station_name)
    if(options.duration < 1):
        logging.error("duration must be at least 1 second")
        raise RuntimeError("duration bad")
    if(options.start_date is None):
        options.start_date = make_default_start_date()
    if(options.clock_speed == 0):
        logging.warning("clock_speed=0 is a DEBUG mode, assuming you are testing")
    logging.info("original data_type_in is '%s'", options.data_type_in)
    data_in_int = get_data_type(options.data_type_in)
    if(data_in_int == 26):
        # LOFAR 16 bit mode
        if(options.beamlets_per_lane != 61):
            logging.warning("beamlets_per_lane=%d for the LOFAR 16 bit mode is not known by the developer to be working on the LOFAR station.  Please contact the developer if you are sure this mode is available", options.beamlets_per_lane)
    elif(data_in_int == 25):
        # LOFAR 8 bit mode
        if(options.beamlets_per_lane != 122):
            logging.warning("beamlets_per_lane=%d for the LOFAR 8 bit mode is not known by the developer to be working on the LOFAR station.  Please contact the developer if you are sure this mode is available", options.beamlets_per_lane)
    elif(data_in_int == 24):
        # LOFAR 4 bit mode
        if(options.beamlets_per_lane != 244):
            logging.warning("beamlets_per_lane=%d for the LOFAR 4 bit mode is not known by the developer to be working on the LOFAR station.  Please contact the developer if you are sure this mode is available", options.beamlets_per_lane)
    else:
        logging.warning("data_type_in=%s is not known by the developer to be working on the LOFAR station.  Please contact the developer if you are sure this mode is available", options.data_type_in)
    if(options.samples_per_packet != 16):
        logging.warning("samples_per_packet=%d is not known by the developer to be working on the LOFAR station.  Please contact the developer if you are sure this mode is available", options.samples_per_packet)
    #LuMP_Computer_Information.check_recorder_group(options.recorder_group)
    NUM_TOTAL_BEAMLETS = MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES * options.beamlets_per_lane
    if(type(options.physical_beamlet_array) == type([])):
        NB = len(options.physical_beamlet_array)
        NS = len(options.subband_array)
        NR = len(options.rcumode_array)
        NA = len(options.rightascension_array)
        ND = len(options.declination_array)
        NE = len(options.epoch_array)
        NN = len(options.sourcename_array)
        if((NB != NS) or (NB != NR) or (NB != NA) or (NB != ND) or (NB != NE)
           or (NB != NN)):
            logging.error("Number of writer specifications for array parameters do not match.  num_writers(physical_beamlet_array)=%d, num_writers(subband_array)=%d, num_writers(rcumode_array)=%d, num_writers(rightascension_array)=%d, num_writers(declination_array)=%d, num_writers(epoch_array)=%d, num_writers(sourcename_array)=%d", NB, NS, NR, NA, ND, NE, NN)
            raise RuntimeError("array size mismatch")
        for w in range(len(options.physical_beamlet_array)):
            beamlet_array = get_int_beamlets(options.physical_beamlet_array[w],NUM_TOTAL_BEAMLETS)
            subband_array = get_int_subbands(options.subband_array[w],NUM_TOTAL_BEAMLETS)
            rcumode_array = get_int_rcumodes(options.rcumode_array[w],NUM_TOTAL_BEAMLETS)
            rightascension_array = get_double_rightascensions(options.rightascension_array[w],NUM_TOTAL_BEAMLETS)
            declination_array = get_double_declinations(options.declination_array[w],NUM_TOTAL_BEAMLETS)
            epoch_array = get_string_epochs(options.epoch_array[w],NUM_TOTAL_BEAMLETS)
            sourcename_array = get_string_sourcenames(options.sourcename_array[w],NUM_TOTAL_BEAMLETS)
            
            check_array_values(options.beamlets_per_lane,
                               beamlet_array,
                               subband_array,
                               rcumode_array,
                               rightascension_array,
                               declination_array,
                               epoch_array,
                               sourcename_array)
    else:
        beamlet_array = get_int_beamlets(options.physical_beamlet_array,NUM_TOTAL_BEAMLETS)
        subband_array = get_int_subbands(options.subband_array,NUM_TOTAL_BEAMLETS)
        rcumode_array = get_int_rcumodes(options.rcumode_array,NUM_TOTAL_BEAMLETS)
        rightascension_array = get_double_rightascensions(options.rightascension_array,NUM_TOTAL_BEAMLETS)
        declination_array = get_double_declinations(options.declination_array,NUM_TOTAL_BEAMLETS)
        epoch_array = get_string_epochs(options.epoch_array,NUM_TOTAL_BEAMLETS)
        sourcename_array = get_string_sourcenames(options.sourcename_array,NUM_TOTAL_BEAMLETS)

        check_array_values(options.beamlets_per_lane,
                           beamlet_array,
                           subband_array,
                           rcumode_array,
                           rightascension_array,
                           declination_array,
                           epoch_array,
                           sourcename_array)
    return options


############################################################################
def check_recorder_cpu_specification(options):
    if((options.recorder_num_cores == 0) or (options.recorder_cache_size == 0)):
        NUM_CPU,CACHE_SIZE=get_cpuinfo()
        if(options.recorder_num_cores == 0):
            options.recorder_num_cores = NUM_CPU
        if(options.recorder_cache_size == 0):
            options.recorder_cache_size = CACHE_SIZE
    if(options.recorder_ram_size == 0.0):
        options.recorder_ram_size = get_raminfo()
    return options


############################################################################
def setup_command_for_low_level_python_recorder_call(options):
    command = []
    command.append(FULL_PYTHON_RECORDER_PROGRAM_CALL)
    command.append("--start_date=%s"%options.start_date)
    command.append("--duration=%d"%options.duration)
    command.append("--station_name=%s"%options.station_name)
    command.append("--port=%s"%options.port)
    command.append("--datadir=%s"%options.datadir)
    command.append("--data_type_in=%s"%options.data_type_in)
    command.append("--clock_speed=%d"%options.clock_speed)
    command.append("--beamlets_per_lane=%d"%options.beamlets_per_lane)
    command.append("--samples_per_packet=%d"%options.samples_per_packet)
    if(options.backfill_to_start_time):
        command.append("--backfill_to_start_time")
    command.append("--main_recorder_logfile=%s"%options.main_recorder_logfile)
    command.append("--recorder_num_cores=%d"%options.recorder_num_cores)
    command.append("--recorder_cache_size=%d"%options.recorder_cache_size)
    command.append("--recorder_ram_size=%.0f"%options.recorder_ram_size)
    command.append("--writer_type=%s"%options.writer_type)
    command.append("--filename_base=%s"%options.filename_base)
    command.append("--physical_beamlet_array=%s"%options.physical_beamlet_array)
    command.append("--subband_array=%s"%options.subband_array)
    command.append("--rcumode_array=%s"%options.rcumode_array)
    command.append("--rightascension_array=%s"%options.rightascension_array)
    command.append("--declination_array=%s"%options.declination_array)
    command.append("--epoch_array=%s"%options.epoch_array)
    command.append("--sourcename_array=%s"%options.sourcename_array)
    command.append("--data_type_process=%s"%options.data_type_process)
    command.append("--data_type_out=%s"%options.data_type_out)
    command.append("--num_output_channels=%d"%options.num_output_channels)
    command.append("--num_polyphase_filter_taps=%d"%options.num_polyphase_filter_taps)
    command.append("--window_function=%s"%options.window_function)
    command.append("--window_parameter=%.16E"%options.window_parameter)
    command.append("--integration_time=%.16E"%options.integration_time)
    command.append("--scale_by_inverse_samples=%d"%options.scale_by_inverse_samples)
    command.append("--extra_scale_factor=%.16E"%options.extra_scale_factor)
    command.append("--bounds_check_output=%d"%options.bounds_check_output)
    # extra string options
    command.append("--extra_string_option_0=%s"%options.extra_string_option_0)
    command.append("--extra_string_option_1=%s"%options.extra_string_option_1)
    command.append("--extra_string_option_2=%s"%options.extra_string_option_2)
    command.append("--extra_string_option_3=%s"%options.extra_string_option_3)
    command.append("--extra_string_option_4=%s"%options.extra_string_option_4)

    # if(options.separate_windows):
    #     command.append("--separate_windows")
    if(options.verbose):
        command.append("--verbose")
    if(options.echo_only):
        command.append("--echo_only")
    return command

