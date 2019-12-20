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

#include "RESTPackage.h"
#include <json/reader.h>
#include "Log.h"
#include <algorithm>
#include "common.h"

#ifdef WIN32
#include "../TLVCom/communication.h"
#else
#include "communication.h"
#endif

CRestPackage::CRestPackage(){
    // define pointer avoid 4251 waring
    jNullValue = new Json::Value();
    vMsgRecs = new vector<Json::Value>();
    strDescription="";
    iErrorCode = -1;
    dErrorCode = -1;
}

CRestPackage::CRestPackage(const CRestPackage &pkgRest){
    this->strDescription = pkgRest.strDescription;
    jNullValue = new Json::Value();
    vMsgRecs = new vector<Json::Value>();
    this->iErrorCode = pkgRest.iErrorCode;
    this->dErrorCode = pkgRest.dErrorCode;

    (void)std::copy(pkgRest.vMsgRecs->begin(), pkgRest.vMsgRecs->end(), this->vMsgRecs->begin());
}

CRestPackage::~CRestPackage(){
    try{
        vMsgRecs->clear();
        delete(jNullValue);
        delete(vMsgRecs);
    }
    catch(...){}
    
    vMsgRecs = NULL;
    jNullValue = NULL;
}

int CRestPackage::decode(std::string &strMsgInfo){
    // clear data
    vMsgRecs->clear();
    m_responseObjects.clear();

    Json::Reader reader;
    Json::Value objects;
    this->iErrorCode = -1;
    this->dErrorCode = -1;

    if (!reader.parse(strMsgInfo, m_responseObjects)){
        COMMLOG(OS_LOG_ERROR, "decode json value failed");
        return -1;
    }
    
    if (g_bFusionStorage){
        //this->iErrorCode = m_responseObjects["result"].asInt();
        this->dErrorCode = getFusionStorageDetailErrorcode();
        this->iErrorCode = getFusionStorageResult();
        this->strDescription = getFusionDescription();
        return 0;
    }

    this->iErrorCode = m_responseObjects["error"]["code"].asInt();
    this->strDescription = m_responseObjects["error"]["description"].asString();

    // add child when data is array
    if (m_responseObjects.isMember("data")){
        if (m_responseObjects["data"].isArray()){
            for (int i = 0; i < m_responseObjects["data"].size(); ++i){
                vMsgRecs->push_back(m_responseObjects["data"][i]);
            }
        }
        else{
            vMsgRecs->push_back(m_responseObjects["data"]);
        }
    }
    return 0;
}

void CRestPackage::dump(std::ostringstream &str) const{
    str <<"{/*package*/" <<std::endl;
    str <<"    error: {" <<std::endl;
    str <<"        code:" <<this->iErrorCode <<std::endl;
    str <<"           description:" <<this->strDescription <<std::endl;
    str <<"    }," <<std::endl;

    str <<"    data: [" <<endl;
    for (std::vector<Json::Value>::const_iterator it = vMsgRecs->begin();
        it != vMsgRecs->end(); ++it){
        str <<(*it).toStyledString() <<endl;
    }
    str <<"    ]" <<endl;
    str <<"}" << std::endl;
}

Json::Value& CRestPackage::operator [](size_t index){
    if (vMsgRecs->size() <= index){
        COMMLOG(OS_LOG_ERROR, "operator[] error: request index=%d \n", index);
        return *jNullValue;
    }

    return (*vMsgRecs)[index];
}

CRestPackage &CRestPackage::operator =(const CRestPackage &pkgRest){
    if (this == &pkgRest){
        return *this;
    }
    this->iErrorCode = pkgRest.iErrorCode;
    this->strDescription = pkgRest.strDescription;
    this->vMsgRecs->assign(pkgRest.vMsgRecs->begin(), pkgRest.vMsgRecs->end());
    return *this;
}

int CRestPackage::getFusionStorageDetailErrorcode(){
    if (!m_responseObjects.isNull()){
        if (m_responseObjects.isMember("detail")){
            if (m_responseObjects["detail"][0].isMember("errorCode")){
                if ((m_responseObjects["detail"][0])["errorCode"].isString()){
                    return atoi((m_responseObjects["detail"][0])["errorCode"].asString().c_str());
                }
                else{
                    return (m_responseObjects["detail"][0])["errorCode"].asInt();
                }
            }
            else{
                return RETURN_ERR;
            }            
        }
    }
}
int CRestPackage::getFusionStorageResult(){
    if (!m_responseObjects.isNull()){
        if (m_responseObjects.isMember("error")){
            if (m_responseObjects["error"].isMember("description"))
                strDescription = m_responseObjects["error"]["description"].asString();
            else
                strDescription = "";
            if (m_responseObjects["error"].isMember("code")){
                if (m_responseObjects["error"]["code"].isString()){
                    return atoi(m_responseObjects["error"]["code"].asString().c_str());
                }
                else{
                    return m_responseObjects["error"]["code"].asInt();
                }
            }
            else
                return RETURN_ERR;
        }
        else if (m_responseObjects.isMember("result")){
            if (m_responseObjects.isMember("errorCode")){
                if (m_responseObjects.isMember("description"))
                    strDescription = m_responseObjects["description"].asString();
                if (m_responseObjects["errorCode"].isString()){
                    return atoi(m_responseObjects["errorCode"].asString().c_str());
                }
                else{
                    return m_responseObjects["errorCode"].asInt();
                }               
            }
            else if(m_responseObjects.isMember("serviceCmdData")){
                stringstream io;
                int e;
                if (m_responseObjects["serviceCmdData"].isArray()){
                    for (int i = 0; i < m_responseObjects["serviceCmdData"].size(); i++){
                        io.clear();
                        io << m_responseObjects["serviceCmdData"][i]["result"];
                        if (io.str() != "0"){
                            io >> e;
                            return e;
                        }
                            
                    }
                    return RETURN_OK;
                }
                else{
                    io.clear();
                    io.clear();
                    io << m_responseObjects["serviceCmdData"]["result"];
                    io >> e;
                    return e;
                }
            }
            else{
                strDescription = "";
                return m_responseObjects["result"].asInt();
            }
            
        }
        else{
            return RETURN_ERROR;
        }

    }
    else{
        return RETURN_ERROR;
    }

}
string CRestPackage::getFusionDescription(){
    return strDescription;
}
