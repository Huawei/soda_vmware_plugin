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
#include "CmdRESTAdapter.h"
#include "RESTPackage.h"
#include "RESTConn.h"

//Parse the list string according to Json data
bool jsonValue2Array(Json::Value &jsonObject, list<string> &lRstArray)
{
    if (!jsonObject.isString()){
        return false;
    }

    string strTmp = jsonObject.asString();
    strTmp = strTmp.substr(1, strTmp.length() - 2);
    lRstArray.clear();
    OS_StrToken(strTmp, ",", &lRstArray);

    for (list<string>::iterator iter = lRstArray.begin(); iter != lRstArray.end(); ++iter){
        if ((*iter)[0] == '"')
            *iter = iter->substr(1, iter->length() - 2);
    }
    return true;
}

// Convert type, only for json converted to unsigned long long, long long, int, uint, bool
template<typename T>
T jsonValue2Type(Json::Value &jsonObject)
{
    stringstream ss;
    string strTmp = jsonObject.toStyledString();

    if (strTmp[0] == '\"'){
        ss <<jsonObject.asString();
    }
    else{
        ss <<strTmp;
    }

    T rstValue;
    const char *pTemp = typeid(rstValue).name();
    if(pTemp){
        string typeName = pTemp;
        if (jsonObject == Json::nullValue){

#ifdef WIN32
            if (typeName.compare("int") == 0 || typeName.compare("unsigned int") == 0){
                return (T)-1;
            }
#else
            if (typeName.compare("i") == 0 || typeName.compare("j") == 0){
                return (T)-1;
            }
#endif
        }
    }
    ss >>rstValue;
    return rstValue;
}

CRESTCmd::CRESTCmd(const string& strSN)
    : CCmdAdapter(strSN)
{
    m_connList.clear();
}

CRESTCmd::~CRESTCmd()
{
    try{
        for (list<pair<string, CRESTConn *>>::iterator iter = m_connList.begin();
            iter != m_connList.end(); ++iter){
            delete(iter->second);
            iter->second = NULL;
        }
        m_connList.clear();
    }
    catch(...){}
}

CRESTConn *CRESTCmd::getConn(std::string &strDeviceIP, string &strUserName, string &strPwd)
{
    for (list<pair<string, CRESTConn *>>::iterator iter = m_connList.begin();
        iter != m_connList.end(); ++iter){
        if ((strDeviceIP.compare(iter->first) == 0)){
            time_t now(NULL);
            time(&now);

            //If you create CRESTConn and login for more than the specified time (10 minutes), log out the original saved session and login again.
            if ((int)difftime(now, iter->second->getCreateTime()) > SESSION_INVALID_TIME){
                delete(iter->second);
                if (g_bFusionStorage){
                    iter->second = new CRESTConn(strDeviceIP, strUserName, strPwd, true);
                }
                else{
                    iter->second = new CRESTConn(strDeviceIP, strUserName, strPwd);
                }
            }

            return iter->second;
        }
    }

    CRESTConn *newConn;
    if (g_bFusionStorage){
        newConn = new CRESTConn(strDeviceIP, strUserName, strPwd, true);
    }
    else{
        newConn = new CRESTConn(strDeviceIP, strUserName, strPwd);
    }

    m_connList.push_back(make_pair(strDeviceIP, newConn));

    return newConn;
}

