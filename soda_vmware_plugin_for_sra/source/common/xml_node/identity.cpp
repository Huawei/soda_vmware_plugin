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

#include "identity.h"

void IdentityInfo::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/Identity/Wwn", source_wwn.c_str());
}

void IdentityInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void IdentityInfoFS::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/Identity/NfsName", source_NfsName.c_str());
}

void IdentityInfoFS::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void StoragePortsInfo::writeXml(XmlWriter *writer)
{
    list<StoragePortsIPS>::iterator iter = lst_storageportsips.begin();

    while (iter != lst_storageportsips.end()){
        (void)writer->set_xml("/StoragePorts", &(*iter));

        ++iter;
    }

    return;
}

void StoragePortsInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void StoragePortsIPS::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/StoragePort/@id", source_IP.c_str());
    (void)writer->set_string("/StoragePort/@type","NFS");
}

void StoragePortsIPS::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

