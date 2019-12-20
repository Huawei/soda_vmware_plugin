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

#include "sync_once.h"

int sync_once(XmlReader &reader)
{
    SyncOnce so;

    COMMLOG(OS_LOG_INFO, "%s", "SyncOnce begin.");
    print("%s", "SyncOnce begin.");

    if (RETURN_OK != so.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "synchronize array failed.");

        return RETURN_ERR;
    }
    
    COMMLOG(OS_LOG_INFO, "%s", "SyncOnce end.");
    print("%s", "SyncOnce end.");
    return RETURN_OK;
}

SyncOnce::SyncOnce() : SraBasic()
{
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(peer_array_id, sizeof(peer_array_id), 0, sizeof(peer_array_id));
    memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
}

SyncOnce::~SyncOnce()
{

};

int SyncOnce::_read_command_para(XmlReader &reader)
{
    CHECK_FALSE(reader.get_string("/Command/SyncOnceParameters/ArrayId", array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/SyncOnceParameters/ArrayId", input_array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/SyncOnceParameters/PeerArrayId", peer_array_id, 0), RETURN_ERR);

    char sync_obj_id[LENGTH_COMMON] = {0};
    int consistgr_count = 0;
    int mirror_count = 0;
    int index = 0;

    consistgr_count = reader.get_count("/Command/SyncOnceParameters/ConsistencyGroups/ConsistencyGroup");
    for (index = 0; index < consistgr_count; ++index){
        
        list<string> lst_token;
        string cgid;
        string lun_id;
        
        memset_s(sync_obj_id, sizeof(sync_obj_id), 0, sizeof(sync_obj_id));
        CHECK_FALSE(reader.get_string("/Command/SyncOnceParameters/ConsistencyGroups/ConsistencyGroup/@id", sync_obj_id, index), ERROR_INTERNAL_PROCESS_FAIL);

        ConGroupInfo cg;
        
        COMMLOG(OS_LOG_INFO, "_read_command_para sync_obj_id %s", sync_obj_id);
        print("_read_command_para sync_obj_id %s", sync_obj_id);
        OS_StrToken(sync_obj_id, "_", &lst_token);
        if (lst_token.empty()){
            COMMLOG(OS_LOG_ERROR, "OS_StrToken result of sync_obj_id %s is empty", sync_obj_id);
            continue;
        }

        cg.cg_id = sync_obj_id;
        cg.lun_id = lst_token.back();
        
        consisgrs_info.lst_groups_info.push_back(cg);
    }

    mirror_count = reader.get_count("/Command/SyncOnceParameters/SourceDevices/SourceDevice");
    for (index = 0; index < mirror_count; ++index){
        memset_s(sync_obj_id, sizeof(sync_obj_id), 0, sizeof(sync_obj_id));
        CHECK_FALSE(reader.get_string("/Command/SyncOnceParameters/SourceDevices/SourceDevice/@id", sync_obj_id, index), ERROR_INTERNAL_PROCESS_FAIL);

        SourceDeviceInfo sd;
        sd.source_id = string(sync_obj_id);
        devices_info.lst_source_devices.push_back(sd);

    }

    return RETURN_OK;
}

int SyncOnce::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "SyncOnce::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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

int SyncOnce::_outband_process()
{
    int ret = RETURN_ERR;
    CCmdOperate cmdOperate;
    HYPER_STORAGE_STRU stStorageInfo;

    (void)setStorageInfo(stStorageInfo);

    ret = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    
    ret = _get_array_info(cmdOperate);
    CHECK_UNEQ(RETURN_OK, ret);

    string array_id_temp(array_id);
    ret = check_array_id_validate_out(cmdOperate, array_id_temp, string(peer_array_id));
    CHECK_UNEQ(RETURN_OK, ret);  

    
    (void)_sync_mirror_out(cmdOperate);

    if (g_bFusionStorage && !g_testFusionStorageStretch){
        return RETURN_OK;
    }
 
    
    if (g_bstretch){
        (void)_sync_hypermetro_out(cmdOperate);
    }

    if (g_bFusionStorage){
        return RETURN_OK;
    }

    
    ret = _sync_consistgroup_out(cmdOperate);
    CHECK_UNEQ(RETURN_OK, ret);
    
    if (g_bstretch){
        ret = _sync_consistgrouphm_out(cmdOperate);
        CHECK_UNEQ(RETURN_OK, ret);
    }

    return RETURN_OK;
}

