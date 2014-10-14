// LOFAR_Station_Beamformed_Writer_Signals.cxx
// deal with quit signal from keyboard, for example
//_HIST  DATE NAME PLACE INFO
// 2011 Mar 29  James M Anderson  --MPIfR  Start
// 2012 Mar 18  JMA  --change to MPIfR::LOGGING error messages



// Copyright (c) 2011, 2014, James M. Anderson <anderson@mpifr-bonn.mpg.de>

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
#include <pthread.h>
#include <signal.h>
#include "LOFAR_Station_Beamformed_Writer_Signals.h"
#include "MPIfR_logging.h"



// set up a namespace area for stuff.
namespace MPIfR {

namespace LOFAR {

namespace Station {

namespace Signals {




// GLOBALS
const int_fast32_t USER_SIGNAL_SHUTDOWN = +1000;
int_fast32_t user_signal_found = USER_SIGNAL_NOTHING_HAPPENING;
pthread_mutex_t user_signal_found_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t user_signal_thread;
void *(*user_signal_handler_routine)(void*)=0;

// FUNCTIONS

namespace {
extern "C" void* signal_handler(void* arg)
{
    sigset_t signal_set;
    while(1) {
        // wait for all signals
        int signal=0;
        sigfillset(&signal_set);
        sigwait(&signal_set, &signal);
        switch(signal) {
        case SIGTERM:
            DEF_LOGGING_INFO("received SIGTERM\n");
            pthread_mutex_lock(&user_signal_found_mutex);
            user_signal_found = SIGTERM;
            pthread_mutex_unlock(&user_signal_found_mutex);
            break;
        case SIGINT:
            DEF_LOGGING_INFO("received SIGINT\n");
            pthread_mutex_lock(&user_signal_found_mutex);
            user_signal_found = SIGINT;
            pthread_mutex_unlock(&user_signal_found_mutex);
            break;
        case SIGUSR1:
            DEF_LOGGING_INFO("received SIGUSR1\n");
            pthread_mutex_lock(&user_signal_found_mutex);
            if(user_signal_found != USER_SIGNAL_SHUTDOWN) {
                // do nothing
                DEF_LOGGING_INFO("received SIGUSR1 but not SHUTDOWN CODE\n");
            }
            else {
                DEF_LOGGING_INFO("shutting down signal handling thread\n");
                pthread_mutex_unlock(&user_signal_found_mutex);
                goto thread_done;
            }
            pthread_mutex_unlock(&user_signal_found_mutex);
            break;
            
        default:
            DEF_LOGGING_INFO("received signal %d\n", signal);
            pthread_mutex_lock(&user_signal_found_mutex);
            if(user_signal_found < 0) {
                user_signal_found = 0;
            }
            pthread_mutex_unlock(&user_signal_found_mutex);
            break;
        }
    }
thread_done:
    pthread_exit(NULL);
    return (void*)0;
}
}

int_fast32_t setup_user_signal(void *(*handler_routine)(void*)) throw()
{
    user_signal_handler_routine = handler_routine;
    sigset_t signal_set;
    // block all signals
    sigfillset(&signal_set);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    // create the signal handling thread
    int retval =  pthread_create(&user_signal_thread, NULL, signal_handler, NULL);
    if((retval)) {
        DEF_LOGGING_PERROR("creating signal thread failed");
        DEF_LOGGING_CRITICAL("creating signal thread failed with %d\n", retval);
        exit(1);
    }

    return 0;
}



int_fast32_t shutdown_user_signal(void) throw()
{
    pthread_mutex_lock(&user_signal_found_mutex);
    user_signal_found = USER_SIGNAL_SHUTDOWN;
    pthread_mutex_unlock(&user_signal_found_mutex);
    pthread_kill(user_signal_thread,SIGUSR1);
    void* join_retval;
    int retval = pthread_join(user_signal_thread, &join_retval);
    if((retval)) {
        DEF_LOGGING_PERROR("joining signal thread failed");
        DEF_LOGGING_CRITICAL("joining signal thread failed with %d\n", retval);
        exit(1);
    }
    if(join_retval != NULL) {
        DEF_LOGGING_WARNING("signal thread returned %p\n", join_retval);
    }
    return 0;
}



// // somwehere in your user code, you should do:
// pthread_mutex_lock(&MPIfR::LOFAR::Station::Signals::user_signal_found_mutex);
// switch(MPIfR::LOFAR::Station::Signals::user_signal_found) {
// case USER_SIGNAL_NOTHING_HAPPENING
//     // do nothing
//     break;
// case 0:
//     do_something;
//     break;
// case SIGTERM:
// case SIGINT:
//     if(MPIfR::LOFAR::Station::Signals::user_signal_handler_routine != 0) {
//         MPIfR::LOFAR::Station::Signals::user_signal_handler_routine(NULL);
//     }
//     break;
// default:
//     DEF_LOGGING_WARNING("Unrecognized signal %d\n", int(MPIfR::LOFAR::Station::Signals::user_signal_found));
// }
// pthread_mutex_unlock(&MPIfR::LOFAR::Station::Signals::user_signal_found_mutex);



}  // end namespace Signals
}  // end namespace Station
}  // end namespace LOFAR
}  // end namespace MPIfR


