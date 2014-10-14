#
# JMA_code.rules
#
# Example Makefile for JMA coding
# 2004 Feb 13  James M Anderson  --NRAO  reorganize from various places
# 2006 Jul 05  JMA  --JIVE  more changes for better installation stuff
# 2013 Jan 05  JMA  --MPIfR  usage for Eff_recording project
# 2013 Mar 16  JMA  --- update for LOFAR_Station_Beamformed_LuMP1_Expander
# 2013 Jul 13  JMA  --- update for MPIfR_Angle
# 2013 Aug 16  JMA  --- update for Ubuntu/Debian and Python modules
# 2014 Apr 26  JMA  --- update for new LuMP make scripts
# 2014 Jul 27  JMA  --- add test_ascii_dump_PFB0
# 2014 Sep 01  JMA  --- add FFT1 writer type
# 2014 Sep 05  JMA  --- add PFB1, P_FFT1, P_PFB1, FFT2 writer types
# 2014 Sep 25  JMA  --- add make_wisdom target

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



SHELL = /bin/bash

#INSTALLDIR = /usr/local
#FFTW3DIR = /usr/local



# Version numbering for the libiri library
VER_MAJOR = 0
VER_MINOR = 0
VER_SUBSUB = 0





# do not use debugging by default
DEBUG=0
# do not make a shared library by default
SHARED = 0
EXT = a



# define the compiler
#CC = gcc
#F77 = gfortran
C++ = $(CXX)

CPP = cpp -P

MAKEDEPENDREAL = .dependencies.mk
MAKEDEPENDTEMP = .dependencies.mk.temp
MAKEDEPEND = gcc -MM -MF $(MAKEDEPENDTEMP)




#INCLUDES = -I.  -I/aux/vlb054a/anderson/diana2/prog/include -I/aux/vlb054a/anderson/local/include -I$(INSTALLDIR)/include
INCLUDES = -I.  -I$(FFTW3DIR)/include
DEFINES = -DHAVE_CONFIG_H


# The following forces using the static version of the FFTW libraries,
# then uses shared objects for anything that follows
FFTW_LIBRARY_FLAGS = -L$(FFTW3DIR)/lib64 -L$(FFTW3DIR)/lib -Wl,-Bstatic -lfftw3f -lfftw3 -lfftw3l -lfftw3q -Wl,-Bdynamic





# Try using gcc -march=native -Q --help=target to see if you are missing
# specific flags because gcc does not understand your processor correctly.
CFLAGS = -O3 -ffast-math -funroll-loops -Wall $(INCLUDES) $(DEFINES) 
FFLAGS = -O3 -Wall $(INCLUDES) $(DEFINES) 
DEFINES += -D_FILE_OFFSET_BITS=64 -pthread

ifeq ($(DEBUG),0)
  CFLAGS += 
  FFLAGS += 
else
  # -pg for profiling
  #DEFINES += -DDEBUG=$(DEBUG) -DLOFAR_DEBUG=3
  #DEFINES += -DDEBUG=$(DEBUG) -DLOFAR_DEBUG=2
  DEFINES += -DDEBUG=$(DEBUG) -DLOFAR_DEBUG=1
  CFLAGS += 
  FFLAGS += 
endif

ifeq ($(SHARED),0)
	EXT = a
else
	CFLAGS += -fPIC
	FFLAGS += -fPIC
	EXT = so
endif

CXXFLAGS = $(CFLAGS) -std=$(LUMP_CXX_STD)







# set up a postfix to use for file and directory names
ifeq ($(DEBUG),0)
  POSTFIX=$(OSTYPE)
else
  POSTFIX=$(OSTYPE)_debug
endif







RM = /bin/rm -f
CP = /bin/cp 
MKDIR = /bin/mkdir -p
MV = /bin/mv
AR = ar