void SyncOnce::_write_response(XmlWriter &writer)
{
    _write_consistgroup_result(writer);
    _write_mirror_result(writer);

    return;
}

int SyncOnce::_sync_consistgroup_out(CCmdOperate& cmdoperate)
{
    int ret = RETURN_ERR;
    list<ConGroupInfo>::iterator it;
    int consisgr_status;
    string desc;
    string cmd;
    string cg_id;
    string arrayid;
    string lun_id;
    string lun_flag;
    GROUP_INFO_STRU rstGroupInfo;
    VSTORE_PAIR_INFO_STRU vstorePairInfo;
    unsigned int model;

    it = consisgrs_info.lst_groups_info.begin();

    while (it != consisgrs_info.lst_groups_info.end()){
        ret = get_device_info(it->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it->cg_id.c_str());
            ++it;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++it;
            continue;
        }
        
        if (cg_id.empty()){
            ++it;
            continue;
        }

        if ("--" == cg_id){
            it->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it;
            continue;
        }

        is_Fs_by_lun(cmdoperate, lun_id);
        if (isNFS){
            ret = cmdoperate.CMD_showvstorepair_info(cg_id, vstorePairInfo);
            CHECK_UNEQ(RETURN_OK, ret);
            consisgr_status = (int)vstorePairInfo.uiState;
            model = MODE_ASYNCHRONOUS;
        }
        else{
            
            rstGroupInfo.strID = cg_id;
            ret = cmdoperate.CMD_showconsistgrinfo(rstGroupInfo);
            CHECK_UNEQ(RETURN_OK, ret);

            consisgr_status = (int)rstGroupInfo.uiState;
            model = rstGroupInfo.uiModel;
        }
        
        string status;
        switch(consisgr_status){
        case MIRROR_GROUP_STATUS_NORAML:
            status = MIRROR_GROUP_STATUS_STR_NORAML;
            break;
        case MIRROR_GROUP_STATUS_SYNCHRONIZING:
            status = MIRROR_GROUP_STATUS_STR_SYNCHRONIZING;
            break;
        case MIRROR_GROUP_STATUS_INTERRUPTED:
            status = MIRROR_GROUP_STATUS_STR_INTERRUPTED;
            break;
        case MIRROR_GROUP_STATUS_TOBERECOV:
            status = MIRROR_GROUP_STATUS_STR_TOBERECOV;
            break;
        case MIRROR_GROUP_STATUS_SPLITED:
            status = MIRROR_GROUP_STATUS_STR_SPLITED;
            break;
        case MIRROR_GROUP_STATUS_INVALID:
            status = MIRROR_GROUP_STATUS_STR_INVALID;
            break;
        case MIRROR_GROUP_STATUS_FAULT:
            status = MIRROR_GROUP_STATUS_STR_FAULT;
            break;
        case MIRROR_GROUP_STATUS_NOT_EXIST:
            status = "--";
            break;
        default:
            break;
        }
        COMMLOG(OS_LOG_INFO,"Replica of ConsistencyGroup[%s] with GroupID[%s] has status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            status.c_str());
        print("Replica of ConsistencyGroup[%s] with GroupID[%s] has status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            status.c_str());

        if (MIRROR_GROUP_STATUS_SYNCHRONIZING == consisgr_status){
            WarnInfo warninfo;
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + MIRROR_LUN_CONSISTENT_TAG + "_" + cg_id;
            warninfo.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
            it->warnings.lst_warn.push_back(warninfo); 
            COMMLOG(OS_LOG_WARN, "Consisttentgroup(%s) is Synchronizing", cg_id.c_str());
            ++it;
            continue;
        }

        
        if (MODE_SYNCHRONOUS == model && MIRROR_GROUP_STATUS_NORAML == consisgr_status){
            COMMLOG(OS_LOG_INFO,"consist group(%s) is sync and normal.", cg_id.c_str());
            print("consist group(%s) is sync and normal.", cg_id.c_str());
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + MIRROR_LUN_CONSISTENT_TAG + "_" + cg_id;
            it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
            it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
            it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
            ++it;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Replica of ConsistencyGroup[%s] with GroupID[%s] begin sync",
            it->lun_id.c_str(),
            cg_id.c_str());
        print("Replica of ConsistencyGroup[%s] with GroupID[%s] begin sync",
            it->lun_id.c_str(),
            cg_id.c_str());

        if(isNFS){
            ret = cmdoperate.CMD_syncvstorepair(cg_id);
        }
        else{
            ret = cmdoperate.CMD_syncconsistgr(cg_id);
        }
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR,"Replica of ConsistencyGroup[%s] with GroupID[%s] sync err [%s]",
                it->lun_id.c_str(),
                cg_id.c_str(),
                it->error_info.code.c_str());
            ++it;
            continue;
        }
        else{
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + MIRROR_LUN_CONSISTENT_TAG + "_" + cg_id;
            it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
            it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
            it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
        }
        
        COMMLOG(OS_LOG_INFO,"Replica of ConsistencyGroup[%s] with GroupID[%s] sync finish,syncID[%s] sync status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        print("Replica of ConsistencyGroup[%s] with GroupID[%s] sync finish,syncID[%s] sync status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        ++it;
    }
    return RETURN_OK;
}

