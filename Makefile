# Makefile
# for LuMP version 2.0
# 2014 Sep 25  James Anderson  --- document existing Makefile

# Copyright (c) 2014 James M. Anderson <anderson@gfz-potsdam.de>

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



################################################################################




SETUP_FILES = \
	SETUP.csh \
	SETUP.sh

DIRS_TO_MAKE = \
	doc \
	fftw \
	lump \
	pulsar

.PHONY: code_dirs

all: $(SETUP_FILES) code_dirs



SETUP.csh: make_setup.py
	./make_setup.py
SETUP.sh: make_setup.py
	./make_setup.py

gcc: $(SETUP_FILES)
	$(MAKE) -C gcc install


code_dirs: $(SETUP_FILES)
	$(MAKE) -C doc install
	$(MAKE) -C fftw install
	$(MAKE) -C pulsar install
	$(MAKE) -C lump install





clean:
	$(RM) *~
	$(foreach var,$(DIRS_TO_MAKE), $(MAKE) -C $(var) clean)

distclean: clean
	$(RM) $(SETUP_FILES)
	$(foreach var,$(DIRS_TO_MAKE), $(MAKE) -C $(var) distclean)

install: 
	$(RM) $(SETUP_FILES)
	$(foreach var,$(DIRS_TO_MAKE), $(MAKE) -C $(var) install)


RM = /bin/rm -f
