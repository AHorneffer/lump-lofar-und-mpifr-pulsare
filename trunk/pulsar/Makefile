# Makefile
# for LuMP version 2.0
# pulsar/Makefile
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




all: INSTALL_LuMP_Pulsar_DSPSR.done

pulsar_local/bin/dspsr:
	bash export USE_LUMP_PULSAR_STUFF=1;export USE_PULSAR_SEARCH=1;./INSTALL_LOFAR.bat


INSTALL_LuMP_Pulsar_INIT.done:
	mkdir -p pulsar_local
	mkdir -p pulsar_local/bin  pulsar_local/include  pulsar_local/lib  pulsar_local/lib64  pulsar_local/share  pulsar_local/src  pulsar_local/web pulsar_local/opt/search/bin pulsar_local/opt/search/lib
	touch $@

INSTALL_LuMP_Pulsar_CFITSIO.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_CFITSIO.bat INSTALL_LuMP_Pulsar_CFITSIO.control
	bash INSTALL_LuMP_Pulsar_CFITSIO.bat
	touch $@

INSTALL_LuMP_Pulsar_SOFA.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_CFITSIO.done INSTALL_LuMP_Pulsar_SOFA.bat INSTALL_LuMP_Pulsar_SOFA.control
	bash INSTALL_LuMP_Pulsar_SOFA.bat
	touch $@

INSTALL_LuMP_Pulsar_PGPLOT.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_SOFA.done INSTALL_LuMP_Pulsar_PGPLOT.bat INSTALL_LuMP_Pulsar_PGPLOT.control
	bash INSTALL_LuMP_Pulsar_PGPLOT.bat
	touch $@

INSTALL_LuMP_Pulsar_TEMPO.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PGPLOT.done INSTALL_LuMP_Pulsar_TEMPO.bat INSTALL_LuMP_Pulsar_TEMPO.control
	bash INSTALL_LuMP_Pulsar_TEMPO.bat
	touch $@

INSTALL_LuMP_Pulsar_ATLAS.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_TEMPO.done INSTALL_LuMP_Pulsar_ATLAS.bat INSTALL_LuMP_Pulsar_ATLAS.control
	bash INSTALL_LuMP_Pulsar_ATLAS.bat
	touch $@

INSTALL_LuMP_Pulsar_TEMPO2.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_ATLAS.done INSTALL_LuMP_Pulsar_TEMPO2.bat INSTALL_LuMP_Pulsar_TEMPO2.control
	bash INSTALL_LuMP_Pulsar_TEMPO2.bat
	touch $@

INSTALL_LuMP_Pulsar_PSRCAT.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_TEMPO2.done INSTALL_LuMP_Pulsar_PSRCAT.bat INSTALL_LuMP_Pulsar_PSRCAT.control
	bash INSTALL_LuMP_Pulsar_PSRCAT.bat
	touch $@

INSTALL_LuMP_Pulsar_PSRXML.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PSRCAT.done INSTALL_LuMP_Pulsar_PSRXML.bat INSTALL_LuMP_Pulsar_PSRXML.control
	bash INSTALL_LuMP_Pulsar_PSRXML.bat
	touch $@

INSTALL_LuMP_Pulsar_PSRCHIVE.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PSRXML.done INSTALL_LuMP_Pulsar_PSRCHIVE.bat INSTALL_LuMP_Pulsar_PSRCHIVE.control
	bash INSTALL_LuMP_Pulsar_PSRCHIVE.bat
	touch $@

INSTALL_LuMP_Pulsar_PSRDADA.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PSRCHIVE.done INSTALL_LuMP_Pulsar_PSRDADA.bat INSTALL_LuMP_Pulsar_PSRDADA.control
	bash INSTALL_LuMP_Pulsar_PSRDADA.bat
	touch $@

INSTALL_LuMP_Pulsar_WCSLIB.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PSRDADA.done INSTALL_LuMP_Pulsar_WCSLIB.bat INSTALL_LuMP_Pulsar_WCSLIB.control
	bash INSTALL_LuMP_Pulsar_WCSLIB.bat
	touch $@

INSTALL_LuMP_Pulsar_CASACORE.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_WCSLIB.done INSTALL_LuMP_Pulsar_CASACORE.bat INSTALL_LuMP_Pulsar_CASACORE.control
	bash INSTALL_LuMP_Pulsar_CASACORE.bat
	touch $@

INSTALL_LuMP_Pulsar_LOFARDAL.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_CASACORE.done INSTALL_LuMP_Pulsar_LOFARDAL.bat INSTALL_LuMP_Pulsar_LOFARDAL.control
	bash INSTALL_LuMP_Pulsar_LOFARDAL.bat
	touch $@

INSTALL_LuMP_Pulsar_PRESTO.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_LOFARDAL.done INSTALL_LuMP_Pulsar_PRESTO.bat INSTALL_LuMP_Pulsar_PRESTO.control
	bash INSTALL_LuMP_Pulsar_PRESTO.bat
	touch $@

INSTALL_LuMP_Pulsar_SIGPROC.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_PRESTO.done INSTALL_LuMP_Pulsar_SIGPROC.bat INSTALL_LuMP_Pulsar_SIGPROC.control
	bash INSTALL_LuMP_Pulsar_SIGPROC.bat
	touch $@

# INSTALL_LuMP_Pulsar_PULSARHUNTER.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_SIGPROC.done INSTALL_LuMP_Pulsar_PULSARHUNTER.bat INSTALL_LuMP_Pulsar_PULSARHUNTER.control
# 	bash INSTALL_LuMP_Pulsar_PULSARHUNTER.bat
# 	touch $@

INSTALL_LuMP_Pulsar_DSPSR.done: INSTALL_LuMP_Pulsar_INIT.done INSTALL_LuMP_Pulsar_SIGPROC.done INSTALL_LuMP_Pulsar_DSPSR.bat INSTALL_LuMP_Pulsar_DSPSR.control
	bash INSTALL_LuMP_Pulsar_DSPSR.bat
	touch $@








clean:
	$(RM) *~
	$(RM) *.done

distclean: clean
	$(RM) -fr pulsar_local
	$(RM) -f *.control


install: INSTALL_LuMP_Pulsar_DSPSR.done


RM = /bin/rm -f
