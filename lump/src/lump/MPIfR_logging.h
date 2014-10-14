// MPIfR_logging.h
// make a logging class with properties similar to Python's logging
//_HIST  DATE NAME PLACE INFO
// 2012 Mar 17  James M Anderson  --MPIfR  Start
// 2013 Aug 03  JMA  ---- make all error message codes the same length
// 2013 Aug 03  JMA  ---- use mutexes to prevent writing error messages from
//                        different threads simultaneously
// 2014 May 04  JMA  ---- modify perror to pass errno value
// 2014 Sep 17  JMA  --- add gcc printf attribute to log function

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



#ifndef MPIfR_logging_H
#define MPIfR_logging_H

// INCLUDES
#define __STDC_LIMIT_MACROS
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
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>




#ifdef MPIfR_logging_H_FILE
#  define NUM(x) =x;
#  define EXTERNNUM
#else
#  define NUM(x)
#  define EXTERNNUM extern
#endif








// set up a namespace area for stuff.
namespace MPIfR {
namespace LOGGING {

extern const char PROGRAM_NAME[];
extern const char PROGRAM_VERSION[];
extern const char PROGRAM_DATE[];
extern const char PROGRAM_TIME[];


#define MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Array \
    LOFAR_ENUM_VALS(     MPIfR_LOGGING_MESSAGE_TYPE_NOTSET  ,  0, "NOTSET  : ", "\033[7;34m") \
        LOFAR_ENUM_VALS( MPIfR_LOGGING_MESSAGE_TYPE_DEBUG   , 10, "DEBUG   : ", "\033[1;34m") \
        LOFAR_ENUM_VALS( MPIfR_LOGGING_MESSAGE_TYPE_INFO    , 20, "INFO    : ", "\033[00m") \
        LOFAR_ENUM_VALS( MPIfR_LOGGING_MESSAGE_TYPE_WARNING , 30, "WARNING : ", "\033[1;31m") \
        LOFAR_ENUM_VALS( MPIfR_LOGGING_MESSAGE_TYPE_ERROR   , 40, "ERROR   : ", "\033[7;31m") \
        LOFAR_ENUM_VALS( MPIfR_LOGGING_MESSAGE_TYPE_CRITICAL, 50, "CRITICAL: ", "\033[7;31m") 



enum MPIfR_LOGGING_MESSAGE_TYPE_ENUM {
#define LOFAR_ENUM_VALS(Enum, Val, String, Color) Enum = Val,
    MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Array
#undef LOFAR_ENUM_VALS
};
const char* const restrict MPIfR_LOGGING_MESSAGE_TYPE_ENUM_Str(MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) throw();
const char* const restrict MPIfR_LOGGING_MESSAGE_TYPE_COLORS_ENUM_Str(MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) throw();









//_CLASS  Logging
class Logging {
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

    
    Logging() throw() : fp_disk(NULL), fp_display(NULL), MIN_DISK_LEVEL(MPIfR_LOGGING_MESSAGE_TYPE_ENUM::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET), MIN_DISPLAY_LEVEL(MPIfR_LOGGING_MESSAGE_TYPE_ENUM::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET) {
        int retval = pthread_mutex_init(&logging_mutex, NULL);
        if((retval)) {
            std::perror("initializing logging_mutex failed");
        }
    };
    Logging(const char * const restrict filename,
            const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level) throw();
    Logging(FILE * const restrict fp,
            const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level) throw();
    Logging(const char * const restrict filename,
            const MPIfR_LOGGING_MESSAGE_TYPE_ENUM  min_disk_level,
            FILE * const restrict fp_display,
            const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_display_level) throw();
    ~Logging() throw();



    int log(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM level,
            const char* const restrict program_name,
            const char* const restrict FILE, const int LINE,
            const char* const restrict format, ...) throw() JMA_FUNCTION_IS_FORMAT(printf,6,7);
    int perror(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM level,
               const char* const restrict program_name,
               const char* const restrict FILE, const int LINE,
               const int errnum) throw();