int SyncOnce::_sync_consistgrouphm_out(CCmdOperate& cmdoperate)
{
    int ret = RETURN_ERR;
    list<ConGroupInfo>::iterator it;
    int consisgr_status;
    string desc;
    string cmd;
    string cg_id;
    string arrayid;
    string lun_id;
    string lun_flag;

    it = consisgrs_info.lst_groups_info.begin();

    while (it != consisgrs_info.lst_groups_info.end()){
        ret = get_device_info(it->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it->cg_id.c_str());
            ++it;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++it;
            continue;
        }
        
        if (cg_id.empty()){
            ++it;
            continue;
        }

        if ("--" == cg_id){
            it->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it;
            continue;
        }

        
        GROUP_INFO_STRU rstGroupInfo;
        rstGroupInfo.strID = cg_id;
        ret = cmdoperate.CMD_showconsistgrhminfo(rstGroupInfo);
        CHECK_UNEQ(RETURN_OK, ret);

        consisgr_status = (int)rstGroupInfo.uiState;
        
        string status;
        switch(consisgr_status){
        case MIRROR_GROUP_STATUS_NORAML:
            status = MIRROR_GROUP_STATUS_STR_NORAML;
            break;
        case MIRROR_GROUP_STATUS_SYNCHRONIZING:
            status = MIRROR_GROUP_STATUS_STR_SYNCHRONIZING;
            break;
        case MIRROR_GROUP_STATUS_INTERRUPTED:
            status = MIRROR_GROUP_STATUS_STR_INTERRUPTED;
            break;
        case MIRROR_GROUP_STATUS_TOBERECOV:
            status = MIRROR_GROUP_STATUS_STR_TOBERECOV;
            break;
        case MIRROR_GROUP_STATUS_SPLITED:
            status = MIRROR_GROUP_STATUS_STR_SPLITED;
            break;
        case MIRROR_GROUP_STATUS_INVALID:
            status = MIRROR_GROUP_STATUS_STR_INVALID;
            break;
        case MIRROR_GROUP_STATUS_FAULT:
            status = MIRROR_GROUP_STATUS_STR_FAULT;
            break;
        case MIRROR_GROUP_STATUS_NOT_EXIST:
            status = "--";
            break;
        default:
            break;
        }
        COMMLOG(OS_LOG_INFO,"Hypermetro of ConsistencyGroup[%s] with GroupID[%s] has status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            status.c_str());
        print("Hypermetro of ConsistencyGroup[%s] with GroupID[%s] has status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            status.c_str());

        if (MIRROR_GROUP_STATUS_SYNCHRONIZING == consisgr_status){
            WarnInfo warninfo;
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + HYPERMETRO_LUN_CONSISTENT_TAG + "_" + cg_id;
            warninfo.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
            it->warnings.lst_warn.push_back(warninfo); 
            COMMLOG(OS_LOG_WARN, "hmdomain(%s) is Synchronizing", cg_id.c_str());
            ++it;
            continue;
        }

        
        if (MIRROR_GROUP_STATUS_NORAML == consisgr_status){
            COMMLOG(OS_LOG_INFO,"Hypermetro group(%s) is sync and normal.", cg_id.c_str());
            print("Hypermetro group(%s) is sync and normal.", cg_id.c_str());
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + HYPERMETRO_LUN_CONSISTENT_TAG + "_" + cg_id;
            it->device_sync_info.sync_status = COMPLETE_STATUS;
            it->device_sync_info.sync_progress = SYNC_COMPLETE_RATE;
            it->device_sync_info.sync_remainingtimeestimate = "0";
            ++it;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro of ConsistencyGroup[%s] with GroupID[%s] begin sync",
            it->lun_id.c_str(),
            cg_id.c_str());
        print("Hypermetro of ConsistencyGroup[%s] with GroupID[%s] begin sync",
            it->lun_id.c_str(),
            cg_id.c_str());
        ret = cmdoperate.CMD_syncconsistgrhm(cg_id);
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR,"Hypermetro of ConsistencyGroup[%s] with GroupID[%s] sync err [%s]",
                it->lun_id.c_str(),
                cg_id.c_str(),
                it->error_info.code.c_str());
            ++it;
            continue;
        }
        else{
            it->device_sync_info.sync_id = SYNC_PREFIX + lun_id + "_" + HYPERMETRO_LUN_CONSISTENT_TAG + "_" + cg_id;
            it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
            it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
            it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
        }

        
        COMMLOG(OS_LOG_INFO,"Hypermetro of ConsistencyGroup[%s] with GroupID[%s] sync finish,syncID[%s] sync status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        print("Hypermetro of ConsistencyGroup[%s] with GroupID[%s] sync finish,syncID[%s] sync status[%s]",
            it->lun_id.c_str(),
            cg_id.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        ++it;
    }
    return RETURN_OK;
}

