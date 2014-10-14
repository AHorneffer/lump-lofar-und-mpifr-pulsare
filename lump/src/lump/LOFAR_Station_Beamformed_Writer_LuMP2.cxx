// LOFAR_Station_Beamformed_Writer_LuMP2.cxx
// writes out multiple subband LuMP pulsar format data
//_HIST  DATE NAME PLACE INFO
// 2013 Aug 18  James M Anderson  --- MPIfR  start from LuMP1 format
// 2013 Sep 05  JMA  ---- development fixes to get working and provide logfile
//                        for child process stdout/stderr
// 2014 Sep 09  JMA  ---- remove duplicate free of output_storage



// Copyright (c) 2011, 2012, 2013, 2014 James M. Anderson <anderson@gfz-potsdam.de>

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



// Raw Data Format
// written in "wb" mode for all files
//
// Data files:  No data file is written out by this writer.  The data are
// sent through a pipe to a child process to be processed.  The data are
// written to the pipe as one (complex) voltage value of type DATA_TYPE, 
// for each polarization, for each beamlet (with blank data filling subband
// gaps) for each time sample.
//
// Data header file:  A LuMP header file with READ_DATA_FROM_PIPE set to true
// is written to the file filename_base.raw
//
//
// Information file:
// Filename: filename_base.info
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Beamlets file:
// Filename: filename_base.beamlets_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Subbands file:
// Filename: filename_base.subbands_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// RCUMODEs file:
// Filename: filename_base.rcumodes_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Right Ascensions file:
// Filename: filename_base.rightascensions_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Declinations file:
// Filename: filename_base.declinations_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// Epochs file:
// Filename: filename_base.epochs_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
// SourceNamess file:
// Filename: filename_base.sourcenames_lis
// The description is found in LOFAR_Station_Beamformed_Info.cxx
//
//
// Valid file.  A single file is written out to indicate which samples are
// valid (data came from the station, non-zero value) or invalid (no data
// available from the station, default value written to the data area, zero
// valued data written to the valid file).
// The filename is
// filename_base.valid
// The valid flags are written as
// individual bits to 8-bit bytes
// [0 1 2 3 4 5 6 7] [0 1 2 3 4 5 6 7] [0 1 2 3 4 5 6 7]
// So the valid signal for sample x is read as, assuming variables are unsigned
// byte = x >> 3
// bit  = x & 0x7
// signal = (valid_array[byte] >> bit) & 0x1
//
// Filename file:
// A file of name
// filename_base.file_lis
// is written.  It contains a single \n terminated string with the name of the
// filename_base.raw









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
#include "LOFAR_Station_Beamformed_Writer_LuMP0.h"
#include "LOFAR_Station_Beamformed_Writer_LuMP2.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "MPIfR_Sexagesimal_Angle.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>









// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {

namespace {
extern "C" void* pipe_writer_thread_entry(void* pthis)
{
    LOFAR_Station_Beamformed_Writer_LuMP2* object = reinterpret_cast<LOFAR_Station_Beamformed_Writer_LuMP2*>(pthis);
    return object->start_pipe_writer_thread();
}

}









// GLOBALS


// FUNCTIONS

LOFAR_Station_Beamformed_Writer_LuMP2::
LOFAR_Station_Beamformed_Writer_LuMP2(const char* const restrict filename_base_,
                                      const char* const restrict station_name_,
                                      const uint32_t reference_time_,
                                      const uint32_t end_time_,
                                      const uint_fast16_t CLOCK_SPEED_IN_MHz_,
                                      const uint_fast32_t PHYSICAL_BEAMLET_OFFSET_,
                                      const uint_fast16_t NUM_Beamlets_,
                                      const uint_fast16_t NUM_Blocks_,
                                      const uint_fast16_t NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE_,
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
                                      const char* const child_process_command_line_,
                                      const char* const child_process_channel_count_multiplier_,
                                      const char* const child_process_sample_blocksize_,
                                      const char* const child_process_logfile_)
    restrict throw()
            :
        LOFAR_Station_Beamformed_Writer_Base(filename_base_,
                                             station_name_,
                                             reference_time_,
                                             CLOCK_SPEED_IN_MHz_,
                                             PHYSICAL_BEAMLET_OFFSET_,
                                             NUM_Beamlets_,
                                             NUM_Blocks_,
                                             DATA_TYPE_,
                                             id_,
                                             NUM_OUTPUT_CHANNELS_,
                                             NUM_Output_Beamlets_,
                                             Physical_Beamlets_Array_,
                                             Physical_Subband_Array_,
                                             RCUMODE_Array_,
                                             RightAscension_Array_,
                                             Declination_Array_,
                                             Epoch_Array_,
                                             SourceName_Array_),
    end_time(end_time_),
    fp_data(NULL),
    num_raw_packet_commands_processed(0),
    initialized(0),
    status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
    valid_object(0),
    NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE(NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE_),
    CHILD_PROCESS_SAMPLE_BLOCKSIZE(0),
    CHILD_CHANNEL_COUNT_MULTIPLIER(0),
    child_process_command_line(0),
    child_process_logfile(0),
    output_beamlets_are_contiguous(true),
    LuMP2_BANDWIDTH_IN_HZ(-1.0),
    LuMP2_CENTER_FREQUENCY_HZ(-1.0),
    NUM_OUTPUT_BEAMLETS(0),
    beamlet_index(0),
    OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR(0),
    OUTPUT_BUFFER_LOFAR_BLOCK_LENGTH_CHAR(0),
    OUTPUT_BUFFER_NUM_SAMPLE_LINES(0),
    OUTPUT_BUFFER_TOTAL_LENGTH_CHAR(0),
    output_storage(0),
    output_storage_boss(0),
    output_storage_pipe_writer(0),
    num_bytes_in_boss_storage_buffer(0),
    num_bytes_in_pipe_writer_storage_buffer(0),
    pipe_writer_thread_running(0),
    pipe_writer_thread_ready(0),
    boss_has_pipe_writer_thread_ready_lock(0),
    thread_has_pipe_writer_thread_ready_lock(0),
    pipe_writer_thread_work_code(LOFAR_WRITER_BASE_WORK_WAIT),
    pipe_writer_thread_status(int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)),
    pipe_writer_thread_write_command_count(0),
    pipe_writer_thread_write_byte_count(0),
    pipe_writer_thread_write_sample_count(0),
    child_process_id(-1),
    child_fd(-1)
{
    // stuff for our own pipe_writer_thread
    int retval = pthread_mutex_init(&pipe_writer_thread_ready_mutex, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing pipe_writer_thread mutex failed");
        DEF_LOGGING_CRITICAL("initializing pipe_writer_thread %d mutex failed with %d\n", id, retval);
        exit(1);
    }
    retval = pthread_cond_init(&pipe_writer_thread_go_to_work_condition, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("initializing pipe_writer_thread cond failed");
        DEF_LOGGING_CRITICAL("initializing pipe_writer_thread %d cond failed with %d\n", id, retval);
        exit(1);
    }
    retval = pthread_create(&pipe_writer_thread, NULL, pipe_writer_thread_entry, this);    
    if((retval)) {
        DEF_LOGGING_PERROR("creating pipe_writer_thread failed");
        DEF_LOGGING_CRITICAL("creating pipe_writer_thread %d failed with %d\n", id, retval);
        exit(1);
    }

    copy_child_process_string_information(child_process_command_line_,
                                          child_process_channel_count_multiplier_,
                                          child_process_sample_blocksize_,
                                          child_process_logfile_);


    
    int_fast32_t retcode = wait_for_thread_sleeping();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to init properly\n", int(id));
        exit(1);
    }
    retcode = init_writer_processing();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to send init command\n", int(id));
        exit(1);
    }
    

    retval = int(wait_for_pipe_writer_thread_startup());
    if((retval)) {
        DEF_LOGGING_PERROR("wait_for_pipe_writer_thread_startup failed");
        DEF_LOGGING_CRITICAL("wait_for_pipe_writer_thread_startup WRITER %d failed with %d\n", id, retval);
        exit(1);
    }
    retcode = wait_for_pipe_writer_thread_sleeping();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to init properly\n", int(id));
        exit(1);
    }
    retcode = tell_pipe_writer_thread_to_init();
    if((retcode)) {
        DEF_LOGGING_CRITICAL("WRITER %d failed to send init command to pipe_writer_thread\n", int(id));
        exit(1);
    }

    return;
}

LOFAR_Station_Beamformed_Writer_LuMP2::
~LOFAR_Station_Beamformed_Writer_LuMP2()
{
    stop_thread();

    stop_pipe_writer_thread();
    WRITER_LuMP2_BOSS_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    pthread_mutex_destroy(&pipe_writer_thread_ready_mutex);
    pthread_cond_destroy(&pipe_writer_thread_go_to_work_condition);
    
    
    DEF_LOGGING_INFO("WRITER %d pipe_writer_thread statistics", int(id));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX write commands processed\n", int(id), (unsigned long long)(pipe_writer_thread_write_command_count));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX bytes sent to child\n", int(id), (unsigned long long)(pipe_writer_thread_write_byte_count));
    DEF_LOGGING_INFO("WRITER %d 0x%16.16llX samples sent to child\n", int(id), (unsigned long long)(pipe_writer_thread_write_sample_count));

    free(reinterpret_cast<void*>(child_process_command_line)); child_process_command_line=0;
    free(reinterpret_cast<void*>(child_process_logfile)); child_process_logfile=0;
    free(output_storage); output_storage=0; output_storage_boss=0; output_storage_pipe_writer=0;
    return;
}









