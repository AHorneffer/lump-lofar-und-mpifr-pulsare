#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_ATLAS.bat
# usage: INSTALL_LuMP_Pulsar_ATLAS.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS.control



if [ $BUILD_LUMP_PULSAR_ATLAS == 1 ]; then
    # install atlas and lapack
    # See http://math-atlas.sourceforge.net/
    # lapack at http://www.netlib.org/lapack/
    # Note: need cpufrequtils package loaded
    SYSTEM=`uname -s`
    . "${LUMPINSTALLDIR}"/cpu/"${SYSTEM}"/cpu_speed.sh
    # then, 
    # ls /sys/devices/system/cpu/cpu0/cpufreq/
    # cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
    # cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
    # cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
    # for athena2
    # 3301000 3300000 3100000 2900000 2700000 2500000 2300000 2100000 1900000 1700000 1600000
    # Then, set to one frequency with
    # sudo cpufreq-set -c N -f 1600000
    # or
    # sudo cpufreq-set -r -u 1600000
    # sudo cpufreq-set -r -d 1600000
    # sudo cpufreq-set -r -g performance
    # This needs to be done for all cpus!!!
    # man cpufreq-set
    # ls /sys/devices/system/cpu/cpu*/cpufreq/
    # on vlb091, cpu frequency settings are
    #3501000 3500000 3200000 3000000 2800000 2600000 2400000 2200000 2000000 1800000 1600000 
    #conservative ondemand userspace powersave performance 
    # sudo cpufreq-set -r -u 3000000
    # sudo cpufreq-set -r -d 3000000
    # sudo cpufreq-set -r -g performance
    cd "${MYDIR}"/src
    FILENAME=atlas3.11.30.tar.bz2
    while [ ! -f "${FILENAME}" ]; do
        cat "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS.txt
        firefox http://math-atlas.sourceforge.net/ &
        echo "Move the ${FILENAME} file to the ${MYDIR}/src directory"
        "${EDITOR}" "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS.txt
    done
    FILENAMELAPACK=lapack-3.5.0.tgz
    if [ ! -f "${FILENAMELAPACK}" ]; then
        wget http://www.netlib.org/lapack/"${FILENAMELAPACK}"
    fi
    bunzip2 -k -c "${FILENAME}" | tar xfm -
    mv ATLAS ATLAS3.11.30
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS_P0.patch
    cd ATLAS3.11.30
    mkdir Linux
    cd Linux
    set_performace_cpu_speed_info
    echo "Limited CPU throttling."
    echo "Enter correct clock rate in configure step below."
    echo "Your current CPU clock rate in units of kilohertz is:"
    cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
    CPUSPEEDKHZ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq`
    CPUSPEEDMHZ=`echo "${CPUSPEEDKHZ}" / 1000 | bc`
    echo "I calculate your CPU speed in megahertz to be" "${CPUSPEEDMHZ}".
    #"${EDITOR}"
    #"${EDITOR}" ../CONFIG/src/backend/archinfo_linux.c
    # In ProbeThrottle(), change 
    # /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq
    # to 
    # /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
    MYGCC=`which gcc`
    MYGFORTRAN=`which gfortran`
    echo "Using compilers C ->" "${MYGCC}" "and FORTRAN ->" "${MYGFORTRAN}"
    echo
    echo
    echo "Check for hyperthreading options to configure."
    echo "Your system's thread_siblings_list is:"
    cat /sys/devices/system/cpu/cpu*/topology/thread_siblings_list
    echo "Enter the ATLAS configuration command to deal with CPU core"
    echo "siblings in the text file in the editor that will be opened now."
    echo "For example, if you have 4 real cores, corresponding to core ids"
    echo "0, 2, 4, and 6, then you would write"
    echo "--force-tids=\"4 0 2 4 6\""
    echo "If you have an AMD system that says the thread siblings are"
    echo "0-1  0-1  2-3  2-3, then you would write"
    echo "--force-tids=\"2 0 2\""
    echo "If there are no core siblings, leave the file blank and exit the editor."
    touch tidlist.txt
    "${EDITOR}" tidlist.txt
    TIDARG=`cat tidlist.txt`
    #echo "check CPU frequency above, and set in configure statement below"
    #"${EDITOR}"
    ../configure --shared -b 64 -D c -DPentiumCPS="${CPUSPEEDMHZ}"  --prefix="${MYDIR}" --with-netlib-lapack-tarfile="${MYDIR}"/src/"${FILENAMELAPACK}" -C acg "${MYGCC}" -C if "${MYGFORTRAN}" -Fa alg "-march=native -O2" -Fa al "${MY_EXTRA_CFLAGS}" "${TIDARG}"
    # -Si archdef 0
    echo "If build fails, then replace all instances of "
    echo "-Wl,\"rpath-link $(*)\""
    echo "with"
    echo "-Wl,-rpath-link,\"$(*)\""
    #"${EDITOR}" lib/Makefile
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS_P1.patch
    cd ATLAS3.11.30/Linux
    make
    echo "If using gcc 4.6, then do not optimize test routines."
    echo "If F77FLAGS macro in Make.inc has -mfpmath=387, then append -O1 to it."
    echo "Make these changes in the file that will be opened now in an editor."
    echo "You are using gcc version"
    gcc --version
    GCC_VER=(`gcc -v 2>&1 | fgrep 'gcc version'`)
    GCC_BAD_FLAG=""
    vercomp "${GCC_VER[2]}" "4.6.99"
    case $? in
        0) GCC_BAD_FLAG="-O1";;
        1) GCC_BAD_FLAG="";;
        2) GCC_BAD_FLAG="LOW";;
    esac
    if [ "${GCC_BAD_FLAG}" == "LOW" ];then
        vercomp "${GCC_VER[2]}" "4.6.0"
        case $? in
            0) GCC_BAD_FLAG="-O1";;
            1) GCC_BAD_FLAG="-O1";;
            2) GCC_BAD_FLAG="";;
        esac
    fi
    export GCC_BAD_FLAG
    "${EDITOR}" Make.inc
    make check
    echo "Check check results"
    "${EDITOR}"
    make ptcheck
    echo "Check ptcheck results"
    "${EDITOR}"
    make time
    echo "Check time results"
    "${EDITOR}"
    make install
    # now install the static version
    cd ..
    /bin/rm -fr Linux
    mkdir Linux
    cd Linux
    ../configure -b 64 -D c -DPentiumCPS="${CPUSPEEDMHZ}"  --prefix="${MYDIR}" --with-netlib-lapack-tarfile="${MYDIR}"/src/"${FILENAMELAPACK}" -C acg "${MYGCC}" -C if "${MYGFORTRAN}" -Fa alg "-march=native -O2" -Fa al "${MY_EXTRA_CFLAGS}" "${TIDARG}"
    # -Si archdef 0
    make
    echo "If using gcc 4.6, then do not optimize test routines."
    echo "If F77FLAGS macro in Make.inc has -mfpmath=387, then append -O1 to it."
    echo "Make these changes in the file that will be opened now in an editor."
    echo "You are using gcc version"
    gcc --version
    "${EDITOR}" Make.inc
    make check
    echo "Check check results"
    "${EDITOR}"
    make ptcheck
    echo "Check ptcheck results"
    "${EDITOR}"
    make time
    echo "Check time results"
    "${EDITOR}"
    make install
    cd ../..
    rm -fr ATLAS3.11.30
    set_original_cpu_speed_info
fi
echo "BUILD_LUMP_PULSAR_ATLAS=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS.control
