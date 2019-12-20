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
#include <exception>
#include <ostream>
#include "RESTPackage.h"
#include "CmdRESTAdapter.h"
#include "CmdRESTFusionStorage.h"


std::ostream& operator<<(std::ostream& out, tag_HYMIRROR_LF_INFO& item)
{
    out <<"iProgress: "<< item.iProgress << std::endl;
    out << "strID: " << item.strID << std::endl;
    out << "strName: " << item.strName << std::endl;
    out << "strMirrorID: " << item.strMirrorID << std::endl;
    out << "uiRelationType: " << item.uiRelationType << std::endl;
    out << "uilocalResType: " << item.uilocalResType << std::endl;
    out << "strArrayWWN: " << item.strArrayWWN << std::endl;
    out << "strArrayName: " << item.strArrayName << std::endl;
    out << "strSyncStartTime: " << item.strSyncStartTime << std::endl;
    out << "uiSyncType: " << item.uiSyncType << std::endl;
    out << "uiDataStatus: " << item.uiDataStatus << std::endl;
    out << "uiPairStatus: " << item.uiPairStatus << std::endl;
    out << "uiSecResAccess: " << item.uiSecResAccess << std::endl;
    return out;
}


bool castTobool(const std::string& in)
{
    return in != "false";
}

template<typename FROM,typename TO>
TO typeCast(const FROM& from)
{
    stringstream ss;
    try{
        TO rstValue;
        ss << from;
        ss >> rstValue;
        return rstValue;
    }
    catch (std::exception e){
        COMMLOG(OS_LOG_ERROR, "invalid cast");
        return TO();
    }
}

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
            if (typeName.compare("int") == 0 || typeName.compare("unsigned int") == 0){
                return (T)-1;
            }
        }
    }

    ss >>rstValue;
    return rstValue;
}

CRESTFusionStorage::CRESTFusionStorage(const string& strSN)
    : CCmdAdapter(strSN)
    {
    m_connList.clear();
}

CRESTFusionStorage::~CRESTFusionStorage()
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

