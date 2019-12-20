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

#include "prepare_failover.h"

PrepareFailove::PrepareFailove() : SraBasic()
{
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
}

PrepareFailove::~PrepareFailove()
{

};

int prepare_failover(XmlReader &reader)
{
    PrepareFailove pre_failover;

    COMMLOG(OS_LOG_INFO, "%s", "prepare_failover begin.");
    print("%s", "prepare_failover begin.");
    if (RETURN_OK != pre_failover.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "failed to prepare failover.");
        return RETURN_ERR;
    }
    
    COMMLOG(OS_LOG_INFO, "%s", "prepare_failover end.");
    print("%s", "prepare_failover end.");
    return RETURN_OK;
}

int PrepareFailove::_read_command_para(XmlReader &reader)
{
    int i = 0;
    int cg_count = 0;
    int device_count = 0;

    (void)reader.get_string("/Command/PrepareFailoverParameters/ArrayId", array_id, 0);
    (void)reader.get_string("/Command/PrepareFailoverParameters/ArrayId", input_array_id, 0);

    cg_count = reader.get_count("/Command/PrepareFailoverParameters/ConsistencyGroups/ConsistencyGroup");
    for (i = 0; i < cg_count; i++){
        char cg_id[LENGTH_COMMON];
        ConGroupInfo gp_info;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        (void)reader.get_string("/Command/PrepareFailoverParameters/ConsistencyGroups/ConsistencyGroup/@id", cg_id, i);

        
        gp_info.cg_id = cg_id;
        group_info.lst_groups_info.push_back(gp_info);
    }

    device_count = reader.get_count("/Command/PrepareFailoverParameters/SourceDevices/SourceDevice");
    for (i = 0; i < device_count; i++){
        char dev_id[LENGTH_COMMON];
        SourceDeviceInfo device_info;

        memset_s(dev_id, sizeof(dev_id), 0, sizeof(dev_id));
        (void)reader.get_string("/Command/PrepareFailoverParameters/SourceDevices/SourceDevice/@id", dev_id, i);

        
        device_info.source_id = dev_id;
        devices_info.lst_source_devices.push_back(device_info);
    }

    return RETURN_OK;
}

int PrepareFailove::_outband_process()
{
    int iRet = RETURN_ERR;
    HYPER_STORAGE_STRU stStorageInfo;
    CCmdOperate cmdOperate;

    (void)setStorageInfo(stStorageInfo);

    iRet = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, iRet);

    
    iRet = _get_array_info(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);

    
    string array_id_temp(array_id);
    iRet = check_array_id_validate_out(cmdOperate, array_id_temp);
    CHECK_UNEQ(RETURN_OK, iRet);

    
    (void)_outband_swap_hypermirror(cmdOperate);

    if (g_bFusionStorage && !g_testFusionStorageStretch){
        return RETURN_OK;
    }

    if (g_bstretch){
        
        (void)_outband_swap_hypermetro(cmdOperate);
    }

    if(g_bFusionStorage){
        return RETURN_OK;
    }

    
    iRet = _outband_swap_consist(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);

    if (g_bstretch){
        
        iRet = _outband_swap_consisthm(cmdOperate);
        CHECK_UNEQ(RETURN_OK, iRet);
    }

    return RETURN_OK;
}

int PrepareFailove::_operate_snapshot(const string& lun_id)
{   
    (void)lun_id;

    return RETURN_OK;
}

int PrepareFailove::_create_snapshot(const string& lun_id)
{
    (void)lun_id;

    return RETURN_OK;
}

int PrepareFailove::_recreate_snapshot(const string& snapshot_id)
{
    (void)snapshot_id;

    return RETURN_OK;
}

int PrepareFailove::_active_snapshot(const string& snapshot_id)
{
    (void)snapshot_id;

    return RETURN_OK;
}

int PrepareFailove::_get_snapshot_id(const string& lun_id, string& snapshot_id)
{
    (void)lun_id;
    (void)snapshot_id;

    return RETURN_OK;
}

void PrepareFailove::_write_response(XmlWriter &writer)
{
    (void)writer.set_xml("/Response/PrepareFailoverResults", &group_info);
    (void)writer.set_xml("/Response/PrepareFailoverResults", &devices_info);

    return;
}

void PrepareFailove::_operate_lun_snapshot(const string& cg_id, list<CommonLunInfo>& lst_lun_info)
{
    (void)cg_id;
    (void)lst_lun_info;

    return;   
}

int PrepareFailove::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "FailOver::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
        return ERROR_INVALIDATE_LOCAL_ARRAY_SN;
    }

    if (peer_array_id.empty()){
        return RETURN_OK;
    }

    
    ret = cmdOperate.CMD_showarrayinfo(lstRemoteArrayInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    for (itRemoteArrayInfo = lstRemoteArrayInfo.begin(); 
        itRemoteArrayInfo != lstRemoteArrayInfo.end(); itRemoteArrayInfo++){
        lst_sn_ids.push_back(itRemoteArrayInfo->strSN);
    }

    if (lst_sn_ids.end() == find(lst_sn_ids.begin(), lst_sn_ids.end(), peer_array_id)){
        return ERROR_INVALIDATE_PEER_ARRAY_SN;
    }

    return RETURN_OK;
}

int PrepareFailove::_get_cghm_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess)
{
    int iRet = RETURN_ERR;

    GROUP_INFO_STRU stGroupInfo;
    stGroupInfo.strID = cgid;

    iRet = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    _convert_pair_status(stGroupInfo.uiState, status);
    _convert_relation_type(stGroupInfo.uiIsPrimary, role);
    uiResAccess = stGroupInfo.uiSecResAccess;

    return RETURN_OK;
}

