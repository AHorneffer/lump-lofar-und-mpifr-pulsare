#!/bin/bash
# MPIfR_FFTW3_default_wisdom.sh
# automate generation of the default FFTW3 wisdom file for LuMP
#_HIST  DATE NAME PLACE INFO
# 2014 Sep 25  James M Anderson  --- GFZ  start



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


set -e
set -v


if [ "a${LUMPINSTALLDIR}" = "a" ]; then
    origdir=`pwd`
    export LUMPINSTALLDIR=`dirname "${origdir}"`
fi
source "${LUMPINSTALLDIR}"/SETUP.sh
export LUMPINSTALLDIRLUMP="${LUMPINSTALLDIR}"/lump
source "${LUMPINSTALLDIRLUMP}"/SETUP.sh

SYSTEM=`uname -s`
. "${LUMPINSTALLDIR}"/cpu/"${SYSTEM}"/cpu_speed.sh

set_performace_cpu_speed_info

MPIfR_FFTW3_default_wisdom.py "$@"

set_original_cpu_speed_info
