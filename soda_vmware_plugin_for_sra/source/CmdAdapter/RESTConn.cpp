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

#include <time.h>
#include <sstream>
#include <algorithm> 
#include <json/reader.h>

#include "RESTConn.h"
#include "Log.h"
#include "Commf.h"
#include "common.h"
#include "curl/curl.h"

#define SSL_ERROR_RETRY_NUM 10

/* callback function
   This callback function is called by libcurl as soon as there is data received that needs to be saved. 
   ptr points to the delivered data, and the size of that data is size multiplied with nmemb.

   The callback function will be passed as much data as possible in all invokes, but you must not make any 
   assumptions. It may be one byte, it may be thousands. The maximum amount of body data that will be passed 
   to the write callback is defined in the curl.h header file: CURL_MAX_WRITE_SIZE (the usual default is 16K).
   If CURLOPT_HEADER is enabled, which makes header data get passed to the write callback, you can get up to 
   CURL_MAX_HTTP_HEADER bytes of header data passed into it. This usually means 100K.
*/
size_t writeBodyData(void *ptr, size_t size, size_t nmemb, void *stream){
    CRESTConn *restCon = (CRESTConn *)stream;
    // Size*nmemb is up to 100K
    if (!restCon){
        COMMLOG(OS_LOG_ERROR,"%s","writeBodyData (CRESTConn *)stream is NULL");
        return size * nmemb; 
    }
    
    //Initialize bodyData to prevent allocation space from failing
    char *bodyData = NULL;
    bodyData = new char[size * nmemb + 1];
    if (bodyData != NULL){
        memcpy_s(bodyData, size * nmemb + 1, ptr, size * nmemb);
        bodyData[size * nmemb] = '\0';
        restCon->setResponseString(bodyData);
        delete[] bodyData;

        return size * nmemb;
    }

    COMMLOG(OS_LOG_ERROR,"%s","writeBodyData bodyData is NULL");
    return 0;
}

size_t writeHeaderData(const void *ptr, size_t size, size_t nmemb, void *stream){
    CRESTConn *restCon = (CRESTConn *)stream;
    if (!restCon){
        COMMLOG(OS_LOG_ERROR,"%s","writeHeaderData (CRESTConn *)stream is NULL");
        return size * nmemb;
    }

    char *headData = NULL;
    headData = new char[size * nmemb + 1];
    if (headData != NULL){
        memcpy_s(headData, size * nmemb + 1, ptr, size * nmemb);
        headData[size * nmemb] = '\0';
        restCon->append(headData);
        delete[] headData;

        return size * nmemb;
    }

    COMMLOG(OS_LOG_ERROR,"%s","writeHeaderData headData is NULL");
    return 0;
}

size_t writeHeaderStripData(const void *ptr, size_t size, size_t nmemb, void *stream){
    CRESTConn *restCon = (CRESTConn *)stream;
    if (!restCon){
        COMMLOG(OS_LOG_ERROR,"%s","writeHeaderData (CRESTConn *)stream is NULL");
    }
    return size * nmemb;
}

/* callback function
This callback function is called by libcurl as soon as it needs to read data in order to send it to 
the peer - like if you ask it to upload or post data to the server. The data area pointed at by the 
pointer ptr should be filled up with at most size multiplied with nmemb number of bytes by your function.

Your function must then return the actual number of bytes that it stored in that memory area. 
Returning 0 will signal end-of-file to the library and cause it to stop the current transfer.
*/
static size_t readCallback(void *ptr, size_t size, size_t nmemb, void *stream){
    CRESTConn *restCon = (CRESTConn *)stream;
    if (restCon == NULL){
        COMMLOG(OS_LOG_ERROR,"%s","writeHeaderData (CRESTConn *)stream is NULL");
        return size * nmemb; 
    }

    string bodyData = restCon->getPutBodyData();
    if (bodyData.length() > nmemb * size){
        COMMLOG(OS_LOG_ERROR, "readCallback the len of bodyData[%d] > [%d] ", bodyData.length(), nmemb * size);
        return 0; 
    }

    errno_t rst = memcpy_s(ptr, bodyData.length(), bodyData.c_str(), bodyData.length());
    if (rst != EOK){
         COMMLOG(OS_LOG_ERROR, "memcpy_s failed ");
         return 0; 
    }

    return bodyData.length();
}