int PrepareFailove::_outband_swap_consist(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cgid;
    string lunid;
    string arrayid;
    string lun_flag;
    list<ConGroupInfo>::iterator iter;
    list<string> lst_token;

    for (iter = group_info.lst_groups_info.begin(); iter != group_info.lst_groups_info.end(); ++iter){
        
        iRet = get_device_info(iter->cg_id, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            continue;
        }

        
        if (cgid.empty()){
            continue;
        }

        
        if ("--" == cgid){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR, "id of hymirror group(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }
        
        iter->is_success = true;
    }
    return RETURN_OK;
}

int PrepareFailove::_outband_swap_consisthm(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cmd;
    string status;
    string role;
    string cgid;
    string lunid;
    string arrayid;
    string lun_flag;
    list<string> lst_token;
    unsigned int uiResAccess = 0;
    list<ConGroupInfo>::iterator iter;

    
    for (iter = group_info.lst_groups_info.begin(); iter != group_info.lst_groups_info.end(); ++iter){
        iRet = get_device_info(iter->cg_id, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        
        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            continue;
        }

        if (cgid.empty() ){
            continue;
        }

        
        if ("--" == cgid){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR, "id of hypermetro group(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        
        iRet = _get_cghm_status_out(cmdOperate, cgid, status, role, uiResAccess);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "_get_cg_status_out faild(%d).", iRet);
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro Group with groupID [%s] status [%s] role [%s]",
            cgid.c_str(),
            status.c_str(),
            role.c_str());
        print("Hypermetro Group with groupID [%s] status [%s] role [%s]",
            cgid.c_str(),
            status.c_str(),
            role.c_str());

        if (HM_RELATION_SLAVE == role){
            iter->is_success = true;
            continue;
        }

        string TimeNowe = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Hypermetro consisgroup id [%s] is being swap promoted from Recovery Point [%s]",
            cgid.c_str(),TimeNowe.c_str());
        print("Hypermetro consisgroup id [%s] is being swap promoted from Recovery Point [%s]",
            cgid.c_str(),TimeNowe.c_str());

        
        iRet = cmdOperate.CMD_swapconsistgrhm(cgid);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "swap hypermetro consistgr ret(%d).", iRet);
            iter->error_info.code = OS_IToString(iRet);
            continue;
        }

        iter->is_success = true;

    }
    return RETURN_OK;
}

int PrepareFailove::_outband_swap_hypermirror(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_OK;
    string strid;
    string hymirror_id;
    
    string arrayid;
    string lun_flag;
    string mirror_id;

    list<string> lst_tmp;
    list<SourceDeviceInfo>::iterator iter;

    for (iter = devices_info.lst_source_devices.begin(); iter != devices_info.lst_source_devices.end(); ++iter){
        iRet = get_device_info(iter->source_id, arrayid, strid, lun_flag, mirror_id);

        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->source_id.c_str());
            continue;
        }
        if (lun_flag.compare(MIRROR_LUN_TAG)){
            continue;
        }
        if (mirror_id.empty()){
            continue;
        }
        
        iter->is_success = true;
    }

    return RETURN_OK;
}

int PrepareFailove::_outband_swap_hypermetro(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cmd;
    string strid;
    string hmpair_id;
    string master_id;
    string slave_key;
    string Slave_id;
    string key;
    string statu;
    string arrayid;
    string lun_flag;

    list<string> lst_tmp;
    list<SourceDeviceInfo>::iterator iter;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;


    for (iter = devices_info.lst_source_devices.begin(); iter != devices_info.lst_source_devices.end(); ++iter){
        iRet = get_device_info(iter->source_id, arrayid, strid, lun_flag, hmpair_id);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->source_id.c_str());
            continue;
        }

        
        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            continue;
        }
        
        if (hmpair_id.empty()){
            continue;
        }

        key = arrayid + "." + strid;

        
        string strTime;
        strTime = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Hypermetro of [%s] with id [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            strTime.c_str());
        print("Hypermetro of [%s] with id [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            strTime.c_str());

        
        COMMLOG(OS_LOG_INFO,"The Hypermetro id of lun [%s] is [%s]",strid.c_str(),hmpair_id.c_str());
        print("The Hypermetro id of lun [%s] is [%s]",strid.c_str(),hmpair_id.c_str());
        
        iRet = cmdOperate.CMD_showhypermetrolun_all(hmpair_id, lstHMPairLFInfo);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            continue;
        }

        
        for (itHMPairLFInfo = lstHMPairLFInfo.begin(); 
            itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
            if (MIRROR_LUN_RELATION_MASTER == itHMPairLFInfo->uiRelationType){
                master_id = itHMPairLFInfo->strArrayWWN + "_" + itHMPairLFInfo->strID + "_" + HYPERMETRO_LUN_TAG + "_" + hmpair_id; 
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHMPairLFInfo->uiRelationType){
                Slave_id = itHMPairLFInfo->strArrayWWN + "_" + itHMPairLFInfo->strID + "_" + HYPERMETRO_LUN_TAG + "_" + hmpair_id; 
                slave_key = itHMPairLFInfo->strArrayWWN + "." + itHMPairLFInfo->strID + "_" + HYPERMETRO_LUN_TAG + "_" + hmpair_id; 
                _convert_hmpair_status(itHMPairLFInfo->uiPairStatus, statu);

            }
        }

        if ((0 == master_id.length())){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            continue;
        }

        
        if (Slave_id == iter->source_id){
            iter->is_success = true;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Hypermetro dvice [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        print("Hypermetro device [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        
        iRet = cmdOperate.CMD_swaphypermetro(hmpair_id);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "CMD_swaphypermetro failed (%d).", iRet);
            continue;
        }

        iter->is_success = true;

    }

    return RETURN_OK;
}
