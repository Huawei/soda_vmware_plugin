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

#include "device_sync.h"

DeviceSyncInfo::DeviceSyncInfo()
{
    sync_status = "complete";
    sync_progress = "100";
    sync_remainingtimeestimate = "100";

    return;
}

DeviceSyncInfo::~DeviceSyncInfo()
{
    return;
}

void DeviceSyncInfo::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}

void DeviceSyncInfo::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/DeviceSync/@id", sync_id.c_str());

    if (!sync_status.empty()){
        (void)writer->set_string("/DeviceSync/@status", sync_status.c_str());
    }

    if ("inProgress" == sync_status){
        (void)writer->set_string("/DeviceSync/Progress", sync_progress.c_str());
        (void)writer->set_string("/DeviceSync/RemainingTimeEstimate", sync_remainingtimeestimate.c_str());
    }

    return;
}