CRESTConn::CRESTConn(std::string strDeviceIP, std::string strUserName, std::string strPwd)
    : CONTEXT_PATH("/deviceManager"), FIELDNAME_DEVICEID("deviceid"), FIELDNAME_COOKIES("SET-COOKIE"),FILEDNAME_IBASETOKEN("iBaseToken"), bHaveRecvData(false), vstoreID("----"){
    // Initialize the creation time. If it is more than 10 minutes to prevent the session from expiring, you need to log out RESTCONN and regenerate a conn.
    memset_s(&m_createTime, sizeof(m_createTime), 0, sizeof(m_createTime));
    time(&m_createTime);

    // define pointer avoid 4251 waring
    m_commonHeaders = new vector<string>();
    m_vecHeaders = new vector<string>();

    m_strDeviceIP = strDeviceIP;
    m_strDeviceIPPort = strDeviceIP;
    if (strDeviceIP.find(":") == string::npos){
        m_strDeviceIPPort = strDeviceIP + ":8088";
    }

    m_strUserName = strUserName;
    m_strPwd = strPwd;

    // any string is ok when login
    setDeviceSN("xxxx");
    m_strSessionKey = "";

    // init common header
    ostringstream oss;
    oss.str("");
    oss <<"Host:" <<m_strDeviceIP;
    m_commonHeaders->push_back(oss.str());
    m_commonHeaders->push_back("Content-Type:application/json");
    m_commonHeaders->push_back("Connection:keep-alive");
    m_commonHeaders->push_back("Accept-Language:en");

    bLoginOK = true;
    bsendLoginOK = true;

    string strResponse;
    oss.str("");
    oss <<"{\"username\":\"" <<m_strUserName <<"\",\"password\":\"" <<m_strPwd 
        <<"\",\"scope\":\"0\"}"; // scope:0 local user/ 1 domain user

    CRestPackage pkgLogin;
    vector<string> vHeaders;
    // login
    int res = doRequest("/sessions", REST_REQUEST_MODE_POST, oss.str(), pkgLogin, vHeaders, true);
    COMMLOG(OS_LOG_INFO, "login [%s] session %d", m_strDeviceIP.c_str(), res);
    if (res == 0){
        bsendLoginOK = true;
        if (pkgLogin.errorCode() != 0 || pkgLogin.count() == 0){
            bLoginOK = false;
            m_loginPkg = pkgLogin;
            COMMLOG(OS_LOG_ERROR, "login failed: errorCode=%d, desc=%s \n", pkgLogin.errorCode(), pkgLogin.description().c_str());
            return;
        }
        
        bLoginOK = true;
        // set sessionkey and device id
        setDeviceSN(pkgLogin[0][FIELDNAME_DEVICEID].asString());

        iBaseToken=pkgLogin[0][FILEDNAME_IBASETOKEN].asString();
        if( !iBaseToken.empty()){
            m_commonHeaders->push_back("iBaseToken:"+iBaseToken);
        }

        if(pkgLogin[0].isMember("vstoreId")){
            vstoreID = pkgLogin[0]["vstoreId"].asString();
        }
        
        string strUpper;
        for (vector<string>::const_iterator iter = vHeaders.begin(); iter != vHeaders.end(); ++iter){
            //cookie formate  Set-Cookie: JSESSIONID=303717255084662CA009A3B19EEF238A; Path=/deviceManager; Secure; HttpOnly
            strUpper = (*iter).substr(0, FIELDNAME_COOKIES.length());
            (void)transform(strUpper.begin(), strUpper.end(), strUpper.begin(), ::toupper);
            if (strUpper.compare(FIELDNAME_COOKIES) == 0){
                strUpper = *iter;
                size_t idxColon = strUpper.find(':');
                size_t idxSemi    = strUpper.find(';');
                
                string sessionKey = strUpper.substr(idxColon + 1, idxSemi - idxColon);
                oss.str("");
                oss <<"Cookie:" <<sessionKey;
                m_commonHeaders->push_back(oss.str());

                break;
            }
        }    
    }
    else{
        bLoginOK = false;
        m_loginPkg = pkgLogin;
        bsendLoginOK = false;
    }
}

