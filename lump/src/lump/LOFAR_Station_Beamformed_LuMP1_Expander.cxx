// LOFAR_Station_Beamformed_LuMP1_Expander.cxx
// Code to dump out LuMP1 formatted data to header file and data pipe
//_HIST  DATE NAME PLACE INFO
// 2013 Feb 24  James M Anderson  --- MPIfR  start from test_ascii_dump_raw0.cxx
// 2013 Aug 12  JMA  ---- file list file name change to _lis
// 2014 Sep 17  JMA  --- fix debug string formatting errors



// Copyright (c) 2011, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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




// see LOFAR_Station_Beamformed_Writer_LuMP1.cxx for format details



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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LOFAR_Station_Common.h"
#include "LOFAR_Station_Complex.h"
#include "LOFAR_Station_Beamformed_Info.h"
#include "MPIfR_logging.h"
#include "LOFAR_Station_Beamformed_LuMP1_Expander.h"
#include "LOFAR_Station_Beamformed_Writer_LuMP1.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>




namespace MPIfR {
namespace LOGGING {
extern const char PROGRAM_NAME[] = "LOFAR_Station_Beamformed_LuMP1_Expander";
extern const char PROGRAM_VERSION[] = MPIfR_LOFAR_LuMP_VERSION_DATE_STRING;
extern const char PROGRAM_DATE[] = __DATE__;
extern const char PROGRAM_TIME[] = __TIME__;
};};




// set up a namespace area for stuff.
namespace {




// GLOBALS
const uint16_t LuMP_HEADER_SIZE = 4096;



// FUNCTIONS



}  // end namespace






int_fast32_t check_header(const int fd,
                          size_t* const ACTUAL_LuMP_HEADER_SIZE)
{
    static const char match[] = "\nREAD_DATA_FROM_PIPE ";
    static const char hdr_size_match[] = "\nHDR_SIZE ";
    // we are guaranteed elsewhere in the program that there is enough
    // data in the file for the header
    *ACTUAL_LuMP_HEADER_SIZE = 0;
    char header[LuMP_HEADER_SIZE+1];
    size_t total_read = 0;
    ssize_t retval;
    while(total_read < LuMP_HEADER_SIZE) {
        errno = 0;
        retval = read(fd, reinterpret_cast<void*>(header+total_read), size_t(LuMP_HEADER_SIZE-total_read));
        if(retval == -1) {
            DEF_LOGGING_PERROR("read on raw datafile failed");
            if((errno == EAGAIN) || (errno = EWOULDBLOCK) || (errno == EINTR)) {
                // try again
                retval = 0;
            }
            else {
                return -1;
            }
        }
        else if(retval == 0) {
            // end of file
            DEF_LOGGING_ERROR("end of file --- only (%zu) bytes read for header --- expected %" PRIu16,total_read,LuMP_HEADER_SIZE);
            return -2;
        }
        total_read += retval;
        if(total_read != LuMP_HEADER_SIZE) {
            DEF_LOGGING_WARNING("wrong number of bytes (%zu) bytes read for header --- expected %" PRIu16 " --- will try again",total_read,LuMP_HEADER_SIZE);
        }
    }
    // guarantee an end to the string
    header[LuMP_HEADER_SIZE] = 0;
    // search for the actual header size
    char* p = strstr(header, hdr_size_match);
    if(p == NULL) {
        // No match, which means that this is not a valid LuMP file
        DEF_LOGGING_ERROR("string '%s' not found in header --- this is not a proper LuMP datafile to use with this program", hdr_size_match);
        return -3;
    }
    p += strlen(hdr_size_match);
    errno = 0;
    char* endptr = NULL;
    *ACTUAL_LuMP_HEADER_SIZE = strtoull(p, &endptr, 10);
    if((endptr == p) || (errno != 0)) {
        DEF_LOGGING_ERROR("unable to read size of LuMP header");
        return -4;
    }
    if(*ACTUAL_LuMP_HEADER_SIZE < LuMP_HEADER_SIZE) {
        DEF_LOGGING_ERROR("LuMP HDR_SIZE %zu is invalid", *ACTUAL_LuMP_HEADER_SIZE);
        return -5;
    }
    // Now look for the PIPE keyword
    p = strstr(header, match);
    if(p == NULL) {
        // No match, which means that this dataset does not match
        DEF_LOGGING_ERROR("string '%s' not found in header --- this is not a proper LuMP datafile to use with this program", match);
        return -6;
    }
    p += strlen(match);
    while(*p == ' ') {
        ++p;
    }
    if((strncmp(p,"TRUE",4)==0) || (strncmp(p,"true",4)==0) || (strncmp(p,"TRUE",1)==0)) {
        // We need to process the data
        return +1;
    }
    // no need to modify the dataset
    return 0;
}

