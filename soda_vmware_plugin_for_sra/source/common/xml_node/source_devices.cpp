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

#include "source_devices.h"

void SourceDevicesInfo::writeXml(XmlWriter *writer)
{
    list<SourceDeviceInfo>::iterator iter = lst_source_devices.begin();

    while (iter != lst_source_devices.end()){
        (void)writer->set_xml("/SourceDevices", &(*iter));
        ++iter;
    }
}

void SourceDevicesInfo::readXml(XmlReader *reader)
{
    (void)reader;
}
