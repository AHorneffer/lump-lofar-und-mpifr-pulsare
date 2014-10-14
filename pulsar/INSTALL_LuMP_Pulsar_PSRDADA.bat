#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PSRDADA.bat
# usage: INSTALL_LuMP_Pulsar_PSRDADA.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRDADA.control

if [ $BUILD_LUMP_PULSAR_PSRDADA == 1 ]; then
    # install psrdada
    # http://psrdada.sourceforge.net/
    cd "${MYDIR}"/src
    FILENAME=psrdada.20140926.tgz
    BASEPATH=psrdada
    if [ ! -f "${FILENAME}" ]; then
        echo "Using cvs to download the PSRDADA source code.  When you are"
        echo "prompted for the password for   anonymous   simply hit Enter."
        touch $HOME/.cvspass
        cvs -d:pserver:anonymous@psrdada.cvs.sourceforge.net:/cvsroot/psrdada login
        cvs -z3 -d:pserver:anonymous@psrdada.cvs.sourceforge.net:/cvsroot/psrdada co -P psrdada
        tar zcf "${FILENAME}" ./psrdada
    else
        tar zxf "${FILENAME}"
    fi
    cd "${BASEPATH}"
    ./bootstrap
    #
    XLIBDIR="/usr/lib"
    if [ -f /usr/lib64/libX11.so ];then
        XLIBDIR="/usr/lib64"
    elif [ -f /usr/lib/libX11.so ];then
        XLIBDIR="/usr/lib"
    elif [ -f /usr/lib/x86_64-linux-gnu/libX11.so ];then
        XLIBDIR="/usr/lib/x86_64-linux-gnu"
    elif [ -f /usr/lib64/libX11.so.6 ];then
        XLIBDIR="/usr/lib64"
    elif [ -f /usr/lib/libX11.so.6 ];then
        XLIBDIR="/usr/lib"
    elif [ -f /usr/lib/x86_64-linux-gnu/libX11.so.6 ];then
        XLIBDIR="/usr/lib/x86_64-linux-gnu"
    fi
    ./configure --prefix="${MYDIR}" --enable-shared --with-x --x-libraries="${XLIBDIR}" --with-fftw3-dir="${MYFFTWDIR}" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L/usr/lib64/termcap" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" PACKAGES="${MYDIR}"
    # at line 6631 (5602 on lofarsrv) in libtool, replace by the following lines
    #       MY_SEARCH_LIST=".la $std_shrext .so .a"
    #       if [ "${name}" = "fftw3f" ]; then
    #         MY_SEARCH_LIST="$std_shrext .la .so .a"
    #       fi
    #       #for search_ext in .la $std_shrext .so .a; do
    #       for search_ext in ${MY_SEARCH_LIST}; do
    #"${EDITOR}" libtool
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRDADA_P0.patch
    cd "${BASEPATH}"
    #
    make ${MY_MAKE_THREADS}
    make check
    make install
    cd ..
    rm -fr "${BASEPATH}"
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_PSRDADA=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRDADA.control
