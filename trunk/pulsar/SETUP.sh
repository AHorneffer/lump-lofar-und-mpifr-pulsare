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




# Setup.sh for pulsar stuff for lump_2.3
LUMPINSTALLDIRPULSAR="${LUMPINSTALLDIR}"/pulsar/pulsar_local
LUMPPULSARFFTW3DIR="${LUMPINSTALLDIR}"/fftw/multi
LD_LIBRARY_PATH="${LUMPPULSARFFTW3DIR}"/lib:"${LD_LIBRARY_PATH}"
LIBRARY_PATH="${LUMPPULSARFFTW3DIR}"/lib:"${LIBRARY_PATH}"
# note that some pulsar software requires the / at the end of ${PSRHOME}
PSRHOME="${LUMPINSTALLDIRPULSAR}"
LD_LIBRARY_PATH="${LUMPINSTALLDIRPULSAR}"/lib64:"${LUMPINSTALLDIRPULSAR}"/lib:"${LUMPINSTALLDIRPULSAR}"/src/pgplot:"${LD_LIBRARY_PATH}"
LIBRARY_PATH="${LUMPINSTALLDIRPULSAR}"/lib64:"${LUMPINSTALLDIRPULSAR}"/lib:"${LUMPINSTALLDIRPULSAR}"/src/pgplot:"${LIBRARY_PATH}"
PGPLOT_DIR="${LUMPINSTALLDIRPULSAR}"/src/pgplot
PGPLOT_DEV=/xwin
TEMPO="${LUMPINSTALLDIRPULSAR}"/src/tempo
TEMPO2="${LUMPINSTALLDIRPULSAR}"/share/tempo2/
PATH="${LUMPINSTALLDIRPULSAR}"/bin:"${PATH}"
PSRCAT_FILE="${LUMPINSTALLDIRPULSAR}"/share/psrcat/psrcat.db
#
export LUMPINSTALLDIRPULSAR
export LUMPPULSARFFTW3DIR
export LD_LIBRARY_PATH
export LIBRARY_PATH
export PSRHOME
export LD_LIBRARY_PATH
export PGPLOT_DIR
export PGPLOT_DEV
export TEMPO
export TEMPO2
export PATH
export PSRCAT_FILE


if [ "a$USE_LUMP_PULSAR_SEARCH" = "a" ]; then
    true
else
    PRESTO="${LUMPINSTALLDIRPULSAR}"/src/presto
    LD_LIBRARY_PATH="${LD_LIBRARY_PATH}":"${LUMPINSTALLDIRPULSAR}"/opt/search/lib
    LIBRARY_PATH="${LIBRARY_PATH}":"${LUMPINSTALLDIRPULSAR}"/opt/search/lib
    PATH="${PATH}":"${LUMPINSTALLDIRPULSAR}"/opt/search/bin
    PULSARHUNTER_HOME="${LUMPINSTALLDIRPULSAR}"/opt/search/pulsarhunter
    PATH="${PATH}":"${PULSARHUNTER_HOME}"/scripts
    #
    export PRESTO
    export LD_LIBRARY_PATH
    export LIBRARY_PATH
    export PATH
    export PULSARHUNTER_HOME
fi

