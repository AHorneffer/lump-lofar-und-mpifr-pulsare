#!/usr/bin/env python
# MPIfR_FFTW3_default_wisdom.py
# automate generation of the default FFTW3 wisdom file for LuMP
#_HIST  DATE NAME PLACE INFO
# 2012 May 14  James M Anderson  --- GFZ  start
# 2014 Sep 25  JMA  --- change num_threads option to default to checking
#                       how many CPU cores are present, change default max
#                       channels to 32


# Copyright (c) 2014 James M. Anderson <anderson@gfz-potsdam.de>

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
import socket
import MPIfR_LOFAR_LuMP
import MPIfR_LOFAR_LuMP_Recorder_Common
import tempfile
import errno
import shutil



################################################################################
# GLOBALS
################################################################################

PROGRAM_NAME = "MPIfR_FFTW3_default_wisdom.py"
PROGRAM_VERSION = MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_VERSION_DATE_STRING


EXTERNAL_PROGRAM_NAME = "MPIfR_FFTW3_default_wisdom"

WISDOM_TYPES = ["16 bit half-precision", "32 bit float", "64 bit double", "80 bit long double", "128 bit quadruple precision"]
WISDOM_EXTENSIONS = ["h", "f", "", "l", "q"]
WISDOM_NAME = "wisdom"

MPIfR_Numerical_FFTW3_LuMP_Channel_Number_File = "MPIfR_Numerical_Channelization_List.txt"







############################################################################
def check_num_channels(n, max_n):
    if(n & 0x1):
        return None
    if(n > max_n):
        return None
    return n


############################################################################
def gen_wisdom_channelizations_all(fp, max_n):
    fp.seek(0)
    count = 0
    # all even values up to 32
    for i in range(4,33,2):
        if(check_num_channels(i,max_n) is None):
            continue
        fp.write("%d\n"%(i))
        logging.info("will process for %6d channels", i)
        count = count+1
    fp.flush()
    return fp, 0x1F, count


############################################################################
def gen_wisdom_channelizations_16_32_64_80(fp, max_n):
    fp.seek(0)
    count = 0
    # powers of 2, 64 to 256 (2^6--2^8)
    for i in range(6,9):
        x = 2**i
        if(check_num_channels(x,max_n) is None):
            continue
        fp.write("%d\n"%(x))
        logging.info("will process for %6d channels", x)
        count = count+1
    fp.flush()
    return fp, 0xF, count


############################################################################
def gen_wisdom_channelizations_32_64(fp, max_n):
    fp.seek(0)
    count = 0
    # some other numbers
    for i in [40,60,80,100,200]:
        if(check_num_channels(i,max_n) is None):
            continue
        fp.write("%d\n"%(i))
        logging.info("will process for %6d channels", i)
        count = count+1
    fp.flush()
    return fp,0x6, count


############################################################################
def gen_wisdom_channelizations_32(fp, max_n):
    fp.seek(0)
    count = 0
    # powers of 2, 512 to 1024 (2^9--2^{10})
    for i in range(9,11):
        x = 2**i
        if(check_num_channels(x,max_n) is None):
            continue
        fp.write("%d\n"%(x))
        logging.info("will process for %6d channels", x)
        count = count+1
    fp.flush()
    return fp, 0x2, count



############################################################################
def myhcat(root_path, lump, fftw3, base, hostname):
    return "%s%c%s%c%s%c%s%s"%(root_path, os.path.sep,
                               lump, os.path.sep,
                               fftw3, os.path.sep,
                               base, hostname)
############################################################################
def myrcat(root_path, libdata, lump, fftw3, base, hostname):
    return "%s%c%s%c%s%c%s%c%s%s"%(root_path, os.path.sep,
                                   libdata, os.path.sep,
                                   lump, os.path.sep,
                                   fftw3, os.path.sep,
                                   base, hostname)

############################################################################
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


