#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PRESTO.bat
# usage: INSTALL_LuMP_Pulsar_PRESTO.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PRESTO.control

if [ $BUILD_LUMP_PULSAR_PRESTO == 1 ]; then
    # install PRESTO
    # http://www.cv.nrao.edu/~sransom/presto/
    cd "${MYDIR}"/src
    FILENAME=presto.20140926.tgz
    BASEPATH=presto
    if [ ! -f "${FILENAME}" ]; then
        git clone git://github.com/scottransom/presto.git "${BASEPATH}"
        tar zcf "${FILENAME}" ./"${BASEPATH}"
    else
        tar zxf "${FILENAME}"
    fi
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PRESTO_P0.patch
    cd "${BASEPATH}"
    codedir=`pwd`
    export PRESTO=`pwd`
    export PKG_CONFIG_PATH="${MYDIR}"/lib64/pkgconfig:"${MYFFTWDIR}"/lib64/pkgconfig
    #"${EDITOR}" ${MYDIR}/../SETUP.sh
    # put PRESTO environment variable into SETUP.sh
    # the presto directory needs to remain here
    cd src
    #"${EDITOR}" Makefile
    # make the following changes
    # on vlb091
    # LOCDIR = $(MYDIR)/opt/search
    # FFTINCDIR = -I$(MYFFTWDIR)/include
    # FFTLIBDIR = -L$(MYFFTWDIR)/lib
    # OTHERINCDIR = -I$(SYSDIR)/include/glib-2.0 -I$(SYSDIR)/lib64/glib-2.0/include -I$(SYSDIR)/lib/x86_64-linux-gnu/glib-2.0/include/ -I$(SYSDIR)/include/cfitsio
    # CC = gcc ${MY_EXTRA_CFLAGS} -fPIC
    # FC = gfortran ${MY_EXTRA_FFLAGS} -fPIC
    # FFLAGS = -fPIC
    # 
    # on lofarXN
    # LOCDIR = $(MYDIR)/opt/search
    # FFTINCDIR = -I$(MYFFTWDIR)/include
    # FFTLIBDIR = -L$(MYFFTWDIR)/lib
    # OTHERINCDIR = -I$(SYSDIR)/include/glib-2.0 -I$(SYSDIR)/lib/glib-2.0/include  
    # CC = gcc ${MY_EXTRA_CFLAGS} -fPIC
    # FC = gfortran ${MY_EXTRA_FFLAGS} -fPIC
    # FFLAGS = -fPIC
    # 
    cat /proc/cpuinfo 
    a=(`cat /proc/meminfo | fgrep MemTotal:`)
    m=1
    if [ "${a[2]}" == "kB" ];then
        m=1024
    fi
    mt=`python -c  "print (${a[1]} * ${m})"`
    echo "total RAM is" "${mt}" "Bytes"
    nt=`grep -c ^processor /proc/cpuinfo`
    # Calculate maximum memory to use, *per core*
    m_per_core=`python -c "print int(${mt} * 0.8 / ${nt})"`
    echo "using maximum memory per core of ${m_per_core} Bytes"
    # calculate the longest FFT (floats) to do in memory, 
    # allowing for input and output arrays
    len_fft_max=`python -c "print int(${mt} * 0.8 / 8 / 2)"`
    echo "using maximum FFT length ${len_fft_max} elements"
    echo -e "// The most RAM for a single core to use in accelsearch\n#define MAXRAMUSE             ${m_per_core}\n// The longest length FFT to do in core memory\n#define MAXREALFFT            ${len_fft_max}\n// The biggest length FFT to use FFTW for (if larger, use 6-step)\n#define BIGFFTWSIZE           ${len_fft_max}\n" >| ../include/meminfo.h
    #cat /proc/meminfo | fgrep MemTotal:
    #"${EDITOR}" ../include/meminfo.h
    # for all systems, set upper limits on the memory usage to fit within RAM
    # note that the FFT sizes are in units of complex floats, so divide memory
    # available by 8
    # #define Ramsize               *total memory minus a bit*
    # #define MAXREALFFT            *total memory minus a bit / 8*
    # #define BIGFFTWSIZE           *total memory minus a bit / 8*
    # Since everything is done in FFTW3 as an int, there is no need
    # to have any of the FFT sizes larger than 2147483647.
    # Converting that to a number of complex_floats, use no more than
    #                                         268435455
    # For computers that will not do much processing, try a big value
    # of                                                     262144
    # RAM=  4 GB   =   4294967296 B   MAXFFT  134217728 BIG 8388608
    # RAM=  8 GB   =   8589934592 B   MAXFFT  268435455 BIG 8388608
    # RAM= 16 GB   =  17179869184 B   MAXFFT  268435455 BIG 8388608
    # RAM= 32 GB   =  34359738368 B   MAXFFT  268435455 BIG 8388608
    # RAM= 64 GB   =  68719476736 B   MAXFFT  268435455 BIG 8388608
    # RAM=128 GB   = 137438953472 B   MAXFFT  268435455 BIG 8388608
    make makewisdom
    make prep
    # make ${MY_MAKE_THREADS} fails
    #"${EDITOR}" Makefile
    # change line 143 from
    #   slalib/sla_test
    # to
    #   bash (export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$(PRESTO)/lib;slalib/sla_test)
    # 
    make
    make clean
    cd ../python
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
    export XLIBDIR
    echo "FFTW3 library directory is " "${MYFFTWDIR}"/lib64
    #"${EDITOR}" setup.py
    # edit presto_library_dirs to contain value of ${MYFFTWDIR}/lib
    #"${EDITOR}" Makefile
    # edit to prevent failure on SuSE 12.1
    # python setup.py install --home=${PRESTO} --prefix=""
    make
    make fftfit
    make clean
    cd $origdir
    # the presto directory needs to remain here
    # rm -fr $codedir
fi
echo "BUILD_LUMP_PULSAR_PRESTO=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PRESTO.control

