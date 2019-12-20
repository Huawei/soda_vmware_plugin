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

#ifndef TARGET_GROUPS_H
#define TARGET_GROUPS_H

#include "target_devices.h"
#include "cg_snapshot.h"

class TargetGroupInfo : public XmlSerializable
{
public:
    virtual ~TargetGroupInfo() {};
    TargetGroupInfo() : success(false) {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

private:
    void _process_error_info();

public:
    string cg_id;
    string cg_name;
    string tg_key;
    string status;
    string lun_id;
    string stretched;
    string cptype;
    string isolationRequired;
    string vstoreid;
    bool success;
    TargetDevicesInfo target_devices_info;
    CgSnapshot snapshot_info;
    RecoveryPointInfo repoint_info;
    ErrorInfo err_info;
    WarnInfo war_info;
};

class TargetGroupsInfo : public XmlSerializable
{
public:
    virtual ~TargetGroupsInfo() {};
    TargetGroupsInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    list<TargetGroupInfo> lst_groups_info;

};

#endif
