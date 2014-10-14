#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_CASACORE.bat
# usage: INSTALL_LuMP_Pulsar_CASACORE.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CASACORE.control

if [ $BUILD_LUMP_PULSAR_CASACORE == 1 ]; then
    #install casacore-read-only
    # https://code.google.com/p/casacore/
    # note that the data stuff needs to be downloaded separately
    # from ftp://ftp.atnf.csiro.au/pub/software/asap/data/
    # ftp://ftp.atnf.csiro.au/pub/software/asap/data/asap_data.tar.bz2
    # casacore development version at
    # svn checkout http://casacore.googlecode.com/svn/trunk/ casacore-read-only
    cd "${MYDIR}"/src
    FILENAME0=asap_data.tar.bz2
    if [ ! -f "${FILENAME0}" ]; then
        wget ftp://ftp.atnf.csiro.au/pub/software/asap/data/"${FILENAME0}"
    fi
    FILENAME1=measures_data.tar.bz2
    if [ ! -f "${FILENAME1}" ]; then
        wget ftp://ftp.atnf.csiro.au/pub/software/measures_data/"${FILENAME1}"
    fi
    cd "${MYDIR}"/share
    mkdir -p casapy
    cd casapy
    bunzip2 -k -c "${MYDIR}"/src/"${FILENAME0}" | tar xfm -
    bunzip2 -k -c "${MYDIR}"/src/"${FILENAME1}" | tar xfm -
    cd "${MYDIR}"/src
    FILENAME=casacore-read-only.20140926.tgz
    BASEPATH=casacore-read-only
    if [ ! -f "${FILENAME}" ]; then
        echo "Using svn to download the casacore source code."
        svn checkout http://casacore.googlecode.com/svn/trunk/ "${BASEPATH}"
        tar zcf "${FILENAME}" ./"${BASEPATH}"
    else
        tar zxf "${FILENAME}"
    fi
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CASACORE_P0.patch
    cd "${BASEPATH}"
    codedir=`pwd`
    #echo "Need to correct cmake for change in ATLAS shared library names."
    #echo "(The ATLAS developers have changed the names of some of their libraries,"
    #echo "and if you have an old version of cmake, it may not be able to find them.)"
    #echo "Unfortunately, this may require hand editing."
    #cp /usr/share/cmake*/Modules/FindBLAS.cmake cmake
    #cp /usr/share/cmake*/Modules/FindLAPACK.cmake cmake
    # change line 168 (140,295 on lofarsrv) to put in the library
    # tatlas
    # note that the Debian systems are ancient and need to have
    # line 138 changed from cblas_dgemm to just dgemm
    #"${EDITOR}" cmake/FindBLAS.cmake
    # change line 215 (202 on lofarsrv) to put in the library
    # tatlas
    #"${EDITOR}" cmake/FindLAPACK.cmake
    # 2014-10-09  The following issue seems to be finxed in the current code, so
    # ignore it.
    # change line 94 to also check AIPS_GCC4
    # #if defined(AIPS_CRAY_PGI) || defined(AIPS_GCC4)
    #"${EDITOR}" casa/aipsdef.h
    mkdir build; 
    cd build
    #
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
    cmake .. -DCMAKE_INSTALL_PREFIX="${MYDIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-W -Wall ${MY_EXTRA_CXXFLAGS} -fPIC" -DUSE_HDF5=ON -DUSE_FFTW3=ON -DUSE_THREADS=ON -DBLAS_ROOT_DIR="${MYDIR}" -DLAPACK_ROOT_DIR="${MYDIR}" -DCFITSIO_ROOT_DIR="${MYDIR}" -DCFITSIO_INCLUDE_DIR="${MYDIR}"/include -DCFITSIO_LIBRARY="${MYDIR}"/lib64/libcfitsio.so -DWCSLIB_ROOT_DIR="${MYDIR}" -DHDF5_ROOT_DIR=/usr -DFFTW3_ROOT_DIR="${MYFFTWDIR}" -DDATA_DIR="${MYDIR}"/share/casapy/data -DCXX11="${GCC_CXX11_FLAG}"
    make ${MY_MAKE_THREADS}
    make install
    echo "Expect several failures in the test stage"
    make test || true
    cd "${MYDIR}"/src
    rm -fr "${BASEPATH}"
fi
echo "BUILD_LUMP_PULSAR_CASACORE=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CASACORE.control

