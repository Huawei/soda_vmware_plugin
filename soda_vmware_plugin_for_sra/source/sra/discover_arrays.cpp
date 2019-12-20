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

#include "discover_arrays.h"
#include "../common/xml_node/arrays.h"
#include "sra_basic.h"
#include "sra.h"

int discover_arrays(XmlReader &reader)
{
    DiscoverArrays da;

    COMMLOG(OS_LOG_INFO, "%s", "discover_arrays begin.");
    print("%s", "discover_arrays begin.");
    if (RETURN_OK != da.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "discover array failed.");

        return RETURN_ERR;
    }
    
    COMMLOG(OS_LOG_INFO, "%s", "discover_arrays end.");
    print("%s", "discover_arrays end.");
    return RETURN_OK;
}

int DiscoverArrays::_outband_process()
{
    COMMLOG(OS_LOG_INFO,"%s","DiscoverArrays _outband_process begin");

    int ret = RETURN_ERR;
    CCmdOperate cmdOperate;
    HYPER_STORAGE_STRU stStorageInfo;

    (void)setStorageInfo(stStorageInfo);

    ret = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    ostringstream oss;
    stStorageInfo.dump(oss);

    
    ret = _get_sysinfo(cmdOperate);
    CHECK_UNEQ(RETURN_OK, ret);

    
    return _get_peer_array(cmdOperate);
}

int DiscoverArrays::_get_sysinfo(CCmdOperate& cmdOperate)
{
    COMMLOG(OS_LOG_INFO,"%s", "_get_sysinfo begin");

    int ret = RETURN_ERR;
    string strErrorMsg;
    HYPER_STORAGE_STRU stStorageInfo;

    ret = cmdOperate.CMD_showtlvsysinfo(stStorageInfo, strErrorMsg);
    CHECK_UNEQ(RETURN_OK, ret);

    SNPRINTF(array_id, sizeof(array_id), sizeof(array_id) - 1, "%s", stStorageInfo.strArraySN.c_str());
    array_name = stStorageInfo.strArrayName;
    system_type = stStorageInfo.strArrayModel;

    COMMLOG(OS_LOG_INFO, "%s","_get_sysinfo end");
    return RETURN_OK;
}

int DiscoverArrays::_get_peer_array(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU> lstRemoteStretchedInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayStretched;
    peer_arrays.clear();

    ret = cmdOperate.CMD_showarrayinfo(lstRemoteArrayInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    if(g_bstretch){
        ret = cmdOperate.CMD_showarraystretched(lstRemoteStretchedInfo);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "execute CMD_showarraystretched failed, ret = %d", ret);
        }
    }
    
    if (g_bstretch){
        
        for (itRemoteArrayInfo = lstRemoteArrayInfo.begin(), itRemoteArrayStretched = lstRemoteStretchedInfo.begin();
            itRemoteArrayInfo != lstRemoteArrayInfo.end(); 
            itRemoteArrayInfo++){
            peer_arrays.push_back(itRemoteArrayInfo->strSN);
            peer_stretched.push_back(itRemoteArrayStretched->strStretched);

            COMMLOG(OS_LOG_INFO, "Find array [%s]",itRemoteArrayInfo->strSN.c_str());
        }
    }
    else{
        for (itRemoteArrayInfo = lstRemoteArrayInfo.begin(); itRemoteArrayInfo != lstRemoteArrayInfo.end(); itRemoteArrayInfo++){
            peer_arrays.push_back(itRemoteArrayInfo->strSN);
            COMMLOG(OS_LOG_INFO, "Find array [%s]",itRemoteArrayInfo->strSN.c_str());
        }
    }

    if (peer_arrays.empty()){
        COMMLOG(OS_LOG_ERROR, "get peer arrays of array(%s) failed", id);
        return RETURN_ERR;
    }

    return RETURN_OK;
}

void DiscoverArrays::_write_response(XmlWriter &writer)
{
    string strBandInfo;
    string strManuFactoryInfo;
    CISMConfig oConfig;
    if (oConfig.open("config",".txt") != false){
        if(oConfig.getStringValue("BandInfo", strBandInfo) == false){
            strBandInfo = "OceanStor";
        }
        if(oConfig.getStringValue("ManuFactoryInfo", strManuFactoryInfo) == false){
            strManuFactoryInfo = "******";
        }
    }
    ArrayInfo arrayinfo;

    arrayinfo.peer_arrays = peer_arrays;
    arrayinfo.peer_stretched = peer_stretched;

    
    if (INBAND_TYPE == g_cli_type){
        strncpy_s(arrayinfo.id, sizeof(arrayinfo.id), id, sizeof(arrayinfo.id)-1);
        arrayinfo.id[sizeof(arrayinfo.id)-1] = '\0';
    }
    else{
        strncpy_s(arrayinfo.id, sizeof(arrayinfo.id), array_id, sizeof(arrayinfo.id)-1);
        arrayinfo.id[sizeof(arrayinfo.id)-1] = '\0';
    }
    strncpy_s(arrayinfo.name, sizeof(arrayinfo.name), array_name.c_str(), sizeof(arrayinfo.name)-1);
    arrayinfo.name[sizeof(arrayinfo.name)-1] = '\0';

    arrayinfo.model.model_name = strBandInfo;
    arrayinfo.model.vender_name = strManuFactoryInfo;
    arrayinfo.software.software_name = "ISM";
    arrayinfo.software.software_version = "1.0";

    (void)writer.set_xml("/Response/Arrays", &arrayinfo);

    return;
}
