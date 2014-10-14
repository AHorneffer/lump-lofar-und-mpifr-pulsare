#!/bin/bash --norc
# INSTALL_LuMP_Pulsar_PGPLOT.bat
# usage: INSTALL_LuMP_Pulsar_PGPLOT.bat
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

source "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PGPLOT.control

if [ $BUILD_LUMP_PULSAR_PGPLOT == 1 ]; then
    # install PGPLOT
    # from http://www.astro.caltech.edu/~tjp/pgplot/
    # see also http://www.dur.ac.uk/physics.astrolab/ppgplot.html
    cd "${MYDIR}"/src
    FILENAME=pgplot5.2.tar.gz
    if [ ! -f "${FILENAME}" ]; then
        wget ftp://ftp.astro.caltech.edu/pub/pgplot/"${FILENAME}"
    fi
    tar zxf "${FILENAME}"
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PGPLOT_P0.patch
    cd pgplot
    #"${EDITOR}" drivers.list
    # uncomment the device drivers you want installed
    # I have LATEX FILE NULL PGMF PNG TPNG PS VPS CPS VCPS
    #        XWINDOW XSERVE XMOTIF XATHENA XTK
    # psrchive wants the QT part
    #cp sys_linux/g77_gcc.conf sys_linux/gfortran_gcc.conf
    #"${EDITOR}" sys_linux/gfortran_gcc.conf
    # change the following
    # TK_INCL="-I/usr/include/tk -I/usr/include/tcl $XINCL"
    # The GNU gfortran FORTRAN compiler and Gnu gcc C compiler on an elf-system.
    # FCOMPL="gfortran"
    # FFLAGC="-u -Wall -fPIC ${MY_EXTRA_CFLAGS} -fno-range-check"
    # FFLAGD="-fno-backslash -fPIC ${MY_EXTRA_CFLAGS} -fno-range-check"
    # CFLAGC="-Wall -fPIC -DPG_PPU ${MY_EXTRA_CFLAGS}"
    # CFLAGD="-Wall -fPIC ${MY_EXTRA_CFLAGS}"
    mdir=`pwd`
    ./makemake "${mdir}" linux gfortran_gcc
    cd "${MYDIR}"/src
    patch -Np0 < "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PGPLOT_P1.patch
    cd pgplot
    # "${EDITOR}" drivers/mfdriv.f
    #cp "${origdir}"/pgplot5.2_mfdriv.f drivers/mfdriv.f
    # athena2:pulsar[14]% diff -bc pulsar_local_20130701/src/pgplot/drivers/mfdriv.f pulsar_local/src/pgplot/drivers/mfdriv.f
    # *** pulsar_local_20130701/src/pgplot/drivers/mfdriv.f   Sun Mar 24 11:20:52 2013
    # --- pulsar_local/src/pgplot/drivers/mfdriv.f    Mon Aug 29 19:32:54 1994
    # ***************
    # *** 47,53 ****
    #   C-----------------------------------------------------------------------
    #         LOGICAL    CONT
    #         INTEGER*2  BUFFER(360), COMBUF(5), I0, I1, IB, IC, IG, IR, J0, J1
    # !       INTEGER*2  LASTI, LASTJ, NPICT, NPTS, TEMP(1)
    #         INTEGER*4  HW, IER, LUN, REMCAL
    #         REAL*4     RATIO, SCALE, XMAX, YMAX
    #         CHARACTER  MSG*10
    # --- 47,53 ----
    #   C-----------------------------------------------------------------------
    #         LOGICAL    CONT
    #         INTEGER*2  BUFFER(360), COMBUF(5), I0, I1, IB, IC, IG, IR, J0, J1
    # !       INTEGER*2  LASTI, LASTJ, NPICT, NPTS
    #         INTEGER*4  HW, IER, LUN, REMCAL
    #         REAL*4     RATIO, SCALE, XMAX, YMAX
    #         CHARACTER  MSG*10
    # ***************
    # *** 156,164 ****
    #            RBUF(1) = LUN
    #         END IF
    #   C                                      Open the output file.
    # !       OPEN (UNIT = LUN, FILE = CHR(:LCHR), ACCESS='DIRECT',
    # !      1      STATUS = 'NEW',
    # !      2      RECL = 180, FORM = 'UNFORMATTED', 
    #        3      IOSTAT = IER)
    #   C                                      Check for an error and cleanup if
    #   C                                      one occurred.
    # --- 156,164 ----
    #            RBUF(1) = LUN
    #         END IF
    #   C                                      Open the output file.
    # !       OPEN (UNIT = LUN, FILE = CHR(:LCHR), CARRIAGECONTROL = 'NONE',
    # !      1      DEFAULTFILE = DEFNAM, DISPOSE = 'DELETE', STATUS = 'NEW',
    # !      2      RECL = 180, FORM = 'UNFORMATTED', RECORDTYPE = 'FIXED', 
    #        3      IOSTAT = IER)
    #   C                                      Check for an error and cleanup if
    #   C                                      one occurred.
    # ***************
    # *** 194,205 ****
    #   C
    #     100 CONTINUE
    #   C                                      Send the END_METAFILE command.
    # !       TEMP(1) = '8100'X
    # !       CALL GRMF01 (1, TEMP, BUFFER, LUN, HW)
    #   C                                      Flush the buffer.
    #         CALL GRMF02 (LUN, HW, BUFFER)
    #   C                                      Close the file.
    # !       CLOSE (LUN, STATUS = 'KEEP')
    #   C                                      Deallocate the logical unit.
    #         CALL GRFLUN (LUN)
    #   C
    # --- 194,204 ----
    #   C
    #     100 CONTINUE
    #   C                                      Send the END_METAFILE command.
    # !       CALL GRMF01 (1, '8100'X, BUFFER, LUN, HW)
    #   C                                      Flush the buffer.
    #         CALL GRMF02 (LUN, HW, BUFFER)
    #   C                                      Close the file.
    # !       CLOSE (LUN, DISPOSE = 'KEEP')
    #   C                                      Deallocate the logical unit.
    #         CALL GRFLUN (LUN)
    #   C
    # ***************
    # *** 305,312 ****
    #   C
    #     140 CONTINUE
    #   C                                      Send a END_PICTURE command.
    # !       TEMP(1) = '9100'X
    # !       CALL GRMF01 (1, TEMP, BUFFER, LUN, HW)
    #         RETURN
    #   C
    #   C--- IFUNC = 15, Select color index ------------------------------------
    # --- 304,310 ----
    #   C
    #     140 CONTINUE
    #   C                                      Send a END_PICTURE command.
    # !       CALL GRMF01 (1, '9100'X, BUFFER, LUN, HW)
    #         RETURN
    #   C
    #   C--- IFUNC = 15, Select color index ------------------------------------
    #"${EDITOR}" makefile
    # remove all *.h files from dependency list for pndriv.o
    # For TK_LIBS definition, change to 
    # TK_LIBS=-L/usr/lib -ltk8.5 -ltcl8.5 -L/usr/X11R6/lib -lX11 -ldl
    TCLVER=`echo 'puts $tcl_version;exit 0' | tclsh`
    export TCLTKLIBS="-ltk${TCLVER} -ltcl${TCLVER}"
    make 
    make cpg
    make clean
    cd "${origdir}"
    #"${EDITOR}" SETUP.sh
fi
echo "BUILD_LUMP_PULSAR_PGPLOT=0" >| "${LUMPINSTALLDIRPULSARMAIN}"/INSTALL_LuMP_Pulsar_PGPLOT.control

