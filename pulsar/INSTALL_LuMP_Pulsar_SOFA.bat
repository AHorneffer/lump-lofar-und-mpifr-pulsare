#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_SOFA.bat
# usage: INSTALL_LuMP_Pulsar_SOFA.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SOFA.control

if [ $BUILD_LUMP_PULSAR_SOFA == 1 ]; then
    # install FORTRAN SOFA
    # SOFA fortran library at http://www.iausofa.org/
    cd "${MYDIR}"/src
    FILENAME="sofa_f-20131202_c.tar.gz"
    if [ ! -f "${FILENAME}" ]; then
        wget http://www.iausofa.org/2013_1202_F/"${FILENAME}"
    fi
    tar zxf "${FILENAME}"
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SOFA_P0.patch
    cd sofa/20131202_c/f77/src
    #"${EDITOR}" makefile
    # make edits
    # suggest changes as follows
    # INSTALL_DIR = ${MYDIR}
    # FC = gfortran
    # FX = ${MY_EXTRA_FFLAGS} -Wall -fPIC
    # 
    # to make a patch file, do
    # cp -r orig_dir new_dir
    # edit new_dir
    # diff -Naur orig_dir/ new_dir/ > original.patch
    #
    # then, to patch, do
    # patch -Np0 < original.patch
    make ${MY_MAKE_THREADS}
    make test
    "${EDITOR}"
    # check results
    # You should see
    # gfortran ${MY_EXTRA_CFLAGS} -Wall t_sofa_f.for -L/home/anderson/work/prog/C++/Eff_recording/pulsar/pulsar_local/lib/ -lsofa -o t_sofa_f
    # ./t_sofa_f
    #  T_SOFA_F validation successful
    a=`./t_sofa_f`
    if [ "${a"} == " T_SOFA_F validation successful" ];then
        true
    else
        echo "SOFA test failed"
        false
    fi
    make ${MY_MAKE_THREADS} install
    # rm -f t_sofa_f
    cd ../../../..
    /bin/rm -fr sofa
    cd "${origdir}"
fi
echo "BUILD_LUMP_PULSAR_SOFA=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SOFA.control
