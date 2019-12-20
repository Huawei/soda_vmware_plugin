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

#ifndef _COMMOND_INFO
#define _COMMOND_INFO

#include "../xmlserial/xmlserializable.h"
#include "../common.h"

class ErrorInfo: public XmlSerializable
{
public:
    virtual ~ErrorInfo(){};
    ErrorInfo(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    bool hasError();

public:
    string code;
    string desc;
    string hint;
};

class WarnInfo: public XmlSerializable
{
public:
    virtual ~WarnInfo(){};
    WarnInfo(){};
public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);
    bool hasWarning();
public:
    string code;
    string desc;
    string hint;
};

class Warnings: public XmlSerializable
{
public:
    virtual ~Warnings(){};
    Warnings(){};
public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);
public:
   list<WarnInfo> lst_warn;
};

class ErrorDefinition: public XmlSerializable
{
public:
    virtual ~ErrorDefinition(){};
    ErrorDefinition(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string code;
    string desc_id;
    string desc;
    string hint_id;
    string hint;
};

class WarningDefinition: public XmlSerializable
{
public:
    virtual ~WarningDefinition(){};
    WarningDefinition(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string code;
    string desc_id;
    string desc;
    string hint_id;
    string hint;
};

class ErrorDefinitions: public XmlSerializable
{
public:
    virtual ~ErrorDefinitions(){};
    ErrorDefinitions(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    list<ErrorDefinition> lst_err;
};

class WarningDefinitions: public XmlSerializable
{
public:
    virtual ~WarningDefinitions(){};
    WarningDefinitions(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    list<WarningDefinition> lst_warn;
};

class Description: public XmlSerializable
{
public:
    virtual ~Description(){};
    Description(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string str_id;
    string content;
};

class FixHint: public XmlSerializable
{
public:
    virtual ~FixHint(){};
    FixHint(){};

public:
    void writeXml(XmlWriter *writer);
    void readXml(XmlReader *reader);

public:
    string str_id;
    string content;
};

#endif