/*------------------------------------------------------------
Function Name: InitConnectInfo()
Description  : Initialize array connection information
Data Accessed:
Data Updated : None.
Input        : 
Output       : 
Return       :
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::InitConnectInfo(const string& strSN)
{
    HYPER_STORAGE_STRU stHyperStorage;
    TLV_LOGIN_INFO_STRU stTlvLoginInfo;
    
    stTlvLoginInfo.strArrayUser = stHyperStorage.strArrayUser;
    stTlvLoginInfo.strArrayPwd = stHyperStorage.strArrayPasswd;

    SetLoginInfo(stTlvLoginInfo);    
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: ConfigConnectInfo()
Description  : Initialize the connection
Data Accessed: None.
Data Updated : None.
Input        :
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::ConfigConnectInfo(const string& strSN,const string& strIP, const string& strUser, const string& strPwd)
{
    TLV_LOGIN_INFO_STRU stTlvLoginInfo;
    if (strIP.empty()){
        return RETURN_ERR;
    }
    
    stTlvLoginInfo.lstArrayIP.clear();
    OS_StrToken(strIP, ";", &stTlvLoginInfo.lstArrayIP);
    stTlvLoginInfo.strArrayUser = strUser;
    stTlvLoginInfo.strArrayPwd = strPwd;
    
    SetLoginInfo(stTlvLoginInfo);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: TimeChg()
Description  : Time conversion function.
Data Accessed: None.
Data Updated : None.
Input        : ullTime:unsigned long longformat time.
Output       : strTime:stringformat time.
Return       : None.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::TimeChg(IN unsigned long long ullTime,OUT string &strTime)
{
    string str = OS_Time_tToStr((long long)ullTime);
    if (TIME_INVALID_VALUE == str){
        strTime = "--";
    } 
    else{
        strTime = str;
    }
}

/*------------------------------------------------------------
Function Name: CMD_showarrayinfo()
Description  : query remote devices.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstRemoteArrayInfo:remote devices.
Return       : return RETURN_OK if success, otherwise return RETURN_ERR.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_REMOTE_DEVICE, REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_REMOTE_DEVICE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_REMOTE_DEVICE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        rlstRemoteArrayInfo.clear();
        for (unsigned i = 0; i < restPkg.count(); ++i){
            string strarraytype = "";
            if (restPkg[i].isMember(REMOTEDEVICE_TAG_ARRAYTYPE)){
                strarraytype = restPkg[i][REMOTEDEVICE_TAG_ARRAYTYPE].asString();
            }
            if (strarraytype != "1"){
                continue;
            }
            if (restPkg[i][REMOTEDEVICE_TAG_SN].asString().empty()){
                continue;
            }

            REMOTE_ARRAY_STRU stArrayInfo;
            stArrayInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
            stArrayInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
            stArrayInfo.uiRunState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
            stArrayInfo.strWWN = restPkg[i][REMOTEDEVICE_TAG_WWN].asString();
            stArrayInfo.strSN = restPkg[i][REMOTEDEVICE_TAG_SN].asString();
            rlstRemoteArrayInfo.push_back(stArrayInfo);
        }
        return RETURN_OK;
    }
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showarraystretched()
Description  : Query whether the remote device supports dual-active
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstRemoteArrayInfo:Remote device information.
Return       : Success or failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStrecthed)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_ACTIVE_LICENSE, REST_REQUEST_MODE_GET, "", restPkg);
        
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_ACTIVE_LICENSE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_ACTIVE_LICENSE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        rlstRemoteArrayStrecthed.clear();
        for (unsigned i = 0; i < restPkg.count(); ++i){
            unsigned j = 0;
            REMOTE_ARRAY_STRU stArrayInfo;
            CISMConfig oConfig;

    
            Json::Value strStretched = restPkg[0][COMMON_TAG_LICENSEUSAGEINFO];
            for (j = 0; j < strStretched.size(); j++){
                if (0 == (strStretched[j][COMMON_TAG_LICENSE_FEATUREID].compare(HYPERMETROPAIR_TAG_LICENSE_INDEX))){
                    if ((0 == strStretched[j][COMMON_TAG_LICENSE_OPENSTATUS].compare(HYPERMETROPAIR_TAG_LICENSE_OPENSTATUS))
                        && (strStretched[j][COMMON_TAG_LICENSE_STATE].compare(HYPERMETROPAIR_TAG_LICENSE_STATE) != 0)){
                        if (g_bstretch == true){
                            stArrayInfo.strStretched = HM_SUPPORT_STRETCHED;
                            g_bstretch = true;
                            COMMLOG(OS_LOG_INFO, "Check array (ip [%s]) license, is support stretched, "
                                "and config.txt is also support stretched.", iter->c_str());
                        }
                        else{
                            stArrayInfo.strStretched = HM_SUPPORT_NOT_STRETCHED;
                            g_bstretch = false;
                            COMMLOG(OS_LOG_WARN, "Check array (ip [%s]) license, is support stretched, "
                                "but config.txt is not support stretched. so not support stretched", iter->c_str());
                        }
                        break;
                    }
                    else{
                        if (g_bstretch == true){
                            COMMLOG(OS_LOG_WARN, "Check array (ip [%s]) license, is not support stretched, "
                                "config.txt is support stretched. so not support stretched.", iter->c_str());
                        }
                        else{
                            COMMLOG(OS_LOG_WARN, "Check array (ip [%s]) license, is not support stretched, "
                                "and config.txt is also not support stretched.", iter->c_str());
                        }
                        stArrayInfo.strStretched = HM_SUPPORT_NOT_STRETCHED;
                        g_bstretch = false;
                        break;
                    }
                }
            }
            if (j == strStretched.size()){
                stArrayInfo.strStretched = HM_SUPPORT_NOT_STRETCHED;
                if (g_bstretch == true){
                    g_bstretch = false;
                    COMMLOG(OS_LOG_WARN, "Check array (ip [%s]) license, is not support stretched, "
                        "config.txt is support stretched. so not support stretched.", iter->c_str());
                }
                else{
                    g_bstretch = false;
                    COMMLOG(OS_LOG_WARN, "Check array (ip [%s]) license, is not support stretched, "
                        "and config.txt is also not support stretched.", iter->c_str());
                }
            }
            rlstRemoteArrayStrecthed.push_back(stArrayInfo);
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showsys()
Description  : Query system information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rstSysInfo:system information.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SYSTEM, REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SYSTEM, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SYSTEM, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count!=1.", strIP.c_str(), RESTURL_SYSTEM);
        if (RETURN_OK != iRet){
            return iRet;
        }
        else{
            return RETURN_ERR;
        }
    }

    rstSysInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstSysInfo.strDeviceSerialNumber = restPkg[0][COMMON_TAG_ID].asString();
    rstSysInfo.uiProductMode = jsonValue2Type<unsigned int>(restPkg[0][SYSTEM_TAG_PRODUCTMODE]);
    rstSysInfo.strLocation = restPkg[0][COMMON_TAG_LOCATION].asString();
    rstSysInfo.strProductVersion = restPkg[0][SYSTEM_TAG_PRODUCTVERSION].asString();
    rstSysInfo.strWWN = restPkg[0][SYSTEM_TAG_WWN].asString();
    rstSysInfo.patchVersion = "";
    if(restPkg[0].isMember(SYSTEM_PATCH_VERSION)){
        rstSysInfo.patchVersion = restPkg[0][SYSTEM_PATCH_VERSION].asString();
    }
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showarraymodelinfo()
Description  : Get array model information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rstStorageInfo.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showarraymodelinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin();
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SYSTEM, REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SYSTEM, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SYSTEM, iRet, restPkg.description().c_str());
            return iRet;
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count[%d]!=1.", strIP.c_str(), RESTURL_SYSTEM, restPkg.count());
        if (RETURN_OK != iRet){
            return iRet;
        }
        else{
            return RETURN_ERR;
        }
    }

    unsigned int uiMode = jsonValue2Type<unsigned int>(restPkg[0][SYSTEM_TAG_PRODUCTMODE]);
    CMD_translatemodel(uiMode, rstStorageInfo.strArrayModel);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showtlvsysinfo()
Description  : Query TLV system information.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : rstStorageInfo:Returned TLV system information.
               strErrorMsg:Error message
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg)
{
    int iRet = RETURN_OK;

    iRet = CMD_showarraymodelinfo(rstStorageInfo);
    if (iRet != RETURN_OK){
        return iRet;
    }

    SYSTEM_INFO_STRU rstSysInfo;    
    iRet = CMD_showsys(rstSysInfo);
    if (iRet != RETURN_OK){
        return iRet;
    }

    rstStorageInfo.strArrayName = rstSysInfo.strName;
    rstStorageInfo.strArraySN = rstSysInfo.strDeviceSerialNumber;
    rstStorageInfo.iCommuType = COMMUNICATION_TYPE_REST;
    rstStorageInfo.strArrayVersion = rstSysInfo.strProductVersion;

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_createhyimg()
Description  : Create a snapshot.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotName:snapshot name.
               strSrcLUNID:lun id.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_createhyimg(IN string &strSnapshotName, IN string &strSrcLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_SNAPSHOT;
    jSendData[COMMON_TAG_NAME] = strSnapshotName;
    jSendData[COMMON_TAG_PARENTTYPE] = OBJ_LUN;
    jSendData[COMMON_TAG_PARENTID] = strSrcLUNID;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SNAPSHOT, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SNAPSHOT, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SNAPSHOT, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_createFShyimg()
Description  : create snapshot for fs.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotName:snapshot name.
               strSrcFSID:FS ID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_createFShyimg(IN string &strSnapshotName, IN string &strSrcFSID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_FSSNAPSHOT;
    jSendData[COMMON_TAG_NAME] = strSnapshotName;
    jSendData[COMMON_TAG_PARENTTYPE] = OBJ_FILESYSTEM;
    jSendData[COMMON_TAG_PARENTID] = strSrcFSID;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_FSSNAPSHOT, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_FSSNAPSHOT, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_FSSNAPSHOT, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        
        return RETURN_OK;
    }
    
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_delhyimg()
Description  : delete snapshot.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotID: snapshot ID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_delhyimg(IN string &strSnapshotID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_SNAPSHOT <<"/" <<strSnapshotID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                *iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_delFShyimg()
Description  : delete snapshot for fs.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotID:snapshot ID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_delFShyimg(IN string &strSnapshotID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    if (strSnapshotID.empty()){
        COMMLOG(OS_LOG_ERROR, "CMD_delFShyimg strSnapshotID %s", strSnapshotID.c_str());
        return RETURN_ERR;
    }

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_FSSNAPSHOT <<"/" <<strSnapshotID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                *iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_activehyimg()
Description  : active snapshot.
Data Accessed: None.
Data Updated : None.
Input        : rstrSnapshotID:snapshot ID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_activehyimg(IN string rstrSnapshotID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    string strSnapshotID = "";
    strSnapshotID += rstrSnapshotID + ",";
    if(strSnapshotID.size() != 0){
        strSnapshotID = strSnapshotID.substr(0,strSnapshotID.size()-1);
    }

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    Json::Value jSendData;
    Json::Value iDlist;
    iDlist.append(strSnapshotID);
    
    jSendData[SNAPSHOTBATCHOPERATION_TAG_SNAPSHOTLIST] = iDlist;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_ACITVIE_HYPERIMAGE, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_ACITVIE_HYPERIMAGE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_ACITVIE_HYPERIMAGE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_activehyimg()
Description  : Activate multiple snapshots.
Data Accessed: None.
Data Updated : None.
Input        : rlstSnapshotID:Snapshot ID (up to 64)
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_activehyimg(IN list<string> &rlstSnapshotID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    string strSnapshotID = "";
    list<string>::iterator itSnapshotID;

    for (itSnapshotID = rlstSnapshotID.begin(); itSnapshotID != rlstSnapshotID.end(); itSnapshotID++){
        HYIMAGE_INFO_STRU stHyImageInfo;

        strSnapshotID += *itSnapshotID;

        strSnapshotID += ",";
    }

    if(strSnapshotID.size() != 0){
        strSnapshotID = strSnapshotID.substr(0,strSnapshotID.size()-1);
    }

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[SNAPSHOTBATCHOPERATION_TAG_SNAPSHOTLIST] = "[" + strSnapshotID + "]";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_ACITVIE_HYPERIMAGE, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_ACITVIE_HYPERIMAGE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_ACITVIE_HYPERIMAGE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_recreatehyimg()
Description  : Recreate snapshot.
Data Accessed: None.
Data Updated : None.
Input        : rlstSnapshotID:Snapshot ID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_recreatehyimg(IN string &strSnapshotID)
{
    int iRet = CMD_disablehyimg(strSnapshotID);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "CMD_disablehyimg failed, error=%d", iRet);
        return RETURN_ERR;
    }

    iRet = CMD_activehyimg(strSnapshotID);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "CMD_activehyimg failed, error=%d", iRet);
        return RETURN_ERR;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_recreatehyimg()
Description  : Recreate multiple snapshot.
Data Accessed: None.
Data Updated : None.
Input        : rlstSnapshotID:Snapshot ID (up to 64).
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_recreatehyimg(IN list<string> &rlstSnapshotID)
{
    CRestPackage restPkg;
    string strEmpty = "";
    ostringstream oss;
    oss.str("");

    string strSnapshotID = "";
    list<string>::iterator itSnapshotID;

    for (itSnapshotID = rlstSnapshotID.begin(); itSnapshotID != rlstSnapshotID.end(); itSnapshotID++){
        HYIMAGE_INFO_STRU stHyImageInfo;

        strSnapshotID += *itSnapshotID;

        strSnapshotID += ",";
    }

    if(strSnapshotID.size() != 0){
        strSnapshotID = strSnapshotID.substr(0,strSnapshotID.size()-1);
    }
    
    return CMD_recreatehyimg(strSnapshotID);
}

/*------------------------------------------------------------
Function Name: CMD_disablehyimg()
Description  : Disable snapshots.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotID:Snapshot ID 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_disablehyimg(IN string &strSnapshotID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strSnapshotID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_DISABLE_HPERIMAGE, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_DISABLE_HPERIMAGE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_DISABLE_HPERIMAGE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhyimginfo()
Description  : Query snapshot information
Data Accessed: None.
Data Updated : None.
Input        : rstHyImageInfo:Snapshot information.
Output       : rstHyImageInfo:Snapshot information.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strWWN, strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss <<RESTURL_SNAPSHOT <<"/" <<rstHyImageInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstHyImageInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
    rstHyImageInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstHyImageInfo.uiSnapType = jsonValue2Type<unsigned int>(restPkg[0][SNAPSHOT_TAG_SNAPSHOTUSER]);
    SnapshotConvertType(rstHyImageInfo.uiSnapType);

    rstHyImageInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstHyImageInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    SnapshotConvertStatus(rstHyImageInfo.uiStatus,rstHyImageInfo.uiState);

    unsigned int uiRollBackProgress = jsonValue2Type<unsigned int>(restPkg[0][SNAPSHOT_TAG_ROLLBACKRATE]);
    if (0xFFFFFFFF == uiRollBackProgress){
        rstHyImageInfo.uiRollBackProgress = 100;
    }
    else{
        rstHyImageInfo.uiRollBackProgress = uiRollBackProgress;
    }

    unsigned long long ullTimeStamp = jsonValue2Type<unsigned long long>(restPkg[0][SNAPSHOT_TAG_TIMESTAMP]);
    TimeChg(ullTimeStamp,rstHyImageInfo.strTimeStamp);

    strWWN = restPkg[0][SNAPSHOT_TAG_WWN].asString();

    //Add a separator
    CMD_addFilterToWWN(strWWN, rstHyImageInfo.strWWN);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showFShyimginfo()
Description  : Query snapshot information for fs
Data Accessed: None.
Data Updated : None.
Input        : rstHyImageInfo:snapshot information.
Output       : rstHyImageInfo:snapshot information.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showFShyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strWWN, strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_FSSNAPSHOT <<"/" <<rstHyImageInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstHyImageInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
    rstHyImageInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();

    rstHyImageInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstHyImageInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    SnapshotConvertStatus(rstHyImageInfo.uiStatus,rstHyImageInfo.uiState);

    rstHyImageInfo.uiRollBackProgress = 100;

    unsigned long long ullTimeStamp = jsonValue2Type<unsigned long long>(restPkg[0][SNAPSHOT_TAG_TIMESTAMP]);
    TimeChg(ullTimeStamp,rstHyImageInfo.strTimeStamp);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhyimgoflun()
Description  : Query all snapshot information of a LUN.
Data Accessed: None.
Data Updated : None.
Input        : strLUNID:LUN ID.
Output       : lstHyImageInfo:snapshot information of a LUN.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhyimgoflun(IN string &strLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    list<string> lstHyImageID;
    list<string>::iterator itHyImageID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_LUN <<"/" <<strLUNID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    list<string> lstSnapID;
    list<string>::iterator itSnapID;
    lstSnapID.clear();

    if (!jsonValue2Array(restPkg[0][LUN_TAG_SNAPSHOTIDS], lstSnapID)){
        return RETURN_ERR;
    }
    
    for (itSnapID = lstSnapID.begin(); itSnapID != lstSnapID.end(); itSnapID++){
        string strSnapID = *itSnapID;

        lstHyImageID.push_back(strSnapID);
    }

    for (itHyImageID = lstHyImageID.begin(); itHyImageID != lstHyImageID.end(); itHyImageID++){
        HYIMAGE_INFO_STRU stHyImageInfo;

        stHyImageInfo.strID = *itHyImageID;

        iRet = CMD_showhyimginfo(stHyImageInfo);
        if (iRet != RETURN_OK){
            return iRet;
        }

        rlstHyImageInfo.push_back(stHyImageInfo);
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhyimgoffs()
Description  : Query all snapshot information of a fs.
Data Accessed: None.
Data Updated : None.
Input        : strFSID:FS ID.
Output       : lstHyImageInfo:all snapshot information of a fs.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhyimgoffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    list<string> lstHyImageID;
    list<string>::iterator itHyImageID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    ostringstream tmposs;
    oss.str("");
    oss << RESTURL_FSSNAPSHOT << "?PARENTID=" << strFSID << "&sortby=TIMESTAMP,a";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyImageInfo.clear();
        
        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";

            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); i++){
                HYIMAGE_INFO_STRU rstHyImageInfo;

                rstHyImageInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                rstHyImageInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                rstHyImageInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
                rstHyImageInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                SnapshotConvertStatus(rstHyImageInfo.uiStatus,rstHyImageInfo.uiState);//mark

                unsigned long long ullTimeStamp = jsonValue2Type<unsigned long long>(restPkg[i][SNAPSHOT_TAG_TIMESTAMP]);
                TimeChg(ullTimeStamp,rstHyImageInfo.strTimeStamp);

                if(restPkg[i].isMember(COMMON_TAG_VSTOREID)){
                    rstHyImageInfo.vstoreID = restPkg[i][COMMON_TAG_VSTOREID].asString();
                }
                else{
                    rstHyImageInfo.vstoreID = STR_NOT_EXIST;
                }

                rlstHyImageInfo.push_back(rstHyImageInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }        

        if (flag){
            strIP = *iter;
            break;
        }
        else{
            continue;
        }
    }
 
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showClonefsOffs()
Description  : query clonefs of strFSID.
Data Accessed: None.
Data Updated : None.
Input        : strFSName: name of clonefs.
Output       : lstHyImageInfo:list of clonefs named strFSName.
Return       : Success or fail.
Call         :
Called by    : TestFailoverStop::_stop_test_device_out
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showClonefsOffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    list<string> lstHyImageID;
    list<string>::iterator itHyImageID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    ostringstream tmposs;
    oss.str("");
    oss << RESTURL_FILESYSTEM << "?PARENTFILESYSTEMID=" << strFSID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyImageInfo.clear();

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
            strIP = *iter;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        size_t i = 0;
        for (; i < restPkg.count(); i++){
            HYIMAGE_INFO_STRU rstHyImageInfo;

            rstHyImageInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
            rstHyImageInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
            rstHyImageInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
            rstHyImageInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);

            if (restPkg[i].isMember(COMMON_TAG_VSTOREID)){
                rstHyImageInfo.vstoreID = restPkg[i][COMMON_TAG_VSTOREID].asString();
            }
            else{
                rstHyImageInfo.vstoreID = STR_NOT_EXIST;
            }

            SnapshotConvertStatus(rstHyImageInfo.uiStatus,rstHyImageInfo.uiState);//mark

            rlstHyImageInfo.push_back(rstHyImageInfo);
        }    

        break;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showlunforhyimg()
Description  : Query all LUN information that can be snapshotd on the array.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstLUNInfo:LUN Information.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showlunforhyimg(OUT list<LUN_INFO_STRU> &rlstLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_LUN_CREATE_SNAPSHOT, REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_LUN_CREATE_SNAPSHOT, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_LUN_CREATE_SNAPSHOT, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        rlstLUNInfo.clear();
        for (size_t i = 0; i < restPkg.count(); ++i){
            LUN_INFO_STRU stLUNInfo;

            stLUNInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
            stLUNInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();

            rlstLUNInfo.push_back(stLUNInfo);
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_synchymirror()
Description  : Remote replication synchronization.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_synchymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_REPLICATIONPAIR;
    jSendData[COMMON_TAG_ID] = strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SYNCRONIZE_HYPERMIRROR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the param [%s] the iRet is (%d).", iter->c_str(), RESTURL_SYNCRONIZE_HYPERMIRROR, jSendData.toStyledString().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the param [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SYNCRONIZE_HYPERMIRROR, jSendData.toStyledString().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_splithymirror()
Description  : Remote replication split.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_REPLICATIONPAIR;
    jSendData[COMMON_TAG_ID] = rstrMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SPILT_HYPERMIRROR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SPILT_HYPERMIRROR, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SPILT_HYPERMIRROR, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_disablehcpair()
Description  : pause hypeMetro.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID,bool isPrimary)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETROPAIR;
    jSendData[COMMON_TAG_ID] = rstrHMPairID;
    if(isPrimary){
        jSendData[HYPERMETROPAIR_TAG_ISPRIMARY] = "true";
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_DISABLE_HYPERMETRO, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_DISABLE_HYPERMETRO, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_DISABLE_HYPERMETRO, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_swaphymirror()
Description  : Remote replication master-slave switching.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_swaphymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_REPLICATIONPAIR;
    jSendData[COMMON_TAG_ID] = strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SWITCHROLE_HYPERMIRROR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SWITCHROLE_HYPERMIRROR, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SWITCHROLE_HYPERMIRROR, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_swaphypermetro()
Description  : hypeMetro master-slave switching.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_swaphypermetro(IN string &strHMPairID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETROPAIR;
    jSendData[COMMON_TAG_ID] = strHMPairID;
    jSendData[COMMON_TAG_HCRESOURCETYPE] = HCRESOURCETYPE_LUN;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SWAP_HCPAIR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SWAP_HCPAIR, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SWAP_HCPAIR, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_synchmpair()
Description  : hypeMetro sync.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTCmd::CMD_synchmpair(IN string &strHMPairID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETROPAIR;
    jSendData[COMMON_TAG_ID] = strHMPairID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SYNC_HCPAIR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SYNC_HCPAIR, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SYNC_HCPAIR, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}


/*------------------------------------------------------------
Function Name: CMD_showhymirrorinfo()
Description  : query remote replication information
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss <<RESTURL_REPLICATIONPAIR <<"/" <<rstHyMirrorInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstHyMirrorInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstHyMirrorInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_REPLICATIONMODEL]);
    UpdateModel(rstHyMirrorInfo.uiModel);
    
    rstHyMirrorInfo.bIsBelongGroup = (restPkg[0][REPLICATIONPAIR_TAG_ISINCG].asString().compare("true") == 0);
    if(rstHyMirrorInfo.bIsBelongGroup){
        string strCGID = restPkg[0][REPLICATIONPAIR_TAG_CGID].asString();
        rstHyMirrorInfo.strGroupID = "";
        if (strCGID.compare(STR_INVALID_CGID) != 0)
            rstHyMirrorInfo.strGroupID = strCGID;
    }    
    rstHyMirrorInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    UpdateMirrorStatus(rstHyMirrorInfo.uiStatus);

    rstHyMirrorInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    rstHyMirrorInfo.bIsPrimary = (restPkg[0][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);
    rstHyMirrorInfo.uilocalResType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_LOCALRESTYPE]);
    rstHyMirrorInfo.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0]["SECRESACCESS"]);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhymirrorinfo_all()
Description  : Query all remote replication information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream tmposs;
    LUN_INFO_STRU stLUNInfo;
    FS_INFO_STRU  stFSInfo;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        while(true){
            tmposs.str("");
            tmposs << RESTURL_REPLICATIONPAIR << "?range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";

            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            HYMIRROR_INFO_STRU stHyMirrorInfo;
            VSTORE_PAIR_INFO_STRU stVstorePairInfo;
            for (; i < restPkg.count(); ++i){
                stHyMirrorInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stHyMirrorInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                stHyMirrorInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_REPLICATIONMODEL]);
                UpdateModel(stHyMirrorInfo.uiModel);
                stHyMirrorInfo.uilocalResType = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESTYPE]);    
            
                if(jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESTYPE]) == OBJ_LUN){
                    stHyMirrorInfo.bIsBelongGroup = (restPkg[i][REPLICATIONPAIR_TAG_ISINCG].asString().compare("true") == 0);
                    if(stHyMirrorInfo.bIsBelongGroup){
                        string strCGID = restPkg[i][REPLICATIONPAIR_TAG_CGID].asString();
                        stHyMirrorInfo.strGroupID = "";
                        if (strCGID.compare(STR_INVALID_CGID) != 0)
                            stHyMirrorInfo.strGroupID = strCGID;
                    }

                    stLUNInfo.strID = restPkg[i][REPLICATIONPAIR_TAG_LOCALRESID].asString();
                    iRet = CMD_showlun(stLUNInfo);
                    if (iRet != RETURN_OK){
                        COMMLOG(OS_LOG_ERROR, "CMD_showlun (%s) fail, ret=%d", stLUNInfo.strID.c_str(), iRet);
                        flag = false;
                        break;
                    }
                    stHyMirrorInfo.strPriVstoreID = stLUNInfo.strVstoreID;
                }
                else if(jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESTYPE]) == OBJ_FILESYSTEM && g_bnfs){
                    stHyMirrorInfo.bIsBelongGroup = (restPkg[i].isMember(REPLICATIONPAIR_TAG_VSTOREPAIRID) && (restPkg[i][REPLICATIONPAIR_TAG_VSTOREPAIRID].compare(STR_NOT_EXIST) != 0));
                    if(stHyMirrorInfo.bIsBelongGroup){
                        COMMLOG(OS_LOG_INFO, "[%s]SRA not support fs remote replications in vstore pair, ignore it...", stHyMirrorInfo.strID.c_str());
                        continue;                        
                    }
                    else{
                        stHyMirrorInfo.strGroupID = "";
                    }

                    stFSInfo.strID = restPkg[i][REPLICATIONPAIR_TAG_LOCALRESID].asString();
                    iRet = CMD_showfs(stFSInfo);
                    if (iRet != RETURN_OK){
                        COMMLOG(OS_LOG_ERROR, "CMD_showlun (%s) fail, ret=%d", stFSInfo.strID.c_str(), iRet);
                        flag = false;
                        break;
                    }
                    stHyMirrorInfo.strPriVstoreID = stFSInfo.vstoreId;
                }
                else{
                    continue;
                }

                stHyMirrorInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
                UpdateMirrorStatus(stHyMirrorInfo.uiStatus);
                stHyMirrorInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                stHyMirrorInfo.bIsPrimary = (restPkg[i][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);

                if(restPkg[i].isMember(REPLICATIONPAIR_TAG_VSTOREPAIRID)){
                    stHyMirrorInfo.vstorePairID = restPkg[i][REPLICATIONPAIR_TAG_VSTOREPAIRID].asString();
                }
                else{
                    stHyMirrorInfo.vstorePairID = STR_NOT_EXIST;
                }

                if((restConn->getVstoreID().compare(STR_NOT_EXIST) == 0) && (stHyMirrorInfo.strPriVstoreID.compare(STR_NOT_EXIST) != 0)){
                    COMMLOG(OS_LOG_INFO, "Current login is system view, ignoring rm pair %s beglongs to vstore %s", stHyMirrorInfo.strID.c_str(),stHyMirrorInfo.strPriVstoreID.c_str());
                    continue;
                }

                rlstHyMirrorInfo.push_back(stHyMirrorInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }        

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;    
}

/*------------------------------------------------------------
Function Name: CMD_showhypermetroinfo()
Description  : query hypermetro information.
Data Accessed: None.
Data Updated : None.
Input        : rstHMPairInfo
Output       : rstHMPairInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_HYPERMETROPAIR <<"/" <<rstHMPairInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstHMPairInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();   
    rstHMPairInfo.strDomainID = restPkg[0][HYPERMETROPAIR_TAG_DOMAINID].asString();
    rstHMPairInfo.bIsPrimary = (restPkg[0][HYPERMETROPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);
    rstHMPairInfo.bIsBelongGroup = (restPkg[0][HYPERMETROPAIR_TAG_ISINCG].asString().compare("true") == 0);
    if(rstHMPairInfo.bIsBelongGroup){
        GROUP_INFO_STRU stGroupInfo;
        string strCGID = restPkg[0][HYPERMETROPAIR_TAG_CGID].asString();
        rstHMPairInfo.strGroupID = "";
        if (strCGID.compare(STR_INVALID_CGID) != 0){
            rstHMPairInfo.strGroupID = strCGID;
            stGroupInfo.strID = strCGID;
            CMD_showconsistgrhminfo(stGroupInfo);
            if (stGroupInfo.uiIsPrimary == HM_GROUP_RELATION_SLAVE){
                rstHMPairInfo.bIsPrimary = false;
            }
            else{
                rstHMPairInfo.bIsPrimary = true;
            }
        }
    }

    rstHMPairInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    UpdateMirrorStatus(rstHMPairInfo.uiStatus);
    rstHMPairInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);

    if (restPkg[0].isMember(HYPERMETROPAIR_TAG_REPLICATIONPROGRESS)){
        rstHMPairInfo.iProgress = restPkg[0][HYPERMETROPAIR_TAG_REPLICATIONPROGRESS].asString(); 
    }
    else{
        rstHMPairInfo.iProgress = "100";
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showHyperMetroPair_all()
Description  : query all hypermetro information.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : rlstHyperMetroPairInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream tmposs;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyperMetroPairInfo.clear();

        while(true){
            tmposs.str("");
           
            tmposs << RESTURL_HYPERMETROPAIR << "?range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

           
            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                if (HCRESOURCETYPE_FS == restPkg[i][COMMON_TAG_HCRESOURCETYPE].asString()){
                    continue;
                }

                HYPERMETROPAIR_INFO_STRU stHyperMetroPairInfo;
                stHyperMetroPairInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stHyperMetroPairInfo.strDomainID = restPkg[i][HYPERMETROPAIR_TAG_DOMAINID].asString();
                stHyperMetroPairInfo.strDomainName = restPkg[i][HYPERMETROPAIR_TAG_DOMAINNAME].asString();
                
                if (g_bstretch){
                    stHyperMetroPairInfo.stretched = "true";
                }
                else{
                    stHyperMetroPairInfo.stretched = "false";
                }
                stHyperMetroPairInfo.bIsPrimary = (restPkg[i][HYPERMETROPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);
                stHyperMetroPairInfo.bIsBelongGroup = (restPkg[i][HYPERMETROPAIR_TAG_ISINCG].asString().compare("true") == 0);
                GROUP_INFO_STRU stGroupInfo;
                string strCGID = restPkg[i][REPLICATIONPAIR_TAG_CGID].asString();
                stHyperMetroPairInfo.strGroupID = "";
                if(stHyperMetroPairInfo.bIsBelongGroup){
                    if (strCGID.compare(STR_INVALID_CGID) != 0){
                        stHyperMetroPairInfo.strGroupID = strCGID;
                        stGroupInfo.strID = strCGID;
                        CMD_showconsistgrhminfo(stGroupInfo);
                        if (stGroupInfo.uiIsPrimary == HM_GROUP_RELATION_SLAVE){
                            stHyperMetroPairInfo.bIsPrimary = false;
                        }
                        else{
                            stHyperMetroPairInfo.bIsPrimary = true;
                        }
                    }
                }
                stHyperMetroPairInfo.strLocalLUNID = restPkg[i][HYPERMETROPAIR_TAG_LOCALOBJID].asString();
                stHyperMetroPairInfo.strRemoteLunID = restPkg[i][HYPERMETROPAIR_TAG_REMOTEOBJID].asString();
                
                stHyperMetroPairInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][HYPERMETROPAIR_TAG_RUNNINGSTATUS]);
                if (stHyperMetroPairInfo.bIsPrimary ){
                    stHyperMetroPairInfo.uiRelationType = LUN_RELATION_MASTER;    
                    stHyperMetroPairInfo.uiSecResAccess =jsonValue2Type<unsigned int>(restPkg[i][HYPERMETROPAIR_TAG_REMOTEHOSTACCESSSTATE]);
                }
                else{
                    stHyperMetroPairInfo.uiRelationType = LUN_RELATION_SLAVE;
                    stHyperMetroPairInfo.uiSecResAccess =jsonValue2Type<unsigned int>(restPkg[i][HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE]);
                }

                if(stHyperMetroPairInfo.bIsBelongGroup){
                    stHyperMetroPairInfo.uiSecResAccess = stGroupInfo.uiSecResAccess;
                }

                rlstHyperMetroPairInfo.push_back(stHyperMetroPairInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }
        
        if(flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;    
}

/*------------------------------------------------------------
Function Name: CMD_showhymirrorlun()
Description  : Query remote replication member LUN information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rstHyMirrorLUNInfo:
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorlun(IN OUT HYMIRROR_LF_INFO_STRU &rstHyMirrorLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_REPLICATIONPAIR <<"/" <<rstHyMirrorLUNInfo.strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (restPkg[0][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);
    unsigned int resType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_LOCALRESTYPE]);

    string localID = restPkg[0][REPLICATIONPAIR_TAG_LOCALRESID].asString();
    string remoteID = restPkg[0][REPLICATIONPAIR_TAG_REMOTERESID].asString();

    unsigned int uiSyncType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SYNCHRONIZETYPE]);
    if (0xFFFFFFFF == uiSyncType){
        uiSyncType = 0;
    }

    int iProgress = jsonValue2Type<int>(restPkg[0][REPLICATIONPAIR_TAG_REPLICATIONPROGRESS]);
    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_PRIRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }

    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    if (isPrimary){
        rstHyMirrorLUNInfo.uiRelationType = LUN_RELATION_MASTER;
    }
    else{
        rstHyMirrorLUNInfo.uiRelationType = LUN_RELATION_SLAVE;
    }

    if (localID == rstHyMirrorLUNInfo.strID){
        rstHyMirrorLUNInfo.strID = localID;
        rstHyMirrorLUNInfo.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
        rstHyMirrorLUNInfo.strArrayName = rstSysInfo.strName;
    }
    else if (remoteID == rstHyMirrorLUNInfo.strID){
        rstHyMirrorLUNInfo.strID = remoteID;
        rstHyMirrorLUNInfo.strArrayWWN = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICESN].asString();
        rstHyMirrorLUNInfo.strArrayName = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICENAME].asString();
    }
    else{
        COMMLOG(OS_LOG_ERROR, "LUN id[%s] not found.", rstHyMirrorLUNInfo.strID.c_str());
        return RETURN_ERR;
    }
    
    rstHyMirrorLUNInfo.uilocalResType = resType;
    rstHyMirrorLUNInfo.uiSyncType = uiSyncType;
    rstHyMirrorLUNInfo.iProgress = iProgress;
    rstHyMirrorLUNInfo.uiDataStatus = uiDataStatusLocal;
    ConvertMirrorSlaveLUNStatus(rstHyMirrorLUNInfo.uiDataStatus);

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(restPkg[0][REPLICATIONPAIR_TAG_STARTTIME]);
    TimeChg(ullStartTime,rstHyMirrorLUNInfo.strSyncStartTime);

    rstHyMirrorLUNInfo.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(rstHyMirrorLUNInfo.uiPairStatus);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhymirrorlun_all()
Description  : Query all remote replication member LUN information
Data Accessed: None.
Data Updated : None.
Input        : 
               strMirrorID
Output       : rlstHyMirrorLUNInfo:
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorlun_all(IN string &strMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;
    int iLunRet = RETURN_OK;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_REPLICATIONPAIR <<"/" <<strMirrorID;

    rlstHyMirrorLUNInfo.clear();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;

        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    unsigned int resType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_LOCALRESTYPE]);

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);
    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (restPkg[0][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);

    unsigned int uiSyncType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SYNCHRONIZETYPE]);
    if (0xFFFFFFFF == uiSyncType){
        uiSyncType = 0;
    }

    int iProgress = jsonValue2Type<int>(restPkg[0][REPLICATIONPAIR_TAG_REPLICATIONPROGRESS]);

    unsigned int uiDataStatusRemote = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusRemote){
        uiDataStatusRemote = 0;
    }

    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_PRIRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }
    
    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    if (uiPairStatus == RUNNING_STATUS_NORMAL && iProgress == -1){
        iProgress = 100;
    }

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(restPkg[0][REPLICATIONPAIR_TAG_STARTTIME]);

    HYMIRROR_LF_INFO_STRU stHyMirrorLUNInfoLocal, stHyMirrorLUNInfoRemote;

    stHyMirrorLUNInfoLocal.uiSyncType = uiSyncType;
    stHyMirrorLUNInfoLocal.iProgress = iProgress;
    stHyMirrorLUNInfoLocal.uiDataStatus = uiDataStatusLocal;
    ConvertMirrorSlaveLUNStatus(stHyMirrorLUNInfoLocal.uiDataStatus);    
    TimeChg(ullStartTime,stHyMirrorLUNInfoLocal.strSyncStartTime);
    stHyMirrorLUNInfoLocal.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorLUNInfoLocal.uiPairStatus);
    stHyMirrorLUNInfoLocal.uilocalResType = resType;

    stHyMirrorLUNInfoRemote.uiSyncType = uiSyncType;
    stHyMirrorLUNInfoRemote.iProgress = iProgress;
    stHyMirrorLUNInfoRemote.uiDataStatus = uiDataStatusRemote;
    ConvertMirrorSlaveLUNStatus(stHyMirrorLUNInfoRemote.uiDataStatus);
    TimeChg(ullStartTime,stHyMirrorLUNInfoRemote.strSyncStartTime);
    stHyMirrorLUNInfoRemote.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorLUNInfoRemote.uiPairStatus);
    stHyMirrorLUNInfoRemote.uilocalResType = resType;

    stHyMirrorLUNInfoLocal.strID = restPkg[0][REPLICATIONPAIR_TAG_LOCALRESID].asString();
    if (!stHyMirrorLUNInfoLocal.strID.empty()){
        if(resType == OBJ_LUN){
            COMMLOG(OS_LOG_INFO, "Query LUN[%s] info", stHyMirrorLUNInfoLocal.strID.c_str());
            LUN_INFO_STRU localLUNInfo;
            localLUNInfo.strID = stHyMirrorLUNInfoLocal.strID;
            iLunRet = CMD_showlun(localLUNInfo);
            if (RETURN_OK == iLunRet){
                stHyMirrorLUNInfoLocal.strName = localLUNInfo.strName;
            }
            else{
                return iLunRet;
            }
        }
        else if(resType == OBJ_FILESYSTEM){
            COMMLOG(OS_LOG_INFO, "Query FS[%s] info", stHyMirrorLUNInfoLocal.strID.c_str());
            FS_INFO_STRU localFSInfo;
            localFSInfo.strID = stHyMirrorLUNInfoLocal.strID;
            iLunRet = CMD_showfs(localFSInfo);
            if (RETURN_OK == iLunRet){
                stHyMirrorLUNInfoLocal.strName = localFSInfo.strName;
            }
            else{
                return iLunRet;
            }
        }
        else{
            COMMLOG(OS_LOG_ERROR, "Unknow local resType[%d]", resType);
            return RETURN_ERR;
        }
    }

    stHyMirrorLUNInfoLocal.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
    stHyMirrorLUNInfoLocal.strArrayName = rstSysInfo.strName;

    stHyMirrorLUNInfoRemote.strID = restPkg[0][REPLICATIONPAIR_TAG_REMOTERESID].asString();
    stHyMirrorLUNInfoRemote.strArrayWWN = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICESN].asString();
    stHyMirrorLUNInfoRemote.strArrayName = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICENAME].asString();

    stHyMirrorLUNInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);
    stHyMirrorLUNInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);

    if (isPrimary){
        stHyMirrorLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
        stHyMirrorLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
    }
    else{
        stHyMirrorLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
        stHyMirrorLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
    }

    rlstHyMirrorLUNInfo.push_back(stHyMirrorLUNInfoLocal);
    rlstHyMirrorLUNInfo.push_back(stHyMirrorLUNInfoRemote);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhypermetrolun_all()
Description  : Query the information of the hypermetro member LUN.
Data Accessed: None.
Data Updated : None.
Input        : hypermetroid:
Output       : rlstHMLUNInfo:
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HYPERMETROPAIR <<"/" <<hypermetroid;

    rlstHMLUNInfo.clear();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (restPkg[0][HYPERMETROPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);

    int iProgress = jsonValue2Type<int>(restPkg[0][HYPERMETROPAIR_TAG_REPLICATIONPROGRESS]);

    unsigned int uiDataStatusRemote = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_REMOTEDATASTATE]);
    if (0xFFFFFFFF == uiDataStatusRemote){
        uiDataStatusRemote = 0;
    }

    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_LOCALDATASTATE]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }
    
    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    if (uiPairStatus == RUNNING_STATUS_NORMAL && iProgress == -1){
        iProgress = 100;
    }

    unsigned int uiHealthStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    if (0xFFFFFFFF == uiHealthStatus){
        uiHealthStatus = 0;
    }

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(restPkg[0][HYPERMETROPAIR_TAG_STARTTIME]);

    HYPERMETRO_LF_INFO_STRU stHMLUNInfoLocal, stHMLUNInfoRemote;
  
    stHMLUNInfoLocal.strHMpairID = hypermetroid;
    stHMLUNInfoLocal.iProgress = iProgress;
    stHMLUNInfoLocal.uiDataStatus = uiDataStatusLocal;
    ConvertHMSlaveLUNStatus(stHMLUNInfoLocal.uiDataStatus);    
    TimeChg(ullStartTime,stHMLUNInfoLocal.strSyncStartTime);
    stHMLUNInfoLocal.uiPairStatus = uiPairStatus;
    ConvertHMPairStatus(stHMLUNInfoLocal.uiPairStatus);
    stHMLUNInfoLocal.uiHealthStatus = uiHealthStatus;

    stHMLUNInfoRemote.strHMpairID = hypermetroid;
    stHMLUNInfoRemote.iProgress = iProgress;
    stHMLUNInfoRemote.uiDataStatus = uiDataStatusRemote;
    ConvertHMSlaveLUNStatus(stHMLUNInfoRemote.uiDataStatus);
    TimeChg(ullStartTime,stHMLUNInfoRemote.strSyncStartTime);
    stHMLUNInfoRemote.uiPairStatus = uiPairStatus;
    ConvertHMPairStatus(stHMLUNInfoRemote.uiPairStatus);
    stHMLUNInfoRemote.uiHealthStatus = uiHealthStatus;

    stHMLUNInfoLocal.strID = restPkg[0][HYPERMETROPAIR_TAG_LOCALOBJID].asString();
    stHMLUNInfoLocal.strName = restPkg[0][HYPERMETROPAIR_TAG_LOCALOBJNAME].asString();
    stHMLUNInfoLocal.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
    stHMLUNInfoLocal.strArrayName = rstSysInfo.strName;
    stHMLUNInfoLocal.strDomainID = restPkg[0][HYPERMETROPAIR_TAG_DOMAINID].asString();
    stHMLUNInfoLocal.strIsinCg = restPkg[0][HYPERMETROPAIR_TAG_ISINCG].asString();
    stHMLUNInfoLocal.uiLocalAccess = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE]);

    GROUP_INFO_STRU stGroupInfo;
    string strCGID = restPkg[0][HYPERMETROPAIR_TAG_CGID].asString();
    if(stHMLUNInfoLocal.strIsinCg.compare("true") == 0){
        stGroupInfo.strID = strCGID;
        iRet = CMD_showconsistgrhminfo(stGroupInfo);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "CMD_showconsistgrhminfo failed, the iRet is (%d).", iRet);
            return iRet;
        }
        if (stGroupInfo.uiIsPrimary == HM_GROUP_RELATION_SLAVE){
            isPrimary = false;
        }
        else{
            isPrimary = true;
        }

        if (isPrimary){
            stHMLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
            stHMLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
        }
        else{
            stHMLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
            stHMLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
        }
        stHMLUNInfoRemote.uiSecResAccess = stGroupInfo.uiSecResAccess;
        stHMLUNInfoLocal.uiSecResAccess = stGroupInfo.uiSecResAccess;
    }
    if (g_bstretch){
        stHMLUNInfoLocal.strstretched = "true";
    }
    else{
        stHMLUNInfoLocal.strstretched = "false";
    }

    HYPERMETRODOMAIN_LF_INFO_STRU stHMDomainInfoLocal;
    iRet = CMD_showhyperMetroDomain_all(stHMLUNInfoLocal.strDomainID, stHMDomainInfoLocal);
    if (iRet == RETURN_OK){
        stHMLUNInfoLocal.strCptype = stHMDomainInfoLocal.strCptype;
    }

    stHMLUNInfoRemote.strID = restPkg[0][HYPERMETROPAIR_TAG_REMOTEOBJID].asString();
    stHMLUNInfoRemote.strName = restPkg[0][HYPERMETROPAIR_TAG_REMOTEOBJNAME].asString();
    stHMLUNInfoRemote.strDomainID = restPkg[0][HYPERMETROPAIR_TAG_DOMAINID].asString();
    stHMLUNInfoRemote.strIsinCg = restPkg[0][HYPERMETROPAIR_TAG_ISINCG].asString();
    if (g_bstretch){
        stHMLUNInfoRemote.strstretched = "true";
    }
    else{
        stHMLUNInfoRemote.strstretched = "false";
    }

    iRet = CMD_showhyperMetroDomain_all(stHMLUNInfoRemote.strDomainID, stHMDomainInfoLocal);
    if (iRet == RETURN_OK){
        stHMLUNInfoRemote.strArrayWWN = stHMDomainInfoLocal.strRemoteArraySN;
        stHMLUNInfoRemote.strArrayName = stHMDomainInfoLocal.strRemoteArrayName;
        stHMLUNInfoRemote.strCptype = stHMDomainInfoLocal.strCptype;
    }

    if(stHMLUNInfoLocal.strIsinCg.compare("true") != 0){
        if (isPrimary){
            stHMLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
            stHMLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
            stHMLUNInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_REMOTEHOSTACCESSSTATE]);
            stHMLUNInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_REMOTEHOSTACCESSSTATE]);
        }
        else{
            stHMLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
            stHMLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
            stHMLUNInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE]);
            stHMLUNInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE]);
        }
        if ("false" == stHMLUNInfoLocal.strCptype && HM_STATUS_FAULT == stHMLUNInfoLocal.uiHealthStatus){
            unsigned int uiLocalAccessState = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE]);
            unsigned int uiRemoteAccessState = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETROPAIR_TAG_REMOTEHOSTACCESSSTATE]);
            if (RESOURCE_ACCESS_READ_WRITE == uiLocalAccessState && RESOURCE_ACCESS_NODATA == uiRemoteAccessState){
                stHMLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
                stHMLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;     
                stHMLUNInfoLocal.uiSecResAccess = RESOURCE_ACCESS_NODATA;
                stHMLUNInfoRemote.uiSecResAccess = RESOURCE_ACCESS_NODATA;
            }
            if (RESOURCE_ACCESS_NOACCESS == uiLocalAccessState && RESOURCE_ACCESS_NODATA == uiRemoteAccessState){
                stHMLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
                stHMLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
                stHMLUNInfoLocal.uiSecResAccess = RESOURCE_ACCESS_READ_WRITE;
                stHMLUNInfoRemote.uiSecResAccess = RESOURCE_ACCESS_READ_WRITE;
            }
        }
    }

    rlstHMLUNInfo.push_back(stHMLUNInfoLocal);
    rlstHMLUNInfo.push_back(stHMLUNInfoRemote);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhyperMetroDomain_all()
Description  : Query the information of the hypermetro member LUN.
Data Accessed: None.
Data Updated : None.
Input        : 
               hypermetroid:
Output       : rlstHMLUNInfo:
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HYPERMETRODOMAIN <<"/" <<hmdomainid;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }
    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    HYPERMETRODOMAIN_LF_INFO_STRU stHMDomainInfoLocal;

    stHMDomainInfoLocal.strID = restPkg[0][COMMON_TAG_ID].asString();
    stHMDomainInfoLocal.strName = restPkg[0][COMMON_TAG_NAME].asString();
    stHMDomainInfoLocal.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETRODOMAIN_TAG_RUNNINGSTATUS]);
    
    Json::Reader reader;  
    Json::Value strRemoteDevices;  
    std::string str = restPkg[0][HYPERMETRODOMAIN_TAG_REMOTEDEVICES].asString();
    if (reader.parse(str, strRemoteDevices))  {  
        stHMDomainInfoLocal.strRemoteArraySN = strRemoteDevices[0][HYPERMETRODOMAIN_TAG_REMOTEDEVICES_SN].asString();
        stHMDomainInfoLocal.strRemoteArrayName = strRemoteDevices[0][HYPERMETRODOMAIN_TAG_REMOTEDEVICES_DEVNAME].asString(); 
    }  

    if (0 == restPkg[0][HYPERMETRODOMAIN_TAG_CPTYPE].asString().compare(HM_CPTYPE_NULL)){
        stHMDomainInfoLocal.strCptype = "true";
    }
    else {
        //Since the SRM has a problem with the staticSitePreference field support, it is temporarily changed to true, that is, whether the array supports arbitration or not, returning to vmware is true.
        stHMDomainInfoLocal.strCptype = "false";
    }

    rstHMDomainInfo = stHMDomainInfoLocal;

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhymirrorfs_all()
Description  : Query remote replication member FS information.
Data Accessed: None.
Data Updated : None.
Input        : 
               strMirrorID:
Output       : rlstHyMirrorLUNInfo:
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorfs_all(IN string &strMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;
    int iLunRet = RETURN_OK;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_REPLICATIONPAIR<<"/"<<strMirrorID;

    rlstHyMirrorFSInfo.clear();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    unsigned int resType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_LOCALRESTYPE]);

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (restPkg[0][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);
    unsigned int uiSyncType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SYNCHRONIZETYPE]);
    if (0xFFFFFFFF == uiSyncType){
        uiSyncType = 0;
    }

    int iProgress = jsonValue2Type<int>(restPkg[0][REPLICATIONPAIR_TAG_REPLICATIONPROGRESS]);
    unsigned int uiDataStatusRemote = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusRemote){
        uiDataStatusRemote = 0;
    }

    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_PRIRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }

    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    if (uiPairStatus == RUNNING_STATUS_NORMAL && iProgress == -1){
        iProgress = 100;
    }

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(restPkg[0][REPLICATIONPAIR_TAG_STARTTIME]);
    HYMIRROR_LF_INFO_STRU stHyMirrorFSInfoLocal, stHyMirrorFSInfoRemote;
    
    stHyMirrorFSInfoLocal.strMirrorID = restPkg[0][COMMON_TAG_ID].asString();
    stHyMirrorFSInfoLocal.uiSyncType = uiSyncType;
    stHyMirrorFSInfoLocal.iProgress = iProgress;
    stHyMirrorFSInfoLocal.uiDataStatus = uiDataStatusLocal;
    ConvertMirrorSlaveLUNStatus(stHyMirrorFSInfoLocal.uiDataStatus);    
    TimeChg(ullStartTime,stHyMirrorFSInfoLocal.strSyncStartTime);
    stHyMirrorFSInfoLocal.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorFSInfoLocal.uiPairStatus);
    stHyMirrorFSInfoLocal.uilocalResType = resType;

    stHyMirrorFSInfoRemote.uiSyncType = uiSyncType;
    stHyMirrorFSInfoRemote.iProgress = iProgress;
    stHyMirrorFSInfoRemote.uiDataStatus = uiDataStatusRemote;
    ConvertMirrorSlaveLUNStatus(stHyMirrorFSInfoRemote.uiDataStatus);
    TimeChg(ullStartTime,stHyMirrorFSInfoRemote.strSyncStartTime);
    stHyMirrorFSInfoRemote.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorFSInfoRemote.uiPairStatus);
    stHyMirrorFSInfoRemote.uilocalResType = resType;

    stHyMirrorFSInfoLocal.strID = restPkg[0][REPLICATIONPAIR_TAG_LOCALRESID].asString();
    if (!stHyMirrorFSInfoLocal.strID.empty()){
        FS_INFO_STRU localFSInfo;
        localFSInfo.strID = stHyMirrorFSInfoLocal.strID;
        iLunRet = CMD_showfs(localFSInfo);
        if (RETURN_OK == iLunRet){
            stHyMirrorFSInfoLocal.strName = localFSInfo.strName;
        }
    }
    stHyMirrorFSInfoLocal.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
    stHyMirrorFSInfoLocal.strArrayName = rstSysInfo.strName;

    stHyMirrorFSInfoRemote.strID = restPkg[0][REPLICATIONPAIR_TAG_REMOTERESID].asString();
    stHyMirrorFSInfoRemote.strArrayWWN = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICESN].asString();
    stHyMirrorFSInfoRemote.strArrayName = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICENAME].asString();

    stHyMirrorFSInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);
    stHyMirrorFSInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);

    if (isPrimary){
        stHyMirrorFSInfoLocal.uiRelationType = LUN_RELATION_MASTER;
        stHyMirrorFSInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
    }
    else{
        stHyMirrorFSInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
        stHyMirrorFSInfoRemote.uiRelationType = LUN_RELATION_MASTER;
    }

    rlstHyMirrorFSInfo.push_back(stHyMirrorFSInfoLocal);
    rlstHyMirrorFSInfo.push_back(stHyMirrorFSInfoRemote);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showhymirrorfs_all()
Description  : Query remote replication member fs information.
Data Accessed: None.
Data Updated : None.
Input        : 
               strMirrorID
Output       : rlstHyMirrorLUNInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhymirrorbyfs(IN string &strID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo,IN bool bismirrorID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;
    int iLunRet = RETURN_OK;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    if (bismirrorID){
        oss <<RESTURL_REPLICATIONPAIR <<"/"<<strID;
    }
    else{
        oss <<RESTURL_REPLICATIONPAIR_ASSOCIATE <<"?ASSOCIATEOBJTYPE="<<(int)OBJ_FILESYSTEM<<"&ASSOCIATEOBJID="<<strID;
    }

    rlstHyMirrorFSInfo.clear();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    unsigned int resType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_LOCALRESTYPE]);

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (restPkg[0][REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);

    unsigned int uiSyncType = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SYNCHRONIZETYPE]);
    if (0xFFFFFFFF == uiSyncType){
        uiSyncType = 0;
    }

    unsigned int uiDataStatusRemote = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusRemote){
        uiDataStatusRemote = 0;
    }

    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_PRIRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }
    
    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    int iProgress = 0;
    if (uiPairStatus == RUNNING_STATUS_NORMAL){
        iProgress = 100;
    }
    else if (uiPairStatus == RUNNING_STATUS_SYNCHRONIZING){
        iProgress = 50;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "Status of [%s]:uiPairStatus=[%d], iProgress=[%d].", restPkg[0][COMMON_TAG_ID].asCString(), uiPairStatus, iProgress);
    }

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(restPkg[0][REPLICATIONPAIR_TAG_STARTTIME]);

    HYMIRROR_LF_INFO_STRU stHyMirrorFSInfoLocal, stHyMirrorFSInfoRemote;

    stHyMirrorFSInfoLocal.strMirrorID = restPkg[0][COMMON_TAG_ID].asString();
    stHyMirrorFSInfoLocal.uiSyncType = uiSyncType;
    stHyMirrorFSInfoLocal.iProgress = iProgress;
    stHyMirrorFSInfoLocal.uiDataStatus = uiDataStatusLocal;
    ConvertMirrorSlaveLUNStatus(stHyMirrorFSInfoLocal.uiDataStatus);    
    TimeChg(ullStartTime,stHyMirrorFSInfoLocal.strSyncStartTime);
    stHyMirrorFSInfoLocal.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorFSInfoLocal.uiPairStatus);
    stHyMirrorFSInfoLocal.uilocalResType = resType;

    stHyMirrorFSInfoRemote.strMirrorID = restPkg[0][COMMON_TAG_ID].asString();
    stHyMirrorFSInfoRemote.uiSyncType = uiSyncType;
    stHyMirrorFSInfoRemote.iProgress = iProgress;
    stHyMirrorFSInfoRemote.uiDataStatus = uiDataStatusRemote;
    ConvertMirrorSlaveLUNStatus(stHyMirrorFSInfoRemote.uiDataStatus);
    TimeChg(ullStartTime,stHyMirrorFSInfoRemote.strSyncStartTime);
    stHyMirrorFSInfoRemote.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorFSInfoRemote.uiPairStatus);
    stHyMirrorFSInfoRemote.uilocalResType = resType;

    stHyMirrorFSInfoLocal.strID = restPkg[0][REPLICATIONPAIR_TAG_LOCALRESID].asString();
    stHyMirrorFSInfoLocal.strName = restPkg[0][REPLICATIONPAIR_TAG_LOCALRESNAME].asString();

    stHyMirrorFSInfoLocal.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
    stHyMirrorFSInfoLocal.strArrayName = rstSysInfo.strName;

    stHyMirrorFSInfoRemote.strID = restPkg[0][REPLICATIONPAIR_TAG_REMOTERESID].asString();
    stHyMirrorFSInfoRemote.strArrayWWN = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICESN].asString();
    stHyMirrorFSInfoRemote.strArrayName = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICENAME].asString();

    stHyMirrorFSInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);
    stHyMirrorFSInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECRESACCESS]);

    if (isPrimary){
        stHyMirrorFSInfoLocal.uiRelationType = LUN_RELATION_MASTER;
        stHyMirrorFSInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
    }
    else{
        stHyMirrorFSInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
        stHyMirrorFSInfoRemote.uiRelationType = LUN_RELATION_MASTER;
    }

    rlstHyMirrorFSInfo.push_back(stHyMirrorFSInfoLocal);
    rlstHyMirrorFSInfo.push_back(stHyMirrorFSInfoRemote);
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_changeSlaveFsRw()
Description  : Set the slave LUN for remote replication to be readable and writable for fs.
Data Accessed: None.
Data Updated : None.
Input        : strHyMirrorID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_changeSlaveFsRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    oss.str("");

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_REPLICATIONPAIR;
    jSendData[COMMON_TAG_ID] = strHyMirrorID;

    if (RESOURCE_ACCESS_READ_ONLY == accessMode){
        oss <<RESTURL_REPLICATIONPAIR <<"/" <<"SET_SECODARY_FILESYSTEM_READ_ONLY";
    } 
    else if(RESOURCE_ACCESS_READ_WRITE == accessMode){
        oss <<RESTURL_REPLICATIONPAIR <<"/" <<"CANCEL_SECODARY_WRITE_LOCK";
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){    
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), restPkg.errorCode(), restPkg.description().c_str());
            return RETURN_ERR;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_changeSlaveLunRw()
Description  : Set the slave LUN for remote replication to be readable and writable.
Data Accessed: None.
Data Updated : None.
Input        : strHyMirrorID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    stringstream ss;
    ostringstream oss;
    oss.str("");

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    ss <<(int)accessMode;
    string strTmp;
    ss >>strTmp;

    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_REPLICATIONPAIR;
    jSendData[COMMON_TAG_ID] = strHyMirrorID;
    jSendData[REPLICATIONPAIR_TAG_SECRESACCESS] = ss.str();

    oss <<RESTURL_REPLICATIONPAIR <<"/" <<strHyMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){    
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), restPkg.errorCode(), restPkg.description().c_str());
            return RETURN_ERR;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

int CRESTCmd::CMD_changeCGSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    stringstream ss;
    ostringstream oss;
    oss.str("");

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    ss <<(int)accessMode;
    string strTmp;
    ss >>strTmp;

    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;
    jSendData[CONSISTENTGROUP_TAG_SECRESACCESS] = ss.str();

    oss <<RESTURL_CONSISTENTGROUP <<"/" <<strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){    
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), restPkg.errorCode(), restPkg.description().c_str());
            return RETURN_ERR;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_changeVstorePairSlaveLunRw()
Description  : Set tenant Pair from resource protection status.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_changeVstorePairSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    stringstream ss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    ss <<(int)accessMode;
    string strTmp;
    ss >>strTmp;

    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strGroupID;
    jSendData[REPLICATIONPAIR_TAG_SECVSTOREACCESS] = ss.str();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_VSTOREPAIR, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_VSTOREPAIR, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){    
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_VSTOREPAIR, restPkg.errorCode(), restPkg.description().c_str());
            return RETURN_ERR;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_swapconsistgr()
Description  : Consistency group master-slave switching.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_swapconsistgr(IN string strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_CONSISTENTGROUP_SWITCH, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP_SWITCH, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_CONSISTENTGROUP_SWITCH, iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_swapvstorepair()
Description  :Tenant pair master slave switch.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID:租户pair ID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_swapvstorepair(IN const string& strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_VSTOREPAIR_SWITCH, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP_SWITCH, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_CONSISTENTGROUP_SWITCH, iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_swapconsistgrhm()
Description  : hypermetro consistency group master-slave switching.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_swapconsistgrhm(IN string strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETRO_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_HYPERMETRO_CONSISTENTGROUP_SWITCH, 
                REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), 
                    RESTURL_HYPERMETRO_CONSISTENTGROUP_SWITCH, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_HYPERMETRO_CONSISTENTGROUP_SWITCH, 
                    iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_splitconsistgr()
Description  : Consistency group split.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_splitconsistgr(IN string strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_CONSISTENTGROUP_SPLIT, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP_SPLIT, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_CONSISTENTGROUP_SPLIT, iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_splitvstorepair()
Description  : Tenant pair split.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID:租户pairID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_splitvstorepair(IN const string& strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_VSTOREPAIR_SPLIT, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_VSTOREPAIR_SPLIT, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_VSTOREPAIR_SPLIT, iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_stopconsistgrhm()
Description  : hypermetro consistency group is suspended.
Data Accessed: None.
Data Updated : None.
Input        : 
               strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_stopconsistgrhm(IN string strGroupID,bool isPrimary)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETRO_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;
    if(isPrimary){
        jSendData[CONSISTENTGROUP_TAG_PRIORITYSTATIONTYPE] = "0";
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        for (int i = 0; i < 3; i++){
            iRet = restConn->doRequest(RESTURL_HYPERMETRO_CONSISTENTGROUP_STOP, REST_REQUEST_MODE_PUT, 
                jSendData.toStyledString(), restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), 
                    RESTURL_HYPERMETRO_CONSISTENTGROUP_STOP, iRet);
                break;
            }

            iRet = restPkg.errorCode();
            if (0x4040071F == iRet){
                OS_Sleep(3*1000);
                continue;
            }

            if (iRet != RETURN_OK){    
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_HYPERMETRO_CONSISTENTGROUP_STOP, iRet, restPkg.description().c_str());
                return RETURN_ERR;
            }
            return RETURN_OK;
        }
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_syncconsistgr()
Description  : Synchronous consistency group.
Data Accessed: None.
Data Updated : None.
Input        : strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_syncconsistgr(IN string strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_CONSISTENTGROUP_SYNC, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP_SYNC, iRet);
            continue; 
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_CONSISTENTGROUP_SYNC, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_syncconsistgrhm()
Description  : Synchronous hypermetro consistency group.
Data Accessed: None.
Data Updated : None.
Input        : strGroupID
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_syncconsistgrhm(IN string strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_HYPERMETRO_CONSISTENTGROUP;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_HYPERMETRO_CONSISTENTGROUP_SYNC, REST_REQUEST_MODE_PUT, 
                                    jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), 
                    RESTURL_HYPERMETRO_CONSISTENTGROUP_SYNC, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_HYPERMETRO_CONSISTENTGROUP_SYNC, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_syncvstorepair()
Description  : Synchronize vstore pair.
Data Accessed: None.
Data Updated : None.
Input        : strGroupID:租户pairID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_syncvstorepair(IN const string& strGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_VSTOREPAIR_SYNC, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP_SYNC, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_CONSISTENTGROUP_SYNC, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showconsistgrinfo()
Description  : Query consistency group information.
Data Accessed: None.
Data Updated : None.
Input        : rstGroupInfo
Output       : rstGroupInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    string strIP;
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_CONSISTENTGROUP <<"/" <<rstGroupInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstGroupInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstGroupInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstGroupInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    UpdateGroupStatus(rstGroupInfo.uiState);

    rstGroupInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[0][CONSISTENTGROUP_TAG_REPLICATIONMODEL]);
    UpdateModel(rstGroupInfo.uiModel);

    bool isPrimary = (restPkg[0][CONSISTENTGROUP_TAG_ISPRIMARY].asString().compare("true") == 0);
    if (isPrimary){
        rstGroupInfo.uiIsPrimary = MIRROR_LUN_RELATION_MASTER;
    }
    else{
        rstGroupInfo.uiIsPrimary = MIRROR_LUN_RELATION_SLAVE;
    }

    rstGroupInfo.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][CONSISTENTGROUP_TAG_SECRESACCESS]);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showvstorepair_info()
Description  : Query tenant pair information
Data Accessed: None.
Data Updated : None.
Input        : pairid
Output       : vstorePair
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showvstorepair_info(IN const string &pairid, OUT VSTORE_PAIR_INFO_STRU &vstorePair)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    string strIP;
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_VSTOREPAIR <<"/" <<pairid;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    vstorePair.strID= restPkg[0][COMMON_TAG_ID].asString();
    vstorePair.strLocalVstoreid = restPkg[0][REPLICATIONPAIR_TAG_LOCALVSTOREID].asString();
    vstorePair.strRemoteArrayWWN = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICESN].asString();
    vstorePair.strRemoteVstoreid = restPkg[0][REPLICATIONPAIR_TAG_REMOTEVSTOREID].asString();
    vstorePair.strRemoteDeviceid = restPkg[0][REPLICATIONPAIR_TAG_REMOTEDEVICEID].asString();
    vstorePair.uiIdentyReserve = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_IDENTITYPERSERVE]);
    vstorePair.isPrimary = (jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_ROLE]) == 0);
    vstorePair.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    vstorePair.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    UpdateGroupStatus(vstorePair.uiState);

    vstorePair.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[0][REPLICATIONPAIR_TAG_SECVSTOREACCESS]);

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showconsistgrhminfo()
Description  : Query the hypermetro consistency group information.
Data Accessed: None.
Data Updated : None.
Input        : rstGroupInfo
Output       : rstGroupInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    string strIP;
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_HYPERMETRO_CONSISTENTGROUP <<"/" <<rstGroupInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstGroupInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstGroupInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstGroupInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
    rstGroupInfo.hmdomain = restPkg[0][HYPERMETROPAIR_TAG_DOMAINID].asString();
    rstGroupInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETRO_CONSISTENTGROUP_TAG_MODEL]);
    rstGroupInfo.uiSyncDirection = jsonValue2Type<unsigned int>(restPkg[0][HYPERMETRO_CONSISTENTGROUP_TAG_SYNCDIRECTION]);
    rstGroupInfo.uiSecResAccess = RESOURCE_ACCESS_NOACCESS;

    if (HM_SLAVE_PAIR_STATUS_CHAR_NORMAL == rstGroupInfo.uiState){
        rstGroupInfo.uiSecResAccess = RESOURCE_ACCESS_READ_WRITE;
    }
    
    UpdateModel(rstGroupInfo.uiModel);

    bool priorityStationType = (restPkg[0][CONSISTENTGROUP_TAG_PRIORITYSTATIONTYPE].asString().compare("1") == 0);
    if (priorityStationType){
        rstGroupInfo.uiIsPrimary = HM_GROUP_RELATION_SLAVE;
    }
    else{
        rstGroupInfo.uiIsPrimary = HM_GROUP_RELATION_MASTER;
    }

    HYPERMETRODOMAIN_LF_INFO_STRU stHMDomainInfoLocal;
    iRet = CMD_showhyperMetroDomain_all(rstGroupInfo.hmdomain, stHMDomainInfoLocal);
    if (RETURN_OK == iRet){
        rstGroupInfo.strCptype = stHMDomainInfoLocal.strCptype;
    }

    if ("false" == rstGroupInfo.strCptype && HM_STATUS_FAULT == rstGroupInfo.uiStatus){
        if (HM_CONSISGROUP_DATA_FROM_LOCAL_TO_REMOTE == rstGroupInfo.uiSyncDirection){
            rstGroupInfo.uiIsPrimary = HM_GROUP_RELATION_MASTER;
        }
        if (HM_CONSISGROUP_DATA_FROM_REMOTE_TO_LOCAL == rstGroupInfo.uiSyncDirection){
            rstGroupInfo.uiIsPrimary = HM_GROUP_RELATION_SLAVE;
        }
        rstGroupInfo.uiSecResAccess = RESOURCE_ACCESS_NOACCESS;
    }
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showconsistgrinfo()
Description  : Query consistency group information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstGroupInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showconsistgrinfo(OUT list<GROUP_INFO_STRU> &rlstGroupInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_CONSISTENTGROUP, REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_CONSISTENTGROUP, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_CONSISTENTGROUP, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        rlstGroupInfo.clear();
        for (size_t i = 0; i < restPkg.count(); ++i){
            GROUP_INFO_STRU stGroupInfo;

            stGroupInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
            stGroupInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
            stGroupInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
            stGroupInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
            UpdateGroupStatus(stGroupInfo.uiState);

            stGroupInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[i][CONSISTENTGROUP_TAG_REPLICATIONMODEL]);
            UpdateModel(stGroupInfo.uiModel);

            stGroupInfo.uiIsPrimary = jsonValue2Type<unsigned int>(restPkg[i][CONSISTENTGROUP_TAG_ISPRIMARY]);  //true为1，false为0
            stGroupInfo.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[i][CONSISTENTGROUP_TAG_SECRESACCESS]);

            rlstGroupInfo.push_back(stGroupInfo);
        }
    }
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showconsistgrmember()
Description  : Query consistency group member information.
Data Accessed: None.
Data Updated : None.
Input        : strGroupID
Output       : rlstHyMirrorInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    ostringstream tmposs;
    
    oss <<RESTURL_REPLICATIONPAIR_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"="<<(int)OBJ_REPLICATIONPAIR
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_CONSISTENTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strGroupID;


    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyMirrorInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i <restPkg.count(); ++i){
                HYMIRROR_INFO_STRU stHyMirrorInfo;

                string strCGID = restPkg[i][REPLICATIONPAIR_TAG_CGID].asString();
                if (strCGID.compare(strGroupID) != 0){
                    continue;
                }

                stHyMirrorInfo.strGroupID = strGroupID;
                stHyMirrorInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stHyMirrorInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                stHyMirrorInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                UpdateGroupStatus(stHyMirrorInfo.uiState);

                stHyMirrorInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_REPLICATIONMODEL]);
                UpdateModel(stHyMirrorInfo.uiModel);
                stHyMirrorInfo.uilocalResType = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESTYPE]);
                stHyMirrorInfo.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_SECRESACCESS]);
                stHyMirrorInfo.strPriLUNID = restPkg[i]["LOCALRESID"].asString();

                bool bFindSameCg = false;
                for (list<HYMIRROR_INFO_STRU>::const_iterator iterinfo = rlstHyMirrorInfo.begin(); iterinfo != rlstHyMirrorInfo.end();
                    ++iterinfo){
                    if (iterinfo->strID.compare(stHyMirrorInfo.strID) == 0){
                        bFindSameCg = true;
                        break;
                    }
                }
                if (bFindSameCg){
                    continue;
                }
                rlstHyMirrorInfo.push_back(stHyMirrorInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showconsistgrmember()
Description  : Query the vstore pair member information.
Data Accessed: None.
Data Updated : None.
Input        : strGroupID
Output       : rlstHyMirrorInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showvstorepairmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    ostringstream tmposs;
    
    oss <<RESTURL_REPLICATIONPAIR_ASSOCIATE <<"?" << COMMON_TAG_ASSOCIATEOBJID << "=" << strGroupID
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_REPLICATIONVSTOREPAIR;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyMirrorInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i <restPkg.count(); ++i){
                HYMIRROR_INFO_STRU stHyMirrorInfo;

                string strCGID = restPkg[i][REPLICATIONPAIR_TAG_VSTOREPAIRID].asString();
                if (strCGID.compare(strGroupID) != 0){
                    continue;
                }

                stHyMirrorInfo.strGroupID = strGroupID;
                stHyMirrorInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stHyMirrorInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                stHyMirrorInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                UpdateGroupStatus(stHyMirrorInfo.uiState);

                stHyMirrorInfo.uiModel = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_REPLICATIONMODEL]);
                UpdateModel(stHyMirrorInfo.uiModel);
                stHyMirrorInfo.uilocalResType = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESTYPE]);
                stHyMirrorInfo.uiSecResAccess = jsonValue2Type<unsigned int>(restPkg[i][REPLICATIONPAIR_TAG_SECRESACCESS]);

                bool bFindSameCg = false;
                for (list<HYMIRROR_INFO_STRU>::const_iterator iterinfo = rlstHyMirrorInfo.begin(); iterinfo != rlstHyMirrorInfo.end();
                    ++iterinfo){
                    if (iterinfo->strID.compare(stHyMirrorInfo.strID) == 0){
                        bFindSameCg = true;
                        break;
                    }
                }
                if (bFindSameCg){
                    continue;
                }
                
                rlstHyMirrorInfo.push_back(stHyMirrorInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhost()
Description  : Query host information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstHostInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhost(OUT list<CMDHOSTINFO_STRU> &rlstHostInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream tmposs;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHostInfo.clear();

        //In the multi-tenancy view, the interface is not supported and returns directly.
        if(restConn->getVstoreID().compare(STR_NOT_EXIST) != 0){
            COMMLOG(OS_LOG_ERROR, "Query host info is not permitted in vstore view...");
            return RETURN_OK;
        }

        while(true){
            tmposs.str("");
            tmposs << RESTURL_HOST << "?range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_HOST, iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), RESTURL_HOST, iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                CMDHOSTINFO_STRU stHostInfo;

                stHostInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stHostInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                stHostInfo.strHostGroupID = restPkg[i][COMMON_TAG_PARENTID].asString();

                rlstHostInfo.push_back(stHostInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }
    
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhost()
Description  : Show host information by id on array.
Data Accessed: None.
Data Updated : None.
Input        : rHostInfo : Host info.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhost(OUT CMDHOSTINFO_STRU &rHostInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    ostringstream tmposs;
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        tmposs.str("");
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        //In the multi-tenancy view, the interface is not supported and returns directly.
        if(restConn->getVstoreID().compare(STR_NOT_EXIST) != 0){
            COMMLOG(OS_LOG_ERROR, "Query host info is not permitted in vstore view...");
            return RETURN_OK;
        }

        tmposs << RESTURL_HOST << "/" << rHostInfo.strID;
        iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        if(restPkg.count() > 0){
            rHostInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
            rHostInfo.strHostGroupID = restPkg[0][COMMON_TAG_PARENTID].asString();
        }

        return RETURN_OK;
    }
    
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhost()
Description  : Show host information by name on array.
Data Accessed: None.
Data Updated : None.
Input        : rHostInfo : Host info.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    ostringstream tmposs;
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        tmposs.str("");
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        tmposs << RESTURL_HOST << "?filter=NAME::" << name;
        iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        if(restPkg.count() > 0){
            rHostInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
            rHostInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
            rHostInfo.strHostGroupID = restPkg[0][COMMON_TAG_PARENTID].asString();
        }
        else{
            COMMLOG(OS_LOG_ERROR, "Cannot find any hosts named [%s].", name.c_str());
            rHostInfo.strID.clear();
            rHostInfo.strName.clear();
            rHostInfo.strHostGroupID.clear();
        }

        return RETURN_OK;
    }
    
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showlun()
Description  : Query LUN information.
Data Accessed: None.
Data Updated : None.
Input        : rstLUNInfo
Output       : rstLUNInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showlun(LUN_INFO_STRU &rstLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_LUN <<"/" <<rstLUNInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        
        strIP = *iter;

        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    rstLUNInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstLUNInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstLUNInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);

    ConvertSrcLUNStatus(rstLUNInfo.uiStatus,rstLUNInfo.uiState,rstLUNInfo.uiStatus);

    unsigned long long ullCapacity = jsonValue2Type<unsigned long long>(restPkg[0][LUN_TAG_CAPACITY]);

    stringstream ssCapacity;
    ssCapacity<<ullCapacity;
    rstLUNInfo.strCapacity = ssCapacity.str();

    strWWN = restPkg[0][LUN_TAG_WWN].asString();
    if(restPkg[0].isMember(LUN_TAG_DISGUISESTATUS)){
        unsigned int disguisestatus = jsonValue2Type<unsigned int>(restPkg[0][LUN_TAG_DISGUISESTATUS]);
        COMMLOG(OS_LOG_INFO,"GET DISGUISESTATUS:%d",disguisestatus);
        //0: No masquerading 1: Basic masquerading (eDevLUN support) 2: Extended masquerading (eDevLUN support) 3: Inheritance masquerading (eDevLUN is not supported Temporarily not processed) 4: Third party (eDevLUN support)
        if ((disguisestatus == 1) || (disguisestatus == 2) || (disguisestatus == 4)){
            if (restPkg[0].isMember(LUN_TAG_TAKEOVERLUNWWN)){
                strWWN = restPkg[0][LUN_TAG_TAKEOVERLUNWWN].asString();
                COMMLOG(OS_LOG_INFO,"GET takeOverLUNwwn:%s",strWWN.c_str());
            }
        }
    }
    rstLUNInfo.strOwnerControlID = restPkg[0][LUN_TAG_OWNINGCONTROLLER].asString();

    //Convert WWN, add separator
    CMD_addFilterToWWN(strWWN, rstLUNInfo.strWWN);

    //Obtain the snapshot ID contained in the LUN
    lstHyperID.clear();
    if (!jsonValue2Array(restPkg[0][LUN_TAG_SNAPSHOTIDS], lstHyperID)){       
        return RETURN_ERR;
    }

    strHyperID = "";
    for (itHyperID = lstHyperID.begin(); itHyperID != lstHyperID.end(); itHyperID++){
        strHyperID += (*itHyperID + ":");            
    }

    if ("" != strHyperID){
        strHyperID = strHyperID.substr(0,strHyperID.length()-1);
    }
    rstLUNInfo.strImgID = strHyperID;

    //Obtain the LUNCopy ID of the LUN
    lstHyperID.clear();
    if (restPkg[0].isMember(LUN_TAG_LUNCOPYIDS)){
        if(!jsonValue2Array(restPkg[0][LUN_TAG_LUNCOPYIDS], lstHyperID)){
            return RETURN_ERR;
        }
    }

    strHyperID = "";
    for (itHyperID = lstHyperID.begin(); itHyperID != lstHyperID.end(); itHyperID++){
        strHyperID += (*itHyperID + ":");            
    }

    if ("" != strHyperID){
        strHyperID = strHyperID.substr(0,strHyperID.length()-1);
    }
    rstLUNInfo.strCopyID = strHyperID;

    //Obtain the remote replication ID contained in the LUN
    lstHyperID.clear();
    if (!jsonValue2Array(restPkg[0][LUN_TAG_REMOTEREPLICATIONIDS], lstHyperID)){
        return RETURN_ERR;
    }

    strHyperID = "";
   
    //Delete "00" before the remote copy ID  
    for (itHyperID = lstHyperID.begin(); itHyperID != lstHyperID.end(); itHyperID++){
        string strHyper = *itHyperID;   
        string str;
        CMD_delFrontZero(strHyper, str);
        strHyperID += (str + ":");            
    }

    if ("" != strHyperID){
        strHyperID = strHyperID.substr(0,strHyperID.length()-1);
    }

    rstLUNInfo.strMirrorID = strHyperID;

    //Obtain the split mirror ID contained in the LUN.
    lstHyperID.clear();
    if (restPkg[0].isMember(LUN_TAG_SPLITMIRRORIDS)){
        if (!jsonValue2Array(restPkg[0][LUN_TAG_SPLITMIRRORIDS], lstHyperID)){
            return RETURN_ERR;
        }
    }

    strHyperID = "";
    for (itHyperID = lstHyperID.begin(); itHyperID != lstHyperID.end(); itHyperID++){
        strHyperID += (*itHyperID + ":");            
    }

    if ("" != strHyperID){
        strHyperID = strHyperID.substr(0,strHyperID.length()-1);
    }
    rstLUNInfo.strCloneID = strHyperID;

    if(restPkg[0].isMember(COMMON_TAG_VSTOREID)){
        rstLUNInfo.strVstoreID = restPkg[0][COMMON_TAG_VSTOREID].asString();
    }
    else{
        rstLUNInfo.strVstoreID = STR_NOT_EXIST;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showfs()
Description  : Query the fs information.
Data Accessed: None.
Data Updated : None.
Input        : rstLUNInfo
Output       : rstLUNInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showfs(FS_INFO_STRU &rstFSInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    list<NFS_INFO_STRU> lstnfsInfo;
    list<NFS_INFO_STRU>::iterator itNfsInfo;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_FILESYSTEM <<"/" <<rstFSInfo.strID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_WARN, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_WARN, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }
    rstFSInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
    rstFSInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_HEALTHSTATUS]);
    rstFSInfo.uiState = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);

    ConvertSrcLUNStatus(rstFSInfo.uiStatus,rstFSInfo.uiState,rstFSInfo.uiStatus);

    unsigned long long ullCapacity = jsonValue2Type<unsigned long long>(restPkg[0][LUN_TAG_CAPACITY]);

    stringstream ssCapacity;
    ssCapacity<<ullCapacity;
    rstFSInfo.strCapacity = ssCapacity.str();

    rstFSInfo.strOwnerControlID = restPkg[0][LUN_TAG_OWNINGCONTROLLER].asString();

    strHyperID = restPkg[0]["PARENTSNAPSHOTID"].asString();
    rstFSInfo.strImgID = strHyperID;

    if (restPkg[0].isMember(COMMON_TAG_VSTOREID)){
        rstFSInfo.vstoreId = restPkg[0][COMMON_TAG_VSTOREID].asString();
    }
    else{
        rstFSInfo.vstoreId = STR_NOT_EXIST;
    }

    rstFSInfo.strPath = "";
    CMD_shownfs(rstFSInfo.vstoreId, lstnfsInfo);
    for (itNfsInfo = lstnfsInfo.begin();itNfsInfo != lstnfsInfo.end();++itNfsInfo ){
        if (itNfsInfo->strFsID == rstFSInfo.strID){
            std::string::size_type pos  = itNfsInfo->strSharePath.find_last_of("/");
            if (pos == itNfsInfo->strSharePath.size()-1){
                rstFSInfo.strPath = itNfsInfo->strSharePath.substr(0,pos);
            }
            else{
                rstFSInfo.strPath = itNfsInfo->strSharePath;
            }
            rstFSInfo.strNFSID = itNfsInfo->strNfsID;
        }
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showLIF()
Description  : Query logical port information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstLtfInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLIF( OUT list<LIF_INFO_STRU> &rlstLifInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    ostringstream tmposs;
    oss <<RESTURL_LIF<<"?filter=OPERATIONALSTATUS::1";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstLifInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                LIF_INFO_STRU stLif;
                string strIPv4;
                string strIPv6;

                strIPv4 = restPkg[i]["IPV4ADDR"].asString();
                strIPv6 = restPkg[i]["IPV6ADDR"].asString();

                if (0 == strIPv4.empty()){
                    stLif.strLIFIP = strIPv4;
                }
                else if (0 == strIPv6.empty()){
                    stLif.strLIFIP = strIPv6;
                }
                else{
                    continue;
                }

                if(restPkg[i].isMember(COMMON_TAG_VSTOREID)){
                    stLif.vstoreID = restPkg[i][COMMON_TAG_VSTOREID].asString();
                }
                else{
                    stLif.vstoreID = STR_NOT_EXIST;
                }

                //Ignore vstore lifs when login with system admin
                if((restConn->getVstoreID().compare(STR_NOT_EXIST) == 0) && (stLif.vstoreID.compare(STR_NOT_EXIST) != 0)){
                    COMMLOG(OS_LOG_INFO, "Current login is system login, ignoring vstore LIF %s.", stLif.strLIFIP.c_str());
                    continue;
                }
                stLif.operationalStatus = restPkg[i][LF_TAG_OPERATIONALSTATUS].asString();
                stLif.role = jsonValue2Type<unsigned int>(restPkg[i][LF_TAG_ROLE]);

                rlstLifInfo.push_back(stLif);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }
    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showlun()
Description  : Query LUN information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstLUNInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showlun(list<LUN_INFO_STRU> &rlstLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstLUNInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << RESTURL_LUN << "?range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                LUN_INFO_STRU stLUNInfo;

                stLUNInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                stLUNInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();

                unsigned long long ullCapacity = jsonValue2Type<unsigned long long>(restPkg[i][LUN_TAG_CAPACITY]);
                stringstream ssCapacity;
                ssCapacity<<ullCapacity;
                stLUNInfo.strCapacity = ssCapacity.str();

                stLUNInfo.strOwnerControlID = restPkg[i][LUN_TAG_OWNINGCONTROLLER].asString();

                rlstLUNInfo.push_back(stLUNInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showmap()
Description  : Query mapping information.
Data Accessed: None.
Data Updated : None.
Input        : stHostIDInfo
               rlstMapInfo
Output       : rlstMapInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showmap(IN HOST_ID_INFO_STRU &stHostIDInfo, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    int iRet = RETURN_OK;
    string strInbandLUNWWN;
    string strTmpWWN;
    string strHostGroupID;
    string strLunGroupID;
    list<LUN_INFO_STRU> lstLunInfos;
    list<HYIMAGE_INFO_STRU> lstSnapInfos;
    list<CMDHOSTINFO_STRU> lstCmdHostInfo;
    list<CMDHOSTINFO_STRU>::iterator iterCmdHostInfo;
    CRestPackage restPkg;

    string strIP;
    ostringstream oss;
    oss.str("");

    if(stHostIDInfo.IsMapToGroup){
        strHostGroupID = stHostIDInfo.strHostGroupID;
    }
    else{
        iRet = CMD_showhost(lstCmdHostInfo);
        if (RETURN_OK != iRet){
            return iRet;
        }

        for (iterCmdHostInfo = lstCmdHostInfo.begin(); iterCmdHostInfo != lstCmdHostInfo.end(); ++iterCmdHostInfo){
            list<HOST_PORT_INFO_STRU> lstHostPort;
            list<HOST_PORT_INFO_STRU>::iterator iterHostPort;

            if(stHostIDInfo.strHostGroupID == iterCmdHostInfo->strID){
                strHostGroupID = iterCmdHostInfo->strHostGroupID;
                break;
            }
        }
    }

    if (strHostGroupID.empty()){
        COMMLOG(OS_LOG_ERROR, "Cannot find any hostgroup...");
        return RETURN_OK;
    }

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss <<RESTURL_MAPPINGVIEW_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strHostGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        
        if(restConn->getVstoreID().compare(STR_NOT_EXIST) != 0){
            COMMLOG(OS_LOG_ERROR, "Query mappingview info is not permitted in vstore view...");
            return RETURN_OK;
        }

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    string strMapID = restPkg[0][COMMON_TAG_ID].asString();
    strTmpWWN = restPkg[0][MAPPINGVIEW_TAG_INBANDLUNWWN].asString();
    CMD_addFilterToWWN(strTmpWWN, strInbandLUNWWN);

    //Query LUN group according to MappingView
    iRet = CMD_showLunGroupByMap(strMapID, strLunGroupID);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "CMD_showLunGroupByMap failed, errorcode=%d", iRet);
        return iRet;
    }

    //Query the host group according to MappingView
    iRet = CMD_showHostGroupByMap(strMapID, strHostGroupID);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "CMD_showHostGroupByMap failed, errorcode=%d", iRet);
        return iRet;
    }

    //Query LUNs based on LUN groups 
    lstLunInfos.clear();
    if(!strLunGroupID.empty() && !strHostGroupID.empty()){
        iRet = CMD_showLunByLunGroup(strLunGroupID, lstLunInfos);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "CMD_showLunByLunGroup failed, errorcode=%d", iRet);
            return iRet;
        }
    }

    //Query the LUN mapping table
    int j = 0;
    for (list<LUN_INFO_STRU>::const_iterator iter = lstLunInfos.begin(); iter != lstLunInfos.end(); iter++){
        LUN_INFO_STRU stLUNInfo = *iter;
        MAP_INFO_STRU stMapInfo;

        stMapInfo.strHostGroupID = strHostGroupID;
        stMapInfo.strDevLUNID = stLUNInfo.strID;
        stMapInfo.strMapID = stMapInfo.strHostGroupID + ";" + stMapInfo.strDevLUNID + ";" + strMapID;

        stringstream ssID;
        ssID<<j;
        stMapInfo.strHostLUNID = ssID.str();

        stMapInfo.uiIsCmdLUN = 0;
        stMapInfo.uiIsSnap = 0;
        stMapInfo.strLUNWWN = stLUNInfo.strWWN;
        if (stLUNInfo.strWWN == strInbandLUNWWN){
            stMapInfo.uiIsCmdLUN = 1;
        }

        rlstMapInfo.push_back(stMapInfo);

        j++;
    }

    //Query snapshot mapping table
    lstSnapInfos.clear();
    if(!strLunGroupID.empty() && !strHostGroupID.empty()){
        iRet = CMD_showSnapShotByLUNGroup(strLunGroupID, lstSnapInfos);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "CMD_showSnapShotByLUNGroup failed, errorcode=%d", iRet);
            return iRet;
        }
    }

    j = 0;
    for (list<HYIMAGE_INFO_STRU>::const_iterator iter = lstSnapInfos.begin(); iter != lstSnapInfos.end(); iter++){
        HYIMAGE_INFO_STRU rstHyImageInfo = *iter;
        MAP_INFO_STRU stMapInfo;

        stMapInfo.strHostGroupID = strHostGroupID;
        stMapInfo.strDevLUNID = rstHyImageInfo.strID;
        stMapInfo.strMapID = stMapInfo.strHostGroupID + ";" + stMapInfo.strDevLUNID + ";" + strMapID;

        stringstream ssID;
        ssID<<j;
        stMapInfo.strHostLUNID = ssID.str();

        stMapInfo.uiIsCmdLUN = 0;
        stMapInfo.uiIsSnap = 1;
        stMapInfo.strLUNWWN = rstHyImageInfo.strWWN;
        rlstMapInfo.push_back(stMapInfo);

        j++;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showmap()
Description  : Query mapping information. Return to the host group ID directly. The host group ID is also used when adding or deleting.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstMapInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    int iRet = RETURN_OK;
    string strInbandLUNWWN;
    string strTmpWWN;
    string strHostGroupID;
    list<LUN_INFO_STRU> lstLunInfos;
    list<HYIMAGE_INFO_STRU> lstSnapInfos;
    string strLunGroupID;
    CRestPackage restPkg;
    ostringstream oss;

    //The Dorado NAS V300R002C10 does not support the query mapping view and returns OK directly.
    SYSTEM_INFO_STRU sysInfo;
    iRet = CMD_showsys(sysInfo);
    if(iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "show system info fail, the iRet is (%d).", iRet);
        return iRet;
    }
    
    if (PRODUCT_MODEL_DORADO_NAS == sysInfo.uiProductMode && sysInfo.strProductVersion == PRODUCT_VERSION_SUPPORT_CLONEFS_DORADO){
        COMMLOG(OS_LOG_INFO, "Query mapping view is not permitted in Dorado NAS: V300R002C10.");
        return RETURN_OK;
    }

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_MAPPINGVIEW <<"?range=[0-" <<MAX_MAPPVIEW_COUNT <<"]";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        if(restConn->getVstoreID().compare(STR_NOT_EXIST) != 0){
            COMMLOG(OS_LOG_WARN, "Query mapping view is not permitted in vstore view.");
            return RETURN_OK;
        }

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_MAPPINGVIEW, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_MAPPINGVIEW, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        rlstMapInfo.clear();
        CRESTConn *pRestConn = restConn;
        for (size_t i = 0; i < restPkg.count(); ++i){
            string strMapID = restPkg[i][COMMON_TAG_ID].asString();
            strTmpWWN = restPkg[i][MAPPINGVIEW_TAG_INBANDLUNWWN].asString();

            CMD_addFilterToWWN(strTmpWWN, strInbandLUNWWN);

            iRet = CMD_showLunGroupByMap(strMapID, strLunGroupID, pRestConn);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "CMD_showLunGroupByMap faild, errocode=%d", iRet);
                continue;
            }
            if (strLunGroupID.empty()){
                COMMLOG(OS_LOG_WARN, "Not found lun group associated to map[%s]", strMapID.c_str());
                continue;
            }

            pRestConn = restConn;
            iRet = CMD_showHostGroupByMap(strMapID, strHostGroupID, pRestConn);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "CMD_showHostGroupByMap failed, errorcode=%d", iRet);
                continue;
            }
            if (strHostGroupID.empty()){
                COMMLOG(OS_LOG_WARN, "Not found host group associated to map[%s]", strMapID.c_str());
                continue;
            }

            lstLunInfos.clear();
            pRestConn = restConn;
            iRet = CMD_showLunByLunGroup(strLunGroupID, lstLunInfos, pRestConn);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "CMD_showLunByLunGroup failed, errorcode=%d", iRet);
                continue;
            }
            if (!lstLunInfos.empty()){
                int j = 0;
                for (list<LUN_INFO_STRU>::const_iterator iterinfo = lstLunInfos.begin(); iterinfo != lstLunInfos.end(); iterinfo++){
                    LUN_INFO_STRU stLUNInfo = *iterinfo;
                    MAP_INFO_STRU stMapInfo;

                    stMapInfo.strHostGroupID = strHostGroupID;
                    stMapInfo.strDevLUNID = stLUNInfo.strID;
                    stMapInfo.strMapID = strMapID;
                    stMapInfo.strLunGroupID = strLunGroupID;
                    stringstream ssID;
                    ssID<<j;
                    stMapInfo.strHostLUNID = ssID.str();

                    stMapInfo.uiIsCmdLUN = 0;
                    stMapInfo.uiIsSnap = 0;
                    stMapInfo.strLUNWWN = stLUNInfo.strWWN;
                    if (stLUNInfo.strWWN == strInbandLUNWWN){
                        stMapInfo.uiIsCmdLUN = 1;
                    }
                    rlstMapInfo.push_back(stMapInfo);

                    j++;
                }
            }

            lstSnapInfos.clear();
            iRet = CMD_showSnapShotByLUNGroup(strLunGroupID, lstSnapInfos);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "CMD_showSnapShotByLUNGroup failed, errorcode=%d", iRet);
                continue;
            }
            if (!lstSnapInfos.empty()){
                int j = 0;
                for (list<HYIMAGE_INFO_STRU>::const_iterator iterhyimage = lstSnapInfos.begin(); iterhyimage != lstSnapInfos.end(); iterhyimage++){
                    HYIMAGE_INFO_STRU rstHyImageInfo = *iterhyimage;
                    MAP_INFO_STRU stMapInfo;

                    stMapInfo.strHostGroupID = strHostGroupID;
                    stMapInfo.strDevLUNID = rstHyImageInfo.strID;
                    stMapInfo.strMapID = strMapID;
                    stMapInfo.strLunGroupID = strLunGroupID;

                    stringstream ssID;
                    ssID<<j;
                    stMapInfo.strHostLUNID = ssID.str();

                    stMapInfo.uiIsCmdLUN = 0;
                    stMapInfo.uiIsSnap = 1;
                    stMapInfo.strLUNWWN = rstHyImageInfo.strWWN;
                    rlstMapInfo.push_back(stMapInfo);

                    j++;
                }
            }

            if (lstLunInfos.empty() && lstSnapInfos.empty()){
                COMMLOG(OS_LOG_WARN, "Not found luns or snapshots in lun group[%s]", strLunGroupID.c_str());
            }
        }
        return RETURN_OK;
    }
    return RETURN_ERR;
}
/*------------------------------------------------------------
Function Name: CMD_shownfs()
Description  : Query NFS share information.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstnfsInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_shownfs(IN string vstoreID, OUT list<NFS_INFO_STRU> &rlstnfsInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstnfsInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << RESTURL_SNAS_NFS_SHARE << "?range=[" << rangeIndex << "-" << (rangeIndex + SPECIAL_RANGE_COUNT) << "]";
            if ((!vstoreID.empty()) && (vstoreID.compare(STR_NOT_EXIST) != 0)){
                tmposs << "&vstoreId=" << vstoreID;
            }
        
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            
            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                NFS_INFO_STRU stnfsInfo;

                stnfsInfo.strFsID = restPkg[i][NFS_TAG_FSID].asString();
                stnfsInfo.strNfsID = restPkg[i][COMMON_TAG_ID].asString();
                stnfsInfo.strSharePath = restPkg[i][NFS_TAG_SHAREPATH].asString();
                if(restPkg[i].isMember(COMMON_TAG_VSTOREID)){
                    stnfsInfo.strVstoreID = restPkg[i][COMMON_TAG_VSTOREID].asString();
                }
                else{
                    stnfsInfo.strVstoreID = STR_NOT_EXIST;
                }
                rlstnfsInfo.push_back(stnfsInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_shownfs_byfsid()
Description  : Query NFS share information.
Data Accessed: None.
Data Updated : None.
Input        : fsid: 
Output       : rlstnfsInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_shownfs_byfsid(IN const string& vstoreid, IN string fsid, OUT list<NFS_INFO_STRU> &rlstnfsInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstnfsInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << RESTURL_SNAS_NFS_SHARE << "?filter=FSID::" << fsid << "&range=[" << rangeIndex << "-" << (rangeIndex + SPECIAL_RANGE_COUNT) << "]";

            if((!vstoreid.empty()) && (vstoreid.compare(STR_NOT_EXIST) != 0)){
                tmposs << "&vstoreId=" << vstoreid;
            }

            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                continue;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                        iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                NFS_INFO_STRU stnfsInfo;

                stnfsInfo.strFsID = restPkg[i][NFS_TAG_FSID].asString();
                stnfsInfo.strNfsID = restPkg[i][COMMON_TAG_ID].asString();
                stnfsInfo.strSharePath = restPkg[i][NFS_TAG_SHAREPATH].asString();
                rlstnfsInfo.push_back(stnfsInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}
/*------------------------------------------------------------
Function Name: CMD_addnfs()
Description  : add NFS share.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstnfsInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_addnfs(IN const string& vstoreid, IN string &strFSID,OUT string &rstrNFSID,OUT string &rstrsharPath)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string sharName;
    string sharpath;
    FS_INFO_STRU stFsInfo; 
    stFsInfo.strID = strFSID;
    iRet = CMD_showfs(stFsInfo);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "show CMD_showfs failed (%d)",iRet);
        return RETURN_ERR;
    }
    if (!stFsInfo.strPath.empty()){
        rstrsharPath = stFsInfo.strPath;
        rstrNFSID = stFsInfo.strNFSID;
        COMMLOG(OS_LOG_ERROR, "NFS is exist shareID (%s) share path(%s)",rstrNFSID.c_str(),stFsInfo.strPath.c_str());
        return 1077939724;//共享已存在
    }
    sharName = "/" + stFsInfo.strName + "/";
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_SNAS_NFS_SHARE;
    Json::Value jSendData;
    jSendData["FSID"] = strFSID;
    jSendData["SHAREPATH"] = sharName;

    //vstoreid is valid
    if((!vstoreid.empty()) && (vstoreid.compare(STR_NOT_EXIST) != 0)){
        jSendData["vstoreId"] = vstoreid;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        rstrNFSID = restPkg[0][COMMON_TAG_ID].asString();
        sharpath = restPkg[0][NFS_TAG_SHAREPATH].asString();
        std::string::size_type pos  = sharpath.find_last_of("/");
        if (pos == sharpath.size()-1){
            rstrsharPath = sharpath.substr(0,pos);
        }
        else{
            rstrsharPath = sharpath;
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_delnfs()
Description  : delete NFS share information.
Data Accessed: None.
Data Updated : None.
Input        : strId
Output       : .
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_delnfs(IN const string& vstoreid, IN const string& strId)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
 
    if (strId.empty()){
        COMMLOG(OS_LOG_ERROR,"%s","CMD_delnfs strId.empty().");
        return RETURN_ERR;
    }
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_SNAS_NFS_SHARE<<"/"<<strId;

    //vstoreid is valid
    if((!vstoreid.empty()) && (vstoreid.compare(STR_NOT_EXIST) != 0)){
        oss << "?vstoreId=" << vstoreid;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_delclonefs()
Description  : Delete clone file system information.
Data Accessed: None.
Data Updated : None.
Input        : strId
Output       : .
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_delclonefs(IN const string &strId)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
 
    if (strId.empty()){
        COMMLOG(OS_LOG_ERROR,"%s","CMD_delclonefs strId.empty().");
        return RETURN_ERR;
    }
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_FILESYSTEM << "/" << strId << "?ISDELETEPARENTSNAPSHOT=true";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_addnfsclient()
Description  : Add NFS share permissions control.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_addnfsclient(IN const string& vstoreid, IN string &strIP,IN string &strfsID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<"/NFS_SHARE_AUTH_CLIENT";
    Json::Value jSendData;
    jSendData[COMMON_TAG_TYPE] = OBJ_NFS_SHARE_AUTH_CLIENT;
    jSendData[COMMON_TAG_NAME] = strIP;
    jSendData[COMMON_TAG_PARENTID] = strfsID;
    jSendData["ACCESSVAL"] = 1;  //Read/Write
    jSendData["SYNC"] = 0;       //synchronization
    jSendData["ALLSQUASH"] = 1;  //no_all_squash
    jSendData["ROOTSQUASH"] = 1; //no_root_squash

    if((!vstoreid.empty()) && (vstoreid.compare(STR_NOT_EXIST) != 0)){
        jSendData["vstoreId"] = vstoreid;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showiscsihostport()
Description  : Query the iscsi port information.
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : rlstHostPortInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showiscsihostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_ISCSI_INITIATOR <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_ISCSI_INITIATOR
        <<"&" <<COMMON_TAG_PARENTTYPE <<"=" <<(int)OBJ_HOST
        <<"&" <<COMMON_TAG_PARENTID <<"=" <<strHostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHostPortInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            
            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                HOST_PORT_INFO_STRU stHostPortInfo;

                stHostPortInfo.strHostID = strHostID;

                stHostPortInfo.strIQN = restPkg[i][COMMON_TAG_ID].asString();
                stHostPortInfo.strChapName = restPkg[i][COMMON_TAG_CHAPNAME].asString();
                stHostPortInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
                stHostPortInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                ConvertHostPortStatus(stHostPortInfo.uiState);
                stHostPortInfo.uifcOriscsi = 0; 
                rlstHostPortInfo.push_back(stHostPortInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showfchostport()
Description  : Query the fc port information.
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : rlstHostPortInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showfchostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    oss.str("");
    oss <<RESTURL_FC_INITIATOR <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_FC_INITIATOR
        <<"&" <<COMMON_TAG_PARENTTYPE <<"=" <<(int)OBJ_HOST
        <<"&" <<COMMON_TAG_PARENTID <<"=" <<strHostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        rlstHostPortInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                HOST_PORT_INFO_STRU stHostPortInfo;

                stHostPortInfo.strHostID = strHostID;

                stHostPortInfo.strIQN = restPkg[i][COMMON_TAG_ID].asString();
                stHostPortInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
                stHostPortInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                ConvertHostPortStatus(stHostPortInfo.uiState);
                stHostPortInfo.uifcOriscsi = 1;

                rlstHostPortInfo.push_back(stHostPortInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhostport()
Description  : Query the port information, first query the iSCSI port information, if not, find the fc port information.
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : rlstHostPortInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo)
{
    list<HOST_PORT_INFO_STRU>::iterator itHost;
    rlstHostPortInfo.clear();
    int iRet = RETURN_ERR;

    CMD_showiscsihostport(strHostID, rlstHostPortInfo);
    for(itHost = rlstHostPortInfo.begin(); itHost != rlstHostPortInfo.end();++itHost){
        if(!itHost->strIQN.empty() && itHost->strIQN.size() > 1){
            return RETURN_OK;
        }
    }
    rlstHostPortInfo.clear();
    iRet = CMD_showfchostport(strHostID, rlstHostPortInfo);
    if (RETURN_OK == iRet){
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showhostlink()
Description  : Query host link information.
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : rlstHostLinkInfo
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showhostlink(IN string initatorWwn,IN unsigned int nFlag,OUT list<HOST_LINK_INFO> &rlstHostLinkInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    ostringstream tmposs;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");

    if(nFlag == 0){
        oss <<RESTURL_HOST_LINK <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOST_LINK
            <<"&" <<HOSTLINK_INITIATOR_TYPE <<"=" <<(int)OBJ_ISCSI_INITIATOR
            <<"&" <<HOSTLINK_INITIATOR_PORT_WWN <<"=" <<initatorWwn
            <<"&"<<HOSTLINK_INITIATOR_NODE_WWN<<"="    <<"";
    }
    else{
        oss <<RESTURL_HOST_LINK <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOST_LINK
            <<"&" <<HOSTLINK_INITIATOR_TYPE <<"=" <<(int)OBJ_FC_INITIATOR
            <<"&" <<HOSTLINK_INITIATOR_PORT_WWN <<"=" <<initatorWwn
            <<"&" <<HOSTLINK_INITIATOR_NODE_WWN <<"="<<initatorWwn;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        rlstHostLinkInfo.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";

            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            
            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                HOST_LINK_INFO stHostLink;

                stHostLink.target_id = restPkg[i][HOSTLINK_TARGET_ID].asString();
                stHostLink.target_port_wwn = restPkg[i][HOSTLINK_TARGET_PORT_WWN].asString();

                rlstHostLinkInfo.push_back(stHostLink);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showLunGroupByMap()
Description  : Query the corresponding LUN group through the mapping view. The mapping view and LUN group and host group are one-to-one.
Data Accessed: None.
Data Updated : None.
Input        : strMapID
Output       : strLunGroupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strIP;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_LUNGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strMapID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() > 0){
        strLunGroupID = restPkg[0][COMMON_TAG_ID].asString();
    }
    else{
        strLunGroupID.clear();
        COMMLOG(OS_LOG_INFO, "Not found lungroup associated to mapping view[%s]", strMapID.c_str());
    }

    
    return RETURN_OK;    
}

/*------------------------------------------------------------
Function Name: CMD_showLunGroupByMap()
Description  : Query the corresponding LUN group through the mapping view. The mapping view and LUN group and host group are one-to-one.
Data Accessed: None.
Data Updated : None.
Input        : strMapID
Output       : strLunGroupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID,CRESTConn *restConn)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strIP;
 
    oss.str("");
    oss <<RESTURL_LUNGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strMapID;

    iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d).", oss.str().c_str(), iRet);
        return RETURN_ERR;
    }
    if (restPkg.errorCode() != RETURN_OK){
        iRet = restPkg.errorCode();
        COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d), description %s.", 
             oss.str().c_str(), iRet, restPkg.description().c_str());
        return restPkg.errorCode();
    }

    if (restPkg.count() > 0){
        strLunGroupID = restPkg[0][COMMON_TAG_ID].asString();
    }
    else{
        strLunGroupID.clear();
        COMMLOG(OS_LOG_INFO, "url [%s] restPkg.count== %d", oss.str().c_str(),restPkg.count());
    }

    return RETURN_OK;    
}

/*------------------------------------------------------------
Function Name: CMD_showHostGroupByMap()
Description  : Query the corresponding Host group through the mapping view. The mapping view and LUN group and host group are one-to-one.
Data Accessed: None.
Data Updated : None.
Input        : strMapID
Output       : strHostGroupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strIP;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HOSTGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strMapID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() > 0){
        strHostGroupID = restPkg[0][COMMON_TAG_ID].asString();
    }
    else{
        strHostGroupID.clear();
        COMMLOG(OS_LOG_INFO, "Not found hostgroup associated to mapping view[%s]", strMapID.c_str());
    }

    return RETURN_OK;    
}

/*------------------------------------------------------------
Function Name: CMD_showHostGroupByMap()
Description  : Query the corresponding host group through the mapping view. The mapping view and LUN group and host group are one-to-one.
Data Accessed: None.
Data Updated : None.
Input        : strMapID,  restConn
Output       : strHostGroupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID,CRESTConn *restConn)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

    oss.str("");
    oss <<RESTURL_HOSTGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strMapID;

    iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d).",  oss.str().c_str(), iRet);
        return RETURN_ERR;
    }

    if (restPkg.errorCode() != RETURN_OK){
        iRet = restPkg.errorCode();
        COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d), description %s.", oss.str().c_str(), iRet, restPkg.description().c_str());
        return restPkg.errorCode();
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "url [%s] restPkg.count==1.", oss.str().c_str());
        return RETURN_ERR;
    }

    strHostGroupID = restPkg[0][COMMON_TAG_ID].asString();
    return RETURN_OK;    
}

/*------------------------------------------------------------
Function Name: CMD_showLunByLunGroup()
Description  : Query the corresponding LUN through the LUN group.
Data Accessed: None.
Data Updated : None.
Input        : strLunGroupID
Output       : lstLuns
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;
    ostringstream tmposs;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_LUN_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_LUN
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strLunGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        lstLuns.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);

            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                LUN_INFO_STRU rstLUNInfo;
                rstLUNInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                CMD_showlun(rstLUNInfo);            
                lstLuns.push_back(rstLUNInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }
        
        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showLunByLunGroup()
Description  : Query the corresponding LUN through the LUN group.
Data Accessed: None.
Data Updated : None.
Input        : strLunGroupID, restConn
Output       : lstLuns
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns,CRESTConn *restConn)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;
    ostringstream tmposs;
    oss.str("");
 
    oss <<RESTURL_LUN_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_LUN
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strLunGroupID;

    lstLuns.clear();

    int rangeIndex = 0;
    while(true){
        tmposs.str("");
        tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
        iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d).", tmposs.str().c_str(), iRet);
            return RETURN_ERR;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d), description %s.",  tmposs.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        
        size_t i = 0;
        for (; i < restPkg.count(); ++i){
            LUN_INFO_STRU rstLUNInfo;
            rstLUNInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
            CMD_showlun(rstLUNInfo);            
            lstLuns.push_back(rstLUNInfo);
        }
        if (RECOMMEND_RANGE_COUNT == i){
            rangeIndex += RECOMMEND_RANGE_COUNT;
        }
        else{
            break;
        }
    }
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showSnapShotByLUNGroup()
Description  : Query the corresponding snapshot through the host group
Data Accessed: None.
Data Updated : None.
Input        : strLunGroupID.
Output       : lstSnapShots
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showSnapShotByLUNGroup(const string &strLunGroupID, list<HYIMAGE_INFO_STRU> &lstSnapShots)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;
    ostringstream tmposs;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_SNAPSHOT_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_SNAPSHOT
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strLunGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        lstSnapShots.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";

            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            
            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                HYIMAGE_INFO_STRU rstHyImageInfo;
        
                rstHyImageInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                rstHyImageInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                rstHyImageInfo.uiSnapType = jsonValue2Type<unsigned int>(restPkg[i][SNAPSHOT_TAG_SNAPSHOTUSER]);
                SnapshotConvertType(rstHyImageInfo.uiSnapType);

                rstHyImageInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_HEALTHSTATUS]);
                rstHyImageInfo.uiState = jsonValue2Type<unsigned int>(restPkg[i][COMMON_TAG_RUNNINGSTATUS]);
                SnapshotConvertStatus(rstHyImageInfo.uiStatus,rstHyImageInfo.uiState);

                unsigned int uiRollBackProgress = jsonValue2Type<unsigned int>(restPkg[i][SNAPSHOT_TAG_ROLLBACKRATE]);
                if (0xFFFFFFFF == uiRollBackProgress){
                    rstHyImageInfo.uiRollBackProgress = 100;
                }
                else{
                    rstHyImageInfo.uiRollBackProgress = uiRollBackProgress;
                }

                unsigned long long ullTimeStamp = jsonValue2Type<unsigned long long>(restPkg[i][SNAPSHOT_TAG_TIMESTAMP]);
                TimeChg(ullTimeStamp,rstHyImageInfo.strTimeStamp);

                strWWN = restPkg[i][SNAPSHOT_TAG_WWN].asString();

                CMD_addFilterToWWN(strWWN, rstHyImageInfo.strWWN);

                lstSnapShots.push_back(rstHyImageInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }

        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

int CRESTCmd::CMD_showLUNGroupByName(const string &strLunGroupName, string &strLunGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_LUNGROUP_FILTER << "=" 
        << COMMON_TAG_NAME << "::" << strLunGroupName;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (restPkg.count() == 0){
        strLunGroupID.clear();
    }
    else{
        strLunGroupID = restPkg[0][COMMON_TAG_ID].asString();
        COMMLOG(OS_LOG_WARN, "the LunGroup [%s] has existed, LunGroup ID is [%s].",  strLunGroupName.c_str(), strLunGroupID.c_str());        
    }

    return RETURN_OK;
}

int CRESTCmd::CMD_showHostGroupByName(const string &strHostGroupName, string &strHostGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HOSTGROUP_FILTER << "=" 
        << COMMON_TAG_NAME << "::" << strHostGroupName;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (restPkg.count() == 0){
        strHostGroupID.clear();
    }
    else{
        strHostGroupID = restPkg[0][COMMON_TAG_ID].asString();
        COMMLOG(OS_LOG_WARN, "the HostGroup [%s] has existed, HostGroup ID is [%s].",  strHostGroupName.c_str(), strHostGroupID.c_str());        
    }

    return RETURN_OK;
}

int CRESTCmd::CMD_showMappingByName(const string &strMappingName, string &mapID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_MAPPINGVIEW_FILTER << "=" 
        << COMMON_TAG_NAME << "::" << strMappingName;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (restPkg.count() == 0){
        mapID.clear();
    }
    else{
        mapID = restPkg[0][COMMON_TAG_ID].asString();
        COMMLOG(OS_LOG_WARN, "the Mapping [%s] has existed, Mapping ID is [%s].",  strMappingName.c_str(), mapID.c_str());        
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showLunGroupByLUN()
Description  : Query the LUN group to which the LUN belongs.
Data Accessed: None.
Data Updated : None.
Input        : strMapName
Output       : strMapID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showLunGroupByLUN(const string &strLunID, list<string> &strLunGroupID, int objType)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_LUNGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<objType
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strLunID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any lun group by lun[%s].", strIP.c_str(), oss.str().c_str(), strLunID.c_str());
        strLunGroupID.clear();
        return RETURN_OK;
    }

    size_t i = 0;
    for (; i < restPkg.count(); i++){
        strLunGroupID.push_back(restPkg[i][COMMON_TAG_ID].asString());
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showMapbyLunGroup()
Description  : Querying mapping information through a LUN group
Data Accessed: None.
Data Updated : None.
Input        : strLunGroupID
Output       : lstMapID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showMapbyLunGroup(const string &strLunGroupID, list<string> &lstMapID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_MAPPINGVIEW_ASSOCIATE <<"?" 
        << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_LUNGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << strLunGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        strIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (iRet != RETURN_OK){
        return iRet;
    }

    lstMapID.clear();
    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any map by lun Group[%s].", strIP.c_str(), oss.str().c_str(), strLunGroupID.c_str());    
        return RETURN_OK;
    }

    size_t i = 0;
    for (; i < restPkg.count(); i++){
        lstMapID.push_back(restPkg[i][COMMON_TAG_ID].asString());
    }

    return RETURN_OK;
}

int CRESTCmd::CMD_showMapbyHostGroup(const string &strHostGroupID, list<string> &lstMapID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_MAPPINGVIEW_ASSOCIATE <<"?" 
        << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << strHostGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        strIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (iRet != RETURN_OK){
        return iRet;
    }

    lstMapID.clear();
    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any map by lun Group[%s].", strIP.c_str(), oss.str().c_str(), strHostGroupID.c_str());    
        return RETURN_OK;
    }

    size_t i = 0;
    for (; i < restPkg.count(); i++){
        lstMapID.push_back(restPkg[i][COMMON_TAG_ID].asString());
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showHostbyHostGroup()
Description  : Query the host through the host group
Data Accessed: None.
Data Updated : None.
Input        : HostGroupID
Output       : lstHostID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_HOST_ASSOCIATE <<"?" 
        << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << HostGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        strIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
    
        break;
    }

    if (iRet != RETURN_OK){
        return iRet;
    }

    lstHostID.clear();
    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any map by lun Group[%s].", strIP.c_str(), oss.str().c_str(), HostGroupID.c_str());    
        return RETURN_OK;
    }

    size_t i = 0;
    for (; i < restPkg.count(); i++){
        lstHostID.push_back(restPkg[i][COMMON_TAG_ID].asString());
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showHostGroupByHost()
Description  : Query the host group through the host
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : lstHostGrupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostGroupByHost(const string &strHostID, list<string> &lstHostGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_HOSTGROUP_ASSOCIATE <<"?" 
        << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_HOST
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << strHostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        strIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    if (iRet != RETURN_OK){
        return RETURN_ERR;
    }

    lstHostGroupID.clear();
    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any host group by host[%s].", strIP.c_str(), oss.str().c_str(), strHostID.c_str());    
        return RETURN_OK;
    }

    size_t i = 0;
    for (; i < restPkg.count(); i++){
        lstHostGroupID.push_back(restPkg[i][COMMON_TAG_ID].asString());
    }

    return RETURN_OK;
}

int CRESTCmd::CMD_removeLunGroupFromMap(const string &mapID, const string &lunGroupID)
{
    int iRet = RETURN_OK;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        // remove lungroup    
        CRestPackage restPkg;
        oss.str("");
        oss << RESTURL_MAPPINGVIEW_ASSOCIATE_DEL <<"?" 
            << COMMON_TAG_ID << "=" << mapID << "&"
            << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_LUNGROUP
            <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << lunGroupID;

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        
        break;
    }

    return iRet;
}

int CRESTCmd::CMD_removeHostGroupFromMap(const string &mapID, const string &hostGroupID)
{
    int iRet = RETURN_OK;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        //remove hostgroup        
        CRestPackage restPkg;
        oss.str("");
        oss << RESTURL_MAPPINGVIEW_ASSOCIATE_DEL <<"?" 
            << COMMON_TAG_ID << "=" << mapID << "&"
            << COMMON_TAG_ASSOCIATEOBJTYPE <<"=" << (int)OBJ_HOSTGROUP
            <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" << hostGroupID;

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
        
        break;
    }

    return iRet;
}

int CRESTCmd::CMD_delMappingByMapID(const string &mapID)
{
    int iRet = RETURN_OK;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        // delete the mapping
        CRestPackage restPkg;
        oss.str("");
        oss << RESTURL_MAPPINGVIEW << "/" << mapID;

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    return iRet;
}

int CRESTCmd::CMD_delMapping(const string& mapID, const string& hostGroupID, const string& lunGroupID)
{        
    if (mapID.empty()){
        COMMLOG(OS_LOG_ERROR, "the map id is empty, delete the mapping failed");
        return RETURN_ERR;
    }

    int iRet = RETURN_OK;

    //remove hostgroup    from map
    if (!hostGroupID.empty()){
        iRet = CMD_removeHostGroupFromMap(mapID, hostGroupID);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "remove hostGroup [%s] from map [%s].", hostGroupID.c_str(), mapID.c_str());
            return iRet;
        }
    }

    //remove lungroup from map
    if (!lunGroupID.empty()){
        iRet = CMD_removeLunGroupFromMap(mapID, lunGroupID);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "remove lungroup [%s] from map [%s].", lunGroupID.c_str(), mapID.c_str());
            return iRet;
        }
    }

    //delete map
    iRet = CMD_delMappingByMapID(mapID);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "delete map [%s] error.", mapID.c_str());
        return iRet;
    }

    return iRet;
}

int CRESTCmd::CMD_delLunGroup(const string& lunGroupID, const string& snapshotID, int objType)
{
    int iRet = RETURN_OK;

    if (!snapshotID.empty()){
        iRet = CMD_delLunFromLunGroup(snapshotID, lunGroupID, objType);
        if ( iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "snapshot [%s] can not remove from lunGroup [%s].", snapshotID.c_str(), lunGroupID.c_str());
            return iRet;
        }
    }

    CRestPackage restPkg;
    ostringstream oss;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_LUNGROUP << "/" << lunGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    return iRet;
}

int CRESTCmd::CMD_delHostFromHostGroup(const string& hostGroupID, const string& hostID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_HOST_ASSOCIATE << "?" 
        << COMMON_TAG_ID << "=" << hostGroupID << "&"
        << COMMON_TAG_ASSOCIATEOBJTYPE << "=" << (int) OBJ_HOST << "&"
        << COMMON_TAG_ASSOCIATEOBJID << "=" << hostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    return iRet;
}

int CRESTCmd::CMD_delHostGroup(const string& hostGroupID, const list<string>& lstHostIDs)
{
    int iRet = RETURN_OK;

    if (!lstHostIDs.empty()){
        list<string>::const_iterator itrHostID = lstHostIDs.begin();
        for (; itrHostID != lstHostIDs.end(); itrHostID++){
            iRet = CMD_delHostFromHostGroup(hostGroupID, *itrHostID);
            if ( iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "remove host[%s] from host group[%s] failed.", itrHostID->c_str(), hostGroupID.c_str());
                return iRet;
            }
        }
    }
    
    CRestPackage restPkg;
    ostringstream oss;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_HOSTGROUP << "/" << hostGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        break;
    }

    return iRet;
}

/*------------------------------------------------------------
Function Name: CMD_showHostGroupByHost()
Description  : Query the host group through the host
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : stringHostGrupID
Return       : Success or Failure
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostGroupByHost(const string &strHostID, string &strHostGrupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HOSTGROUP_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_HOST
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strHostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() != 1){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] restPkg.count==1.", strIP.c_str(), oss.str().c_str());
        return RETURN_ERR;
    }

    strHostGrupID = restPkg[0][COMMON_TAG_ID].asString();
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showMapByHostGroup()
Description  : Query Mappingview through host group
Data Accessed: None.
Data Updated : None.
Input        : strHostID
Output       : stringHostGrupID
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showMapByHostGroup(const string &strHostGrupID, string &strMapID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_MAPPINGVIEW_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_MAPPINGVIEW
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strHostGrupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        strIP = *iter;
        break;
    }

    if (restPkg.count() > 0){
        strMapID = restPkg[0][COMMON_TAG_ID].asString();
    }
    else{
        strMapID.clear();
        COMMLOG(OS_LOG_INFO, "HostGroup is not associated to mapping view", strIP.c_str(), oss.str().c_str());
    }

    
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_showHostByHostGroup()
Description  : Query the host through the host group
Data Accessed: None.
Data Updated : None.
Input        : strHostGroupID
Output       : lstHosts
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showHostByHostGroup(const string &strHostGroupID, list<CMDHOSTINFO_STRU> &lstHosts)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    ostringstream tmposs;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss << RESTURL_HOST_ASSOCIATE <<"?" <<COMMON_TAG_TYPE <<"=" <<(int)OBJ_HOST
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<(int)OBJ_HOSTGROUP
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strHostGroupID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        lstHosts.clear();

        while(true){
            tmposs.str("");
            tmposs << oss.str() << "&range=[" << rangeIndex << "-" << (rangeIndex + RECOMMEND_RANGE_COUNT) << "]";
            iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
                flag = false;
                break;
            }

            if (restPkg.errorCode() != RETURN_OK){
                iRet = restPkg.errorCode();
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), tmposs.str().c_str(), iRet, restPkg.description().c_str());
                return restPkg.errorCode();
            }

            size_t i = 0;
            for (; i < restPkg.count(); ++i){
                tag_CMD_HOST_INFO rstHostInfo;

                rstHostInfo.strHostGroupID = strHostGroupID;
                rstHostInfo.strID = restPkg[i][COMMON_TAG_ID].asString();
                rstHostInfo.strName = restPkg[i][COMMON_TAG_NAME].asString();
                rstHostInfo.strHostIP = restPkg[i][HOST_TAG_IP].asString();

                lstHosts.push_back(rstHostInfo);
            }

            if (RECOMMEND_RANGE_COUNT == i){
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }
            else{
                break;
            }
        }
        
        if (flag)
            return RETURN_OK;
        else
            continue;
    }

    return RETURN_ERR;
}

int CRESTCmd::CMD_showHostByLUN(const string &lunID, list<string> &lstHosts, const int objType)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");

    oss << RESTURL_HOST_ASSOCIATE << "?" <<COMMON_TAG_ASSOCIATEOBJTYPE << "=" << objType
        << "&" <<COMMON_TAG_ASSOCIATEOBJID << "=" << lunID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        lstHosts.clear();

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d).",  oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "url [%s] the iRet is (%d), description %s.", oss.str().c_str(), iRet, restPkg.description().c_str());
            return iRet;
        }

        size_t i = 0;
        for (; i < restPkg.count(); ++i){
            string  strHostID = restPkg[i][COMMON_TAG_ID].asString();
            lstHosts.push_back(strHostID);
        }

        return RETURN_OK;
    }

    return RETURN_ERR;    
}

/*------------------------------------------------------------
Function Name: CMD_addLun2LunGroup()
Description  : Add a LUN or snapshot to a LUN group.
Data Accessed: None.
Data Updated : None.
Input        : strLunID:lunID, strLunGroupID lungroupID
Output       : 
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_addLun2LunGroup(const string &strLunID, const string &strLunGroupID, int objType)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strLunGroupID;
    jSendData[COMMON_TAG_ASSOCIATEOBJTYPE] = objType;
    jSendData[COMMON_TAG_ASSOCIATEOBJID] = strLunID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_LUNGROUP_ASSOCIATE, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_LUNGROUP_ASSOCIATE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_LUNGROUP_ASSOCIATE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_delLunFromLunGroup()
Description  : Delete a LUN or snapshot from a LUN group
Data Accessed: None.
Data Updated : None.
Input        : strLunID:lunID, strLunGroupID lungroupID
Output       : 
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_delLunFromLunGroup(const string &strLunID, const string &strLunGroupID, int objType)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_LUNGROUP_ASSOCIATE <<"?" <<COMMON_TAG_ID <<"=" <<strLunGroupID
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<objType
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<strLunID;


    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

int CRESTCmd::CMD_showlunsbyconsisid(const string& consis_id,vector<string>& local_lun_ids)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss<<RESTURL_REPLICATIONPAIR<<"?"<<"filter=CGID::"<<consis_id;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }
         for (int i = 0; i < restPkg.count(); ++i){
            local_lun_ids.push_back(restPkg[i][REPLICATIONPAIR_TAG_LOCALRESID].asString());
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: UpdateGroupStatus
Description  : Convert consistency group state to integer
Data Accessed:
Data Updated : None.
Input        : 
Output       : 
Return       :
Call         :
Called by    :
Create By    : 
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::UpdateGroupStatus(IN OUT unsigned int &ruiIntStatus)
{
    map<unsigned int, unsigned int> mapGroupStatus;
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_NORMAL, MIRROR_GROUP_STATUS_NORAML));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_SYNCHRONIZING, MIRROR_GROUP_STATUS_SYNCHRONIZING));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_INTERRUPTED, MIRROR_GROUP_STATUS_INTERRUPTED));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_TOBERECOVERD, MIRROR_GROUP_STATUS_TOBERECOV));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_SPLIT, MIRROR_GROUP_STATUS_SPLITED));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_INVALID, MIRROR_GROUP_STATUS_INVALID));
    mapGroupStatus.insert(make_pair(RUNNING_STATUS_UNKNOWN, MIRROR_GROUP_STATUS_FAULT));

    if (mapGroupStatus.find(ruiIntStatus) == mapGroupStatus.end()){
        ruiIntStatus = MIRROR_GROUP_STATUS_NOT_EXIST;
        return;
    }

    ruiIntStatus = mapGroupStatus[ruiIntStatus];
}

/*------------------------------------------------------------
Function Name: UpdateModel
Description  : Mode conversion
Data Accessed:
Data Updated : 
Input        : 
Output       : 
Return       :
Call         :
Called by    :
Create By    : 
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::UpdateModel(IN OUT unsigned int &ruiMode)
{
    map<unsigned int, unsigned int> mapGroupMode;
    mapGroupMode.insert(make_pair(RM_MODEL_SYNC, MODE_SYNCHRONOUS));
    mapGroupMode.insert(make_pair(RM_MODEL_ASYNC, MODE_ASYNCHRONOUS));

    if (mapGroupMode.find(ruiMode) == mapGroupMode.end()){
        return;
    }

    ruiMode = mapGroupMode[ruiMode];
}

/*------------------------------------------------------------
Function Name: :SnapshotConvertStatus
Description  :  Virtual snapshot running state transition
Data Accessed:
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::SnapshotConvertStatus(IN const unsigned int uiHealthStatus, IN OUT unsigned int &ruiStatus)
{
    //Virtual snapshot health status: 1-Disable;2-Active;3-Rollback;4-Active(Fault);5-Disable(Fault).
    if (HEALTH_STATUS_FAULT != uiHealthStatus){
        if (RUNNING_STATUS_INACTIVATED == ruiStatus){
            ruiStatus = STATUS_IMAGE_CHAR_DISABLE;
        }
        else if (RUNNING_STATUS_ACTIVATED == ruiStatus){
            ruiStatus = STATUS_IMAGE_CHAR_ACTIVE;
        }
        else if (RUNNING_STATUS_ROLLBACK == ruiStatus){
            ruiStatus = STATUS_IMAGE_CHAR_ROLLBACK;
        }
        else{
            ruiStatus = STATUS_IMAGE_CHAR_INVALID;
        }
    }
    else{
        if (RUNNING_STATUS_ACTIVATED == ruiStatus){
            ruiStatus = STATUS_IMAGE_CHAR_ACTIVE_FAULT;
        }
        else if (RUNNING_STATUS_INACTIVATED == ruiStatus){
            ruiStatus = STATUS_IMAGE_CHAR_DISABLE_FAULT;
        }
        else{
            ruiStatus = STATUS_IMAGE_CHAR_INVALID;
        }
    }
}

/*------------------------------------------------------------
Function Name: SnapshotConvertType
Description  : Virtual snapshot type conversion
Data Accessed:
Data Updated : 
Input        : 
Output       : 
Return       :
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::SnapshotConvertType(IN OUT unsigned int &ruiType)
{
    map<unsigned int, unsigned int> mapGroupType;
    mapGroupType.insert(make_pair(SNAPSHOT_PUBLIC, TYPE_IMAGE_PUBLIC));
    mapGroupType.insert(make_pair(SNAPSHOT_PRIVATE, TYPE_IMAGE_PRIVATE_2));

    if (mapGroupType.find(ruiType) == mapGroupType.end()){
        ruiType = TYPE_IMAGE_INVALID;
        return;
    }

    ruiType = mapGroupType[ruiType];
}

/*------------------------------------------------------------
Function Name: UpdateMirrorStatus()
Description  : Remote replication status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::UpdateMirrorStatus(IN OUT unsigned int &ruiIntStatus)
{
    map<unsigned int, unsigned int> mapGroup;
    mapGroup.insert(make_pair(HEALTH_STATUS_NORMAL, MIRROR_STATUS_NORMAL));
    mapGroup.insert(make_pair(HEALTH_STATUS_FAULT, MIRROR_STATUS_FAULT));

    if (mapGroup.find(ruiIntStatus) == mapGroup.end()){
        ruiIntStatus = MORROR_STATUS_NOT_EXIST;
        return;
    }

    ruiIntStatus = mapGroup[ruiIntStatus];

    return;
}

/*------------------------------------------------------------
Function Name: ConvertMirrorPairStatus
Description  : Remote replication running status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertMirrorPairStatus(IN OUT unsigned int &ruiIntState)
{
    map<unsigned int, unsigned int> mapMirrorPairStatus;
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_NORMAL, MIRROR_SLAVE_PAIR_STATUS_CHAR_NORMAL));
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_SYNCHRONIZING, MIRROR_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING));
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_TOBERECOVERD, MIRROR_SLAVE_PAIR_STATUS_CHAR_TOBERECOVERED));
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_INTERRUPTED, MIRROR_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED));
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_SPLIT, MIRROR_SLAVE_PAIR_STATUS_CHAR_SPLITED));
    mapMirrorPairStatus.insert(make_pair(RUNNING_STATUS_INVALID, MIRROR_SLAVE_PAIR_STATUS_CHAR_INVALID));

    if (mapMirrorPairStatus.find(ruiIntState) == mapMirrorPairStatus.end()){
        ruiIntState = MIRROR_SLAVE_PAIR_STATUS_CHAR_NOT_EXIST;

        return;
    }

    ruiIntState = mapMirrorPairStatus[ruiIntState];

    return;
}

/*------------------------------------------------------------
Function Name: ConvertHMPairStatus
Description  : HyperMetroPair running status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertHMPairStatus(IN OUT unsigned int &ruiIntState)
{
    map<unsigned int, unsigned int> mapHMPairStatus;
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_NORMAL, HM_SLAVE_PAIR_STATUS_CHAR_NORMAL));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_SYNCHRONIZING, HM_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_INTERRUPTED, HM_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_INVALID, HM_SLAVE_PAIR_STATUS_CHAR_INVALID));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_PAUSE, HM_SLAVE_PAIR_STATUS_CHAR_PAUSE));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_TOBESYNCHRONIZED, HM_SLAVE_PAIR_STATUS_CHAR_TOBESYNCHRONIZED));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_ERROR, HM_SLAVE_PAIR_STATUS_CHAR_ERROR));

    if (mapHMPairStatus.find(ruiIntState) == mapHMPairStatus.end()){
        ruiIntState = HM_SLAVE_PAIR_STATUS_CHAR_NOT_EXIST;

        return;
    }

    ruiIntState = mapHMPairStatus[ruiIntState];

    return;
}

/*------------------------------------------------------------
Function Name: ConvertMirrorSlaveLUNStatus
Description  : MirrorSlaveLUN running status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertMirrorSlaveLUNStatus(IN OUT unsigned int &ruiIntState)
{
    map<unsigned int, unsigned int> mapMirrorSlaveLUNStatus;
    mapMirrorSlaveLUNStatus.insert(make_pair(RM_LUN_MIRROR_STATE_SYNCED, MIRROR_STATUS_CHAR_SYNCHRONIZED));
    mapMirrorSlaveLUNStatus.insert(make_pair(RM_LUN_MIRROR_STATE_CONSISTENT, MIRROR_STATUS_CHAR_CONSISTENT));
    mapMirrorSlaveLUNStatus.insert(make_pair(RM_LUN_MIRROR_STATE_SYNCING, MIRROR_STATUS_CHAR_SYNCHRONIZING));
    mapMirrorSlaveLUNStatus.insert(make_pair(RM_LUN_MIRROR_STATE_INIT_SYNCING, MIRROR_STATUS_CHAR_INITIALSYN));
    mapMirrorSlaveLUNStatus.insert(make_pair(RM_LUN_MIRROR_STATE_INCONSISTENT, MIRROR_STATUS_CHAR_INCONSISTENT));

    if (mapMirrorSlaveLUNStatus.find(ruiIntState) == mapMirrorSlaveLUNStatus.end()){
        ruiIntState = MIRROR_STATUS_CHAR_NOT_EXIST;

        return;
    }

    ruiIntState = mapMirrorSlaveLUNStatus[ruiIntState];

    return;
}

/*------------------------------------------------------------
Function Name: ConvertHMSlaveLUNStatus
Description  : HyperMetroSlaveLUN running status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertHMSlaveLUNStatus(IN OUT unsigned int &ruiIntState)
{
    map<unsigned int, unsigned int> mapHMSlaveLUNStatus;
    mapHMSlaveLUNStatus.insert(make_pair(RM_LUN_HM_STATE_SYNCED, HM_STATUS_CHAR_SYNCHRONIZED));
    mapHMSlaveLUNStatus.insert(make_pair(RM_LUN_HM_STATE_CONSISTENT, HM_STATUS_CHAR_CONSISTENT));
    mapHMSlaveLUNStatus.insert(make_pair(RM_LUN_HM_STATE_SYNCING, HM_STATUS_CHAR_SYNCHRONIZING));
    mapHMSlaveLUNStatus.insert(make_pair(RM_LUN_HM_STATE_INIT_SYNCING, HM_STATUS_CHAR_INITIALSYN));
    mapHMSlaveLUNStatus.insert(make_pair(RM_LUN_HM_STATE_INCONSISTENT, HM_STATUS_CHAR_INCONSISTENT));

    if (mapHMSlaveLUNStatus.find(ruiIntState) == mapHMSlaveLUNStatus.end()){
        ruiIntState = HM_STATUS_CHAR_NOT_EXIST;

        return;
    }

    ruiIntState = mapHMSlaveLUNStatus[ruiIntState];

    return;
}

/*------------------------------------------------------------
Function Name: ConvertSrcLUNStatus
Description  : LUN status conversion
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : 
Return       :  
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertSrcLUNStatus(IN const unsigned int uiHealthStatus, IN const unsigned int uiRunStatus, IN OUT unsigned int &ruiIntState)
{
    ruiIntState = INT_LUN_STATUS_NOT_EXIST;

    if(HEALTH_STATUS_NORMAL == uiHealthStatus){
        ruiIntState = INT_LUN_STATUS_NORMAL;
    }
    else if(HEALTH_STATUS_FAULT == uiHealthStatus){
        ruiIntState = INT_LUN_STATUS_FAULT;
    }

    if(RUNNING_STATUS_NOTFORMAT == uiRunStatus){
        ruiIntState = INT_LUN_STATUS_NOT_FORMATTED;
    }
    else if(RUNNING_STATUS_FORMATING == uiRunStatus){
        ruiIntState = INT_LUN_STATUS_NOT_FORMATTING;
    }
}

/*------------------------------------------------------------
Function Name: ConvertHostPortStatus()
Description  : Convert Link state from string type to integer type
Data Accessed:
Data Updated : None.
Input        : None.
Output       : None
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
-------------------------------------------------------------*/
void CRESTCmd::ConvertHostPortStatus(IN OUT unsigned int &ruiIntStatus)
{
    if (RUNNING_STATUS_ONLINE == ruiIntStatus){
        ruiIntStatus = RUNNING_STATUS_NORMAL;
    }

    else if (RUNNING_STATUS_OFFLINE == ruiIntStatus){
        ruiIntStatus = LINK_STATUS_UNCONNECT;
    }

    else{
        ruiIntStatus = (unsigned int)LINK_STATUS_INVALID;
    }

    return;
}

