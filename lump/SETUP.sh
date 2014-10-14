# SETUP.sh for lump_2.3

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


echoerr() { echo "$@" 1>&2; }

if test -z "${LUMPINSTALLDIR}"; then
    echoerr "LUMPINSTALLDIR environment variable not defined"
    exit 1
fi
LUMP_ROOT="${LUMPINSTALLDIR}"/lump
if test -z "$LD_LIBRARY_PATH"; then
    LD_LIBRARY_PATH="${LUMP_ROOT}"/lib
else
    LD_LIBRARY_PATH="${LUMP_ROOT}"/lib:"${LD_LIBRARY_PATH}"
fi
if test -z "$LD_LIBRARY_PATH"; then
    LIBRARY_PATH="${LUMP_ROOT}"/lib
else
    LIBRARY_PATH="${LUMP_ROOT}"/lib:"${LIBRARY_PATH}"
fi
PATH="${LUMP_ROOT}"/bin:"${PATH}"
if test -z "$PYTHONPATH"; then
    PYTHONPATH="${LUMP_ROOT}"/share/python
else
    PYTHONPATH="${LUMP_ROOT}"/share/python:"${PYTHONPATH}"
fi
#
export LUMP_ROOT
export LD_LIBRARY_PATH
export LIBRARY_PATH
export PATH
export PYTHONPATH
