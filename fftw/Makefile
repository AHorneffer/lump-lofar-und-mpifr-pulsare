# Makefile
# for LuMP version 2.0
# fftw/Makefile
# 2014 Sep 25  James Anderson  --- document existing Makefile
# 2014 Sep 28  JMA  --- change for new install system

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




all: multi/lib/libfftw3q.so

multi/lib/libfftw3q.so:
	./INSTALL_LuMP_FFTW.bat





clean:
	$(RM) *~

distclean: clean
	$(RM) src/*.tgz src/*.tar.gz

uninstall: distclean
	$(RM) -fr multi single


install: multi/lib/libfftw3q.so


RM = /bin/rm -f
