#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_WCSLIB.bat
# usage: INSTALL_LuMP_Pulsar_WCSLIB.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_WCSLIB.control

if [ $BUILD_LUMP_PULSAR_WCSLIB == 1 ]; then
    #install wcslib
    # http://www.atnf.csiro.au/people/mcalabre/WCS/
    cd "${MYDIR}"/src
    FILENAME=wcslib-4.24.tar.bz2
    BASEPATH=wcslib-4.24
    if [ ! -f "${FILENAME}" ]; then
        wget ftp://ftp.atnf.csiro.au/pub/software/wcslib/"${FILENAME}"
    fi
    bunzip2 -k -c "${FILENAME}" | tar xfm -
    cd "${BASEPATH}"
    #
    TERMCAPLIBDIR="/usr/lib"
    if [ -f /usr/lib64/termcap/libtermcap.so ];then
        TERMCAPLIBDIR="/usr/lib64/termcap"
    elif [ -f /usr/lib/termcap/libtermcap.so ];then
        TERMCAPLIBDIR="/usr/lib/termcap"
    elif [ -f /usr/lib64/libtermcap.so ];then
        TERMCAPLIBDIR="/usr/lib64"
    elif [ -f /usr/lib/libtermcap.so ];then
        TERMCAPLIBDIR="/usr/lib"
    fi
    ./configure --prefix="${MYDIR2}" --with-cfitsiolib="${MYDIR}"/lib64 --with-cfitsioinc="${MYDIR}"/include -with-x CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L${TERMCAPLIBDIR}" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" --with-pgplotlib="${MYDIR}"/src/pgplot --with-pgplotinc="${MYDIR}"/src/pgplot
    make build
    make check
    make install
    cd ..
    rm -fr "${BASEPATH}"
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_WCSLIB=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_WCSLIB.control

