#!/usr/bin/env python
# Basic_LuMP_Power0_Recorder.py
# basing running of LOFAR beamformed data recording software for Power0
#_HIST  DATE NAME PLACE INFO
# 2012 Jan 18  James M Anderson  --MPIfR  start
# 2012 Mar 18  JMA --update for new logging system in main recorder
# 2012 Apr 20  JMA  --update for modulal setup
# 2012 May 14  JMA  --update for info2 control setup



# Copyright (c) 2011, 2012, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
import MPIfR_LOFAR_LuMP_Recorder_Common



################################################################################
# GLOBALS
################################################################################

PROGRAM_NAME = "Basic_LuMP_Power0_Recorder.py"
PROGRAM_VERSION = MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_VERSION_DATE_STRING










############################################################################
def main():
    p = argparse.ArgumentParser(description="Python program to run LOFAR_Station_Beamformed_Recorder.py for basic LuMP Power0 output mode operation on a single LOFAR recording computer.", epilog="See the accompanying manual for more information.")
    # General setup
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_general_setup(p)
    # setup for specific writers
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_writer_setup("5",
                                                                         "16",
                                                                         "16",
                                                                         1,
                                                                         1,
                                                                         "0",
                                                                         1.0,
                                                                         p)
    # Information for the user
    p = MPIfR_LOFAR_LuMP_Recorder_Common.set_option_parsing_user_info(p)
    options = p.parse_args()
    MPIfR_LOFAR_LuMP_Recorder_Common.setup_logging(PROGRAM_NAME,options)
    options = MPIfR_LOFAR_LuMP_Recorder_Common.check_general_setup_options(options)
    command = MPIfR_LOFAR_LuMP_Recorder_Common.setup_command_for_low_level_python_recorder_call(options)
    logging.info("Running command %s",command)
    if(not options.echo_only):
        try:
            p = subprocess.Popen(command,shell=False)
            os.nice(10)
            retcode = os.waitpid(p.pid,0)[1]
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
