// MPIfR_FFTW3.cxx
// routines for dealing with FFTW3 in *SINGLE* threaded mode
//_HIST  DATE NAME PLACE INFO
// 2011 Jan 29  James M Anderson  --MPIfR  Start
// 2013 Feb 12  JMA  ---- use hostname in wisdom files for NSF mounted homes
// 2013 Aug 03  JMA  ---- use defines to avoid __FILE__,__LINE__ visibility
// 2013 Sep 06  JMA  ---- add init and shutdown functions
// 2014 Sep 17  JMA  --- add file locking to the wisdom files
//                       see http://beej.us/guide/bgipc/output/html/multipage/flocking.html
//                       for code source guide
// 2014 Sep 20  JMA  --- wrap gethostname call in ifdef


// Copyright (c) 2012, 2013, 2014, James M. Anderson <anderson@gfz-potsdam.de>

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
#define MPIfR_FFTW3_H_FILE 1
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
#include <stdint.h>
// we want to use ISO C9X stuff
// we want to use some GNU stuff
// But this sometimes breaks time.h
#include <time.h>

#include "JMA_math.h"
#include <stdio.h>
#include <stdlib.h>
#include "fftw3.h"
#include <pthread.h>
#include "MPIfR_FFTW3.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include "MPIfR_logging.h"






namespace {

bool WISDOM_loaded = false;
bool WISDOM_exported = false;
bool MPIfR_FFTW3_initted = false;

void myncat(const char * const restrict src0,
            const char * const restrict src1,
            const char * const restrict src2,
            char * const restrict dest,
            const size_t n)
{
    if(n<=0) {
        DEF_LOGGING_CRITICAL("destination too small (%ld bytes) to hold '%s', '%s' and '%s'", long(n), src0, src1, src2);
        exit(1);
    }
    dest[0] = 0;
    strncpy(dest, src0, n);
    dest[n-1] = 0;
    size_t l = strlen(dest);
    strncpy(dest+l,src1,n-l);
    dest[n-1] = 0;
    l = strlen(dest);
    strncpy(dest+l,src2,n-l);
    dest[n-1] = 0;
    return;
}

// &*$! GCC and GLIBC think ignoring return on strtol
// is a required warning.  Casting to (void) no longer
// works to get the compiler to shut up.
template<class T> inline T ignore_return(T i) {return i;}
}



