# INSTALL_LuMP_Pulsar_DSPSR_P0.patch.py
# fix the link order for a Makefile in dspsr

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

import os

FILENAME_IN  = "dspsr/Kernel/Applications/Makefile"
FILENAME_OUT = "dspsr/Kernel/Applications/Makefile.lump"

fp_in  = open(FILENAME_IN, "r")
fp_out = open(FILENAME_OUT, "w")

for line in fp_in:
    if(line.startswith("INCLUDE_LDFLAGS =")):
        lcopy = line[:-1] + " -lpsrmore -lpsrutil"
        Lpos = line.rfind(" -L")
        if(Lpos > 0):
            lpos = line.find(" -l", Lpos)
            lcopy = lcopy + line[lpos:]
        else:
            lcopy = lcopy + '\n'
        fp_out.write(lcopy)
    else:
        fp_out.write(line)
fp_in.close()
fp_out.close()
os.rename(FILENAME_IN, FILENAME_OUT)
