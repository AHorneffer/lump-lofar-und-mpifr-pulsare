// LOFAR_Station_Beamformed_Info.cxx
// code for the Info class
//_HIST  DATE NAME PLACE INFO
// 2011 Aug 06  James M Anderson  --MPIfR  start
// 2012 Feb 05  JMA  --strip path from filename_base
// 2012 Apr 12  JMA  --update for info type 0002
// 2012 Apr 19  JMA  -- add version_date string
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 13  JMA  --bugfix
// 2013 Jul 04  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2013 Aug 09  JMA  ---- change info array data filenames to _lis extension
//                        from _raw
// 2013 Sep 21  JMA  ---- put 0x before hexadecimal text output
// 2014 Aug 29  JMA  ---- update documentation for description of non-sky
//                        values for beamlets, subbands, and so on files.
// 2014 Sep 09  JMA  ---- update for new versioning system


// Copyright (c) 2011,2012,2013,2014 James M. Anderson <anderson@gfz-potsdam.de>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.







// Information file:
// Filename: filename_base.info
// The description is found below in the code.
//
// Beamlets file:
// Filename: filename_base.beamlets_lis
// One beamlets file is written out specifying the physical
// beamlet numbers of all beamlets written out by this writer.  The format is
// uint32_t beamlet[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
// Values of UINT32_MAX indicate non-sky data (usually for the sample valid
// information).
//
// Subbands file:
// Filename: filename_base.subbands_lis
// One subbands file is written out specifying the physical
// subband numbers of all beamlets written out by this writer.  The format is
// uint32_t subband[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
// Values of UINT32_MAX indicate non-sky data (usually for the sample valid
// information).
//
// RCUMODEs file:
// Filename: filename_base.rcumodes_lis
// One RCUMODEs file is written out specifying the RCUMODE
// numbers of all beamlets written out by this writer.  The format is
// uint32_t rcumode[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
//
// RightAscensions file:
// Filename: filename_base.rightascensions_lis
// One RightAscensions file is written out specifying the Right Ascension
// values (in units of radians) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// Real64_t rightascension[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
// Values of NaN indicate non-sky data (usually for the sample valid
// information).
//
// Declinations file:
// Filename: filename_base.declinations_lis
// One Declinations file is written out specifying the Declination
// values (in units of radians) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// Real64_t declination[beamlets_per_sample]
// where beamlets_per_sample is obtained from the Information file above.
// Values of NaN indicate non-sky data (usually for the sample valid
// information).
//
// Epochs file:
// Filename: filename_base.epochs_lis
// One Epochs file is written out specifying the Epoch
// values (string value) used to generate the LOFAR station beams
// of all beamlets written out by this writer.  The format is
// char epoch[beamlets_per_sample][]
// where beamlets_per_sample is obtained from the Information file above.
// Values of "None" indicate non-sky data (usually for the sample valid
// information).
//
// SourceNames file:
// Filename: filename_base.sourcenames_lis
// One SourceNames file is written out specifying the source name
// values (string value) associated with the  LOFAR station beams
// of all beamlets written out by this writer.  The format is
// char sourcenames[beamlets_per_sample][]
// where beamlets_per_sample is obtained from the Information file above.
// Values of "None" indicate non-sky data (usually for the sample valid
// information).






// INCLUDES
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif
#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif
#ifndef __USE_ISOC99
#  define __USE_ISOC99 1
#endif
#ifndef _ISOC99_SOURCE
#  define _ISOC99_SOURCE
#endif
#ifndef __USE_MISC
#  define __USE_MISC 1
#endif
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <inttypes.h>
#include <limits>
#ifdef __cplusplus
#  include <cstddef>
#else
#  include <stddef.h>
#endif
#include <stdint.h>
// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#include <time.h>

#include "JMA_math.h"
#include "LOFAR_Station_Beamformed_Info.h"


#include "LOFAR_Station_Beamformed_Info_0001.h"
#include "LOFAR_Station_Beamformed_Info_0002.h"

#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include "LOFAR_Station_Swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "MPIfR_Numerical_sampling_windows.h"






// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {






// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Info::
LOFAR_Station_Beamformed_Info(const char* const restrict filename_base_,
                              const char* const restrict mode) throw()
        :

    VERSION(MPIfR_LOFAR_STATION_SOFTWARE_VERSION),
    WRITER_TYPE(-1),
    RCUMODE(-1),
    station_clock_frequency(-1),
    time_reference_zero(-1LL),
    seconds_per_sample(-1.0),
    sampler_frequency(-1.0),
    Nyquist_offset(-1.0),
    beamlets_per_sample(-1),
    GRONINGEN_SAMPLE_COUNT_PER_BLOCK(-1),
    station_DATA_TYPE(-1),
    processing_DATA_TYPE(-1),
    output_DATA_TYPE(-1),
    channelizer_TYPE(-1),
    FFT_window_parameter(-1.0),
    FFT_window_TYPE(-1),
    NUM_CHANNELS(-1),
    NUM_POLYPHASE_TAPS(-1),
    samples_per_channelization(-1),
    samples_per_integration(-1),
    seconds_per_channelization_full(-1.0),
    seconds_per_channelization_eff(-1.0),
    samples_per_channelization_eff(-1.0),
    seconds_per_integration_full(-1.0),
    seconds_per_integration_eff(-1.0),
    samples_per_integration_eff(-1.0),
    seconds_per_output(-1.0),
    integration_multiplier(-1.0),
    sample_start(-1LL),
    time_center_output_zero(-1.0),
    num_output_timeslots(-1LL),
    read_init_valid(false),
    is_writeable(false),
    fp(0)

        
{
    station_name[0] = 0;
    version_date[0] = 0;
    new_filename[0] = 0;
    
    // Now open the files
    int retval = snprintf(new_filename, FSIZE, "%s.info", filename_base_);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating info filename from base '%s'\n", filename_base_);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    fp = fopen(new_filename,mode);
#else
    fp = fopen("/dev/null",mode);
#endif
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("unable to open info file '%s' with mode '%s'\n",
                new_filename, mode);
        exit(2);
    }
    if(mode[0] == 'w') {
        // we are writing out a dataset.  For now, all is ok
        is_writeable = true;
    }
    else if(mode[0] == 'r') {
        // reading in a dataset
        size_t r = fread(&VERSION,sizeof(VERSION),1,fp);
        if(r != 1) {
            DEF_LOGGING_ERROR("unable to read version from info file '%s'\n",
                              new_filename);
        }
        else {
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
            VERSION = uint16_t(LOFAR_Station_Swap_int16_t(int16_t(VERSION)));
#endif
            if(VERSION == 0x0001) {
                read_0001();
            }
            else if( (VERSION == 0x0002)
                   || ((VERSION >= 0x0220) && (VERSION < 0x0300))
                     ) {
                read_0002();
            }
            else {
                DEF_LOGGING_ERROR("Unknown version 0x%X from info file '%s'\n",
                        VERSION, new_filename);
                read_init_valid = false;
            }
        }
    }
    else {
        DEF_LOGGING_CRITICAL("unrecognized mode '%s' for info file\n",
                mode);
        exit(1);
    }
}


LOFAR_Station_Beamformed_Info::
~LOFAR_Station_Beamformed_Info() throw()
{
    if((fp)) {
        int retval = fclose(fp);
        if(retval != 0) {
            DEF_LOGGING_PERROR("Error closing file");
            DEF_LOGGING_ERROR("Error closing info file '%s'\n", new_filename);
        }
        fp = 0;
    }
    return;
}





int LOFAR_Station_Beamformed_Info::write_to_file() throw() {
    if(!is_writeable) {
        DEF_LOGGING_CRITICAL("Software developer error, info not writeable\n");
        exit(1);
    }
    if(VERSION == 0x0001) {
        return write_0001();
    }
    else if(VERSION == 0x0002) {
        return write_0002();
    }
    else if((VERSION >= 0x0220) && (VERSION < 0x0300)) {
        return write_0002();
    }
    else {
        DEF_LOGGING_ERROR("Software developer error, unknown version 0x%X\n",
                VERSION);
        return -10;
    }
    return -11;
}