// set up a namespace area for stuff.
namespace MPIfR {

namespace Numerical {

namespace FFTW3 {



// GLOBALS


// FUNCTIONS
void load_standard_wisdom() throw()
{
    const int PATHSIZE = 2048;
    char filename[PATHSIZE];
    char hostname[HOST_NAME_MAX+1];
    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    if(WISDOM_loaded) {
        MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
        return;
    }
#if (defined _BSD_SOURCE) || ((defined _XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)) || ((defined _POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L))
    {
        errno = 0;
        int retval = gethostname(hostname, HOST_NAME_MAX+1);
        hostname[HOST_NAME_MAX] = 0;
        if((retval)) {
            DEF_LOGGING_WARNING("gethostname call failed with code %d\n", errno);
            strncpy(hostname, "none", HOST_NAME_MAX);
            hostname[HOST_NAME_MAX] = 0;
        }
    }
#else
    {
        strncpy(hostname, "none", HOST_NAME_MAX);
        hostname[HOST_NAME_MAX] = 0;
    }
#endif
    // Stuff for file locking.  Set up the basic file locking stuff.
    struct flock file_lock;
    int fd;
    int unix_retval;
    file_lock.l_whence = SEEK_SET;
    file_lock.l_start  = 0;
    file_lock.l_len    = 0;
    file_lock.l_pid    = getpid();
    
    // Try getting wisdom from the standard LuMP location
    const char* const restrict LuMP_ROOT = getenv(MPIfR_LOFAR_LuMP_ROOT);
    if((LuMP_ROOT)) {
        int retval = 0;
        // main directory is present.  Try getting the wisdom data
        myncat(LuMP_ROOT, "/" MPIfR_LOFAR_LIBDATA_DIRECTORY MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomf.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("LuMP_ROOT wisdomf file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomf file '%s' could not be locked --- bad things may happen", filename);
            }
            retval = fftwf_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomf file '%s' had error on import\n", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomf file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomf file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("LuMP_ROOT wisdomf file '%s' not found\n", filename);
        }
        myncat(LuMP_ROOT, "/" MPIfR_LOFAR_LIBDATA_DIRECTORY MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdom.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("LuMP_ROOT wisdom file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdom file '%s' could not be locked --- bad things may happen", filename);
            }
            retval = fftw_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("LuMP_ROOT wisdom file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdom file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdom file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("LuMP_ROOT wisdom file '%s' not found\n", filename);
        }
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        myncat(LuMP_ROOT, "/" MPIfR_LOFAR_LIBDATA_DIRECTORY MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdoml.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("LuMP_ROOT wisdoml file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdoml file '%s' could not be locked --- bad things may happen", filename);
            }
            retval = fftwl_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("LuMP_ROOT wisdoml file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdoml file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdoml file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("LuMP_ROOT wisdoml file '%s' not found\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        myncat(LuMP_ROOT, "/" MPIfR_LOFAR_LIBDATA_DIRECTORY MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomq.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("LuMP_ROOT wisdomq file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomq file '%s' could not be locked --- bad things may happen", filename);
            }
            retval = fftwq_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomq file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomq file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("LuMP_ROOT wisdomq file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("LuMP_ROOT wisdomq file '%s' not found\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    }
    else {
        DEF_LOGGING_WARNING("LuMP_ROOT environment variable not found\n");
    }
    // get any wisdom from the user home area
    const char* const restrict HOME = getenv("HOME");
    if((HOME)) {
        int retval = 0;
        // HOME directory is present.  Try getting the wisdom data
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomf.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdomf file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be locked for reading --- bad things may happen", filename);
            }
            retval = fftwf_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            // DEF_LOGGING_PERROR("open");
            // DEF_LOGGING_WARNING("HOME wisdomf file '%s' not found\n", filename);
        }
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdom.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdom file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be locked for reading --- bad things may happen", filename);
            }
            retval = fftw_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("HOME wisdom file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            // DEF_LOGGING_PERROR("open");
            // DEF_LOGGING_WARNING("HOME wisdom file '%s' not found\n", filename);
        }
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdoml.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdoml file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be locked for reading --- bad things may happen", filename);
            }
            retval = fftwl_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            // DEF_LOGGING_PERROR("open");
            // DEF_LOGGING_WARNING("HOME wisdoml file '%s' not found\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomq.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDONLY);
        if(fd >= 0) {
            file_lock.l_type   = F_RDLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdomq file '%s' currently locked --- trying to get read lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be locked for reading --- bad things may happen", filename);
            }
            retval = fftwq_import_wisdom_from_filename(filename);
            if(!retval) {
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' had error on import", filename);
            }
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            // DEF_LOGGING_PERROR("open");
            // DEF_LOGGING_WARNING("HOME wisdomq file '%s' not found\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    }
    else {
        DEF_LOGGING_WARNING("HOME environment variable not found\n");
    }
    WISDOM_loaded = true;
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}





void store_current_wisdom() throw()
{
    const int PATHSIZE = 2048;
    char filename[PATHSIZE];
    char hostname[HOST_NAME_MAX+1];
    MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
    if(WISDOM_exported) {
        MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
        return;
    }
#if (defined _BSD_SOURCE) || ((defined _XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)) || ((defined _POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L))
    {
        errno = 0;
        int retval = gethostname(hostname, HOST_NAME_MAX+1);
        hostname[HOST_NAME_MAX] = 0;
        if((retval)) {
            DEF_LOGGING_WARNING("gethostname call failed with code %d\n", errno);
            strncpy(hostname, "none", HOST_NAME_MAX);
            hostname[HOST_NAME_MAX] = 0;
        }
    }
#else
    {
        strncpy(hostname, "none", HOST_NAME_MAX);
        hostname[HOST_NAME_MAX] = 0;
    }
#endif
    // Stuff for file locking.  Set up the basic file locking stuff.
    struct flock file_lock;
    int fd;
    int unix_retval;
    char* w;
    size_t len;
    ssize_t wr;
    size_t total_written;
    file_lock.l_whence = SEEK_SET;
    file_lock.l_start  = 0;
    file_lock.l_len    = 0;
    file_lock.l_pid    = getpid();
    
    // get any wisdom from the user
    const char* const restrict HOME = getenv("HOME");
    if((HOME)) {
        int retval = 0;
        struct stat st;
        // main directory is present.  Try getting the wisdom data
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY, "", filename, PATHSIZE);
        if(stat(filename,&st) == 0) {
        }
        else {
            retval = mkdir(filename, 0777);
            if((retval)) {
                DEF_LOGGING_PERROR("mkdir $HOME/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY);
                DEF_LOGGING_ERROR("making '%s' directory\n", filename);
                goto store_current_wisdom_exit;
            }
        }
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory, "", filename, PATHSIZE);
        if(stat(filename,&st) == 0) {
        }
        else {
            retval = mkdir(filename, S_IRWXU|S_IRWXG|S_IROTH);
            if((retval)) {
                DEF_LOGGING_PERROR("mkdir $HOME/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory);
                goto store_current_wisdom_exit;
            }
        }
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomf.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd >= 0) {
            file_lock.l_type   = F_WRLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdomf file '%s' currently locked --- trying to get write lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be locked for writing --- bad things may happen", filename);
            }
            w = fftwf_export_wisdom_to_string();
            if(w == NULL) {
                DEF_LOGGING_ERROR("wisdomf export to string failed");
            }
            else {
                len = strlen(w)+1;
                total_written = 0;
                while(total_written < len) {
                    wr = write(fd, w+total_written, len-total_written);
                    if(wr < 0) {
                        DEF_LOGGING_PERROR("write");
                        DEF_LOGGING_ERROR("write to wisdomf file '%s' failed --- bad things may happen", filename);
                    }
                    total_written += wr;
                }
            }
            free(w);
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("HOME wisdomf file '%s' could not be opened for writing (returned %d)\n", filename, fd);
        }
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdom.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd >= 0) {
            file_lock.l_type   = F_WRLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdom file '%s' currently locked --- trying to get write lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be locked for writing --- bad things may happen", filename);
            }
            w = fftw_export_wisdom_to_string();
            if(w == NULL) {
                DEF_LOGGING_ERROR("wisdom export to string failed");
            }
            else {
                len = strlen(w)+1;
                total_written = 0;
                while(total_written < len) {
                    wr = write(fd, w+total_written, len-total_written);
                    if(wr < 0) {
                        DEF_LOGGING_PERROR("write");
                        DEF_LOGGING_ERROR("write to wisdom file '%s' failed --- bad things may happen", filename);
                    }
                    total_written += wr;
                }
            }
            free(w);
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("HOME wisdom file '%s' could not be opened for writing\n", filename);
        }