    void reset_disk(const char * const restrict filename,
                    const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level) throw();
    void reset_display(FILE * const restrict fp,
                       const MPIfR_LOGGING_MESSAGE_TYPE_ENUM min_level) throw();
    


    bool fp_disk_valid() const throw() {return (fp_disk != NULL);}
    bool fp_display_valid() const throw() {return (fp_display != NULL);}
        
protected:
    


private:
    FILE* restrict fp_disk;
    FILE* restrict fp_display;
    MPIfR_LOGGING_MESSAGE_TYPE_ENUM MIN_DISK_LEVEL;
    MPIfR_LOGGING_MESSAGE_TYPE_ENUM MIN_DISPLAY_LEVEL;


    bool no_message(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) const throw()
    {return (((fp_disk==NULL) || (e < MIN_DISK_LEVEL))
            && ((fp_display==NULL) || (e < MIN_DISPLAY_LEVEL)));}
    bool will_do_disk(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) const throw()
    {return ((fp_disk!=NULL) && (e >= MIN_DISK_LEVEL));}
    bool will_do_display(const MPIfR_LOGGING_MESSAGE_TYPE_ENUM e) const throw()
    {return ((fp_display!=NULL) && (e >= MIN_DISPLAY_LEVEL));}

    pthread_mutex_t logging_mutex;
};


// CLASS FUNCTIONS

#ifdef LOGGING_ERROR
#  error "Some other code has set LOGGING_ERROR --- fix these defines"
#endif
#define LOGGING_NOTSET(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_NOTSET,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_DEBUG(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_DEBUG,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_INFO(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_INFO,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_WARNING(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_WARNING,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_ERROR(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_ERROR,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_CRITICAL(logger, ...) \
    logger.log(MPIfR_LOGGING_MESSAGE_TYPE_CRITICAL,       \
               PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define LOGGING_PERROR(logger, ...) \
        logger.perror(MPIfR_LOGGING_MESSAGE_TYPE_ERROR,                    \
                      PROGRAM_NAME, __FILE__, __LINE__, errno);               \
        logger.log(MPIfR_LOGGING_MESSAGE_TYPE_ERROR,              \
                   PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__);





EXTERNNUM Logging LOGGING_BLANK_ NUM(Logging(stderr,MPIfR_LOGGING_MESSAGE_TYPE_NOTSET));

#ifndef DEBUG
#define DEF_LOGGING_NOTSET(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET, \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_DEBUG(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_DEBUG,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_INFO(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_INFO,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_WARNING(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_WARNING,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_ERROR(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_CRITICAL(...) \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_CRITICAL,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_PERROR(...) \
        MPIfR::LOGGING::LOGGING_BLANK_.perror(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR,                    \
                                              MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, errno); \
        MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR,              \
                                           MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__);
#else // DEBUG
#define DEF_LOGGING_NOTSET(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_NOTSET,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_DEBUG(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_DEBUG,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_INFO(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_INFO,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_WARNING(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_WARNING,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_ERROR(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_CRITICAL(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_CRITICAL,       \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__)
#define DEF_LOGGING_PERROR(...) \
    fprintf(stderr, __VA_ARGS__); \
    MPIfR::LOGGING::LOGGING_BLANK_.perror(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR, \
                                          MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, errno); \
    MPIfR::LOGGING::LOGGING_BLANK_.log(MPIfR::LOGGING::MPIfR_LOGGING_MESSAGE_TYPE_ERROR, \
                                       MPIfR::LOGGING::PROGRAM_NAME, __FILE__, __LINE__, __VA_ARGS__);
#endif // DEBUG

// HELPER FUNCTIONS


}  // end namespace LOGGING
}  // end namespace MPIfR


#undef NUM
#undef EXTERNNUM
#endif // MPIfR_logging_H
