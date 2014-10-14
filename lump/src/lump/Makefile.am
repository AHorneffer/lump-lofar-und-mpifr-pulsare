lib_LIBRARIES = libMPIfRLOFAR.a
lib_LTLIBRARIES = libMPIfRLOFAR.la
libMPIfRLOFAR_a_SOURCES = \
	LOFAR_Station_Beamformed_Packet.cxx      \
	LOFAR_Station_Beamformed_Sorter.cxx       \
	LOFAR_Station_Beamformed_Writer_Base.cxx  \
	LOFAR_Station_Beamformed_Writer_Raw.cxx \
	LOFAR_Station_Beamformed_Writer_Raw1.cxx \
        LOFAR_Station_Common.cxx 
libMPIfRLOFAR_la_SOURCES = \
	LOFAR_Station_Beamformed_Packet.cxx      \
	LOFAR_Station_Beamformed_Sorter.cxx       \
	LOFAR_Station_Beamformed_Writer_Base.cxx  \
	LOFAR_Station_Beamformed_Writer_Raw.cxx \
	LOFAR_Station_Beamformed_Writer_Raw1.cxx \
        LOFAR_Station_Common.cxx 
pkginclude_HEADERS = \
	LOFAR_Station_Beamformed_Packet.h       \
	LOFAR_Station_Beamformed_Sorter.h       \
	LOFAR_Station_Beamformed_Writer_Base.h  \
	LOFAR_Station_Beamformed_Writer_Raw.h \
	LOFAR_Station_Beamformed_Writer_Raw1.h \
	LOFAR_Station_Common.h\
	LOFAR_Station_Complex.h \

libMPIfRLOFAR_a_CXXFLAGS = -dLOFAR_DEBUG
libMPIfRLOFAR_la_CXXFLAGS = -dLOFAR_DEBUG


bin_PROGRAMS = testdump
testdump_SOURCES = test_writer.cxx
testdump_LDADD = libMPIfRLOFAR.a libm.so
testdump_CXXFLAGS = -dLOFAR_DEBUG
