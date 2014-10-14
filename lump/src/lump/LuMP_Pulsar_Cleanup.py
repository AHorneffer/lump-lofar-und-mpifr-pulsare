#!/usr/bin/env python
# LuMP_Pulsar_Cleanup.py
# clean up LuMP data header information
#_HIST  DATE NAME PLACE INFO
# 2011 Sep 11  James M Anderson  --MPIfR  start
# 2012 Apr 20  JMA  --update for modulal setup
# 2013 Jul 24  JMA  ---- updates for LuMP v2.0
# 2013 Aug 12  JMA  ---- file list file name change to _lis
# 2013 Sep 05  JMA  ---- bugfix



# Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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

PROGRAM_NAME = "LuMP_Pulsar_Cleanup.py"
PROGRAM_VERSION = MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_VERSION_DATE_STRING


INFO_PRINT = "LOFAR_Station_Beamformed_Info_Dump"




############################################################################
def data_type_in_bytes(data_type):
    if(data_type == "L_Complex32_t"):
        return 4
    elif(data_type == "L_Complex64_t"):
        return 8
    elif(data_type == "L_Complex128_t"):
        return 16
    elif(data_type == "L_Complex160_t"):
        return 20
    elif(data_type == "L_Complex256_t"):
        return 32
    elif(data_type == "L_intComplex8_t"):
        return 1
    elif(data_type == "L_intComplex16_t"):
        return 2
    elif(data_type == "L_intComplex32_t"):
        return 4
    elif(data_type == "L_intComplex64_t"):
        return 8
    elif(data_type == "L_intComplex128_t"):
        return 16
    else:
        logging.error("Unknown LOFAR_raw_data_type_enum value '%s'", data_type)
        raise RuntimeError("Unknown LOFAR_raw_data_type_enum value")


################################################################################
def get_info(filename_base):
    sp = subprocess.Popen([INFO_PRINT,filename_base],
                          bufsize=-1,
                          stdout=subprocess.PIPE)
    data_type = None
    seconds_per_output = None
    num_channels = None
    time_center_output_zero = None
    beamlets_per_sample = None
    writer_type = None
    out_data_size = None
    for line in sp.stdout:
        if(line.startswith("WRITER_TYPE ")):
            t = line.split()[1]
            if((t == "LOFAR_DOFF_LuMP0_OUT")
               or (t == "LOFAR_DOFF_LuMP1_OUT")):
                pass
            else:
                logging.error("Not a LuMP pulsar format.  Found '%s' instead", t)
                raise RuntimeError("Not LuMP")
            writer_type = t
        elif(line.startswith("output_DATA_TYPE")):
            data_type = line.split()[1].strip()
        elif(line.startswith("seconds_per_output")):
            seconds_per_output = float(line.split()[1])
        elif(line.startswith("NUM_CHANNELS")):
            num_channels = int(line.split()[1])
        elif(line.startswith("time_center_output_zero")):
            time_center_output_zero = float(line.split()[1])
        elif(line.startswith("beamlets_per_sample")):
            beamlets_per_sample = int(line.split()[1])
    if((writer_type in ["LOFAR_DOFF_LuMP0_OUT", "LOFAR_DOFF_LuMP1_OUT"])
       and (num_channels == 1)):
        pass
    else:
        if((writer_type is None) or (num_channels is None)):
            raise RuntimeError("could not read LuMP file")
        logging.error("NUM_CHANNELS == %d, not 1.  This is not a valid %s output dataset", num_channels, writer_type)
        raise RuntimeError("bad NUM_CHANNELS for writer type")
    return [data_type, seconds_per_output, num_channels, time_center_output_zero, beamlets_per_sample, writer_type, out_data_size]
    



############################################################################
def override_header_val(header, name, value, chars):
    pos = header.find(name)
    if(pos < 0):
        logging.error("Cannot find keyword '%s' in header", name)
        raise RuntimeError("keyword not found")
    pos += len(name)
    if(name[-1] != ' '):
        pos += 1
    s = "%-" + "%d"%chars + "s"
    s = s%value
    header = header[:pos] + s + header[pos+chars:]
    return header






############################################################################
def fix_header(filename, offset, start, source, RA, Dec, obs_id, telescope,
               pipe):
    fp = open(filename, "r+")
    header = fp.read(4096)
    if(offset == 0):
        pass
    elif(offset > 0):
        keyword = "\nHDR_SIZE"
        size = header.find(keyword)
        if(size < 0):
            logging.error("Cannot find 'HDR_SIZE' in header")
            raise RuntimeError("no HDR_SIZE")
        size += len(keyword)+1
        size = int(header[size:].split(None,1)[0])
        size += offset
        size = "%d"%size
        header = override_header_val(header, keyword, size, 8)
        keyword = "\nUTC_OFFSET_START_CENTER"
        pos = header.find(keyword)
        if(pos < 0):
            logging.error("Cannot find 'UTC_OFFSET_START_CENTER' in header")
            raise RuntimeError("no UTC_OFFSET_START_CENTER")
        pos += len(keyword)+1
        start_c = float(header[pos:].split(None,1)[0])
        header = override_header_val(header, keyword, "%.17E"%start, 24)
        diff = start-start_c
        keyword = "\nUTC_OFFSET_START_LEADING"
        pos = header.find(keyword)
        if(pos < 0):
            logging.error("Cannot find 'UTC_OFFSET_START_LEADING' in header")
            raise RuntimeError("no UTC_OFFSET_START_LEADING")
        pos += len(keyword)+1
        start_l = float(header[pos:].split(None,1)[0])
        header = override_header_val(header, keyword, "%.17E"%(start_l+diff), 24)
    else:
        logging.error("negative offset %d not allowed", offset)
        raise RuntimeError("bad offset")
    if(source is not None):
        header = override_header_val(header, "\nSOURCE", source, 558-525)
    if(RA is not None):
        header = override_header_val(header, "\nRA ", RA, 600-563)
    if(Dec is not None):
        header = override_header_val(header, "\nDEC ", Dec, 642-606)
    if(obs_id is not None):
        header = override_header_val(header, "\nOBS_ID", obs_id, 124-91)
    if(telescope is not None):
        header = override_header_val(header, "\nTELESCOPE", telescope, 684-654)
    if(pipe is not None):
        header = override_header_val(header, "\nREAD_DATA_FROM_PIPE", pipe, 1409-1403)
    fp.seek(0)
    fp.write(header)
    fp.close()
    return






