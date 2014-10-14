#!/usr/bin/env python
# LuMP_Computer_Information.py
# Information on known recording computers
#_HIST  DATE NAME PLACE INFO
# 2012 Apr 25  James M Anderson  --MPIfR  start



# Copyright (c) 2012, 2013, 2014 James M. Anderson <anderson@mpifr-bonn.mpg.de>

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








import os
import logging
import MPIfR_LOFAR_LuMP



LuMP_Recording_Computer_Dict = {}
LuMP_Station_Dict = {}
LuMP_Recorder_Group_Dict = {}



# Recording Computer Information
# These are the key values that shoudl be present for each computer
# in the LuMP_Recording_Computer_Dict dict of computer dicts
# 
#    num_cores               # Number of physical (not hyperthreading) cores
#    cache_per_core          # in Bytes
#    ram                     # in Bytes
#    observer_account        # name of the account
                             # to use for observing
                             # if None, then use
                             # current user account name
#    login_sh                # Path and filename for
                             # the LuMP LOGIN.SH file
#    main_data_dir           # Full path to the main
                             # directory under which data
                             # should be written to
#    ip_internal             # string like "123.456.789.1"
#    mac_internal            # string like "01:23:45:67:89:ab"
#    ip_single_station       # string like "123.456.789.1"
#    mac_single_station      # string like "01:23:45:67:89:ab"
#    ip_international        # string like "123.456.789.1"
#    mac_international       # string like "01:23:45:67:89:ab"
#
#
# The name of the computer is the key for each entry in the main
# LuMP_Recording_Computer_Dict dict





# Station Information
# These are the key values that shoudl be present for each dtation
# in the LuMP_Station_Dict dict of computer dicts
# 
#    full_name               # LuMP name of the station, e.g. EfDE601
#    packet_header_station_code # 16 bit station integer code in the data packets
#    ip_rsp_lane0            # IP address of RSP lane 0
#    mac_rsp_lane0           # MAC address of RSP lane 0
#    vlan_rsp_lane0          # VLAN of RSP lane 0
#    port_rsp_lane0          # port number for data sent by RSP lane 0
#    ip_rsp_lane1            # IP address of RSP lane 1
#    mac_rsp_lane1           # MAC address of RSP lane 1
#    vlan_rsp_lane1          # VLAN of RSP lane 1
#    port_rsp_lane1          # port number for data sent by RSP lane 1
#    ip_rsp_lane2            # IP address of RSP lane 2
#    mac_rsp_lane2           # MAC address of RSP lane 2
#    vlan_rsp_lane2          # VLAN of RSP lane 2
#    port_rsp_lane2          # port number for data sent by RSP lane 2
#    ip_rsp_lane3            # IP address of RSP lane 3
#    mac_rsp_lane3           # MAC address of RSP lane 3
#    vlan_rsp_lane3          # VLAN of RSP lane 3
#    port_rsp_lane3          # port number for data sent by RSP lane 3
#    ip_lcu                  # IP address of the LCU for the station
#    mac_lcu                 # MAC address of the LCU for the station
#    observer_account        # Name of the observer account to use
#    main_data_dir           # full path name for main data storage area




# Station Information
# These are the key values that should be present for each dtation
# in the LuMP_Station_Dict dict of computer dicts
# 
#    station                 # name of the Station Information entry (see above)
#    recorder_lane0          # name of the lane 0 Recording Computer entry
#    recorder_lane1          # name of the lane 1 Recording Computer entry
#    recorder_lane2          # name of the lane 2 Recording Computer entry
#    recorder_lane3          # name of the lane 3 Recording Computer entry








################################################################################
def get_filesystem_freespace(pathname):
    """Get the free space in bytes of the filesystem containing pathname

From http://stackoverflow.com/questions/4260116/find-size-and-free-space-of-the-filesystem-containing-a-given-file
"""
    stat= os.statvfs(pathname)
    # use f_bfree for superuser, or f_bavail if filesystem
    # has reserved space for superuser
    return stat.f_bavail*stat.f_bsize





