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

#include "query_connection.h"
#include "sra_basic.h"

string ConnectPassword::getTitle()
{
    return title;
}

void ConnectPassword::setTitle(string strTitle)
{
    title = strTitle;
}

string ConnectPassword::getHint()
{
    return hint;
}

void ConnectPassword::setHint(string strHint)
{
    hint = strHint;
}

string ConnectPassword::getHintId()
{
    return hintId;
}

void ConnectPassword::setHintId(string strHintId)
{
    hintId = strHintId;
}

void ConnectPassword::setTitleId(string strTitleId)
{
    titleId = strTitleId;
}

string ConnectPassword::getTitleId()
{
    return titleId;
}

void ConnectPassword::writeXml(XmlWriter *writer)
{
    if (!titleId.empty()){
        (void)writer->set_string("/Password/Title/@stringId", titleId.c_str());
    }
    if (!title.empty()){
        (void)writer->set_string("/Password/Title", title.c_str());
    }
    
    if (!hintId.empty()){
        (void)writer->set_string("/Password/Hint/@stringId", hintId.c_str());
    }
    if (!hint.empty()){
        (void)writer->set_string("/Password/Hint", hint.c_str());
    }
       
    return;
}

void ConnectPassword::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

string ConnectUserName::getTitle()
{
    return title;
}

void ConnectUserName::setTitle(string strTitle)
{
    title = strTitle;
}

void ConnectUserName::setHint(string strHint)
{
    hint = strHint;
}

string ConnectUserName::getHint()
{
    return hint;
}

string ConnectUserName::getHintId()
{
    return hintId;
}

void ConnectUserName::setHintId(string strHintId)
{
    hintId = strHintId;
}

void ConnectUserName::setTitleId(string strTitleId)
{
    titleId = strTitleId;
}

string ConnectUserName::getTitleId()
{
    return titleId;
}

void ConnectUserName::writeXml(XmlWriter *writer)
{
    if (!titleId.empty()){
        (void)writer->set_string("/Username/Title/@stringId", titleId.c_str());
    }
    if (!title.empty()){
        (void)writer->set_string("/Username/Title", title.c_str());
    }
    
    if (!hintId.empty()){
        (void)writer->set_string("/Username/Hint/@stringId", hintId.c_str());
    }
    if (!hint.empty()){
        (void)writer->set_string("/Username/Hint", hint.c_str());
    }

    return;
}

void ConnectUserName::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

string ConnectAddressTitle::getTitle()
{
    return title;
}

void ConnectAddressTitle::setTitle(string strTitle)
{
    title = strTitle;
}

void ConnectAddressTitle::setTitleID(string strTitleID)
{
    titleStr = strTitleID;
}

string ConnectAddressTitle::getTitleID()
{
    return titleStr;
}

void ConnectAddressTitle::writeXml(XmlWriter *writer)
{
    
    if (!titleStr.empty()){
        (void)writer->set_string("/Title/@stringId", titleStr.c_str());
    }
    if (!title.empty()){
        (void)writer->set_string("/Title", title.c_str());
    }

    return;
}

void ConnectAddressTitle::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

string ConnectAddressHint::getHint()
{
    return hint;
}

void ConnectAddressHint::setHint(string strHint)
{
    hint = strHint;
}

void ConnectAddressHint::setHintID(string strHintID)
{
    hintStr = strHintID;
}

string ConnectAddressHint::getHintID()
{
    return hintStr;
}

void ConnectAddressHint::writeXml(XmlWriter *writer)
{
    if (!hintStr.empty()){
        (void)writer->set_string("/Hint/@stringId", hintStr.c_str());
    }
    if (!hint.empty()){
        (void)writer->set_string("/Hint", hint.c_str());
    }

    return;
}

void ConnectAddressHint::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

string ConnectAddress::getHint()
{
    return hint;
}

void ConnectAddress::setHint(string strHint)
{
    hint = strHint;
}

string ConnectAddress::getId()
{
    return id;
}

