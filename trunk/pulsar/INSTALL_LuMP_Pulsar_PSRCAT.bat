#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PSRCAT.bat
# usage: INSTALL_LuMP_Pulsar_PSRCAT.bat
# 2014 Sep 26  James Anderson  --- GFZ  move from original huge install file

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



if [ "a${LUMPINSTALLDIR}" = "a" ]; then
    origdir=`pwd`
    export LUMPINSTALLDIR=`dirname "${origdir}"`
fi

source "${LUMPINSTALLDIR}"/pulsar/INSTALL_LuMP_Pulsar.control
source "${LUMPINSTALLDIR}"/pulsar/INSTALL_LuMP_Pulsar_common.sh

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCAT.control

if [ $BUILD_LUMP_PULSAR_PSRCAT == 1 ]; then
    # install psrcat
    # http://www.atnf.csiro.au/people/pulsar/psrcat/
    cd "${MYDIR}"/src
    FILENAME=psrcat_pkg.tar.gz
    if [ ! -f "${FILENAME}" ]; then
        wget http://www.atnf.csiro.au/people/pulsar/psrcat/downloads/"${FILENAME}"
    fi
    tar zxf "${FILENAME}"
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCAT_P0.patch
    cd psrcat_tar
    #echo "set gcc arguments to"
    #echo "${MY_EXTRA_CFLAGS} -fPIC "
    #"${EDITOR}" makeit
    # make edits
    # suggest changes as follows
    # gcc arguments ${MY_EXTRA_CFLAGS} -fPIC 
    ./makeit
    cp psrcat "${MYDIR}"/bin
    cp libpsrcat.a "${MYDIR}"/lib
    mkdir -p "${MYDIR}"/share/psrcat
    cp *.db "${MYDIR}"/share/psrcat
    cd ..
    /bin/rm -fr psrcat_tar
fi
echo "BUILD_LUMP_PULSAR_PSRCAT=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCAT.control

