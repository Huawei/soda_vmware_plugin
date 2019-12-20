// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#ifndef CONSISTENTCY_GROUPS
#define CONSISTENTCY_GROUPS

#include "source_devices.h"
#include "target_groups.h"

class ConGroupInfo : public XmlSerializable
{
public:
    virtual ~ConGroupInfo() {};
    ConGroupInfo() : is_success(false) {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    bool is_success;
    string cg_id;
    string cg_name;
    string status;
    string lun_id;
    SourceDevicesInfo source_devices_info;
    DeviceSyncInfo device_sync_info;
    TargetGroupInfo target_group;
    Warnings warnings;
    ErrorInfo error_info;
    WarnInfo war_info;
};

class ConGroupsInfo : public XmlSerializable
{
public:
    virtual ~ConGroupsInfo() {};
    ConGroupsInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    list<ConGroupInfo> lst_groups_info;

};

class HMConGroupInfo : public XmlSerializable
{
public:
    virtual ~HMConGroupInfo() {};
    HMConGroupInfo() : is_success(false) {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    bool is_success;
    string cghm_id;
    string cghm_name;
    string status;
    string lun_id;
    string stretched;
    string cptype;
    SourceDevicesInfo source_devices_info;
    DeviceSyncInfo device_sync_info;
    TargetGroupInfo target_group;
    Warnings warnings;
    ErrorInfo error_info;
    WarnInfo war_info;
};

class HMConGroupsInfo : public XmlSerializable
{
public:
    virtual ~HMConGroupsInfo() {};
    HMConGroupsInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    list<HMConGroupInfo> lst_hmgroups_info;

};


#endif