/*------------------------------------------------------------
Function Name: CMD_showarrayclonefs()
Description  : Query whether the array supports the clone file system
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : g_bnfs.
Return       : Success or Failure.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showarrayclonefs()
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    bool nfsFlag = false;

    SYSTEM_INFO_STRU sysInfo;
    iRet = CMD_showsys(sysInfo);
    if(iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "show system info fail, the iRet is (%d).", iRet);
        return iRet;
    }
    
    string sys_version = sysInfo.strProductVersion;
    unsigned int sys_model = sysInfo.uiProductMode;
    string patch_version = sysInfo.patchVersion;

    if(PRODUCT_MODEL_DORADO_5000V3 == sys_model || PRODUCT_MODEL_DORADO_6000V3 == sys_model){
        COMMLOG(OS_LOG_INFO, "Dorado Series Array not support clonefs.");
        nfsFlag = false;
    }
    else{
        if (sys_version > PRODUCT_VERSION_SUPPORT_CLONEFS){
            nfsFlag = true;
        }
        else if(sys_version < PRODUCT_VERSION_SUPPORT_CLONEFS){
            nfsFlag = false;
        }
        else{
            if((!patch_version.empty()) && (patch_version >= PRODUCT_PATCH_SUPPORT_CLONEFS))
                nfsFlag = true;
            else
                nfsFlag = false;
        }

        //Adapted to the Dorado NAS version of FileSysem, added V300R002C10 version
        if (sys_version == PRODUCT_VERSION_SUPPORT_CLONEFS_DORADO){
            nfsFlag = true;
        }
    }

    if (nfsFlag == true){
        if (g_bnfs == true){
            g_bnfs = true;
            COMMLOG(OS_LOG_INFO, "Check array is support nfs, "
                "and config.txt is also support nfs.");
        }
        else{
            g_bnfs = false;
            COMMLOG(OS_LOG_INFO, "Check array is support nfs, "
                "but config.txt is not support nfs. so not support nfs");
        }
    }
    else{
        if (g_bnfs == true){
            COMMLOG(OS_LOG_WARN, "Check array is not support nfs, "
                "but config.txt is support nfs. so not support nfs.");
        }
        else{
            COMMLOG(OS_LOG_WARN, "Check array is not support nfs, "
                "and config.txt is also not support nfs.");
        }
        g_bnfs = false;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_createCloneFS()
Description  : create clonefs.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotName:clonefs name.
               rstrSrcFSID:parent fsid.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_createCloneFS(IN string vstoreID, IN string &rstrSnapshotName, IN string &rstrSrcFSID, OUT CLONEFS_INFO_STRU &dstClonefsInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_NAME] = rstrSnapshotName;
    jSendData[COMMON_TAG_ALLOCTYPE] = ALLOCTYPE_THIN;
    jSendData[FS_TAG_PARENTFILESYSTEMID] = rstrSrcFSID;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;

    //Not in vstore
    if((!vstoreID.empty()) && (vstoreID.compare(STR_NOT_EXIST) != 0)){
        jSendData[COMMON_TAG_VSTOREID] = vstoreID;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_FILESYSTEM, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_FILESYSTEM, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SNAPSHOT, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        dstClonefsInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
        dstClonefsInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
        dstClonefsInfo.strParentFSID = restPkg[0][FS_TAG_PARENTFILESYSTEMID].asString();
        dstClonefsInfo.strParentSnapID = restPkg[0][FS_TAG_PARENTSNAPSHOTID].asString();;

        if(restPkg[0].isMember(COMMON_TAG_VSTOREID)){
            dstClonefsInfo.vstoreId = restPkg[0][COMMON_TAG_VSTOREID].asString();
        }
        else{
            dstClonefsInfo.vstoreId = STR_NOT_EXIST;
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showInitiatorInfo()
Description  : Show Initiator Information on array.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_showInitiatorInfo(const string &initiatorID, const string& type, INITIATOR_INFO& initiatorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    if (type.compare("FC") == 0){
        string strTargetWWN;
        string strSourceWWN = initiatorID;
        CMD_delFilterFromWWN(strSourceWWN, strTargetWWN);
        oss << RESTURL_FC_INITIATOR << "/" << strTargetWWN;
    }
    else if (type.compare("iSCSI") == 0){
        oss << RESTURL_ISCSI_INITIATOR << "/" << initiatorID;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "Unknown initiator type:[%s].", type.c_str());
        return RETURN_ERR;
    }

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return iRet;
        }

        if (restPkg.count() > 0){
            initiatorInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
            initiatorInfo.isFree = restPkg[0][INITIATOR_ISFREE].asString().compare("true") == 0 ? true : false;
            if (restPkg[0].isMember(COMMON_TAG_PARENTID)){
                initiatorInfo.strHostID = restPkg[0][COMMON_TAG_PARENTID].asString();
            }
            else{
                initiatorInfo.strHostID.clear();
            }
            initiatorInfo.uiType = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_TYPE]);
            initiatorInfo.uiStatus = jsonValue2Type<unsigned int>(restPkg[0][COMMON_TAG_RUNNINGSTATUS]);
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_addInitiatorToHost()
Description  : Add Initiator to Host.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    if (type.compare("FC") == 0){
        string strTargetWWN;
        string strSourceWWN = initiatorID;
        CMD_delFilterFromWWN(strSourceWWN, strTargetWWN);
        oss << RESTURL_FC_INITIATOR << "/" << strTargetWWN;
    }
    else if (type.compare("iSCSI") == 0){
        oss << RESTURL_ISCSI_INITIATOR << "/" << initiatorID;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "Unknown initiator type:[%s].", type.c_str());
        return RETURN_ERR;
    }

    Json::Value jSendData;
    jSendData[COMMON_TAG_PARENTTYPE] = OBJ_HOST;
    jSendData[COMMON_TAG_PARENTID] = strHostID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_removeInitiatorFromHost()
Description  : Remove Initiator from Host.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_removeInitiatorFromHost(const string &initiatorID, const string &type)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    if (type.compare("FC") == 0){
        oss << RESTURL_FC_INITIATOR << "/remove_fc_from_host";
    }
    else if (type.compare("iSCSI") == 0){
        oss << RESTURL_ISCSI_INITIATOR << "/remove_iscsi_from_host";
    }
    else{
        COMMLOG(OS_LOG_ERROR, "Unknown initiator type:[%s].", type.c_str());
        return RETURN_ERR;
    }

    Json::Value jSendData;
    string strTargetWWN;
    string strSourceWWN = initiatorID;
    CMD_delFilterFromWWN(strSourceWWN, strTargetWWN);
    jSendData[COMMON_TAG_ID] = strTargetWWN;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_removeHostFromHostGroup()
Description  : Remove Host from HostGroup.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_removeHostFromHostGroup(const string& hostID, const string& groupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    oss.str("");
    oss <<RESTURL_HOST_ASSOCIATE <<"?" <<COMMON_TAG_ID <<"=" <<groupID
        <<"&" <<COMMON_TAG_ASSOCIATEOBJTYPE <<"=" <<OBJ_HOST
        <<"&" <<COMMON_TAG_ASSOCIATEOBJID <<"=" <<hostID;


    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_DELETE, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_createHost()
Description  : Create Host.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_NAME] = hostName;
    jSendData[COMMON_TAG_LOCATION] = strEmpty;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;
    jSendData[HOST_TAG_OPERATIONSYSTEM] = HOSTOS_ESX;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_HOST, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_HOST, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_HOST, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        hostInfo.strID = restPkg[0][COMMON_TAG_ID].asString();
        hostInfo.strName = restPkg[0][COMMON_TAG_NAME].asString();
        hostInfo.strHostGroupID = strEmpty;
        hostInfo.strHostIP = strEmpty;

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_CreateLunGroup()
Description  : Create LunGroup.
Data Accessed: None.
Data Updated : None.
Input        : lunGroupName:lungroup name.
Output       : lunGroupID.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_CreateLunGroup(const string &lunGroupName, string &lunGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_NAME] = lunGroupName;
    jSendData[LUNGRP_TAG_GROUPTYPE] = 0;
    jSendData[LUNGRP_TAG_APPTYPE] = 0;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;
    lunGroupID.clear();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_LUNGROUP, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_LUNGROUP, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_LUNGROUP, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        lunGroupID = restPkg[0][COMMON_TAG_ID].asString();

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_CreateHostGroup()
Description  : Create HostGroup.
Data Accessed: None.
Data Updated : None.
Input        : hostGroupName:hostgroup name.
Output       : hostGroupID.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_CreateHostGroup(const string& hostGroupName, string &hostGroupID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";
    hostGroupID.clear();

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_NAME] = hostGroupName;
    jSendData[COMMON_TAG_DESCRIPTION] = strEmpty;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_HOSTGROUP, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_HOSTGROUP, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_HOSTGROUP, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        hostGroupID = restPkg[0][COMMON_TAG_ID].asString();

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_addHostsToHostGroup()
Description  : Add Hosts to HostGroup.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_addHostsToHostGroup(const string& hostGroupID, const string &host)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = hostGroupID;
    jSendData[COMMON_TAG_ASSOCIATEOBJTYPE] = OBJ_HOST;
    jSendData[COMMON_TAG_ASSOCIATEOBJID] = host;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_HOSTGROUP_ASSOCIATE, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_HOSTGROUP_ASSOCIATE, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_HOSTGROUP_ASSOCIATE, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_createMappingView()
Description  : Create MappingView.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_createMappingView(const string& name, string &mvID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_NAME] = name;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_MAPPINGVIEW, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_MAPPINGVIEW, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_MAPPINGVIEW, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        mvID = restPkg[0][COMMON_TAG_ID].asString();
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_associateToMappingview()
Description  : Associate Hostgroup or Lungroup to MappingView.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : success or fail.
Call         :
Called by    :
Called by    :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTCmd::CMD_associateToMappingview(const string& mvID, const int &type, const string& ID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

 
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = mvID;
    jSendData[COMMON_TAG_TYPE] = OBJ_MAPPINGVIEW;
    jSendData[COMMON_TAG_ASSOCIATEOBJTYPE] = type;
    jSendData[COMMON_TAG_ASSOCIATEOBJID] = ID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_MAPPINGVIEW_ASSOCIATE_ADD, REST_REQUEST_MODE_PUT, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_MAPPINGVIEW_ASSOCIATE_ADD, iRet);
            continue;
        }

        if (restPkg.errorCode() != RETURN_OK){
            iRet = restPkg.errorCode();
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_MAPPINGVIEW_ASSOCIATE_ADD, iRet, restPkg.description().c_str());
            return restPkg.errorCode();
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}