CRESTConn *CRESTFusionStorage::getConn(std::string &strDeviceIP, string &strUserName, string &strPwd)
{
    for (list<pair<string, CRESTConn *>>::iterator iter = m_connList.begin();
        iter != m_connList.end(); ++iter){
        if ((strDeviceIP.compare(iter->first) == 0)){
            time_t now(NULL);
            time(&now);

            // If you create CRESTConn and log in for more than the specified time (10 minutes), log out the original saved session and log in again.
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

int CRESTFusionStorage::InitConnectInfo(const string& strSN)
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
Input        : req
               rsp
               strErrorMsg
               getSys
Output       : None.
Return       :  Success or Failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/

int CRESTFusionStorage::ConfigConnectInfo(const string& strSN,const string& strIP, const string& strUser, const string& strPwd)
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
void CRESTFusionStorage::TimeChg(IN unsigned long long ullTime,OUT string &strTime)
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
void CRESTFusionStorage::UpdateModel(IN OUT unsigned int &ruiMode)
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
void CRESTFusionStorage::SnapshotConvertStatus(IN const unsigned int uiHealthStatus, IN OUT unsigned int &ruiStatus)
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

int HmPairRunStatusConvert(const string& row)
{
    if (row == "NORMAL"){
        return HM_STATUS_NORMAL;
    }
    else if (row == "SYNCHRONIZING"){
        return HM_STATUS_SYNCHRONIZING;
    }
    else if (row == "PAUSE"){
        return HM_SLAVE_PAIR_STATUS_CHAR_PAUSE;
    }
    else if (row == "INVALID"){
        return HM_SLAVE_PAIR_STATUS_CHAR_INVALID;
    }
    else if (row == "FORCED_START"){
        return HM_SLAVE_PAIR_STATUS_CHAR_FORCEDSTART;
    }
    else if (row == "ERROR"){
        return HM_SLAVE_PAIR_STATUS_CHAR_ERROR;
    }
    else if (row == "TO_BE_SYNCHRONIZED"){
        return HM_SLAVE_PAIR_STATUS_CHAR_TOBESYNCHRONIZED;
    }
    else{
        return HM_SLAVE_PAIR_STATUS_CHAR_UNKNOW;
    }
}

int HmPairAccessConvert(const string& row)
{
    if (row == "NOACCESS"){
        return HM_ACCESS_DENY;
    }
    else if (row == "READ_ONLY"){
        return HM_ACCESS_READ_ONLY;
    }
    else if (row == "READ_WRITE"){
        return HM_ACCESS_READ_WRITE;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error access parameter");
        return HM_ACCESS_ERROE;
    }
}

int HmPairDataStateConvert(const string& row)
{
    if (row == "HC_DATA_INTEGRITY"){
        return HC_DATA_INTEGRITY;
    }
    else if (row == "HC_DATA_INCOMPLETENESS"){
        return HC_DATA_INCOMPLETENESS;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error dataState parameter");
        return -1;
    }
}

int HmHealthSatusConvert(const string& row)
{
    if (row == "NORMAL"){
        return FUSION_HEALTH_NORMAL;
    }
    else if (row == "FAULT"){
        return FUSION_HEALTH_FAULT;
    }
    else if (row == "DEGRADE"){
        return FUSION_HEALTH_DEGRADE;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error healthSatus parameter");
        return -1;
    }
}

int HmDomainRunSatusConvert(const string& row)
{
    if (row == "NORMAL"){
        return FUSION_DOMAIN_RUNING_NORMAL;
    }
    else if (row == "TO_BE_RECOVERED"){
        return FUSION_DOMAIN_RUNING_BE_RECOVERED;
    }
    else if (row == "INVALID"){
        return FUSION_DOMAIN_RUNING_INVALID;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error domainRunSatus parameter");
        return -1;
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
void CRESTFusionStorage::SnapshotConvertType(IN OUT unsigned int &ruiType)
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
void CRESTFusionStorage::UpdateMirrorStatus(IN OUT unsigned int &ruiIntStatus)
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
void CRESTFusionStorage::ConvertMirrorPairStatus(IN OUT unsigned int &ruiIntState)
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
void CRESTFusionStorage::ConvertMirrorSlaveLUNStatus(IN OUT unsigned int &ruiIntState)
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
void CRESTFusionStorage::ConvertSrcLUNStatus(IN const unsigned int uiHealthStatus, IN const unsigned int uiRunStatus, IN OUT unsigned int &ruiIntState)
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
Function Name: CMD_showarrayinfo()
Description  : query remote devices.
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstRemoteArrayInfo:remote devices.
Return       : return RETURN_OK if success, otherwise return RETURN_ERR.
Call         :
Called by    :
Created By   :
Modification : 
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream tmposs;
    Json::Value objects;

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

        rlstRemoteArrayInfo.clear();
        
        restPkg.getResponseObjects(objects);
        
        int resCode = RETURN_ERROR;
        string description=" ";
        if (!objects.isNull()){
            if (objects.isMember("error")){
                if(objects["error"].isMember("description"))
                    description = objects["error"]["description"].asString();
                if(objects["error"].isMember("code")){
                    if(objects["error"]["code"].isString())
                        resCode = atoi(objects["error"]["code"].asString().c_str());
                    else
                        resCode = objects["error"]["code"].asInt();
                }
                
            }
            else if (objects.isMember("result")){
                if (!objects.isMember("errorCode")){
                    resCode = objects["result"].asInt();
                }
                else{
                    resCode = objects["errorCode"].asInt();
                    if(objects["errorCode"].isString())
                        resCode = atoi(objects["errorCode"].asString().c_str());
                    else
                        resCode = objects["errorCode"].asInt();
                }
            }
            else
                resCode = -1;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),RESTURL_REMOTE_DEVICE,iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }
        
        Json::FastWriter writer;
        string str="";

        if(objects.isMember("data"))
            if(!objects["data"][0].isNull())
                str = writer.write(objects["data"][0]);

        Json::Value objects_parameter;
        Json::Reader reader;
        REMOTE_ARRAY_STRU stArrayInfo;
        if (!reader.parse(str.c_str(), objects_parameter)){
            return -1;
        }
        else{
            if(objects_parameter.isMember("id"))
                stArrayInfo.strID = objects_parameter["id"].asString();
            if(objects_parameter.isMember("name"))
                stArrayInfo.strName = objects_parameter["name"].asString();
            if(objects_parameter.isMember("runningStatus"))
                stArrayInfo.uiRunState = jsonValue2Type<unsigned int>(objects_parameter["runningStatus"]);

            stArrayInfo.strSN = objects_parameter["system_wwn"].asString();
            if(objects_parameter.isMember("sn"))
                stArrayInfo.strWWN = objects_parameter["sn"].asString();
            rlstRemoteArrayInfo.push_back(stArrayInfo);
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
int CRESTFusionStorage::CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    Json::Value objects;
    string strIP;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest("/cluster/sn", REST_REQUEST_MODE_GET, "", restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "/cluster/sn", iRet);
            continue;
        }
        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).",
                iter->c_str(),"/cluster/sn",iRet);
            return resCode;
        }

        if(objects.isMember("sn"))
            rstSysInfo.strDeviceSerialNumber = objects["sn"].asString();
            
        break;
    }

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
int CRESTFusionStorage::CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg)
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
    rstStorageInfo.strArraySN = rstSysInfo.strDeviceSerialNumber;
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
int CRESTFusionStorage::CMD_showarraymodelinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    Json::Value objects; 
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin();
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        iRet = restConn->doRequest("/systemSummary", REST_REQUEST_MODE_GET, "", restPkg); 

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "/systemSummary", iRet);
            continue;
        }
        restPkg.getResponseObjects(objects);

        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", 
                iter->c_str(),"/systemSummary",iRet);
            return resCode;
        }
    
        if(objects.isMember("productModel"))
            rstStorageInfo.strArrayModel = objects["productModel"].asString();
        if(objects.isMember("clusterName"))
            rstStorageInfo.strArrayName = objects["clusterName"].asString();
        
        break;
    }
    
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
int CRESTFusionStorage::CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream tmposs;
    LUN_INFO_STRU stLUNInfo;
    FS_INFO_STRU  stFSInfo;
    Json::Value objects;
    Json::Value objects_parameter;
    Json::FastWriter writer;
    Json::Reader reader;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    
    string description = "";
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        int rangeIndex = 0;
        int count = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        tmposs.str("");
        tmposs << RESTURL_REPLICATIONPAIR;
        iRet = restConn->doRequest(tmposs.str(), REST_REQUEST_MODE_GET, "", restPkg);
        restPkg.getResponseObjects(objects);
        string str;
        
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), tmposs.str().c_str(), iRet);
            flag = false;
            break;
        }

        while(count < objects["data"].size()){    
            str = writer.write(objects["data"][count]);
            HYMIRROR_INFO_STRU stHyMirrorInfo_m;

            if (!reader.parse(str.c_str(), objects_parameter)){
                return -1;
            }
            else{
                int resCode = RETURN_ERROR;
                if (!objects.isNull()){
                    if (objects.isMember("error"))
                        if(objects["error"]["code"].isString())
                            resCode = atoi(objects["error"]["code"].asString().c_str());
                        else
                            resCode = objects["error"]["code"].asInt();
                    else if (objects.isMember("result"))
                        if (!objects.isMember("errorCode"))
                            resCode = objects["result"].asInt();
                        else
                            resCode = objects["errorCode"].asInt();
                    else
                        return RETURN_ERROR;
                }
                if (resCode != RETURN_OK){
                    iRet = resCode;
                    COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                        iter->c_str(),tmposs.str().c_str(),iRet,restPkg.getFusionDescription().c_str());
                    return resCode;
                }
                if(objects_parameter.isMember("ID"))
                    stHyMirrorInfo_m.strID = objects_parameter["ID"].asString();
                if(objects_parameter.isMember("REPLICATIONMODEL"))
                    stHyMirrorInfo_m.uiModel = jsonValue2Type<unsigned int>(objects_parameter["REPLICATIONMODEL"]);
                if(objects_parameter.isMember("LOCALRESTYPE"))
                    stHyMirrorInfo_m.uilocalResType =  jsonValue2Type<unsigned int>(objects_parameter["LOCALRESTYPE"]);
                if(objects_parameter.isMember("LOCALRESID"))
                    stHyMirrorInfo_m.strPriLUNID = objects_parameter["LOCALRESID"].asString();
                if(objects_parameter.isMember("ISPRIMARY"))
                    stHyMirrorInfo_m.bIsPrimary = jsonValue2Type<unsigned int>(objects_parameter["ISPRIMARY"]);
                if(objects_parameter.isMember("RUNNINGSTATUS"))
                    stHyMirrorInfo_m.uiState = jsonValue2Type<unsigned int>(objects_parameter["RUNNINGSTATUS"]);
                if(objects_parameter.isMember("HEALTHSTATUS"))
                    stHyMirrorInfo_m.uiStatus = jsonValue2Type<unsigned int>(objects_parameter["HEALTHSTATUS"]);

                if(stHyMirrorInfo_m.uilocalResType == OBJ_LUN){
                    COMMLOG(OS_LOG_DEBUG, "Parse lun remote replication %s", stHyMirrorInfo_m.strID.c_str());
                    if(objects_parameter.isMember("ISINCG"))
                        stHyMirrorInfo_m.bIsBelongGroup = (objects_parameter["ISINCG"].asString().compare("true") == 0);
                    if(stHyMirrorInfo_m.bIsBelongGroup){
                        string strCGID ="";
                        if(objects_parameter.isMember("CGID"))
                            strCGID = objects_parameter["CGID"].asString();
                        stHyMirrorInfo_m.strGroupID = "";
                        if (strCGID.compare(STR_INVALID_CGID) != 0)
                            stHyMirrorInfo_m.strGroupID = strCGID;
                    }

                    if(objects_parameter.isMember("LOCALRESID"))
                        stLUNInfo.strID = objects_parameter["LOCALRESID"].asString();
                    if(objects_parameter.isMember("LOCALRESNAME"))
                        stLUNInfo.strName = objects_parameter["LOCALRESNAME"].asString();
                    iRet = CMD_showlun(stLUNInfo);
                    if (iRet != RETURN_OK){
                        COMMLOG(OS_LOG_ERROR, "CMD_showlun (%s) fail, ret=%d", stLUNInfo.strID.c_str(), iRet);
                        flag = false;
                        
                        break;
                        
                    }
                    
                    stHyMirrorInfo_m.strPriVstoreID = stLUNInfo.strVstoreID;
                }
                else if(stHyMirrorInfo_m.uilocalResType == OBJ_FILESYSTEM && g_bnfs){
                    COMMLOG(OS_LOG_DEBUG, "Parse fs remote replication %s", stHyMirrorInfo_m.strID.c_str());
                    stHyMirrorInfo_m.bIsBelongGroup = (objects_parameter.isMember(REPLICATIONPAIR_TAG_VSTOREPAIRID) && (objects_parameter[REPLICATIONPAIR_TAG_VSTOREPAIRID].compare(STR_NOT_EXIST) != 0));
                    if(stHyMirrorInfo_m.bIsBelongGroup){
                        COMMLOG(OS_LOG_INFO, "[%s]SRA not support fs remote replications in vstore pair, ignore it...", stHyMirrorInfo_m.strID.c_str());
                        continue;                        
                    }
                    else{
                        stHyMirrorInfo_m.strGroupID = "";
                    }

                    if(objects_parameter.isMember("LOCALRESID"))
                        stFSInfo.strID = objects_parameter["LOCALRESID"].asString();
                    iRet = CMD_showfs(stFSInfo);
                    if (iRet != RETURN_OK){
                        COMMLOG(OS_LOG_ERROR, "CMD_showlun (%s) fail, ret=%d", stFSInfo.strID.c_str(), iRet);
                        flag = false;
                        break;
                    }
                    stHyMirrorInfo_m.strPriVstoreID = stFSInfo.vstoreId;
                }
                else{
                    continue;
                }

                if(objects_parameter.isMember(REPLICATIONPAIR_TAG_VSTOREPAIRID)){
                    stHyMirrorInfo_m.vstorePairID = objects_parameter[REPLICATIONPAIR_TAG_VSTOREPAIRID].asString();
                }
                else{
                    stHyMirrorInfo_m.vstorePairID = STR_NOT_EXIST;
                }

                if((restConn->getVstoreID().compare(STR_NOT_EXIST) == 0) && (stHyMirrorInfo_m.strPriVstoreID.compare(STR_NOT_EXIST) != 0)){
                    COMMLOG(OS_LOG_INFO, "Current login is system view, ignoring rm pair %s beglongs to vstore %s", stHyMirrorInfo_m.strID.c_str(),stHyMirrorInfo_m.strPriVstoreID.c_str());
 
                }
                rlstHyMirrorInfo.push_back(stHyMirrorInfo_m);
                rangeIndex += RECOMMEND_RANGE_COUNT;
            }

            count++;
        }        

        if (flag)
            return RETURN_OK;
        else{
            continue;
        }
    }

    return RETURN_ERR;    
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
int CRESTFusionStorage::CMD_showhymirrorlun_all(IN string &strMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;
    int iLunRet = RETURN_OK;
    
    oss.str("");
    oss << RESTURL_REPLICATIONPAIR << "/" <<strMirrorID;

    iRet = fullRestPkg(oss.str(), REST_REQUEST_MODE_GET, "", strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);

    GET_JSON_VALUE(object, data, FUSION_DATA);

    unsigned int resType = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_LOCALRESTYPE]);

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), oss.str().c_str(), res);
        return RETURN_ERR;
    }

    bool isPrimary = (data[REPLICATIONPAIR_TAG_ISPRIMARY].asString().compare("true") == 0);

    unsigned int uiSyncType = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_SYNCHRONIZETYPE]);
    if (0xFFFFFFFF == uiSyncType){
        uiSyncType = 0;
    }

    int iProgress = jsonValue2Type<int>(data[REPLICATIONPAIR_TAG_REPLICATIONPROGRESS]);

    unsigned int uiDataStatusRemote = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_SECRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusRemote){
        uiDataStatusRemote = 0;
    }

    unsigned int uiDataStatusLocal = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_PRIRESDATASTATUS]);
    if (0xFFFFFFFF == uiDataStatusLocal){
        uiDataStatusLocal = 0;
    }

    unsigned int uiPairStatus = jsonValue2Type<unsigned int>(data[COMMON_TAG_RUNNINGSTATUS]);
    if (0xFFFFFFFF == uiPairStatus){
        uiPairStatus = 0;
    }

    if (uiPairStatus == RUNNING_STATUS_NORMAL && iProgress == -1){
        iProgress = 100;
    }

    unsigned long long ullStartTime = jsonValue2Type<unsigned long long>(data[REPLICATIONPAIR_TAG_STARTTIME]);

    HYMIRROR_LF_INFO_STRU stHyMirrorLUNInfoLocal, stHyMirrorLUNInfoRemote;

    stHyMirrorLUNInfoLocal.uiSyncType = uiSyncType;
    stHyMirrorLUNInfoLocal.iProgress = iProgress;
    stHyMirrorLUNInfoLocal.uiDataStatus = uiDataStatusLocal;
    ConvertMirrorSlaveLUNStatus(stHyMirrorLUNInfoLocal.uiDataStatus);
    TimeChg(ullStartTime, stHyMirrorLUNInfoLocal.strSyncStartTime);
    stHyMirrorLUNInfoLocal.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorLUNInfoLocal.uiPairStatus);
    stHyMirrorLUNInfoLocal.uilocalResType = resType;

    stHyMirrorLUNInfoRemote.uiSyncType = uiSyncType;
    stHyMirrorLUNInfoRemote.iProgress = iProgress;
    stHyMirrorLUNInfoRemote.uiDataStatus = uiDataStatusRemote;
    ConvertMirrorSlaveLUNStatus(stHyMirrorLUNInfoRemote.uiDataStatus);
    TimeChg(ullStartTime, stHyMirrorLUNInfoRemote.strSyncStartTime);
    stHyMirrorLUNInfoRemote.uiPairStatus = uiPairStatus;
    ConvertMirrorPairStatus(stHyMirrorLUNInfoRemote.uiPairStatus);
    stHyMirrorLUNInfoRemote.uilocalResType = resType;

    stHyMirrorLUNInfoLocal.strID = data[REPLICATIONPAIR_TAG_LOCALRESID].asString();
    stHyMirrorLUNInfoLocal.strName = data[REPLICATIONPAIR_TAG_LOCALRESNAME].asString();
    if (!stHyMirrorLUNInfoLocal.strID.empty()){
        if (resType == OBJ_LUN){
            COMMLOG(OS_LOG_INFO, "Query LUN[%s] info", stHyMirrorLUNInfoLocal.strID.c_str());
            LUN_INFO_STRU localLUNInfo;
            localLUNInfo.strID = stHyMirrorLUNInfoLocal.strID;
            localLUNInfo.strName = stHyMirrorLUNInfoLocal.strName;
            iLunRet = CMD_showlun(localLUNInfo);
            if (RETURN_OK == iLunRet){
                stHyMirrorLUNInfoLocal.strName = localLUNInfo.strName;
            }
            else{
                COMMLOG(OS_LOG_ERROR, "query lun fail");
                return iLunRet;
            }
        }
        else if (resType == OBJ_FILESYSTEM){
            COMMLOG(OS_LOG_INFO, "Query FS[%s] info", stHyMirrorLUNInfoLocal.strID.c_str());
            FS_INFO_STRU localFSInfo;
            localFSInfo.strID = stHyMirrorLUNInfoLocal.strID;
            iLunRet = CMD_showfs(localFSInfo);
            if (RETURN_OK == iLunRet){
                stHyMirrorLUNInfoLocal.strName = localFSInfo.strName;
            }
            else{
                COMMLOG(OS_LOG_ERROR, "query fs fail");
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

    stHyMirrorLUNInfoRemote.strID = data[REPLICATIONPAIR_TAG_REMOTERESID].asString();

    REMOTE_ARRAY_STRU remote_dev;
    string remote_sn = data[FUSION_REPLICATIONPAIR_REMOTEDEVICESN].asString();
    int ret = findRemoteArraySn(remote_sn, remote_dev);
    if (ret == RETURN_OK){
        stHyMirrorLUNInfoRemote.strArrayWWN = remote_dev.strSN;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "decode remote device fail[%s]", remote_sn.c_str());
        return RETURN_ERR;
    }
    
    stHyMirrorLUNInfoRemote.strArrayName = data[REPLICATIONPAIR_TAG_REMOTEDEVICENAME].asString();
    stHyMirrorLUNInfoRemote.strName = data[REPLICATIONPAIR_TAG_REMOTERESNAME].asString();

    stHyMirrorLUNInfoRemote.uiSecResAccess = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_SECRESACCESS]);
    stHyMirrorLUNInfoLocal.uiSecResAccess = jsonValue2Type<unsigned int>(data[REPLICATIONPAIR_TAG_SECRESACCESS]);

    if (isPrimary){
        stHyMirrorLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
        stHyMirrorLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;
    }
    else {
        stHyMirrorLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
        stHyMirrorLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
    }

    stHyMirrorLUNInfoLocal.strMirrorID = strMirrorID;
    stHyMirrorLUNInfoRemote.strMirrorID = strMirrorID;
    rlstHyMirrorLUNInfo.push_back(stHyMirrorLUNInfoLocal);
    rlstHyMirrorLUNInfo.push_back(stHyMirrorLUNInfoRemote);

    return RETURN_OK;
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
int CRESTFusionStorage::CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    Json::Value objects;
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = rstrMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SPILT_HYPERMIRROR, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SPILT_HYPERMIRROR, iRet);
            continue;
        }
        
        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SPILT_HYPERMIRROR, iRet, restPkg.getFusionDescription().c_str());
            return resCode;
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
int CRESTFusionStorage::CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode)
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
    jSendData[REPLICATIONPAIR_TAG_SECRESACCESS] = ss.str();
    Json::Value objects;
    
    oss <<RESTURL_REPLICATIONPAIR <<"/" <<strHyMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        
        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());

            return resCode;
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
int CRESTFusionStorage::CMD_synchymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    ostringstream oss;
    Json::Value objects;
   
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SYNCRONIZE_HYPERMIRROR, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the param [%s] the iRet is (%d).", iter->c_str(), RESTURL_SYNCRONIZE_HYPERMIRROR, jSendData.toStyledString().c_str(), iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SYNCRONIZE_HYPERMIRROR, iRet, restPkg.getFusionDescription().c_str());
            return resCode;
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
int CRESTFusionStorage::CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    Json::Value objects;

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
                
        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
            return resCode;
        }
        strIP = *iter;

        break;
    }
    
    unsigned int replicationmodel = typeCast<string,unsigned int>(objects["data"]["REPLICATIONMODEL"].asString());
    rstHyMirrorInfo.uiModel = replicationmodel;
    UpdateModel(rstHyMirrorInfo.uiModel);

    bool isincg = (objects["data"]["ISINCG"].asString().compare("true") == 0);
    rstHyMirrorInfo.bIsBelongGroup = isincg;
    if(rstHyMirrorInfo.bIsBelongGroup){
        string strCGID = restPkg[0][REPLICATIONPAIR_TAG_CGID].asString();
        rstHyMirrorInfo.strGroupID = "";
        string cgid = objects["data"]["CGID"].asString();
        if (cgid.compare(STR_INVALID_CGID) != 0)
            rstHyMirrorInfo.strGroupID = cgid;
    }    

    unsigned int healthstatus = typeCast<string,unsigned int>(objects["data"]["HEALTHSTATUS"].asString());
    rstHyMirrorInfo.uiStatus = healthstatus;
    UpdateMirrorStatus(rstHyMirrorInfo.uiStatus);

    unsigned int runningstatus = typeCast<string,unsigned int>(objects["data"]["RUNNINGSTATUS"].asString());     
    rstHyMirrorInfo.uiState = runningstatus;

    bool isprimary = (objects["data"]["ISPRIMARY"].asString().compare("true") == 0);
    rstHyMirrorInfo.bIsPrimary = isprimary;

    unsigned int localrestype = typeCast<string,unsigned int>(objects["data"]["LOCALRESTYPE"].asString());     
    rstHyMirrorInfo.uilocalResType = localrestype;

    string localresid = objects["data"]["LOCALRESID"].asString();
    rstHyMirrorInfo.strPriLUNID = localresid;

    unsigned int secresaccess=typeCast<string,unsigned int>(objects["data"]["SECRESACCESS"].asString()); 
    rstHyMirrorInfo.uiSecResAccess = secresaccess;

    string remotesid = objects["data"]["REMOTERESID"].asString();
    unsigned int synchronizetype = typeCast<string,unsigned int>(objects["data"]["SYNCHRONIZETYPE"].asString());
    unsigned int replicationprogress = typeCast<string,unsigned int>(objects["data"]["REPLICATIONPROGRESS"].asString());
    unsigned int prireesdatastatus = typeCast<string,unsigned int>(objects["data"]["PRIRESDATASTATUS"].asString());
    string remotesn = objects["data"]["REMOTEDEVICESN"].asString();
    string remotedevice= objects["data"]["REMOTEDEVICENAME"].asString();
    unsigned long long starttime = typeCast<string,unsigned long long>(objects["data"]["STARTTIME"].asString());
    int secresdatastatus = typeCast<string,unsigned int>(objects["data"]["SECRESDATASTATUS"].asString());

    return RETURN_OK;
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
int CRESTFusionStorage::CMD_swaphymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    Json::Value objects;
    jSendData[COMMON_TAG_ID] = strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(RESTURL_SWITCHROLE_HYPERMIRROR, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), RESTURL_SWITCHROLE_HYPERMIRROR, iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), RESTURL_SWITCHROLE_HYPERMIRROR, iRet, restPkg.getFusionDescription().c_str());
            return resCode;
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
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showInitiatorInfo(const string &initiatorName, const string& type, INITIATOR_INFO& initiatorInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_PORT_NAME] = initiatorName;

    iRet = fullRestPkg(FUSION_URL_PORT_LIST, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_WARN, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_PORT_LIST, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);
    GET_JSON_VALUE(object, ports, FUSION_DATA);

    if (ports.size() != 1){
        COMMLOG(OS_LOG_ERROR, "query port[%s] fail", initiatorName.c_str());
        return RETURN_ERR;
    }
    else{
        initiatorInfo.isFree = ports[0][FUSION_PORT_ISFREE].asBool();
        initiatorInfo.strID = ports[0][FUSION_PORT_ID].asString();
        initiatorInfo.uiStatus = ports[0][FUSION_PORT_STATUS].asInt64();
    }

    if (!initiatorInfo.isFree){
        initiatorInfo.strHostID = ports[0][FUSION_PORT_HOST_ID].asString();
        initiatorInfo.strHostName = ports[0][FUSION_PORT_HOST_NAME].asString();
    }
    return RETURN_OK;
}


