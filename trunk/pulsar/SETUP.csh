# SETUP.sh
# extra setup environment variables for pulsar stuff
# 2014 Sep 25  James Anderson  --- document existing Python script and extend

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


# Setup.csh for pulsar stuff for lump_2.3
setenv LUMPINSTALLDIRPULSAR {$LUMPINSTALLDIR}/pulsar/pulsar_local
setenv LUMPPULSARFFTW3DIR {$LUMPINSTALLDIR}/fftw/multi
setenv LD_LIBRARY_PATH {$LUMPPULSARFFTW3DIR}/lib:{$LD_LIBRARY_PATH}
setenv LIBRARY_PATH {$LUMPPULSARFFTW3DIR}/lib:{$LIBRARY_PATH}
# note that some pulsar software requires the / at the end of ${PSRHOME}
setenv PSRHOME {$LUMPINSTALLDIRPULSAR}
setenv LD_LIBRARY_PATH {$LUMPINSTALLDIRPULSAR}/lib64:{$LUMPINSTALLDIRPULSAR}/lib:{$LUMPINSTALLDIRPULSAR}/src/pgplot:{$LD_LIBRARY_PATH}
setenv LIBRARY_PATH {$LUMPINSTALLDIRPULSAR}/lib64:{$LUMPINSTALLDIRPULSAR}/lib:{$LUMPINSTALLDIRPULSAR}/src/pgplot:{$LIBRARY_PATH}
setenv PGPLOT_DIR {$LUMPINSTALLDIRPULSAR}/src/pgplot
setenv PGPLOT_DEV /xwin
setenv TEMPO {$LUMPINSTALLDIRPULSAR}/src/tempo
setenv TEMPO2 {$LUMPINSTALLDIRPULSAR}/share/tempo2/
setenv PATH {$LUMPINSTALLDIRPULSAR}/bin:{$PATH}
setenv PSRCAT_FILE {$LUMPINSTALLDIRPULSAR}/share/psrcat/psrcat.db
#
if ($?USE_LUMP_PULSAR_SEARCH) then
    setenv PRESTO {$LUMPINSTALLDIRPULSAR}/src/presto
    setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:{$LUMPINSTALLDIRPULSAR}/opt/search/lib
    setenv LIBRARY_PATH {$LIBRARY_PATH}:{$LUMPINSTALLDIRPULSAR}/opt/search/lib
    setenv PATH {$PATH}:{$LUMPINSTALLDIRPULSAR}/opt/search/bin
    setenv PULSARHUNTER_HOME {$LUMPINSTALLDIRPULSAR}/opt/search/pulsarhunter
    setenv PATH {$PATH}:{$PULSARHUNTER_HOME}/scripts
else
    # do nothing
endif