################################################################################
def read_computer_file_info_dict(filename):
    """Read a text file of recording computer info into LuMP_Recording_Computer_Dict"""

    global LuMP_Recording_Computer_Dict
    try:
        fp = open(filename,"r")
        linecount = 0
        lline = fp.readline()
        linecount += 1
        while(len(lline) > 0):
            line = lline.strip()
            ha_pos = line.find('#')
            if(ha_pos>=0):
                line = line[0:ha_pos].strip()
            if(len(line) == 0):
                lline = fp.readline()
                linecount += 1
                continue
            eq_pos = line.find('=')
            if(eq_pos <= 0):
                logging.error("bad computer dict start in file '%s': line %d: content '%s', no '='", filename, linecount, lline)
                raise RuntimeError("bad computer_file_info_dict")
            cb_pos = line.find('{')
            if(cb_pos <= 0):
                logging.error("bad computer dict start in file '%s': line %d: content '%s', no '{'", filename, linecount, lline)
                raise RuntimeError("bad computer_file_info_dict")
            if(cb_pos < eq_pos):
                logging.error("bad computer dict start in file '%s': line %d: content '%s', '{' before '='", filename, linecount, lline)
                raise RuntimeError("bad computer_file_info_dict")
            namea = line[0:eq_pos].split()
            if(len(namea) != 1):
                logging.error("bad computer dict start in file '%s': line %d: content '%s', bad computer name", filename, linecount, lline)
                raise RuntimeError("bad computer_file_info_dict")
            NAME = namea[0]
            if(LuMP_Recording_Computer_Dict.has_key(NAME)):
                logging.warning("recording computer dict already has computer '%s', entry in file '%', line %d will overwrite the old value", NAME, filename, linecount)
            end_found = False
            computer = {}
            computer["filename"] = filename
            computer_line = linecount
            lline = fp.readline()
            linecount += 1
            while(len(lline) > 0):
                line = lline.strip()
                ha_pos = line.find('#')
                if(ha_pos>=0):
                    line = line[0:ha_pos].strip()
                if(len(line) == 0):
                    lline = fp.readline()
                    linecount += 1
                    continue
                if(line[0] == '}'):
                    end_found = True
                    break
                if(line[-1] == ','):
                    line = line[:-1]
                co_pos = line.find(':')
                if(co_pos <= 0):
                    logging.error("bad computer dict entry start in file '%s': line %d: content '%s', no ':'", filename, linecount, lline)
                    raise RuntimeError("bad computer_file_info_dict")
                if(co_pos == len(line)):
                    logging.error("bad computer dict entry start in file '%s': line %d: content '%s', no value following ':'", filename, linecount, lline)
                    raise RuntimeError("bad computer_file_info_dict")
                key = line[0:co_pos].strip()
                value = line[co_pos+1:].strip()
                if(key not in read_computer_file_info_dict.known_computer_keys):
                    logging.warning("bad computer dict entry in file '%s': line %d: content '%s', key '%s' not recognized", filename, linecount, lline, key)
                elif(computer.has_key(key)):
                    logging.warning("computer dict entry in file '%s': line %d: content '%s', key '%s' already exists, overwriting", filename, linecount, lline, key)
                if(key in read_computer_file_info_dict.integer_computer_keys):
                    try:
                        value = int(value,0)
                    except ValueError:
                        logging.error("bad computer dict entry in file '%s': line %d: content '%s', invalid argument for integer key '%s'", filename, linecount, lline, key)
                        raise RuntimeError("bad computer_file_info_dict")
                else:
                    # value is a string
                    if(value == "None"):
                        value = None
                computer[key] = value
                lline = fp.readline()
                linecount += 1
            if(not end_found):
                logging.error("bad computer dict end in file '%s': line %d: no end of dict found", filename, linecount, lline)
                raise RuntimeError("bad computer_file_info_dict")
            for key in read_computer_file_info_dict.known_computer_keys:
                if(computer.has_key(key)):
                    pass
                else:
                    logging.warning("computer dict in file '%s': line %d: has no key named '%s'", filename, computer_line, key)
                    computer[key] = None
            LuMP_Recording_Computer_Dict[NAME] = computer
            lline = fp.readline()
            linecount += 1
        fp.close()
    except:
        logging.error("Error while parsing recording computer information file '%s'", filename)
        raise
        raise RuntimeError("bad computer_file_info_dict")
    return
read_computer_file_info_dict.known_computer_keys = [
    "num_cores", "cache_per_core", "ram", "observer_account", "login_sh",
    "main_data_dir", "ip_internal", "mac_internal",
    "ip_single_station", "mac_single_station", "ip_international",
    "mac_international"]
read_computer_file_info_dict.integer_computer_keys = [
    "num_cores", "cache_per_core", "ram"]