std::string trimSpace(std::string s) {
    if (s.empty()) {
        return s;
    }
    string character = "";
    for (int i = 0; i < 33; i++) {
        character += char(i);
    }
    character += char(127);
    s.erase(0, s.find_first_not_of(character));
    s.erase(s.find_last_not_of(character) + 1);
    return s;
}

CRESTConn::CRESTConn(string strDeviceIP, string strUserName, string strPwd, bool isFusionStorage)
    :  FIELDNAME_COOKIES("SET-COOKIE"),FILEDNAME_XAUTHTOKEN("X-AUTH-TOKEN"), CONTEXT_PATH_FUSIONSTORAGE("/dsware/service"), bHaveRecvData(false), vstoreID("----"){
    //Initialize the creation time. If it is more than 10 minutes to prevent the session from expiring, you need to log out RESTCONN and regenerate a conn.
    memset_s(&m_createTime, sizeof(m_createTime), 0, sizeof(m_createTime));
    time(&m_createTime);

    // define pointer avoid 4251 waring
    m_commonHeaders = new vector<string>();
    m_vecHeaders = new vector<string>();

    m_strDeviceIP = strDeviceIP;
    m_strDeviceIPPort = strDeviceIP;
    if (strDeviceIP.find(":") == string::npos){
        m_strDeviceIPPort = strDeviceIP + ":28443";
    }
    /*lint -e539*/
    m_strUserName = strUserName;
    m_strPwd = strPwd;

    // any string is ok when login
    m_prefixUrl = CONTEXT_PATH_FUSIONSTORAGE + "/rest/version";
    m_strSessionKey = "";

    // init common header
    ostringstream oss;
    oss.str("");
    oss <<"Host:" <<m_strDeviceIP;
    m_commonHeaders->push_back(oss.str());
    m_commonHeaders->push_back("Content-Type:application/json");
    m_commonHeaders->push_back("Connection:keep-alive");
    m_commonHeaders->push_back("Accept-Language:en");

    oss.str("");
    oss <<"Referer:https://" << m_strDeviceIPPort << CONTEXT_PATH_FUSIONSTORAGE << "/";
    m_commonHeaders->push_back(oss.str());

    bLoginOK = true;
    bsendLoginOK = true;

    CRestPackage pkgLogin;
    vector<string> vHeaders;
    int res = doRequest("", REST_REQUEST_MODE_GET, "", pkgLogin, vHeaders, true);

    Json::Value responseObjects;
    pkgLogin.getResponseObjects(responseObjects);

    if (0 != responseObjects["result"].asInt()){
        COMMLOG(OS_LOG_ERROR, "get the version failed, the result is [%d].", responseObjects["result"].asInt());
        return;
    }

    m_version = responseObjects["currentVersion"].asString();
    m_prefixUrl = CONTEXT_PATH_FUSIONSTORAGE + "/" + m_version;

    COMMLOG(OS_LOG_INFO, "the version is [%s]", m_version.c_str());

    string strResponse;
    oss.str("");
    oss <<"{\"userName\":\"" <<m_strUserName <<"\",\"password\":\"" <<m_strPwd 
        <<"\",\"scope\":\"0\"}"; // scope:0 local user/ 1 domain user
    
    // login
    vHeaders.clear();
    res = doRequest("/sec/login", REST_REQUEST_MODE_POST, oss.str(), pkgLogin, vHeaders, true);
    COMMLOG(OS_LOG_INFO, "login [%s] session %d", m_strDeviceIP.c_str(), res);

    responseObjects.clear();
    pkgLogin.getResponseObjects(responseObjects);

    if (res == 0){
        bsendLoginOK = true;
        if (responseObjects["result"].asInt() != 0){
            bLoginOK = false;
            m_loginPkg = pkgLogin;
            COMMLOG(OS_LOG_ERROR, "login failed: errorCode=%d, desc=%s \n", responseObjects["result"].asInt(), responseObjects["description"].asString().c_str());
            return;
        }

        bLoginOK = true;
        string strUpper;
        for (vector<string>::const_iterator iter = vHeaders.begin(); iter != vHeaders.end(); ++iter){        
            // cookie formate Set-Cookie: JSESSIONID=303717255084662CA009A3B19EEF238A; Path=/deviceManager; Secure; HttpOnly
            strUpper = (*iter).substr(0, FIELDNAME_COOKIES.length());
            (void)transform(strUpper.begin(), strUpper.end(), strUpper.begin(), ::toupper);
            if (strUpper.compare(FIELDNAME_COOKIES) == 0){
                strUpper = *iter;
                size_t idxColon = strUpper.find(':');
                size_t idxSemi    = strUpper.find(';');

                if ((idxSemi - idxColon) <= 0){
                    COMMLOG(OS_LOG_ERROR, "the SET-COOKIE  is error [%s] " , strUpper.c_str());
                    continue;
                }

                string sessionKey = strUpper.substr(idxColon + 1, idxSemi - idxColon);
                oss.str("");
                oss <<"Cookie:" <<sessionKey;
                m_commonHeaders->push_back(oss.str());
            }

            strUpper = (*iter).substr(0, FILEDNAME_XAUTHTOKEN.length());
            (void)transform(strUpper.begin(), strUpper.end(), strUpper.begin(), ::toupper);
            if (strUpper.compare(FILEDNAME_XAUTHTOKEN) == 0){
                strUpper = *iter;
                size_t idxColon = strUpper.find(':');
                if ((strUpper.length() - idxColon) <= 0){
                    COMMLOG(OS_LOG_ERROR, "the X-AUTH-TOKEN  is error [%s] " , strUpper.c_str());
                    continue;
                }

                string tokenKey = trimSpace(strUpper.substr(idxColon + 1,  strUpper.length() - idxColon));
                oss.str("");
                oss <<"x-auth-token:" << tokenKey;
                m_commonHeaders->push_back(oss.str());
            }
        }    
    }
    else{
        COMMLOG(OS_LOG_ERROR, "login [%s] session %d failed", m_strDeviceIP.c_str(), res);
        bLoginOK = false;
        m_loginPkg = pkgLogin;
        bsendLoginOK = false;
    }
}


