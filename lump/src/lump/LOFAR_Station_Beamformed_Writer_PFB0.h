// LOFAR_Station_Beamformed_Writer_PFB0.h
// Derived class for writing polyphase filterbank spectra, case 0
//_HIST  DATE NAME PLACE INFO
// 2012 Dec 18  James M Anderson  --MPIfR  Start
// 2013 Jan 05  JMA  --finish and bug fixes
// 2013 Jan 12  JMA  --more fixes
// 2013 Feb 12  JMA  ---- minor optimizations
// 2013 Mar 09  JMA  ---- change to use uint32_t for LOFAR timestamp
// 2013 Jul 07  JMA  ---- add RA,Dec,Epoch info
// 2013 Jul 07  JMA  ---- add SourceName info
// 2014 Jul 13  JMA  ---- Bugfixes to make the PFB taps work right


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



#ifndef LOFAR_Station_Beamformed_Writer_PFB0_H
#define LOFAR_Station_Beamformed_Writer_PFB0_H

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





//_CLASS  LOFAR_Station_Beamformed_Writer_PFB0
class LOFAR_Station_Beamformed_Writer_PFB0 :
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
    LOFAR_Station_Beamformed_Writer_PFB0(const char* const restrict filename_base_,
                                         const char* const restrict station_name_,
                                         const uint32_t reference_time_,
                                         const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                         const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                         const uint_fast16_t NUM_Beamlets_,
                                         const uint_fast16_t NUM_Blocks_,
                                         const LOFAR_raw_data_type_enum DATA_TYPE_,
                                         const int32_t id_,
                                         const uint_fast32_t NUM_OUTPUT_CHANNELS_,
                                         const uint_fast16_t NUM_TAPS_,
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
                                         const Real64_t multiplier_
                                         )
        throw();
    
    virtual ~LOFAR_Station_Beamformed_Writer_PFB0();


