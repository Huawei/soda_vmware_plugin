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

#ifndef SOURCE_DEVICE_H
#define SOURCE_DEVICE_H

#include "target_device.h"
#include "identity.h"
#include "device_sync.h"
#include "commond_info.h"

class SourceDeviceInfo : public XmlSerializable
{
public:
    virtual ~SourceDeviceInfo(){};
    SourceDeviceInfo() : is_success(false) {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    TargetDeviceInfo target_info;
    IdentityInfo identity_info;
    IdentityInfoFS identity_infofs;
    DeviceSyncInfo device_sync_info;

    string source_name;
    string source_id;
    string cg_id;
    string cg_name;
    string percent;
    string status;
    string stretched;
    string cptype;

    Warnings warnings;
    ErrorInfo error_info;

    bool is_success;
};


#endif