int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
copy_child_process_string_information(const char* const child_process_command_line_,
                                      const char* const child_process_channel_count_multiplier_,
                                      const char* const child_process_sample_blocksize_,
                                      const char* const child_process_logfile_) throw()
{
    if(child_process_command_line_ == NULL) {
        DEF_LOGGING_CRITICAL("NULL command line string for child process");
        exit(2);
    }
    size_t command_size = strlen(child_process_command_line_)+1;
    if(command_size == 0) {
        DEF_LOGGING_CRITICAL("empty command line string for child process");
        exit(2);
    }
    child_process_command_line = reinterpret_cast<char*>(malloc(command_size));
    if(child_process_command_line == NULL) {
        DEF_LOGGING_CRITICAL("could not malloc %llu bytes for command_line", (unsigned long long)(command_size));
        exit(1);
    }
    strcpy(child_process_command_line,child_process_command_line_);
    if((child_process_sample_blocksize_ != NULL)
      && (child_process_sample_blocksize_[0] != 0)) {
        char* endptr = NULL;
        errno = 0;
        CHILD_PROCESS_SAMPLE_BLOCKSIZE = uint_fast64_t(strtoull(child_process_sample_blocksize_,&endptr,0));
        if((errno)) {
            DEF_LOGGING_PERROR("integer conversion failure");
            DEF_LOGGING_ERROR("Could not convert integer from string '%s'", child_process_sample_blocksize_);
            exit(2);
        }
        if(endptr == child_process_sample_blocksize_) {
            DEF_LOGGING_ERROR("Could not convert integer from string '%s'", child_process_sample_blocksize_);
            exit(2);
        }
    }
    else {
        CHILD_PROCESS_SAMPLE_BLOCKSIZE = 1;
    }
    if((child_process_channel_count_multiplier_ != NULL)
      && (child_process_channel_count_multiplier_[0] != 0)) {
        char* endptr = NULL;
        errno = 0;
        CHILD_CHANNEL_COUNT_MULTIPLIER = uint_fast32_t(strtoul(child_process_channel_count_multiplier_,&endptr,0));
        if((errno)) {
            DEF_LOGGING_PERROR("integer conversion failure");
            DEF_LOGGING_ERROR("Could not convert integer from string '%s'", child_process_channel_count_multiplier_);
            exit(2);
        }
        if(endptr == child_process_channel_count_multiplier_) {
            DEF_LOGGING_ERROR("Could not convert integer from string '%s'", child_process_channel_count_multiplier_);
            exit(2);
        }
    }
    else {
        CHILD_CHANNEL_COUNT_MULTIPLIER = 1;
    }
    // logfile
    if(child_process_logfile_ == NULL) {
        child_process_logfile = NULL;
    }
    else {
        size_t logfile_size = strlen(child_process_logfile_)+1;
        if(logfile_size == 0) {
            child_process_logfile = NULL;
        }
        else {
            child_process_logfile = reinterpret_cast<char*>(malloc(logfile_size));
            if(child_process_logfile == NULL) {
                DEF_LOGGING_CRITICAL("could not malloc %llu bytes for child_process_logfile", (unsigned long long)(logfile_size));
                exit(1);
            }
            strcpy(child_process_logfile,child_process_logfile_);
        }
    }
    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tokenize_command_line(char** command_p, char*** argv_p, char** memory_p) throw()
{
    // After return from tokenize_command_line, the caller is responsible
    // for freeing memory pointed to by *memory_p and *argv_p.
    if((command_p == NULL) || (argv_p == NULL) || (memory_p == NULL)) {
        DEF_LOGGING_CRITICAL("NULL address passed in");
        exit(1);
    }
    char dummy_buffer[1];
    // count the number of %'s
    int percent_count = 0;
    size_t orig_command_line_size = strlen(child_process_command_line)+1;
    for(size_t i=0; i < orig_command_line_size; i++) {
        if(child_process_command_line[i] == '%') {
            percent_count++;
        }
    }
    if((percent_count < 0) || (percent_count > 3)) {
        DEF_LOGGING_WARNING("bad number of percent symbols (%d) in command line string '%s' --- watch for errors in the child process processing", percent_count, child_process_command_line);
    }
    // Create the data header filename
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.raw", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
        exit(2);
    }
    // How many channels in the child processing?
    int CHILD_CHANNELS = int(NUM_OUTPUT_BEAMLETS*CHILD_CHANNEL_COUNT_MULTIPLIER);
    // do a test print to see how many characters we need
    int snprintf_size;
    if(percent_count == 0)
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_command_line);
    }
    else if(percent_count == 1)
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_command_line, new_filename);
    }
    else if(percent_count == 2)
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_command_line, CHILD_CHANNELS, new_filename);
    }
    else
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_command_line, CHILD_CHANNELS, filename_base, new_filename);
    }
    if(snprintf_size < 0) {
        DEF_LOGGING_ERROR("could not expand command string '%s'", child_process_command_line);
        exit(2);
    }
    size_t BYTES_NEEDED = snprintf_size+1;
    char* command_line = reinterpret_cast<char*>(malloc(BYTES_NEEDED));
    if(command_line == NULL) {
        DEF_LOGGING_CRITICAL("could not malloc %llu bytes for command_line", (unsigned long long)(BYTES_NEEDED));
        exit(1);
    }
    // now re-print command line
    if(percent_count == 0)
    {
        snprintf_size = snprintf(command_line,BYTES_NEEDED,child_process_command_line);
    }
    else if(percent_count == 1)
    {
        snprintf_size = snprintf(command_line,BYTES_NEEDED,child_process_command_line, new_filename);
    }
    else if(percent_count == 2)
    {
        snprintf_size = snprintf(command_line,BYTES_NEEDED,child_process_command_line, CHILD_CHANNELS, new_filename);
    }
    else
    {
        snprintf_size = snprintf(command_line,BYTES_NEEDED,child_process_command_line, CHILD_CHANNELS, filename_base, new_filename);
    }
    if(snprintf_size < ssize_t(BYTES_NEEDED)-1) {
        DEF_LOGGING_ERROR("could not expand command string '%s'", child_process_command_line);
        exit(2);
    }
    else if(snprintf_size >= ssize_t(BYTES_NEEDED)) {
        DEF_LOGGING_ERROR("error in second string expansion");
        exit(1);
    }
    // strip any space characters at end
    for(ssize_t pos=BYTES_NEEDED-1; pos >= 0; pos--) {
        if((isspace(command_line[pos]))) {
            command_line[pos] = 0;
        }
        else {
            break;
        }
    }
    DEF_LOGGING_INFO("WRITER %d has child process expanded command line '%s'", int(id), command_line);
    // Now tokenize the string
    int count = 0;
    bool last_was_valid = false;
    for(size_t i=0; i < BYTES_NEEDED; i++) {
        if((command_line[i] == ' ') || (command_line[i] == 0)) {
            command_line[i] = 0;
            if(last_was_valid) {
                count++;
            }
            last_was_valid = false;
        }
        else {
            last_was_valid = true;
        }
    }
    //DEF_LOGGING_DEBUG("have %d tokens", count);
    // The command is token 0
    // The arguments are in tokens 1->
    // allocate space for a NULL at the end
    if(count == 0) {
        DEF_LOGGING_CRITICAL("cound not find command token");
        exit(2);
    }
    char** argv = reinterpret_cast<char**>(malloc(sizeof(char*)*count+1));
    if(argv == NULL) {
        DEF_LOGGING_CRITICAL("could not malloc space for %d tokens", count);
        exit(1);
    }
    // now go back and copy the token locations
    int token_count = 0;
    char* command = NULL;
    last_was_valid = false;
    for(size_t i=0; i < BYTES_NEEDED; i++) {
        if((command_line[i] == ' ') || (command_line[i] == 0)) {
            last_was_valid = false;
        }
        else {
            if(!last_was_valid) {
                // new token
                if(token_count == 0) {
                    command = command_line+i;
                }
                argv[token_count] = command_line+i;
                token_count++;
                if(token_count > count) {
                    DEF_LOGGING_ERROR("programmer error");
                    exit(1);
                }
            }
            last_was_valid = true;
        }
    }
    if(token_count != count) {
        DEF_LOGGING_ERROR("programmer error");
        exit(1);
    }
    argv[token_count] = NULL;
    // copy into return variables
    *command_p = command;
    *argv_p = argv;
    *memory_p = command_line;
    return 0;
}







