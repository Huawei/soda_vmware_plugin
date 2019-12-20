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

#include "target_device.h"

TargetDeviceInfo::TargetDeviceInfo() : success(false)
{
    isNfs = false;
}

TargetDeviceInfo::~TargetDeviceInfo()
{

}

void TargetDeviceInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void TargetDeviceInfo::writeXml(XmlWriter *writer)
{
    if (!target_key.empty()){
        (void)writer->set_string("/TargetDevice/@key", target_key.c_str());
    }

    if (err_info.hasError()){
        (void)writer->set_xml("/TargetDevice", &err_info);

        return;
    }

    if (!target_id.empty()){
        (void)writer->set_string("/TargetDevice/@id", target_id.c_str());
    }

    if (!target_name.empty()){
        (void)writer->set_string("/TargetDevice/Name", target_name.c_str());
    }

    if (!target_state.empty()){
        (void)writer->set_string("/TargetDevice/@state", target_state.c_str());
    }

    if (g_bstretch){
        if (!stretched.empty()){
            if ("true" == stretched){
                (void)writer->set_string("/TargetDevice/@stretchedStorage", stretched.c_str());
            }
        }
    }
    if (!identity_info.source_wwn.empty()){
        (void)writer->set_xml("/TargetDevice", &identity_info);
    }

    if (!identity_infofs.source_NfsName.empty()){
        (void)writer->set_xml("/TargetDevice",&identity_infofs);
    }

    if (success){
        (void)writer->set_string("/TargetDevice/Success/", "");
    }

    if (!devicesync_info.sync_id.empty()){
        (void)writer->set_xml("/TargetDevice", &devicesync_info);
    }

    if (!snapshot_info.snap_id.empty()){
        (void)writer->set_xml("/TargetDevice", &snapshot_info);
    }
    if (!recoverypoint_info.rp_id.empty()){
        (void)writer->set_xml("/TargetDevice", &recoverypoint_info);
    }

    if (war_info.hasWarning()){
        (void)writer->set_xml("/TargetDevice/Warnings", &war_info);
    }
    return;
}



