// LOFAR_Station_Beamformed_Writer_P_FFT1.h
// Derived class for writing FFT spectra power, case 1
//_HIST  DATE NAME PLACE INFO
// 2014 Sep 03  James M Anderson  --GFZ  start from LOFAR_Station_Beamformed_Writer_P_FFT0.h
// 2014 Sep 04  JMA  ---- rename FFT_samples to FFT_blocks

// Copyright (c) 2012, 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



#ifndef LOFAR_Station_Beamformed_Writer_P_FFT1_H
#define LOFAR_Station_Beamformed_Writer_P_FFT1_H

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
#include "LOFAR_Station_Beamformed_Writer_Base.h"
#include <stdio.h>
#include <stdlib.h>
#include "LOFAR_Station_Beamformed_Valid_Real32.h"
#include "MPIfR_Numerical_sampling_windows.h"
#include "MPIfR_FFTW3.h"








// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {





//_CLASS  LOFAR_Station_Beamformed_Writer_P_FFT1
class LOFAR_Station_Beamformed_Writer_P_FFT1 :
        public LOFAR_Station_Beamformed_Writer_Base {
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
    LOFAR_Station_Beamformed_Writer_P_FFT1(const char* const restrict filename_base_,
                                           const char* const restrict station_name_,
                                           const uint32_t reference_time_,
                                           const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                           const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                           const uint_fast16_t NUM_Beamlets_,
                                           const uint_fast16_t NUM_Blocks_,
                                           const LOFAR_raw_data_type_enum DATA_TYPE_,
                                           const int32_t id_,
                                           const uint_fast32_t NUM_OUTPUT_CHANNELS_,
                                           const uint_fast16_t NUM_Output_Beamlets_,
                                           const uint_fast32_t* const restrict Physical_Beamlets_Array_,
                                           const uint_fast32_t* const restrict Physical_Subband_Array_,
                                           const uint_fast32_t* const restrict RCUMODE_Array_,
                                           const Real64_t* const restrict RightAscension_Array_,
                                           const Real64_t* const restrict Declination_Array_,
                                           const char* const * const restrict Epoch_Array_,
                                           const char* const * const restrict SourceName_Array_,
                                           const LOFAR_raw_data_type_enum data_type_process_,
                                           const LOFAR_raw_data_type_enum data_type_out_,
                                           const MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window_type_,
                                           const Real64_t window_parameter_,
                                           const Real64_t multiplier_,
                                           const Real64_t desired_seconds_per_integration_,
                                           const uint8_t scale_by_inverse_num_samples_,
                                           const uint8_t bounds_check_output_data_type_
                                         )
        throw();
    
    virtual ~LOFAR_Station_Beamformed_Writer_P_FFT1();


protected:
    FILE* restrict fp_data;
    
    uint_fast64_t num_raw_packet_commands_processed;
    uint_fast64_t num_spectrum_points_processed;
    uint_fast64_t num_integration_points_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    // FFT workspace
    void* window_storage;
    void* fft_in_storage;
    void* fft_out_storage;
    // FFT plans
    fftwf_plan plan_32;
    fftw_plan plan_64;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    fftwl_plan plan_80;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    fftwq_plan plan_128;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    // Integration workspace
    void* integration_storage;
    // Output workspace
    void* output_storage;
    //
    LOFAR_Station_Beamformed_Valid_Real32_Writer* restrict valid_object;
    // FFT variables/constants
    const Real64_t WINDOW_PARAMETER;
    const Real64_t SCALING_MULTIPLIER;
    uint_fast64_t FFT_BUFFER_LENGTH_CHAR;
    uint_fast32_t samples_in_this_FFT;
    uint_fast32_t valid_samples_in_this_FFT;
    uint_fast32_t NUM_REALS_IN_FFT_BLOCK;
    // Integration variables/constants
    uint_fast64_t INTEGRATION_BUFFER_LENGTH_CHAR;
    uint_fast64_t OUTPUT_BUFFER_LENGTH_CHAR;
    uint_fast32_t FFT_BLOCKS_PER_INTEGRATION;
    uint_fast32_t FFT_blocks_in_this_integration;
    uint_fast32_t valid_FFT_blocks_in_this_integration;
    uint_fast32_t bounds_check_output_data_type;
    void* weighted_valid_FFT_blocks_in_this_integration_p;
    const uint_fast64_t NUM_REALS_IN_FULL_INTEGRATION_BUFFER;
    const Real64_t desired_seconds_per_integration;
    const uint8_t scale_by_inverse_num_samples;
    
    static const uint_fast16_t NUM_OUTPUT_FIELDS = 4;

    const uint_fast16_t NUM_PARALLEL_BEAMLETS; // NUM_Output_Beamlets * NUM_RSP_BEAMLET_POLARIZATIONS


    LOFAR_raw_data_type_enum processing_DATA_TYPE_REAL;
    LOFAR_raw_data_type_enum processing_DATA_TYPE_COMPLEX;
    LOFAR_raw_data_type_enum output_DATA_TYPE_REAL;
    const MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window_TYPE;

    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();





    template<class Tin, class Tproc> int_fast32_t write_complex_to_fft_in() restrict throw()
    {
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[packet]->data_start_const());
            Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(fft_in_storage);
            uint_fast32_t sample_offset = 0;
            while(sample_offset < NUM_Blocks) {
                Tproc* restrict data_out_start = data_out + (samples_in_this_FFT<<1);
            
                uint_fast32_t samples_to_copy = NUM_Blocks - sample_offset;
                if(samples_in_this_FFT+samples_to_copy > NUM_OUTPUT_CHANNELS) {
                    samples_to_copy = NUM_OUTPUT_CHANNELS - samples_in_this_FFT;
                }
                uint_fast32_t reals_to_copy = samples_to_copy << 1;
                for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                    const Tin* restrict data_in_beamlet = data_in + (sample_offset<<1)
                        + Beamlet_Offsets_Array[beamlet];
                    Tproc* restrict data_out_s = data_out_start;
                    for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                        data_out_s[r+0] = Tproc(data_in_beamlet->real());
                        data_out_s[r+1] = Tproc(data_in_beamlet->imag());
                        data_in_beamlet++;
                        data_out_s[NUM_REALS_IN_FFT_BLOCK+r+0] = Tproc(data_in_beamlet->real());
                        data_out_s[NUM_REALS_IN_FFT_BLOCK+r+1] = Tproc(data_in_beamlet->imag());
                        data_in_beamlet++;
                    }
                    data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
                }
                {
                    // Now fill in the valid information into a time series
                    const Tproc valid_value( ((current_valid[packet])) ? 1.0:0.0);
                    Tproc* restrict data_out_s = data_out_start;
                    for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                        data_out_s[r] = valid_value;
                    }
                    data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK);
                }
                samples_in_this_FFT += samples_to_copy;
                sample_offset += samples_to_copy;
                if(current_valid[packet]) {
                    valid_samples_in_this_FFT += samples_to_copy;
                }
                if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                    int_fast32_t retcode = FFT_block<Tproc>();
                    if(retcode == 0) {
                        samples_in_this_FFT = 0;
                        valid_samples_in_this_FFT = 0;
                    }
                    else {
                        DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(packet), (unsigned int)(current_num_packets));
                        return retcode;
                    }
                }
            }
        }
        return 0;
    }



    template<class Tin, class Tproc> int_fast32_t write_real_to_fft_in() restrict throw()
    {
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[packet]->data_start_const());
            Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(fft_in_storage);
            uint_fast32_t sample_offset = 0;
            while(sample_offset < NUM_Blocks) {
                Tproc* restrict data_out_start = data_out + (samples_in_this_FFT<<1);
            
                uint_fast32_t samples_to_copy = NUM_Blocks - sample_offset;
                if(samples_in_this_FFT+samples_to_copy > NUM_OUTPUT_CHANNELS) {
                    samples_to_copy = NUM_OUTPUT_CHANNELS - samples_in_this_FFT;
                }
                uint_fast32_t reals_to_copy = samples_to_copy << 1;
                for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                    const Tin* restrict data_in_beamlet = data_in + (sample_offset<<2)
                        + (Beamlet_Offsets_Array[beamlet]<<1);
                    Tproc* restrict data_out_s = data_out_start;
                    for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                        data_out_s[r+0] = Tproc(*data_in_beamlet++);
                        data_out_s[r+1] = Tproc(*data_in_beamlet++);
                        data_out_s[NUM_REALS_IN_FFT_BLOCK+r+0] = Tproc(*data_in_beamlet++);
                        data_out_s[NUM_REALS_IN_FFT_BLOCK+r+1] = Tproc(*data_in_beamlet++);
                    }
                    data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
                }
                {
                    // Now fill in the valid information into a time series
                    const Tproc valid_value( ((current_valid[packet])) ? 1.0:0.0);
                    Tproc* restrict data_out_s = data_out_start;
                    for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                        data_out_s[r] = valid_value;
                    }
                    data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK);
                }
                samples_in_this_FFT += samples_to_copy;
                sample_offset += samples_to_copy;
                if(current_valid[packet]) {
                    valid_samples_in_this_FFT += samples_to_copy;
                }
                if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                    int_fast32_t retcode = FFT_block<Tproc>();
                    if(retcode == 0) {
                        samples_in_this_FFT = 0;
                        valid_samples_in_this_FFT = 0;
                    }
                    else {
                        DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(packet), (unsigned int)(current_num_packets));
                        return retcode;
                    }
                }
            }
        }
        return 0;
    }



    template<class Tin, class Tproc> int_fast32_t write_complex_to_fft_in_partial(const uint_fast32_t NUM_ACTUAL_BLOCKS) restrict throw()
    {
        const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[0]->data_start_const());
        Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(fft_in_storage);
        uint_fast32_t sample_offset = 0;
        while(sample_offset < NUM_ACTUAL_BLOCKS) {
            Tproc* restrict data_out_start = data_out + (samples_in_this_FFT<<1);
            
            uint_fast32_t samples_to_copy = NUM_ACTUAL_BLOCKS - sample_offset;
            if(samples_in_this_FFT+samples_to_copy > NUM_OUTPUT_CHANNELS) {
                samples_to_copy = NUM_OUTPUT_CHANNELS - samples_in_this_FFT;
            }
            uint_fast32_t reals_to_copy = samples_to_copy << 1;
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const Tin* restrict data_in_beamlet = data_in + (sample_offset<<1)
                    + Beamlet_Offsets_Array[beamlet];
                Tproc* restrict data_out_s = data_out_start;
                for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                    data_out_s[r+0] = Tproc(data_in_beamlet->real());
                    data_out_s[r+1] = Tproc(data_in_beamlet->imag());
                    data_in_beamlet++;
                    data_out_s[NUM_REALS_IN_FFT_BLOCK+r+0] = Tproc(data_in_beamlet->real());
                    data_out_s[NUM_REALS_IN_FFT_BLOCK+r+1] = Tproc(data_in_beamlet->imag());
                    data_in_beamlet++;
                }
                data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK);
            }
            {
                // Now fill in the valid information into a time series
                const Tproc valid_value( ((current_valid[0])) ? 1.0:0.0);
                Tproc* restrict data_out_s = data_out_start;
                for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                    data_out_s[r] = valid_value;
                }
                data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
            }
            samples_in_this_FFT += samples_to_copy;
            sample_offset += samples_to_copy;
            if(current_valid[0]) {
                valid_samples_in_this_FFT += samples_to_copy;
            }
            if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                int_fast32_t retcode = FFT_block<Tproc>();
                if(retcode == 0) {
                    samples_in_this_FFT = 0;
                    valid_samples_in_this_FFT = 0;
                }
                else {
                    DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                    return retcode;
                }
            }
        }
        return 0;
    }



    template<class Tin, class Tproc> int_fast32_t write_real_to_fft_in_partial(const uint_fast32_t NUM_ACTUAL_BLOCKS) restrict throw()
    {
        const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[0]->data_start_const());
        Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(fft_in_storage);
        uint_fast32_t sample_offset = 0;
        while(sample_offset < NUM_ACTUAL_BLOCKS) {
            Tproc* restrict data_out_start = data_out + (samples_in_this_FFT<<1);
            
            uint_fast32_t samples_to_copy = NUM_ACTUAL_BLOCKS - sample_offset;
            if(samples_in_this_FFT+samples_to_copy > NUM_OUTPUT_CHANNELS) {
                samples_to_copy = NUM_OUTPUT_CHANNELS - samples_in_this_FFT;
            }
            uint_fast32_t reals_to_copy = samples_to_copy << 1;
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                const Tin* restrict data_in_beamlet = data_in + (sample_offset<<2)
                    + (Beamlet_Offsets_Array[beamlet]<<1);
                Tproc* restrict data_out_s = data_out_start;
                for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                    data_out_s[r+0] = Tproc(*data_in_beamlet++);
                    data_out_s[r+1] = Tproc(*data_in_beamlet++);
                    data_out_s[NUM_REALS_IN_FFT_BLOCK+r+0] = Tproc(*data_in_beamlet++);
                    data_out_s[NUM_REALS_IN_FFT_BLOCK+r+1] = Tproc(*data_in_beamlet++);
                }
                data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
            }
            {
                // Now fill in the valid information into a time series
                const Tproc valid_value( ((current_valid[0])) ? 1.0:0.0);
                Tproc* restrict data_out_s = data_out_start;
                for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                    data_out_s[r] = valid_value;
                }
                data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK);
            }
            samples_in_this_FFT += samples_to_copy;
            sample_offset += samples_to_copy;
            if(current_valid[0]) {
                valid_samples_in_this_FFT += samples_to_copy;
            }
            if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                int_fast32_t retcode = FFT_block<Tproc>();
                if(retcode == 0) {
                    samples_in_this_FFT = 0;
                    valid_samples_in_this_FFT = 0;
                }
                else {
                    DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                    return retcode;
                }
            }
        }
        return 0;
    }


    
    template<class Tproc> int_fast32_t write_real_to_fft_in_flush(uint_fast32_t* const restrict num_flushed) restrict throw()
    {
        // End of data, flush FFT buffer out
        *num_flushed = 0;
        if(samples_in_this_FFT == 0) {
            // FFT buffer empty already
            return 0;
        }
        Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(fft_in_storage);
        Tproc* restrict data_out_start = data_out + (samples_in_this_FFT<<1);
            
        uint_fast32_t samples_to_copy = NUM_OUTPUT_CHANNELS - samples_in_this_FFT;
        uint_fast32_t reals_to_copy = samples_to_copy << 1;
        Tproc ZERO(0);
        for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
            Tproc* restrict data_out_s = data_out_start;
            for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                data_out_s[r+0] = ZERO;
                data_out_s[r+1] = ZERO;
                data_out_s[NUM_REALS_IN_FFT_BLOCK+r+0] = ZERO;
                data_out_s[NUM_REALS_IN_FFT_BLOCK+r+1] = ZERO;
            }
            data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
        }
        {
            // Now fill in the valid information into a time series
            Tproc* restrict data_out_s = data_out_start;
            for(uint_fast16_t r = 0; r < reals_to_copy; r+=2) {
                data_out_s[r] = ZERO;
            }
            data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK);
        }
        samples_in_this_FFT += samples_to_copy;
        int_fast32_t retcode = FFT_block<Tproc>();
        if(retcode == 0) {
            samples_in_this_FFT = 0;
            valid_samples_in_this_FFT = 0;
        }
        else {
            DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
            return retcode;
        }
        *num_flushed = samples_to_copy;
    
        return 0;
    }


    template<class Tproc> int_fast32_t add_FFT_to_integration(const Tproc Weighted_Valid_Sum) restrict throw()
    {
        // First, if some of the data points were non-valid, then
        // the integration needs to be scaled by the square of the
        // Weighted_Valid_Sum (as the inverse of the Weighted_Valid_Sum
        // is used to scale the voltage values, the power gets scaled
        // as the inverse square of the Weighted_Valid_Sum for this
        // portion of the integration).  Thus, the weight of this block
        // for the integration is Weighted_Valid_Sum^2.
        const Tproc valid_scale_factor = (valid_samples_in_this_FFT != NUM_OUTPUT_CHANNELS) ? (Weighted_Valid_Sum*Weighted_Valid_Sum) : 1.0;
        // Real32_t fraction_valid((valid_samples_in_this_FFT==NUM_OUTPUT_CHANNELS) ? 1.0f : Real64_t(valid_samples_in_this_FFT)/Real64_t(NUM_OUTPUT_CHANNELS));
        // DEF_LOGGING_DEBUG("valid value = %+14.7E for writer %d, simple estimate was %+14.7E", Weighted_Valid_Sum, id, Real32_t((valid_samples_in_this_FFT==NUM_OUTPUT_CHANNELS) ? 1.0f : Real64_t(valid_samples_in_this_FFT)/Real64_t(NUM_OUTPUT_CHANNELS)));
        
        // The data come in as fft_out[beamlet][pol][ch].
        // The integration array should have the form
        //     int_storage[beamlet][ch][pol_index]

        // Only add this FFT block in when the FFT block had data.
        if(valid_samples_in_this_FFT > 0) {
            const Tproc* restrict data_in_pol0 = reinterpret_cast<Tproc* const restrict>(fft_out_storage);
            const Tproc* restrict data_in_pol1 = data_in_pol0 + NUM_REALS_IN_FFT_BLOCK;
            Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(integration_storage);
            
            for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
                for(uint_fast32_t channel=0; channel < NUM_OUTPUT_CHANNELS; channel++) {
                    Tproc Xr = *data_in_pol0++;
                    Tproc Xi = *data_in_pol0++;
                    Tproc Yr = *data_in_pol1++;
                    Tproc Yi = *data_in_pol1++;
                    // Output is X X^*, Y Y^*, \Real(X Y^*), \Imag(X Y^*)
                    *data_out++ += Xr*Xr + Xi*Xi;
                    *data_out++ += Yr*Yr + Yi*Yi;
                    *data_out++ += Xr*Yr + Xi*Yi;
                    *data_out++ += Xi*Yr - Xr*Yi;
                }
                // advance input pointers to account for polarization steps
                data_in_pol0 += NUM_REALS_IN_FFT_BLOCK;
                data_in_pol1 += NUM_REALS_IN_FFT_BLOCK;
            }
            Tproc* iwp = static_cast<Tproc*>(weighted_valid_FFT_blocks_in_this_integration_p);
            *iwp = *iwp + valid_scale_factor;
        }
        if(valid_samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
            valid_FFT_blocks_in_this_integration++;
        }
        FFT_blocks_in_this_integration++;

        if(FFT_blocks_in_this_integration == FFT_BLOCKS_PER_INTEGRATION) {
            return dump_integration_to_output<Tproc>();
        }
        return 0;
    }

    template<class Tproc> int_fast32_t dump_integration_to_output() restrict throw()
    {
        Tproc* iwp = static_cast<Tproc*>(weighted_valid_FFT_blocks_in_this_integration_p);
        Real32_t fraction_valid((valid_FFT_blocks_in_this_integration==FFT_BLOCKS_PER_INTEGRATION) ? 1.0f : (*iwp)/Real64_t(FFT_BLOCKS_PER_INTEGRATION));
        const Tproc SAMPLE_SCALING((*iwp > Tproc(0)) ?
                                   ( (scale_by_inverse_num_samples) ?
                                     1.0/(*iwp)
                                     : FFT_BLOCKS_PER_INTEGRATION/(*iwp) )
                                   : 0.0);
        
        if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
        }
        else {
            goto write_data_status_bad;
        }
        if(processing_DATA_TYPE_REAL == output_DATA_TYPE_REAL) {
            Tproc* const restrict data_in = reinterpret_cast<Tproc* const restrict>(integration_storage);
            for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                data_in[i] *= SAMPLE_SCALING;
            }
            // Write out integration_storage buffer directly
            if(fwrite(integration_storage, OUTPUT_BUFFER_LENGTH_CHAR, 1, fp_data) == 1) {
            }
            else {
                goto write_data_error;
            }
        }
        else {
            // convert data format, then write
            const Tproc* const restrict data_in = reinterpret_cast<const Tproc* const restrict>(integration_storage);
            switch(output_DATA_TYPE_REAL) {
            case L_Real16_t:
                {
                    MPIfR::DATA_TYPE::Real16_t* const restrict data_out = reinterpret_cast<MPIfR::DATA_TYPE::Real16_t* const restrict>(output_storage);
                    for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                        data_out[i] = MPIfR::DATA_TYPE::Real16_t(data_in[i] * SAMPLE_SCALING);
                    }
                }
                break;
            case L_Real32_t:
                {
                    Real32_t* const restrict data_out = reinterpret_cast<Real32_t* const restrict>(output_storage);
                    for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                        data_out[i] = Real32_t(data_in[i] * SAMPLE_SCALING);
                    }
                }
                break;
            case L_Real64_t:
                {
                    Real64_t* const restrict data_out = reinterpret_cast<Real64_t* const restrict>(output_storage);
                    for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                        data_out[i] = Real64_t(data_in[i] * SAMPLE_SCALING);
                    }
                }
                break;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
            case L_Real80_t:
                {
                    Real80_t* const restrict data_out = reinterpret_cast<Real80_t* const restrict>(output_storage);
                    for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                        data_out[i] = Real80_t(data_in[i] * SAMPLE_SCALING);
                    }
                }
                break;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
            case L_Real128_t:
                {
                    Real128_t* const restrict data_out = reinterpret_cast<Real128_t* const restrict>(output_storage);
                    for(uint_fast64_t i=0; i < NUM_REALS_IN_FULL_INTEGRATION_BUFFER; i++) {
                        data_out[i] = Real128_t(data_in[i] * SAMPLE_SCALING);
                    }
                }
                break;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
            default:
                DEF_LOGGING_CRITICAL("output data type %d (%s) not supported.  Contact your developer.\n", int(output_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(output_DATA_TYPE_REAL));
                exit(2);
            }

            if(fwrite(output_storage, OUTPUT_BUFFER_LENGTH_CHAR, 1, fp_data) == 1) {
            }
            else {
                goto write_data_error;
            }
        } // end of processing_DATA_TYPE_REAL != output_DATA_TYPE_REAL
        if(valid_object->write_valid(fraction_valid) == 0){
        }
        else {
            goto write_data_error;
        }

        // increment counter and clear processing area
        num_integration_points_processed++;
        FFT_blocks_in_this_integration = 0;
        valid_FFT_blocks_in_this_integration = 0;
        *iwp = 0.0;
        // the following call works for all IEEE floating point types
        memset(integration_storage,0,INTEGRATION_BUFFER_LENGTH_CHAR);
        
        return status;
    write_data_error:
        DEF_LOGGING_ERROR("Error writing to file base '%s'\n", filename_base);
        close_output_files();
        status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
        return status;
    write_data_status_bad:
        status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
        return status;
    }












    



    