int_fast32_t skip_to_start_of_data(const int fd,
                                   const size_t ACTUAL_LuMP_HEADER_SIZE)
{
    // Read forward in the data to the location of the start of data.
    // This is done as an actual read and not an lseek in the case that
    // at some point in the future the fd is a pipe or socket.
    // Note that LuMP_HEADER_SIZE has already been read from the fd.
    char buf[LuMP_HEADER_SIZE];
    size_t total_read = LuMP_HEADER_SIZE;
    ssize_t retval;
    while(size_t(total_read) < ACTUAL_LuMP_HEADER_SIZE) {
        size_t this_read = 0;
        size_t this_need = ACTUAL_LuMP_HEADER_SIZE - total_read;
        if(this_need > LuMP_HEADER_SIZE) {
            this_need = LuMP_HEADER_SIZE;
        }
        while(this_read < this_need) {
            errno = 0;
            retval = read(fd, reinterpret_cast<void*>(buf+this_read),
                          this_need-this_read);
            if(retval == -1) {
                DEF_LOGGING_PERROR("read on raw datafile failed");
                if((errno == EAGAIN) || (errno = EWOULDBLOCK) || (errno == EINTR)) {
                    // try again
                    retval = 0;
                }
                else {
                    return -1;
                }
            }
            else if(retval == 0) {
                // end of file
                total_read += this_read;
                DEF_LOGGING_ERROR("end of file --- only (%zu) bytes read for header --- expected %" PRIu16,total_read,LuMP_HEADER_SIZE);
                return -2;
            }
            this_read += retval;
            if(this_read != this_need) {
                DEF_LOGGING_WARNING("wrong number of bytes (%zu) bytes read for header --- expected %zu --- will try again",this_read,this_need);
            }
        }
        total_read += this_read;
    }
    return 0;
}

    
    

// Just use standard reads with posix_fadvise instead of memory mapping
// and posix_madvise.  WIth a single read-through of the dataset,
// the system call overhead to mmap and munmap will probably eat
// up too much time, and disk I/O dominates anyway...


// int fstat(int fd, struct stat *buf);

// int posix_fadvise(int fd, off_t offset, off_t len, int advice);
// int posix_madvise(void *addr, size_t len, int advice);

// int madvise(void *addr, size_t length, int advice);




    


