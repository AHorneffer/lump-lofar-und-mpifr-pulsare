#!/usr/bin/env python
# make_setup.py
# generate the setup files, shell instructions for environment variables, and
# so on for building and running LuMP
# 2014 Sep 25  James Anderson  --- document existing Python script and extend

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
import argparse
import sys




################################################################################
# GLOBALS
################################################################################

# Pulsar area filename base
PULSAR_INSTALL_NAME_BASE = "pulsar/INSTALL_LuMP_Pulsar"
GCC_INSTALL_NAME_BASE = "gcc/INSTALL_LuMP_GCC"


COPYRIGHT_NOTICE = """
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
"""







################################################################################
def generate_LuMP_setup_scripts(cwd,gcc):
    SH_base = """# SETUP.sh  set up environment variables for LuMP version 2.3
LUMPINSTALLDIR=%s
export LUMPINSTALLDIR
%s
# is PULSAR stuff requested?
if [ "x${USE_LUMP_PULSAR_STUFF}" = "x" ]; then
    true
else
    source "${LUMPINSTALLDIR}/pulsar/SETUP.sh"
fi
# set up LuMP stuff
source "${LUMPINSTALLDIR}/lump/SETUP.sh"

"""
    if(gcc):
        SH_gcc = """# set up gcc stuff
PATH="${LUMPINSTALLDIR}/gcc/bin":${PATH}
if [ "x${LD_LIBRARY_PATH}" == "x" ]; then
    LD_LIBRARY_PATH="${LUMPINSTALLDIR}/gcc/lib64":"${LUMPINSTALLDIR}/gcc/lib"
else
    LD_LIBRARY_PATH="${LUMPINSTALLDIR}/gcc/lib64":"${LUMPINSTALLDIR}/gcc/lib":${LD_LIBRARY_PATH}
fi
if [ "x${LIBRARY_PATH}" == "x" ]; then
    LIBRARY_PATH="${LUMPINSTALLDIR}/gcc/lib64":"${LUMPINSTALLDIR}/gcc/lib"
else
    LIBRARY_PATH="${LUMPINSTALLDIR}/gcc/lib64":"${LUMPINSTALLDIR}/gcc/lib":${LIBRARY_PATH}
fi
export PATH
export LD_LIBRARY_PATH
export LIBRARY_PATH
"""
    else:
        SH_gcc = ""
    CSH_base = """# SETUP.csh  set up environment variables for LuMP version 2.3
setenv LUMPINSTALLDIR %s
%s
# is PULSAR stuff requested?
if ($?USE_LUMP_PULSAR_STUFF) then
    source "$LUMPINSTALLDIR"/pulsar/SETUP.csh
else
    # do nothing
endif
# set up LuMP stuff
source "$LUMPINSTALLDIR"/lump/SETUP.csh
"""
    if(gcc):
        CSH_gcc = """# set up gcc stuff
setenv PATH "$LUMPINSTALLDIR"/gcc/bin:{$PATH}
if ($?LD_LIBRARY_PATH) then
    setenv LD_LIBRARY_PATH "$LUMPINSTALLDIR"/gcc/lib64:$LUMPINSTALLDIR/gcc/lib
else
    setenv LD_LIBRARY_PATH "$LUMPINSTALLDIR"/gcc/lib64:$LUMPINSTALLDIR/gcc/lib:$LD_LIBRARY_PATH
endif
if ($?LIBRARY_PATH) then
    setenv LIBRARY_PATH "$LUMPINSTALLDIR"/gcc/lib64:$LUMPINSTALLDIR/gcc/lib
else
    setenv LIBRARY_PATH "$LUMPINSTALLDIR"/gcc/lib64:$LUMPINSTALLDIR/gcc/lib:$LIBRARY_PATH
endif
"""
    else:
        CSH_gcc = ""        
    fp = open("SETUP.sh", "w")
    fp.write(COPYRIGHT_NOTICE)
    fp.write(SH_base%(cwd,SH_gcc))
    fp.close()

    fp = open("SETUP.csh", "w")
    fp.write(COPYRIGHT_NOTICE)
    fp.write(CSH_base%(cwd,CSH_gcc))
    fp.close()

    return


    