int SyncOnce::_sync_mirror_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string slave_array_sn;
    string slave_id;
    string pair_status;
    list<SourceDeviceInfo>::iterator it;
    string desc;
    string strid;
    string hymirror_id;
    string arrayid;
    string lun_flag;
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLF;
    string key;

    it = devices_info.lst_source_devices.begin();

    while (it != devices_info.lst_source_devices.end()){
        ret = get_device_info(it->source_id, arrayid, strid, lun_flag, hymirror_id);
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it->source_id.c_str());
            ++it;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++it;
            continue;
        }

        key = arrayid + "." +strid;

        if (hymirror_id.empty()){
            ++it;
            continue;
        }

        
        is_Fs_by_lun(cmdOperate, strid);
        if (isNFS){
            ret = cmdOperate.CMD_showhymirrorbyfs(strid,lstHyMirrorLFInfo);
        } 
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(hymirror_id,lstHyMirrorLFInfo);
        }
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            ++it;
            continue;
        }

        for (itHyMirrorLF = lstHyMirrorLFInfo.begin(); itHyMirrorLF != lstHyMirrorLFInfo.end(); ++itHyMirrorLF){
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLF->uiRelationType){
                slave_array_sn = itHyMirrorLF->strArrayWWN;
                slave_id   = itHyMirrorLF->strID;
                _convert_pair_status(itHyMirrorLF->uiPairStatus,pair_status);
            }
            if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLF->uiRelationType && isNFS){
                hymirror_id = itHyMirrorLF->strMirrorID;
            }
        }


        if (slave_array_sn.empty() || slave_id.empty()){
            it->error_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            ++it;
            continue;
        }

        
        string slave_key;
        slave_key = slave_array_sn + "." + slave_id;
        COMMLOG(OS_LOG_INFO,"Replica of devices[%s] with target devices[%s] has status[%s]",
            key.c_str(),
            slave_key.c_str(),
            pair_status.c_str());
        print("Replica of devices[%s] with target devices[%s] has status[%s]",
            key.c_str(),
            slave_key.c_str(),
            pair_status.c_str());

        
        if (!pair_status.empty() && HM_SYNCING == pair_status){
            WarnInfo warninfo;
            it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
            warninfo.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
            it->warnings.lst_warn.push_back(warninfo);
            COMMLOG(OS_LOG_WARN, "Hymirror(%s) is Synchronizing", hymirror_id.c_str());
            ++it;
            continue;
        }

        HYMIRROR_INFO_STRU hymirrorStu;
        hymirrorStu.strID = hymirror_id;
        
        
        COMMLOG(OS_LOG_INFO,"Replica of devices[%s] with target devices[%s]  being sync",
            key.c_str(),
            slave_key.c_str());
        print("Replica of devices[%s] with target devices[%s]  being sync",
            key.c_str(),
            slave_key.c_str());
        ret = cmdOperate.CMD_showhymirrorinfo(hymirrorStu);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_WARN, "hymirror (%s) is failed", strid.c_str());
            ++it;
            continue;
        }

        
        if (MODE_SYNCHRONOUS == hymirrorStu.uiModel && MIRROR_STATUS_NORMAL == hymirrorStu.uiState){
            it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
            it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
            it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
            it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
        }
        else{
            COMMLOG(OS_LOG_INFO,"hymirror (%s) is begin sysnc.",strid.c_str());
            print("hymirror (%s) is begin sysnc.",strid.c_str());
            ret = cmdOperate.CMD_synchymirror(hymirror_id,slave_array_sn,slave_id);
            if (RETURN_OK != ret){
                it->error_info.code = OS_IToString(ret);
            }
            else{
                it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
                it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
                it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
                it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
            }
        }
        
        COMMLOG(OS_LOG_INFO,"Replica of devices[%s] with target devices[%s] sync finish,sysncID[%s] sysnc status[%s]",
            key.c_str(),
            slave_key.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        print("Replica of devices[%s] with target devices[%s] sync finish,sysncID[%s] sysnc status[%s]",
            key.c_str(),
            slave_key.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        ++it;
    }

    return RETURN_OK;

}

