# SETUP.csh for lump_2.3

# Copyright (c) 2014 James M. Anderson <anderson@gfz-potsdam.de>

# Copyright 2014,  James M. Anderson <anderson@gfz-potsdam.de>

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

setenv LUMP_ROOT {$LUMPINSTALLDIR}/lump
setenv LD_LIBRARY_PATH {$LUMP_ROOT}/lib:{$LD_LIBRARY_PATH}
setenv LIBRARY_PATH {$LUMP_ROOT}/lib:{$LIBRARY_PATH}
setenv PATH {$LUMP_ROOT}/bin:{$PATH}
if ($?PYTHONPATH) then
    setenv PYTHONPATH {$LUMP_ROOT}/share/python:{$PYTHONPATH}
else
    setenv PYTHONPATH {$LUMP_ROOT}/share/python
endif
#