#ifdef JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdoml.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd >= 0) {
            file_lock.l_type   = F_WRLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdoml file '%s' currently locked --- trying to get write lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be locked for writing --- bad things may happen", filename);
            }
            w = fftwl_export_wisdom_to_string();
            if(w == NULL) {
                DEF_LOGGING_ERROR("wisdoml export to string failed");
            }
            else {
                len = strlen(w)+1;
                total_written = 0;
                while(total_written < len) {
                    wr = write(fd, w+total_written, len-total_written);
                    if(wr < 0) {
                        DEF_LOGGING_PERROR("write");
                        DEF_LOGGING_ERROR("write to wisdoml file '%s' failed --- bad things may happen", filename);
                    }
                    total_written += wr;
                }
            }
            free(w);
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("HOME wisdoml file '%s' could not be opened for writing\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_80_BIT_FLOAT
#ifdef JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
        myncat(HOME, "/" MPIfR_LOFAR_LuMP_HOME_USER_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_Storage_Directory "wisdomq.", hostname, filename, PATHSIZE);
        fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd >= 0) {
            file_lock.l_type   = F_WRLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_INFO("HOME wisdomq file '%s' currently locked --- trying to get write lock", filename);
                unix_retval = fcntl(fd, F_SETLKW, &file_lock);
            }
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be locked for writing --- bad things may happen", filename);
            }
            w = fftwq_export_wisdom_to_string();
            if(w == NULL) {
                DEF_LOGGING_ERROR("wisdomq export to string failed");
            }
            else {
                len = strlen(w)+1;
                total_written = 0;
                while(total_written < len) {
                    wr = write(fd, w+total_written, len-total_written);
                    if(wr < 0) {
                        DEF_LOGGING_PERROR("write");
                        DEF_LOGGING_ERROR("write to wisdomq file '%s' failed --- bad things may happen", filename);
                    }
                    total_written += wr;
                }
            }
            free(w);
            file_lock.l_type   = F_UNLCK;
            unix_retval = fcntl(fd, F_SETLK, &file_lock);
            if(unix_retval == -1) {
                DEF_LOGGING_PERROR("fnctl");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be unlocked --- bad things may happen", filename);
            }
            fd = close(fd);
            if(fd == -1) {
                DEF_LOGGING_PERROR("close");
                DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be closed --- bad things may happen", filename);
            }
        }
        else {
            DEF_LOGGING_PERROR("open");
            DEF_LOGGING_WARNING("HOME wisdomq file '%s' could not be opened for writing\n", filename);
        }
