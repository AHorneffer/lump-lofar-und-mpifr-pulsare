// test_dog.cxx
// write stdin to a file
// 2013 Aug 30  James M Anderson  --- MPIfR start
// 2013 Sep 04  JMA  ---- expand to mode of dog (opposite of cat) program

// Copyright 2013,2014,  James M. Anderson <anderson@gfz-potsdam.de>

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

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Error: correct usage is %s filename0 [filename1...]", argv[0]);
        exit(2);
    }
    FILE** fp_array = reinterpret_cast<FILE**>(malloc(sizeof(FILE*)*argc));
    if(fp_array == NULL) {
        fprintf(stderr, "Error: count not malloc space for file list");
        exit(1);
    }
    bool* fp_good_array = reinterpret_cast<bool*>(malloc(sizeof(bool)*argc));
    if(fp_good_array == NULL) {
        fprintf(stderr, "Error: count not malloc space for file ok list");
        exit(1);
    }
    bool stdout_used = false;
    for(int fp=1; fp < argc; fp++) {
        fp_good_array[fp] = true;
        if((argv[fp][0] == '-') && (argv[fp][1] == 0)) {
            if(stdout_used) {
                fprintf(stderr, "Error: multiple instances of stdout used --- the special filename '-' to indicate stdout may only be used once");
                exit(2);
            }
            stdout_used = true;
            fp_array[fp] = stdout;
        }
        else {
            fp_array[fp] = fopen(argv[fp], "wb");
            if(fp_array[fp] == NULL) {
                fprintf(stderr, "Error: could not open file '%s' for writing", argv[1]);
                exit(2);
            }
        }
    }
    const size_t SIZE = 16384;
    char buf[SIZE];
    while(1) {
        size_t NUM = fread(reinterpret_cast<void*>(buf), 1, SIZE, stdin);
        if(NUM == 0) {
            if(feof(stdin)) {
            }
            else {
                fprintf(stderr, "Error reading from stdin");
            }
            break;
        }
        size_t NUM_OUT;
        for(int fp=1; fp < argc; fp++) {
            if(fp_good_array[fp]) {
                NUM_OUT = fwrite(reinterpret_cast<void*>(buf), 1, NUM, fp_array[fp]);
                if(NUM_OUT != NUM) {
                    fprintf(stderr, "Error: could not write to '%s'", argv[fp]);
                    fp_good_array[fp] = false;
                }
            }
        }
    }
    for(int fp=1; fp < argc; fp++) {
        fclose(fp_array[fp]); fp_array[fp]=NULL;
    }
    free(fp_good_array); fp_good_array = NULL;
    free(fp_array); fp_array = NULL;
    return 0;
}
