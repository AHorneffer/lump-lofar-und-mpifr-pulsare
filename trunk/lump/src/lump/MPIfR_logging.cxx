// MPIfR_logging.cxx
// make a logging class with properties similar to Python's logging
//_HIST  DATE NAME PLACE INFO
// 2012 Mar 17  James M Anderson  --MPIfR  Start
// 2013 Aug 03  JMA  ---- put message level first on disk output, use colons
//                   to separate prelim parts instead of spaces
// 2013 Aug 03  JMA  ---- use mutexes to prevent writing error messages from
//                        different threads simultaneously
// 2014 May 04  JMA  ---- modify perror to pass errno value



// Copyright (c) 2012, 2013, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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



// INCLUDES
#define MPIfR_logging_H_FILE 1
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
#include <string.h>
#include <error.h>
#include <time.h>
#include "MPIfR_logging.h"
#include <pthread.h>



// set up a namespace area for stuff.
namespace MPIfR {
namespace LOGGING {




// GLOBALS


// FUNCTIONS



Logging::
Logging(const char * const restrict filename,
        const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level)
    throw()
        : fp_display(NULL),
          MIN_DISK_LEVEL(min_level),
          MIN_DISPLAY_LEVEL(MPIfR_LOGGING_MESSAGE_TYPE_NOTSET)
{
    fp_disk = fopen(filename, "a");
    if(fp_disk == NULL) {
        fprintf(stderr, "Error: could not open file '%s' for logging\n", filename);
    }
    // according to the C standard, the C library *may* use any
    // size it likes for allocating its own buffer, but in case it does look,
    // give it something reasonable to try to use.
    int retval = setvbuf(fp_disk, NULL, _IOLBF, 2048);
    if((retval)) {
        std::perror("Unable to set line buffering");
    }
    retval = pthread_mutex_init(&logging_mutex, NULL);
    if((retval)) {
        std::perror("initializing logging_mutex failed");
    }
    return;
}

Logging::
Logging(FILE * const restrict fp,
        const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level)
    throw()
        : fp_disk(NULL),
          fp_display(fp),
          MIN_DISK_LEVEL(MPIfR_LOGGING_MESSAGE_TYPE_NOTSET),
          MIN_DISPLAY_LEVEL(min_level)
{
    int retval = pthread_mutex_init(&logging_mutex, NULL);
    if((retval)) {
        std::perror("initializing logging_mutex failed");
    }
    return;
}


Logging::
Logging(const char * const restrict filename,
        const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_disk_level,
        FILE * const restrict fp_display_,
        const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_display_level)
    throw()
        : fp_display(fp_display_),
          MIN_DISK_LEVEL(min_disk_level),
          MIN_DISPLAY_LEVEL(min_display_level)
{
    fp_disk = fopen(filename, "a");
    if(fp_disk == NULL) {
        fprintf(stderr, "Error: could not open file '%s' for logging\n", filename);
    }
    // according to the C standard, the C library *may* use any
    // size it likes for allocating its own buffer, but in case it does look,
    // give it something reasonable to try to use.
    int retval = setvbuf(fp_disk, NULL, _IOLBF, 2048);
    if((retval)) {
        std::perror("Unable to set line buffering");
    }
    retval = pthread_mutex_init(&logging_mutex, NULL);
    if((retval)) {
        std::perror("initializing logging_mutex failed");
    }
    return;
} 

Logging::
~Logging()
    throw()
{
    if((fp_disk)) {
        fclose(fp_disk);fp_disk=NULL;
    }
    // caller is responsible for closing fp_display
    pthread_mutex_destroy(&logging_mutex);
    return;
}



void Logging::
reset_disk(const char * const restrict filename,
           const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level)
    throw()
{
    if((fp_disk)) {
        fclose(fp_disk);fp_disk=NULL;
    }
    fp_disk = fopen(filename, "a");
    if(fp_disk == NULL) {
        fprintf(stderr, "Error: could not open file '%s' for logging\n", filename);
    }
    // according to the C standard, the C library *may* use any
    // size it likes for allocating its own buffer, but in case it does look,
    // give it something reasonable to try to use.
    int retval = setvbuf(fp_disk, NULL, _IOLBF, 2048);
    if((retval)) {
        std::perror("Unable to set line buffering");
    }
    MIN_DISK_LEVEL = min_level;
    return;
}

void Logging::
reset_display(FILE * const restrict fp,
              const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level)
    throw()
{
    fp_display = fp;
    MIN_DISPLAY_LEVEL = min_level;
    return;
}








int Logging::
log(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM level,
    const char* const restrict program_name,
    const char* const restrict FILE, const int LINE,
    const char* const restrict format, ...) throw()
{
    int return_code = 0;
    if(no_message(level)) {
        return return_code;
    }
    {
        int retval = pthread_mutex_lock(&logging_mutex);
        if((retval)) {
            std::perror("locking logging_mutex failed");
        }
    }
    const int SIZE = 1024;
    char msg[SIZE];
    char prelim[SIZE];
    char* msg_p = msg;
    char* prelim_p = prelim;
    if(will_do_disk(level)) {
        prelim[0] = 0;
        struct timespec local_time;
        struct tm time_s;
        struct tm* tmp;
    
        int retval = clock_gettime(CLOCK_REALTIME,&local_time);
        if((retval)) {
            goto prelim_error;
        }
        tmp = gmtime_r(&(local_time.tv_sec), &time_s);
        if(tmp == NULL) {
            goto prelim_error;
        }
        retval = snprintf(prelim, SIZE, "%04d-%02d-%02dT%02d:%02d:%02d.%09ld:%s:%s:%d: ",
                          time_s.tm_year+1900,
                          time_s.tm_mon+1,
                          time_s.tm_mday,
                          time_s.tm_hour,
                          time_s.tm_min,
                          time_s.tm_sec,
                          local_time.tv_nsec,
                          program_name,
                          FILE,
                          LINE
                          );
        if(retval >= SIZE) {
            const int NEW_SIZE = retval+2;
            prelim_p = reinterpret_cast<char*>(malloc(NEW_SIZE));
            if((prelim_p)) {
                retval = snprintf(prelim_p, SIZE, "%04d-%02d-%02dT%02d:%02d:%02d.%09ld:%s:%s:%d: ",
                                  time_s.tm_year+1900,
                                  time_s.tm_mon+1,
                                  time_s.tm_mday,
                                  time_s.tm_hour,
                                  time_s.tm_min,
                                  time_s.tm_sec,
                                  local_time.tv_nsec,
                                  program_name,
                                  FILE,
                                  LINE
                                  );
                if(retval >= NEW_SIZE) {
                    return_code = -3;
                }
            }
            else {
                return_code = -3;
            }
        }
        else if(retval < 0) {
            return_code = -4;
        }
prelim_error:
        if(prelim_p[0] == 0) {
            strcpy(prelim, "Failed logging prelim filling: ");
        }
    }
    bool need_newline = false;
    {
        msg[0] = 0;
        va_list ap;
        va_start(ap, format);
        int retval = vsnprintf(msg, SIZE, format, ap);
        va_end(ap);
        if(retval >= SIZE) {
            const int NEW_SIZE = retval+2;
            msg_p = reinterpret_cast<char*>(malloc(NEW_SIZE));
            if((msg_p)) {
                va_start(ap, format);
                retval = vsnprintf(msg_p, SIZE, format, ap);
                va_end(ap);
                if(retval >= NEW_SIZE) {
                    return_code = -1;
                }
            }
            else {
                return_code = -1;
            }
        }
        else if(retval < 0) {
            return_code = -2;
        }
        size_t pos = strlen(msg_p);
        if(pos > 0) {
            if(msg_p[pos-1] == '\n') {}
            else {
                need_newline = true;
            }
        }
        else {
            need_newline = true;
        }
    }
    const char* const restrict level_message = MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Str(level);
    // Dump out to the disk file if possible
    if(will_do_disk(level)) {
        int retval = fputs(level_message, fp_disk);
        if(retval < 0) {
            return_code = -5;
        }
        retval = fputs(prelim_p, fp_disk);
        if(retval < 0) {
            return_code = -6;
        }
        retval = fputs(msg_p, fp_disk);
        if(retval < 0) {
            return_code = -7;
        }
        if(need_newline) {
            retval = fputc('\n', fp_disk);
            if(retval < 0) {
                return_code = -8;
            }
        }
    }
    // Dump out to the display if possible
    if(will_do_display(level)) {
        int retval = fputs(MPIfR_LOGGING_MESSAGE_TYPE_COLORS_ENUM_Str(level), fp_display);
        if(retval < 0) {
            return_code = -9;
        }
        retval = fputs(level_message, fp_display);
        if(retval < 0) {
            return_code = -10;
        }
        retval = fputs(msg_p, fp_display);
        if(retval < 0) {
            return_code = -11;
        }
        retval = fputs("\033[00m", fp_display); // clear the color command
        if(retval < 0) {
            return_code = -12;
        }
        if(need_newline) {
            retval = fputc('\n', fp_display);
            if(retval < 0) {
                return_code = -13;
            }
        }
    }
    if(prelim_p != prelim) {
        free(prelim_p); prelim_p=NULL;
    }
    if(msg_p != msg) {
        free(msg_p); msg_p=NULL;
    }
    {
        int retval = pthread_mutex_unlock(&logging_mutex);
        if((retval)) {
            std::perror("unlocking logging_mutex failed");
        }
    }
    return return_code;
}


int Logging::
perror(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM level,
       const char* const restrict program_name,
       const char* const restrict FILE, const int LINE,
       const int errnum) throw()
{
    const size_t PERROR_SIZE=128;
    char perror_buf[PERROR_SIZE];
    perror_buf[0] = 0;
    strerror_r(errnum, perror_buf, PERROR_SIZE);
    return log(level, program_name, FILE, LINE, "call failed with errno=%d, message='%s'", errnum, perror_buf);
}
    




const char* const restrict MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Str(MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String, Color) case Enum: return String;
        MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Array
#undef LOFAR_ENUM_VALS
    default:
        return "Unknown MPIfR_LOGGING_MESSAGE_TYPE_ENUM";
    }
    return NULL;
}




const char* const restrict MPIfR_LOGGING_MESSAGE_TYPE_COLORS_ENUM_Str(MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) throw()
{
    switch(e) {
#define LOFAR_ENUM_VALS(Enum, Val, String, Color) case Enum: return Color;
        MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Array
#undef LOFAR_ENUM_VALS
    default:
        return "\033[00m";
    }
    return NULL;
}


}  // end namespace LOGGING
}  // end namespace MPIfR


