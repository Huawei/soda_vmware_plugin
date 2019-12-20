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

#ifndef _BASEEXCEPTION_H_
#define _BASEEXCEPTION_H_

#include "Type.h"

class BaseException
{
public:
    int m_iErrCode;
    string m_strErrDesc;

public:
    BaseException(){m_iErrCode = 0;};
    virtual ~BaseException(){};

    BaseException(int iErrCode, string strErrDesc)
    {
        m_iErrCode = iErrCode;
        m_strErrDesc = strErrDesc;
    };

    int GetErrCode()
    {
        return m_iErrCode;
    };

    string GetErrString()
    {
        return m_strErrDesc;
    };
};

class XmlException : public BaseException
{
private:

public:
    XmlException();
    XmlException(IN int iErrCode, IN const string &strErrDesc)
    {
        m_iErrCode   = iErrCode;
        m_strErrDesc = strErrDesc;
    };
    ~XmlException(){};
    int SaveDb()
    {
        return ISSP_RTN_OK;
    }
};

#endif