int LOFAR_Station_Beamformed_Info::write_0001() throw() {
    LOFAR_Station_Beamformed_Info_0001 info;
    info.VERSION = VERSION;
    info.WRITER_TYPE = WRITER_TYPE;
    info.RCUMODE = RCUMODE;
    strncpy(info.station_name,station_name,info.STATION_NAME_STR_SIZE);
    info.station_name[info.STATION_NAME_STR_SIZE-1] = 0;
    info.time_reference_zero = time_reference_zero;
    info.seconds_per_sample = seconds_per_sample;
    info.sampler_frequency = sampler_frequency;
    info.Nyquist_offset = Nyquist_offset;
    info.beamlets_per_sample = beamlets_per_sample;
    info.GRONINGEN_SAMPLE_COUNT_PER_BLOCK = GRONINGEN_SAMPLE_COUNT_PER_BLOCK;
    info.station_DATA_TYPE = station_DATA_TYPE;
    info.processing_DATA_TYPE = processing_DATA_TYPE;
    info.output_DATA_TYPE = output_DATA_TYPE;
    info.channelizer_TYPE = channelizer_TYPE;
    info.NUM_CHANNELS = NUM_CHANNELS;
    info.samples_per_channelization = samples_per_channelization;
    info.samples_per_integration = samples_per_integration;
    info.seconds_per_channelization_full = seconds_per_channelization_full;
    info.seconds_per_channelization_eff = seconds_per_channelization_eff;
    info.samples_per_channelization_eff = samples_per_channelization_eff;
    info.seconds_per_integration_full = seconds_per_integration_full;
    info.seconds_per_integration_eff = seconds_per_integration_eff;
    info.samples_per_integration_eff = samples_per_integration_eff;
    info.seconds_per_output = seconds_per_output;
    info.integration_multiplier = integration_multiplier;
    info.sample_start = sample_start;
    info.time_center_output_zero = time_center_output_zero;
    info.num_output_timeslots = num_output_timeslots;

    rewind(fp);
    size_t retval =  info.write_to_file(fp);
    if(retval != 1) {
        DEF_LOGGING_ERROR("unable to write to output info file '%s'\n",
                new_filename);
        return -1;
    }
    if(fflush(fp) == 0) {}
    else {
        DEF_LOGGING_ERROR("unable to write to output info file '%s'\n",
                new_filename);
        return -2;
    }
    return 0;
}


void LOFAR_Station_Beamformed_Info::read_0001() throw() {
    rewind(fp);
    LOFAR_Station_Beamformed_Info_0001 info(fp);
    VERSION = info.VERSION;
    WRITER_TYPE = info.WRITER_TYPE;
    RCUMODE = info.RCUMODE;
    strncpy(station_name,info.station_name,STATION_NAME_STR_SIZE);
    station_name[STATION_NAME_STR_SIZE-1] = 0;
    time_reference_zero = info.time_reference_zero;
    seconds_per_sample = info.seconds_per_sample;
    sampler_frequency = info.sampler_frequency;
    Nyquist_offset = info.Nyquist_offset;
    beamlets_per_sample = info.beamlets_per_sample;
    GRONINGEN_SAMPLE_COUNT_PER_BLOCK = info.GRONINGEN_SAMPLE_COUNT_PER_BLOCK;
    station_DATA_TYPE = info.station_DATA_TYPE;
    processing_DATA_TYPE = info.processing_DATA_TYPE;
    output_DATA_TYPE = info.output_DATA_TYPE;
    channelizer_TYPE = info.channelizer_TYPE;
    NUM_CHANNELS = info.NUM_CHANNELS;
    samples_per_channelization = info.samples_per_channelization;
    samples_per_integration = info.samples_per_integration;
    seconds_per_channelization_full = info.seconds_per_channelization_full;
    seconds_per_channelization_eff = info.seconds_per_channelization_eff;
    samples_per_channelization_eff = info.samples_per_channelization_eff;
    seconds_per_integration_full = info.seconds_per_integration_full;
    seconds_per_integration_eff = info.seconds_per_integration_eff;
    samples_per_integration_eff = info.samples_per_integration_eff;
    seconds_per_output = info.seconds_per_output;
    integration_multiplier = info.integration_multiplier;
    sample_start = info.sample_start;
    time_center_output_zero = info.time_center_output_zero;
    num_output_timeslots = info.num_output_timeslots;

    if(VERSION != 0x0001) {
        read_init_valid = false;
    }
    else {
        read_init_valid = true;
    }
    return;
}


