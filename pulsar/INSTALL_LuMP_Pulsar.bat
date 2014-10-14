#!/bin/bash


# 2014 Sep 25  James Anderson  --- document existing script and extend

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


# needs FFTW
# should also install own blas (atlas?) and LAPACK
# CFITSIO from http://heasarc.gsfc.nasa.gov/fitsio/
#         used version 3370
# SOFA fortran library at http://www.iausofa.org/
#         used version sofa_f-20131202_c
# PGPLOT from http://www.astro.caltech.edu/~tjp/pgplot/
#         used version pgplot5.2.tar.gz
# tempo at http://sourceforge.net/projects/tempo/
#       see also http://tempo.sourceforge.net/
#       using git repository, last known update 2014-04-30
# atlas at http://math-atlas.sourceforge.net/
#       using version 3.11.30
# lapack at http://www.netlib.org/lapack/ 
#       using version 3.5.0
# tempo2 at http://sourceforge.net/projects/tempo2/
#       using version tempo2-2013.9.1.tar.gz
# psrcat at http://www.atnf.csiro.au/people/pulsar/psrcat/
#       using version 1.50
# psrxml at http://www.pulsarastronomy.net/wiki/Software/PsrXML
#       using version 1.05
# psrchive at http://psrchive.sourceforge.net/   use git
#       using git repository, last working update was 2014-09-26
# psrdada at http://psrdada.sourceforge.net/
#       using cvs repository, last working update was 2014-09-26
# WCS from http://www.atnf.csiro.au/people/mcalabre/WCS/
#       using version 4.24
# casacore from https://code.google.com/p/casacore/
#          note that the data stuff needs to be downloaded separately
#          from ftp://ftp.atnf.csiro.au/pub/software/asap/data/
#          ftp://ftp.atnf.csiro.au/pub/software/asap/data/asap_data.tar.bz2
#          casacore development version at
#          svn checkout http://casacore.googlecode.com/svn/trunk/ casacore-read-only
#       using svn version, last working update was 2014-09-26
# DAL from http://www.lofar.org/wiki/doku.php?id=public:user_software:dal
#     used git clone https://github.com/nextgen-astrodata/DAL.git DAL
#       using git version, last working update was 2014-09-26
# PRESTO from http://www.cv.nrao.edu/~sransom/presto/
#       using git version, last working update was 2014-09-26
# SIGPROC from http://sigproc.sourceforge.net/
#       using version 4.3
# PulsarHunter from http://www.pulsarastronomy.net/wiki/Software/PulsarHunter
# see also software at http://www.pulsarastronomy.net/wiki/Software/Software
#       using version pulsarhunter_v1.3r63.tar.gz

# dspsr at http://dspsr.sourceforge.net/index.shtml
#       for developing dspsr, do
#       git clone ssh://USERNAME@git.code.sf.net/p/dspsr/code dspsr
#       for just downloading, do
#       git clone git://git.code.sf.net/p/dspsr/code dspsr
#       using git version, last working update was 2014-09-26


if [ "a${LUMPINSTALLDIR}" = "a" ]; then
    origdir=`pwd`
    export LUMPINSTALLDIR=`dirname "${origdir}"`
fi

source "${LUMPINSTALLDIR}"/pulsar/INSTALL_LuMP_Pulsar.control
source "${LUMPINSTALLDIR}"/pulsar/INSTALL_LuMP_Pulsar_common.sh


mkdir -p pulsar_local
cd pulsar_local
mkdir -p bin  include  lib  lib64  share  src  web opt/search/bin opt/search/lib
cd ..







source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CFITSIO.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SOFA.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PGPLOT.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_ATLAS.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_TEMPO2.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCAT.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRXML.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRCHIVE.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PSRDADA.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_WCSLIB.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_CASACORE.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_LOFARDAL.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PRESTO.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_SIGPROC.bat
#source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PULSARHUNTER.bat
source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_DSPSR.bat


