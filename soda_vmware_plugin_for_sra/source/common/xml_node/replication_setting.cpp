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

#include "replication_setting.h"

ReplicationSettingInfo::ReplicationSettingInfo()
{
    return;
}

ReplicationSettingInfo::~ReplicationSettingInfo()
{

}

void ReplicationSettingInfo::readXml(XmlReader *reader)
{
    list<TargetDeviceInfo>::iterator rs_info = target_device.begin();

    while (rs_info != target_device.end()){
        TargetDeviceInfo target_device_info;

        (void)reader->get_xml("/ReplicationSettings//TargetDevices", &target_device_info, 0);

        target_device.push_back(target_device_info);

        ++rs_info;
    }

    return;
}

void ReplicationSettingInfo::writeXml(XmlWriter *writer)
{
    list<TargetDeviceInfo>::iterator rs_info = target_device.begin();

    while (rs_info != target_device.end()){
        (void)writer->set_xml("/ReplicationSettings//TargetDevices", &(*rs_info));

        ++rs_info;
    }

    return;
}




