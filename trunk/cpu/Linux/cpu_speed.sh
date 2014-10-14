#
# cpu_speed.sh
# bash shell script stuff to be loaded to help with setting CPU speed
################################################################################
#_HIST  DATE NAME PLACE INFO
# 2014 Sep 24  James Anderson  --- GFZ start from past cpufreq use 

# Copyright (c) 2014, James M. Anderson <anderson@gfz-potsdam.de>

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




################################################################################
#
# Usage:
#
# In your bash scprit, do:
# . SOME_PATH_TO/cpu_speed.sh
# # some of your code goes here
# set_performace_cpu_speed_info
# # your code that needs constant high performance goes here
# set_original_cpu_speed_info
# # back to original performance, continue with more code
# # when you exit (even after a set -e and a program fails),
# # the cleanup function above is called to make
# # sure the performance is what it should be by default.
#
################################################################################







if [ -z ${CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE:+x} ]; then
    CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE="N"
fi

function get_original_cpu_speed_info {
    if [ "${CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE}" == "N" ]; then
        echo "Gathering information about the current CPU speed settings"
        CPU_SPEED_INFO_ORIGINAL_MAX_FREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq`
        CPU_SPEED_INFO_ORIGINAL_MIN_FREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq`
        CPU_SPEED_INFO_ORIGINAL_GOVERNOR=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
        CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE="Y"
    fi
    echo "Found MAX_FREQ=${CPU_SPEED_INFO_ORIGINAL_MAX_FREQ}, MIN_FREQ=${CPU_SPEED_INFO_ORIGINAL_MIN_FREQ}, GOVERNOR=${CPU_SPEED_INFO_ORIGINAL_GOVERNOR}"
}

# make sure we always have *some* original values to fall back upon
get_original_cpu_speed_info

contains_element () {
    local e
    for e in "${@:2}"; do [[ "${e}" == "${1}" ]] && return 0; done
    return 1
}

function set_performace_cpu_speed_info {
    # check the current values
    get_original_cpu_speed_info
    echo "Setting the CPU speed settings to high performance"
    if [ "${CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE}" == "Y" ]; then
        CPU_SPEED_INFO_PERFORMANCE_GOVERNOR="performance"
        CPU_SPEED_INFO_AVAILABLE_GOVERNORS=(`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors`)
        if contains_element "${CPU_SPEED_INFO_PERFORMANCE_GOVERNOR}" "${CPU_SPEED_INFO_AVAILABLE_GOVERNORS[@]}"; then
            echo "Setting the speed governor to ${CPU_SPEED_INFO_PERFORMANCE_GOVERNOR}"
             sudo cpufreq-set -r -g "${CPU_SPEED_INFO_PERFORMANCE_GOVERNOR}"
        else
            echo "The governor \'${CPU_SPEED_INFO_PERFORMANCE_GOVERNOR}\' is not in the list of available governors (${CPU_SPEED_INFO_AVAILABLE_GOVERNORS[@]}) --- will not change the governor"
        fi
        echo "Setting MIN_FREQ to ${CPU_SPEED_INFO_ORIGINAL_MAX_FREQ}"
        sudo cpufreq-set -r -d "${CPU_SPEED_INFO_ORIGINAL_MAX_FREQ}"
        CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE="H"
    else
        echo "Already at high performance settings"
    fi
        echo "Done setting speed settings to high performance."
}
    



function set_original_cpu_speed_info {
    echo "Setting the CPU speed settings to their original values"
    if [ "${CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE}" == "H" ]; then
        echo "Setting the speed governor to ${CPU_SPEED_INFO_ORIGINAL_GOVERNOR}"
        sudo cpufreq-set -r -g "${CPU_SPEED_INFO_ORIGINAL_GOVERNOR}"
        echo "Setting MIN_FREQ to ${CPU_SPEED_INFO_ORIGINAL_MIN_FREQ}"
        sudo cpufreq-set -r -d "${CPU_SPEED_INFO_ORIGINAL_MIN_FREQ}"
        CPU_SPEED_INFO_AT_ORIGINAL_PERFORMANCE="Y"
    else
        echo "Already at original performance settings"
    fi
    echo "Done setting speed settings to their original values."
}
    

function set_cpu_speed_cleanup {
    echo "CPU speed settings cleanup function..."
    set_original_cpu_speed_info
    echo "Done with CPU speed setting cleanup."
}

trap set_cpu_speed_cleanup EXIT