################################################################################
def read_station_file_info_dict(filename):
    """Read a text file of recording computer info into LuMP_Station_Dict"""

    global LuMP_Station_Dict
    try:
        fp = open(filename,"r")
        linecount = 0
        lline = fp.readline()
        linecount += 1
        while(len(lline) > 0):
            line = lline.strip()
            ha_pos = line.find('#')
            if(ha_pos>=0):
                line = line[0:ha_pos].strip()
            if(len(line) == 0):
                lline = fp.readline()
                linecount += 1
                continue
            eq_pos = line.find('=')
            if(eq_pos <= 0):
                logging.error("bad station dict start in file '%s': line %d: content '%s', no '='", filename, linecount, lline)
                raise RuntimeError("bad station_file_info_dict")
            cb_pos = line.find('{')
            if(cb_pos <= 0):
                logging.error("bad station dict start in file '%s': line %d: content '%s', no '{'", filename, linecount, lline)
                raise RuntimeError("bad station_file_info_dict")
            if(cb_pos < eq_pos):
                logging.error("bad station dict start in file '%s': line %d: content '%s', '{' before '='", filename, linecount, lline)
                raise RuntimeError("bad station_file_info_dict")
            namea = line[0:eq_pos].split()
            if(len(namea) != 1):
                logging.error("bad station dict start in file '%s': line %d: content '%s', bad station name", filename, linecount, lline)
                raise RuntimeError("bad station_file_info_dict")
            NAME = namea[0]
            if(LuMP_Station_Dict.has_key(NAME)):
                logging.warning("recording station dict already has station '%s', entry in file '%', line %d will overwrite the old value", NAME, filename, linecount)
            end_found = False
            station = {}
            station["filename"] = filename
            station_line = linecount
            lline = fp.readline()
            linecount += 1
            while(len(lline) > 0):
                line = lline.strip()
                ha_pos = line.find('#')
                if(ha_pos>=0):
                    line = line[0:ha_pos].strip()
                if(len(line) == 0):
                    lline = fp.readline()
                    linecount += 1
                    continue
                if(line[0] == '}'):
                    end_found = True
                    break
                if(line[-1] == ','):
                    line = line[:-1]
                co_pos = line.find(':')
                if(co_pos <= 0):
                    logging.error("bad station dict entry start in file '%s': line %d: content '%s', no ':'", filename, linecount, lline)
                    raise RuntimeError("bad station_file_info_dict")
                if(co_pos == len(line)):
                    logging.error("bad station dict entry start in file '%s': line %d: content '%s', no value following ':'", filename, linecount, lline)
                    raise RuntimeError("bad station_file_info_dict")
                key = line[0:co_pos].strip()
                value = line[co_pos+1:].strip()
                if(key not in read_station_file_info_dict.known_station_keys):
                    logging.warning("bad station dict entry in file '%s': line %d: content '%s', key '%s' not recognized", filename, linecount, lline, key)
                elif(station.has_key(key)):
                    logging.warning("station dict entry in file '%s': line %d: content '%s', key '%s' already exists, overwriting", filename, linecount, lline, key)
                if(key in read_station_file_info_dict.integer_station_keys):
                    try:
                        value = int(value,0)
                    except ValueError:
                        logging.error("bad station dict entry in file '%s': line %d: content '%s', invalid argument for integer key '%s'", filename, linecount, lline, key)
                        raise RuntimeError("bad station_file_info_dict")
                else:
                    # value is a string
                    if(value == "None"):
                        value = None
                station[key] = value
                lline = fp.readline()
                linecount += 1
            if(not end_found):
                logging.error("bad station dict end in file '%s': line %d: no end of dict found", filename, linecount, lline)
                raise RuntimeError("bad station_file_info_dict")
            for key in read_station_file_info_dict.known_station_keys:
                if(station.has_key(key)):
                    pass
                else:
                    logging.warning("station dict in file '%s': line %d: has no key named '%s'", filename, station_line, key)
                    station[key] = None
            LuMP_Station_Dict[NAME] = station
            lline = fp.readline()
            linecount += 1
        fp.close()
    except:
        logging.error("Error while parsing recording station information file '%s'", filename)
        raise
        raise RuntimeError("bad station_file_info_dict")
    return
