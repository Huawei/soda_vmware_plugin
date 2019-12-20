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

#include "replicated_devices.h"

void ReplicatedDevices::writeXml(XmlWriter *writer)
{
    (void)writer->set_xml("/ReplicatedDevices", &cgs_info);
    if (g_bstretch)
    {
        (void)writer->set_xml("/ReplicatedDevices", &hmcgs_info);
    }
    (void)writer->set_xml("/ReplicatedDevices", &tgs_info);
    (void)writer->set_xml("/ReplicatedDevices", &sources_info);
    (void)writer->set_xml("/ReplicatedDevices", &targets_info);
    if (!storageports_info.lst_storageportsips.empty())
    {
        (void)writer->set_xml("/ReplicatedDevices", &storageports_info);
    }
}

void ReplicatedDevices::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}
