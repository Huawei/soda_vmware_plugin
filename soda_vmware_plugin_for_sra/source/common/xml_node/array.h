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

#ifndef ARRAY_H
#define ARRAY_H

#include "../xmlserial/xmlserializable.h"
#include "../common.h"

class ModelInfo : public XmlSerializable
{
public:
    virtual ~ModelInfo() {};
    ModelInfo() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string model_name;
    string vender_name;
};

class ReplicationSoftware : public XmlSerializable
{
public:
    virtual ~ReplicationSoftware() {};
    ReplicationSoftware() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string software_name;
    string software_version;

};

class ArrayInfo: public XmlSerializable
{
public:
    virtual ~ArrayInfo();
    ArrayInfo();

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    char id[LENGTH_COMMON];
    char name[LENGTH_COMMON];

    ReplicationSoftware software;
    ModelInfo  model;

    list<string> peer_arrays;
    list<string> peer_stretched;
};

class PeerArray: public XmlSerializable
{
public:
    virtual ~PeerArray() {};
    PeerArray() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string id;
    string strtched;
};

class PeerArrays: public XmlSerializable
{
public:
    virtual ~PeerArrays() {};
    PeerArrays() {};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    list<PeerArray> lst_peers;
};

#endif

