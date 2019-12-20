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

#include "CmdAdapter.h"

/*------------------------------------------------------------
Function Name: int CCmdAdapter::SetSN(const string& sn) 
Description  : Set the target array SN to be commanded
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
int CCmdAdapter::SetSN(const string& sn) 
{

    if (sn == m_strSN){
        return ISSP_RTN_OK;
    }
    
    (void)InitConnectInfo(sn);

    m_strSN = sn;

    return RETURN_OK;
};
/*------------------------------------------------------------
Function Name: int CCmdAdapter::SetLoginInfo(const TLV_LOGIN_INFO_STRU &rstLoginInfo) 
Description  : Set the IP, username, and password of the target array that needs to be commanded.
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
void CCmdAdapter::SetLoginInfo(const TLV_LOGIN_INFO_STRU &rstLoginInfo)
{
    m_stLoginInfo.lstArrayIP.clear();
    m_stLoginInfo.strArrayPwd = "";
    m_stLoginInfo.strArrayUser = "";


    m_stLoginInfo.lstArrayIP = rstLoginInfo.lstArrayIP;
    m_stLoginInfo.strArrayUser = rstLoginInfo.strArrayUser;
    m_stLoginInfo.strArrayPwd = rstLoginInfo.strArrayPwd;
}

/*------------------------------------------------------------
Function Name: CMD_translatemodel 
Description  : Convert to a string product model based on the acquired digital product model
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
void CCmdAdapter::CMD_translatemodel(IN unsigned int &uiProductModel,OUT string &strProductModel)
{
    map<unsigned int, string> mapGroupStatus;
    (void)mapGroupStatus.insert(make_pair(HUASY_V1500,         PRODUCT_MODEL_V1500));
    (void)mapGroupStatus.insert(make_pair(HUASY_V1800,         PRODUCT_MODEL_V1800));
    (void)mapGroupStatus.insert(make_pair(HUASY_S2100,         PRODUCT_MODEL_S2100));
    (void)mapGroupStatus.insert(make_pair(HUASY_S2300,         PRODUCT_MODEL_S2300));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5100,         PRODUCT_MODEL_S5100));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5300,         PRODUCT_MODEL_S5300));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5500,         PRODUCT_MODEL_S5500));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5600,         PRODUCT_MODEL_S5600));
    (void)mapGroupStatus.insert(make_pair(HUASY_VIS,           PRODUCT_MODEL_VIS));
    (void)mapGroupStatus.insert(make_pair(HUASY_S2600,         PRODUCT_MODEL_S2600));
    (void)mapGroupStatus.insert(make_pair(HUASY_S6800E,        PRODUCT_MODEL_S6800E));
    (void)mapGroupStatus.insert(make_pair(HUASY_V1500N,        PRODUCT_MODEL_V1500N));
    (void)mapGroupStatus.insert(make_pair(HUASY_S2300E,        PRODUCT_MODEL_S2300E));
    (void)mapGroupStatus.insert(make_pair(HUASY_COMMON,        PRODUCT_MODEL_COMMON));
    (void)mapGroupStatus.insert(make_pair(HUASY_N8000,         PRODUCT_MODEL_N8000));
    (void)mapGroupStatus.insert(make_pair(HUASY_S6900,         PRODUCT_MODEL_S6900));
    (void)mapGroupStatus.insert(make_pair(HUASY_S3900,         PRODUCT_MODEL_S3900));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5500T,        PRODUCT_MODEL_S5500T));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5600T,        PRODUCT_MODEL_S5600T));
    (void)mapGroupStatus.insert(make_pair(HUASY_S6800T,        PRODUCT_MODEL_S6800T));
    (void)mapGroupStatus.insert(make_pair(HUASY_S6900_M100,    PRODUCT_MODEL_S6900_M100));
    (void)mapGroupStatus.insert(make_pair(HUASY_S3900_M200,    PRODUCT_MODEL_S3900_M200));
    (void)mapGroupStatus.insert(make_pair(HUASY_S3900_M300,    PRODUCT_MODEL_S3900_M300));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5900_M100,    PRODUCT_MODEL_S5900_M100));
    (void)mapGroupStatus.insert(make_pair(HUASY_S8100,         PRODUCT_MODEL_S8100));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5800T,        PRODUCT_MODEL_S5800T));
    (void)mapGroupStatus.insert(make_pair(HUASY_V1600N,        PRODUCT_MODEL_V1600N));
    (void)mapGroupStatus.insert(make_pair(HUASY_S5900_M200,    PRODUCT_MODEL_S5900_M200));
    (void)mapGroupStatus.insert(make_pair(HUASY_N8001,         PRODUCT_MODEL_N8001));
    (void)mapGroupStatus.insert(make_pair(HUASY_S2600T,        PRODUCT_MODEL_S2600T));
    (void)mapGroupStatus.insert(make_pair(HUASY_S8000_I,       PRODUCT_MODEL_S8000));
    (void)mapGroupStatus.insert(make_pair(HUASY_SSS,           PRODUCT_MODEL_SSS));
    (void)mapGroupStatus.insert(make_pair(HUASY_VIS8200,       PRODUCT_MODEL_VIS8200));
    (void)mapGroupStatus.insert(make_pair(HUASY_VIS8400,       PRODUCT_MODEL_VIS8400));
    (void)mapGroupStatus.insert(make_pair(HUASY_VIS8600,       PRODUCT_MODEL_VIS8600));
    (void)mapGroupStatus.insert(make_pair(HUASY_VIS8800,       PRODUCT_MODEL_VIS8800));

    if ( ( RETURN_OK == uiProductModel) || (mapGroupStatus.find(uiProductModel) == mapGroupStatus.end())){
        strProductModel = PRODUCT_MODEL_UNKOWN;
        return;
    }

    strProductModel = mapGroupStatus[uiProductModel];
}

/*------------------------------------------------------------
Function Name: CMD_addFilterToWWN 
Description  : Query the WWN add separator:
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
void CCmdAdapter::CMD_addFilterToWWN(IN string& strSourceWWN, OUT string& strTargetWWN)
{
    unsigned int strLen = 0;

    strTargetWWN.clear();

    if (string::npos != strSourceWWN.find(WWN_FILTER_CON)){
        strTargetWWN = strSourceWWN;
        return;
    }

    strTargetWWN = "";
    strLen = strSourceWWN.size();
    for (unsigned int i = 0; i < strLen; i += 2){
        if (i < strLen - 2){
            strTargetWWN = strTargetWWN + strSourceWWN.substr(i, 2) + WWN_FILTER_CON;
        }
        else{
            strTargetWWN = strTargetWWN + strSourceWWN.substr(i, 2);
        }
    }
    if (strTargetWWN.find("0x:") != string::npos){
        strTargetWWN = string(strTargetWWN.begin() + 3, strTargetWWN.end());
    }
    return;
}

/*------------------------------------------------------------
Function Name: CMD_delFrontZero 
Description  : Remove the 0 before the numeric string
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
void CCmdAdapter::CMD_delFrontZero(IN string& strSourceID, OUT string& strTargetID)
{
    string strNumerics("123456789abcdef");
    string::size_type strIndex;

    strIndex =  strSourceID.find_first_of(strNumerics);
    if (string::npos != strIndex){
        strTargetID = strSourceID.substr(strIndex);
    }
    else{
        strTargetID = "0";
    }

    return;
}

/*------------------------------------------------------------
Function Name: CMD_delFilterFromWWN 
Description  : Remove the delimiter from the wwn delivered by SRM:
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
void CCmdAdapter::CMD_delFilterFromWWN(IN string strSourceWWN, OUT string& strTargetWWN)
{
    std::string::size_type pos;

    strTargetWWN.clear();

    strSourceWWN += WWN_FILTER_CON;
    int size = strSourceWWN.size();

    for(int i=0; i<size; i++){
        pos = strSourceWWN.find(WWN_FILTER_CON, i);
        if(pos < size){
            std::string s = strSourceWWN.substr(i, pos-i);
            strTargetWWN += s;
            i = pos;
        }
    }

    return;
}