HEADERS = \
	JMA_code.h \
	JMA_math.h \
	LOFAR_Station_Beamformed_Info.h \
	LOFAR_Station_Beamformed_Info_0001.h \
	LOFAR_Station_Beamformed_Info_0002.h \
	LOFAR_Station_Beamformed_Packet.h \
	LOFAR_Station_Beamformed_Sorter.h \
	LOFAR_Station_Beamformed_VDIF_Packet.h \
	LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.h \
	LOFAR_Station_Beamformed_Valid.h \
	LOFAR_Station_Beamformed_Valid_Real32.h \
	LOFAR_Station_Beamformed_Writer_Base.h \
	LOFAR_Station_Beamformed_Writer_FFT0.h \
	LOFAR_Station_Beamformed_Writer_FFT1.h \
	LOFAR_Station_Beamformed_Writer_FFT2.h \
	LOFAR_Station_Beamformed_Writer_LuMP0.h \
	LOFAR_Station_Beamformed_Writer_LuMP1.h \
	LOFAR_Station_Beamformed_Writer_LuMP2.h \
	LOFAR_Station_Beamformed_Writer_P_FFT0.h \
	LOFAR_Station_Beamformed_Writer_P_FFT1.h \
	LOFAR_Station_Beamformed_Writer_P_PFB0.h \
	LOFAR_Station_Beamformed_Writer_P_PFB1.h \
	LOFAR_Station_Beamformed_Writer_Packet0.h \
	LOFAR_Station_Beamformed_Writer_PFB0.h \
	LOFAR_Station_Beamformed_Writer_PFB1.h \
	LOFAR_Station_Beamformed_Writer_Power0.h \
	LOFAR_Station_Beamformed_Writer_Raw.h \
	LOFAR_Station_Beamformed_Writer_Raw0.h \
	LOFAR_Station_Beamformed_Writer_Raw1.h \
	LOFAR_Station_Beamformed_Writer_Signals.h \
	LOFAR_Station_Beamformed_Writer_VDIF0.h \
	LOFAR_Station_Common.h \
	LOFAR_Station_Complex.h \
	LOFAR_Station_Swap.h \
	MPIfR_FFTW3.h \
	MPIfR_Real16_t.h \
	MPIfR_Sexagesimal_Angle.h \
	MPIfR_logging.h \
	config.h






SRCS = \
	LOFAR_Station_Beamformed_Info.cxx \
	LOFAR_Station_Beamformed_Info_0001.cxx \
	LOFAR_Station_Beamformed_Info_0002.cxx \
	LOFAR_Station_Beamformed_Info_Dump.cxx \
	LOFAR_Station_Beamformed_LuMP1_Expander.cxx \
	LOFAR_Station_Beamformed_Packet.cxx \
	LOFAR_Station_Beamformed_Recorder.cxx \
	LOFAR_Station_Beamformed_Sorter.cxx \
	LOFAR_Station_Beamformed_VDIF_Packet.cxx \
	LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.cxx \
	LOFAR_Station_Beamformed_Valid.cxx \
	LOFAR_Station_Beamformed_Valid_Real32.cxx \
	LOFAR_Station_Beamformed_Writer_Base.cxx \
	LOFAR_Station_Beamformed_Writer_FFT0.cxx \
	LOFAR_Station_Beamformed_Writer_FFT1.cxx \
	LOFAR_Station_Beamformed_Writer_FFT2.cxx \
	LOFAR_Station_Beamformed_Writer_LuMP0.cxx \
	LOFAR_Station_Beamformed_Writer_LuMP1.cxx \
	LOFAR_Station_Beamformed_Writer_LuMP2.cxx \
	LOFAR_Station_Beamformed_Writer_P_FFT0.cxx \
	LOFAR_Station_Beamformed_Writer_P_FFT1.cxx \
	LOFAR_Station_Beamformed_Writer_P_PFB0.cxx \
	LOFAR_Station_Beamformed_Writer_P_PFB1.cxx \
	LOFAR_Station_Beamformed_Writer_Packet0.cxx \
	LOFAR_Station_Beamformed_Writer_PFB0.cxx \
	LOFAR_Station_Beamformed_Writer_PFB1.cxx \
	LOFAR_Station_Beamformed_Writer_Power0.cxx \
	LOFAR_Station_Beamformed_Writer_Raw.cxx \
	LOFAR_Station_Beamformed_Writer_Raw0.cxx \
	LOFAR_Station_Beamformed_Writer_Raw1.cxx \
	LOFAR_Station_Beamformed_Writer_Signals.cxx \
	LOFAR_Station_Beamformed_Writer_VDIF0.cxx \
	LOFAR_Station_Common.cxx \
	LOFAR_Station_Complex.cxx \
	MPIfR_FFTW3.cxx \
	MPIfR_FFTW3_default_wisdom.cxx \
	MPIfR_Numerical_sampling_windows.cxx \
	MPIfR_Real16_t.cxx \
	MPIfR_Sexagesimal_Angle.cxx \
	MPIfR_logging.cxx \
	test_LuMP_disk_access.cxx \
	test_ascii_dump_power0.cxx \
	test_ascii_dump_power0_mean.cxx \
	test_ascii_dump_raw0.cxx \
	test_writer.cxx