int SyncOnce::_sync_hypermetro_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string slave_array_sn;
    string slave_id;
    string pair_status;
    list<SourceDeviceInfo>::iterator it;
    string desc;
    string strid;
    string hmpair_id;
    string arrayid;
    string lun_flag;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLF;
    string key;

    it = devices_info.lst_source_devices.begin();

    while (it != devices_info.lst_source_devices.end()){
        ret = get_device_info(it->source_id, arrayid, strid, lun_flag, hmpair_id);
        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it->source_id.c_str());
            ++it;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++it;
            continue;
        }

        key = arrayid + "." +strid;

        if (hmpair_id.empty()){
            ++it;
            continue;
        }

        ret = cmdOperate.CMD_showhypermetrolun_all(hmpair_id,lstHMPairLFInfo);

        if (RETURN_OK != ret){
            it->error_info.code = OS_IToString(ret);
            ++it;
            continue;
        }

        for (itHMPairLF = lstHMPairLFInfo.begin(); itHMPairLF != lstHMPairLFInfo.end(); ++itHMPairLF){
            if (MIRROR_LUN_RELATION_SLAVE == itHMPairLF->uiRelationType){
                slave_array_sn = itHMPairLF->strArrayWWN;
                slave_id   = itHMPairLF->strID;
                _convert_hmpair_status(itHMPairLF->uiPairStatus,pair_status);
            }
            if (MIRROR_LUN_RELATION_MASTER == itHMPairLF->uiRelationType && isNFS){
                hmpair_id = itHMPairLF->strHMpairID;
            }
        }


        if (slave_array_sn.empty() || slave_id.empty()){
            it->error_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            ++it;
            continue;
        }

        
        string slave_key;
        slave_key = slave_array_sn + "." + slave_id;
        COMMLOG(OS_LOG_INFO,"Hypermetro of devices[%s] with target devices[%s] has status[%s]",
            key.c_str(),
            slave_key.c_str(),
            pair_status.c_str());
        print("Hypermetro of devices[%s] with target devices[%s] has status[%s]",
            key.c_str(),
            slave_key.c_str(),
            pair_status.c_str());

        
        if (!pair_status.empty() && HM_SYNCING == pair_status){
            WarnInfo warninfo;
            it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
            warninfo.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
            it->warnings.lst_warn.push_back(warninfo);
            COMMLOG(OS_LOG_WARN, "Hypermetro(%s) is Synchronizing", hmpair_id.c_str());
            ++it;
            continue;
        }

        HYPERMETROPAIR_INFO_STRU hypermetroStu;
        hypermetroStu.strID = hmpair_id;
        
        
        COMMLOG(OS_LOG_INFO,"HyperMetro of devices[%s] with target devices[%s]  being sync",
            key.c_str(),
            slave_key.c_str());
        print("HyperMetro of devices[%s] with target devices[%s]  being sync",
            key.c_str(),
            slave_key.c_str());
        ret = cmdOperate.CMD_showhypermetroinfo(hypermetroStu);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_WARN, "HyperMetro (%s) is failed", strid.c_str());
            ++it;
            continue;
        }

        
        if (HM_STATUS_NORMAL == hypermetroStu.uiState || HM_STATUS_SYNCHRONIZING == hypermetroStu.uiState){
            it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
            it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
            it->device_sync_info.sync_progress = hypermetroStu.iProgress;
            it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
        }
        else{
            COMMLOG(OS_LOG_INFO,"hypermetro (%s) is begin sysnc.",strid.c_str());
            print("hypermetro (%s) is begin sysnc.",strid.c_str());
            ret = cmdOperate.CMD_synchmpair(hmpair_id);
            if (RETURN_OK != ret){
                it->error_info.code = OS_IToString(ret);
            }
            else{
                it->device_sync_info.sync_id = SYNC_PREFIX + it->source_id;
                it->device_sync_info.sync_status = SYNC_PROCESSING_STATUS;
                it->device_sync_info.sync_progress = SYNC_PROCESSING_RATE;
                it->device_sync_info.sync_remainingtimeestimate = SYNC_REMAIN_TIME;
            }
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro of devices[%s] with target devices[%s] sync finish,sysncID[%s] sysnc status[%s]",
            key.c_str(),
            slave_key.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        print("Hypermetro of devices[%s] with target devices[%s] sync finish,sysncID[%s] sysnc status[%s]",
            key.c_str(),
            slave_key.c_str(),
            it->device_sync_info.sync_id.c_str(),
            it->device_sync_info.sync_status.c_str());
        ++it;
    }

    return RETURN_OK;

}

void SyncOnce::_write_consistgroup_result(XmlWriter &writer)
{
    (void)writer.set_xml("/Response/SyncOnceResults", &consisgrs_info);

    return;
}

void SyncOnce::_write_mirror_result(XmlWriter &writer)
{
    (void)writer.set_xml("/Response/SyncOnceResults", &devices_info);

    return;

}