int LOFAR_Station_Beamformed_Info::write_0002() throw() {
    LOFAR_Station_Beamformed_Info_0002 info;
    info.VERSION = VERSION;
    info.WRITER_TYPE = WRITER_TYPE;
    info.RCUMODE = RCUMODE;
    info.station_clock_frequency = station_clock_frequency;
    strncpy(info.station_name,station_name,info.STATION_NAME_STR_SIZE);
    info.station_name[info.STATION_NAME_STR_SIZE-1] = 0;
    strncpy(info.version_date,version_date,info.VERSION_DATE_STR_SIZE);
    info.version_date[info.VERSION_DATE_STR_SIZE-1] = 0;
    info.time_reference_zero = time_reference_zero;
    info.seconds_per_sample = seconds_per_sample;
    info.sampler_frequency = sampler_frequency;
    info.Nyquist_offset = Nyquist_offset;
    info.beamlets_per_sample = beamlets_per_sample;
    info.GRONINGEN_SAMPLE_COUNT_PER_BLOCK = GRONINGEN_SAMPLE_COUNT_PER_BLOCK;
    info.station_DATA_TYPE = station_DATA_TYPE;
    info.processing_DATA_TYPE = processing_DATA_TYPE;
    info.output_DATA_TYPE = output_DATA_TYPE;
    info.channelizer_TYPE = channelizer_TYPE;
    info.FFT_window_parameter = FFT_window_parameter;
    info.FFT_window_TYPE = FFT_window_TYPE;
    info.NUM_CHANNELS = NUM_CHANNELS;
    info.NUM_POLYPHASE_TAPS = NUM_POLYPHASE_TAPS;
    info.samples_per_channelization = samples_per_channelization;
    info.samples_per_integration = samples_per_integration;
    info.seconds_per_channelization_full = seconds_per_channelization_full;
    info.seconds_per_channelization_eff = seconds_per_channelization_eff;
    info.samples_per_channelization_eff = samples_per_channelization_eff;
    info.seconds_per_integration_full = seconds_per_integration_full;
    info.seconds_per_integration_eff = seconds_per_integration_eff;
    info.samples_per_integration_eff = samples_per_integration_eff;
    info.seconds_per_output = seconds_per_output;
    info.integration_multiplier = integration_multiplier;
    info.sample_start = sample_start;
    info.time_center_output_zero = time_center_output_zero;
    info.num_output_timeslots = num_output_timeslots;

    rewind(fp);
    size_t retval =  info.write_to_file(fp);
    if(retval != 1) {
        DEF_LOGGING_ERROR("unable to write to output info file '%s'\n",
                new_filename);
        return -1;
    }
    if(fflush(fp) == 0) {}
    else {
        DEF_LOGGING_ERROR("unable to write to output info file '%s'\n",
                new_filename);
        return -2;
    }
    return 0;
}