int LOFAR_Station_Beamformed_Writer_LuMP2::
open_child_logfile() throw()
{
    if((child_process_logfile == NULL) || (child_process_logfile[0] == 0)) {
        return -1;
    }
    char dummy_buffer[1];
    // count the number of %'s
    size_t orig_logfile_size = strlen(child_process_logfile)+1;
    int percent_count = 0;
    for(size_t i=0; i < orig_logfile_size; i++) {
        if(child_process_logfile[i] == '%') {
            percent_count++;
        }
    }
    if((percent_count < 0) || (percent_count > 1)) {
        DEF_LOGGING_WARNING("bad number of percent symbols (%d) in logfile string '%s' --- watch for errors in the child process processing", percent_count, child_process_logfile);
    }
    // do a test print to see how many characters we need
    int snprintf_size;
    if(percent_count == 0)
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_logfile);
    }
    else
    {
        snprintf_size = snprintf(dummy_buffer,1,child_process_logfile, int(id));
    }
    if(snprintf_size < 0) {
        DEF_LOGGING_ERROR("could not expand logfile string '%s'", child_process_logfile);
        exit(2);
    }
    size_t BYTES_NEEDED = snprintf_size+1;
    char* logfile = reinterpret_cast<char*>(malloc(BYTES_NEEDED));
    if(logfile == NULL) {
        DEF_LOGGING_CRITICAL("could not malloc %llu bytes for logfile", (unsigned long long)(BYTES_NEEDED));
        exit(1);
    }
    // now re-print command line
    if(percent_count == 0)
    {
        snprintf_size = snprintf(logfile,BYTES_NEEDED,child_process_logfile);
    }
    else
    {
        snprintf_size = snprintf(logfile,BYTES_NEEDED,child_process_logfile, int(id));
    }
    if(snprintf_size < ssize_t(BYTES_NEEDED)-1) {
        DEF_LOGGING_ERROR("could not expand logfile string '%s'", child_process_logfile);
        exit(2);
    }
    else if(snprintf_size >= ssize_t(BYTES_NEEDED)) {
        DEF_LOGGING_ERROR("error in second string expansion");
        exit(1);
    }
    DEF_LOGGING_INFO("WRITER %d has child process expanded logfile '%s'", int(id), logfile);
    // Now open the file for append
    int fd = open(logfile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd < 0) {
        DEF_LOGGING_PERROR("could not open child process logfile");
        DEF_LOGGING_ERROR("could not open child process logfile '%s'", logfile);
        free(reinterpret_cast<void*>(logfile)); logfile=0;
        return fd;
    };
    free(reinterpret_cast<void*>(logfile)); logfile=0;
    return fd;
}







int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
thread_constructor() restrict
{
    if((check_input_parameters())) return -1;
    if((set_up_work_buffer_areas())) return -2;
    if((open_standard_files())) return -5;

    return write_header_init();
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
thread_destructor() restrict
{
    close_output_files();
    delete valid_object; valid_object=0;
    free(reinterpret_cast<void*>(beamlet_index)); beamlet_index=0;
    // don't free the output_storage here --- do it in the main destructor
    // after the pipe_writer_thread is finished.

    DEF_LOGGING_INFO("WRITER %d processed 0x%16.16llX raw packet commands\n", int(id), (unsigned long long)(num_raw_packet_commands_processed));

    return 0;
}    










int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
check_input_parameters() restrict throw()
{
    if(NUM_OUTPUT_CHANNELS != 1) {
        DEF_LOGGING_CRITICAL("coded for 1 output channel, but have %u instead\n",
                (unsigned int)(NUM_OUTPUT_CHANNELS));
        exit(1);
    }
    if(NUM_RSP_BEAMLET_POLARIZATIONS != 2) {
        DEF_LOGGING_CRITICAL("coded for 2 polarizations, but have %d instead\n",
                int(NUM_RSP_BEAMLET_POLARIZATIONS));
        exit(1);
    }
    if(NUM_Output_Beamlets <= 0) {
        DEF_LOGGING_CRITICAL("Invalid number of output beamlets (%d) for this writer\n",
                (int)(NUM_Output_Beamlets));
        exit(1);
    }
    // Post-processing using the LuMP2 format assumes that all beamlets come
    // from the same physical antenna array, that all beamlets are pointed
    // at the same source, and so on.  Verify this.
    for(uint_fast16_t b=1; b < NUM_Output_Beamlets; b++) {
        bool match = true;
        if(RCUMODE_Array[b] != RCUMODE_Array[0]) match = false;
        if(RightAscension_Array[b] != RightAscension_Array[0]) match = false;
        if(Declination_Array[b] != Declination_Array[0]) match = false;
        if(strcmp(Epoch_Array[b], Epoch_Array[0])) match = false;
        if(strcmp(SourceName_Array[b], SourceName_Array[0])) match = false;
        if(!match) {
            DEF_LOGGING_WARNING("RCUMODE/RA/DEC/EPOCH/SOURCE_NAME for beamlet %d do not math those of beamlet 0 in WRITER %d --- downstream processing may be affected", int(b), int(id));
        }
    }
    return 0;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
set_up_work_buffer_areas() restrict throw()
{
    output_beamlets_are_contiguous = true;
    uint_fast32_t NUM_LuMP2_CHANNELS = 0;
    LuMP2_BANDWIDTH_IN_HZ = -1.0;
    LuMP2_CENTER_FREQUENCY_HZ = -1.0;
    Real64_t CHANNEL_CENTER_MIN = -1.0;
    const Real64_t CHANNEL_BANDWIDTH = NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz);
    {
        uint_fast32_t RCUMODE_0 = RCUMODE_Array[0];
        uint_fast32_t PHYSICAL_SUBBAND_0 = Physical_Subband_Array[0];
        CHANNEL_CENTER_MIN = 
            LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                           LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[0]),
                                           Physical_Subband_Array[0]);
        Real64_t CHANNEL_CENTER_MAX = CHANNEL_CENTER_MIN;
        for(uint_fast16_t beamlet=0; beamlet < NUM_Output_Beamlets; beamlet++) {
            if(RCUMODE_Array[beamlet] != RCUMODE_0) {
                output_beamlets_are_contiguous = false;
            }
            if(Physical_Subband_Array[beamlet] != PHYSICAL_SUBBAND_0 + beamlet) {
                output_beamlets_are_contiguous = false;
            }
            // Do not allow duplicate subbands
            for(uint_fast16_t beamlet_check=0; beamlet_check < beamlet; beamlet_check++) {
                if((RCUMODE_Array[beamlet_check] == RCUMODE_Array[beamlet])
                  && (Physical_Subband_Array[beamlet_check] == Physical_Subband_Array[beamlet])) {
                    DEF_LOGGING_ERROR("in WRITER %d, beamlet indices %u and %u have the same RCUMODE (%d) and subband (%u) --- this is not allowed by the LuMP2 format", int(id), (unsigned int)(beamlet_check), (unsigned int)(beamlet), int(RCUMODE_Array[beamlet_check]), (unsigned int)(Physical_Subband_Array[beamlet_check]));
                    return -1;
                }
            }
            Real64_t this_freq =
                MPIfR::LOFAR::Station::LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                                                      MPIfR::LOFAR::Station::LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[beamlet]),
                                                                      Physical_Subband_Array[beamlet]);
            if(this_freq < CHANNEL_CENTER_MIN) {
                CHANNEL_CENTER_MIN = this_freq;
            }
            else if(this_freq > CHANNEL_CENTER_MAX) {
                CHANNEL_CENTER_MAX = this_freq;
            }
        }
        LuMP2_BANDWIDTH_IN_HZ = (CHANNEL_CENTER_MAX-CHANNEL_CENTER_MIN)+CHANNEL_BANDWIDTH;
        NUM_LuMP2_CHANNELS = uint_fast32_t(floor(LuMP2_BANDWIDTH_IN_HZ/CHANNEL_BANDWIDTH + 0.5));
        LuMP2_CENTER_FREQUENCY_HZ = (CHANNEL_CENTER_MAX+CHANNEL_CENTER_MIN)*0.5;
    }
    DEF_LOGGING_INFO("received data has %d beamlets, outputing %d beamlets",
                     int(NUM_Output_Beamlets), int(NUM_LuMP2_CHANNELS));
    NUM_OUTPUT_BEAMLETS = NUM_LuMP2_CHANNELS;

    void* beamlet_index_temp;
    int retval = posix_memalign(&beamlet_index_temp, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, sizeof(int_fast32_t)*NUM_OUTPUT_BEAMLETS);
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for beamlet_offset with retval %d\n", (unsigned long long)(sizeof(int_fast32_t)*NUM_OUTPUT_BEAMLETS), retval);
        return -1;
    }
    beamlet_index = reinterpret_cast<uint_fast32_t* restrict>(beamlet_index_temp);

    // Now set up the beamlet offsets
    for(uint_fast32_t beamlet = 0; beamlet < NUM_Output_Beamlets; beamlet++) {
        Real64_t this_freq =
            MPIfR::LOFAR::Station::LOFAR_SUBBAND_FREQUENCY_CENTER(CLOCK_SPEED_IN_MHz,
                                                                  MPIfR::LOFAR::Station::LOFAR_NYQUIST_FREQUENCY_OFFSET(RCUMODE_Array[beamlet]),
                                                                  Physical_Subband_Array[beamlet]);
        uint_fast32_t pos = floor((this_freq - CHANNEL_CENTER_MIN)/CHANNEL_BANDWIDTH + 0.5);
        beamlet_index[beamlet] = pos;
    }
    // Optimization for copying with limited-width integers
    {
        uint_fast32_t multiplier = 1;
        switch(Voltage_Size) {
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
        case 128:
            multiplier = 16;
            break;
        default:
            DEF_LOGGING_CRITICAL("unsupported voltage size value %d bytes", int(Voltage_Size));
            exit(1);
        }
        // scale up the beamlet_index values
        for(uint_fast32_t beamlet = 0;
            beamlet < NUM_Output_Beamlets;
            beamlet++) {
            beamlet_index[beamlet] *= multiplier * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;
        }
    }

    // Now, how many sample lines do we store in our buffer before sending
    // them out to the child process?
    uint_fast64_t samples =
        NUM_PACKETS_TO_DUMP_TO_WRITERS_AT_ONCE * NUM_Blocks;
    // make a minimum buffer size of 128 sample lines
    if(samples < 128) {
        uint_fast64_t n = 128 / samples;
        if(( 128 % samples )) {
            n++;
        }
        samples = n*samples;
    }
    if(samples < CHILD_PROCESS_SAMPLE_BLOCKSIZE) {
        // Our natural block size is smaller than the child process block size.
        // Bump up to some multiple.
        uint_fast64_t n = CHILD_PROCESS_SAMPLE_BLOCKSIZE / samples;
        if(( CHILD_PROCESS_SAMPLE_BLOCKSIZE % samples )) {
            n++;
        }
        samples = n*samples;
    }
    OUTPUT_BUFFER_NUM_SAMPLE_LINES = samples;
    OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR = uint_fast64_t(NUM_OUTPUT_BEAMLETS) * Voltage_Size * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS;
    OUTPUT_BUFFER_LOFAR_BLOCK_LENGTH_CHAR = NUM_Blocks * OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR;
    OUTPUT_BUFFER_TOTAL_LENGTH_CHAR = OUTPUT_BUFFER_NUM_SAMPLE_LINES * OUTPUT_BUFFER_SAMPLE_LINE_LENGTH_CHAR;

    // Now allocate memory for the work arrays
    retval = posix_memalign(&output_storage, MPIFR_NUMERICAL_DEFAULT_DATA_ALIGNMENT, size_t(OUTPUT_BUFFER_TOTAL_LENGTH_CHAR*2));
    if((retval)) {
        DEF_LOGGING_ERROR("Could not allocate %llu bytes for output_storage with retval %d\n", (unsigned long long)(OUTPUT_BUFFER_TOTAL_LENGTH_CHAR*2), retval);
        return -1;
    }
    output_storage_boss = output_storage;
    output_storage_pipe_writer = reinterpret_cast<void*>(reinterpret_cast<char*>(output_storage)+OUTPUT_BUFFER_TOTAL_LENGTH_CHAR);

    // Clear the output buffer area.  Note that for all forms of LOFAR
    // data representations, all bits 0 indicates a value of 0.  This
    // makes sure that any subband frequency areas that are missing
    // are set to zero.
    // TODO:  Figure out what value dspsr uses to indicate data that
    //        need to be flagged.
    memset(output_storage, 0, size_t(OUTPUT_BUFFER_TOTAL_LENGTH_CHAR*2));
    
    // set up the area for the LuMP header
    strncpy(LuMP_header,LuMP_HEADER_BASE,LuMP_HEADER_SIZE);
    LuMP_header[LuMP_HEADER_SIZE-1] = 0;
    
    return 0;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