CRESTConn::CRESTConn(const CRESTConn &conn)
        : CONTEXT_PATH("/deviceManager"), FIELDNAME_DEVICEID("deviceid"), FIELDNAME_COOKIES("SET-COOKIE"),FILEDNAME_IBASETOKEN("iBaseToken"),FILEDNAME_XAUTHTOKEN("X-AUTH-TOKEN"), CONTEXT_PATH_FUSIONSTORAGE("/dsware/service"), bHaveRecvData(false), vstoreID("----"){
    m_createTime = conn.m_createTime;
    m_strDeviceIP = conn.m_strDeviceIP;
    m_strDeviceIPPort = conn.m_strDeviceIPPort;
    m_strUserName = conn.m_strUserName;
    m_strPwd = conn.m_strPwd;
    m_strSessionKey = conn.m_strSessionKey;
    m_strDeviceSN = conn.m_strDeviceSN;
    m_strPutBodyData = conn.m_strPutBodyData;
    m_prefixUrl = conn.m_prefixUrl;
    bsendLoginOK = conn.bsendLoginOK;
    m_strResponse = conn.m_strResponse;
    bHaveRecvData = conn.bHaveRecvData;
    bLoginOK = conn.bLoginOK;
    iBaseToken = conn.iBaseToken;
    m_loginPkg = conn.m_loginPkg;
    vstoreID = conn.vstoreID;

    // define pointer avoid 4251 waring
    m_commonHeaders = new vector<string>();
    m_vecHeaders = new vector<string>();

    for(vector<string>::iterator it = conn.m_commonHeaders->begin();
        it != conn.m_commonHeaders->end(); ++it){
        m_commonHeaders->push_back(*it);
    }

    for(vector<string>::iterator it = conn.m_vecHeaders->begin();
        it != conn.m_vecHeaders->end(); ++it){
        m_vecHeaders->push_back(*it);
    }

}

