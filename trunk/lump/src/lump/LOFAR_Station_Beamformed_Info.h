// LOFAR_Station_Beamformed_Info.h
// Base class for output Info information
//_HIST  DATE NAME PLACE INFO
// 2011 Aug 06  James M Anderson  --MPIfR  Start
// 2012 Feb 05  JMA  --strip path from filename_base
// 2012 May 12  JMA  --RCUMODE is now per beamlet
// 2013 Jan 05  JMA  --bug fixes for PFB work
// 2013 Jul 04  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2014 Sep 12  JMA  ---- update documentation to describe how to deal with
//                        station names for stations using ring splitters
//                        (to get CS001HBA0 and CS001HBA1, for example).

// Copyright (c) 2014, James M. Anderson <anderson@gfz-potsdam.de>

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



#ifndef LOFAR_Station_Beamformed_Info_H
#define LOFAR_Station_Beamformed_Info_H

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
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Common.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {






//_CLASS  LOFAR_Station_Beamformed_Info
class LOFAR_Station_Beamformed_Info {
//_DESC  full description of class

//_FILE  files used and logical units used

//_LIMS  design limitations

//_BUGS  known bugs

//_CALL  list of calls

//_KEYS  

//_HIST  DATE NAME PLACE INFO

//_END


// NAMESPACE ISSUES    


public:
    LOFAR_Station_Beamformed_Info(const char* const restrict filename_base_,
                                  const char* const restrict mode) throw();
    ~LOFAR_Station_Beamformed_Info() throw();

    bool read_init_was_valid() const throw() {return read_init_valid;}

    int write_to_file() throw();
    void print_as_text(FILE* fp, const char* const restrict separator) const throw();
    

    uint16_t get_VERSION() const throw() {return VERSION;}

    uint16_t get_WRITER_TYPE() const throw() {return WRITER_TYPE;}
    void set_WRITER_TYPE(uint16_t WRITER_TYPE_) throw() { WRITER_TYPE = WRITER_TYPE_;}

    int16_t get_RCUMODE() const throw() {return RCUMODE;}
    void set_RCUMODE(int16_t RCUMODE_) throw() { RCUMODE = RCUMODE_;}

    int16_t get_station_clock_frequency() const throw() {return station_clock_frequency;}
    void set_station_clock_frequency(int16_t station_clock_frequency_) throw() { station_clock_frequency = station_clock_frequency_;}

    const char* const restrict get_station_name() const throw() {return station_name;}
    void set_station_name(const char* const restrict name) throw() {
        strncpy(station_name,name,STATION_NAME_STR_SIZE);
        station_name[STATION_NAME_STR_SIZE-1] = 0;
    }

    const char* const restrict get_version_date() const throw() {return version_date;}
    void set_version_date(const char* const restrict date) throw() {
        strncpy(version_date,date,VERSION_DATE_STR_SIZE);
        version_date[VERSION_DATE_STR_SIZE-1] = 0;
    }


    int64_t get_time_reference_zero() const throw() {return time_reference_zero;}
    void set_time_reference_zero(int64_t time_reference_zero_) throw() { time_reference_zero = time_reference_zero_;}

    Real64_t get_seconds_per_sample() const throw() {return seconds_per_sample;}
    void set_seconds_per_sample(Real64_t seconds_per_sample_) throw() { seconds_per_sample = seconds_per_sample_;}

    Real64_t get_sampler_frequency() const throw() {return sampler_frequency;}
    void set_sampler_frequency(Real64_t sampler_frequency_) throw() { sampler_frequency = sampler_frequency_;}

    Real64_t get_Nyquist_offset() const throw() {return Nyquist_offset;}
    void set_Nyquist_offset(Real64_t Nyquist_offset_) throw() { Nyquist_offset = Nyquist_offset_;}

    uint32_t get_beamlets_per_sample() const throw() {return beamlets_per_sample;}
    void set_beamlets_per_sample(uint32_t beamlets_per_sample_) throw() { beamlets_per_sample = beamlets_per_sample_;}

    uint32_t get_GRONINGEN_SAMPLE_COUNT_PER_BLOCK() const throw() {return GRONINGEN_SAMPLE_COUNT_PER_BLOCK;}
    void set_GRONINGEN_SAMPLE_COUNT_PER_BLOCK(uint32_t GRONINGEN_SAMPLE_COUNT_PER_BLOCK_) throw() { GRONINGEN_SAMPLE_COUNT_PER_BLOCK = GRONINGEN_SAMPLE_COUNT_PER_BLOCK_;}

    int32_t get_station_DATA_TYPE() const throw() {return station_DATA_TYPE;}
    void set_station_DATA_TYPE(int32_t station_DATA_TYPE_) throw() { station_DATA_TYPE = station_DATA_TYPE_;}

    int32_t get_processing_DATA_TYPE() const throw() {return processing_DATA_TYPE;}
    void set_processing_DATA_TYPE(int32_t processing_DATA_TYPE_) throw() { processing_DATA_TYPE = processing_DATA_TYPE_;}

    int32_t get_output_DATA_TYPE() const throw() {return output_DATA_TYPE;}
    void set_output_DATA_TYPE(int32_t output_DATA_TYPE_) throw() { output_DATA_TYPE = output_DATA_TYPE_;}

    int32_t get_channelizer_TYPE() const throw() {return channelizer_TYPE;}
    void set_channelizer_TYPE(int32_t channelizer_TYPE_) throw() { channelizer_TYPE = channelizer_TYPE_;}

    Real64_t get_FFT_window_parameter() const throw() {return FFT_window_parameter;}
    void set_FFT_window_parameter(Real64_t FFT_window_parameter_) throw() {FFT_window_parameter = FFT_window_parameter_;}

    int32_t get_FFT_window_TYPE() const throw() {return FFT_window_TYPE;}
    void set_FFT_window_TYPE(int32_t FFT_window_TYPE_) throw() {FFT_window_TYPE = FFT_window_TYPE_;}
    
    uint32_t get_NUM_CHANNELS() const throw() {return NUM_CHANNELS;}
    void set_NUM_CHANNELS(uint32_t NUM_CHANNELS_) throw() { NUM_CHANNELS = NUM_CHANNELS_;}

    uint32_t get_NUM_POLYPHASE_TAPS() const throw() {return NUM_POLYPHASE_TAPS;}
    void set_NUM_POLYPHASE_TAPS(uint32_t NUM_POLYPHASE_TAPS_) throw() { NUM_POLYPHASE_TAPS = NUM_POLYPHASE_TAPS_;}

    uint32_t get_samples_per_channelization() const throw() {return samples_per_channelization;}
    void set_samples_per_channelization(uint32_t samples_per_channelization_) throw() { samples_per_channelization = samples_per_channelization_;}

    uint32_t get_samples_per_integration() const throw() {return samples_per_integration;}
    void set_samples_per_integration(uint32_t samples_per_integration_) throw() { samples_per_integration = samples_per_integration_;}

    Real64_t get_seconds_per_channelization_full() const throw() {return seconds_per_channelization_full;}
    void set_seconds_per_channelization_full(Real64_t seconds_per_channelization_full_) throw() { seconds_per_channelization_full = seconds_per_channelization_full_;}

    Real64_t get_seconds_per_channelization_eff() const throw() {return seconds_per_channelization_eff;}
    void set_seconds_per_channelization_eff(Real64_t seconds_per_channelization_eff_) throw() { seconds_per_channelization_eff = seconds_per_channelization_eff_;}

    Real64_t get_samples_per_channelization_eff() const throw() {return samples_per_channelization_eff;}
    void set_samples_per_channelization_eff(Real64_t samples_per_channelization_eff_) throw() { samples_per_channelization_eff = samples_per_channelization_eff_;}

    Real64_t get_seconds_per_integration_full() const throw() {return seconds_per_integration_full;}
    void set_seconds_per_integration_full(Real64_t seconds_per_integration_full_) throw() { seconds_per_integration_full = seconds_per_integration_full_;}

    Real64_t get_seconds_per_integration_eff() const throw() {return seconds_per_integration_eff;}
    void set_seconds_per_integration_eff(Real64_t seconds_per_integration_eff_) throw() { seconds_per_integration_eff = seconds_per_integration_eff_;}

    Real64_t get_samples_per_integration_eff() const throw() {return samples_per_integration_eff;}
    void set_samples_per_integration_eff(Real64_t samples_per_integration_eff_) throw() { samples_per_integration_eff = samples_per_integration_eff_;}

    Real64_t get_seconds_per_output() const throw() {return seconds_per_output;}
    void set_seconds_per_output(Real64_t seconds_per_output_) throw() { seconds_per_output = seconds_per_output_;}

    Real64_t get_integration_multiplier() const throw() {return integration_multiplier;}
    void set_integration_multiplier(Real64_t integration_multiplier_) throw() { integration_multiplier = integration_multiplier_;}

    int64_t get_sample_start() const throw() {return sample_start;}
    void set_sample_start(int64_t sample_start_) throw() { sample_start = sample_start_;}

    Real64_t get_time_center_output_zero() const throw() {return time_center_output_zero;}
    void set_time_center_output_zero(Real64_t time_center_output_zero_) throw() { time_center_output_zero = time_center_output_zero_;}

    uint64_t get_num_output_timeslots() const throw() {return num_output_timeslots;}
    void set_num_output_timeslots(uint64_t num_output_timeslots_) throw() { num_output_timeslots = num_output_timeslots_;}





    // Helper functions for other data

    // These six routines allocate memory with malloc.  The caller is responsible
    // for de-allocating the memory.  On error, these return NULL.
    uint_fast32_t* restrict read_beamlets_data(const char* const restrict filename_base) const throw()
    {return read_array_data_uint_fast32_t(filename_base,"beamlets");}
    uint_fast32_t* restrict read_subbands_data(const char* const restrict filename_base) const throw()
    {return read_array_data_uint_fast32_t(filename_base,"subbands");}
    uint_fast32_t* restrict read_rcumode_data(const char* const restrict filename_base) const throw()
    {return read_array_data_uint_fast32_t(filename_base,"rcumodes");}
    Real64_t* restrict read_rightascension_data(const char* const restrict filename_base) const throw()
    {return read_array_data_Real64_t(filename_base,"rightascensions");}
    Real64_t* restrict read_declination_data(const char* const restrict filename_base) const throw()
    {return read_array_data_Real64_t(filename_base,"declinations");}
    // Note that read_epoch_data returns a char**.  The memory was allocated
    // with two mallocs.  To free the memory, for char** epoch, do
    // free(epoch[0]);epoch[0] = NULL;free(epoch);epoch = NULL;
    char* restrict * restrict read_epoch_data(const char* const restrict filename_base) const throw()
    {return read_array_data_char(filename_base,"epochs");}
    // Note that read_sourcename_data returns a char**.  The memory was allocated
    // with two mallocs.  To free the memory, for char** epoch, do
    // free(epoch[0]);epoch[0] = NULL;free(epoch);epoch = NULL;
    char* restrict * restrict read_sourcename_data(const char* const restrict filename_base) const throw()
    {return read_array_data_char(filename_base,"sourcenames");}

    
    void write_beamlets_data(const char* const restrict filename_base,
                             const uint_fast32_t* const restrict Physical_Beamlets_Array) const throw()
    {
        return write_array_data(filename_base,Physical_Beamlets_Array,"beamlets");
    }
    void write_subbands_data(const char* const restrict filename_base,
                             const uint_fast32_t* const restrict Subband_Array) const throw()
    {
        return write_array_data(filename_base,Subband_Array,"subbands");
    }
    void write_rcumode_data(const char* const restrict filename_base,
                            const uint_fast32_t* const restrict RCUMODE_Array) const throw()
    {
        return write_array_data(filename_base,RCUMODE_Array,"rcumodes");
    }
    void write_rightascension_data(const char* const restrict filename_base,
                                   const Real64_t* const restrict RightAscension_Array) const throw()
    {
        return write_array_data(filename_base,RightAscension_Array,"rightascensions");
    }
    void write_declination_data(const char* const restrict filename_base,
                                const Real64_t* const restrict Declination_Array) const throw()
    {
        return write_array_data(filename_base,Declination_Array,"declinations");
    }
    void write_epoch_data(const char* const restrict filename_base,
                          const char* const restrict * const restrict Epoch_Array) const throw()
    {
        return write_array_data(filename_base,Epoch_Array,"epochs");
    }
    void write_sourcename_data(const char* const restrict filename_base,
                               const char* const restrict * const restrict SourceName_Array) const throw()
    {
        return write_array_data(filename_base,SourceName_Array,"sourcenames");
    }


    

    void append_to_beamlets_data(const char* const restrict filename_base,
                                 const uint_fast32_t beamlet) const throw()
    {
        return append_to_array_data(filename_base,beamlet,"beamlets");
    }
    void append_to_subbands_data(const char* const restrict filename_base,
                             const uint_fast32_t subband) const throw()
    {
        return append_to_array_data(filename_base,subband,"subbands");
    }
    void append_to_rcumode_data(const char* const restrict filename_base,
                                const uint_fast32_t rcumode) const throw()
    {
        return append_to_array_data(filename_base,rcumode,"rcumodes");
    }
    void append_to_rightascension_data(const char* const restrict filename_base,
                                       const Real64_t ra) const throw()
    {
        return append_to_array_data(filename_base,ra,"rightascensions");
    }
    void append_to_declination_data(const char* const restrict filename_base,
                                    const Real64_t dec) const throw()
    {
        return append_to_array_data(filename_base,dec,"declinations");
    }
    void append_to_epoch_data(const char* const restrict filename_base,
                              const char* const restrict epoch) const throw()
    {
        return append_to_array_data(filename_base,epoch,"epochs");
    }
    void append_to_sourcename_data(const char* const restrict filename_base,
                                   const char* const restrict sourcename) const throw()
    {
        return append_to_array_data(filename_base,sourcename,"sourcenames");
    }
    

protected:
    

private:
    static const size_t STATION_NAME_STR_SIZE = 8;
    static const size_t VERSION_DATE_STR_SIZE = 16;


    uint16_t VERSION;               // Software version
    uint16_t WRITER_TYPE;           // File type, from
                                    // LOFAR_DATA_OUTPUT_FILE_FORMAT
    int16_t RCUMODE;                // The RCUMODE of the measurement.
                                    // This is mainly specifying the filter
                                    // selection.
                                    // -1 Unknown or more than one type applies
                                    //  1 LBL  10--80 MHz,  200 MHz clock
                                    //  2 LBL  30--80 MHz,  200 MHz clock
                                    //  3 LBH  10--80 MHz,  200 MHz clock
                                    //  4 LBH  30--80 MHz,  200 MHz clock
                                    //  5 HBA 110--190 MHz, 200 MHz clock
                                    //  6 HBA 170--230 MHz, 160 MHz clock
                                    //  7 HBA 210--260 MHz, 200 MHz clock
                                    //  8 LBH  10--80 MHz,  160 MHz clock
                                    //  9 LBL  30--80 MHz,  160 MHz clock
    int16_t station_clock_frequency;// The station clock frequency, in MHz.
                                    // For LOFAR, this should either be 200
                                    // or 160.  For debug test runs, it is 0
                                    // -1 indicates an unknown value;
    char station_name[STATION_NAME_STR_SIZE]; // the name of the station
                                    // as a combination of VLBI and
                                    // LOFAR styles, so
                                    // EfDE601
                                    // is the name of Effelsberg.  The first
                                    // 2 characters are the VLBI name, the
                                    // next 5 characters are the LOFAR name.
                                    // Character 7 is 0.
                                    // Note that in order to properly determine
                                    // the station location of LOFAR stations
                                    // with ring splitters (such as the core
                                    // stations) that can split their output,
                                    // the LOFAR name will be changed such that
                                    // the second letter of the LOFAR station name
                                    // will refer to the segment of the ring
                                    // splitter.  So, CS001HBA0 would be
                                    // written as ??C0001 in the LuMP notation,
                                    // where ?? is some unknown VLBI letter
                                    // designation for the station (that must
                                    // be different from the non-ring-splitter
                                    // VLBI designation for the station, and
                                    // different from the VLBI code for
                                    // CS001HBA1, and so on.
    char version_date[VERSION_DATE_STR_SIZE]; // the date of compilation
                                    // of the data writer, as
                                    // YYYYMMMDD
                                    // were YYYY is the 4 digit year,
                                    // MMM is the 3 character month abbreviation,
                                    // and DD is the 2 digit day
    int64_t time_reference_zero;    // a time_t-like reference time as the
                                    // number of seconds since the Unix epoch
    Real64_t seconds_per_sample;    // the number of seconds per raw time step
    Real64_t sampler_frequency;     // The sampling clock frequency in Hz
    Real64_t Nyquist_offset;        // The Nyquist zone offset (local
                                    // oscillator) frequency, in Hz.
                                    // If the value is -1.0, then more than one
                                    // Nyquist offset applies, and the Nyquist
                                    // offset must be calculated from the
                                    // RCUMODE per beamlet
    uint32_t beamlets_per_sample;   // How many beamlets are present
    uint32_t GRONINGEN_SAMPLE_COUNT_PER_BLOCK;
                                    // The number of time samples per block
    int32_t station_DATA_TYPE;      // The data type of the raw beamformed
                                    // data from the station,
                                    // as a LOFAR_raw_data_type_enum
    int32_t processing_DATA_TYPE;   // The data type of the intermediate
                                    // processing steps, as a
                                    // LOFAR_raw_data_type_enum
    int32_t output_DATA_TYPE;       // The data type of the output integration
                                    // points, as a LOFAR_raw_data_type_enum
    int32_t channelizer_TYPE;       // The type of channelizer used, as a
                                    // LOFAR_Channelizer_Enum
    Real64_t FFT_window_parameter;  // The FFT window parameter used
                                    // (this typically controls the width)
    int32_t FFT_window_TYPE;        // The type of FFT windowing applied
                                    // for channelization
    uint32_t NUM_CHANNELS;          // The number of channels per beamlet
    uint32_t NUM_POLYPHASE_TAPS;    // The number of taps used for the
                                    // polyphase filter, should it be used
    uint32_t samples_per_channelization; //The full number of samples per
                                    // channelization step.  For a polyphase
                                    // filter, this is the full width of the
                                    // window, in samples
    uint32_t samples_per_integration; //The full number of raw samples per
                                    // integration step.  With channelization,
                                    // this is the full width of the
                                    // channelization and integration together.
    Real64_t seconds_per_channelization_full; //The full width of the time per
                                    // channelization in seconds.  This is just
                                    // samples_per_channelization
                                    // * seconds_per_sample, provided for
                                    // convenience.
    Real64_t seconds_per_channelization_eff; //The effective width of the time
                                    // per channelization in seconds.  This takes
                                    // into account the weighting of the samples.
    Real64_t samples_per_channelization_eff; //The effective number of samples
                                    // that are used to make the channelization,
                                    // allowing for weighting of the samples.
                                    // This might be useful for noise statistics.
    Real64_t seconds_per_integration_full; //The full width of the integration
                                    // time in seconds, including channelization
                                    // if that was done.  This is just
                                    // seconds_per_sample *
                                    // samples_per_integration, provided for
                                    // user convenience
    Real64_t seconds_per_integration_eff; //The effective width of the time
                                    // per integration in seconds.  This takes
                                    // into account the weighting of the samples.
    Real64_t samples_per_integration_eff; //The effective number of samples
                                    // that are used to make the integration,
                                    // allowing for weighting of the samples.
                                    // This might be useful for noise statistics.
    Real64_t seconds_per_output;    // This is the time in seconds between each
                                    // output value.  This is the key time to look
                                    // at when calculating the timestamps of
                                    // individual output data-points.
    Real64_t integration_multiplier; //The scaling factor by which all
                                    // of the integration points have been
                                    // multiplied prior to recording to
                                    // output.  The output points are thus
                                    // integration_multiplier \times
                                    // \sum_i^{samples_per_integration}
                                    // X_i X_i^\ast
                                    // for the X power, and so on for Y and
                                    // the cross terms.
                                    // This multiplier may also be used for
                                    // voltage spectrum writers (such as FFT
                                    // and polyphase filters), where the
                                    // raw voltages have been multiplied
                                    // by integration_multiplier.
    int64_t sample_start;           // The sample number of the 0th time sample
                                    // used for the initial integration,
                                    // as samples since the reference_time_zero.
                                    // Note that this may be negative, as when
                                    // using a polyphase filterbank to make
                                    // channels the first few channelizations
                                    // are done with blank data from before
                                    // recording started.
    Real64_t time_center_output_zero; //The center time in seconds
                                    // since time_reference_zero of the zeroeth
                                    // output point.
                                    // For raw voltage output, this is just
                                    // sample_start * seconds_per_sample
                                    // For channelizers and integrators,
                                    // this is just
                                    // sample_start * seconds_per_sample
                                    // + seconds_per_integration / 2
                                    // - seconds_per_sample / 2,
                                    // again, provided as a user convenience
    uint64_t num_output_timeslots;  // The total number of time points
                                    // recorded in the data files.


    bool read_init_valid;
    bool is_writeable;
    static const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    FILE* fp;


    int write_0001() throw();
    void read_0001() throw();

    int write_0002() throw();
    void read_0002() throw();


    void write_array_data(const char* const restrict filename_base,
                          const char* const restrict * const restrict array,
                          const char* const restrict data_type)
        const throw();
    void write_array_data(const char* const restrict filename_base,
                          const uint_fast32_t* const restrict array,
                          const char* const restrict data_type)
        const throw();
    void write_array_data(const char* const restrict filename_base,
                          const Real64_t* const restrict array,
                          const char* const restrict data_type)
        const throw();
    void append_to_array_data(const char* const restrict filename_base,
                              const char* const restrict value,
                              const char* const restrict data_type)
        const throw();
    void append_to_array_data(const char* const restrict filename_base,
                              const uint_fast32_t value,
                              const char* const restrict data_type)
        const throw();
    void append_to_array_data(const char* const restrict filename_base,
                              const Real64_t value,
                              const char* const restrict data_type)
        const throw();
    char* restrict * restrict read_array_data_char(const char* const restrict filename_base,
                                                 const char* const restrict data_type) const throw();
    uint_fast32_t* restrict read_array_data_uint_fast32_t(const char* const restrict filename_base,
                                                          const char* const restrict data_type) const throw();
    Real64_t* restrict read_array_data_Real64_t(const char* const restrict filename_base,
                                                     const char* const restrict data_type) const throw();
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS



const char * const strip_path(const char * const restrict filename) throw();
char* restrict * restrict copy_string_array(const uint_fast32_t NUM_BEAMLETS,
                                            const char* const restrict * const restrict Array_Orig) throw();




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Info_H
