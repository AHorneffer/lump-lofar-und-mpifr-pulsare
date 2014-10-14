#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_LOFARDAL.bat
# usage: INSTALL_LuMP_Pulsar_LOFARDAL.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_LOFARDAL.control

if [ $BUILD_LUMP_PULSAR_LOFARDAL == 1 ]; then
    # install LOFAR DAL
    # http://www.lofar.org/wiki/doku.php?id=public:user_software:dal
    #     used git clone https://github.com/nextgen-astrodata/DAL.git DAL
    cd "${MYDIR}"/src
    FILENAME=lofardal.20140926.tgz
    BASEPATH=DAL
    if [ ! -f "${FILENAME}" ]; then
        echo "Using git to download the LOFAR DAL source code."
        git clone https://github.com/nextgen-astrodata/DAL.git "${BASEPATH}"
        tar zcf "${FILENAME}" ./"${BASEPATH}"
    else
        tar zxf "${FILENAME}"
    fi
    cd "${BASEPATH}"
    codedir=`pwd`
    mkdir build
    cd build
    export CC=`which gcc`
    export CXX=`which g++`
    export FC=`which gfortran`
    export F77=`which gfortran`
    GCC_VER=(`gcc -v 2>&1 | fgrep 'gcc version'`)
    GCC_CXX11_FLAG="NO"
    vercomp "${GCC_VER[2]}" "4.5.99"
    case $? in
        0) GCC_CXX11_FLAG="NO";;
        1) GCC_CXX11_FLAG="ON";;
        2) GCC_CXX11_FLAG="NO";;
    esac
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${MYDIR}"  -DCMAKE_C_FLAGS="-W -Wall ${MY_EXTRA_CXXFLAGS} -fPIC" -DCMAKE_CXX_FLAGS="-W -Wall ${MY_EXTRA_CXXFLAGS} -fPIC" -DPYTHON_SITE_PACKAGES="${MYDIR}"/lib/python2.7/site-packages
    make ${MY_MAKE_THREADS}
    make install
    ctest ${MY_MAKE_THREADS}
    cd "${MYDIR}"/src
    rm -fr "${BASEPATH}"
fi
echo "BUILD_LUMP_PULSAR_LOFARDAL=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_LOFARDAL.control

