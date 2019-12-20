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

#ifndef IDENTITY_H
#define IDENTITY_H

#include "../xmlserial/xmlserializable.h"
#include "../common.h"

class IdentityInfo : public XmlSerializable
{
public:
    virtual ~IdentityInfo() {};
    IdentityInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    string source_wwn;
};
class IdentityInfoFS : public XmlSerializable
{
public:
    virtual ~IdentityInfoFS() {};
    IdentityInfoFS() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    string source_NfsName;
};
class StoragePortsIPS:public XmlSerializable
{
public:
    virtual ~StoragePortsIPS() {};
    StoragePortsIPS() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:

    string source_IP;
};
class StoragePortsInfo:public XmlSerializable
{
public:
    virtual ~StoragePortsInfo() {};
    StoragePortsInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    list<StoragePortsIPS> lst_storageportsips;
    
};
#endif