open_standard_files() restrict throw()
{
    // Now open the files
    const size_t FSIZE = 2048;
    char new_filename[FSIZE];
    int retval = snprintf(new_filename, FSIZE, "%s.file_lis", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating file filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    FILE* fp_out_file = fopen(new_filename,"wbx");
#else
    FILE* fp_out_file = fopen("/dev/null","wb");
#endif
    if(fp_out_file == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output file file '%s'\n",
                new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    
    retval = snprintf(new_filename, FSIZE, "%s.raw", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    fp_data = fopen(new_filename,"wbx");
#else
    fp_data = fopen("/dev/null","wb");
#endif
    if(fp_data == NULL) {
        DEF_LOGGING_CRITICAL("unable to open raw output data file '%s'\n",
                             new_filename);
        status=int32_t(LOFAR_WRITER_RETVAL_FILE_OPEN_FAIL);
        exit(2);
    }
    retval = fputs(strip_path(new_filename),fp_out_file);
    if(retval<0) {
        DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                             new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
        exit(2);
    }
    retval = fputc('\n', fp_out_file);
    if(retval<0) {
        DEF_LOGGING_CRITICAL("unable to write output data file name '%s' to file file\n",
                             new_filename);
        status=int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
        exit(2);
    }
    retval = fclose(fp_out_file);
    if(retval != 0) {
        DEF_LOGGING_PERROR("Error closing file");
        DEF_LOGGING_CRITICAL("Error closing file file from base '%s'\n", filename_base);
        exit(2);
    }
    retval = snprintf(new_filename, FSIZE, "%s.valid", filename_base);
    if((retval < 0) || (size_t(retval) >= FSIZE)) {
        DEF_LOGGING_CRITICAL("Error creating valid filename from base '%s'\n", filename_base);
        exit(2);
    }
#if !defined(LOFAR_DEBUG) || (LOFAR_DEBUG < 3)
    valid_object = new LOFAR_Station_Beamformed_Valid_Writer(new_filename);
#else
    valid_object = new LOFAR_Station_Beamformed_Valid_Writer("/dev/null");
#endif

    return 0;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
close_output_files() restrict throw()
{
    if((fp_data)) {
        int retval = fclose(fp_data); fp_data = NULL;
        if((retval)) {
            DEF_LOGGING_PERROR("closing raw data file failed");
            DEF_LOGGING_ERROR("closing '%s' data failed\n", filename_base);
            status = int_fast32_t(LOFAR_WRITER_RETVAL_CLOSE_FAIL);
        }
    }
            
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
report_file_error(const char* const restrict msg) restrict const throw()
{
    DEF_LOGGING_ERROR("error %s writing to file '%s'\n", msg, filename_base);
    return status;
}






int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    //DEF_LOGGING_DEBUG("do_work %d (%s) for LOFAR_Station_Beamformed_Writer_LuMP2 WRITER %d\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id));
    switch(work_code_copy) {
    case LOFAR_WRITER_BASE_WORK_WAIT:
        status = LOFAR_WRITER_RETVAL_WORK_IS_ZERO;
        break;
    case LOFAR_WRITER_BASE_WORK_INIT:
        status = thread_constructor();
        break;
    case LOFAR_WRITER_BASE_WORK_TIME:
        status = write_header_start();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_DATA:
        status = write_standard_packets();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA:
        status = write_partial_packet();
        break;
    case LOFAR_WRITER_BASE_WORK_SHUTDOWN:
        status = write_header_end();
        break;
    case LOFAR_WRITER_BASE_WORK_EXIT:
        thread_destructor();
        break;
    default:
        status = LOFAR_WRITER_RETVAL_UNKNOWN_WORK;
        break;
    }
    return status;
}











int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
write_header_init() restrict throw()
{
    // set up Info area
    Info.set_WRITER_TYPE(uint16_t(WRITER_TYPE()));
    LOFAR_STATION_BEAMFORMED_WRITER_BASE_SET_INFO_VERSION_DATE();
    Info.set_processing_DATA_TYPE(int32_t(DATA_TYPE));
    Info.set_output_DATA_TYPE(int32_t(DATA_TYPE));
    Info.set_channelizer_TYPE(int32_t(LOFAR_CHANNELIZER_NONE));
    Info.set_NUM_CHANNELS(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_channelization(uint32_t(NUM_OUTPUT_CHANNELS));
    Info.set_samples_per_integration(1);
    Info.set_seconds_per_channelization_full(SECONDS_PER_LINE);
    Info.set_seconds_per_channelization_eff(SECONDS_PER_LINE);
    Info.set_samples_per_channelization_eff(NUM_OUTPUT_CHANNELS);
    Info.set_seconds_per_integration_full(SECONDS_PER_LINE);
    Info.set_seconds_per_integration_eff(SECONDS_PER_LINE);
    Info.set_samples_per_integration_eff(1);
    Info.set_seconds_per_output(SECONDS_PER_LINE);
    Info.set_integration_multiplier(1.0);

    size_t retval;
    // Set up LuMP header as much as possible
    {
        int i;
        long long l;
        double d;
        char s[80];
        const char* cp=0;
        struct tm time_s;
        struct tm* tmp;
        time_t tt(reference_time_zero);

        const char* filename_p = filename_base + strlen(filename_base);
        while((*filename_p != '/') && (filename_p != filename_base))
        {
            --filename_p;
        }
        if(*filename_p == '/')
        {
            ++filename_p;
        }
        if(strlen(filename_p) > 40) {
            filename_p = "FILENAME_TOO_LONG";
        }

        
        i = int(LuMP_HEADER_SIZE);
        write_into_header(LuMP_HEADER_BASE_HDR_SIZE_OFFSET,"%d",6,i);
        i = 0;
        write_into_header(LuMP_HEADER_BASE_OBS_ID_OFFSET,"%d",1,i);
        {
            const size_t FSIZE = 2048;
            char new_filename[FSIZE];
            int retcode = snprintf(new_filename, FSIZE, "%s.raw", filename_p);
            if((retcode < 0) || (size_t(retcode) >= FSIZE)) {
                DEF_LOGGING_CRITICAL("Error creating data filename from base '%s'\n", filename_base);
                exit(2);
            }
            write_into_header(LuMP_HEADER_BASE_FILE_NAME_OFFSET,"%s",60,new_filename);
        }
        write_into_header(LuMP_HEADER_BASE_FILE_NUMBER_OFFSET,"%d",4,int(id));
        tmp = gmtime_r(&tt, &time_s);
        if(tmp == NULL) {
            goto write_header_error;
        }
        retval = strftime(s, 80, "%F-%T", &time_s);
        if(retval == 0) {
            goto write_header_error;
        }
        write_into_header(LuMP_HEADER_BASE_UTC_START_OFFSET,"%s",19,s);
        d = MJD(time_s.tm_year+1900, time_s.tm_mon+1, time_s.tm_mday,
                time_s.tm_hour, time_s.tm_min, time_s.tm_sec, 0.0);
        write_into_header(LuMP_HEADER_BASE_MJD_START_OFFSET,"%.12f",18,d);

        write_into_header(LuMP_HEADER_BASE_SOURCE_OFFSET,"%s", 33, SourceName_Array[0]);
        {
            using namespace MPIfR::Angles;
            const size_t RADEC_SIZE = 40;
            char radec_str[RADEC_SIZE];
            Sexagesimal_Angle<Real64_t> RA(RightAscension_Array[0]);
            Sexagesimal_Type_Enum t_RA = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_HOURS);
            // For a Real64_t, the machine precision is equivalent to about
            // 45 picoarcseconds, so there is no point in going beyond
            // picoarcseconds for printing positions with our Real64_t angles.
            // Printing to a picoarcsecond should only require 22 characters.
            // This is in hours, so need 13 digits to get to picoarcseconds
            int_fast32_t sretval = RA.to_sexagesimal_string(t_RA, ':', ':', 0, 13, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make RA string from radian value %E", RA.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_RA_OFFSET,"%s", 37, radec_str);
            Sexagesimal_Angle<Real64_t> Dec(Declination_Array[0]);
            Sexagesimal_Type_Enum t_Dec = Sexagesimal_Type_Enum(SEXAGESIMAL_TYPE_DEGREES);
            sretval = Dec.to_sexagesimal_string(t_Dec, ':', ':', 0, 12, RADEC_SIZE, radec_str);
            if((sretval)) {
                DEF_LOGGING_ERROR("could not make Dec string from radian value %E", Dec.radians());
                exit(1);
            }
            write_into_header(LuMP_HEADER_BASE_DEC_OFFSET,"%s", 36, radec_str);
        }

        // The telescope name is fudged to include the antenna field
        // so that the downstream pulsar software knows where the delay center
        // is located.  The LuMP2 format should not be used to cross frequencies
        // from low to high band (such as in the 3 5 7 mode).  The user is
        // responsible for making separate LuMP2 writers for the different
        // bands.
        {
            char telescope_name[LuMP_STATION_NAME_STR_SIZE];
            const char* const LuMP_name = LuMP_Telescope_Name_Converter(Info.get_station_name());
            if(LuMP_name != NULL) {
                // Use this one
                strncpy(telescope_name, LuMP_name, LuMP_STATION_NAME_STR_SIZE);
                const char* const input_name = LOFAR_RCUMODE_INPUT_CABLE_NAME(RCUMODE_Array[0]);
                telescope_name[5] = input_name[0];
                telescope_name[6] = input_name[1];
                telescope_name[7] = input_name[2];
                telescope_name[8] = 0;
            }
            else {
                strncpy(telescope_name, Info.get_station_name(), LuMP_STATION_NAME_STR_SIZE);
            }
            telescope_name[LuMP_STATION_NAME_STR_SIZE-1] = 0;
            write_into_header(LuMP_HEADER_BASE_TELESCOPE_OFFSET,"%s", 30, telescope_name);
        }
        // Frequency center
        d = LuMP2_CENTER_FREQUENCY_HZ * 1E-6; // Pulsar people use MHz
        write_into_header(LuMP_HEADER_BASE_FREQ_OFFSET,"%.17E",24,d);
        // Bandwidth
        // pulsar people use MHz
        d = LuMP2_BANDWIDTH_IN_HZ * 1E-6;
        write_into_header(LuMP_HEADER_BASE_BW_OFFSET,"%.16E",23,d);
        // Time per sample
        // pulsar people use \mu s
        d = 1.0E6/NUM_TIME_SAMPLES_PER_SECOND_REAL64(CLOCK_SPEED_IN_MHz); 
        write_into_header(LuMP_HEADER_BASE_TSAMP_OFFSET,"%.16E",23,d);
        // number of bits per sample type (real part of complex)
        // is our Voltage_Size in bytes * 8 bits per byte / 2 (real) sizes per
        // complex.  8/2 = 4, *4 --> <<2
        i = int(Voltage_Size<<2);
        write_into_header(LuMP_HEADER_BASE_NBIT_OFFSET,"%d",3,i);
        i = int(NUM_OUTPUT_BEAMLETS);
        // 10 digits is enough for 2^{31} channels, which would take
        // 3 hours of integration time to get enough samples.  
        write_into_header(LuMP_HEADER_BASE_NCHAN_OFFSET,"%d",10,i);
        // binary formats are
        // "INTEGER_BIN_FORM"
        // "IEEE_FLOAT"
        write_into_header(LuMP_HEADER_BASE_BINARY_FORMAT_OFFSET,"%s",16,"INTEGER_BIN_FORM");
        // pol ordering formats are
        // "DSPSR"
        // "JONES_MATRIX"
        write_into_header(LuMP_HEADER_BASE_POL_ORDERING_OFFSET,"%s",5,"DSPSR");
        // endian formats are
        // "BIG"
        // "LITTLE"
        write_into_header(LuMP_HEADER_BASE_ENDIAN_OFFSET,"%s",6,
#if __BYTE_ORDER == __LITTLE_ENDIAN
                          "LITTLE"
#else
                          "BIG"
#endif
                          );
        l = (long long)reference_time_zero;
        write_into_header(LuMP_HEADER_BASE_UTC_REFERENCE_TIME_T_OFFSET,"0x%llX",17,l);
        // State can be
        // //! Nyquist sampled voltages (real)
        // Nyquist,
        // //! In-phase and Quadrature sampled voltages (complex)
        // Analytic,
        // //! Square-law detected total power
        // Intensity,
        // //! Square-law detected nth power
        // NthPower,
        // //! Square-law detected, two polarizations
        // PPQQ,
        // //! PP, QQ, Re[PQ], Im[PQ]
        // Coherence,
        // //! Stokes I,Q,U,V
        // Stokes,
        // //! PseudoStokes S0,S2,S2,S3
        // PseudoStokes,
        // //! Stokes invariant interval
        // Invariant,
        // //! Other
        // Other,
        // //! Just PP
        // PP_State,
        // //! Just QQ
        // QQ_State,
        // //! Fourth moment of the electric field (covariance of Stokes parameters)
        // FourthMoment
        write_into_header(LuMP_HEADER_BASE_STATE_OFFSET,"%s",8,"Analytic");
        i = int(VERSION());
        write_into_header(LuMP_HEADER_BASE_LUMP_VERSION_OFFSET,"%X",4,i);
        d = 1.0;
        write_into_header(LuMP_HEADER_BASE_LUMP_SCALE_OFFSET,"%.16E",23,d);

        // Antenna and filter sets
        cp = LOFAR_ANTENNA_SET_Str(Info.get_station_name(),int16_t(RCUMODE_Array[0]));
        write_into_header(LuMP_HEADER_BASE_ANTENNA_SET_OFFSET,"%s",15,cp);
        cp = LOFAR_FILTER_SET_Str(int16_t(RCUMODE_Array[0]));
        write_into_header(LuMP_HEADER_BASE_FILTER_SELECTION_OFFSET,"%s",15,cp);
        cp = "TRUE";
        write_into_header(LuMP_HEADER_BASE_READ_DATA_FROM_PIPE_OFFSET,"%s",6,cp);
        i = int(NUM_Output_Beamlets);
        // Write out the actual number of beamlets written to the file,
        // as opposed to the effective number of channels (including gaps)
        // that is written into NCHAN.
        // 10 digits is enough for 2^{31} channels, which would take
        // 3 hours of integration time to get enough samples.  
        write_into_header(LuMP_HEADER_BASE_NCHAN_RECORDED_OFFSET,"%d",10,i);
        
    }

        
    if(status == int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET)) {
        // Good
    }
    else {
        goto write_header_status_bad;
    }
    
    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    rewind(fp_data);
    retval = fwrite(LuMP_header, LuMP_HEADER_SIZE, 1, fp_data);
    if(retval != 1) {
        goto write_header_error;
    }
    
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_init");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
write_header_start() restrict throw()
{
    Info.set_sample_start(int64_t(first_sample_received));
    // The center time in seconds
    // since time_reference_zero of the zeroeth
    // output point.
    // This is just
    // sample_start * seconds_per_sample
    Real64_t time_center_output_zero =
        first_sample_received * SECONDS_PER_LINE;
    Info.set_time_center_output_zero(time_center_output_zero);
    Real64_t time_leading_edge_output_zero = time_center_output_zero
        - 0.5*SECONDS_PER_LINE;

    // Make a guess at the total number of samples we will see
    uint_fast64_t NUM_TOTAL_SAMPLES = expected_sample_count();
    Info.set_num_output_timeslots(uint64_t(NUM_TOTAL_SAMPLES));
    // the FILE_SIZE keyword in the LuMP header gives the size in bytes
    // that the file would have had if written out with the real data only,
    // including the header.
    uint_fast64_t NUM_TRUE_BYTES = NUM_TOTAL_SAMPLES * Voltage_Size * MPIfR::LOFAR::Station::NUM_RSP_BEAMLET_POLARIZATIONS * NUM_Output_Beamlets + LuMP_HEADER_SIZE;
    write_into_header(LuMP_HEADER_BASE_FILE_SIZE_OFFSET,"%16lld",16,(long long)(NUM_TRUE_BYTES));
 

    size_t retval;
    double d;
    int_fast32_t pipe_writer_return_code;

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==0)) {
        // Good
    }
    else {
        goto write_header_status_bad;
    }

    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    d = time_center_output_zero; // This one is in seconds as it is my keyword.
    write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_CENTER_OFFSET,"%.17E",24,d);
    d = time_leading_edge_output_zero;//This one is in seconds as it is my keyword.
    write_into_header(LuMP_HEADER_BASE_UTC_OFFSET_START_LEADING_OFFSET,"%.17E",24,d);
        
    rewind(fp_data);
    retval = fwrite(LuMP_header, LuMP_HEADER_SIZE, 1, fp_data);
    if(retval != 1) {
        goto write_header_error;
    }
    // flush the header, so that the child process can read it.
    retval = fflush(fp_data);
    if(retval < 0) {
        goto write_header_error;
    }

    pipe_writer_return_code = tell_pipe_writer_thread_to_start_child();
    if((pipe_writer_return_code)) {
        goto pipe_writer_status_bad;
    }
    
    
    initialized=1;
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_start");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
pipe_writer_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_CHILD_FAILURE);
    return status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
write_header_end() restrict throw()
{
    int_fast32_t pipe_writer_return_code;
    Info.set_num_output_timeslots(uint64_t(last_sample_written_plus_1-first_sample_received));

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_header_status_bad;
    }

    if(Info.write_to_file() == 0) {}
    else {
        goto write_header_error;
    }

    pipe_writer_return_code = tell_pipe_writer_thread_to_end_data();
    if((pipe_writer_return_code)) {
        goto pipe_writer_status_bad;
    }

    initialized=2;
    status = int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
    return status;
write_header_error:
    report_file_error("write_header_end");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_header_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    return status;
pipe_writer_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_CHILD_FAILURE);
    return status;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
write_standard_packets() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing packet command number %llX line %llX\n", (unsigned long long)num_raw_packet_commands_processed, (unsigned long long)current_sample_offset);

    int_fast32_t return_val;
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the different data streams
    switch(Voltage_Size) {
    case 1:
        return_val = copy_block_to_output<uint8_t>(1);
        break;
    case 2:
        return_val = copy_block_to_output<uint16_t>(1);
        break;
    case 4:
        return_val = copy_block_to_output<uint32_t>(1);
        break;
    case 8:
        return_val = copy_block_to_output<uint64_t>(1);
        break;
    case 16:
        return_val = copy_block_to_output<uint64_t>(2);
        break;
    case 32:
        return_val = copy_block_to_output<uint64_t>(4);
        break;
    case 64:
        return_val = copy_block_to_output<uint64_t>(8);
        break;
    case 128:
        return_val = copy_block_to_output<uint64_t>(16);
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d bytes\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    if((return_val)) {
        goto write_data_error;
    }
    // write to the valid stream
    last_sample_written_plus_1 = current_sample_offset;
    for(uint_fast32_t packet=0; packet < current_num_packets; packet++) {
        if(valid_object->write_N_points(NUM_Blocks,current_valid[packet]) == 0){
        }
        else {
            goto write_data_error;
        }
        if((current_valid[packet])) {
            num_valid_samples += NUM_Blocks;
        }
        else {
            num_invalid_samples += NUM_Blocks;
        }
        last_sample_written_plus_1 += NUM_Blocks;
    }
    num_packets_received += current_num_packets;
    ++num_raw_packet_commands_processed;
    return status;
write_data_error:
    report_file_error("write_standard_packets");
    close_output_files();
    status = int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
write_partial_packet() restrict throw()
{
    //DEF_LOGGING_DEBUG("Writing partial line %llX\n", (unsigned long long)current_sample_offset);
    int_fast32_t return_val;
    const uint_fast16_t NUM_ACTUAL_BLOCKS(current_sample_stop_offset
                                         - current_sample_offset);
    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    // write to the data stream
    if(NUM_ACTUAL_BLOCKS > NUM_Blocks) {
        DEF_LOGGING_CRITICAL("Error in write_partial_packet(), got %llX %llX sample positions\n",
                (unsigned long long)(current_sample_offset),
                (unsigned long long)(current_sample_stop_offset));
        exit(1);
    }
    switch(Voltage_Size) {
    case 1:
        return_val = copy_block_to_output_partial<uint8_t>(1);
        break;
    case 2:
        return_val = copy_block_to_output_partial<uint16_t>(1);
        break;
    case 4:
        return_val = copy_block_to_output_partial<uint32_t>(1);
        break;
    case 8:
        return_val = copy_block_to_output_partial<uint64_t>(1);
        break;
    case 16:
        return_val = copy_block_to_output_partial<uint64_t>(2);
        break;
    case 32:
        return_val = copy_block_to_output_partial<uint64_t>(4);
        break;
    case 64:
        return_val = copy_block_to_output_partial<uint64_t>(8);
        break;
    case 128:
        return_val = copy_block_to_output_partial<uint64_t>(16);
        break;
    default:
        DEF_LOGGING_CRITICAL("unsupported LOFAR_raw_data_type_enum %d size %d\n", int(DATA_TYPE), int(Voltage_Size));
        exit(1);
    }
    if((return_val)) {
        goto write_data_error;
    }
    // write to the valid stream
    last_sample_written_plus_1 = current_sample_offset;
    if(valid_object->write_N_points(NUM_ACTUAL_BLOCKS,
                                    current_valid[0]) == 0){
    }
    else {
        goto write_data_error;
    }
    if((current_valid[0])) {
        num_valid_samples += NUM_ACTUAL_BLOCKS;
    }
    else {
        num_invalid_samples += NUM_ACTUAL_BLOCKS;
    }
    last_sample_written_plus_1 += NUM_ACTUAL_BLOCKS;
    ++num_packets_received;
    return status;
write_data_error:
    report_file_error("write_partial_packet");
    close_output_files();
    status = int32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
    return status;
write_data_status_bad:
    status = int32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
    return status;
}




void LOFAR_Station_Beamformed_Writer_LuMP2::
write_into_header(uint_fast32_t pos,
                  const char* const format,
                  uint_fast32_t MAX_SIZE)
    restrict throw()
{
    // clear any previous value
    memset(LuMP_header+pos, ' ', MAX_SIZE);

    size_t len = strlen(format);
    if(len <= MAX_SIZE) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("formatted string  '%s' more than %u characters, pos %d\n",
                format, int(MAX_SIZE), (unsigned int)(pos));
        exit(1);
    }
    if(pos+len < LuMP_HEADER_SIZE) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("formatted string  '%s' overflows LuMP_header at pos %u\n",
                format, (unsigned int)(pos));
        exit(1);
    }
    strncpy(LuMP_header+pos, format, len);
    return;
}