/*------------------------------------------------------------
Function Name: CMD_showlunByname()
Description  : query LUN information.
Data Accessed: None.
Data Updated : None.
Input        : rstLUNInfo
Output       : rstLUNInfo
Return       :  Success or Failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showlunByname(const string& name , LUN_INFO_STRU &rstLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    ostringstream oss;
    oss.str("");
    oss << FUSION_URL_QUERY_LUN_BY_NAME<<name;

    

    iRet = fullRestPkg(oss.str(), REST_REQUEST_MODE_GET, "", strIP, restPkg);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);

    GET_JSON_VALUE(object, luns, FUSION_LUN_DETAIL_INFO);

    rstLUNInfo.strName = name;
    TimeChg(jsonValue2Type<long long>(luns[FUSION_LUN_CREATE_TIME]), rstLUNInfo.strCreateTime);
    rstLUNInfo.strID = typeCast<long long,string>(luns[FUSION_VOL_ID].asInt64());
    string wwn = luns[FUSION_WWN].asString();
    CMD_addFilterToWWN(wwn, rstLUNInfo.strWWN);
        

    return RETURN_OK;
}
int CRESTFusionStorage::CMD_showlun(LUN_INFO_STRU &rstLUNInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;

    list<string> lstHyperID;
    list<string>::iterator itHyperID;
    string strHyperID;
    string strWWN;
    string strTmpMirrorID;
    string strIP;

    ostringstream oss;
    oss.str("");
    oss << FUSION_URL_VOL<< RESTURL_URL_QUERY_BY_ID <<"?"<< FUSION_VOL_ID <<"="<< rstLUNInfo.strID;

    iRet = fullRestPkg(oss.str(), REST_REQUEST_MODE_GET, "", strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);

    GET_JSON_VALUE(object, lunObj, FUSION_LUN_DETAIL_INFO);

    rstLUNInfo.strName = lunObj[FUSION_VOL_NAME].asString();
    rstLUNInfo.uiStatus = jsonValue2Type<unsigned int>(lunObj[FUSION_STATUS]);
    TimeChg(jsonValue2Type<long long>(lunObj[FUSION_LUN_CREATE_TIME]), rstLUNInfo.strCreateTime);
    strWWN = lunObj[FUSION_WWN].asString();
    CMD_addFilterToWWN(strWWN, rstLUNInfo.strWWN);

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
int CRESTFusionStorage::CMD_showhyimgoflun(IN string &strLunID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    list<string> lstHyImageID;
    list<string>::iterator itHyImageID;
    string strIP;

    ostringstream oss;
    oss.str("");
    oss << FUSION_URL_VOL_SNAP_LIST;

    LUN_INFO_STRU rstLUNInfo;
    rstLUNInfo.strID = strLunID;
    CMD_showlun(rstLUNInfo);

    Json::Value jSendData;
    Json::Value object;
    int batchnum = 1;
    while(true){
        jSendData[FUSION_REQ_VOL_NAME] = rstLUNInfo.strName;
        jSendData[FUSION_REQ_BATCH_LIMIT] = 1000;
        jSendData[FUSION_REQ_BATCH_NUM] = batchnum;
    
    iRet = fullRestPkg(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

        restPkg.getResponseObjects(object);

        GET_JSON_VALUE(object, snapshotList, FUSION_SNAP_LIST);
        if (!snapshotList.isArray()){
            COMMLOG(OS_LOG_ERROR, "the snapshotlist is not array");
            return RETURN_ERROR;
        }
        int i = 0;
        for (; i < snapshotList.size(); ++i){
            HYIMAGE_INFO_STRU stHyImageInfo;
            stHyImageInfo.strID = typeCast<long long,string>(snapshotList[i][FUSION_SNAPSHOTID].asInt64());
            stHyImageInfo.strName = snapshotList[i][FUSION_SNAPSHOTNAME].asString();
            COMMLOG(OS_LOG_INFO,"size[%d],index[%d],id[%d],snapshotname[%s]", snapshotList.size(),\
                i,snapshotList[i][FUSION_SNAPSHOTID].asInt64(),snapshotList[i][FUSION_SNAPSHOTNAME].asString().c_str());
            iRet = CMD_showhyimginfo(stHyImageInfo);
            if (iRet != RETURN_OK){
                return iRet;
            }
            rlstHyImageInfo.push_back(stHyImageInfo);
        }
        if(i == 1000){
            batchnum += 1;
            stringstream strStream;
            strStream << batchnum;
            strStream >> jSendData[FUSION_REQ_BATCH_NUM];
        }
        else{
            break;
        }
    }
    return RETURN_OK;

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
int CRESTFusionStorage::CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strWWN, strIP;
    ostringstream oss;
    oss << RESTURL_SNAPSHOT << RESTURL_URL_QUERY_BY_NAME << "?" <<"snapshotName="<<rstHyImageInfo.strName;

    iRet = fullRestPkg(oss.str(), REST_REQUEST_MODE_GET, "", strIP, restPkg);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);

    GET_JSON_VALUE(object, snapshot, FUSION_SNAPSHOT);

    rstHyImageInfo.strID = typeCast<long long, string>(snapshot[FUSION_SNAPSHOTID].asInt64());
    rstHyImageInfo.strName = snapshot[FUSION_SNAPSHOTNAME].asString();
    rstHyImageInfo.uiStatus = snapshot[FUSION_STATUS].asInt();

    unsigned long long ullTimeStamp = jsonValue2Type<unsigned long long>(snapshot[FUSION_SNAPSHOT_CREATETIME]);
    TimeChg(ullTimeStamp, rstHyImageInfo.strTimeStamp);

    strWWN = snapshot[FUSION_WWN].asString();
    CMD_addFilterToWWN(strWWN, rstHyImageInfo.strWWN);

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
int CRESTFusionStorage::CMD_createhyimg(IN string &strSnapshotName, IN string &rstrSrcLUNID)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strEmpty = "";

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData["volName"] = rstrSrcLUNID;
    jSendData["snapshotName"] = strSnapshotName;

    Json::Value objects;
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        ostringstream oss;
        oss.str("");
        oss << RESTURL_SNAPSHOT << "/create";
        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",iter->c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
            return resCode;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}
int CRESTFusionStorage::CMD_cloneSnap2Lun(const string& newLunname, const string& srcname)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strWWN, strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_VOL_NAME] = newLunname;
    jSendData[FUSION_REQ_SRC_SNAP_NAME] = srcname;

    iRet = fullRestPkg(FUSION_URL_CREATE_LUN_BY_SNAP, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_CREATE_LUN_BY_SNAP, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }
    return RETURN_OK;
}

/*------------------------------------------------------------
Function Name: CMD_dellun()
Description  : delete lun.
Data Accessed: None.
Data Updated : None.
Input        : strSnapshotID.
Output       : None.
Return       : Success or Failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_dellun(const list<string> &lunsname)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strWWN, strIP;

    Json::Value jSendData, lunList;

    for (list<string>::const_iterator it = lunsname.begin(); it != lunsname.end(); it++){
        lunList.append(string(*it));
    }
    jSendData[FUSION_REQ_VOL_NAMES] = lunList;

    iRet = fullRestPkg(FUSION_URL_DEL_LUN, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);

    if (iRet != RETURN_OK){
        if (restPkg.errorCode_d() == VOLUME_NOT_EXIST){
            COMMLOG(OS_LOG_WARN, "the lun is not exist.");
            return RETURN_WAR;
        }
        else{
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_DEL_LUN, iRet, restPkg.getFusionDescription().c_str());
        }
        return iRet;
    }
    return RETURN_OK;
}

int CRESTFusionStorage::CMD_delhyimg(IN string &strSnapshotName)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    Json::Value objects;
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    ostringstream oss;
    oss.str("");
    oss <<RESTURL_SNAPSHOT <<"/" <<"delete";

    Json::Value jSendData;
    jSendData["snapshotName"] = strSnapshotName;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(oss.str(), REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), oss.str().c_str(), iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        int resCode = RETURN_ERROR;
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), oss.str().c_str(), iRet, restPkg.getFusionDescription().c_str());
            return resCode;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
}

/*------------------------------------------------------------
Function Name: CMD_showmap()
Description  : query mappingview
Data Accessed: None.
Data Updated : None.
Input        : 
Output       : rlstMapInfo:映射信息.
Return       :  Success or Failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    return RETURN_OK;
}

int CRESTFusionStorage::QueryHostMapByVolName(const LUN_INFO_STRU& lun, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_LUN_NAME] = lun.strName;

    iRet = fullRestPkg(FUSION_URL_QUERY_HOST_MAP, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_QUERY_HOST_MAP, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);
    GET_JSON_VALUE(object, hostList, FUSION_HOST_LIST);

    if (hostList.isArray()){
        for (int i = 0; i < hostList.size();++i){
            MAP_INFO_STRU mapInfo;
            string hostName = hostList[i][FUSION_HOST_NAME].asString();
            long long hostId;
            string createTime;
            int ret = QueryHostInfoByHostName(hostName, createTime, hostId);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "query hostInfo error,hostname[%s]", hostName.c_str());
                return ret;
            }
            mapInfo.strHostID = typeCast<long long, string>(hostId);
            mapInfo.strHostLUNID = typeCast<long long, string>(hostList[i][FUSION_HOST_LUN_ID].asInt64());
            mapInfo.strLUNWWN = lun.strWWN;
            mapInfo.strDevLUNID = lun.strID;
            mapInfo.strHostName = hostName;
            rlstMapInfo.push_back(mapInfo);
        }
        
    }
    else{
        COMMLOG(OS_LOG_ERROR, "the result is not a array");
        return RETURN_ERR;
    }
    return RETURN_OK;
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
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_HOST_NAME] = hostName;

    iRet = fullRestPkg(FUSION_URL_HOST_CREATE, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_HOST_CREATE, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    iRet = CMD_showhost(hostName, hostInfo);
    if (hostInfo.strID.empty()){
        COMMLOG(OS_LOG_ERROR, "create host[%s] fail ,the hostId is not exist", hostName.c_str());
        return RETURN_ERR;
    }

    return RETURN_OK;;
}

int CRESTFusionStorage::QueryHostInfoByHostName(const string& hostname, string& createTime, long long & hostId)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    iRet = fullRestPkg(FUSION_URL_HOST_LIST, REST_REQUEST_MODE_GET, "", strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_HOST_LIST, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);
    GET_JSON_VALUE(object, hostList, FUSION_HOST_LIST);

    if (hostList.isArray()){
        for (int i = 0; i < hostList.size(); ++i){
            if (hostname == hostList[i][FUSION_HOST_NAME].asString()){
                TimeChg(jsonValue2Type<unsigned long long>(hostList[i][FUSION_HOST_CREATETIME]), createTime);
                hostId = hostList[i][FUSION_HOST_ID].asInt64();
                return RETURN_OK;
            }
        }
        COMMLOG(OS_LOG_INFO, "not find the host[%s]", hostname.c_str());
        return RETURN_OK;
    }
    COMMLOG(OS_LOG_ERROR, "not find the host[%s]", hostname.c_str());
    return RETURN_ERR;
}

int CRESTFusionStorage::QueryHostsByHostGroupName(const string& groupName, vector<string>& hosts)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_HOSTGROUP_NAME] = groupName;

    iRet = fullRestPkg(FUSION_URL_QUERY_HOSTS_BY_GROUPNAME, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_QUERY_HOSTS_BY_GROUPNAME, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);
    GET_JSON_VALUE(object, hostList, FUSION_HOST_LIST);

    if (hostList.isArray()){
        for (int i = 0; i < hostList.size(); ++i){
            hosts.push_back(hostList[i].asString());
        }
    }
    else{
        COMMLOG(OS_LOG_ERROR, "the result is not a array");
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int CRESTFusionStorage::QueryHostGroupMapByVolName(const LUN_INFO_STRU& lun, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData;
    jSendData[FUSION_REQ_VOLUME_NAME] = lun.strName;

    iRet = fullRestPkg(FUSION_URL_QUERY_HOSTGROUP_MAP, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), FUSION_URL_QUERY_HOSTGROUP_MAP, iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    Json::Value object;
    restPkg.getResponseObjects(object);
    GET_JSON_VALUE(object, hostGroupList, FUSION_HOST_GROUP_LIST);
    if (hostGroupList.isArray()){
        vector<string> hosts;
        for (int i = 0; i < hostGroupList.size(); ++i){
            string hostGroupName = hostGroupList[i][FUSION_HOST_GROUP_NAME].asString();
            hosts.clear();
            int ret = QueryHostsByHostGroupName(hostGroupName,hosts);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "query hosts by hostGroupname[%s] error", hostGroupName.c_str());
                return ret;
            }
            string createTime;
            long long hostId;
            for (int i = 0; i < hosts.size(); ++i){
                MAP_INFO_STRU mapInfo;
                int ret = QueryHostInfoByHostName(hosts[i], createTime, hostId);
                if (ret != RETURN_OK){
                    COMMLOG(OS_LOG_ERROR, "query hostInfo error,hostname[%s]", hosts[i].c_str());
                    return ret;
                }
                mapInfo.strHostID = typeCast<long long, string>(hostId);
                mapInfo.strLUNWWN = lun.strWWN;
                mapInfo.strDevLUNID = lun.strID;
                mapInfo.strHostGroupName = hostGroupName;
                mapInfo.strHostName = hosts[i];
                rlstMapInfo.push_back(mapInfo);
            }
        }
    }
    else{
        COMMLOG(OS_LOG_ERROR, "the result is not a array");
        return RETURN_ERR;
    }
    return RETURN_OK;
}

int CRESTFusionStorage::CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    LUN_INFO_STRU rstLUNInfo;
    rstLUNInfo.strID = volID;
    int ret = CMD_showlun(rstLUNInfo);

    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "query volume failed,the id is [%s]", volID.c_str());
        return ret;
    }

    QueryHostMapByVolName(rstLUNInfo, rlstMapInfo);
    QueryHostGroupMapByVolName(rstLUNInfo, rlstMapInfo);

    return RETURN_OK;
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
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo)
{
    string createTime;
    long long hostId = -1;
    rHostInfo.strID.clear();
    rHostInfo.strName.clear();
    int ret = QueryHostInfoByHostName(name, createTime,hostId);
    if (ret != RETURN_OK){
        return ret;
    }
    else{
        if (hostId != -1){
            rHostInfo.strID = typeCast<long long, string>(hostId);
            rHostInfo.strName = name;
        }
        return RETURN_OK;
    }
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
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostName)
{
    FUSION_URL_PORT_INITIATOR_2_HOST;
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    if (type.compare("iSCSI") == 0){
        Json::Value jSendData;
        jSendData[FUSION_REQ_HOST_NAME] = strHostName;
        Json::Value ports;
        ports.append(initiatorID);
        jSendData[FUSION_REQ_PORT_NAMES] = ports;

        iRet = fullRestPkg(FUSION_URL_PORT_INITIATOR_2_HOST, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
                strIP.c_str(), FUSION_URL_PORT_INITIATOR_2_HOST, iRet, restPkg.getFusionDescription().c_str());
            return iRet;
        }
    }
    
    return RETURN_OK;
}

int CRESTFusionStorage::CMD_showmapByName(IN const string& name, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    LUN_INFO_STRU rstLUNInfo;
    rstLUNInfo.strName = name;

    QueryHostMapByVolName(rstLUNInfo, rlstMapInfo);
    QueryHostGroupMapByVolName(rstLUNInfo, rlstMapInfo);

    return RETURN_OK;
}

int CRESTFusionStorage::CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo,LUN_INFO_STRU& rstLUNInfo)
{
    rstLUNInfo.strID = volID;
    int ret = CMD_showlun(rstLUNInfo);

    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "query volume failed,the id is [%s]", volID.c_str());
        return ret;
    }
    COMMLOG(OS_LOG_INFO,"show map for volID:%s ",volID.c_str());
    QueryHostMapByVolName(rstLUNInfo, rlstMapInfo);
    QueryHostGroupMapByVolName(rstLUNInfo, rlstMapInfo);

    return RETURN_OK;
}


int CRESTFusionStorage::CMD_maplun2host(const string& hostname, const list<string>& luns)
{
    return operateHostMapping(MAP,hostname,luns);
}

int CRESTFusionStorage::CMD_maplun2hostGroup(const string& hostGroupname, const list<string>& luns)
{
    return operateHostGroupMapping(MAP, hostGroupname, luns);
}

int CRESTFusionStorage::CMD_unmaplun2host(const string& hostname, const list<string>& luns)
{
    return operateHostMapping(UNMAP, hostname, luns);
}

int CRESTFusionStorage::CMD_unmaplun2hostGroup(const string& hostGroupname, const list<string>& luns)
{
    return operateHostGroupMapping(UNMAP, hostGroupname, luns);
}

int CRESTFusionStorage::operateHostMapping(MAP_METHOD method, const string& hostname, const list<string>& luns)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData, lunNames;
    jSendData[FUSION_REQ_HOST_NAME] = hostname;
    for (list<string>::const_iterator it = luns.begin(); it != luns.end(); it++){
        lunNames.append(string(*it));
    }
    jSendData[FUSION_REQ_LUN_NAMES] = lunNames;

    string url;
    if (method == MAP){
        url = FUSION_URL_MAP_LUN_TO_HOST;
    }
    else if (method == UNMAP){
        url = FUSION_URL_UNMAP_LUN_TO_HOST;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error method[map or unmap]");
        return RETURN_ERR;
    }

    iRet = fullRestPkg(url, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK || restPkg.getFusionStorageResult() != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), data[%s],description %s.",
            strIP.c_str(), url.c_str(), iRet, jSendData.toStyledString().c_str(), restPkg.getFusionDescription().c_str());
        return iRet;
    }

    return RETURN_OK;
}
int CRESTFusionStorage::operateHostGroupMapping(MAP_METHOD method, const string& hostGroupname, const list<string>& luns)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;

    Json::Value jSendData, lunList;
    jSendData[FUSION_REQ_HOSTGROUP_NAME] = hostGroupname;
    for (list<string>::const_iterator it = luns.begin(); it != luns.end(); it++){
        lunList.append(string(*it));
    }
    jSendData[FUSION_REQ_LUN_LIST] = lunList;

    string url;
    if (method == MAP){
        url = FUSION_URL_MAP_LUN_TO_HOSTGROUP;
    }
    else if(method == UNMAP){
        url = FUSION_URL_UNMAP_LUN_TO_HOSTGROUP;
    }
    else{
        COMMLOG(OS_LOG_ERROR, "error method[map or unmap]");
        return RETURN_ERR;
    }

    iRet = fullRestPkg(url, REST_REQUEST_MODE_POST, jSendData.toStyledString(), strIP, restPkg);
    if (iRet != RETURN_OK || restPkg.getFusionStorageResult() != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.",
            strIP.c_str(), url.c_str(), iRet, restPkg.getFusionDescription().c_str());
        return iRet;
    }

    return RETURN_OK;
}

int CRESTFusionStorage::fullRestPkg(const string& url, REST_REQUEST_MODE reqMode, const string& body, string& strIp, CRestPackage& restPkg)
{
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    int iRet = RETURN_OK;
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin();
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(url, reqMode, body, restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), url.c_str(), iRet);
            continue;
        }

        Json::Value objects;
        restPkg.getResponseObjects(objects);
        if(objects.isMember("error")){
            if(objects["error"].isMember("code"))
                if(objects["error"]["code"].isString())
                    iRet = atoi(objects["error"]["code"].asString().c_str());
        }
        else{
            iRet = restPkg.getFusionStorageResult();
        }
        strIp = *iter;

        if (iRet != RETURN_OK){
            return iRet;
        }

        return iRet;
    }
}

/*------------------------------------------------------------
Function Name: CMD_unmountSlave()
Description  : Cancel remote copy from lun map
Data Accessed:
Data Updated : None.
Input        : strMirrorID.
Output       : None
Return       : None.
Call         :
Called by    :
Create By    : 
Modification :
Others       :
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_unmountSlave(IN string &strMirrorID)
{
    int iRet = RETURN_ERR;
    int resCode = RETURN_ERR;
    CRestPackage restPkg;
    ostringstream oss;
    Json::Value objects;
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    jSendData[COMMON_TAG_ID] = strMirrorID;

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest("/REPLICATIONPAIR/unmount", REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the param [%s] the iRet is (%d).", iter->c_str(), "/REPLICATIONPAIR/unmount", jSendData.toStyledString().c_str(), iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        if (!objects.isNull()){
            if (objects.isMember("error"))
                if(objects["error"]["code"].isString())
                    resCode = atoi(objects["error"]["code"].asString().c_str());
                else
                    resCode = objects["error"]["code"].asInt();
            else if (objects.isMember("result"))
                if (!objects.isMember("errorCode"))
                    resCode = objects["result"].asInt();
                else
                    resCode = objects["errorCode"].asInt();
            else
                return RETURN_ERROR;
        }
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(), "/REPLICATIONPAIR/unmount", iRet, restPkg.getFusionDescription().c_str());
            return resCode;
        }
        return RETURN_OK;
    }

    return RETURN_ERR;
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
void CRESTFusionStorage::ConvertHostPortStatus(IN OUT unsigned int &ruiIntStatus)
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
void CRESTFusionStorage::UpdateGroupStatus(IN OUT unsigned int &ruiIntStatus)
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
void CRESTFusionStorage::ConvertHMPairStatus(IN OUT unsigned int &ruiIntState)
{
    map<unsigned int, unsigned int> mapHMPairStatus;
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_NORMAL, HM_SLAVE_PAIR_STATUS_CHAR_NORMAL));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_SYNCHRONIZING, HM_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_INTERRUPTED, HM_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_INVALID, HM_SLAVE_PAIR_STATUS_CHAR_INVALID));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_PAUSE, HM_SLAVE_PAIR_STATUS_CHAR_PAUSE));
    mapHMPairStatus.insert(make_pair(RUNNING_STATUS_TOBESYNCHRONIZED, HM_SLAVE_PAIR_STATUS_CHAR_TOBESYNCHRONIZED));

    if (mapHMPairStatus.find(ruiIntState) == mapHMPairStatus.end()){
        ruiIntState = HM_SLAVE_PAIR_STATUS_CHAR_NOT_EXIST;
        return;
    }

    ruiIntState = mapHMPairStatus[ruiIntState];

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
void CRESTFusionStorage::ConvertHMSlaveLUNStatus(IN OUT unsigned int &ruiIntState)
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
Others       :FusionStorage8.0 does not provide this interface for the time being. The default is to support hyperMetro
-------------------------------------------------------------*/
int CRESTFusionStorage::CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStrecthed)
{
    REMOTE_ARRAY_STRU peer;
    peer.strStretched = "true";
    rlstRemoteArrayStrecthed.push_back(peer);
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
int CRESTFusionStorage::CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{   
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo)
{
    int iRet = RETURN_OK;
    CRestPackage restPkg;
    string strIP;
    Json::Value jSendData;
    Json::Value objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";  
    jSendData["id"] = rstHMPairInfo.strID;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "GetOneHyperMetroPair";

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "query hyper metro info failed [CMD_showhypermetroinfo]", iRet);
            break;
        }

        restPkg.getResponseObjects(objects);
        iRet = restPkg.getFusionStorageResult();

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"query hyper metro info error [CMD_showhypermetroinfo]",iRet,restPkg.getFusionDescription().c_str());
            return iRet;
        }
        strIP = *iter;
        break;
    }

    rstHMPairInfo.strName = rstHMPairInfo.strID;     //No Name field Temporarily replaced by ID
    rstHMPairInfo.strDomainID = objects["serviceCmdData"][0]["domainID"].asString();
    rstHMPairInfo.bIsPrimary = (objects["serviceCmdData"][0]["isPrimary"].asString().compare("true") == 0);
    if(objects["serviceCmdData"][0]["cgId"].asString() != "--"){
        rstHMPairInfo.bIsBelongGroup = "true";
    }
    else{
        rstHMPairInfo.bIsBelongGroup = "false";
    }
            
    if (rstHMPairInfo.bIsBelongGroup){
        GROUP_INFO_STRU stGroupInfo;
        string strCGID = objects["serviceCmdData"][0]["cgId"].asString();
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

    rstHMPairInfo.uiStatus = HmHealthSatusConvert(objects["serviceCmdData"][0]["healthStatus"].asString());
    UpdateMirrorStatus(rstHMPairInfo.uiStatus);
    rstHMPairInfo.uiState = HmPairRunStatusConvert(objects["serviceCmdData"][0]["runningStatus"].asString());
    rstHMPairInfo.iProgress = typeCast<int,string>(objects["serviceCmdData"][0]["syncProgress"].asInt());   

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
int CRESTFusionStorage::CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo)
{
    int iRet = RETURN_OK;
    Json::Value jSendData;
    Json::Value objects;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();    

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){  
        int strStartId = 0;
        int startId = 0;
        string strDeviceIP = *iter;
        bool flag = true;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        rlstHyperMetroPairInfo.clear();

        while (true){
            CRestPackage restPkg;
            int resCode = -1; 
            
            jSendData.clear();
            objects.clear();
            jSendData["controlClusterId"] = "0";                 
            jSendData["count"] = 40;
            jSendData["id"] = strStartId;                        
            jSendData["serviceType"] = "dr";
            jSendData["subOp"] = "GetBatchHyperMetroPair";

            
            iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);   
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(),"doRequest failed [CMD_showHyperMetroPair_all]", iRet);
                flag = false;
                break;
            }

            restPkg.getResponseObjects(objects);
            resCode = restPkg.getFusionStorageResult();

            if (resCode != RETURN_OK){
                iRet = resCode;
                COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                    iter->c_str(),"query all the Hyper Metro Pair[CMD_showHyperMetroPair_all]", iRet, restPkg.getFusionDescription().c_str());    
                return resCode;
            }

            int i = 0;
            if (objects.isMember("serviceCmdData")){
                if (objects["serviceCmdData"].isArray()){
                    for (; i < objects["serviceCmdData"].size(); ++i){
                        Json::Value tempData = objects["serviceCmdData"][i];

                        if(tempData.isMember("hcResourceType"))                                                    {
                            if (FUSION_HCRESOURCETYPE_FS == tempData["hcResourceType"].asString()){
                                continue;
                            }
                        }

                        HYPERMETROPAIR_INFO_STRU stHyperMetroPairInfo;            
                        stHyperMetroPairInfo.strID = tempData["id"].asString(); 
                        stHyperMetroPairInfo.strDomainID = tempData["domainID"].asString();            
                        stHyperMetroPairInfo.strDomainName = tempData["domainName"].asString();

                        if (g_bstretch){
                            stHyperMetroPairInfo.stretched = "true";
                        }
                        else{
                            stHyperMetroPairInfo.stretched = "false";
                        }

                        stHyperMetroPairInfo.bIsPrimary = (tempData["isPrimary"].asString().compare("true") == 0); 

                        GROUP_INFO_STRU stGroupInfo;
                        string strCGID="";
                        if(tempData.isMember("cgId")){
                            strCGID = tempData["cgId"].asString();
                        }

                        if (strCGID == "--"){
                            stHyperMetroPairInfo.bIsBelongGroup = false;
                        }
                        else{
                            stHyperMetroPairInfo.bIsBelongGroup = true;
                            continue;                                                //The consistency group is not supported, and the consistency group temporarily skips directly.
                        }

                        stHyperMetroPairInfo.strGroupID = "";

                        if(stHyperMetroPairInfo.bIsBelongGroup){
                            if (strCGID.compare(STR_INVALID_CGID) != 0){
                                stHyperMetroPairInfo.strGroupID = strCGID;
                                stGroupInfo.strID = strCGID;
                                CMD_showconsistgrhminfo(stGroupInfo);
                                if (stGroupInfo.uiIsPrimary == HM_GROUP_RELATION_SLAVE)  {
                                    stHyperMetroPairInfo.bIsPrimary = false;
                                }
                                else{
                                    stHyperMetroPairInfo.bIsPrimary = true;
                                }
                            }
                        }

                        stHyperMetroPairInfo.strLocalLUNID = tempData["localObjID"].asString();
                        stHyperMetroPairInfo.strRemoteLunID = tempData["remoteObjID"].asString();
                        stHyperMetroPairInfo.uiState = HmPairRunStatusConvert(tempData["runningStatus"].asString());

                        if (stHyperMetroPairInfo.bIsPrimary ){
                            stHyperMetroPairInfo.uiRelationType = LUN_RELATION_MASTER;    
                            stHyperMetroPairInfo.uiSecResAccess = HmPairAccessConvert(tempData["remoteHostAccessState"].asString());
                        }
                        else{
                            stHyperMetroPairInfo.uiRelationType = LUN_RELATION_SLAVE;
                            stHyperMetroPairInfo.uiSecResAccess = HmPairAccessConvert(tempData["localHostAccessState"].asString());
                        }

                        if(stHyperMetroPairInfo.bIsBelongGroup){
                            stHyperMetroPairInfo.uiSecResAccess = stGroupInfo.uiSecResAccess;
                        }

                        rlstHyperMetroPairInfo.push_back(stHyperMetroPairInfo);
                    }
                }
            }

            if (40 == i){
                startId += 40;
                stringstream strStream;
                strStream << startId;
                strStream >> strStartId;
            }
            else{
                break;
            }
        }

        if(flag){
            return RETURN_OK;
        }
        else{
            continue;
        }
    }

    return RETURN_ERR;    
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
int CRESTFusionStorage::CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo)
{
    int iRet = RETURN_OK;
    int resCode = -1;
    CRestPackage restPkg;
    string strTmpID;
    list<string> lstTmpID;
    string strIP;
    Json::Value jSendData;
    Json::Value    objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";
    jSendData["id"] = hypermetroid;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "GetOneHyperMetroPair";

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    rlstHMLUNInfo.clear();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "query hypermetro by Id failed [CMD_showhypermetrolun_all] ", iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        resCode = restPkg.getFusionStorageResult();

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"query hypermetro by Id failed [CMD_showhypermetrolun_all]",iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }

        strIP = *iter;
        break;
    }

    SYSTEM_INFO_STRU rstSysInfo;
    int res = CMD_showsys(rstSysInfo);

    if (res != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] CMD_showsys fail [%d].", strIP.c_str(), "[CMD_showhypermetrolun_all]", res);
        return RETURN_ERR;
    }

    if(objects.isMember("serviceCmdData")){
        bool isPrimary = false;
        isPrimary = (objects["serviceCmdData"][0]["isPrimary"].asString().compare("true") == 0);

        int iProgress = 0;
        iProgress = jsonValue2Type<int>(objects["serviceCmdData"][0]["syncProgress"]);   

        unsigned int uiDataStatusRemote = 0;
        uiDataStatusRemote = HmPairDataStateConvert(objects["serviceCmdData"][0]["remoteDataState"].asString());  
        if (0xFFFFFFFF == uiDataStatusRemote){
            uiDataStatusRemote = 0;
        }

        unsigned int uiDataStatusLocal = 0;
        uiDataStatusLocal = HmPairDataStateConvert(objects["serviceCmdData"][0]["localDataState"].asString());  
        if (0xFFFFFFFF == uiDataStatusLocal){
            uiDataStatusLocal = 0;
        }
    
        unsigned int uiPairStatus = 0;
        uiPairStatus = HmPairRunStatusConvert(objects["serviceCmdData"][0]["runningStatus"].asString());
        if (0xFFFFFFFF == uiPairStatus){
            uiPairStatus = 0;
        }

        if (uiPairStatus == RUNNING_STATUS_NORMAL && iProgress == -1)        {
            iProgress = 100;
        }

        unsigned int uiHealthStatus = 0;
        uiHealthStatus = HmHealthSatusConvert(objects["serviceCmdData"][0]["healthStatus"].asString());
        if (0xFFFFFFFF == uiHealthStatus){
            uiHealthStatus = 0;
        }

        unsigned long long ullStartTime = 0;
        ullStartTime = jsonValue2Type<unsigned long long>(objects["serviceCmdData"][0]["lastSyncStartTime"]);   //LASTSYNCSTARTTIME 是string类型，可以直接转为 long long吗？？

        HYPERMETRO_LF_INFO_STRU stHMLUNInfoLocal, stHMLUNInfoRemote;

        stHMLUNInfoLocal.strHMpairID = hypermetroid;
        stHMLUNInfoLocal.iProgress = iProgress;
        stHMLUNInfoLocal.uiDataStatus = uiDataStatusLocal;
        ConvertHMSlaveLUNStatus(stHMLUNInfoLocal.uiDataStatus);    
        TimeChg(ullStartTime, stHMLUNInfoLocal.strSyncStartTime);
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

        stHMLUNInfoLocal.strID = objects["serviceCmdData"][0]["localObjID"].asString();
        stHMLUNInfoLocal.strName = objects["serviceCmdData"][0]["localObjName"].asString();
        stHMLUNInfoLocal.strArrayWWN = rstSysInfo.strDeviceSerialNumber;
        stHMLUNInfoLocal.strArrayName = rstSysInfo.strName;
        stHMLUNInfoLocal.strDomainID = objects["serviceCmdData"][0]["domainID"].asString();

        string strCGID = "";
        strCGID = objects["serviceCmdData"][0]["cgId"].asString();
        if (strCGID == "--"){
            stHMLUNInfoLocal.strIsinCg = "false";
        }
        else{
            stHMLUNInfoLocal.strIsinCg = "true";
        }


        GROUP_INFO_STRU stGroupInfo;

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
        
        stHMLUNInfoRemote.strID = objects["serviceCmdData"][0]["remoteObjID"].asString();
        stHMLUNInfoRemote.strName = objects["serviceCmdData"][0]["remoteObjName"].asString();
        stHMLUNInfoRemote.strDomainID = objects["serviceCmdData"][0]["domainID"].asString();

        if (strCGID == "--"){
            stHMLUNInfoRemote.strIsinCg = "false";
        }    
        else{
            stHMLUNInfoRemote.strIsinCg = "true";
        }
                
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
        else{
            COMMLOG(OS_LOG_ERROR, "query remote lunInfo fail,Remote.strDomainID[%s]", stHMLUNInfoRemote.strDomainID.c_str());
        }

        if(stHMLUNInfoLocal.strIsinCg.compare("true") != 0){
            if (isPrimary){
                stHMLUNInfoLocal.uiRelationType = LUN_RELATION_MASTER;
                stHMLUNInfoRemote.uiRelationType = LUN_RELATION_SLAVE;    
                if(objects["serviceCmdData"][0].isMember("remoteHostAccessState")){
                    stHMLUNInfoRemote.uiSecResAccess = HmPairAccessConvert(objects["serviceCmdData"][0]["remoteHostAccessState"].asString());
                    stHMLUNInfoLocal.uiSecResAccess = HmPairAccessConvert(objects["serviceCmdData"][0]["remoteHostAccessState"].asString());
                }
            }
            else{
                stHMLUNInfoLocal.uiRelationType = LUN_RELATION_SLAVE;
                stHMLUNInfoRemote.uiRelationType = LUN_RELATION_MASTER;
                if(objects["serviceCmdData"][0].isMember("localHostAccessState"))  {
                    stHMLUNInfoRemote.uiSecResAccess = HmPairAccessConvert(objects["serviceCmdData"][0]["localHostAccessState"].asString());
                    stHMLUNInfoLocal.uiSecResAccess = HmPairAccessConvert(objects["serviceCmdData"][0]["localHostAccessState"].asString());
                }
            }
            
            // Use arbitration, and the health status of the current active pair is faulty
            if ("false" == stHMLUNInfoLocal.strCptype && FUSION_HEALTH_FAULT == stHMLUNInfoLocal.uiHealthStatus){
                unsigned int uiLocalAccessState = 0;
                if(objects["serviceCmdData"][0].isMember("localHostAccessState")){                                                                                  
                    uiLocalAccessState = HmPairAccessConvert(objects["serviceCmdData"][0]["localHostAccessState"].asString());
                }

                unsigned int uiRemoteAccessState = 0;
                if(objects["serviceCmdData"][0].isMember("remoteHostAccessState")){
                    uiRemoteAccessState = HmPairAccessConvert(objects["serviceCmdData"][0]["remoteHostAccessState"].asString());
                }

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
    }
    
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
int CRESTFusionStorage::CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo)
{
    int iRet = RETURN_OK;
    int resCode = -1;
    CRestPackage restPkg;
    string strTmpID;
    list<string> lstTmpID;
    string strIP = "";
    Json::Value jSendData;
    Json::Value objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";       
    jSendData["hyperMetroDomainId"] = hmdomainid;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "QueryHyperMetroDomain";
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "query hyper metro domain failed [CMD_showhyperMetroDomain_all]", iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        resCode = restPkg.getFusionStorageResult();

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"query hyper metro domain error [CMD_showhyperMetroDomain_all]",iRet,restPkg.getFusionDescription().c_str());

            return resCode;
        }

        strIP = *iter;
        break;
    }

    if(objects.isMember("serviceCmdData")){
        HYPERMETRODOMAIN_LF_INFO_STRU stHMDomainInfoLocal;
        stHMDomainInfoLocal.strID = objects["serviceCmdData"][0]["hyperMetroDomainId"].asString();
        stHMDomainInfoLocal.strName = objects["serviceCmdData"][0]["name"].asString();
        stHMDomainInfoLocal.uiStatus = HmDomainRunSatusConvert(objects["serviceCmdData"][0]["runningStatus"].asString());
    
        std::string str;
        str = objects["serviceCmdData"][0]["remoteDeviceId"].asString();
        REMOTE_ARRAY_STRU remote_dev;
        
        int ret = findRemoteArraySn(str, remote_dev);
        if (ret == RETURN_OK){  
            stHMDomainInfoLocal.strRemoteArraySN = remote_dev.strSN;
            stHMDomainInfoLocal.strRemoteArrayName = remote_dev.strName;
        }
        else{
            COMMLOG(OS_LOG_ERROR, "decode remote device fail[%s]", str.c_str());
            return RETURN_ERR;
        }
        
        if(objects["serviceCmdData"][0].isMember("CPType")){
            if (0 == objects["serviceCmdData"][0]["CPType"].asString().compare(HM_CPTYPE_NONE)){
                stHMDomainInfoLocal.strCptype = "true";
            }
        }
        else {
            //Since the SRM has a problem with the staticSitePreference field support, it is temporarily changed to true, that is, whether the array supports arbitration or not, returning to vmware is true.
            stHMDomainInfoLocal.strCptype = "false";
        }

        rstHMDomainInfo = stHMDomainInfoLocal;
    }
    
    return RETURN_OK;
}

