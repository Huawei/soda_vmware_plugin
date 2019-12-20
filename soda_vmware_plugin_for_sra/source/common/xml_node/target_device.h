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

#ifndef TARGETDEVICE_H
#define TARGETDEVICE_H

#include "../xmlserial/xmlserializable.h"
#include "../common.h"
#include "device_sync.h"
#include "recovery_point.h"
#include "snapshot.h"
#include "commond_info.h"
#include "identity.h"

class TargetDeviceInfo: public XmlSerializable
{
public:
    virtual ~TargetDeviceInfo();
    TargetDeviceInfo();

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string target_key;
    string target_id;
    string target_state;
    string target_name;
    string cg_id;
    string cg_name;
    string testname;
    string stretched;
    string cptype;
    string isolationRequired;
    string vstoreid;
    bool isNfs;
    bool success;

    string cg_accessgroups;
 
    DeviceSyncInfo devicesync_info;
    RecoveryPointInfo recoverypoint_info;
    SnapShotInfo snapshot_info;
    IdentityInfo identity_info;
    IdentityInfoFS identity_infofs;
    ErrorInfo err_info;
    WarnInfo war_info;
};

#endif



