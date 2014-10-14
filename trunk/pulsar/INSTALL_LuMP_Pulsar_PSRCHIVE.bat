#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PSRCHIVE.bat
# usage: INSTALL_LuMP_Pulsar_PSRCHIVE.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE.control


if [ $BUILD_LUMP_PULSAR_PSRCHIVE == 1 ]; then
    # install psrchive
    # http://psrchive.sourceforge.net/
    # use git to download source
    cd "${MYDIR}"/src
    FILENAME=psrchive.20140926.tgz
    BASEPATH=psrchive
    if [ ! -f "${FILENAME}" ]; then
        git clone git://git.code.sf.net/p/psrchive/code psrchive
        tar zcf "${FILENAME}" ./psrchive
    else
        tar zxf "${FILENAME}"
    fi
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE_P0.patch
    cd "${BASEPATH}"
    ./bootstrap
    #echo "SuSE insists on installing termcap libraries in strange places"
    #echo "Do not install Qt3 stuff on Ubuntu/Debian"
    #echo "dspsr doesn't compile well with static libraries"
    #echo "at line 21439 (20707 on lofarsrv) in configure, insert the following"
    #   if test "$have_fftw3" = yes; then
    #    FFTW_LIBS="$fftw3_LIBS $FFTW_LIBS"
    #    FFTW_CFLAGS="$fftw3_CFLAGS $FFTW_CFLAGS"
    # to
    #   if test "$have_fftw3" = yes; then
    #    FFTW_LIBS="$fftw3_LIBS $FFTW_LIBS"
    #    FFTW_CFLAGS="$fftw3_CFLAGS $FFTW_CFLAGS"
    #    ac_save_LIBS="$LIBS"
    #    LIBS="$FFTW_LIBS $ac_save_LIBS"
    #"${EDITOR}" configure
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE_P1.patch
    cd "${BASEPATH}"
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
    ./configure --prefix="${MYDIR}" --with-compiler=gcc --enable-shared --with-x --x-libraries="${XLIBDIR}" --with-psrcat=psrcat --with-fftw3-dir="${MYFFTWDIR}" --with-cfitsio-lib-dir="${MYDIR}"/lib64 --with-cfitsio-include-dir="${MYDIR}"/include --with-psrxml-dir="${MYDIR}" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L${TERMCAPLIBDIR}" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" PACKAGES="${MYDIR}"
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
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE_P2.patch
    cd "${BASEPATH}"
    make ${MY_MAKE_THREADS}
    make check
    make install
    #cd Util/fft/
    #echo "change install_bench to increment number of threads by 1 instead of"
    #echo "multiplying by 2"
    # at line 58, replace by
    #    @ nthread = $nthread + 1
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE_P3.patch
    cd "${BASEPATH}"/Util/fft/
    SYSTEM=`uname -s`
    . "${LUMPINSTALLDIR}"/cpu/"${SYSTEM}"/cpu_speed.sh
    set_performace_cpu_speed_info
    #"${EDITOR}" install_bench
    make bench
    set_original_cpu_speed_info
    cd "${MYDIR}"/src
    rm -fr "${BASEPATH}"
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_PSRCHIVE=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE.control

