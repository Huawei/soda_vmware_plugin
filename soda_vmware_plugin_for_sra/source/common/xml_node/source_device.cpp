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

#include "source_device.h"

void SourceDeviceInfo::writeXml(XmlWriter *writer)
{
    if (!source_name.empty()){
        (void)writer->set_string("/SourceDevice/Name", source_name.c_str());
    }

    (void)writer->set_string("/SourceDevice/@id", source_id.c_str());

    if (!status.empty()){
        (void)writer->set_string("/SourceDevice/@state", status.c_str());
    }

    if (g_bstretch){
        if (!stretched.empty()){
            if ("true" == stretched){
                (void)writer->set_string("/SourceDevice/@stretchedStorage", stretched.c_str());
            }
        }

        if (!cptype.empty()){
            if ("false" == cptype){
                (void)writer->set_string("/SourceDevice/@staticSitePreference", cptype.c_str());
            }
        }
    }

    if (!target_info.target_key.empty()){
        (void)writer->set_xml("/SourceDevice", &target_info);
    }

    if (!identity_info.source_wwn.empty()){
        (void)writer->set_xml("/SourceDevice", &identity_info);
    }

    if (!identity_infofs.source_NfsName.empty()){
        (void)writer->set_xml("/SourceDevice", &identity_infofs);
    }

    if (!device_sync_info.sync_id.empty()){
        (void)writer->set_xml("/SourceDevice", &device_sync_info);
    }

    if (error_info.hasError()){
        (void)writer->set_xml("/SourceDevice", &error_info);
    }

    if (!warnings.lst_warn.empty()){
        (void)writer->set_xml("/SourceDevice", &warnings);
    }

    if (is_success){
        (void)writer->set_string("/SourceDevice/Success", "");
    }
}

void SourceDeviceInfo::readXml(XmlReader *reader)
{
    (void)reader;
}
