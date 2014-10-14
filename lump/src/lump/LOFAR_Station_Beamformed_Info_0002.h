// LOFAR_Station_Beamformed_Info_0002.h
// Class for output Info information version 0x0002
//_HIST  DATE NAME PLACE INFO
// 2012 Apr 13  James M Anderson  --MPIfR  start based on 0001
// 2012 Apr 19  JMA  -- add version_date string
// 2012 May 12  JMA  --RCUMODE is now per beamlet

// Copyright (c) 2012, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_Info_0002_H
#define LOFAR_Station_Beamformed_Info_0002_H

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
#include <stdio.h>
#include <string.h>











// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {




//_CLASS  LOFAR_Station_Beamformed_Info_0002
class LOFAR_Station_Beamformed_Info_0002 {
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
    LOFAR_Station_Beamformed_Info_0002() throw() {memset(reinterpret_cast<void*>(this),0,sizeof(LOFAR_Station_Beamformed_Info_0002));};
    LOFAR_Station_Beamformed_Info_0002(FILE* fp) throw();

    size_t write_to_file(FILE* fp) throw();
    
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
                                    // or 160.  For debug test runts, it is 0
                                    // -1 indicates an unknown value;
    char station_name[STATION_NAME_STR_SIZE]; // the name of the station
                                    // as a combination of VLBI and
                                    // LOFAR styles, so
                                    // EfDE601
                                    // is the name of Effelsberg.  The first
                                    // 2 characters are the VLBI name, the
                                    // next 5 characters are the LOFAR name.
                                    // Character 7 is 0
    char version_date[VERSION_DATE_STR_SIZE]; // the date of compilation
                                    // of the data writer, as
                                    // YYYYMMMDD
                                    // were YYYY is the 4 digit year,
                                    // MMM is the 3 character month abbreviation,
                                    // and DD is the 2 digit day
    int64_t time_reference_zero;    // a time_t-like reference time as the
                                    // number of seconds since the Unix epoch
    Real64_t seconds_per_sample;    // the number of seconds per time step
    Real64_t sampler_frequency;     // The sampling clock frequency in Hz
    Real64_t Nyquist_offset;        // The Nyquist zone offset (local
                                    // oscillator) frequency, in Hz
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
    uint32_t PAD1;
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
                                    // This is just
                                    // sample_start * seconds_per_sample
                                    // + seconds_per_integration / 2
                                    // - seconds_per_sample / 2,
                                    // again, provided as a user convenience
    uint64_t num_output_timeslots;  // The total number of time points
                                    // recorded in the data files.

                                                                

protected:
    void byteswap() throw();


private:


    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Info_0002_H