int_fast32_t
open_LuMP1_file(const char* const filename_base,
                const MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info& info,
                const uint_fast32_t INPUT_LINE_SIZE_BYTES,
                const uint16_t LuMP_HEADER_SIZE,
                int* file_descriptor_p,
                int_fast64_t* NUM_ACTUAL_SAMPLES_P,
                off_t* TOTAL_FILE_SIZE_P
                )
{
    const size_t FSIZE=2048;
    char new_filename[FSIZE];
    char data_filename[FSIZE];
    char data_path_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_ERROR("Error creating file file filename from base '%s'\n", filename_base);
        exit(2);
    }
    FILE* fp_name = fopen(new_filename,"r");
    if(fp_name == NULL) {
        DEF_LOGGING_ERROR("unable to open file file '%s'",new_filename);
        return -1;
    }
    if(fgets(data_filename, FSIZE, fp_name) == NULL) {
        DEF_LOGGING_ERROR("unable to read from file file '%s'",new_filename);
        return -2;
    }
    fclose(fp_name);
    {
        size_t l = strlen(data_filename);
        if(l) {
            l--;
            if(data_filename[l] == '\n') {
                data_filename[l] = 0;
            }
        }
    }
    {
        char* cp = new_filename + strlen(new_filename);
        bool path_flag = false;
        // cp must be greater than new_filename by snprintf above
        do {
            if(*cp == '/') {
                *cp = 0;
                path_flag = true;
                break;
            }
            cp--;
        }
        while(cp > new_filename);
        if(!path_flag) {
            retval = snprintf(data_path_filename, FSIZE, "%s",data_filename);
        }
        else {
            retval = snprintf(data_path_filename, FSIZE, "%s/%s", new_filename,data_filename);
        }
        if((retval < 0) || (size_t(retval) >= FSIZE)) {
            DEF_LOGGING_ERROR("Error creating full path data filename from base '%s'\n", filename_base);
            exit(2);
        }
    }
    // FILE* fp_data = fopen(data_path_filename,"rb");
    // if(fp_data == NULL) {
    //     DEF_LOGGING_ERROR("unable to open raw output data file '%s' in %s:%d\n",data_path_filename);
    //     return -3;
    // }
    // fclose(fp);
    const uint64_t num_samples_info = info.get_num_output_timeslots();


    // tell the system to not update the access time, as this slows things
    // down substantially.
    errno = 0;
    int fd = open (data_path_filename, O_RDONLY|O_NOATIME);
    if (fd == -1) {
        DEF_LOGGING_INFO("unable to open file '%s' for reading with O_NOATIME", data_path_filename);
        // try opening without the O_NOATIME specification
        fd = open (data_path_filename, O_RDONLY);
    }    
    if (fd == -1) {
        DEF_LOGGING_PERROR("open on raw datafile failed");
        DEF_LOGGING_ERROR("unable to open file '%s' for reading", data_path_filename);
        return -4;
    }

    struct stat sb;
    errno = 0;
    if (fstat (fd, &sb) == -1) {
        DEF_LOGGING_PERROR("fstat failure");
        DEF_LOGGING_ERROR("unable to fstat file '%s'", data_path_filename);
        return -5;
    }

    if (!S_ISREG (sb.st_mode)) {
        DEF_LOGGING_ERROR("file '%s' is not a regular file", data_path_filename);
        exit(2);
    }

    const off_t TOTAL_FILE_SIZE = sb.st_size;
    if(TOTAL_FILE_SIZE < LuMP_HEADER_SIZE) {
        DEF_LOGGING_ERROR("file '%s' size %llu bytes is too small", data_path_filename, (unsigned long long)(TOTAL_FILE_SIZE));
        return -6;
    }

    int_fast64_t NUM_ACTUAL_SAMPLES = (TOTAL_FILE_SIZE - LuMP_HEADER_SIZE)
                                      / INPUT_LINE_SIZE_BYTES;
    DEF_LOGGING_INFO("for file '%s' found %lld samples, expected %lld", data_path_filename, (long long)(NUM_ACTUAL_SAMPLES), (long long)(num_samples_info));


    // tell the operating system to optimize for a sequential read
    {
        int retval = posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
        if((retval)) {
            DEF_LOGGING_ERROR("file '%s' gave error code %d on posix_fadvise call", data_path_filename, retval);
            return -7;
        }
    }

    *NUM_ACTUAL_SAMPLES_P = NUM_ACTUAL_SAMPLES;
    *file_descriptor_p = fd;
    *TOTAL_FILE_SIZE_P = TOTAL_FILE_SIZE;
    return 0;
}