void LOFAR_Station_Beamformed_Info::read_0002() throw() {
    rewind(fp);
    LOFAR_Station_Beamformed_Info_0002 info(fp);
    VERSION = info.VERSION;
    WRITER_TYPE = info.WRITER_TYPE;
    RCUMODE = info.RCUMODE;
    station_clock_frequency = info.station_clock_frequency;
    strncpy(station_name,info.station_name,STATION_NAME_STR_SIZE);
    station_name[STATION_NAME_STR_SIZE-1] = 0;
    strncpy(version_date,info.version_date,VERSION_DATE_STR_SIZE);
    version_date[VERSION_DATE_STR_SIZE-1] = 0;
    time_reference_zero = info.time_reference_zero;
    seconds_per_sample = info.seconds_per_sample;
    sampler_frequency = info.sampler_frequency;
    Nyquist_offset = info.Nyquist_offset;
    beamlets_per_sample = info.beamlets_per_sample;
    GRONINGEN_SAMPLE_COUNT_PER_BLOCK = info.GRONINGEN_SAMPLE_COUNT_PER_BLOCK;
    station_DATA_TYPE = info.station_DATA_TYPE;
    processing_DATA_TYPE = info.processing_DATA_TYPE;
    output_DATA_TYPE = info.output_DATA_TYPE;
    channelizer_TYPE = info.channelizer_TYPE;
    FFT_window_parameter = info.FFT_window_parameter;
    FFT_window_TYPE = info.FFT_window_TYPE;
    NUM_CHANNELS = info.NUM_CHANNELS;
    NUM_POLYPHASE_TAPS = info.NUM_POLYPHASE_TAPS;
    samples_per_channelization = info.samples_per_channelization;
    samples_per_integration = info.samples_per_integration;
    seconds_per_channelization_full = info.seconds_per_channelization_full;
    seconds_per_channelization_eff = info.seconds_per_channelization_eff;
    samples_per_channelization_eff = info.samples_per_channelization_eff;
    seconds_per_integration_full = info.seconds_per_integration_full;
    seconds_per_integration_eff = info.seconds_per_integration_eff;
    samples_per_integration_eff = info.samples_per_integration_eff;
    seconds_per_output = info.seconds_per_output;
    integration_multiplier = info.integration_multiplier;
    sample_start = info.sample_start;
    time_center_output_zero = info.time_center_output_zero;
    num_output_timeslots = info.num_output_timeslots;

    if(VERSION != 0x0002) {
        read_init_valid = false;
    }
    else {
        read_init_valid = true;
    }
    return;
}




void LOFAR_Station_Beamformed_Info::
print_as_text(FILE* fp,
              const char* const restrict separator)
    const throw()
{
    static const char* blank = "";
    const char* const restrict s = (separator == NULL) ? blank:separator;
    
    fprintf(fp, "VERSION %s 0x%4.4X\n", s, VERSION);
    fprintf(fp, "WRITER_TYPE %s %s\n", s, LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(LOFAR_DATA_OUTPUT_FILE_FORMAT(WRITER_TYPE)));
    fprintf(fp, "RCUMODE %s %d\n", s, int(RCUMODE));
    fprintf(fp, "station_clock_frequency[MHz] %s %d\n", s, int(station_clock_frequency));
    fprintf(fp, "station_name %s %s\n", s, station_name);
    fprintf(fp, "version_date %s %s\n", s, version_date);
    fprintf(fp, "time_reference_zero %s 0x%llX\n", s, (long long)(time_reference_zero));
    {
        struct tm result;
        time_t tt(time_reference_zero);
        gmtime_r(&tt, &result);
        fprintf(fp, "calendar_time_reference_zero %s %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2dZ\n", s,
                result.tm_year+1900,
                result.tm_mon+1,
                result.tm_mday,
                result.tm_hour,
                result.tm_min,
                result.tm_sec
                );
    }
    fprintf(fp, "seconds_per_sample %s %.17E\n", s, seconds_per_sample);
    fprintf(fp, "sampler_frequency %s %.17E\n", s, sampler_frequency);
    fprintf(fp, "Nyquist_offset[Hz] %s %.17E\n", s, Nyquist_offset);
    fprintf(fp, "beamlets_per_sample %s %u\n", s, beamlets_per_sample);
    fprintf(fp, "GRONINGEN_SAMPLE_COUNT_PER_BLOCK %s %u\n", s, GRONINGEN_SAMPLE_COUNT_PER_BLOCK);
    fprintf(fp, "station_DATA_TYPE %s %s\n", s, LOFAR_raw_data_type_enum_Str(LOFAR_raw_data_type_enum(station_DATA_TYPE)));
    fprintf(fp, "processing_DATA_TYPE %s %s\n", s, LOFAR_raw_data_type_enum_Str(LOFAR_raw_data_type_enum(processing_DATA_TYPE)));
    fprintf(fp, "output_DATA_TYPE %s %s\n", s, LOFAR_raw_data_type_enum_Str(LOFAR_raw_data_type_enum(output_DATA_TYPE)));
    fprintf(fp, "channelizer_TYPE %s %s\n", s, LOFAR_Channelizer_Enum_Str(LOFAR_Channelizer_Enum(channelizer_TYPE)));
    fprintf(fp, "FFT_window_parameter %s %.17E\n", s, FFT_window_parameter);
    fprintf(fp, "FFT_window_TYPE %s %s\n", s, MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM_Str(MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM(FFT_window_TYPE)));
    fprintf(fp, "NUM_CHANNELS %s %u\n", s, NUM_CHANNELS);
    fprintf(fp, "NUM_POLYPHASE_TAPS %s %u\n", s, NUM_POLYPHASE_TAPS);
    fprintf(fp, "samples_per_channelization %s %u\n", s, samples_per_channelization);
    fprintf(fp, "samples_per_integration %s %u\n", s, samples_per_integration);
    fprintf(fp, "seconds_per_channelization_full %s %.17E\n", s, seconds_per_channelization_full);
    fprintf(fp, "seconds_per_channelization_eff %s %.17E\n", s, seconds_per_channelization_eff);
    fprintf(fp, "samples_per_channelization_eff %s %.17E\n", s, samples_per_channelization_eff);
    fprintf(fp, "seconds_per_integration_full %s %.17E\n", s, seconds_per_integration_full);
    fprintf(fp, "seconds_per_integration_eff %s %.17E\n", s, seconds_per_integration_eff);
    fprintf(fp, "samples_per_integration_eff %s %.17E\n", s, samples_per_integration_eff);
    fprintf(fp, "seconds_per_output %s %.17E\n", s, seconds_per_output);
    fprintf(fp, "integration_multiplier %s %.17E\n", s, integration_multiplier);
    fprintf(fp, "sample_start %s %lld\n", s, (long long)(sample_start));
    fprintf(fp, "time_center_output_zero %s %.17E\n", s, time_center_output_zero);
    fprintf(fp, "num_output_timeslots %s %llu\n", s, (unsigned long long)(num_output_timeslots));
    return;
}






















