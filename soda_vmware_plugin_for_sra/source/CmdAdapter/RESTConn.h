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

#ifndef _CMD_REST_CONN_H_
#define _CMD_REST_CONN_H_

#include <string>
#include <vector>

#include "RESTPackage.h"

using namespace std;
#define FUSION_URL_STRETCH "/dsware/service/serviceCmd"
typedef enum 
{
    REST_REQUEST_MODE_GET = 0,
    REST_REQUEST_MODE_POST,
    REST_REQUEST_MODE_PUT,
    REST_REQUEST_MODE_DELETE
} REST_REQUEST_MODE;

/************************************************************************
REST mode connection class
************************************************************************/
class CRESTConn
{
    const string CONTEXT_PATH;
    const string FIELDNAME_DEVICEID;
    const string FIELDNAME_COOKIES;
    const string FILEDNAME_IBASETOKEN;

    //FusionStorage
    const string FILEDNAME_XAUTHTOKEN;
    const string CONTEXT_PATH_FUSIONSTORAGE;
public:
    CRESTConn(string strDeviceIP, string strUserName, string strPwd);
    CRESTConn(string strDeviceIP, string strUserName, string strPwd, bool isFusionStorage);

    CRESTConn(const CRESTConn &);
    CRESTConn& operator=(const CRESTConn &);
    ~CRESTConn();

    // Request information
    int doRequest(string strUrl, REST_REQUEST_MODE requstMode, string strBodyData, CRestPackage &pkgResponse);

    int doRequest(string strUrl, REST_REQUEST_MODE requstMode, string strBodyData, CRestPackage &pkgResponse, 
        vector<string> &vecHeaders, bool isRecvHeader = false);

    void setResponseString(string strResponse);
    void append(string strHeader);
    time_t getCreateTime() { return m_createTime; }
    string getPutBodyData() { return m_strPutBodyData; }
    string getVstoreID() { return vstoreID; }

private:
    int doRequestInner(string strUrl, REST_REQUEST_MODE requstMode, string strBodyData, 
        CRestPackage &pkgResponse,  vector<string> &vecHeaders, bool isRecvHeader = false);


    time_t m_createTime;
    string m_strDeviceIP;            // device ip
    string m_strDeviceIPPort;        // device IP port
    string m_strUserName;
    string m_strPwd;
    string m_strSessionKey;            // the sessionkey after login
    string m_strDeviceSN;              // the device SN after login
    string m_strPutBodyData;           // Construct a message body using a callback function when the PUT method is submitted

    string m_prefixUrl;                // Url request prefix /deviceManager/rest/xxx/
    string m_version;

    vector<string> *m_commonHeaders;
    bool bsendLoginOK;                // false send login failed
                                      // true send login ok

    string m_strResponse;
    bool bHaveRecvData;
    vector<string> *m_vecHeaders;
    // set up device SN
    void setDeviceSN(string strDeviceID);

    // get login info
    bool bLoginOK;
    CRestPackage m_loginPkg;
    string iBaseToken;        //Record the tag random number returned successfully by login
    string vstoreID;          //Record the vstore id returned by login
};


#endif