OBJS = \
	LOFAR_Station_Beamformed_Info.o \
	LOFAR_Station_Beamformed_Info_0001.o \
	LOFAR_Station_Beamformed_Info_0002.o \
	LOFAR_Station_Beamformed_Packet.o \
	LOFAR_Station_Beamformed_Sorter.o \
	LOFAR_Station_Beamformed_VDIF_Packet.o \
	LOFAR_Station_Beamformed_VDIF_Packet_LOFAR0.o \
	LOFAR_Station_Beamformed_Valid.o \
	LOFAR_Station_Beamformed_Valid_Real32.o \
	LOFAR_Station_Beamformed_Writer_Base.o \
	LOFAR_Station_Beamformed_Writer_FFT0.o \
	LOFAR_Station_Beamformed_Writer_FFT1.o \
	LOFAR_Station_Beamformed_Writer_FFT2.o \
	LOFAR_Station_Beamformed_Writer_LuMP0.o \
	LOFAR_Station_Beamformed_Writer_LuMP1.o \
	LOFAR_Station_Beamformed_Writer_LuMP2.o \
	LOFAR_Station_Beamformed_Writer_P_FFT0.o \
	LOFAR_Station_Beamformed_Writer_P_FFT1.o \
	LOFAR_Station_Beamformed_Writer_P_PFB0.o \
	LOFAR_Station_Beamformed_Writer_P_PFB1.o \
	LOFAR_Station_Beamformed_Writer_Packet0.o \
	LOFAR_Station_Beamformed_Writer_PFB0.o \
	LOFAR_Station_Beamformed_Writer_PFB1.o \
	LOFAR_Station_Beamformed_Writer_Power0.o \
	LOFAR_Station_Beamformed_Writer_Raw.o \
	LOFAR_Station_Beamformed_Writer_Raw0.o \
	LOFAR_Station_Beamformed_Writer_Raw1.o \
	LOFAR_Station_Beamformed_Writer_Signals.o \
	LOFAR_Station_Beamformed_Writer_VDIF0.o \
	MPIfR_FFTW3.o \
	MPIfR_Numerical_sampling_windows.o \
	MPIfR_Real16_t.o \
	MPIfR_Sexagesimal_Angle.o \
	MPIfR_logging.o \
	LOFAR_Station_Common.o \
	LOFAR_Station_Complex.o

PROG = \
	LOFAR_Station_Beamformed_Info_Dump \
	LOFAR_Station_Beamformed_LuMP1_Expander \
	LOFAR_Station_Beamformed_Recorder \
	LOFAR_Station_Beamformed_Test_Writer \
	MPIfR_FFTW3_default_wisdom \
	test_LuMP_disk_access \
	test_MPIfR_Sexagesimal_Angle \
	test_ascii_dump_P_PFB0 \
	test_ascii_dump_PFB0 \
	test_ascii_dump_power0 \
	test_ascii_dump_power0_mean \
	test_ascii_dump_raw0 \
	test_dog 

LIBS = \
	libMPIfRLOFAR.$(EXT)

PYTHONLIBS = \
        LuMP_Computer_Information.py \
        MPIfR_LOFAR_LuMP.py \
        MPIfR_LOFAR_LuMP_Recorder_Common.py

PYTHONPROG = \
	Basic_LuMP_Power0_Recorder.py \
	Basic_LuMP_Recorder.py \
	LOFAR_Station_Beamformed_Recorder.py \
	LuMP_Pulsar_Cleanup.py \
	MPIfR_FFTW3_default_wisdom.py \
	test_record.py




