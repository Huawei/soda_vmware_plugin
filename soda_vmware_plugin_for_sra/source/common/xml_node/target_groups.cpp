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

#include "target_groups.h"

void TargetGroupInfo::writeXml(XmlWriter *writer)
{
    _process_error_info();

    if (!cg_id.empty()){
        (void)writer->set_string("/TargetGroup/@id", cg_id.c_str());
    }

    if (!tg_key.empty()){
        (void)writer->set_string("/TargetGroup/@key", tg_key.c_str());       
    }
    if (g_bstretch){
        if (!stretched.empty()){
            if ("true" == stretched){
                (void)writer->set_string("/TargetGroup/@stretchedStorage", stretched.c_str());
            }
        }
    }
    if (err_info.hasError()){
        (void)writer->set_xml("/TargetGroup", &err_info);
        return;
    }
    if (war_info.hasWarning()){
        (void)writer->set_xml("/TargetGroup", &war_info);
    }

    if (!status.empty()){
        (void)writer->set_string("/TargetGroup/@state", status.c_str());
    }

    if (!cg_name.empty()){
        (void)writer->set_string("/TargetGroup/Name", cg_name.c_str());
    }

    if (!snapshot_info.id.empty()){
        (void)writer->set_xml("/TargetGroup", &snapshot_info);
    }

    if (!repoint_info.rp_id.empty()){
        (void)writer->set_xml("/TargetGroup", &repoint_info);
    }

    if (success){
        (void)writer->set_string("/TargetGroup/Success/", "");
    }

    if (!target_devices_info.lst_target_devices.empty()){
        (void)writer->set_xml("/TargetGroup", &target_devices_info);
    }

    return;
}

void TargetGroupInfo::_process_error_info()
{
    list<TargetDeviceInfo>::iterator iter;

    iter = snapshot_info.tg_devices_info.lst_target_devices.begin();

    if (!err_info.hasError()){
        while (iter != snapshot_info.tg_devices_info.lst_target_devices.end()){
            if (iter->err_info.hasError()){
                err_info = iter->err_info;
                iter->err_info.code.clear();
                iter->err_info.desc.clear();
            }

            ++iter;
        }
    }
    return;
}

void TargetGroupInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void TargetGroupsInfo::writeXml(XmlWriter *writer)
{
    list<TargetGroupInfo>::iterator iter = lst_groups_info.begin();

    while (iter != lst_groups_info.end()){
        (void)writer->set_xml("/TargetGroups", &(*iter));
        ++iter;
    }

    return;
}

void TargetGroupsInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}
