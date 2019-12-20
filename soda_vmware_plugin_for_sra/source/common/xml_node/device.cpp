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

#include "device.h"

DeviceInfo::DeviceInfo()
{
    is_success = false;
    return;
}

void DeviceInfo::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/Device/@id", id.c_str());

    if (is_success){
        (void)writer->set_string("/Device/Success", "");
    }

    if (error_info.hasError()){
        (void)writer->set_xml("/Device", &error_info);
    }

    if (war_info.hasWarning()){
        (void)writer->set_xml("/Device", &war_info);
    }

    if (!warnings.lst_warn.empty()){
        (void)writer->set_xml("/Device", &warnings);
    }

    return;
}

void DeviceInfo::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}

void DevicesInfo::writeXml(XmlWriter *writer)
{
    list<DeviceInfo>::iterator iter = lst_device_info.begin();

    while (iter != lst_device_info.end()){
        (void)writer->set_xml("/Devices", &(*iter));

        ++iter;
    }

    return;
}

void DevicesInfo::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}