CRESTConn& CRESTConn::operator =(const CRESTConn &conn){
    if (this == &conn)
        return *this;

    if (m_commonHeaders){
        m_commonHeaders->clear();        
        delete(m_commonHeaders);        
    }

    if (m_vecHeaders){
        m_vecHeaders->clear();
        delete(m_vecHeaders);
    }

    m_createTime = conn.m_createTime;
    m_strDeviceIP = conn.m_strDeviceIP;
    m_strDeviceIPPort = conn.m_strDeviceIPPort;
    m_strUserName = conn.m_strUserName;
    m_strPwd = conn.m_strPwd;
    m_strSessionKey = conn.m_strSessionKey;
    m_strDeviceSN = conn.m_strDeviceSN;
    m_strPutBodyData = conn.m_strPutBodyData;
    m_prefixUrl = conn.m_prefixUrl;
    bsendLoginOK = conn.bsendLoginOK;
    m_strResponse = conn.m_strResponse;
    bHaveRecvData = conn.bHaveRecvData;
    bLoginOK = conn.bLoginOK;
    iBaseToken = conn.iBaseToken;
    m_loginPkg = conn.m_loginPkg;
    vstoreID = conn.vstoreID;

    // define pointer avoid 4251 waring
    m_commonHeaders = new vector<string>();
    m_vecHeaders = new vector<string>();

    for(vector<string>::iterator it = conn.m_commonHeaders->begin();
        it != conn.m_commonHeaders->end(); ++it){
        m_commonHeaders->push_back(*it);
    }

    for(vector<string>::iterator it = conn.m_vecHeaders->begin();
        it != conn.m_vecHeaders->end(); ++it){
        m_vecHeaders->push_back(*it);
    }

    return *this;
}

CRESTConn::~CRESTConn(){
    int iRet = RETURN_OK;
    try{
        CRestPackage pkgLogout;
        // logout
        if (g_bFusionStorage){
            iRet = doRequest("/sec/logout", REST_REQUEST_MODE_POST, "", pkgLogout);
        }
        else{
            iRet = doRequest("/sessions", REST_REQUEST_MODE_DELETE, "", pkgLogout);
        }

        COMMLOG(OS_LOG_INFO, "logout [%s] session %d", m_strDeviceIP.c_str(), iRet);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "logout failed: errorCode=%d", iRet);
        }

        if (RETURN_OK != pkgLogout.errorCode()){
            COMMLOG(OS_LOG_ERROR, "logout failed: errorCode=%d, desc=%s ", pkgLogout.errorCode(), pkgLogout.description().c_str());
        }

        m_commonHeaders->clear();
        m_vecHeaders->clear();
        delete(m_commonHeaders);
        delete(m_vecHeaders);
    }
    catch(...){}

    m_commonHeaders = NULL;
    m_vecHeaders = NULL;
}

void CRESTConn::setDeviceSN(std::string strDeviceID){
    ostringstream oss;
    oss.str("");
    m_strDeviceSN = strDeviceID;
    oss <<CONTEXT_PATH <<"/rest/" <<m_strDeviceSN;
    m_prefixUrl = oss.str();
}

