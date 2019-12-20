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

#ifndef _REST_COMMON_H_
#define _REST_COMMON_H_

#include <string>
#include <vector>
#include <sstream>
#include <json/reader.h>

using namespace std;

class CRestPackage
{
public:
    CRestPackage();
    CRestPackage(const CRestPackage &pkgRest);
    ~CRestPackage();

    /*************************************************
    Function    : void dump(std::ostringstream& str) const
    Description : Export package data
    Calls       : 
    Called By   : 
    Input       : 
    Output      : str 
    Return      : 
    Others      :
    *************************************************/
    void dump(std::ostringstream& str) const;

    /*************************************************
    Function    : int decode(string strMsgInfo)
    Description : Decoding function
    Calls       : 
    Called By   : 
    Input       : strMsgInfo
    Output      : 
    Return      : Successfully returns 0, failure returns -1
    Others      : 
    *************************************************/
    int decode(string &strMsgInfo);

    /*************************************************
    Function    : CRestRecord& operator [](size_t index)
    Description : Get record
    Calls       : 
    Called By   : 
    Input       : index Offset
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    Json::Value &operator [](size_t index);

    /*************************************************
    Function    : count(void) const
    Description : Get the number of records
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      :
    Others      : 
    *************************************************/
    size_t count(void) const { return vMsgRecs->size(); }

    /*************************************************
    Function    : errorCode(void) const
    Description : get errorCode
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    int errorCode(void) const { return iErrorCode; }

    /*************************************************
    Function    : errorCode_d(void) const
    Description : Get the error code in the detail return message
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    int errorCode_d(void) const { return dErrorCode; }

    /*************************************************
    Function    : description(void) const
    Description : Get the error description
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    string description(void) const { return strDescription; }

    /*************************************************
    Function    : operator=
    Description : Copy function
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    CRestPackage &operator =(const CRestPackage &pkgRest);

/*************************************************
    Function    : getResponseObjects
    Description : Get response value
    Calls       : 
    Called By   : 
    Input       : 
    Output      : 
    Return      : 
    Others      : 
    *************************************************/
    const void getResponseObjects(Json::Value &Objects) 
    { 
        Objects = m_responseObjects;
    }
    int getFusionStorageResult();
    int getFusionStorageDetailErrorcode();
    string getFusionDescription();

private:
    vector<Json::Value> *vMsgRecs;
    Json::Value *jNullValue;
    Json::Value m_responseObjects;

    int iErrorCode;
    int dErrorCode;
    string strDescription;
};


#endif