template <class T>
void LOFAR_Station_Beamformed_Writer_LuMP2::
write_into_header(uint_fast32_t pos,
                  const char* const format,
                  uint_fast32_t MAX_SIZE,
                  const T datap)
    restrict throw()
{
    static const uint_fast32_t BUF_SIZE= 80;
    char buf[BUF_SIZE];
    // now get the new information
    int retval = snprintf(buf, BUF_SIZE, format, datap);
    if((retval > 0) && (uint_fast32_t(retval) < BUF_SIZE)) {
        // ok
    }
    else {
        DEF_LOGGING_CRITICAL("Error printing to format '%s', pos %u\n",
                             format, (unsigned int)(pos));
        exit(1);
    }
    write_into_header(pos,buf,MAX_SIZE);
    return;
}









///////////////////////////////////////////////////////////////////////////////
// boss functions to send signals to pipe_writer_thread to do things
///////////////////////////////////////////////////////////////////////////////



int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tell_pipe_writer_thread_to_init() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_pipe_writer_thread_to_init");
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_INIT;
    
    WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tell_pipe_writer_thread_to_start_child() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_pipe_writer_thread_to_start_child");
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_TIME;
    
    WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tell_pipe_writer_thread_to_send_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_pipe_writer_thread_to_send_data");
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_WRITE_DATA;
    num_bytes_in_pipe_writer_storage_buffer = num_bytes_in_boss_storage_buffer;
    num_bytes_in_boss_storage_buffer = 0;
    void* temp=output_storage_pipe_writer;
    output_storage_pipe_writer = output_storage_boss;
    output_storage_boss = temp;
    
    WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tell_pipe_writer_thread_to_send_partial_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_pipe_writer_thread_to_send_partial_data");
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA;
    num_bytes_in_pipe_writer_storage_buffer = num_bytes_in_boss_storage_buffer;
    num_bytes_in_boss_storage_buffer = 0;
    void* temp=output_storage_pipe_writer;
    output_storage_pipe_writer = output_storage_boss;
    output_storage_boss = temp;
    
    WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}

