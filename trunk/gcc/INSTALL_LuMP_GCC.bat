#!/bin/bash
# usage:
# INSTALL_LuMP_GCC.bat
# 2014 Sep 25  James Anderson  --- document existing install script

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

export LUMPINSTALLDIRGCC="${LUMPINSTALLDIR}"/gcc
source "${LUMPINSTALLDIRGCC}"/INSTALL_LuMP_GCC.control

if [ $BUILD_LUMP_GCC == 1 ]; then
    source "${LUMPINSTALLDIR}"/SETUP.sh

    unset CPLUS_INCLUDE_PATH
    unset CPATH
    unset C_INCLUDE_PATH


    if [ "a${LuMP_COMPILER_MARCH}" = "a" ]; then
        export CFLAGS="-march=native -O3"
        export CXXFLAGS="-march=native -O3"
    else
        export CFLAGS="${LuMP_COMPILER_MARCH}"
        export CXXFLAGS="${LuMP_COMPILER_MARCH}"
    fi

    MY_MAKE_THREADS="-j `grep -c ^processor /proc/cpuinfo`"


    if [ -z {EDITOR:+x} ];then
        export EDITOR=emacs
    fi

    # Set up for CPU speed settings
    SYSTEM=`uname -s`
    . "${LUMPINSTALLDIR}"/cpu/"${SYSTEM}"/cpu_speed.sh
    #source "${LUMPINSTALLDIR}"/gcc/CHECK_LuMP_CPU_SETTINGS.bat


    ISL_NAME=isl-0.12.2
    CLOOG_NAME=cloog-0.18.1
    GCC_NAME=gcc-4.9.1
    GMP_NAME=gmp-6.0.0
    MPFR_NAME=mpfr-3.1.2
    MPC_NAME=mpc-1.0.2


    set -e
    set -v

    cd src




    FILENAME_GCC=gcc-4.9.1.tar.bz2
    if [ ! -f "${FILENAME_GCC}" ]; then
        wget ftp://ftp.mpi-sb.mpg.de/pub/gnu/mirror/gcc.gnu.org/pub/gcc/releases/gcc-4.9.1/"${FILENAME_GCC}"
    fi
    FILENAME_GMP=gmp-6.0.0a.tar.lz
    if [ ! -f "${FILENAME_GMP}" ]; then
        wget https://gmplib.org/download/gmp/"${FILENAME_GMP}"
    fi
    FILENAME_MPFR=mpfr-3.1.2.tar.bz2
    if [ ! -f "${FILENAME_MPFR}" ]; then
        wget http://www.mpfr.org/mpfr-current/"${FILENAME_MPFR}"
    fi
    FILENAME_MPC=mpc-1.0.2.tar.gz
    if [ ! -f "${FILENAME_MPC}" ]; then
        wget ftp://ftp.gnu.org/gnu/mpc/"${FILENAME_MPC}"
    fi
    FILENAME_ISL=isl-0.12.2.tar.bz2
    if [ ! -f "${FILENAME_ISL}" ]; then
        wget ftp://gcc.gnu.org/pub/gcc/infrastructure/"${FILENAME_ISL}"
    fi
    FILENAME_CLOOG=cloog-0.18.1.tar.gz
    if [ ! -f "${FILENAME_CLOOG}" ]; then
        wget ftp://gcc.gnu.org/pub/gcc/infrastructure/"${FILENAME_CLOOG}"
    fi
    FILENAME_ECJ=ecj-latest.jar
    if [ ! -f "${FILENAME_ECJ}" ]; then
        wget ftp://sourceware.org/pub/java/"${FILENAME_ECJ}"
    fi


    # do not use the ISL that comes with cloog (does not work right)
    /bin/rm -fr "${ISL_NAME}"
    bunzip2 -c -k "${ISL_NAME}".tar.bz2 | tar xf -
    cd "${ISL_NAME}"
    ./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-gcc-arch=native
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    cd ..
    /bin/rm -fr "${ISL_NAME}"




    /bin/rm -fr "${CLOOG_NAME}"
    tar zxf "${CLOOG_NAME}".tar.gz
    cd "${CLOOG_NAME}"
    #./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-gmp-prefix="${LUMPINSTALLDIRGCC}" --with-bits=gm --with-gcc-arch=native --with-isl=bundled
    ./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-bits=gmp --with-gcc-arch=native --with-isl=system --with-isl-prefix="${LUMPINSTALLDIRGCC}"
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    cd ..
    /bin/rm -fr "${CLOOG_NAME}"


    # Now build gcc
    /bin/rm -fr "${GCC_NAME}"
    #
    cat "${GCC_NAME}".tar.bz2 > /dev/null
    bunzip2 -c -k "${GCC_NAME}".tar.bz2 | tar xf -
    lzip -c -k -d "${GMP_NAME}"a.tar.lz | tar xf -
    mv "${GMP_NAME}" "${GCC_NAME}"/gmp
    bunzip2 -c -k "${MPFR_NAME}".tar.bz2 | tar xf -
    mv "${MPFR_NAME}" "${GCC_NAME}"/mpfr
    tar zxf "${MPC_NAME}".tar.gz
    mv "${MPC_NAME}" "${GCC_NAME}"/mpc
    cp "${FILENAME_ECJ}" "${GCC_NAME}"/ecj.jar
    #
    /bin/rm -fr build
    mkdir build
    cd build
    ../"${GCC_NAME}"/configure --prefix="${LUMPINSTALLDIRGCC}" --disable-multilib --with-cloog="${LUMPINSTALLDIRGCC}" --with-isl="${LUMPINSTALLDIRGCC}" --with-arch=native
    #
    make BOOT_CFLAGS='-O2' ${MY_MAKE_THREADS} bootstrap
    if [ $BUILD_LUMP_GCC_CHECK == 1 ]; then
        make -k check
    fi
    make install
    cd ..
    /bin/rm -fr build "${GCC_NAME}"



    # now rebuild the libraries with the new compiler






    # gmp
    /bin/rm -fr gmp_build "${GMP_NAME}"
    lzip -c -k -d "${GMP_NAME}"a.tar.lz | tar xf -
    mkdir gmp_build
    cd gmp_build
    ../"${GMP_NAME}"/configure --prefix="${LUMPINSTALLDIRGCC}" --with-readline --enable-cxx
    make ${MY_MAKE_THREADS}
    cd tune
    make tuneup
    set_performace_cpu_speed_info
    ./tuneup >| tuneup.output
    tail -n +5 tuneup.output |> tuneup.output2
    header=`head -n 1 tuneup.output | cut -c 16-`
    echo "Look over the report above from the GMP tuneup program, and"
    echo "compare this against the current file ../${header} ."
    echo "If you wish to use the values computed for your computer,"
    echo "then edit the current file."
    ${EDITOR} tuneup.output2 "../${header}"
    set_original_cpu_speed_info
    /bin/rm -f ../../LuMP_gmp-mparam.h
    cp "../${header}" ../../LuMP_gmp-mparam.h
    cd ..
    make clean
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    #
    cd ..
    /bin/rm -fr "${GMP_NAME}" gmp_build



    # mpfr
    /bin/rm -fr "${MPC_NAME}"
    bunzip2 -c -k "${MPFR_NAME}".tar.bz2 | tar xf -
    cd "${MPFR_NAME}"
    ./configure --prefix="${LUMPINSTALLDIRGCC}" --with-gmp="${LUMPINSTALLDIRGCC}"
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    #
    cd ..
    /bin/rm -fr "${MPFR_NAME}"


    # mpc
    /bin/rm -fr "${MPC_NAME}"
    tar zxf "${MPC_NAME}".tar.gz
    cd "${MPC_NAME}" 
    ./configure --prefix="${LUMPINSTALLDIRGCC}" --with-gmp="${LUMPINSTALLDIRGCC}" --with-mpfr-include="${LUMPINSTALLDIRGCC}"
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    #
    cd ..
    /bin/rm -fr "${MPC_NAME}"










    # ISL
    /bin/rm -fr "${ISL_NAME}"
    bunzip2 -c -k "${ISL_NAME}".tar.bz2 | tar xf -
    cd "${ISL_NAME}"
    ./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-gcc-arch=native --with-gmp=system --with-gmp-prefix="${LUMPINSTALLDIRGCC}"
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    #
    cd ..
    /bin/rm -fr "${ISL_NAME}"




    /bin/rm -fr "${CLOOG_NAME}"
    tar zxf "${CLOOG_NAME}".tar.gz
    cd "${CLOOG_NAME}"
    #./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-gmp-prefix="${LUMPINSTALLDIRGCC}" --with-bits=gm --with-gcc-arch=native --with-isl=bundled
    ./configure  --prefix="${LUMPINSTALLDIRGCC}" --with-gmp-prefix="${LUMPINSTALLDIRGCC}" --with-bits=gmp --with-gcc-arch=native --with-isl=system --with-isl-prefix="${LUMPINSTALLDIRGCC}"
    make ${MY_MAKE_THREADS}
    make ${MY_MAKE_THREADS} check
    make install
    #
    cd ..
    /bin/rm -fr "${CLOOG_NAME}"



    # Now rebuild gcc
    /bin/rm -fr "${GCC_NAME}"
    #
    cat "${GCC_NAME}".tar.bz2 > /dev/null
    bunzip2 -c -k "${GCC_NAME}".tar.bz2 | tar xf -
    lzip -c -k -d "${GMP_NAME}"a.tar.lz | tar xf -
    mv "${GMP_NAME}" "${GCC_NAME}"/gmp
    bunzip2 -c -k "${MPFR_NAME}".tar.bz2 | tar xf -
    mv "${MPFR_NAME}" "${GCC_NAME}"/mpfr
    tar zxf "${MPC_NAME}".tar.gz
    mv "${MPC_NAME}" "${GCC_NAME}"/mpc
    cp "${FILENAME_ECJ}" "${GCC_NAME}"/ecj.jar
    /bin/rm -fr build
    mkdir build
    cd build
    #../"${GCC_NAME}"/configure --prefix="${LUMPINSTALLDIRGCC}" --disable-multilib --with-cloog="${LUMPINSTALLDIRGCC}" --with-isl="${LUMPINSTALLDIRGCC}" --with-mpc="${LUMPINSTALLDIRGCC}" --with-mpfr="${LUMPINSTALLDIRGCC}" --with-gmp="${LUMPINSTALLDIRGCC}" --with-arch=native
    ../"${GCC_NAME}"/configure --prefix="${LUMPINSTALLDIRGCC}" --disable-multilib --with-cloog="${LUMPINSTALLDIRGCC}" --with-isl="${LUMPINSTALLDIRGCC}" --with-arch=native
    # Copy our hand-edited version of the GMP parameters over.
    headerloc="${header#*/}"
    headerloc="${headerloc#*/}"
    /bin/cp -f ../LuMP_gmp-mparam.h ../"${GCC_NAME}"/gmp/"${headerloc}"
    make BOOT_CFLAGS='-O2' ${MY_MAKE_THREADS} bootstrap
    if [ $BUILD_LUMP_GCC_CHECK == 1 ]; then
        make -k check
    fi
    make install
    #
    cd ..
    /bin/rm -fr build "${GCC_NAME}"



    cd ..
    # set CPU speed back to normal
    set_original_cpu_speed_info
fi
echo "BUILD_LUMP_GCC=0" >| "${LUMPINSTALLDIRGCC}"/INSTALL_LuMP_GCC.control
echo "BUILD_LUMP_GCC_CHECK=${BUILD_LUMP_GCC_CHECK}" >> "${LUMPINSTALLDIRGCC}"/INSTALL_LuMP_GCC.control