############################################################################
def gen_INSTALL_LuMP_Pulsar_control(pulsar, search):
    code_pulsar = 0
    if(pulsar):
        code_pulsar = 1
    code_search = 0
    if(pulsar):
        code_search = 1
    fp = open(PULSAR_INSTALL_NAME_BASE + ".control", "w")
    fp.write("export USE_LUMP_PULSAR_STUFF=1\n")
    fp.write("export USE_LUMP_PULSAR_SEARCH=1\n")
    fp.write("export BUILD_LUMP_PULSAR_STUFF=%d\n"%(code_pulsar))
    fp.write("export BUILD_LUMP_PULSAR_SEARCH=%d\n"%(code_search))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_CFITSIO.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_CFITSIO=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_SOFA.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_SOFA=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PGPLOT.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PGPLOT=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_TEMPO.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_TEMPO=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_ATLAS.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_ATLAS=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_TEMPO2.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_TEMPO2=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PSRCAT.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PSRCAT=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PSRXML.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PSRXML=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PSRCHIVE.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PSRCHIVE=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PSRDADA.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PSRDADA=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_WCSLIB.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_WCSLIB=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_CASACORE.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_CASACORE=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_LOFARDAL.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_LOFARDAL=%d\n"%(code_pulsar))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PRESTO.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PRESTO=%d\n"%(code_search))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_SIGPROC.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_SIGPROC=%d\n"%(code_search))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_PULSARHUNTER.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_PULSARHUNTER=%d\n"%(code_search))
    fp.close()
    fp = open(PULSAR_INSTALL_NAME_BASE + "_DSPSR.control", "w")
    fp.write("export BUILD_LUMP_PULSAR_DSPSR=%d\n"%(code_pulsar))
    fp.close()
    return
    


############################################################################
def gen_INSTALL_LuMP_gcc_control(gcc, gcc_check):
    code_gcc = 0
    if(gcc):
        code_gcc = 1
    code_gcc_check = 0
    if(gcc):
        code_gcc_check = 1
    fp = open(GCC_INSTALL_NAME_BASE + ".control", "w")
    fp.write("export BUILD_LUMP_GCC=%d\n"%(code_gcc))
    fp.write("export BUILD_LUMP_GCC_CHECK=%d\n"%(code_gcc_check))
    fp.close()
    return



    

############################################################################
def main():
    p = argparse.ArgumentParser(description="Python program to set up the build and run environment for LuMP.  This program will generate environment variable definition files for building and running LuMP that should be sourced from your standard shell.  Bash and (t)csh shells are supported.  This script will also set up instruction files for building various subcomponents, or not building them.  See below for more information about specific options available.", epilog="See the accompanying manual for more information.")
    p.add_argument("--build_gcc", "--gcc",  action="store_true", help="Tell LuMP to build its own version of the gcc compiler suite.")
    p.add_argument("--build_gcc_check", "--gcc_check",  action="store_true", help="Tell LuMP to run the test checks on gcc compiler suite if it is built.  These checks can take a *long* time (4 hours on a computer from 2012), so you may decide to skip them.")
    p.add_argument("--build_pulsar_dspsr", "--pulsar", action="store_true", help="Tell LuMP to build the relevant pulsar software for working with dspsr.")
    p.add_argument("--build_pulsar_search", "--pulsarsearch", action="store_true", help="Tell LuMP to build the pulsar searching software.  Note that building the pulsar search software also requires installing the software for the pulsar dspsr chain too.")
    p.add_argument("--verbose", "-v", action="store_true", help="write commands to screen as well as executing")

    options = p.parse_args()
    if(options.build_pulsar_search):
        options.build_pulsar_dspsr = True

    cwd = os.getcwd()
    generate_LuMP_setup_scripts(cwd, options.build_gcc)
    gen_INSTALL_LuMP_Pulsar_control(options.build_pulsar_dspsr,
                                    options.build_pulsar_search)
    gen_INSTALL_LuMP_gcc_control(options.build_gcc, options.build_gcc_check)
    return




if __name__ == "__main__":
    main()