############################################################################
def backup_home_wisdom(HOME, HOSTNAME, copy_not_move):
    # check for path
    path =  myhcat(HOME,
                   MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY,
                   MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                   "",
                   "")
    path = os.path.dirname(path)
    if(not os.path.isdir(path)):
        mkdir_p(path)
    for i,e in enumerate(WISDOM_EXTENSIONS):
        base = WISDOM_NAME + e + '.'
        filename = myhcat(HOME,
                          MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY,
                          MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                          base,
                          HOSTNAME)
        if(os.path.isfile(filename)):
            backup = filename+".bak"
            if(copy_not_move):
                shutil.copy2(filename, backup)
            else:
                shutil.move(filename, backup)
        else:
            if(i!=0):
                logging.warning("Could not find HOME area file '%s' (for %s wisdom) to make a backup", filename, WISDOM_TYPES[i])
            else:
                logging.info("Could not find HOME area file '%s' (for %s wisdom) to make a backup, but this type is not expected to be working yet", filename, WISDOM_TYPES[i])
    return
    
    


############################################################################
def backup_root_wisdom(ROOT, HOSTNAME, copy_not_move):
    # check for path
    path =  myrcat(ROOT,
                   MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                   MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                   MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                   "",
                   "")
    path = os.path.dirname(path)
    if(not os.path.isdir(path)):
        mkdir_p(path)
    for i,e in enumerate(WISDOM_EXTENSIONS):
        base = WISDOM_NAME + e + '.'
        filename = myrcat(ROOT,
                          MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                          MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                          MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                          base,
                          HOSTNAME)
        if(os.path.isfile(filename)):
            backup = filename+".bak"
            if(copy_not_move):
                shutil.copy2(filename, backup)
            else:
                shutil.move(filename, backup)
        else:
            if(i!=0):
                logging.warning("Could not find ROOT area file '%s' (for %s wisdom) to make a backup", filename, WISDOM_TYPES[i])
            else:
                logging.info("Could not find ROOT area file '%s' (for %s wisdom) to make a backup, but this type is not expected to be working yet", filename, WISDOM_TYPES[i])
    return
    
    


############################################################################
def copy_home_wisdom_to_root(HOME, ROOT, HOSTNAME):
    for i,e in enumerate(WISDOM_EXTENSIONS):
        base = WISDOM_NAME + e + '.'
        home_filename = myhcat(HOME,
                               MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY,
                               MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                               base,
                               HOSTNAME)
        root_filename = myrcat(ROOT,
                               MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                               MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                               MPIfR_LOFAR_LuMP.MPIfR_Numerical_FFTW3_Storage_Directory,
                               base,
                               HOSTNAME)
        if(os.path.isfile(home_filename)):
            shutil.copy(home_filename, root_filename)
        else:
            if(i!=0):
                logging.warning("Could not find HOME area file '%s' (for %s wisdom) to make a backup", home_filename, WISDOM_TYPES[i])
            else:
                logging.info("Could not find HOME area file '%s' (for %s wisdom) to make a backup, but this type is not expected to be working yet", home_filename, WISDOM_TYPES[i])
    return
    
    

############################################################################
def my_int(x):
    return int(x,0)



    



