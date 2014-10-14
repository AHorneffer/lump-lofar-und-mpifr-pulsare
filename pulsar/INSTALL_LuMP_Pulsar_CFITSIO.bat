#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_CFITSIO.bat
# usage: INSTALL_LuMP_Pulsar_CFITSIO.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CFITSIO.control

if [ $BUILD_LUMP_PULSAR_CFITSIO == 1 ]; then
    # install CFITSIO
    # see http://heasarc.gsfc.nasa.gov/fitsio/
    cd "${MYDIR}"/src
    FILENAME="cfitsio3370.tar.gz"
    if [ ! -f "${FILENAME}" ]; then
        wget http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/"${FILENAME}"
    fi
    tar zxf "${FILENAME}"
    cd cfitsio
    codedir=`pwd`
    SSETWO=""
    SSSETHREE=""
    if [ `uname -p` == "x86_64" ];then
        a=(`gcc -march=native -Q --help=target | grep msse2`)
        if [ "${a[1]}" == "[enabled]" ];then
            SSETWO="--enable-sse2"
        fi
        a=(`gcc -march=native -Q --help=target | grep mssse3`)
        if [ "${a[1]}" == "[enabled]" ];then
            SSSETHREE="--enable-ssse3"
        fi
    fi
    ./configure --prefix="${MYDIR}" --enable-reentrant "${SSETWO}" "${SSSETHREE}" CC=gcc --with-bzip2 CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" 
    #
    make ${MY_MAKE_THREADS} shared
    make shared install
    make testprog
    /bin/rm -f testprog.lis
    ./testprog > testprog.lis
    diff testprog.lis testprog.out
    echo "checking result for differences"
    cmp -l testprog.fit testprog.std
    echo "check result, there should be no differences"
    # the set -e command should cause the script to stop if there is a difference
    #"${EDITOR}"
    cd "${origdir}"
    /bin/rm -fr "${codedir}"
fi
echo "BUILD_LUMP_PULSAR_CFITSIO=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CFITSIO.control
