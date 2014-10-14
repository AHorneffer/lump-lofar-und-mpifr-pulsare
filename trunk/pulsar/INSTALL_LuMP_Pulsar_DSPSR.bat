#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_DSPSR.bat
# usage: INSTALL_LuMP_Pulsar_DSPSR.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR.control


if [ $BUILD_LUMP_PULSAR_DSPSR == 1 ]; then
    # install dspsr
    # http://dspsr.sourceforge.net/index.shtml
    #       for developing dspsr, do
    #       git clone ssh://USERNAME@git.code.sf.net/p/dspsr/code dspsr
    #       for just downloading, do
    #       git clone git://git.code.sf.net/p/dspsr/code dspsr
    cd "${MYDIR}"/src
    FILENAME=dspsr.20140926.tgz
    BASEPATH=dspsr
    if [ ! -f "${FILENAME}" ]; then
        git clone git://git.code.sf.net/p/dspsr/code dspsr
        tar zcf "${FILENAME}" ./dspsr
    else
        tar zxf "${FILENAME}"
    fi
    cd "${BASEPATH}"
    codedir=`pwd`
    echo "guppi_daq installation does not exist, so guppi not compiled"
    echo "fadc code broken, will not use"
    echo "no libpuma stuff, so not using puma"
    echo "wapp code broken, will not use"
    echo -e "apsr asp bcpm bpsr caspsr cpsr cpsr2 dada dummy fits gmrt lbadr lbadr64 lofar_dal lump mark4 mark5 maxim mwa pdev pmdaq puma2 s2 sigproc spda1k spigot vdif \n" > backends.list
    ./bootstrap
    #
    #
    # echo "configure fails checking PSRCHIVE because it does not link in"
    # echo "PGPLOT libraries in ${PGPLOT_DIR}"
    # echo "Add the following line to the PSRCHIVE check area in the file configure (was line 17920)"
    # echo '      PSRCHIVE_LIBS="$PSRCHIVE_LIBS -lcpgplot -lpgplot -lgfortran -lm -lquadmath"'
    # echo "following the line"
    # echo '      PSRCHIVE_LIBS=`$psrchive_config --libs`'
    # "${EDITOR}" configure
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
    ./configure --prefix="${MYDIR}" --enable-shared --with-x --x-libraries="${XLIBDIR}" --with-psrchive=psrchive --with-cfitsio-lib-dir="${MYDIR}"/lib64 --with-cfitsio-include-dir="${MYDIR}"/include --with-pgplot-extra="-L${PGPLOT_DIR} -lcpgplot -lXaPgplot" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L${PGPLOT_DIR}" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" PACKAGES=${MYDIR} CPPFLAGS="-I${MYDIR}/include -I${PGPLOT_DIR}" 
    #
    #
    #echo "Fix linking order, add -lpsrmore -lpsrutil to end of INCLUDE_LDFLAGS"
    #echo "was line 166"
    #"${EDITOR}" Kernel/Applications/Makefile
    #
    # add 
    #include <unistd.h>
    # to the following files
    #"${EDITOR}" Kernel/Formats/gmrt/GMRTFile.C Kernel/Formats/pmdaq/PMDAQFile.C Signal/General/test_OptimalFFT.C
    # at line 6631 (5602 on lofarsrv) in libtool, replace by the following lines
    #       MY_SEARCH_LIST=".la $std_shrext .so .a"
    #       if [ "${name}" = "fftw3f" ]; then
    #         MY_SEARCH_LIST="$std_shrext .la .so .a"
    #       fi
    #       #for search_ext in .la $std_shrext .so .a; do
    #       for search_ext in ${MY_SEARCH_LIST}; do
    #"${EDITOR}" libtool
    #
    cd "${MYDIR}"/src
    python "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR_P0.patch.py
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR_P0.patch
    echo "The following patch fixes problems with spelling errors in the PsrXML library"
    echo "that have presumably been fixed in newer versions of the PsrXML library"
    echo "than have been tested for LuMP.  If compilation fails because"
    echo "'receiver_t' has no member named 'feedSymetry'     or"
    echo "'telescope_t' has no member named 'lattitude'      then"
    echo "comment out the patch command below."
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR_P1.patch
    cd "${BASEPATH}"
    make ${MY_MAKE_THREADS}
    make check
    make install
    cd "${MYDIR}"/src
    rm -fr "${BASEPATH}"
fi
echo "BUILD_LUMP_PULSAR_DSPSR=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR.control