void ConnectAddress::setId(string strId)
{
    id = strId;
}

string ConnectAddress::getTitle()
{
    return title;
}

void ConnectAddress::setTitle(string strTitle)
{
    title = strTitle;
}

string ConnectAddress::getHintStr()
{
    return hintStr;
}

void ConnectAddress::setHintStr(string strHintStr)
{
    hintStr = strHintStr;
}

void ConnectAddress::setTitleStr(string strTitleStr)
{
    titleStr = strTitleStr;
}

string ConnectAddress::getTitleStr()
{
    return titleStr;
}

void ConnectAddress::setConnectHint(ConnectAddressHint &objConnectHint)
{
    connectHint = objConnectHint;
}

void ConnectAddress::setConnectTitle(ConnectAddressTitle &objConnectTitle)
{
    connectTitle = objConnectTitle;
}

void ConnectAddress::writeXml(XmlWriter *writer)
{
    if (!id.empty()){
        (void)writer->set_string("/Address/@id", id.c_str());
    }
    
    (void)writer->set_xml("/Address", &connectTitle);

    (void)writer->set_xml("/Address", &connectHint);
    
    return;
}

void ConnectAddress::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void ConnectAddresses::setConnectAddress(list<ConnectAddress> &rlstConnectAddress)
{
    lstConnectAddress = rlstConnectAddress;
}

void ConnectAddresses::writeXml(XmlWriter *writer)
{
    list<ConnectAddress>::iterator itConnectAddress;
    
    if (!lstConnectAddress.empty()){
        for (itConnectAddress = lstConnectAddress.begin(); itConnectAddress != lstConnectAddress.end(); itConnectAddress++){
            ConnectAddress connectAddress = *itConnectAddress;
            (void)writer->set_xml("/Addresses", &connectAddress);
        }
    }
    return;
}

void ConnectAddresses::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

string Connection::getId()
{
    return id;
}

void Connection::setId(string strId)
{
    id = strId;
}

string Connection::getTitle()
{
    return title;
}

void Connection::setTitle(string strTitle)
{
    title = strTitle;
}

void Connection::setHint(string strHint)
{
    hint = strHint;
}

string Connection::getHint()
{
    return hint;
}

string Connection::getHintId()
{
    return hintId;
}

void Connection::setHintId(string strHintId)
{
    hintId = strHintId;
}

void Connection::setTitleId(string strTitleId)
{
    titleId = strTitleId;
}

string Connection::getTitleId()
{
    return titleId;
}

void Connection::setAddresses(ConnectAddresses objConnectAddress)
{
    connectAddresses = objConnectAddress;
}

void Connection::setUserName(ConnectUserName objConnectUserName)
{
    connectUserName = objConnectUserName;
}

void Connection::setPassword(ConnectPassword objConnectPassword)
{
    connectPassword = objConnectPassword;
}

void Connection::writeXml(XmlWriter *writer)
{
    if (!id.empty()){
        (void)writer->set_string("/Connection/@id", id.c_str());
    }
    if (!titleId.empty()){
        (void)writer->set_string("/Connection/Title/@stringId", titleId.c_str());
    }
    if (!title.empty()){
        (void)writer->set_string("/Connection/Title", title.c_str());
    }
    if (!hintId.empty()){
        (void)writer->set_string("/Connection/Hint/@stringId", hintId.c_str());
    }
    if (!hint.empty()){
        (void)writer->set_string("/Connection/Hint", hint.c_str());
    }
    
    (void)writer->set_xml("/Connection", &connectAddresses);
    (void)writer->set_xml("/Connection", &connectUserName);
    (void)writer->set_xml("/Connection", &connectPassword);
    return;
}

void Connection::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