read_station_file_info_dict.known_station_keys = [
    "full_name", "packet_header_station_code", "ip_rsp_lane0", "mac_rsp_lane0", "vlan_rsp_lane0", "port_rsp_lane0", "ip_rsp_lane1", "mac_rsp_lane1", "vlan_rsp_lane1", "port_rsp_lane1", "ip_rsp_lane2", "mac_rsp_lane2", "vlan_rsp_lane2", "port_rsp_lane2", "ip_rsp_lane3", "mac_rsp_lane3", "vlan_rsp_lane3", "port_rsp_lane3", "ip_lcu", "mac_lcu", "observer_account", "main_data_dir"]
read_station_file_info_dict.integer_station_keys = [
    "packet_header_station_code", "vlan_rsp_lane0", "vlan_rsp_lane1", "vlan_rsp_lane2", "vlan_rsp_lane3"]







################################################################################
def read_recorder_group_file_info_dict(filename):
    """Read a text file of recording computer info into LuMP_Recorder_Group_Dict"""

    global LuMP_Recording_Computer_Dict
    global LuMP_Station_Dict
    global LuMP_Recorder_Group_Dict
    try:
        fp = open(filename,"r")
        linecount = 0
        lline = fp.readline()
        linecount += 1
        while(len(lline) > 0):
            line = lline.strip()
            ha_pos = line.find('#')
            if(ha_pos>=0):
                line = line[0:ha_pos].strip()
            if(len(line) == 0):
                lline = fp.readline()
                linecount += 1
                continue
            eq_pos = line.find('=')
            if(eq_pos <= 0):
                logging.error("bad recorder_group dict start in file '%s': line %d: content '%s', no '='", filename, linecount, lline)
                raise RuntimeError("bad recorder_group_file_info_dict")
            cb_pos = line.find('{')
            if(cb_pos <= 0):
                logging.error("bad recorder_group dict start in file '%s': line %d: content '%s', no '{'", filename, linecount, lline)
                raise RuntimeError("bad recorder_group_file_info_dict")
            if(cb_pos < eq_pos):
                logging.error("bad recorder_group dict start in file '%s': line %d: content '%s', '{' before '='", filename, linecount, lline)
                raise RuntimeError("bad recorder_group_file_info_dict")
            namea = line[0:eq_pos].split()
            if(len(namea) != 1):
                logging.error("bad recorder_group dict start in file '%s': line %d: content '%s', bad recorder_group name", filename, linecount, lline)
                raise RuntimeError("bad recorder_group_file_info_dict")
            NAME = namea[0]
            if(LuMP_Recorder_Group_Dict.has_key(NAME)):
                logging.warning("recording recorder_group dict already has recorder_group '%s', entry in file '%', line %d will overwrite the old value", NAME, filename, linecount)
            end_found = False
            recorder_group = {}
            recorder_group["filename"] = filename
            recorder_group_line = linecount
            lline = fp.readline()
            linecount += 1
            while(len(lline) > 0):
                line = lline.strip()
                ha_pos = line.find('#')
                if(ha_pos>=0):
                    line = line[0:ha_pos].strip()
                if(len(line) == 0):
                    lline = fp.readline()
                    linecount += 1
                    continue
                if(line[0] == '}'):
                    end_found = True
                    break
                if(line[-1] == ','):
                    line = line[:-1]
                co_pos = line.find(':')
                if(co_pos <= 0):
                    logging.error("bad recorder_group dict entry start in file '%s': line %d: content '%s', no ':'", filename, linecount, lline)
                    raise RuntimeError("bad recorder_group_file_info_dict")
                if(co_pos == len(line)):
                    logging.error("bad recorder_group dict entry start in file '%s': line %d: content '%s', no value following ':'", filename, linecount, lline)
                    raise RuntimeError("bad recorder_group_file_info_dict")
                key = line[0:co_pos].strip()
                value = line[co_pos+1:].strip()
                if(key not in read_recorder_group_file_info_dict.known_recorder_group_keys):
                    logging.warning("bad recorder_group dict entry in file '%s': line %d: content '%s', key '%s' not recognized", filename, linecount, lline, key)
                elif(recorder_group.has_key(key)):
                    logging.warning("recorder_group dict entry in file '%s': line %d: content '%s', key '%s' already exists, overwriting", filename, linecount, lline, key)
                if(value == "None"):
                    value = None
                recorder_group[key] = value
                lline = fp.readline()
                linecount += 1
            if(not end_found):
                logging.error("bad recorder_group dict end in file '%s': line %d: no end of dict found", filename, linecount, lline)
                raise RuntimeError("bad recorder_group_file_info_dict")
            for key in read_recorder_group_file_info_dict.known_recorder_group_keys:
                if(recorder_group.has_key(key)):
                    if(key in read_recorder_group_file_info_dict.station_keys):
                        # check that station exists
                        station = recorder_group[key]
                        if(station is None):
                            pass
                        elif(LuMP_Station_Dict.has_key(station)):
                            pass
                        else:
                            logging.error("recorder_group dict in file '%s': line %d: no station name '%s' in station data", filename, recorder_group_line, station)
                            raise RuntimeError("bad recorder_group_file_info_dict")
                    elif(key in read_recorder_group_file_info_dict.recorder_keys):
                        # check that computer exists
                        computer = recorder_group[key]
                        if(computer is None):
                            pass
                        elif(LuMP_Recording_Computer_Dict.has_key(computer)):
                            pass
                        else:
                            logging.error("recorder_group dict in file '%s': line %d: no computer name '%s' in computer data", filename, recorder_group_line, computer)
                            raise RuntimeError("bad recorder_group_file_info_dict")                        
                else:
                    logging.warning("recorder_group dict in file '%s': line %d: has no key named '%s'", filename, recorder_group_line, key)
                    recorder_group[key] = None
            LuMP_Recorder_Group_Dict[NAME] = recorder_group
            lline = fp.readline()
            linecount += 1
        fp.close()
    except:
        logging.error("Error while parsing recording recorder_group information file '%s'", filename)
        raise RuntimeError("bad recorder_group_file_info_dict")
    return
