#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_TEMPO2.bat
# usage: INSTALL_LuMP_Pulsar_TEMPO2.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2.control

if [ $BUILD_LUMP_PULSAR_TEMPO2 == 1 ]; then
    # install tempo2
    # See http://sourceforge.net/projects/tempo2/
    cd "${MYDIR}"/src
    FILENAME=tempo2-2013.9.1.tar.gz
    while [ ! -f "${FILENAME}" ]; do
        cat "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2.txt
        firefox http://sourceforge.net/projects/tempo2/ &
        echo "Move the ${FILENAME} file to the ${MYDIR}/src directory"
        "${EDITOR}" "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2.txt
    done
    tar zxf "${FILENAME}"
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2_P0.patch
    cd tempo2-2013.9.1
    mkdir -p "${MYDIR}"/share/tempo2
    cp -r T2runtime/* "${MYDIR}"/share/tempo2
    # replace observatory.dat file with one including LOFAR stations
    #cp $origdir/observatories.dat "${MYDIR}"/share/tempo2/observatory
    # this has been fixed in the patch above as of 2014 Oct 06
    ./configure --prefix="${MYDIR}"/ --with-fftw3-dir="${MYFFTWDIR}" --with-cfitsio-lib-dir="${MYDIR}"/lib --with-cfitsio-include-dir="${MYDIR}"/include --with-x      CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" --x-libraries=/usr/lib/x86_64-linux-gnu --with-lapack=tatlas --with-blas=tatlas LIBS="-ltatlas"
    #
    make ${MY_MAKE_THREADS}
    make install
    #echo "fix cfitsio/ include directory in inlcude <fitsio.h>"
    #"${EDITOR}" plugin/fermi_plug.C plugin/photons_plug.C
    #echo "fix ./libtool to not f*** up fftw3 library"
    # at line 6628 in libtool, replace by the following lines
    #       MY_SEARCH_LIST=".la $std_shrext .so .a"
    #       if [ "${name}" = "fftw3" ]; then
    #         MY_SEARCH_LIST="$std_shrext .la .so .a"
    #       fi
    #       #for search_ext in .la $std_shrext .so .a; do
    #       for search_ext in ${MY_SEARCH_LIST}; do
    #"${EDITOR}" libtool
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2_P1.patch
    cd tempo2-2013.9.1
    make ${MY_MAKE_THREADS} plugins
    make plugins-install
    cd ..
    rm -fr tempo2-2013.9.1
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_TEMPO2=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2.control
