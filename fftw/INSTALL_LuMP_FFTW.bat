#!/bin/bash --norc
# usage INSTALL_LuMP_FFTW.bat 
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

source "${LUMPINSTALLDIR}"/SETUP.sh
export FFTW_SINGLE="${LUMPINSTALLDIR}"/fftw/single
export FFTW_MULTI="${LUMPINSTALLDIR}"/fftw/multi
unset CPLUS_INCLUDE_PATH
unset CPATH
unset C_INCLUDE_PATH

# use gcc -march=native -Q --help=target to get native target options
# to help see what arch statement to use when compiling on a different 
# computer.
#
# lofarsrv -march=corei7 -maes -mcx16 -mpclmul -mpopcnt -msahf -msse -msse2 -msse3  -msse4 -msse4.1 -msse4.2 -mssse3 
# lofara1  -march=core2 -mcx16 -msahf -msse -msse2 -msse3 -mssse3 
# lofara4  -march=core2 -mcx16 -msahf -msse -msse2 -msse3 -mssse3 -msse4.1
# lofarb1  -march=corei7 -maes -mcx16 -mpclmul -mpopcnt -msahf -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mssse3
# lofarb2  -march=corei7 -maes -mavx -mcx16 -mpclmul -mpopcnt -msahf -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mssse3
# lofarx   -march=corei7-avx
# vlb091   -march=corei7-avx -maes -mavx -mcx16 -mpclmul -mpopcnt -msahf -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mssse3
#
# Then, prior to running make, set the LuMP_COMPILER_MARCH environment variable
# to the desired architechture statements.  For example, in bash, do
# export LuMP_COMPILER_MARCH="-march=corei7-avx -maes -mavx -mcx16 -mpclmul -mpopcnt -msahf -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mssse3"

set -e
set -v


if [ "a${LuMP_COMPILER_MARCH}" = "a" ]; then
    export CC="gcc -std=gnu99 -O3  -march=native"
    export F77="gfortran -O3 -march=native"
else
    export CC="gcc -std=gnu99 -O3  ${LuMP_COMPILER_MARCH}"
    export F77="gfortran -O3 ${LuMP_COMPILER_MARCH}"
fi
MY_MAKE_THREADS="-j `grep -c ^processor /proc/cpuinfo`"


mkdir -p "${FFTW_SINGLE}"
mkdir -p "${FFTW_MULTI}"

mkdir -p src
cd src

FFTW_VERSION=3.3.4
FFTW_DIR=fftw-"$FFTW_VERSION"
FFTW_TARFILE="$FFTW_DIR.tar.gz"
# Is the current version installed?
if [ -f ""${FFTW_MULTI}"/lib64/libfftw3q.so.${FFTW_VERSION}" ]; then
    exit 0
fi

# do we need to download the tarball?
if [ -f "${FFTW_TARFILE}" ]; then
    true
else
    wget ftp://ftp.fftw.org/pub/fftw/"$FFTW_TARFILE"
fi


# 32 bit single thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-shared --enable-fma --enable-float --enable-sse2 --enable-avx 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 32 bit single thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-fma --enable-float --enable-sse2 --enable-avx 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 32 bit multi thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-shared --enable-fma --enable-float --enable-sse2 --enable-avx --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 32 bit multi thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-fma --enable-float --enable-sse2 --enable-avx --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"


# 64 bit single thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-shared --enable-fma --enable-sse2 --enable-avx 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 64 bit single thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-fma --enable-sse2 --enable-avx 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 64 bit multi thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-shared --enable-fma --enable-sse2 --enable-avx --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 64 bit multi thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-fma --enable-sse2 --enable-avx --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"


# 80 bit single thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-shared --enable-fma --enable-long-double 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 80 bit single thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-fma --enable-long-double 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 80 bit multi thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-shared --enable-fma --enable-long-double --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 80 bit multi thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-fma --enable-long-double --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"


# 128 bit single thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-shared --enable-fma --enable-quad-precision 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 128 bit single thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_SINGLE}" --enable-fma --enable-quad-precision 
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 128 bit multi thread shared
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-shared --enable-fma --enable-quad-precision --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"

# 128 bit multi thread static
tar zxf "$FFTW_TARFILE"
cd "$FFTW_DIR"
./configure --prefix="${FFTW_MULTI}" --enable-fma --enable-quad-precision --enable-threads
make ${MY_MAKE_THREADS}
make ${MY_MAKE_THREADS} install
cd ..
/bin/rm -fr "$FFTW_DIR"