void LOFAR_Station_Beamformed_Info::
write_array_data(const char* const restrict filename_base,
                 const char* const restrict * const restrict array,
                 const char* const restrict data_type)
    const throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out = fopen(new_filename,"wbx");
#else
    FILE* fp_out = fopen("/dev/null","wb");
#endif
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        if(fwrite(array[beamlet], strlen(array[beamlet])+1, 1, fp_out) != 1) {
            DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
            exit(2);
        }
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}


void LOFAR_Station_Beamformed_Info::
write_array_data(const char* const restrict filename_base,
                 const uint_fast32_t* const restrict array,
                 const char* const restrict data_type)
    const throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out = fopen(new_filename,"wbx");
#else
    FILE* fp_out = fopen("/dev/null","wb");
#endif
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        uint32_t b = array[beamlet];
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        b = uint32_t(LOFAR_Station_Swap_int32_t(int32_t(b)));
#endif
        if(fwrite(&b, sizeof(b), 1, fp_out) != 1) {
            DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
            exit(2);
        }
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}


void LOFAR_Station_Beamformed_Info::
write_array_data(const char* const restrict filename_base,
                 const Real64_t* const restrict array,
                 const char* const restrict data_type)
    const throw()
{
    union {
        Real64_t u_d;
        int64_t  u_i;
    };
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out = fopen(new_filename,"wbx");
#else
    FILE* fp_out = fopen("/dev/null","wb");
#endif
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        u_d = array[beamlet];
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        u_i = LOFAR_Station_Swap_int64_t(u_i);
#endif
        if(fwrite(&u_i, sizeof(u_i), 1, fp_out) != 1) {
            DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
            exit(2);
        }
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}



void LOFAR_Station_Beamformed_Info::
append_to_array_data(const char* const restrict filename_base,
                     const char* const restrict value,
                     const char* const restrict data_type)
    const throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    FILE* fp_out = fopen(new_filename,"ab");
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    if(fwrite(value, strlen(value)+1, 1, fp_out) != 1) {
        DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
        exit(2);
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}