int query_connection(XmlReader &reader)
{
    XmlWriter writer;

    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    COMMLOG(OS_LOG_INFO, "%s", "query_connection begin.");
    print("%s", "query_connection begin.");
    if (!reader.get_string(XML_OUTPUTFILE, output_file, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (reader.get_string(XML_REQUSET_TITLE, xmlns, 0)){
        CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, (const char*)xmlns), ERROR_INTERNAL_PROCESS_FAIL);
    }

    if (INBAND_TYPE == g_cli_type){

        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/@id", "arrayConnection");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Title", "Managed Array");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Title/@stringId", "ConnectionTitle");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Hint", "Local Array connection parameters");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Hint/@stringId", "ConnectionHint");

        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Addresses/Address/@id", "group1");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Addresses/Address/Title", "Array SN");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Addresses/Address/Title/@stringId", "GroupTitle");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Addresses/Address/Hint", "Enter Array SN");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Addresses/Address/Hint/@stringId", "GroupHint");

        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Username/Title", "Username");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Username/Title/@stringId", "GroupUsernameTitle");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Username/Hint", "Enter Username for Array");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Username/Hint/@stringId", "GroupUsernameHint");

        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Password/Title", "Password");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Password/Title/@stringId", "GroupPasswordTitle");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Password/Hint", "Enter Password for Array");
        (void)writer.set_string("/Response/AdapterConnectionParameters/Connection/Password/Hint/@stringId", "GroupPasswordHint");
    }
    else{
        Connection connection;
        connection.setId("arrayConnection");
        connection.setTitle("Managed Array");
        connection.setTitleId("ConnectionTitle");
        connection.setHint("Local Array connection parameters");
        connection.setHintId("ConnectionHint");

        ConnectUserName connectUserName;
        connectUserName.setTitle("Username");
        connectUserName.setTitleId("GroupUsernameTitle");
        connectUserName.setHint("Enter Username for Array");
        connectUserName.setHintId("GroupUsernameHint");

        ConnectPassword connectPassword;
        connectPassword.setTitle("Password");
        connectPassword.setTitleId("GroupPasswordTitle");
        connectPassword.setHint("Enter Password for Array");
        connectPassword.setHintId("GroupPasswordHint");

        connection.setUserName(connectUserName);
        connection.setPassword(connectPassword);

        ConnectAddresses connectAddresses;
        list<ConnectAddress> lstConnectAddress;
        lstConnectAddress.clear();
        ConnectAddress connectionMasterAddress;
        connectionMasterAddress.setId("groupMaster");
        connectionMasterAddress.setTitle("Master Management IP");
        ConnectAddressTitle connectMasterTitle;
        connectMasterTitle.setTitle("Master Management IP");
        connectMasterTitle.setTitleID("GroupTitleMaster");
        ConnectAddressHint connectMasterHint;
        connectMasterHint.setHint("Enter Master Management IP:Port(default 8088)");
        connectMasterHint.setHintID("GroupHintMaster");
        connectionMasterAddress.setConnectHint(connectMasterHint);
        connectionMasterAddress.setConnectTitle(connectMasterTitle);

        lstConnectAddress.push_back(connectionMasterAddress);

        ConnectAddress connectionSlaveAddress;
        connectionSlaveAddress.setId("groupSlave");
        connectionSlaveAddress.setTitle("Slave Management IP");
        ConnectAddressTitle connectSlaveTitle;
        connectSlaveTitle.setTitle("Slave Management IP");
        connectSlaveTitle.setTitleID("GroupTitleSlave");
        ConnectAddressHint connectSlaveHint;
        connectSlaveHint.setHint("Enter Slave Management IP:Port(default 8088)");
        connectSlaveHint.setHintID("GroupHintSlave");
        connectionSlaveAddress.setConnectHint(connectSlaveHint);
        connectionSlaveAddress.setConnectTitle(connectSlaveTitle);

        lstConnectAddress.push_back(connectionSlaveAddress);

        connectAddresses.setConnectAddress(lstConnectAddress);

        connection.setAddresses(connectAddresses);

        (void)writer.set_xml("/Response/AdapterConnectionParameters", &connection);
    }

    if(!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }
    
    COMMLOG(OS_LOG_INFO, "%s", "query_connection end.");
    print("%s", "query_connection end.");
    return RETURN_OK;
}
