#!/bin/bash --norc
# usage INSTALL_LuMP_LuMP.bat MAKE_OPTIONS
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
export LUMPINSTALLDIRLUMP="${LUMPINSTALLDIR}"/lump
source "${LUMPINSTALLDIRLUMP}"/SETUP.sh
export FFTW3DIR="${LUMPINSTALLDIR}"/fftw/single
export INSTALLDIR="${LUMP_ROOT}"



# Code below to compare version number stolen from
#  http://stackoverflow.com/questions/4023830/bash-how-compare-two-strings-in-version-format
vercomp () {
    if [[ $1 == $2 ]]
    then
        return 0
    fi
    local IFS=.
    local i ver1=($1) ver2=($2)
    # fill empty fields in ver1 with zeros
    for ((i=${#ver1[@]}; i<${#ver2[@]}; i++))
    do
        ver1[i]=0
    done
    for ((i=0; i<${#ver1[@]}; i++))
    do
        if [[ -z ${ver2[i]} ]]
        then
            # fill empty fields in ver2 with zeros
            ver2[i]=0
        fi
        if ((10#${ver1[i]} > 10#${ver2[i]}))
        then
            return 1
        fi
        if ((10#${ver1[i]} < 10#${ver2[i]}))
        then
            return 2
        fi
    done
    return 0
}
testvercomp () {
    vercomp $1 $2
    case $? in
        0) op='=';;
        1) op='>';;
        2) op='<';;
    esac
    if [[ $op != $3 ]]
    then
        echo "FAIL: Expected '$3', Actual '$op', Arg1 '$1', Arg2 '$2'"
    else
        echo "Pass: '$1 $op $2'"
    fi
}


GCC_VER=(`gcc -v 2>&1 | fgrep 'gcc version'`)
vercomp "${GCC_VER[2]}" "4.6.99"
case $? in
    0) export LUMP_CXX_STD=c++0x;;
    1) export LUMP_CXX_STD=gnu++11;;
    2) export LUMP_CXX_STD=c++0x;;
esac
# Note that for gcc 4.6 and below, this should be export LUMP_CXX_STD=c++0x

set -e
set -v

if [ "a${LuMP_COMPILER_MARCH}" = "a" ]; then
    export CC="gcc -std=gnu99 -O3  -march=native"
    export CXX="g++ -std=${LUMP_CXX_STD} -O3  -march=native"
    export F77="gfortran -O3 -march=native"
else
    export CC="gcc -std=gnu99 -O3  ${LuMP_COMPILER_MARCH}"
    export CXX="g++ -std=${LUMP_CXX_STD} -O3  ${LuMP_COMPILER_MARCH}"
    export F77="gfortran -O3 ${LuMP_COMPILER_MARCH}"
fi
MY_MAKE_THREADS="-j `grep -c ^processor /proc/cpuinfo`"

set -o nounset # die on undefined variables



mkdir -p bin include lib share/python


origdir=`pwd`


#install lump version 2.0
cd src/lump
echo doing make ${MY_MAKE_THREADS} "$@"
make ${MY_MAKE_THREADS} "$@"

cd ${origdir}