protected:
    FILE* restrict fp_data;
    
    uint_fast64_t num_raw_packet_commands_processed;
    uint_fast64_t num_spectrum_points_processed;

    uint_fast32_t initialized;

    int_fast32_t status;
    //
    void* pfb_storage;
    void** pfb_storage_tap;
    void** pfb_window_tap;
    void* pfb_valid_tap;
    //
    void* window_storage;
    void* fft_in_storage;
    void* fft_out_storage;
    void* output_storage;
    //
    fftwf_plan plan_32;
    fftw_plan plan_64;
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
    fftwl_plan plan_80;
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    fftwq_plan plan_128;
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    //
    LOFAR_Station_Beamformed_Valid_Real32_Writer* restrict valid_object;
    const Real64_t WINDOW_PARAMETER;
    const Real64_t SCALING_MULTIPLIER;
    uint_fast64_t FFT_BUFFER_LENGTH_CHAR;
    uint_fast64_t OUTPUT_BUFFER_LENGTH_CHAR;
    const uint_fast64_t NUM_SAMPLES_IN_FULL_PFB; // NUM_OUTPUT_CHANNELS*NUM_TAPS
    uint_fast32_t samples_in_this_FFT;
    uint_fast32_t valid_samples_in_this_FFT;
    uint_fast64_t valid_samples_in_this_PFB;
    uint_fast32_t NUM_REALS_IN_FFT_BLOCK;
    const uint_fast64_t NUM_REALS_IN_FULL_STORAGE_BUFFER;

    const uint_fast16_t NUM_TAPS;
    const uint_fast16_t NUM_TAPS_m1;  // NUM_TAPS-1
    const uint_fast16_t NUM_TAPS_d2;  // NUM_TAPS/2

    const uint_fast16_t NUM_PARALLEL_BEAMLETS; // NUM_Output_Beamlets * NUM_RSP_BEAMLET_POLARIZATIONS


    LOFAR_raw_data_type_enum processing_DATA_TYPE_REAL;
    LOFAR_raw_data_type_enum processing_DATA_TYPE_COMPLEX;
    LOFAR_raw_data_type_enum output_DATA_TYPE_REAL;
    const MPIfR::Numerical::MPIfR_NUMERICAL_WINDOWING_FUNCTION_TYPE_ENUM window_TYPE;

    virtual int_fast32_t do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) throw();




    template<typename Tproc> int_fast32_t PFB_block() restrict throw()
    {
        // static int max_loop = 0;
        // static const int MAX_LOOP = 33;
        // static const uint_fast16_t beamlet_compare=6;
        // /*JMA*/if(id==0)DEF_LOGGING_DEBUG("show the original values before shifting in the PFB taps");
        // for(uint_fast16_t pb=0, pt=NUM_TAPS_m1; pb < NUM_TAPS_d2; pb++,pt--) {
        //     {
        //         const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[pb]);
        //         Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[pb]);
        //         for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
        //             for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
        //                 if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(pb), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
        //                 data_in++;
        //             }
        //         }
        //     }
        //     {
        //         const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[pt]);
        //         Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[pt]);
        //         for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
        //             for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
        //                 if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(pt), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
        //                 data_in++;
        //             }
        //         }
        //     }
        // }
        // if((NUM_TAPS&0x1)) {
        //     // if NUM_TAPS is odd, do the center tap
        //     const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[NUM_TAPS_d2]);
        //     Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[NUM_TAPS_d2]);
        //     for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
        //         for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
        //             if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(NUM_TAPS_d2), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
        //             data_in++;
        //         }
        //     }
        // }
        // /*JMA*/if(id==0)DEF_LOGGING_DEBUG("now clear the FFT_IN area and shift the PFB taps");

        // zero the FFT in area --- this works for all IEEE floating
        // point formats
        memset(fft_in_storage,0,FFT_BUFFER_LENGTH_CHAR);
        // Rotate the pointers to the tap data areas to prepare for the 
        // upcoming weighting and FFT step.  The raw filling into the blocks
        // has put the latest data into block 0, which gets moved to the end.
        {
            void* temp = pfb_storage_tap[0];
            for(uint_fast16_t p=0; p < NUM_TAPS_m1; p++) {
                pfb_storage_tap[p] = pfb_storage_tap[p+1];
            }
            pfb_storage_tap[NUM_TAPS_m1] = temp;
        }
        // Sum up the number of valid points, and at the same time
        // shift the values down in the array to prepare for the next PFB.
        // Note that the last PFB block comes in as block 0
        // at the beginning of this function.
        {
            uint_fast64_t* const restrict vp(reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap));
            const uint_fast64_t temp = vp[0];
            valid_samples_in_this_PFB = 0;
            for(uint_fast16_t p=0; p < NUM_TAPS_m1; p++) {
                valid_samples_in_this_PFB += vp[p];
                vp[p] = vp[p+1];
            }
            valid_samples_in_this_PFB += vp[NUM_TAPS_m1];
            vp[NUM_TAPS_m1] = temp;
            vp[0] = 0;
        }
        // multiply each PFB block by its corresponding window function
        // value.  This is done from the outside working in, in order
        // to minimize roundoff errors, as the outer samples tend to be
        // weighted far less than the inner samples.

        for(uint_fast16_t pb=0, pt=NUM_TAPS_m1; pb < NUM_TAPS_d2; pb++,pt--) {
            {
                const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[pb]);
                Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[pb]);
                Tproc* restrict fft_in = reinterpret_cast<Tproc* restrict>(fft_in_storage);
                for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
                    for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
                        //if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(pb), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
                        *fft_in++ += *data_in++ * window[i];
                    }
                }
            }
            {
                const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[pt]);
                Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[pt]);
                Tproc* restrict fft_in = reinterpret_cast<Tproc* restrict>(fft_in_storage);
                for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
                    for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
                        //if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(pt), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
                        *fft_in++ += *data_in++ * window[i];
                    }
                }
            }
        }
        if((NUM_TAPS&0x1)) {
            // if NUM_TAPS is odd, do the center tap
            const Tproc* const restrict window = reinterpret_cast<const Tproc* const restrict>(pfb_window_tap[NUM_TAPS_d2]);
            Tproc* restrict data_in = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[NUM_TAPS_d2]);
            Tproc* restrict fft_in = reinterpret_cast<Tproc* restrict>(fft_in_storage);
            for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
                for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
                    //if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA tap=%02d b=%03d i/2=%02d i0=%d %12.4E %12.4E", int(NUM_TAPS_d2), int(b), int(i/2), int(i&0x1), double(*data_in), double(window[i]));}
                    *fft_in++ += *data_in++ * window[i];
                }
            }
        }
        // {
        //     // debug print values
        //     Tproc* restrict fft_in = reinterpret_cast<Tproc* restrict>(fft_in_storage);
        //     for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
        //         for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
        //             if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA fft_in b=%03d i/2=%02d i0=%d %12.4E", int(b), int(i/2), int(i&0x1), double(*fft_in));}
        //             fft_in++;
        //         }
        //     }
        // }

        switch(processing_DATA_TYPE_REAL) {
        case L_Real16_t:
            DEF_LOGGING_CRITICAL("Real16_t processing for FFTs is not yet supported.  Contact your developer.\n");
            exit(1);
            break;
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
        // {
        //     // debug print values
        //     Tproc* restrict fft_out = reinterpret_cast<Tproc* restrict>(fft_out_storage);
        //     for(uint_fast16_t b=0; b < NUM_PARALLEL_BEAMLETS; b++) {
        //         for(uint_fast32_t i=0; i < NUM_REALS_IN_FFT_BLOCK; i++) {
        //             if((id==0)&&(b==beamlet_compare)){/*JMA*/ DEF_LOGGING_DEBUG("JMA fft_out b=%03d i/2=%02d i0=%d %12.4E", int(b), int(i/2), int(i&0x1), double(*fft_out));}
        //             fft_out++;
        //         }
        //     }
        // }
        // if(id==0){max_loop++;DEF_LOGGING_DEBUG("max_loop is %d", max_loop);}/*JMA*/
        // if((id==0)&&(max_loop>=MAX_LOOP)){exit(1);}/*JMA*/

        return dump_PFB_to_output();
    }



    
    template<class Tin, class Tproc> int_fast32_t write_complex_to_pfb_in() restrict throw()
    {
        // Write to the 0th block.  This will be swapped around to the end
        // of the PFB blocks before going to the FFT stage.
        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[packet]->data_start_const());
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
                samples_in_this_FFT += samples_to_copy;
                sample_offset += samples_to_copy;
                if(current_valid[packet]) {
                    valid_samples_in_this_FFT += samples_to_copy;
                }
                if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                    reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
                    int_fast32_t retcode = PFB_block<Tproc>();
                    if(retcode == 0) {
                        samples_in_this_FFT = 0;
                        valid_samples_in_this_FFT = 0;
                        // running the PFB_block cycles around the storage taps.
                        // reset the pointer to the start of the data
                        data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
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



    template<class Tin, class Tproc> int_fast32_t write_real_to_pfb_in() restrict throw()
    {
        // Write to the 0th block.  This will be swapped around to the end
        // of the PFB blocks before going to the FFT stage.
        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
        for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
            const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[packet]->data_start_const());
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
                samples_in_this_FFT += samples_to_copy;
                sample_offset += samples_to_copy;
                if(current_valid[packet]) {
                    valid_samples_in_this_FFT += samples_to_copy;
                }
                if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                    reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
                    int_fast32_t retcode = PFB_block<Tproc>();
                    if(retcode == 0) {
                        samples_in_this_FFT = 0;
                        valid_samples_in_this_FFT = 0;
                        // running the PFB_block cycles around the storage taps.
                        // reset the pointer to the start of the data
                        data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
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



    template<class Tin, class Tproc> int_fast32_t write_complex_to_pfb_in_partial(const uint_fast32_t NUM_ACTUAL_BLOCKS) restrict throw()
    {
        // Write to the 0th block.  This will be swapped around to the end
        // of the PFB blocks before going to the FFT stage.
        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
        const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[0]->data_start_const());
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
                data_out_start += size_t(NUM_REALS_IN_FFT_BLOCK)<<1;
            }
            samples_in_this_FFT += samples_to_copy;
            sample_offset += samples_to_copy;
            if(current_valid[0]) {
                valid_samples_in_this_FFT += samples_to_copy;
            }
            if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
                int_fast32_t retcode = PFB_block<Tproc>();
                if(retcode == 0) {
                    samples_in_this_FFT = 0;
                    valid_samples_in_this_FFT = 0;
                    // running the PFB_block cycles around the storage taps.
                    // reset the pointer to the start of the data
                    data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
                }
                else {
                    DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                    return retcode;
                }
            }
        }
        return 0;
    }



    template<class Tin, class Tproc> int_fast32_t write_real_to_pfb_in_partial(const uint_fast32_t NUM_ACTUAL_BLOCKS) restrict throw()
    {
        // Write to the 0th block.  This will be swapped around to the end
        // of the PFB blocks before going to the FFT stage.
        Tproc* restrict data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
        const Tin* const restrict data_in = reinterpret_cast<const Tin* const restrict>(current_data_packets[0]->data_start_const());
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
            samples_in_this_FFT += samples_to_copy;
            sample_offset += samples_to_copy;
            if(current_valid[0]) {
                valid_samples_in_this_FFT += samples_to_copy;
            }
            if(samples_in_this_FFT == NUM_OUTPUT_CHANNELS) {
                reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
                int_fast32_t retcode = PFB_block<Tproc>();
                if(retcode == 0) {
                    samples_in_this_FFT = 0;
                    valid_samples_in_this_FFT = 0;
                    // running the PFB_block cycles around the storage taps.
                    // reset the pointer to the start of the data
                    data_out = reinterpret_cast<Tproc* restrict>(pfb_storage_tap[0]);
                }
                else {
                    DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                    return retcode;
                }
            }
        }
        return 0;
    }


    
    template<class Tproc> int_fast32_t write_real_to_pfb_in_flush(uint_fast32_t* const restrict num_flushed) restrict throw()
    {
        // End of data, flush PFB buffer out
        *num_flushed = 0;
        if(samples_in_this_FFT == 0) {
            // FFT buffer empty already
            // Do nothing
        }
        else {
            // Write to the 0th block.  This will be swapped around to the end
            // of the PFB blocks before going to the FFT stage.
            Tproc* const restrict data_out = reinterpret_cast<Tproc* const restrict>(pfb_storage_tap[0]);
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
            samples_in_this_FFT += samples_to_copy;
            reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
            int_fast32_t retcode = PFB_block<Tproc>();
            if(retcode == 0) {
                samples_in_this_FFT = 0;
                valid_samples_in_this_FFT = 0;
            }
            else {
                DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                return retcode;
            }
            *num_flushed = samples_to_copy;
        }
        // Now run through the remaining PFB blocks, zeroing all input, and
        // running the PFB
        for(uint_fast16_t p=0; p < NUM_TAPS_m1; p++) {
            // Write to the 0th block.  This will be swapped around to the end
            // of the PFB blocks before going to the FFT stage.
            // Zero the PFB block area --- this works for all IEEE floating
            // point formats
            memset(pfb_storage_tap[0],0,FFT_BUFFER_LENGTH_CHAR);
            samples_in_this_FFT = NUM_OUTPUT_CHANNELS;
            valid_samples_in_this_FFT = 0;
            reinterpret_cast<uint_fast64_t* const restrict>(pfb_valid_tap)[0] = valid_samples_in_this_FFT;
            int_fast32_t retcode = PFB_block<Tproc>();
            if(retcode == 0) {
                samples_in_this_FFT = 0;
                valid_samples_in_this_FFT = 0;
            }
            else {
                DEF_LOGGING_ERROR("WRITER %d failure while processing packet %u of %u\n", int(id), (unsigned int)(0), (unsigned int)(current_num_packets));
                return retcode;
            }
            *num_flushed += samples_in_this_FFT;
        }
    
        return 0;
    }







    
private:

    static uint16_t VERSION() throw() {return uint16_t(MPIfR_LOFAR_STATION_SOFTWARE_VERSION);}
    static uint16_t WRITER_TYPE() throw() {return uint16_t(LOFAR_DOFF_PFB0_OUT);}




    int_fast32_t check_input_parameters() restrict throw();
    int_fast32_t set_up_work_buffer_areas() restrict throw();
    int_fast32_t set_up_PFB_window() restrict throw();
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


    int_fast32_t write_PFB_flush() throw();
    int_fast32_t dump_PFB_to_output() throw();

    // prevent copying
    LOFAR_Station_Beamformed_Writer_PFB0(const LOFAR_Station_Beamformed_Writer_PFB0& a);
    LOFAR_Station_Beamformed_Writer_PFB0& operator=(const LOFAR_Station_Beamformed_Writer_PFB0& a);

    
};


// CLASS FUNCTIONS



// HELPER FUNCTIONS




}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR

#endif // LOFAR_Station_Beamformed_Writer_PFB0_H