all: $(PROG)
.PHONY : all

-include $(SRCS:.cxx=.d)




#find_line_max: $(OBJS) find_line_max.o
#	$(CC) $(CFLAGS) -o $@ $(OBJS) find_line_max.o -lm -lsupc++

test_LuMP_disk_access: test_LuMP_disk_access.o
	$(C++) $(CFLAGS) -o $@ $@.o -lrt 

test_MPIfR_Sexagesimal_Angle: test_MPIfR_Sexagesimal_Angle.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_ascii_dump_P_PFB0: test_ascii_dump_P_PFB0.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_ascii_dump_PFB0: test_ascii_dump_PFB0.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_ascii_dump_power0: test_ascii_dump_power0.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_ascii_dump_power0_mean: test_ascii_dump_power0_mean.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_ascii_dump_raw0: test_ascii_dump_raw0.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

test_dog: test_dog.o
	$(C++) $(CFLAGS) -o $@ $@.o

LOFAR_Station_Beamformed_Recorder: LOFAR_Station_Beamformed_Recorder.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR $(FFTW_LIBRARY_FLAGS) -lrt -lquadmath -lm

LOFAR_Station_Beamformed_Test_Writer: LOFAR_Station_Beamformed_Test_Writer.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

LOFAR_Station_Beamformed_Info_Dump: LOFAR_Station_Beamformed_Info_Dump.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

LOFAR_Station_Beamformed_LuMP1_Expander: LOFAR_Station_Beamformed_LuMP1_Expander.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR -lrt -lquadmath -lm

MPIfR_FFTW3_default_wisdom: MPIfR_FFTW3_default_wisdom.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR $(FFTW_LIBRARY_FLAGS) -lrt -lquadmath -lm

test: test.o libMPIfRLOFAR.$(EXT)
	$(C++) $(CFLAGS) -o $@ $@.o -L. -lMPIfRLOFAR $(FFTW_LIBRARY_FLAGS) -lrt -lquadmath -lm








libMPIfRLOFAR.a: $(OBJS)
	$(AR) -crv libMPIfRLOFAR.a $(OBJS)


libMPIfRLOFAR.so: $(OBJS)
#	$(CC) -shared -Wl,-soname,libMPIfRLOFAR.so.$(VER_MAJOR) -o libMPIfRLOFAR.so $(OBJS) -lc
	$(CC) -shared -Wl,-soname,libMPIfRLOFAR.so -o libMPIfRLOFAR.so $(OBJS) -lc








# This will probably work best with C and C++ code.  Modify
# If you want to do this for FORTRAN too.
.PHONY : clean distclean install
depend:
	$(RM) $(MAKEDEPENDREAL)
	$(foreach i,$(SRCS), $(MAKEDEPEND) $(CFLAGS) $i;cat $(MAKEDEPENDTEMP) >> $(MAKEDEPENDREAL);)
	$(RM) $(MAKEDEPENDTEMP)


clean:
	$(RM) $(OBJS)
	$(RM) *.o *.d

distclean: clean
	$(RM) *~
	$(RM) $(PROG) $(LIBS)


install: $(LIBS) $(PROG)
	mkdir -p $(INSTALLDIR)/bin $(INSTALLDIR)/lib $(INSTALLDIR)/include
	$(CP) $(PROG) $(PYTHONPROG) $(INSTALLDIR)/bin
	$(CP) $(LIBS) $(INSTALLDIR)/lib
	$(CP) $(PYTHONLIBS) $(INSTALLDIR)/share/python
	$(CP) $(HEADERS) $(INSTALLDIR)/include

make_wisdom: MPIfR_FFTW3_default_wisdom
	MPIfR_FFTW3_default_wisdom.sh




# make sure we can compile C++ stuff
.SUFFIXES:
.SUFFIXES: .cxx $(SUFFIXES)









# declare our own default compilation rules
.cxx.o:
	$(C++) $(CXXFLAGS) -c $<

.F.o:
	$(F77) $(FFLAGS) -c $<

.f.o:
	$(F77) $(FFLAGS) -c $<


# dependency generation
%.d: %.cxx
	$(C++) -M $(CXXFLAGS) -MF $@ $< -MT "$*.o $@"
