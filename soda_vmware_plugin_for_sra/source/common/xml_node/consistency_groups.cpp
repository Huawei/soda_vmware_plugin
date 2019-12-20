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

#include "consistency_groups.h"

void ConGroupInfo::writeXml(XmlWriter *writer)
{
    if (!cg_name.empty()){
        (void)writer->set_string("/ConsistencyGroup/Name", cg_name.c_str());
    }

    if (!cg_id.empty()){       
        (void)writer->set_string("/ConsistencyGroup/@id", cg_id.c_str());
    }

    if (!target_group.tg_key.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &target_group);
    }

    if (!status.empty()){
        (void)writer->set_string("/ConsistencyGroup/@state", status.c_str());
    }

    if (!source_devices_info.lst_source_devices.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &source_devices_info);
    }

    if (!device_sync_info.sync_id.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &device_sync_info);
    }

    if (error_info.hasError()){
        (void)writer->set_xml("/ConsistencyGroup", &error_info);
    }

    if (war_info.hasWarning()){
        (void)writer->set_xml("/ConsistencyGroup", &war_info);
    }

    if (!warnings.lst_warn.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &warnings);
    }

    if (is_success){
        (void)writer->set_string("/ConsistencyGroup/Success", "");
    }

    
}

void ConGroupInfo::readXml(XmlReader *reader)
{
    (void)reader;
}

void HMConGroupInfo::writeXml(XmlWriter *writer)
{
    if (!cghm_name.empty()){
        (void)writer->set_string("/ConsistencyGroup/Name", cghm_name.c_str());
    }

    if (!cghm_id.empty()){       
        (void)writer->set_string("/ConsistencyGroup/@id", cghm_id.c_str());
    }

    if (!target_group.tg_key.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &target_group);
    }

    if (!status.empty()){
        (void)writer->set_string("/ConsistencyGroup/@state", status.c_str());
    }

    if (g_bstretch){
        if (!stretched.empty()){
            if ("true" == stretched){
                (void)writer->set_string("/ConsistencyGroup/@stretchedStorage", stretched.c_str());
            }
        }

        if (!cptype.empty()){
            if ("false" == cptype){
                (void)writer->set_string("/ConsistencyGroup/@staticSitePreference", cptype.c_str());
            }
        }
    }

    if (!source_devices_info.lst_source_devices.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &source_devices_info);
    }

    if (!device_sync_info.sync_id.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &device_sync_info);
    }

    if (error_info.hasError()){
        (void)writer->set_xml("/ConsistencyGroup", &error_info);
    }

    if (war_info.hasWarning()){
        (void)writer->set_xml("/ConsistencyGroup", &war_info);
    }

    if (!warnings.lst_warn.empty()){
        (void)writer->set_xml("/ConsistencyGroup", &warnings);
    }

    if (is_success){
        (void)writer->set_string("/ConsistencyGroup/Success", "");
    }
}

void HMConGroupInfo::readXml(XmlReader *reader)
{
    (void)reader;
}

void ConGroupsInfo::writeXml(XmlWriter *writer)
{
    list<ConGroupInfo>::iterator iter = lst_groups_info.begin();

    while (iter != lst_groups_info.end()){
        (void)writer->set_xml("/ConsistencyGroups", &(*iter));

        ++iter;
    }
}

void ConGroupsInfo::readXml(XmlReader *reader)
{
    (void)reader;
}

void HMConGroupsInfo::writeXml(XmlWriter *writer)
{
    list<HMConGroupInfo>::iterator iter = lst_hmgroups_info.begin();

    while (iter != lst_hmgroups_info.end()){
        (void)writer->set_xml("/ConsistencyGroups", &(*iter));

        ++iter;
    }
}

void HMConGroupsInfo::readXml(XmlReader *reader)
{
    (void)reader;
}
