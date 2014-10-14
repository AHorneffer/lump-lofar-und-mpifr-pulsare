#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PSRXML.bat
# usage: INSTALL_LuMP_Pulsar_PSRXML.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML.control


if [ $BUILD_LUMP_PULSAR_PSRXML == 1 ]; then
    # install psrxml
    # http://www.pulsarastronomy.net/wiki/Software/PsrXML
    cd "${MYDIR}"/src
    FILENAME=psrxml-master.zip
    while [ ! -f "${FILENAME}" ]; do
        cat "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML.txt
        firefox http://github.com/SixByNine/psrxml/tree/master &
        echo "Move the ${FILENAME} file to the ${MYDIR}/src directory"
        "${EDITOR}" "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML.txt
    done
    mkdir psrxml
    cd psrxml
    unzip ../"${FILENAME}"
    cd ..
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML_P0.patch
    cd psrxml/psrxml-master
    if [ -f /usr/share/aclocal/libtool.m4 ];then
        cp /usr/share/aclocal/libtool.m4 ./acinclude.m4
    elif [ -f /usr/share/libtool/libtool.m4 ];then
        cp /usr/share/libtool/libtool.m4 ./acinclude.m4
    fi
    if [ -f /usr/share/automake-*/missing ];then
        cp /usr/share/automake-*/missing ./autoconf
    fi
    /bin/rm autoconf/libtool.m4 autoconf/libxml.m4 autoconf/ltoptions.m4 autoconf/ltsugar.m4 autoconf/ltversion.m4
    aclocal
    automake --add-missing --copy
    set +o nounset
    libtoolize  --copy
    set -o nounset
    cat autoconf/libtool.m4 >> aclocal.m4
    autoconf
    autoheader
    ./configure --prefix="${MYDIR}" --enable-shared CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC"  
    #echo "change @OPENSSL_LIBS@ to -lcrypt -lssl   at line 191"
    #"${EDITOR}" src/tools/Makefile
    #echo "add #include <stdlib.h> and #include <string.h> to config.h"
    #"${EDITOR}" config.h
    echo -e '\n#include <stdlib.h>\n#include <string.h>\n' >> config.h
    # add #include "unpack_lookup.h"
    #"${EDITOR}" src/psrxml/psrxml_unpackers.c
    # add #include "config.h"
    #"${EDITOR}" src/psrxml/psrxml.h
    #/bin/mv src/psrxml/psrxml.h src/psrxml/psrxml.h.orig
    #echo -e '#include "config.h"\n' > src/psrxml/psrxml.h
    #cat src/psrxml/psrxml.h.orig >> src/psrxml/psrxml.h
    make
    make install
    # remove #include "config.h"
    #"${EDITOR}" src/psrxml/psrxml.h
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML_P1.patch
    /bin/rm "${MYDIR}"/include/psrxml.h
    /bin/cp psrxml/psrxml-master/src/psrxml/psrxml.h "${MYDIR}"/include/psrxml.h
    rm -fr psrxml
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_PSRXML=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML.control