int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
tell_pipe_writer_thread_to_end_data() restrict throw()
{
    //DEF_LOGGING_DEBUG("start tell_pipe_writer_thread_to_end_data");
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_SHUTDOWN;
    
    WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return 0;
    //DEF_LOGGING_DEBUG("done");
}




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
stop_pipe_writer_thread() restrict throw()
{
    //DEF_LOGGING_INFO("WRITER %d stopping pipe_writer_thread:\n", int(id));
    {
        int_fast32_t retval = wait_for_pipe_writer_thread_sleeping();
        if((retval)) {
            return retval;
        }
    }
    //WRITER_LuMP2_BOSS_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    if(pipe_writer_thread_running != 2) {
        pipe_writer_thread_work_code=LOFAR_WRITER_BASE_WORK_EXIT;
        
        WRITER_LuMP2_BOSS_BROADCAST_CONDITION(&pipe_writer_thread_go_to_work_condition);
    }
    else {
        // continue
    }
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    
    void* join_retval;
    int retval = pthread_join(pipe_writer_thread, &join_retval);
    if((retval)) {
        DEF_LOGGING_PERROR("joining pipe_writer_thread failed");
        DEF_LOGGING_CRITICAL("joining pipe_writer_thread %d failed with %d\n", int(id), retval);
        exit(1);
    }
    if(join_retval != NULL) {
        DEF_LOGGING_WARNING("pipe_writer_thread %d returned %p\n", int(id), join_retval);
    }
    DEF_LOGGING_INFO("WRITER %d pipe_writer_thread stopped:\n", int(id));

    if(pipe_writer_thread_running == 2) {
        return 0;
    }
    return -4;
}