############################################################################
def main():
    p = argparse.ArgumentParser(description="Python program clean up LuMP pulsar datafiles after running LOFAR_Station_Beamformed_Recorder on a single LOFAR recording computer.", epilog="See the accompanying manual for more information.")
    # General setup
    p.add_argument("--datadir", type=str, default=".", help="*OPTIONAL*  Name of the directory of the main data recording area in which the LuMP files reside.  An absolute or a relative path name may be specified.  The datadir '.' may also be specified.", required=False)
    p.add_argument("--filename_base",action="append", type=str,help="*REQUIRED*  The base filename of the data.  If multiple writers were used for the same pulsar, then multiple filename_base arguments should be given, with the various base filenames", required=True)
    p.add_argument("--source_name", type=str, default=None, help="*OPTIONAL*  The name of the pulsar", required=False)
    p.add_argument("--source_RA", type=str, default=None, help="*OPTIONAL*  The right ascension of the pulsar as a string, in standard HH:MM:SS.SSS format.", required=False)
    p.add_argument("--source_Dec", type=str, default=None, help="*OPTIONAL*  The declination of the pulsar as a string, in the standard +-DD:MM:SS.SS format.", required=False)
    p.add_argument("--obs_id", default=None, type=str, help="*OPTIONAL*  Observation ID of this observation, as a string.")
    p.add_argument("--telescope", default=None, type=str, help="*OPTIONAL*  Fake telescope name to provide, when downstream pulsar software does not understand the LOFAR station name provided.")
    p.add_argument("--use_pipe", type=str, default=None, help="*OPTIONAL*  Speficy whether to read the data from a pipe (True) or to not do this (False).", required=False)
    # Information for the user
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_user_info(p,PROGRAM_VERSION)
    if((len(sys.argv) == 2) and (sys.argv[1] == "--stdin")):
        argv = MPIfR_LOFAR_LuMP_Recorder_Common.read_arguments_from_stdin()
        options = p.parse_args(argv)
    else:
        options = p.parse_args()
    MPIfR_LOFAR_LuMP_Recorder_Common.change_to_data_directory(options.datadir)
    MPIfR_LOFAR_LuMP_Recorder_Common.setup_logging(PROGRAM_NAME,options)
    info = []
    start = -1E300
    for fb in options.filename_base:
        i = get_info(fb)
        info.append(i)
        start = max(start,i[3])
    # check for offsets between files
    align_files = True
    for i in info:
        diff = start-i[3]
        num = diff/i[1]
        err = num-math.floor(num+0.5)
        if(abs(err) < 0.001):
            pass
        else:
            logging.warning("file datapoint alignment not possible")
            align_files = False
    # Now run through all of the data files and fix them
    for i,fb in enumerate(options.filename_base):
        # For the LuMP0 format, with 1 subband per file,
        # bytes_per_line = num channels * bytes_per_samp * 2 polarizations
        # For the LuMP1 format, with 1 subband per file,
        # bytes_per_line = num channels * bytes_per_samp * 2 polarizations
        #                  * beamlets_per_sample
        bytes_per_samp = data_type_in_bytes(info[i][0])
        if(info[i][5] == "LOFAR_DOFF_LuMP0_OUT"):
            bytes_per_line = info[i][2] * bytes_per_samp * 2
        elif(info[i][5] == "LOFAR_DOFF_LuMP1_OUT"):
            bytes_per_line = info[i][2] * bytes_per_samp * 2 * info[i][4]
        else:
            logging.error("unsupported writer type '%s'", info[i][5])
            raise RuntimeError("unsupported writer type")
        offset = 0
        if(align_files):
            diff = start-info[i][3]
            num = diff/info[i][1]
            num = int(math.floor(num+0.5))
            offset = num * bytes_per_line
        file_filename = fb + ".file_lis"
        fp = open(file_filename, "r")
        for datafile in fp:
            filename = datafile
            if(filename[-1] == '\n'):
                filename=filename[:-1]
            fix_header(filename, offset, start,
                       options.source_name,
                       options.source_RA, options.source_Dec,
                       options.obs_id, options.telescope,
                       options.use_pipe)
    logging.info("command finished ok")









if __name__ == "__main__":
    main()