read_recorder_group_file_info_dict.known_recorder_group_keys = [
    "station", "recorder_lane0", "recorder_lane1", "recorder_lane2", "recorder_lane3"]
read_recorder_group_file_info_dict.station_keys = ["station"]
read_recorder_group_file_info_dict.recorder_keys = [
    "recorder_lane0", "recorder_lane1", "recorder_lane2", "recorder_lane3"]







################################################################################
def check_recorder_group(group):
    if(not LuMP_Recorder_Group_Dict.has_key(group)):
        logging.error("Recording group '%s' is not found in the list of known recording groups (%s)", group, LuMP_Recorder_Group_Dict.keys())
        raise RuntimeError("unknown recorder_group")
    rg = LuMP_Recorder_Group_Dict[group]
    station = rg["station"]
    if(not LuMP_Station_Dict.has_key(station)):
        logging.error("Recording group '%s' has station '%s' that is not in known list of stations (%s)", group, station, LuMP_Station_Dict.keys())
        raise RuntimeError("unknown station")
    for l in range(MPIfR_LOFAR_LuMP.NUM_LOFAR_STATION_RSP_LANES):
        rl = "recorder_lane%d"%l
        computer = rg[rl]
        if(computer is None):
            continue
        if(not LuMP_Recording_Computer_Dict.has_key(rl)):
            logging.error("Recording group '%s' has %s recording computer '%s' that is not in known list of recording computers (%s)", group, rl, computer, LuMP_Recording_Computer_Dict.keys())
            raise RuntimeError("unknown recording computer")

    












################################################################################
def init():
    global LuMP_Recording_Computer_Dict
    global LuMP_Station_Dict
    global LuMP_Recorder_Group_Dict
    LuMP_Recording_Computer_Dict = {}
    LuMP_Station_Dict = {}
    LuMP_Recorder_Group_Dict = {}
    if(init.initted):
        return
    LuMP_ROOT = os.getenv(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT)
    if(LuMP_ROOT is None):
        logging.warning("Environment variable '%s' not set: standard recording computer info not loaded", MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT)
        # try local directory
        read_computer_file_info_dict("LuMP_Recording_Computers.dat")
        read_station_file_info_dict("LuMP_Stations.dat")
        read_recorder_group_file_info_dict("LuMP_Recorder_Group.dat")
        return
    filename = os.path.join(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                            "LuMP_Recording_Computers.dat")
    read_computer_file_info_dict(filename)
    filename = os.path.join(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                            "LuMP_Stations.dat")
    read_station_file_info_dict(filename)
    filename = os.path.join(MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_ROOT,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LIBDATA_DIRECTORY,
                            MPIfR_LOFAR_LuMP.MPIfR_LOFAR_LuMP_CONFIGURATION_DIRECTORY,
                            "LuMP_Recorder_Group.dat")
    read_recorder_group_file_info_dict(filename)
    init.initted = True
    return
init.initted = False



init()