private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_POWER_FFT1_OUT);}




    int_fast32_t check_input_parameters() restrict throw();
    int_fast32_t set_up_work_buffer_areas() restrict throw();
    int_fast32_t set_up_FFT_window() restrict throw();
    int_fast32_t set_up_FFTW3() restrict throw();
    int_fast32_t open_standard_files() restrict throw();
    int_fast32_t shut_down_FFTW3() restrict throw();

    int_fast32_t thread_constructor() restrict;
    int_fast32_t thread_destructor() restrict;
    int_fast32_t close_output_files() throw();
    int_fast32_t report_file_error(const char* const restrict msg) const throw();
    int_fast32_t write_header_init() throw();
    int_fast32_t write_header_start() throw();
    int_fast32_t write_header_end() throw();
    int_fast32_t write_standard_packets() throw();
    int_fast32_t write_partial_packet() throw();


    int_fast32_t write_integration_flush() throw();






    template<typename Tproc> int_fast32_t FFT_block() restrict throw()
    {
        // multiply by the window function
        const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(window_storage);
        Tproc* restrict fft_in = reinterpret_cast<Tproc* restrict>(fft_in_storage);
        for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
            for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
                *fft_in++ *= window[i];
            }
        }

        // Also multiply the valid information by the window function, and
        // accumulate the sum
        Tproc Weighted_Valid_Sum(0.0);
        // The valid values are stored in the fft_in_storeage buffer past the end
        // of the actual beamlet data.  We arrived at the end of the actual data
        // above.  Correct for the window mucking to get the zero frequency
        // at the center.
        for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i+=4) {
            Weighted_Valid_Sum += fft_in[i  ] * window[i  ];
            Weighted_Valid_Sum -= fft_in[i+2] * window[i+2];
        }
        
        
        // do the FFT
        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
        case L_Real32_t:
            fftwf_execute(plan_32);
            break;
        case L_Real64_t:
            fftw_execute(plan_64);
            break;    
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        case L_Real80_t:
            fftwl_execute(plan_80);
            break;    
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        case L_Real128_t:
            fftwq_execute(plan_128);
            break;    
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        default:
            DEF_LOGGING_CRITICAL("processing data type %d (%s) not supported.  Contact your developer.\n", int(processing_DATA_TYPE_REAL), LOFAR_raw_data_type_enum_Str(processing_DATA_TYPE_REAL));
            exit(2);
        }
        
        ++num_spectrum_points_processed;
        return add_FFT_to_integration<Tproc>(Weighted_Valid_Sum);
    }





    
    // prevent copying
    LOFAR_Station_Beamformed_Writer_P_FFT1(const LOFAR_Station_Beamformed_Writer_P_FFT1& a);
    LOFAR_Station_Beamformed_Writer_P_FFT1& operator=(const LOFAR_Station_Beamformed_Writer_P_FFT1& a);

    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Writer_P_FFT1_H