int main(const int argc, const char* const argv[])
{
    fprintf(stderr, "%s starting\n", MPIfR::LOGGING::PROGRAM_NAME);
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    fprintf(stderr, "Warning: only tested on little-endian systems\n");
#endif

    
    MPIfR::LOGGING::LOGGING_BLANK_.reset_display(stderr,MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_DEBUG);
    DEF_LOGGING_INFO("%s [version %s, build date %sT%s] starting\n", MPIfR::LOGGING::PROGRAM_NAME, MPIfR::LOGGING::PROGRAM_VERSION, MPIfR::LOGGING::PROGRAM_DATE, MPIfR::LOGGING::PROGRAM_TIME);

    if((argc < 2) || (argc > 3)) {
        DEF_LOGGING_CRITICAL("correct usage is %s filename_base [force]", argv[0]);
        exit(2);
    }

    const char* const filename_base = argv[1];
    MPIfR::LOFAR::Station::LOFAR_Station_Beamformed_Info info(filename_base, "rb");
    const bool FORCE_PROCESS((argc == 3)? true:false);





    if(info.get_WRITER_TYPE() != uint16_t(MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP1_OUT)) {
        DEF_LOGGING_ERROR("wrong data format.  Should be %d '%s' but instead got %d '%s'\n",
                          int(MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP1_OUT),
                          MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(MPIfR::LOFAR::Station::LOFAR_DOFF_LuMP1_OUT),
                          int(info.get_WRITER_TYPE()),
                          MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT_Str(MPIfR::LOFAR::Station::LOFAR_DATA_OUTPUT_FILE_FORMAT(info.get_WRITER_TYPE()))
                          );
        exit(2);
    }
    if(info.get_NUM_CHANNELS() != 1) {
        DEF_LOGGING_ERROR("wrong number channels per beamlet '%u' --- allowed value is 1\n",
                          unsigned(info.get_NUM_CHANNELS()));
        exit(2);
    }

    const uint_fast16_t Voltage_Size(MPIfR::LOFAR::LOFAR_raw_data_type_enum_size(MPIfR::LOFAR::LOFAR_raw_data_type_enum(info.get_output_DATA_TYPE())));

    const uint_fast32_t NUM_BEAMLETS = info.get_beamlets_per_sample();
    if(NUM_BEAMLETS == 0) {
        DEF_LOGGING_ERROR("No beamlets written to this dataset\n");
        exit(2);
    }
    const uint_fast32_t NUM_DUAL_POL_CHANNELS = NUM_BEAMLETS*MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;
    const uint_fast16_t CLOCK_SPEED_IN_MHz = info.get_station_clock_frequency();
    
    uint_fast32_t* restrict beamlets = info.read_beamlets_data(filename_base);
    if(beamlets == NULL) {
        DEF_LOGGING_ERROR("reading beamlets data failed\n");
        exit(2);
    }
    uint_fast32_t* restrict subbands = info.read_subbands_data(filename_base);
    if(subbands == NULL) {
        DEF_LOGGING_ERROR("reading subbands data failed\n");
        exit(2);
    }
    uint_fast32_t* restrict rcumode = info.read_rcumode_data(filename_base);
    if(rcumode == NULL) {
        DEF_LOGGING_ERROR("reading rcumode data failed\n");
        exit(2);
    }


    // Perform a sanity check on the RCUMODEs
    for(uint_fast32_t b=0; b < NUM_BEAMLETS; b++) {
        if(MPIfR::LOFAR::Station::LOFAR_RCUMODE_AND_CLOCK_MATCH(rcumode[b], CLOCK_SPEED_IN_MHz)) {
        }
        else {
            DEF_LOGGING_ERROR("RCUMODE %d for beamlet %d does not math clock speed of %d MHz\n", int(rcumode[b]), int(b), int(CLOCK_SPEED_IN_MHz));
            exit(2);
        }
    }


    const size_t INPUT_LINE_SIZE_BYTES = Voltage_Size*size_t(NUM_DUAL_POL_CHANNELS);
    int file_descriptor;
    int_fast64_t NUM_ACTUAL_LINES;
    off_t TOTAL_FILE_SIZE;
    {
        int_fast32_t retval = open_LuMP1_file(filename_base,
                                              info,
                                              INPUT_LINE_SIZE_BYTES,
                                              LuMP_HEADER_SIZE,
                                             &file_descriptor,
                                             &NUM_ACTUAL_LINES,
                                             &TOTAL_FILE_SIZE);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not open input file from base filename '%s' (error %d)", filename_base, int(retval));
            exit(2);
        }
    }



    // Check that we actually need to do something
    size_t ACTUAL_LuMP_HEADER_SIZE = 0;
    const int_fast32_t NEED_TO_PROCESS = check_header(file_descriptor,
                                                     &ACTUAL_LuMP_HEADER_SIZE);
    if(NEED_TO_PROCESS < 0) {
        DEF_LOGGING_ERROR("Could not read header from filename '%s' (error %d)", argv[1], int(NEED_TO_PROCESS));
        exit(2);
    }
    else if((NEED_TO_PROCESS == 0) && (!FORCE_PROCESS)) {
        // Nothing to do
        DEF_LOGGING_INFO("No need to modify raw data");
    }
    else {
        if(FORCE_PROCESS) {
            DEF_LOGGING_INFO("force option to require piping data enabled");
        }
        DEF_LOGGING_INFO("Will modify datastream to correct for missing subbands");

        // skip forward to start of data
        skip_to_start_of_data(file_descriptor, ACTUAL_LuMP_HEADER_SIZE);

        // correct number of lines to process, if needed
        if(ACTUAL_LuMP_HEADER_SIZE != LuMP_HEADER_SIZE) {
            NUM_ACTUAL_LINES = (TOTAL_FILE_SIZE - ACTUAL_LuMP_HEADER_SIZE)
                               / INPUT_LINE_SIZE_BYTES;
        }

        // Make sure stdout is fully buffered
        setvbuf(stdout, NULL, _IOFBF, 0);


        uint_fast32_t NUM_LuMP1_CHANNELS = 0;
        uint_fast32_t NUM_LuMP1_DUAL_POL_CHANNELS = 0;
        Real64_t CHANNEL_CENTER_MIN = -1.0;
        const Real64_t CHANNEL_BANDWIDTH = MPIfR::LOFAR::Station::NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz);
        {
            CHANNEL_CENTER_MIN = 
                MPIfR::LOFAR::Station::LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                                                      MPIfR::LOFAR::Station::LOFAR_NYQUIST_FREQUENCY_OFFSET(rcumode[0]),
                                                                      subbands[0]);
            Real64_t CHANNEL_CENTER_MAX = CHANNEL_CENTER_MIN;
            Real64_t LuMP1_BANDWIDTH_IN_HZ = -1.0;
            for(uint_fast32_t beamlet=0; beamlet < NUM_BEAMLETS; beamlet++) {
                Real64_t this_freq =
                    MPIfR::LOFAR::Station::LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                                                          MPIfR::LOFAR::Station::LOFAR_NYQUIST_FREQUENCY_OFFSET(rcumode[beamlet]),
                                                                          subbands[beamlet]);
                if(this_freq < CHANNEL_CENTER_MIN) {
                    CHANNEL_CENTER_MIN = this_freq;
                }
                else if(this_freq > CHANNEL_CENTER_MAX) {
                    CHANNEL_CENTER_MAX = this_freq;
                }
            }
            LuMP1_BANDWIDTH_IN_HZ = (CHANNEL_CENTER_MAX-CHANNEL_CENTER_MIN)+CHANNEL_BANDWIDTH;
            NUM_LuMP1_CHANNELS = uint_fast32_t(floor(LuMP1_BANDWIDTH_IN_HZ/CHANNEL_BANDWIDTH + 0.5));
            NUM_LuMP1_DUAL_POL_CHANNELS = NUM_LuMP1_CHANNELS*MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;
        }
        DEF_LOGGING_INFO("original dataset has %d beamlets, outputing %d beamlets",
                         int(NUM_BEAMLETS), int(NUM_LuMP1_CHANNELS));

        void* beamlet_index_void;
        uint_fast32_t* restrict beamlet_index;
        int retval = posix_memalign(&beamlet_index_void, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, sizeof(int_fast32_t)*NUM_BEAMLETS);
        if((retval)) {
            DEF_LOGGING_ERROR("Could not allocate %llu bytes for beamlet_offset with retval %d\n", (unsigned long long)(sizeof(int_fast32_t)*NUM_BEAMLETS), retval);
            return -1;
        }
        beamlet_index = reinterpret_cast<uint_fast32_t* restrict>(beamlet_index_void);

        // Now set up the beamlet offsets
        for(uint_fast32_t beamlet = 0; beamlet < NUM_BEAMLETS; beamlet++) {
            // Do not allow duplicate subbands
            for(uint_fast32_t beamlet_check=0; beamlet_check < beamlet; beamlet_check++) {
                if((rcumode[beamlet_check] == rcumode[beamlet])
                  && (subbands[beamlet_check] == subbands[beamlet])) {
                    DEF_LOGGING_ERROR("beamlet indices %u and %u have the same RCUMODE (%d) and subband (%u) --- this is not allowed by the LuMP2 format", (unsigned int)(beamlet_check), (unsigned int)(beamlet), int(rcumode[beamlet_check]), (unsigned int)(subbands[beamlet_check]));
                    return -1;
                }
            }
            Real64_t this_freq =
                MPIfR::LOFAR::Station::LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                                                      MPIfR::LOFAR::Station::LOFAR_NYQUIST_FREQUENCY_OFFSET(rcumode[beamlet]),
                                                                      subbands[beamlet]);
            uint_fast32_t pos = floor((this_freq - CHANNEL_CENTER_MIN)/CHANNEL_BANDWIDTH + 0.5);
            beamlet_index[beamlet] = pos;
        }
        // Now make adjustments for writing out polarization values
        const uint_fast16_t Copy_Voltage_Size =
            Voltage_Size * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;
        // Optimization for copying with limited-width integers
        {
            uint_fast32_t multiplier = 1;
            switch(Copy_Voltage_Size) {
            case 1:
            case 2:
            case 4:
            case 8:
                multiplier = 1;
                break;
            case 16:
                multiplier = 2;
                break;
            case 32:
                multiplier = 4;
                break;
            case 64:
                multiplier = 8;
                break;
            default:
                DEF_LOGGING_CRITICAL("unsupported voltage size value %d bytes (%d bytes with polarization)", int(Voltage_Size), int(Copy_Voltage_Size));
                exit(1);
            }
            if(multiplier != 1) {
                // scale up the beamlet_index values
                for(uint_fast32_t beamlet = 0;
                    beamlet < NUM_BEAMLETS;
                    beamlet++) {
                    beamlet_index[beamlet] *= multiplier;
                }
            }
        }
                

        // Figure out how large to make the read in buffer.  To optimize
        // file descriptor reading, make sure the in buffer is a multiple of
        // the disk block size (assume 512 bytes).  Also make sure that the
        // read is at least 65536 bytes to minimize function call overhead
        // in read.  For 61 beamlets with 16 bit LOFAR data, this results in
        // reading 256 lines, for a total data size of 124928 bytes.
        // Note that 512 == 0x200.
        int_fast32_t NUM_LINES_TO_READ_AT_ONCE = 1;
        size_t INPUT_BLOCK_SIZE_BYTES = NUM_LINES_TO_READ_AT_ONCE * INPUT_LINE_SIZE_BYTES;
        while(((NUM_LINES_TO_READ_AT_ONCE & 0x1FF)) || (INPUT_BLOCK_SIZE_BYTES < 65536)) {
            NUM_LINES_TO_READ_AT_ONCE *= 2;
            INPUT_BLOCK_SIZE_BYTES = NUM_LINES_TO_READ_AT_ONCE * INPUT_LINE_SIZE_BYTES;
        }
        {
            DEF_LOGGING_DEBUG("will read in %d lines at once, for %llu bytes (each line is %llu bytes)", int(NUM_LINES_TO_READ_AT_ONCE), (unsigned long long)(INPUT_BLOCK_SIZE_BYTES), (unsigned long long)(INPUT_LINE_SIZE_BYTES));
        }
        const size_t OUTPUT_LINE_SIZE_BYTES = size_t(NUM_LuMP1_DUAL_POL_CHANNELS)*Voltage_Size;
        const size_t OUTPUT_BLOCK_SIZE_BYTES = NUM_LINES_TO_READ_AT_ONCE * OUTPUT_LINE_SIZE_BYTES;



        // Allocate memory for the in and out buffers
        void* data_in_buffer;
        void* data_out_buffer;
        posix_memalign(&data_in_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT,
                       INPUT_BLOCK_SIZE_BYTES);
        posix_memalign(&data_out_buffer, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT,
                       OUTPUT_BLOCK_SIZE_BYTES);
        if((data_in_buffer == NULL) || (data_out_buffer == NULL)) {
            DEF_LOGGING_ERROR("Could not allocate %lld bytes for in buffer and %lld bytes for out buffer", (long long)(INPUT_BLOCK_SIZE_BYTES), (long long)(OUTPUT_BLOCK_SIZE_BYTES));
            exit(1);
        }
        // Clear the output buffer area.  Note that for all forms of LOFAR
        // data representations, all bits 0 indicates a value of 0.  This
        // makes sure that any subband frequency areas that are missing
        // are set to zero.
        // TODO:  Figure out what value dspsr uses to indicate data that
        //        need to be flagged.
        memset(data_out_buffer, 0, OUTPUT_BLOCK_SIZE_BYTES);            

        

        // loop over reading the data by blocks
        int_fast32_t this_line_count = 0;
        for(int_fast64_t l=0; l < NUM_ACTUAL_LINES; l += this_line_count) {
            this_line_count = NUM_LINES_TO_READ_AT_ONCE;
            if(l+this_line_count > NUM_ACTUAL_LINES) {
                this_line_count = NUM_ACTUAL_LINES - l;
            }

            // How much data do we read in and write out?
            const size_t THIS_INPUT_BLOCK_SIZE_BYTES =
                INPUT_LINE_SIZE_BYTES * this_line_count;
            const size_t THIS_OUTPUT_BLOCK_SIZE_BYTES =
                OUTPUT_LINE_SIZE_BYTES * this_line_count;

            // read in the data
            {
                size_t total_read = 0;
                ssize_t retval;
                while(total_read < THIS_INPUT_BLOCK_SIZE_BYTES) {
                    errno = 0;
                    retval = read(file_descriptor, data_in_buffer,
                                  size_t(THIS_INPUT_BLOCK_SIZE_BYTES-total_read));
                    if(retval == -1) {
                        DEF_LOGGING_PERROR("read on raw datafile failed");
                        if((errno == EAGAIN) || (errno = EWOULDBLOCK) || (errno == EINTR)) {
                            // try again
                            retval = 0;
                        }
                        else {
                            DEF_LOGGING_ERROR("could not read from input file");
                            exit(2);
                        }
                    }
                    else if(retval == 0) {
                        // end of file
                        DEF_LOGGING_ERROR("end of file --- only (%lld) bytes read from data --- tried to read %lld",(long long)(total_read),(long long)(THIS_INPUT_BLOCK_SIZE_BYTES));
                        exit(2);
                    }
                    total_read += retval;
                    if(total_read != THIS_INPUT_BLOCK_SIZE_BYTES) {
                        DEF_LOGGING_WARNING("wrong number of bytes (%lld) bytes read from data --- tried to read %lld --- will try again",(long long)(total_read),(long long)(THIS_INPUT_BLOCK_SIZE_BYTES));
                    }
                }
            }

            const char* data_in_buffer_char = reinterpret_cast<const char*>(data_in_buffer);
            char* data_out_buffer_char = reinterpret_cast<char*>(data_out_buffer);

            // copy over
            switch(Copy_Voltage_Size) {
            case 1:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint8_t* restrict in = reinterpret_cast<const uint8_t* restrict>(data_in_buffer_char);
                        uint8_t* restrict out = reinterpret_cast<uint8_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]] = *in++;
                        }
                    }
                }
                break;
            case 2:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint16_t* restrict in = reinterpret_cast<const uint16_t* restrict>(data_in_buffer_char);
                        uint16_t* restrict out = reinterpret_cast<uint16_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]] = *in++;
                        }
                    }
                }
                break;
            case 4:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint32_t* restrict in = reinterpret_cast<const uint32_t* restrict>(data_in_buffer_char);
                        uint32_t* restrict out = reinterpret_cast<uint32_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]] = *in++;
                        }
                    }
                }
                break;
            case 8:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint64_t* restrict in = reinterpret_cast<const uint64_t* restrict>(data_in_buffer_char);
                        uint64_t* restrict out = reinterpret_cast<uint64_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]] = *in++;
                        }
                    }
                }
                break;
            case 16:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint64_t* restrict in = reinterpret_cast<const uint64_t* restrict>(data_in_buffer_char);
                        uint64_t* restrict out = reinterpret_cast<uint64_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]   ] = *in++;
                            out[beamlet_index[beamlet] +1] = *in++;
                        }
                    }
                }
                break;
            case 32:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint64_t* restrict in = reinterpret_cast<const uint64_t* restrict>(data_in_buffer_char);
                        uint64_t* restrict out = reinterpret_cast<uint64_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]   ] = *in++;
                            out[beamlet_index[beamlet] +1] = *in++;
                            out[beamlet_index[beamlet] +2] = *in++;
                            out[beamlet_index[beamlet] +3] = *in++;
                        }
                    }
                }
                break;
            case 64:
                {
                    for(int_fast32_t ll=0; ll < this_line_count;
                        ll++,
                  data_in_buffer_char += INPUT_LINE_SIZE_BYTES,
                 data_out_buffer_char += OUTPUT_LINE_SIZE_BYTES) {
                        const uint64_t* restrict in = reinterpret_cast<const uint64_t* restrict>(data_in_buffer_char);
                        uint64_t* restrict out = reinterpret_cast<uint64_t* restrict>(data_out_buffer_char);
                        for(uint_fast32_t beamlet = 0;
                            beamlet < NUM_BEAMLETS;
                            beamlet++) {
                            out[beamlet_index[beamlet]   ] = *in++;
                            out[beamlet_index[beamlet] +1] = *in++;
                            out[beamlet_index[beamlet] +2] = *in++;
                            out[beamlet_index[beamlet] +3] = *in++;
                            out[beamlet_index[beamlet] +4] = *in++;
                            out[beamlet_index[beamlet] +5] = *in++;
                            out[beamlet_index[beamlet] +6] = *in++;
                            out[beamlet_index[beamlet] +7] = *in++;
                        }
                    }
                }
                break;
            default:
                DEF_LOGGING_ERROR("Unsupported data size %d including polarization", int(Copy_Voltage_Size));
                exit(1);
            }

            // Now write this out to stdout
            size_t num = fwrite(data_out_buffer, THIS_OUTPUT_BLOCK_SIZE_BYTES, 1, stdout);
            if(num == 1) {
            }
            else {
                DEF_LOGGING_ERROR("Unable to write %lld bytes to stdout", (long long)(THIS_OUTPUT_BLOCK_SIZE_BYTES));
                exit(2);
            }
        } // for l over NUM_ACTUAL_LINES;

        free(data_in_buffer); data_in_buffer=NULL;
        free(data_out_buffer); data_out_buffer=NULL;
        free(beamlet_index_void); beamlet_index_void=NULL;
    } // needed to write out stuff
    

    {
        errno = 0;
        int retval = close(file_descriptor);
        if((retval)) {
            DEF_LOGGING_PERROR("close on raw datafile failed");
        }
    }

    free(reinterpret_cast<void*>(beamlets)); beamlets = NULL;
    free(reinterpret_cast<void*>(subbands)); subbands = NULL;
    free(reinterpret_cast<void*>(rcumode));  rcumode = NULL;

    return 0;
}


    
    
