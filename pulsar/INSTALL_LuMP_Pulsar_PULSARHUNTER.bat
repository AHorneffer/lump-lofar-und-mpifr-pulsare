#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PULSARHUNTER.bat
# usage: INSTALL_LuMP_Pulsar_PULSARHUNTER.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER.control


if [ $BUILD_LUMP_PULSAR_PULSARHUNTER == 1 ]; then
    # install PulsarHunter
    # http://www.pulsarastronomy.net/wiki/Software/PulsarHunter
    # see also software at http://www.pulsarastronomy.net/wiki/Software/Software
    cd "${MYDIR}"/src
    FILENAME=pulsarhunter_v1.3r63.tar.gz
    BASEPATH=pulsarhunter
    while [ ! -f "${FILENAME}" ]; do
        cat "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER.txt
        firefox http://sourceforge.net/projects/pulsarhunter/files/ &
        echo "Move the ${FILENAME} file to the ${MYDIR}/src directory"
        "${EDITOR}" "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER.txt
    done
    FILENAME2=pulsarhunter20141013.tar.gz
    if [ ! -f "${FILENAME2}" ]; then
        echo "When prompted for a password, just hit Enter."
        cvs -d:pserver:anonymous@pulsarhunter.cvs.sourceforge.net:/cvsroot/pulsarhunter login
        cvs -z3 -d:pserver:anonymous@pulsarhunter.cvs.sourceforge.net:/cvsroot/pulsarhunter co -P "${BASEPATH}"
        tar zcf "${FILENAME2}" ./"${BASEPATH}"
        /bin/rm -fr ./"${BASEPATH}"
    fi
    cd "${MYDIR}"/opt/search
    mkdir -p "${BASEPATH}"
    cd "${BASEPATH}"
    tar zxf "${MYDIR}"/src/"${FILENAME}"
    cd ..
    tar zxf "${MYDIR}"/src/"${FILENAME2}"
    cd "${BASEPATH}"
    /bin/rm -f pulsarhunter.jar
    #cd "${MYDIR}"/opt/search
    #patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER_P0.patch
    #cd "${BASEPATH}"
    #echo "gcc installation at ${LUMPINSTALLDIR}/gcc"
    mkdir -p scripts/includes
    #"${EDITOR}" scripts/includes/env
    # add location of java software home area to the script,
    # if it does not already exist in the standard user environment.
    # It should have ${JAVA_HOME}/bin/java
    # export JAVA_HOME=???
    # export JAVA_HOME=/usr
    # export JAVA_HOME=${LUMPINSTALLDIR}/gcc
    echo -e "JAVA_HOME=\"${JAVA_HOME}\"\n" > scripts/includes/env
    ln -s lib lib64
    ant jar
    ln -s dist/pulasrhunter.jar ./pulsarhunter.jar
    cd native
    #echo "gcc java topdir at ${LUMPINSTALLDIR}/gcc/lib/gcc/x86_64-unknown-linux-gnu/4.8.1"
    #echo "gcc java include directory at ${LUMPINSTALLDIR}/gcc/lib/gcc/x86_64-unknown-linux-gnu/4.8.1/include"
    #"${EDITOR}" configure
    # at line 19831, change to JAVAC=gcj
    # change line 19941 to if { ac_try='$JAVAC $JAVACFLAGS $JAVA_TEST --main=Test'
    # comment out line 19964    JNI_INCLUDE_DIRS=""
    # change line 20040 to _JTOPDIR="${LUMPINSTALLDIR}/gcc/lib/gcc/x86_64-unknown-linux-gnu/4.8.1" 
    #
    #
    #
    #
    MYGCC=`which gcc`
    MYGCCHOME=`dirname "${MYGCC}"`
    MYGCCHOME=`dirname "${MYGCCHOME}"`
    MGCJINC=`ls -d "${MYGCCHOME}"/lib*/gcc/*/*/include`
    MGCJINC=`dirname "${MGCJINC}"`
    # echo "for some reason, gcc fails to install an ecj1 script"
    # echo "put the following into this file"
    if [ -f "${LUMPINSTALLDIR}"/gcc/bin/gcc ];then
        if [ -f "${LUMPINSTALLDIR}"/gcc/bin/ecj1 ];then
           true
        else
            echo -e '#!/bin/sh\n\ngij' " --classpath ${MGCJINC}" '\\\n   org.eclipse.jdt.internal.compiler.batch.GCCMain \\\n   ${1+"$@"}' > "${LUMPINSTALLDIR}"/gcc/bin/ecj1
            chmod ugo+rx "${LUMPINSTALLDIR}"/gcc/bin/ecj1
            cp "${LUMPINSTALLDIR}"/gcc/src/ecj-latest.jar "${MGCJINC}"/ecj.jar
        fi
        export JAVA_HOME=${LUMPINSTALLDIR}/gcc
    fi
    # echo -e '#!/bin/sh\n\ngij' " --classpath ${MGCJINC}" '\\\n   org.eclipse.jdt.internal.compiler.batch.GCCMain \\\n   ${1+"$@"}'
    # "${EDITOR}" ${LUMPINSTALLDIR}/gcc/bin/ecj1
    # chmod ugo+rx ${LUMPINSTALLDIR}/gcc/bin/ecj1
    # echo "then do "
    # echo "cp ${LUMPINSTALLDIR}/gcc/src/ecj-latest.jar ${MGCJINC}/ecj.jar"
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
    #./configure --prefix="${MYDIR}"/opt/search/pulsarhunter --with-x --with-pgplot-extra="-lpng" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L${TERMCAPLIBDIR}" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC" JAVAC="gcj -C" JNI_INCLUDE_DIRS="${MGCJINC}/include"
    ./configure --prefix="${MYDIR}"/opt/search/pulsarhunter --with-x --with-pgplot-extra="-lpng" CC=gcc CFLAGS="${MY_EXTRA_CFLAGS} -fPIC" F77=gfortran FFLAGS="${MY_EXTRA_FFLAGS} -fPIC" LDFLAGS="-L${MYDIR}/lib64 -L${MYDIR}/lib -L${TERMCAPLIBDIR}" CXX=g++ CXXFLAGS="${MY_EXTRA_CXXFLAGS} -fPIC" FC=gfortran FCFLAGS="${MY_EXTRA_FFLAGS} -fPIC"
    make
    make install
    make clean
    cd "${origdir}"
    #"${EDITOR}" SETUP.sh
    # add export PULSARHUNTER_HOME=${MYDIR}/opt/search/pulsarhunter
    #     export PATH=${PATH}:${PULSARHUNTER_HOME}/scripts
    pulsarhunter -r test
    "${EDITOR}"
    # check results
fi
echo "BUILD_LUMP_PULSAR_PULSARHUNTER=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER.control