int CRESTConn::doRequest(string strUrl, REST_REQUEST_MODE requstMode, string strBodyData, CRestPackage &pkgResponse){
    int retryNum = 0;
    //Sending a login command fails directly
    if (!bsendLoginOK){
        pkgResponse = m_loginPkg;
        return ERROR_CONNECTSERVER_CODE_LIBCURL;
    }

    //Send login command succeeded, but there is error code
    if (!bLoginOK){
        // return login package
        pkgResponse = m_loginPkg;
        return RETURN_OK;
    }

    vector<string> vecHeaders;
    int iRes = doRequestInner(strUrl, requstMode, strBodyData, pkgResponse, vecHeaders);

    if (CURLE_SSL_CONNECT_ERROR == iRes){
        // retry 10 times
        while (SSL_ERROR_RETRY_NUM > retryNum){
            ++retryNum;
            COMMLOG(OS_LOG_ERROR, "SSL connect error, retry num %d", retryNum);
            iRes = doRequestInner(strUrl, requstMode, strBodyData, pkgResponse, vecHeaders);

            if (CURLE_SSL_CONNECT_ERROR != iRes){
                break;
            }
        }
    }

    return iRes;
}


int CRESTConn::doRequest(string strUrl, REST_REQUEST_MODE requstMode,  string strBodyData, CRestPackage &pkgResponse, 
                         vector<string> &vecHeaders, bool isRecvHeader){
    int retryNum = 0;
    //Sending a login command fails directly
    if (!bsendLoginOK){
        pkgResponse = m_loginPkg;
        return ERROR_CONNECTSERVER_CODE_LIBCURL;
    }

    //Send login command succeeded, but there is error code
    if (!bLoginOK){
        // return login package
        pkgResponse = m_loginPkg;
        return RETURN_OK;
    }

    int iRes = doRequestInner(strUrl, requstMode, strBodyData, pkgResponse, vecHeaders, isRecvHeader);

    if (CURLE_SSL_CONNECT_ERROR == iRes){
        // retry 10 times
        while (SSL_ERROR_RETRY_NUM > retryNum){
            ++retryNum;
            COMMLOG(OS_LOG_ERROR, "SSL connect error, retry num %d", retryNum);
            iRes = doRequestInner(strUrl, requstMode, strBodyData, pkgResponse, vecHeaders, isRecvHeader);

            if (CURLE_SSL_CONNECT_ERROR != iRes){
                break;
            }
        }
    }

    return iRes;
}