############################################################################
def main():
    p = argparse.ArgumentParser(description="Python program to run MPIfR_FFTW3_default_wisdom.  This Python program automates the process of dealing with generating multiple threads to work with and copying the generated wisdom information to the default LuMP installation area.", epilog="See the accompanying manual for more information.")
    p.add_argument("--max_channels", "-m", default=32, type=int, help="The maximum number of channels that will be allowed for pre-computing the FFTW3 wisdom.  Remember that the wisdom calculations will be done for parallel transforms of size N_CHAN x (N_BEAMLETS * 2)  to account for processing all beamlets and polarizations.  For the 4 bit mode, the maximmum of N_BEAMLETS * 2 is 488, so calculating wisdom can take a long time for large channel numbers.  Defaults to %(default)d.")
    p.add_argument("--num_threads", "-n", default=-1, type=int, help="The number of concurrent instances of wisdom generation to start up.  LuMP uses multiple threads of single-threaded FFTW3, and you should pick the same number of threads here that you will normally use for LuMP processing.  If the value -1 is supplied, this program will check for the number of reasonably independent CPU cores and use that number.  Defaults to %(default)d.")
    p.add_argument("--type_mask", "-t", default=0x1F, type=my_int, help="A bitmask for setting which data types to generate wisdom for.  ! means use the type, 0 means do not use the type.  See the code for more details.  Defaults to 0x%(default)X.")
    p.add_argument("--use_existing_home", action='store_true', help="When this argument is given, any existing wisdom in the user's HOME area will be used in the generation of the new wisdom information.  You probably do not want this.")
    p.add_argument("--use_existing_root", action='store_true', help="When this argument is given, any existing wisdom in the user's LuMP_ROOT area will be used in the generation of the new wisdom information.  You probably do not want this.")

    p.add_argument("--main_recorder_logfile",default=PROGRAM_NAME+".log",type=str,help="Name of the logfile to write out for the main recorder program.  Defaults to %(default)s")
    p.add_argument("--verbose", "-v", action="store_true", help="write commands to screen as well as executing")
    p.add_argument("--echo_only", action="store_true", help="Only show the commands that would be processed, do not actually run them")
    p.add_argument("--version", "-V", action='version', version='%(prog)s '+"%s"%PROGRAM_VERSION, help="Print the version number of this software and exit.")
    
    options = p.parse_args()
    MPIfR_LOFAR_LuMP_Recorder_Common.setup_logging(PROGRAM_NAME,options)
    TYPE_MASK = options.type_mask
    if(options.num_threads <= 0):
        NUM_CPU, CACHE_SIZE = MPIfR_LOFAR_LuMP_Recorder_Common.get_cpuinfo()
        options.num_threads = NUM_CPU


    # check that HOME and LuMP_ROOT exist
    HOME = os.getenv("HOME")
    if(HOME is None):
        logging.error("HOME environment variable not defined")
        raise RuntimeError("HOME environment variable not defined")
    LuMP_ROOT = os.getenv(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT)
    if(LuMP_ROOT is None):
        logging.error("%s environment variable not defined"%(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT))
        raise RuntimeError("%s environment variable not defined"%(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT))
    HOSTNAME = socket.gethostname()

    user = raw_input("Are you sure you want to overwrite the LuMP FFTW3 Wisdom files?  Any additional wisdom you may have generated may be lost.  Answer Y for yes to continue: ")
    if((user is None) or (type(user) != type("Y")) or (not ((user[0] == 'Y') or (user[0] == 'y')))):
        logging.warning("User commanded us to stop --- no wisdom files were updated")
        return

    # back up the user's home area and the LuMP_ROOT area wisdom stuff
    backup_home_wisdom(HOME, HOSTNAME, options.use_existing_home)
    backup_root_wisdom(LuMP_ROOT, HOSTNAME, options.use_existing_root)

    # get the tempfile
    fp = tempfile.NamedTemporaryFile(mode="w+")
    logging.info("Using file '%s' for channelization list", fp.name)
    


    for func in [gen_wisdom_channelizations_all, gen_wisdom_channelizations_16_32_64_80, gen_wisdom_channelizations_32_64, gen_wisdom_channelizations_32]:
        # generate the list of channelizations to make wisdom for
        fp, bitmask, count = func(fp, options.max_channels)
        if(count == 0):
            continue
        bitmask = bitmask & TYPE_MASK

        command = [EXTERNAL_PROGRAM_NAME, fp.name, "0x%X"%(bitmask)]
        processes = []
        for i in range(options.num_threads):
            logging.info("Running command %s",command)
            try:
                p = subprocess.Popen(command,shell=False)
                processes.append(p)
            except OSError, e:
                logging.error("exception found trying to run command %s", command[0])
                raise e
        for i in range(options.num_threads):
            try:
                retcode = os.waitpid(processes[i].pid,0)[1]
                logging.info("command returned %d", retcode)
                if(retcode != 0):
                    logging.error("command had an error %d", retcode)
                    raise RuntimeError("command failed")
            except OSError, e:
                logging.error("exception found trying to run command %s", command[0])
                raise e
        logging.info("command %s finished ok", command[0])

    # close the tempfile
    fp.close()

    copy_home_wisdom_to_root(HOME, LuMP_ROOT, HOSTNAME)
    return








if __name__ == "__main__":
    main()