void LOFAR_Station_Beamformed_Info::
append_to_array_data(const char* const restrict filename_base,
                     const uint_fast32_t value,
                     const char* const restrict data_type)
    const throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    FILE* fp_out = fopen(new_filename,"ab");
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    uint32_t b = value;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    b = uint32_t(LOFAR_Station_Swap_int32_t(int32_t(b)));
#endif
    if(fwrite(&b, sizeof(b), 1, fp_out) != 1) {
        DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
        exit(2);
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}


void LOFAR_Station_Beamformed_Info::
append_to_array_data(const char* const restrict filename_base,
                     const Real64_t value,
                     const char* const restrict data_type)
    const throw()
{
    union {
        Real64_t u_d;
        int64_t  u_i;
    };
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating %s filename from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    FILE* fp_out = fopen(new_filename,"ab");
    if(fp_out == NULL) {
        DEF_LOGGING_CRITICAL("unable to open list output %s file '%s'\n",
                             data_type, new_filename);
        exit(2);
    }

    u_d = value;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    u_i = LOFAR_Station_Swap_int64_t(u_i);
#endif
    if(fwrite(&u_i, sizeof(u_i), 1, fp_out) != 1) {
        DEF_LOGGING_CRITICAL("unable to write to %s file\n", data_type);
        exit(2);
    }
    retval = fclose(fp_out);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing %s file from base '%s'\n", data_type, filename_base);
        exit(2);
    }
    return;
}


char* restrict * restrict LOFAR_Station_Beamformed_Info::
read_array_data_char(const char* const restrict filename_base,
                     const char* const restrict data_type) const throw()
{
    // Open the file
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_ERROR("Error creating %s filename from base '%s'\n", data_type, filename_base);
        return NULL;
    }
    FILE* fp_in = fopen(new_filename,"rb");
    if(fp_in == NULL) {
        DEF_LOGGING_ERROR("unable to open list input %s file '%s'\n",
                          data_type, new_filename);
        return NULL;
    }

    retval = fseeko(fp_in, 0, SEEK_END);
    if(retval == -1) {
        DEF_LOGGING_ERROR("unable to seek in list input %s file '%s'\n",
                          data_type, new_filename);
        fclose(fp_in);
        return NULL;
    }
    size_t fp_in_size = ftello(fp_in);
    rewind(fp_in);

    char* const restrict data = reinterpret_cast<char* const restrict>(malloc(fp_in_size));
    void** array_v = reinterpret_cast<void**>(malloc(beamlets_per_sample*sizeof(char*)));
    char* restrict * restrict array = reinterpret_cast<char* restrict * restrict>(array_v);
    if((data == NULL) || (array_v == NULL)) {
        DEF_LOGGING_ERROR("unable to malloc memory for %s file '%s'\n",
                          data_type, new_filename);
        free(data);
        free(array_v);
        fclose(fp_in);
        return NULL;
    }

    if(fread(data, fp_in_size, 1, fp_in) != 1) {
        DEF_LOGGING_CRITICAL("unable to read from %s file\n", data_type);
        free(data);
        free(array_v);
        fclose(fp_in);
        return NULL;
    }

    // set the pointer to the 0th beamlet string to the start of data
    char* restrict next_string = data;
    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        array[beamlet] = next_string;
        next_string += strlen(next_string)+1;
    }
    retval = fclose(fp_in);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_ERROR("Error closing %s file from base '%s'\n", data_type, filename_base);
        free(data);
        free(array_v);
        return NULL;
    }
    return array;
}


uint_fast32_t* restrict LOFAR_Station_Beamformed_Info::
read_array_data_uint_fast32_t(const char* const restrict filename_base,
                              const char* const restrict data_type) const throw()
{
    // Open the file
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_ERROR("Error creating %s filename from base '%s'\n", data_type, filename_base);
        return NULL;
    }
    uint_fast32_t* const restrict array = reinterpret_cast<uint_fast32_t* const restrict>(malloc(beamlets_per_sample*sizeof(uint_fast32_t)));
    if(array == NULL) {
        DEF_LOGGING_ERROR("unable to malloc memory for %s file '%s'\n",
                          data_type, new_filename);
        return NULL;
    }
    FILE* fp_in = fopen(new_filename,"rb");
    if(fp_in == NULL) {
        DEF_LOGGING_ERROR("unable to open list input %s file '%s'\n",
                          data_type, new_filename);
        free(array);
        return NULL;
    }
    
    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        uint32_t b;
        if(fread(&b, sizeof(b), 1, fp_in) != 1) {
            DEF_LOGGING_ERROR("unable to read from %s file\n", data_type);
            fclose(fp_in);
            free(array);
            return NULL;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        b = uint32_t(LOFAR_Station_Swap_int32_t(int32_t(b)));
#endif
        array[beamlet] = b;
    }
    retval = fclose(fp_in);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_ERROR("Error closing %s file from base '%s'\n", data_type, filename_base);
        free(array);
        return NULL;
    }
    return array;
}