#endif // JMA_CODE_HAVE_NATIVE_128_BIT_FLOAT
    }
    else {
        DEF_LOGGING_WARNING("HOME environment variable not found\n");
    }
store_current_wisdom_exit:
    WISDOM_exported = true;
    MPIFR_NUMERICAL_FFTW3_UNLOCK_ACCESS_MUTEX_DEF();
    return;
}







int_fast32_t read_channel_list(int* const restrict NUM_LIST,
                               int* restrict * const restrict channels)
{
    const int PATHSIZE = 2048;
    char filename[PATHSIZE];
    // Try getting channel file from the standard LuMP location
    const char* const restrict LuMP_ROOT = getenv(MPIfR_LOFAR_LuMP_ROOT);
    if((LuMP_ROOT)) {
        // main directory is present.  Try getting the wisdom data
        myncat(LuMP_ROOT, "/" MPIfR_LOFAR_LIBDATA_DIRECTORY MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY MPIfR_Numerical_FFTW3_LuMP_Channel_Number_File, "", filename, PATHSIZE);
    }
    else {
        DEF_LOGGING_WARNING("LuMP_ROOT environment variable not found\n");
        DEF_LOGGING_CRITICAL("No default channel information read\n");
        exit(2);
    }


    
    char* restrict text = NULL;
    int count = 0;
    char* ptr = NULL;
    char* endptr = NULL;
    int_fast32_t num_bytes = 0;
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
        DEF_LOGGING_CRITICAL("opening default number channels file '%s' failed\n", filename);
        exit(1);
    }
    int retval = fseek(fp, 0, SEEK_END);
    if((retval)) goto file_operation_error;
    num_bytes = ftell(fp);
    rewind(fp);

    text = (char* restrict)(malloc(num_bytes+1));
    if(!text) goto memory_error;

    if(fread(text, num_bytes, 1, fp) != 1) goto file_operation_error;

    text[num_bytes] = 0;

    // convert all non-numbers to spaces
    for(int_fast32_t i=0; i < num_bytes; i++) {
        if(!isdigit(text[i])) {
            text[i] = ' ';
        }
    }

    count = 0;
    ptr = text;
    endptr = NULL;

    ignore_return(strtol(ptr, &endptr, 10));
    while(endptr != ptr) {
        count++;
        ptr = endptr;
        ignore_return(strtol(ptr, &endptr, 10));
    }

    if(channels == 0) {
        DEF_LOGGING_ERROR("no valid number of channels list found\n");
        goto file_operation_error;
    }
    *NUM_LIST = count;

    *channels = (int* const restrict)(malloc(sizeof(int)*count));
    if((!*channels)) goto memory_error;

    ptr = text;
    endptr = NULL;
    count = 0;
    
    channels[0][count++] = int(strtol(ptr, &endptr, 10));
    while(endptr != ptr) {
        ptr = endptr;
        channels[0][count++] = int(strtol(ptr, &endptr, 10));
    }
    fclose(fp);
    free(text); text = NULL;
    return 0;


file_operation_error:
    DEF_LOGGING_CRITICAL("something wrong with default number channels file '%s'\n", filename);
    fclose(fp);
    free(text); text = NULL;
    exit(1);
    
memory_error:
    DEF_LOGGING_CRITICAL("memory allocation error\n");
    fclose(fp);
    exit(2);
    return -1;
}





int_fast32_t init_MPIfR_FFTW3(void) throw()
{
    if(!MPIfR_FFTW3_initted) {
        int retval = pthread_mutex_init(&MPIfR_Numerical_FFTW3_access_mutex, NULL);
        if((retval)) {
            DEF_LOGGING_PERROR("initializing MPIfR_Numerical_FFTW3_access_mutex failed");
            DEF_LOGGING_CRITICAL("initializing MPIfR_Numerical_FFTW3_access_mutex failed with %d\n", retval);
            exit(1);
        }
    }
    else {
        return +1;
    }
    MPIfR_FFTW3_initted = true;
    return 0;
}

int_fast32_t shutdown_MPIfR_FFTW3(void) throw()
{
    if(MPIfR_FFTW3_initted) {
        MPIFR_NUMERICAL_FFTW3_LOCK_ACCESS_MUTEX_DEF();
        pthread_mutex_destroy(&MPIfR_Numerical_FFTW3_access_mutex);
    }
    else {
        return +1;
    }
    MPIfR_FFTW3_initted = false;
    return 0;
}




}  // end namespace FFTW3
}  // end namespace Numerical
}  // end namespace MPIfR