/*------------------------------------------------------------
Function Name: doRequest()
Description  : send request
Data Accessed:
Data Updated : None.
Input        : strUrl, vecHeaders, strBodyData
Output       : strResponse
Return       : -1 error, 0 OK
Call         :
Called by    :
Create By    : 
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTConn::doRequestInner(string strUrl, REST_REQUEST_MODE requstMode, string strBodyData, CRestPackage &pkgResponse, 
                              vector<string> &vecHeaders, bool isRecvHeader){
    //COMMLOG(OS_LOG_DEBUG, "doRequest url [%s] ip=%s", strUrl.c_str(), m_strDeviceIPPort.c_str());
    CURL *hCurl;
    CURLcode Lcode;
    // init curl context
    Lcode = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(CURLE_OK != Lcode){
        COMMLOG(OS_LOG_ERROR,"init curl error [%d]",Lcode);
    }
    hCurl = curl_easy_init();
    // error
    if (!hCurl){
        COMMLOG(OS_LOG_ERROR,"%s","doRequest error: hCurl is NULL");
        return RETURN_ERR;
    }

    // set url
    ostringstream streamTmp;
    streamTmp.str("");
    if (strUrl == FUSION_URL_STRETCH){
        streamTmp << "https://" << m_strDeviceIPPort << strUrl;
    }
    else{
        streamTmp << "https://" << m_strDeviceIPPort << m_prefixUrl << strUrl;
    }
    
    curl_easy_setopt(hCurl, CURLOPT_URL, streamTmp.str().c_str());
    if (strUrl.compare("/sessions")!=0){
        COMMLOG(OS_LOG_DEBUG, "doRequest url [%s],request mode[%d] ", streamTmp.str().c_str(),requstMode);
    }
    else{
        COMMLOG(OS_LOG_DEBUG, "doRequest url [%s],request mode[%d] ", streamTmp.str().c_str(),requstMode);
    }
    curl_easy_setopt(hCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(hCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(hCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(hCurl,CURLOPT_CONNECTTIMEOUT,150L);
    // set write data function
    curl_easy_setopt(hCurl, CURLOPT_WRITEFUNCTION, writeBodyData);
    curl_easy_setopt(hCurl, CURLOPT_HEADER, 0L);

    // set headers
    struct curl_slist *headers = NULL;
    // set common header
    for (vector<string>::const_iterator iter = m_commonHeaders->begin(); iter != m_commonHeaders->end(); ++iter){
        headers = curl_slist_append(headers, iter->c_str());
    
    }

    // set requset header
    for (vector<string>::const_iterator iter = vecHeaders.begin(); iter != vecHeaders.end(); ++iter){
        headers = curl_slist_append(headers, iter->c_str());
    }
    curl_easy_setopt(hCurl, CURLOPT_HTTPHEADER, headers);
    

    // set body data
    curl_easy_setopt(hCurl, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(hCurl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(hCurl, CURLOPT_CONNECTTIMEOUT, 150);

    if (isRecvHeader){
        curl_easy_setopt(hCurl, CURLOPT_HEADERFUNCTION, writeHeaderData);
    }
    else{
        curl_easy_setopt(hCurl, CURLOPT_HEADERFUNCTION, writeHeaderStripData);
    }

    // set request mode
    switch (requstMode){
    case REST_REQUEST_MODE_GET:
        curl_easy_setopt(hCurl, CURLOPT_HTTPGET, 1L);
        break;
    case REST_REQUEST_MODE_POST:
        curl_easy_setopt(hCurl, CURLOPT_POST, 1L);
        curl_easy_setopt(hCurl, CURLOPT_POSTFIELDS, strBodyData.c_str());
        break;
    case REST_REQUEST_MODE_PUT:
        curl_easy_setopt(hCurl, CURLOPT_PUT, 1L);
        // Put mode special handling
        this->m_strPutBodyData = strBodyData;
        curl_easy_setopt(hCurl, CURLOPT_READDATA, this);
        curl_easy_setopt(hCurl, CURLOPT_READFUNCTION, readCallback);
        curl_easy_setopt(hCurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)(strBodyData.length()));
        break;
    case REST_REQUEST_MODE_DELETE:
        curl_easy_setopt(hCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    default:
        COMMLOG(OS_LOG_ERROR, "REST_REQUEST_MODE error: request type=%d \n", requstMode);
        return -1;
    }

    // set error buffer
    char errorBuffer[CURL_ERROR_SIZE] = {0};
   
    curl_easy_setopt(hCurl, CURLOPT_ERRORBUFFER, errorBuffer);

    CURLcode res = curl_easy_perform(hCurl);
    /* Check for errors */ 
    if (res != CURLE_OK){
        COMMLOG(OS_LOG_ERROR, "url=[%s] curl_easy_perform() failed: code=%d \n%s\n%s\n", strUrl.c_str(), 
            res, curl_easy_strerror(res), errorBuffer);
        
        // get header data
        if (isRecvHeader){
            vecHeaders.assign(m_vecHeaders->begin(), m_vecHeaders->end());
            m_vecHeaders->clear();
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(hCurl);
        curl_global_cleanup();
        m_strResponse.clear();
        bHaveRecvData = false;
        return res;
    }
    
    if (pkgResponse.decode(m_strResponse) != 0){
        COMMLOG(OS_LOG_ERROR, "pkgResponse.decode() failed.\n%s\n", m_strResponse.c_str());
    }

    // get header data
    if (isRecvHeader){
        vecHeaders.assign(m_vecHeaders->begin(), m_vecHeaders->end());
        m_vecHeaders->clear();
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(hCurl);
    curl_global_cleanup();
    m_strResponse.clear();
    bHaveRecvData = false;

    return 0;
}

void CRESTConn::setResponseString(string strResponse){
    m_strResponse = m_strResponse + strResponse;
}

void CRESTConn::append(string strHeader){
    m_vecHeaders->push_back(strHeader);
}