///////////////////////////////////////////////////////////////////////////////
// pipe_writer_thread functions for doing things
///////////////////////////////////////////////////////////////////////////////

void* LOFAR_Station_Beamformed_Writer_LuMP2::
start_pipe_writer_thread() restrict
{
    //DEF_LOGGING_DEBUG("start_pipe_writer_thread\n");
    WRITER_LuMP2_THREAD_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    if(pipe_writer_thread_running == 0) {
        // ok, continue
    }
    else {
        WRITER_LuMP2_THREAD_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
        return (void*)(-1);
    }
    pipe_writer_thread_running = 1;
    WRITER_LuMP2_THREAD_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    //DEF_LOGGING_DEBUG("start_pipe_writer_thread running thread\n");
    return run_pipe_writer_thread();
}
    
    

void* LOFAR_Station_Beamformed_Writer_LuMP2::
run_pipe_writer_thread() restrict
{
    //DEF_LOGGING_DEBUG("starting run_pipe_writer_thread WRITER %d setting work condition to WAIT\n", id);
    WRITER_LuMP2_THREAD_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    while(1) {
        // inform the boss that we are ready for new data
        pipe_writer_thread_ready = 1;
        //DEF_LOGGING_DEBUG("pipe_writer_thread %d setting work condition to WAIT\n", id);
        // wait for an instruction
        while(pipe_writer_thread_work_code == LOFAR_WRITER_BASE_WORK_WAIT) {
            WRITER_LuMP2_THREAD_CONDITION_WAIT(&pipe_writer_thread_go_to_work_condition,
                                              &pipe_writer_thread_ready_mutex);
        }
        LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_copy = pipe_writer_thread_work_code;
        //DEF_LOGGING_DEBUG("pipe_writer_thread %d go_to_work %d\n", id, int(work_code));
        pipe_writer_thread_ready = 0;
        //DEF_LOGGING_DEBUG("pipe_writer_thread %d go_to_work %d\n", int(id), int(work_code));
        WRITER_LuMP2_THREAD_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
        // check the condition value and do some work
        int_fast32_t return_code = pipe_writer_thread_do_work(work_copy);
        //DEF_LOGGING_DEBUG("pipe_writer_thread %d got %d back from do_work\n", int(id), int(return_code));
        // Now that the work is done, tell the boss that the packets are free
        // to be re-used
        WRITER_LuMP2_THREAD_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
        pipe_writer_thread_work_code = LOFAR_WRITER_BASE_WORK_WAIT;
        if((return_code)) {
            break;
        }
        if(work_copy==LOFAR_WRITER_BASE_WORK_EXIT) {
            break;
        }
    }
    pipe_writer_thread_ready=0;
    pipe_writer_thread_running=2;
    WRITER_LuMP2_THREAD_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    //DEF_LOGGING_DEBUG("pipe_writer_thread %d exiting\n", id);
    pthread_exit(NULL);
    // never reached
    return NULL;
}





int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
wait_for_pipe_writer_thread_startup() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_pipe_writer_thread_startup WRITER %d\n", int(id));
    int_fast32_t return_code = 0;
    while(1) {
        WRITER_LuMP2_BOSS_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_pipe_writer_thread_startup 2 thread %d pipe_writer_thread_ready %d work %d\n", int(id),int(pipe_writer_thread_ready), int(pipe_writer_thread_work_code));
        if(pipe_writer_thread_running==2) {
            // thread died
            return_code = -1;
            break;
        }
        else if(pipe_writer_thread_running == 0) {
            // wait for the thread to finish starting up
        }
        else if((pipe_writer_thread_ready)) {
            return_code = 0;
            break;
        }
        WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    }
    WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    return return_code;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
