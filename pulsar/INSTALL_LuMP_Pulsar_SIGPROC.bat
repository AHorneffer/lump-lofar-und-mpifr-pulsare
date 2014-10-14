#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_SIGPROC.bat
# usage: INSTALL_LuMP_Pulsar_SIGPROC.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC.control


if [ $BUILD_LUMP_PULSAR_SIGPROC == 1 ]; then
    # install SIGPROC
    # http://sigproc.sourceforge.net/
    cd "${MYDIR}"/src
    FILENAME=sigproc-4.3.tar.gz
    BASEPATH=sigproc-4.3
    while [ ! -f "${FILENAME}" ]; do
        cat "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC.txt
        firefox http://sigproc.sourceforge.net/ &
        echo "Move the ${FILENAME} file to the ${MYDIR}/src directory"
        "${EDITOR}" "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC.txt
    done
    tar zxf "${FILENAME}"
    cd "${BASEPATH}"
    codedir=`pwd`
    #"${EDITOR}"
    # give the following values
    echo "set directory for binaries to " "${MYDIR}"/opt/search/bin
    # Ubuntu does not seem to set OSTYPE
    export OSTYPE=`uname -s | tr '[:upper:]' '[:lower:]'`
    echo "${MYDIR}"/opt/search/bin | ./configure
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC_P0.patch
    cd "${BASEPATH}"
    #echo "FFTW libraries in " "${MYFFTWDIR}"/lib64
    #echo "FFTW include in " "${MYFFTWDIR}"/include
    #"${EDITOR}" makefile.linux
    # Change the following values for vlb091
    # BIN=${MYDIR}/opt/search/bin
    # CCC = gcc ${MY_EXTRA_CFLAGS} -fPIC -I${MYFFTWDIR}/include
    # F77=gfortran ${MY_EXTRA_CFLAGS} -fPIC 
    # LINK.f=gfortran ${MY_EXTRA_FFLAGS} -fPIC 
    # LPGPLOT = -L${MYDIR}/src/pgplot -lpgplot -lcpgplot -L/usr/lib/x86_64-linux-gnu -lX11 -lpng
    # LFITS = -L/usr/lib/x86_64-linux-gnu -lcfitsio
    # LFFTW = -L${MYFFTWDIR}/lib -lfftw3 -lfftw3f
    #
    #
    #
    # or for lofarXN,
    # BIN=${MYDIR}/opt/search/bin
    # CCC = gcc ${MY_EXTRA_CFLAGS} -fPIC -I${MYFFTWDIR}/include
    # F77=gfortran ${MY_EXTRA_CFLAGS} -fPIC 
    # LINK.f=gfortran ${MY_EXTRA_FFLAGS} -fPIC 
    # LPGPLOT = -L${MYDIR}/src/pgplot -lpgplot -lcpgplot -L/usr/lib -lX11 -lpng
    # LFITS = -L/usr/lib -lcfitsio
    # LFFTW = -L${MYFFTWDIR}/lib -lfftw3 -lfftw3f
    #
    #
    # "${EDITOR}" makefile
    # # change the export at the end to export2 to get around export being
    # # some keywork to gnu make
    # "${EDITOR}" include.csh
    # # add #include <stdlib.h>
    # #     #include <math.h>
    # #     #include <stdio.h>
    # #     #include <string.h>
    # "${EDITOR}" deg2dms.c chan_freqs.c error_message.c fetch_hdrval.c fshift_prof.c open_file.c phcalc.c print_version.c prof_sumc.c read_aoscan.c read_header.c sizeof_file.c swap_bytes.c whiten.c head_parse.c
    # # add #include "sigproc.h"
    # "${EDITOR}" dosearch.f
    # # edit bad \' at line 265
    # "${EDITOR}" mmzap.f
    # # do loop over integer at line 55
    # "${EDITOR}" pulse.c
    # # at line 62, change NULL to 0
    # "${EDITOR}" seekin.f
    # # fix lines 64, 79, 80
    # # declare f to be integer
    # "${EDITOR}" swap_bytes.c
    # # line 162 pointer cast to 32 bit integer!
    # "${EDITOR}" zap_birdies.f
    # # do loops must be integer for lines 125, 136, 153
    # "${EDITOR}" whiten.c select.c
    # # change select to fselect to avoid name conflict with stdlib.h
    # "${EDITOR}" read_polyco.c
    # # declare poly_override and override_f0 as extern
    # "${EDITOR}" chaninfo.f
    # # continuation line at line 72
    # "${EDITOR}" plotpulses.f
    # # fix line end at line 38
    # # make indices of snhist on line 170 be integer valued for both sides
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
    make
    cd "${MYDIR}"/src
    rm -fr "${BASEPATH}"
fi
echo "BUILD_LUMP_PULSAR_SIGPROC=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC.control

