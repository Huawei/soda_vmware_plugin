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

#ifndef QUERY_CONNECTION
#define QUERY_CONNECTION

#include "../common/common.h"

class ConnectPassword : public XmlSerializable
{
public:
    ConnectPassword(){};
    virtual ~ConnectPassword(){};

    void readXml(XmlReader* reader);
    void writeXml(XmlWriter* writer);

    string getTitle();
    void setTitle(string strTitle);
    string getHint();
    void setHint(string strHint);
    string getHintId();
    void setHintId(string strHintId);
    string getTitleId();
    void setTitleId(string strTitleId);
private:
    string title;
    string titleId;
    string hint;
    string hintId;
};
class ConnectUserName : public XmlSerializable
{
public:
    ConnectUserName(){};
    virtual ~ConnectUserName(){};

    void readXml(XmlReader* reader);
    void writeXml(XmlWriter* writer);

    string getTitle();
    void setTitle(string strTitle);
    string getHint();
    void setHint(string strHint);
    string getHintId();
    void setHintId(string strHintId);
    string getTitleId();
    void setTitleId(string strTitleId);
private:
    string title;
    string titleId;
    string hint;
    string hintId;
};

class ConnectAddressTitle : public XmlSerializable
{
public:
    ConnectAddressTitle(){};
    virtual ~ConnectAddressTitle(){};

    void readXml(XmlReader *reader);
    void writeXml(XmlWriter *writer);

public:
    string getTitle();
    void setTitle(string strTitle);
    string getTitleID();
    void setTitleID(string strTitleID);
private:
    string title;
    string titleStr;
};

class ConnectAddressHint : public XmlSerializable
{
public:
    ConnectAddressHint(){};
    virtual ~ConnectAddressHint(){};

    void readXml(XmlReader *reader);
    void writeXml(XmlWriter *writer);

public:
    string getHint();
    void setHint(string strHint);
    string getHintID();
    void setHintID(string strHintID);
private:
    string hint;
    string hintStr;
};

class ConnectAddress : public XmlSerializable
{
public:
    ConnectAddress(){id="";title = ""; hint = ""; hintStr= ""; titleStr = "";};
    virtual ~ConnectAddress(){};

    void readXml(XmlReader *reader);
    void writeXml(XmlWriter *writer);
public:
    string getId();
    void setId(string strId);
    string getTitle();
    void setTitle(string strTitle);
    string getHint();
    void setHint(string strHint);
    string getHintStr();
    void setHintStr(string strHintStr);
    string getTitleStr();
    void setTitleStr(string strTitleStr);
    void setConnectHint(ConnectAddressHint &objConnectHint);
    void setConnectTitle(ConnectAddressTitle &objConnectTitle);
private:
    string id;
    string title;
    string titleStr;
    string hint;
    string hintStr;
    ConnectAddressHint connectHint;
    ConnectAddressTitle connectTitle;
};

class ConnectAddresses : public XmlSerializable
{
public:
    ConnectAddresses()
    {
        lstConnectAddress.clear();
    }
    virtual ~ConnectAddresses(){};

    void readXml(XmlReader* reader);
    void writeXml(XmlWriter* writer);

    void setConnectAddress(list<ConnectAddress> &rlstConnectAddress);
private:

    list<ConnectAddress> lstConnectAddress;
};

class  Connection : public XmlSerializable
{
public:
    Connection(){};
    virtual ~Connection(){};

    void readXml(XmlReader* reader);
    void writeXml(XmlWriter* writer);

    string getId();
    void setId(string strId);
    string getTitle();
    void setTitle(string strTitle);
    string getHint();
    void setHint(string strHint);
    string getHintId();
    void setHintId(string strHintId);
    string getTitleId();
    void setTitleId(string strTitleId);

    void setAddresses(ConnectAddresses objConnectAddress);
    void setUserName(ConnectUserName objConnectUserName);
    void setPassword(ConnectPassword objConnectPassword);
private:
    string id;
    string title;
    string titleId;
    string hint;
    string hintId;
    ConnectAddresses connectAddresses;
    ConnectPassword connectPassword;
    ConnectUserName connectUserName;
};

int query_connection(XmlReader &reader);

#endif