int CRESTFusionStorage::findRemoteArraySn(const string& sn, REMOTE_ARRAY_STRU& remote_dev)
{
    list<REMOTE_ARRAY_STRU> rlstRemoteArrayInfo;
    int ret = CMD_showarrayinfo(rlstRemoteArrayInfo);

    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "decode remote device fail[%s]", sn.c_str());
        return ret;
    }
    
    for (list<REMOTE_ARRAY_STRU>::iterator it = rlstRemoteArrayInfo.begin(); it != rlstRemoteArrayInfo.end(); it++){
        if (sn.find(it->strWWN) != string::npos){
            remote_dev = *it;
            COMMLOG(OS_LOG_INFO, "find peer array success");
            return RETURN_OK;
        }
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
int CRESTFusionStorage::CMD_swaphypermetro(IN string &strHMPairID)
{
    int iRet = RETURN_OK;
    int resCode = -1;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    Json::Value objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";
    jSendData["id"] = strHMPairID;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "SwapHyperMetroPair";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);
        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "swap hyper metro failed [CMD_swaphypermetro]", iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        resCode = restPkg.getFusionStorageResult();

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"swap hyper metro error [CMD_swaphypermetro]",iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }

        return RETURN_OK;
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
int CRESTFusionStorage::CMD_stopconsistgrhm(IN string strGroupID)
{
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_showconsistgrinfo(IN OUT list<GROUP_INFO_STRU> &rstGroupInfos)
{
    return RETURN_OK;
}

int CRESTFusionStorage::CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{   
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_splitconsistgr(IN string strGroupID)
{  
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_swapconsistgrhm(IN string strGroupID)
{
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
int CRESTFusionStorage::CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID)
{
    int iRet = RETURN_OK,resCode = 0;
    CRestPackage restPkg;
    string strIP;
    Json::Value jSendData,objects;
    jSendData.clear();
    jSendData["hostGroupName"] = "0";
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        strIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest("/hostGroup/host/list", REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "/hostGroup/host/list", iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        if (!objects.isNull()){
            if (objects.isMember("error")){
                if(objects["error"].isMember("code")){
                    if(objects["error"]["code"].isString())
                        resCode = atoi(objects["error"]["code"].asString().c_str());
                    else
                        resCode = objects["error"]["code"].asInt();
                }
            }
            else if (objects.isMember("result")){
                if (!objects.isMember("errorCode")){
                    resCode = objects["result"].asInt();
                }
                else{
                    resCode = objects["errorCode"].asInt();
                    if(objects["errorCode"].isString())
                        resCode = atoi(objects["errorCode"].asString().c_str());
                    else
                        resCode = objects["errorCode"].asInt();
                }
            }
            else
                resCode = -1;
        }
        else
                resCode = -1;
        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),FUSION_URL_STRETCH,iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }
        break;
    }

    if (iRet != RETURN_OK){
        return iRet;
    }

    lstHostID.clear();
    if (restPkg.count() == 0){
        COMMLOG(OS_LOG_INFO, "ip [%s] url [%s] Not found any map by lun Group[%s].", strIP.c_str(), "/hostGroup/host/list", HostGroupID.c_str());    
        return RETURN_OK;
    }

    int i = 0;
    if(objects.isMember("hostList"))
        for (; i < objects["hostList"].size(); i++){
            if(objects["hostList"].isMember("hostName"))
                lstHostID.push_back(objects["objects"]["hostList"][i]["hostName"].asString());
        }

    return RETURN_OK;
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
int CRESTFusionStorage::CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo)
{
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    int iRet = RETURN_OK;
    int resCode = -1;
    CRestPackage restPkg;

    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    Json::Value objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";
    jSendData["id"] = rstrHMPairID;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "DisableHyperMetroPair";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "disable the hyper metro failed [CMD_disablehcpair]", iRet);
            continue;
        }
        restPkg.getResponseObjects(objects);
        resCode = restPkg.getFusionStorageResult();

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"disable the hyper metro failed [CMD_disablehcpair]",iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }

        return RETURN_OK;
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
int CRESTFusionStorage::CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_syncconsistgrhm(IN string strGroupID)
{
    return RETURN_OK;
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
int CRESTFusionStorage::CMD_synchmpair(IN string &strHMPairID)
{
    int iRet = RETURN_OK;
    int resCode = -1;
    CRestPackage restPkg;
    
    TLV_LOGIN_INFO_STRU rstLogInInfo = GetLoginInfo();
    Json::Value jSendData;
    Json::Value objects;
    jSendData.clear();
    jSendData["controlClusterId"] = "0";
    jSendData["id"] = strHMPairID;
    jSendData["serviceType"] = "dr";
    jSendData["subOp"] = "SyncHyperMetroPair";

    for (list<string>::const_iterator iter = rstLogInInfo.lstArrayIP.begin(); 
        iter != rstLogInInfo.lstArrayIP.end(); ++iter){
        string strDeviceIP = *iter;
        CRESTConn *restConn = getConn(strDeviceIP, rstLogInInfo.strArrayUser, rstLogInInfo.strArrayPwd);

        iRet = restConn->doRequest(FUSION_URL_STRETCH, REST_REQUEST_MODE_POST, jSendData.toStyledString(), restPkg);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d).", iter->c_str(), "sync hyper metro failed [CMD_synchmpair]", iRet);
            continue;
        }

        restPkg.getResponseObjects(objects);
        resCode = restPkg.getFusionStorageResult();

        if (resCode != RETURN_OK){
            iRet = resCode;
            COMMLOG(OS_LOG_ERROR, "ip [%s] url [%s] the iRet is (%d), description %s.", 
                iter->c_str(),"sync hyper metro error [CMD_synchmpair]",iRet,restPkg.getFusionDescription().c_str());
            return resCode;
        }

        return RETURN_OK;
    }

    return RETURN_ERR;
}