Real64_t* restrict LOFAR_Station_Beamformed_Info::
read_array_data_Real64_t(const char* const restrict filename_base,
                         const char* const restrict data_type) const throw()
{
    union {
        Real64_t u_d;
        int64_t  u_i;
    };
    // Open the file
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.%s_lis", filename_base, data_type);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_ERROR("Error creating %s filename from base '%s'\n", data_type, filename_base);
        return NULL;
    }
    Real64_t* const restrict array = reinterpret_cast<Real64_t* const restrict>(malloc(beamlets_per_sample*sizeof(Real64_t)));
    if(array == NULL) {
        DEF_LOGGING_ERROR("unable to malloc memory for %s file '%s'\n",
                          data_type, new_filename);
        return NULL;
    }
    FILE* fp_in = fopen(new_filename,"rb");
    if(fp_in == NULL) {
        DEF_LOGGING_ERROR("unable to open list input %s file '%s'\n",
                          data_type, new_filename);
        free(array);
        return NULL;
    }
    
    for(uint_fast16_t beamlet=0; beamlet < beamlets_per_sample; beamlet++) {
        if(fread(&u_i, sizeof(u_i), 1, fp_in) != 1) {
            DEF_LOGGING_ERROR("unable to read from %s file\n", data_type);
            fclose(fp_in);
            free(array);
            return NULL;
        }
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
        u_i = LOFAR_Station_Swap_int64_t(u_i);
#endif
        array[beamlet] = u_d;
    }
    retval = fclose(fp_in);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_ERROR("Error closing %s file from base '%s'\n", data_type, filename_base);
        free(array);
        return NULL;
    }
    return array;
}











const char * const strip_path(const char * const restrict filename) throw()
{
    const char* slash_p = NULL;
    const char* cp = filename;
    while((*cp)) {
        if(*cp == '/') {
            slash_p = cp;
        }
        cp++;
    }
    if((slash_p)) {
        return slash_p+1;
    }
    return filename;
}


char* restrict * restrict copy_string_array(const uint_fast32_t NUM_BEAMLETS,
                                            const char* const restrict * const restrict Array_Orig) throw()
{
    size_t total_string_size = 0;
    for(uint_fast32_t beamlet=0; beamlet < NUM_BEAMLETS; beamlet++) {
        total_string_size += strlen(Array_Orig[beamlet])+1;
    }

    char* const restrict data = reinterpret_cast<char* const restrict>(malloc(total_string_size));
    void** array_v = reinterpret_cast<void**>(malloc(NUM_BEAMLETS*sizeof(char*)));
    char* restrict * restrict array = reinterpret_cast<char* restrict * restrict>(array_v);
    if((data == NULL) || (array_v == NULL)) {
        DEF_LOGGING_ERROR("unable to malloc memory for %d elements, %d bytes\n",
                          int(NUM_BEAMLETS), int(total_string_size));
        free(data);
        free(array_v);
        return NULL;
    }

    // Copy over the strings, and
    // set the pointer to the 0th beamlet string to the start of data.
    // Note that this assumes that nobody has mucked with the original
    // strings since we checked their size --- beware in multithreaded systems.
    char* restrict next_string = data;
    for(uint_fast32_t beamlet=0; beamlet < NUM_BEAMLETS; beamlet++) {
        strcpy(next_string, Array_Orig[beamlet]);
        array[beamlet] = next_string;
        next_string += strlen(next_string)+1;
    }
    return array;
}



} // endnamespace Station
} // endnamespace LOFAR
} // endnamespace MPIfR


