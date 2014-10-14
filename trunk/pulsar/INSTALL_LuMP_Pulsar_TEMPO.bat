#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_TEMPO.bat
# usage: INSTALL_LuMP_Pulsar_TEMPO.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO.control

if [ $BUILD_LUMP_PULSAR_TEMPO == 1 ]; then
    # install tempo
    # http://sourceforge.net/projects/tempo/
    # http://tempo.sourceforge.net/
    cd "${MYDIR}"/src
    FILENAME=tempo.20140430.tgz
    if [ ! -f "${FILENAME}" ]; then
        git clone git://git.code.sf.net/p/tempo/tempo
        tar zcf "${FILENAME}" ./tempo
    else
        tar zxf "${FILENAME}"
    fi
    cd tempo
    ./prepare
    add blas and lapack here
    ./configure --prefix="${MYDIR}" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC -L${MYDIR}/lib -L${MYDIR}/lib64" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC -L${MYDIR}/lib -L${MYDIR}/lib64" --with-blas=atlas --with-lapack=atlas
    make ${MY_MAKE_THREADS}
    make install
    make clean
    #echo Set SETUP.sh to have
    #echo export TEMPO=`pwd`
    #"${EDITOR}" $origdir/SETUP.sh
    cd "${origdir}"
    #source SETUP.sh
fi
echo "BUILD_LUMP_PULSAR_TEMPO=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO.control