wait_for_pipe_writer_thread_sleeping() restrict throw()
{
    //DEF_LOGGING_DEBUG("wait_for_pipe_writer_thread_sleeping in WRITER %d\n", int(id));
    while(1) {
        WRITER_LuMP2_BOSS_LOCK_MUTEX(&pipe_writer_thread_ready_mutex);
        //DEF_LOGGING_DEBUG("wait_for_pipe_writer_thread_sleeping 2 WRITER %d pipe_writer_thread_ready %d work %d (%s)\n", int(id),int(pipe_writer_thread_ready), int(pipe_writer_thread_work_code), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(pipe_writer_thread_work_code));
        if(pipe_writer_thread_running==2) {
            // thread died
            return -1;
        }
        else if(pipe_writer_thread_running == 0) {
            // thread should already have started, wait_for_pipe_writer_thread_startup
            // was called in constructor
            return -2;
        }
        else if(pipe_writer_thread_work_code != LOFAR_WRITER_BASE_WORK_WAIT) {
            // wait for the thread to finish
        }
        else if((pipe_writer_thread_ready)) {
            return 0;
        }
        WRITER_LuMP2_BOSS_UNLOCK_MUTEX(&pipe_writer_thread_ready_mutex);
    }
    // never reached
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_thread_do_work(LOFAR_Station_Beamformed_Writer_Base_Work_Enum work_code_copy) restrict throw()
{
    //DEF_LOGGING_DEBUG("pipe_writer_thread_do_work got work code %d (%s) for LOFAR_Station_Beamformed_Writer_LuMP2 WRITER %d\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id));
    switch(work_code_copy) {
    case LOFAR_WRITER_BASE_WORK_WAIT:
        pipe_writer_thread_status = LOFAR_WRITER_RETVAL_WORK_IS_ZERO;
        break;
    case LOFAR_WRITER_BASE_WORK_INIT:
        pipe_writer_thread_status = LOFAR_WRITER_RETVAL_GOOD;
        break;
    case LOFAR_WRITER_BASE_WORK_TIME:
        pipe_writer_thread_status = pipe_writer_thread_constructor();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_DATA:
        pipe_writer_thread_status = pipe_writer_thread_write_buffer();
        break;
    case LOFAR_WRITER_BASE_WORK_WRITE_PARTIAL_DATA:
        pipe_writer_thread_status = pipe_writer_thread_write_buffer();
        break;
    case LOFAR_WRITER_BASE_WORK_SHUTDOWN:
        pipe_writer_thread_status = pipe_writer_thread_stop_writing();
        break;
    case LOFAR_WRITER_BASE_WORK_EXIT:
        pipe_writer_thread_destructor();
        break;
    default:
        pipe_writer_thread_status = LOFAR_WRITER_RETVAL_UNKNOWN_WORK;
        break;
    }
    //DEF_LOGGING_DEBUG("pipe_writer_thread_do_work finished work code %d (%s) for LOFAR_Station_Beamformed_Writer_LuMP2 WRITER %d with result %d (%s)\n", int(work_code_copy), LOFAR_Station_Beamformed_Writer_Base_Work_Enum_Str(work_code_copy), int(id), int(pipe_writer_thread_status), LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM_Str(LOFAR_Station_Beamformed_Writer_Base_RETVAL_ENUM(pipe_writer_thread_status)));
    return pipe_writer_thread_status;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_thread_constructor() restrict throw()
{
    // Get the command and arguments for the child process
    char* command_p = 0;
    char** argv = 0;
    char* memory_p = 0;
    int_fast32_t return_code = tokenize_command_line(&command_p,
                                                    &argv,
                                                    &memory_p);
    if((return_code)) {
        DEF_LOGGING_ERROR("could not tokenize child process command (code %d)", int(return_code));
        return -1;
    }
    // get a file descriptor for the child process stdout/stderr
    int child_stdout_fd = open_child_logfile();
    // now create the child
    child_process_id = pipe_writer_popen(command_p, argv, child_stdout_fd, &child_fd);
    if(child_process_id < 0) {
        DEF_LOGGING_CRITICAL("could not start child process");
        DEF_LOGGING_DEBUG("Arguments were:");
        int i=0;
        for(char* cp=argv[i]; cp != NULL; cp = argv[++i]) {
            DEF_LOGGING_DEBUG("arg %d='%s'", i, cp);
        }
        return -2;
    }
    return 0;
}


int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_thread_destructor() restrict throw()
{
    if(child_fd>=0) {
        close(child_fd); child_fd = -1;
    }
    DEF_LOGGING_INFO("WRITER %d waiting for child process to finish", int(id));
    if(child_process_id >= 0) {
        int status;
        pid_t wait_return = waitpid(child_process_id, &status, 0);
        if(wait_return != child_process_id) {
            DEF_LOGGING_ERROR("waitpid on child process for WRITER %d failed with return value %d (child process id was %d)", int(id), int(wait_return), int(child_process_id));
            return -1;
        }
        child_process_id = -1;
        DEF_LOGGING_INFO("WRITER %d child process returned status %d", int(id), status);
        if(status != 0) {
            return -2;
        }
    }
    return 0;
}    




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_thread_write_buffer() restrict throw()
{
    //DEF_LOGGING_DEBUG("pipe_writer_thread_write_buffer Writing to child process");
    ssize_t write_return = 0;
    size_t total_written = 0;
    int my_errno = 0;

    if((status == int_fast32_t(LOFAR_WRITER_RETVAL_GOOD)) && (initialized==1)){
        
    }
    else {
        goto write_data_status_bad;
    }
    if(pipe_writer_thread_running != 1) {
        // should not be here
        DEF_LOGGING_ERROR("the pipe_writer_thread is not running");
        goto write_data_running_bad;
    }

    while(total_written < num_bytes_in_pipe_writer_storage_buffer) {
        write_return = write(child_fd, reinterpret_cast<const void*>(reinterpret_cast<char*>(output_storage_pipe_writer)+total_written), num_bytes_in_pipe_writer_storage_buffer-total_written);
        if(write_return < 0) {
            my_errno = errno;
            // check error codes
            if((my_errno == EAGAIN)
              || (my_errno == EWOULDBLOCK)
              || (my_errno == EINTR)) {
                // try again
            }
            else {
                DEF_LOGGING_ERROR("write to child pipe failed with error code %d", my_errno);
                goto write_data_error;
            }
        }
        else {
            total_written += write_return;
        }
    }
    return int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
write_data_error:
    return int_fast32_t(LOFAR_WRITER_RETVAL_WRITE_FAIL);
write_data_status_bad:
    return int_fast32_t(LOFAR_WRITER_RETVAL_NOT_INITIALIZED_YET);
write_data_running_bad:
    return int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
}




int_fast32_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_thread_stop_writing() restrict throw()
{
    //DEF_LOGGING_DEBUG("start pipe_writer_thread_stop_writing");
    if(pipe_writer_thread_running != 1) {
        // should not be here
        DEF_LOGGING_ERROR("the pipe_writer_thread is not running");
        return int_fast32_t(LOFAR_WRITER_RETVAL_UNEXPECTED_INITIALIZATION);
    }
    if(child_fd>=0) {
        close(child_fd); child_fd = -1;
    }
    return int_fast32_t(LOFAR_WRITER_RETVAL_GOOD);
}



uint_fast64_t LOFAR_Station_Beamformed_Writer_LuMP2::
expected_sample_count(void) const throw()
{
    uint_fast64_t reference_time_zero = make_reference_time_zero(reference_time);
    uint_fast64_t time_offset = end_time - reference_time_zero;
    uint_fast64_t total_count = (time_offset >> 1) * NUM_TIME_SAMPLES_PER_TWO_SECONDS(CLOCK_SPEED_IN_MHz);
    if(time_offset & 0x1) {
        // odd number of seconds difference
        total_count += NUM_TIME_SAMPLES_PER_EVEN_SECOND(CLOCK_SPEED_IN_MHz);
    }
    uint_fast64_t difference = total_count - first_sample_received;
    return difference;
}



// The following popen code is taken from
// https://gist.github.com/nitrogenlogic/1022231
// /*
//  * This implementation of popen3() was created from scratch in June of 2011. It
//  * is less likely to leak file descriptors if an error occurs than the 2007
//  * version and has been tested under valgrind. It also differs from the 2007
//  * version in its behavior if one of the file descriptor parameters is NULL.
//  * Instead of closing the corresponding stream, it is left unmodified (typically
//  * sharing the same terminal as the parent process). It also lacks the
//  * non-blocking option present in the 2007 version.
//  *
//  * No warranty of correctness, safety, performance, security, or usability is
//  * given. This implementation is released into the public domain, but if used
//  * in an open source application, attribution would be appreciated.
//  *
//  * Mike Bourgeous
//  * https://github.com/nitrogenlogic
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
 
// /*
//  * Sets the FD_CLOEXEC flag. Returns 0 on success, -1 on error.
//  */
// static int set_cloexec(int fd)
// {
//     if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) == -1) {
//         perror("Error setting FD_CLOEXEC flag");
//         return -1;
//     }
 
//     return 0;
// }
 
// /*
//  * Runs command in another process, with full remote interaction capabilities.
//  * Be aware that command is passed to sh -c, so shell expansion will occur.
//  * Writing to *writefd will write to the command's stdin. Reading from *readfd
//  * will read from the command's stdout. Reading from *errfd will read from the
//  * command's stderr. If NULL is passed for writefd, readfd, or errfd, then the
//  * command's stdin, stdout, or stderr will not be changed. Returns the child
//  * PID on success, -1 on error.
//  */
// pid_t popen3(char *command, int *writefd, int *readfd, int *errfd)
// {
//     int in_pipe[2] = {-1, -1};
//     int out_pipe[2] = {-1, -1};
//     int err_pipe[2] = {-1, -1};
//     pid_t pid;
 
// // 2011 implementation of popen3() by Mike Bourgeous
// // https://gist.github.com/1022231
 
//     if(command == NULL) {
//         fprintf(stderr, "Cannot popen3() a NULL command.\n");
//         goto error;
//     }
 
//     if(writefd && pipe(in_pipe)) {
//         perror("Error creating pipe for stdin");
//         goto error;
//     }
//     if(readfd && pipe(out_pipe)) {
//         perror("Error creating pipe for stdout");
//         goto error;
//     }
//     if(errfd && pipe(err_pipe)) {
//         perror("Error creating pipe for stderr");
//         goto error;
//     }
 
//     pid = fork();
//     switch(pid) {
//     case -1:
// // Error
//         perror("Error creating child process");
//         goto error;
 
//     case 0:
// // Child
//         if(writefd) {
//             close(in_pipe[1]);
//             if(dup2(in_pipe[0], 0) == -1) {
//                 perror("Error assigning stdin in child process");
//                 exit(-1);
//             }
//             close(in_pipe[0]);
//         }
//         if(readfd) {
//             close(out_pipe[0]);
//             if(dup2(out_pipe[1], 1) == -1) {
//                 perror("Error assigning stdout in child process");
//                 exit(-1);
//             }
//             close(out_pipe[1]);
//         }
//         if(errfd) {
//             close(err_pipe[0]);
//             if(dup2(err_pipe[1], 2) == -1) {
//                 perror("Error assigning stderr in child process");
//                 exit(-1);
//             }
//             close(err_pipe[1]);
//         }
//         execl("/bin/sh", "/bin/sh", "-c", command, (char *)NULL);
//         perror("Error executing command in child process");
//         exit(-1);
 
//     default:
// // Parent
//         break;
//     }
 
//     if(writefd) {
//         close(in_pipe[0]);
//         set_cloexec(in_pipe[1]);
//         *writefd = in_pipe[1];
//     }
//     if(readfd) {
//         close(out_pipe[1]);
//         set_cloexec(out_pipe[0]);
//         *readfd = out_pipe[0];
//     }
//     if(errfd) {
//         close(err_pipe[1]);
//         set_cloexec(out_pipe[0]);
//         *errfd = err_pipe[0];
//     }
 
//     return pid;
 
// error:
//     if(in_pipe[0] >= 0) {
//         close(in_pipe[0]);
//     }
//     if(in_pipe[1] >= 0) {
//         close(in_pipe[1]);
//     }
//     if(out_pipe[0] >= 0) {
//         close(out_pipe[0]);
//     }
//     if(out_pipe[1] >= 0) {
//         close(out_pipe[1]);
//     }
//     if(err_pipe[0] >= 0) {
//         close(err_pipe[0]);
//     }
//     if(err_pipe[1] >= 0) {
//         close(err_pipe[1]);
//     }
 
//     return -1;
// }
/*
 * Sets the FD_CLOEXEC flag. Returns 0 on success, -1 on error.
 */
namespace {
int set_cloexec(int fd) throw()
{
    if(fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) == -1) {
        DEF_LOGGING_PERROR("Error setting FD_CLOEXEC flag");
        return -1;
    }
 
    return 0;
}
}
 
/*
 * Runs command in another process, with write-only interaction capabilities.
 * Writing to *writefd will write to the command's stdin. Returns the child
 * PID on success, -1 on error.
 */
pid_t LOFAR_Station_Beamformed_Writer_LuMP2::
pipe_writer_popen(char *command, char** argv, int child_stdout_fd, int *writefd) throw()
{
    int in_pipe[2] = {-1, -1};
    pid_t pid;
 
    if(command == NULL) {
        DEF_LOGGING_ERROR("Cannot popen() a NULL command.\n");
        goto error;
    }
    if(argv == NULL) {
        DEF_LOGGING_ERROR("Cannot popen() a command with NULL argument pointer.\n");
        goto error;
    }
    if(writefd == NULL) {
        DEF_LOGGING_ERROR("Will not popen() a command with NULL writefd pointer.\n");
        goto error;
    }
 
    if(pipe(in_pipe)) {
        DEF_LOGGING_PERROR("Error creating pipe for stdin");
        goto error;
    }
 
    pid = fork();
    switch(pid) {
    case -1:
// Error
        DEF_LOGGING_PERROR("Error creating child process");
        goto error;
 
    case 0:
// Child
        {
            close(in_pipe[1]);
            if(dup2(in_pipe[0], STDIN_FILENO) == -1) {
                DEF_LOGGING_PERROR("Error assigning stdin in child process");
                exit(-1);
            }
            close(in_pipe[0]);
            if(child_stdout_fd >= 0) {
                if(dup2(child_stdout_fd, STDOUT_FILENO) == -1) {
                    DEF_LOGGING_PERROR("Error assigning stdout in child process");
                    exit(-1);
                }
                if(dup2(child_stdout_fd, STDERR_FILENO) == -1) {
                    DEF_LOGGING_PERROR("Error assigning stderr in child process");
                    exit(-1);
                }
                close(child_stdout_fd);
            }
        }
        execvp(command, argv);
        DEF_LOGGING_PERROR("Error executing command in child process");
        exit(-1);
 
    default:
// Parent
        break;
    }
 
    {
        close(in_pipe[0]);
        set_cloexec(in_pipe[1]);
        *writefd = in_pipe[1];
    }
 
    return pid;
 
error:
    if(in_pipe[0] >= 0) {
        close(in_pipe[0]);
    }
    if(in_pipe[1] >= 0) {
        close(in_pipe[1]);
    }
 
    return -1;
}



}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


