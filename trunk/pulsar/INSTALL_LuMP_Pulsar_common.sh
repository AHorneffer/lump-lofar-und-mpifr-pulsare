# INSTALL_LuMP_Pulsar_common.sh
# usage: source from some other file
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

source "${LUMPINSTALLDIR}"/SETUP.sh
export LUMPINSTALLDIRPULSARMAIN="${LUMPINSTALLDIR}"/pulsar
source "${LUMPINSTALLDIRPULSARMAIN}"/SETUP.sh
export MYDIR="${LUMPINSTALLDIRPULSARMAIN}"/pulsar_local/
export MYDIR2="${LUMPINSTALLDIRPULSARMAIN}"/pulsar_local
export MYFFTWDIR="${LUMPINSTALLDIR}"/fftw/multi
export LUMPINSTALLDIRGCC="${LUMPINSTALLDIR}"/gcc
origdir=`pwd`
unset CPLUS_INCLUDE_PATH
unset CPATH
unset C_INCLUDE_PATH



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


if [ "a${LuMP_COMPILER_MARCH}" = "a" ]; then
    export MY_EXTRA_CFLAGS="-O3  -march=native"
    export MY_EXTRA_CXXFLAGS="-O3  -march=native"
    export MY_EXTRA_FFLAGS="-O3 -march=native"
else
    export MY_EXTRA_CFLAGS="-O3  ${LuMP_COMPILER_MARCH}"
    export MY_EXTRA_CXXFLAGS="-O3  ${LuMP_COMPILER_MARCH}"
    export MY_EXTRA_FFLAGS="-O3 ${LuMP_COMPILER_MARCH}"
fi

MY_MAKE_THREADS="-j `grep -c ^processor /proc/cpuinfo`"

export EDITOR="${EDITOR:=emacs}"


set -o nounset # die on undefined variables

#set -e
#set -v

echo "Warning: set -e turned off"

