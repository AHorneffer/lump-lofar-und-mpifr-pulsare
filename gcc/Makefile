# Makefile
# for LuMP version 2.3
# gcc/Makefile
# 2014 Sep 25  James Anderson  --- document existing Makefile
# 2014 Sep 28  JMA  --- edit for new INSTALL script

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




all: INSTALL_LuMP_GCC.done

INSTALL_LuMP_GCC.done: INSTALL_LuMP_GCC.bat INSTALL_LuMP_GCC.control
	./INSTALL_LuMP_GCC.bat
	touch $@





clean:
	$(RM) *~
	$(RM) *.done

distclean: clean
	$(RM) -fr src/*


install: INSTALL_LuMP_GCC.done


RM = /bin/rm -f
