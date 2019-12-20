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

#include "query_syncstatus.h"

int query_syncstatus(XmlReader &reader)
{
    QuerySyncStatus query_sync_status;
    COMMLOG(OS_LOG_INFO, "%s", "query_syncstatus begin.");
    print("%s", "query_syncstatus begin.");
    if (RETURN_OK != query_sync_status.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "query sync status failed.");

        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "query_syncstatus end.");
    print("%s", "query_syncstatus end.");
    return RETURN_OK;
}

int QuerySyncStatus::_read_command_para(XmlReader &reader)
{
    int i;
    int cg_count = 0;
    char sy_id[LENGTH_COMMON] = {0};
    char cg_id[LENGTH_COMMON] = {0};
    char peer_array[LENGTH_COMMON] = {0};

    (void)reader.get_string("/Command/QuerySyncStatusParameters/ArrayId", array_id, 0);
    (void)reader.get_string("/Command/QuerySyncStatusParameters/PeerArrayId", peer_array, 0);
    (void)reader.get_string("/Command/QuerySyncStatusParameters/ArrayId", input_array_id, 0);

    peer_array_id = peer_array;

    cg_count = reader.get_count("/Command/QuerySyncStatusParameters/ConsistencyGroups/ConsistencyGroup");
    for (i = 0; i < cg_count; i++){
        ConGroupInfo cg_info;
        list<string> lst_token;
        string cgid;
        string lun_id;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        memset_s(sy_id, sizeof(sy_id), 0, sizeof(sy_id));
        (void)reader.get_string("/Command/QuerySyncStatusParameters/ConsistencyGroups/ConsistencyGroup/@id", cg_id, i);
        (void)reader.get_string("/Command/QuerySyncStatusParameters/ConsistencyGroups/ConsistencyGroup/@syncId", sy_id, i);

        COMMLOG(OS_LOG_INFO, "_read_command_para cg_id %s", cg_id);
        OS_StrToken(cg_id, "_", &lst_token);
        if (lst_token.empty()){
            COMMLOG(OS_LOG_ERROR, "cg_id %s of lst_token is empty", cg_id);
            continue;
        }        
        lun_id = lst_token.back();

        cg_info.cg_id = cg_id;
        cg_info.device_sync_info.sync_id = sy_id;
        cg_info.lun_id = lun_id;

        consistency_group_info.lst_groups_info.push_back(cg_info);
    }


    cg_count = reader.get_count("/Command/QuerySyncStatusParameters/SourceDevices/SourceDevice");
    for (i = 0; i < cg_count; i++){
        SourceDeviceInfo sd_info;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        memset_s(sy_id, sizeof(sy_id), 0, sizeof(sy_id));

        (void)reader.get_string("/Command/QuerySyncStatusParameters/SourceDevices/SourceDevice/@id", cg_id, i);
        (void)reader.get_string("/Command/QuerySyncStatusParameters/SourceDevices/SourceDevice/@syncId", sy_id, i);
        sd_info.source_id = cg_id;
        sd_info.device_sync_info.sync_id = sy_id;
        source_device_info.lst_source_devices.push_back(sd_info);
    }

    cg_count = reader.get_count("/Command/QuerySyncStatusParameters/TargetDevices/TargetDevice");
    for (i = 0; i < cg_count; i++){
        TargetDeviceInfo td_info;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        memset_s(sy_id, sizeof(sy_id), 0, sizeof(sy_id));

        (void)reader.get_string("/Command/QuerySyncStatusParameters/TargetDevices/TargetDevice/@key", cg_id, i);
        (void)reader.get_string("/Command/QuerySyncStatusParameters/TargetDevices/TargetDevice/@syncId", sy_id, i);
        td_info.target_key = cg_id;
        td_info.devicesync_info.sync_id = sy_id;
        target_device_info.lst_target_devices.push_back(td_info);
    }

    return RETURN_OK;
}

int QuerySyncStatus::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "QuerySyncStatus::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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

int QuerySyncStatus::_outband_process()
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
    iRet = check_array_id_validate_out(cmdOperate, array_id_temp, peer_array_id);
    if (RETURN_OK != iRet){
        return iRet;
    }
    
    (void)_query_hymirror_info_out(cmdOperate);

    if (g_bFusionStorage && !g_testFusionStorageStretch){
        return RETURN_OK;
    }

    if (g_bstretch){
        (void)_query_hypermetro_info_out(cmdOperate);
    }

    if (g_bFusionStorage){
        return RETURN_OK;
    }

    iRet = _query_consistency_info_out(cmdOperate);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_WARN, "%s", "_query_consistency_info fail");
    }

    if (g_bstretch){
        iRet = _query_consistencyhm_info_out(cmdOperate);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_WARN, "%s", "_query_consistencyhm_info_out fail");
        }
    }

    return RETURN_OK;
}

void QuerySyncStatus::_write_response(XmlWriter &writer)
{
    if (!consistency_group_info.lst_groups_info.empty()){
        (void)writer.set_xml("/Response/QuerySyncStatusResults/ConsistencyGroups", &consistency_group_info);
    }

    if (!source_device_info.lst_source_devices.empty()){
        (void)writer.set_xml("/Response/QuerySyncStatusResults/SourceDevices", &source_device_info);
    }

    if (!target_device_info.lst_target_devices.empty()){
        (void)writer.set_xml("/Response/QuerySyncStatusResults/TargetDevices", &target_device_info);
    }

    return;
}

int QuerySyncStatus::_query_consistency_info_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string cg_id;
    string arrayid;
    string lun_id;
    string lun_flag;
    list<ConGroupInfo>::iterator consistency_id;
    string syncid;

    list<HYMIRROR_INFO_STRU> lstCgHyMirrorInfo;

    consistency_id = consistency_group_info.lst_groups_info.begin();
    while (consistency_id != consistency_group_info.lst_groups_info.end()){
        ret = get_device_info(consistency_id->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            consistency_id->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", consistency_id->cg_id.c_str());
            ++consistency_id;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++consistency_id;
            continue;
        }

        syncid = SYNC_PREFIX + lun_id + "_" + lun_flag + "_" + cg_id;

        if(consistency_id->device_sync_info.sync_id != syncid){
            consistency_id->error_info.code = OS_IToString(ERROR_INVALIDATE_SYNC_ID);
            consistency_id->device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "sync_is(%s) is wrong.", consistency_id->device_sync_info.sync_id.c_str());
            ++consistency_id;
            continue;
        }

        
        if (cg_id.empty()){
            ++consistency_id;
            continue;
        }

        is_Fs_by_lun(cmdOperate, lun_id);
        if (isNFS){
            ret = cmdOperate.CMD_showvstorepairmember(cg_id, lstCgHyMirrorInfo);
        }
        else{
            
            ret = cmdOperate.CMD_showconsistgrmember(cg_id, lstCgHyMirrorInfo);                        
        }

        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_ERROR,"",cg_id.c_str());
            ++consistency_id;
            continue;
        }
        
        ret = _get_consistency_info_out(cmdOperate, lstCgHyMirrorInfo, *consistency_id);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_WARN, "query sync status of consistency info,get percent of consistency fail %s", cg_id.c_str());
            ++consistency_id;
            continue;
        }
        
        string strJindu;
        if(consistency_id->device_sync_info.sync_status == COMPLETE_STATUS){
            strJindu = "100";
        }
        else{
            strJindu = consistency_id->device_sync_info.sync_progress;
        }
        COMMLOG(OS_LOG_INFO,"Replica of ConsistencyGroup[%s] with GroupID[%s] has sync_name[%s] sync_status[%s] sync_progress[%s/100]",
            consistency_id->lun_id.c_str(),
            cg_id.c_str(),
            syncid.c_str(),
            consistency_id->device_sync_info.sync_status.c_str(),
            strJindu.c_str());
        print("Replica of ConsistencyGroup[%s] with GroupID[%s] has sync_name[%s] sync_status[%s] sync_progress[%s/100]",
            consistency_id->lun_id.c_str(),
            cg_id.c_str(),
            syncid.c_str(),
            consistency_id->device_sync_info.sync_status.c_str(),
            strJindu.c_str());
        ++consistency_id;
    }
    
    return RETURN_OK;
}

int QuerySyncStatus::_query_consistencyhm_info_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string cg_id;
    list<ConGroupInfo>::iterator consistency_id;
    string syncid;
    string arrayid;
    string lun_id;
    string lun_flag;

    list<HYPERMETROPAIR_INFO_STRU> lstHMPairInfo;

    consistency_id = consistency_group_info.lst_groups_info.begin();
    while (consistency_id != consistency_group_info.lst_groups_info.end()){
        ret = get_device_info(consistency_id->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            consistency_id->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", consistency_id->cg_id.c_str());
            ++consistency_id;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++consistency_id;
            continue;
        }

        syncid = SYNC_PREFIX + lun_id + "_" + lun_flag + "_" + cg_id;

        
        if(consistency_id->device_sync_info.sync_id != syncid){
            consistency_id->error_info.code = OS_IToString(ERROR_INVALIDATE_SYNC_ID);
            consistency_id->device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "sync_is(%s) is wrong.", consistency_id->device_sync_info.sync_id.c_str());
            ++consistency_id;
            continue;
        }

        
        if (cg_id.empty()){
            ++consistency_id;
            continue;
        }

        lstHMPairInfo.clear();
        ret = cmdOperate.CMD_showHyperMetroPair_all(lstHMPairInfo);
        CHECK_UNEQ(RETURN_OK, ret);

        ret = _get_consistencyhm_info_out(cmdOperate, cg_id, lstHMPairInfo, *consistency_id);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_WARN, "query sync status of Hypermetro consistency info,"
                        "get percent of consistency fail %s", cg_id.c_str());
            ++consistency_id;
            continue;
        }
        
        string strJindu;
        if(consistency_id->device_sync_info.sync_status == COMPLETE_STATUS){
            strJindu = "100";
        }
        else{
            strJindu = consistency_id->device_sync_info.sync_progress;
        }
        COMMLOG(OS_LOG_INFO,"Hypermetro of consistency[%s] with GroupID[%s] "
                    "has sync_name[%s] sync_status[%s] sync_progress[%s/100]",
            consistency_id->lun_id.c_str(),
            cg_id.c_str(),
            syncid.c_str(),
            consistency_id->device_sync_info.sync_status.c_str(),
            strJindu.c_str());
        print("Hypermetro of consistency[%s] with GroupID[%s] has sync_name[%s] "
            "sync_status[%s] sync_progress[%s/100]",
            consistency_id->lun_id.c_str(),
            cg_id.c_str(),
            syncid.c_str(),
            consistency_id->device_sync_info.sync_status.c_str(),
            strJindu.c_str());
        ++consistency_id;
    }
    
    return RETURN_OK;
}

int QuerySyncStatus::_query_hymirror_info_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string percent;
    string syncname;
    string strid;
    string hymirror_id;
    string key;
    string arrayid;
    string lun_flag;
    list<string> hyper_token_id;
    list<SourceDeviceInfo>::iterator iter_hymirror_id = source_device_info.lst_source_devices.begin();

    while (iter_hymirror_id != source_device_info.lst_source_devices.end()){
        syncname = SYNC_PREFIX + iter_hymirror_id->source_id;

        ret = get_device_info(iter_hymirror_id->source_id, arrayid, strid, lun_flag, hymirror_id);
        if (RETURN_OK != ret){
            iter_hymirror_id->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hymirror_id->source_id.c_str());
            ++iter_hymirror_id;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++iter_hymirror_id;
            continue;
        }

        key = arrayid + "."+ strid;

        if (hymirror_id.empty()){
            ++iter_hymirror_id;
            continue;
        }

        
        if (syncname != iter_hymirror_id->device_sync_info.sync_id){
            iter_hymirror_id->error_info.code = OS_IToString(ERROR_INVALIDATE_SYNC_ID);
            COMMLOG(OS_LOG_WARN, "sync_is(%s) is wrong.", iter_hymirror_id->device_sync_info.sync_id.c_str());
            iter_hymirror_id->device_sync_info.sync_id = "";
            ++iter_hymirror_id;
            continue;
        }

        
        is_Fs_by_lun(cmdOperate, strid);     
        ret = _get_hymirror_percent_out(cmdOperate, hymirror_id, percent);
        if (RETURN_OK != ret){
            iter_hymirror_id->error_info.code = OS_IToString(ERROR_INVALIDATE_HYMIRROR_STATUS);
            iter_hymirror_id->device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "query sync status of hymirror info,get percent of hyper_token_id fail %s", hymirror_id.c_str());
            ++iter_hymirror_id;
            continue;
        }

        if ("--" == percent){
            ++iter_hymirror_id;
            continue;
        }

        if (SYNC_COMPLETE == percent){
            iter_hymirror_id->device_sync_info.sync_status = COMPLETE_STATUS;
        }
        else{
            iter_hymirror_id->device_sync_info.sync_progress = percent;
            iter_hymirror_id->device_sync_info.sync_status = INPROGRESS_STATUS;

            iter_hymirror_id->device_sync_info.sync_remainingtimeestimate = REMAIN_SYNC_TIME;
        }
        
        COMMLOG(OS_LOG_INFO,"Replica divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iter_hymirror_id->device_sync_info.sync_status.c_str(),
            percent.c_str());
        print("Replica divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iter_hymirror_id->device_sync_info.sync_status.c_str(),
            percent.c_str());
        ++iter_hymirror_id;
    }

    return RETURN_OK;
}

int QuerySyncStatus::_query_hypermetro_info_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string percent;
    string syncname;
    string strid;
    string hypermetroid;
    string key;
    string arrayid;
    string lun_flag;
    list<string> hyper_token_id;
    list<SourceDeviceInfo>::iterator iter_hypermetroid = source_device_info.lst_source_devices.begin();

    while (iter_hypermetroid != source_device_info.lst_source_devices.end()){
        syncname = SYNC_PREFIX + iter_hypermetroid->source_id;

        ret = get_device_info(iter_hypermetroid->source_id, arrayid, strid, lun_flag, hypermetroid);
        if (RETURN_OK != ret){
            iter_hypermetroid->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hypermetroid->source_id.c_str());
            ++iter_hypermetroid;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++iter_hypermetroid;
            continue;
        }

        key = arrayid + "."+ strid;

        if (hypermetroid.empty()){
            ++iter_hypermetroid;
            continue;
        }

        
        if (syncname != iter_hypermetroid->device_sync_info.sync_id){
            iter_hypermetroid->error_info.code = OS_IToString(ERROR_INVALIDATE_SYNC_ID);
            COMMLOG(OS_LOG_WARN, "sync_is(%s) is wrong.", iter_hypermetroid->device_sync_info.sync_id.c_str());
            iter_hypermetroid->device_sync_info.sync_id = "";
            ++iter_hypermetroid;
            continue;
        }

        

        ret = _get_hmpair_percent_out(cmdOperate, hypermetroid, percent);

        if (RETURN_OK != ret){
            iter_hypermetroid->error_info.code = OS_IToString(ERROR_INVALIDATE_HYMIRROR_STATUS);
            iter_hypermetroid->device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "query sync status of hypermetro info,get percent of hyper_token_id fail %s",
                                hypermetroid.c_str());
            ++iter_hypermetroid;
            continue;
        }

        if ("--" == percent){
            ++iter_hypermetroid;
            continue;
        }

        if (SYNC_COMPLETE == percent){
            iter_hypermetroid->device_sync_info.sync_status = COMPLETE_STATUS;
        }
        else{
            iter_hypermetroid->device_sync_info.sync_progress = percent;
            iter_hypermetroid->device_sync_info.sync_status = INPROGRESS_STATUS;

            iter_hypermetroid->device_sync_info.sync_remainingtimeestimate = REMAIN_SYNC_TIME;
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iter_hypermetroid->device_sync_info.sync_status.c_str(),
            percent.c_str());
        print("Hypermetro divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iter_hypermetroid->device_sync_info.sync_status.c_str(),
            percent.c_str());
        ++iter_hypermetroid;
    }

    list<TargetDeviceInfo>::iterator iterhypermetroid = target_device_info.lst_target_devices.begin();

    while (iterhypermetroid != target_device_info.lst_target_devices.end()){
        syncname = SYNC_PREFIX + iterhypermetroid->target_key;

        ret = get_device_info(iterhypermetroid->target_key, arrayid, strid, lun_flag, hypermetroid);
        if (RETURN_OK != ret){
            iterhypermetroid->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iterhypermetroid->target_key.c_str());
            ++iterhypermetroid;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++iterhypermetroid;
            continue;
        }

        key = arrayid +"."+ strid;

        if (hypermetroid.empty()){
            ++iterhypermetroid;
            continue;
        }

        
        if (syncname != iterhypermetroid->devicesync_info.sync_id){
            iterhypermetroid->err_info.code = OS_IToString(ERROR_INVALIDATE_SYNC_ID);
            COMMLOG(OS_LOG_WARN, "sync_is(%s) is wrong.", iterhypermetroid->devicesync_info.sync_id.c_str());
            iterhypermetroid->devicesync_info.sync_id = "";
            ++iterhypermetroid;
            continue;
        }

        

        ret = _get_hmpair_percent_out(cmdOperate, hypermetroid, percent);

        if (RETURN_OK != ret){
            iterhypermetroid->err_info.code = OS_IToString(ERROR_INVALIDATE_HYMIRROR_STATUS);
            iterhypermetroid->devicesync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "query sync status of hypermetro info,get percent of hyper_token_id fail %s",
                hypermetroid.c_str());
            ++iterhypermetroid;
            continue;
        }

        if ("--" == percent){
            ++iterhypermetroid;
            continue;
        }

        if (SYNC_COMPLETE == percent){
            iterhypermetroid->devicesync_info.sync_status = COMPLETE_STATUS;
        }
        else{
            iterhypermetroid->devicesync_info.sync_progress = percent;
            iterhypermetroid->devicesync_info.sync_status = INPROGRESS_STATUS;

            iterhypermetroid->devicesync_info.sync_remainingtimeestimate = REMAIN_SYNC_TIME;
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iterhypermetroid->devicesync_info.sync_status.c_str(),
            percent.c_str());
        print("Hypermetro divece[%s] with syncname[%s] has sync_status[%s] sync_progress[%s/100]",
            key.c_str(),
            syncname.c_str(),
            iterhypermetroid->devicesync_info.sync_status.c_str(),
            percent.c_str());
        ++iterhypermetroid;
    }

    return RETURN_OK;
}

int QuerySyncStatus::_get_consistency_info_out(CCmdOperate& cmdOperate,
                              list<HYMIRROR_INFO_STRU>& lstMirrorInfo, ConGroupInfo& consistency)
{
    int ret = RETURN_ERR;
    int con_percent = SYNC_COMPLETE_FLAG;
    int percent = 0;

    string query_percent;

    list<string>::iterator hy_id;
    list<string> lst_hy_id;
    list<HYMIRROR_INFO_STRU>::iterator itMirrorInfo;
    
    for (itMirrorInfo = lstMirrorInfo.begin(); itMirrorInfo != lstMirrorInfo.end(); itMirrorInfo++){
        lst_hy_id.push_back(itMirrorInfo->strID);
    }
   
    hy_id = lst_hy_id.begin();
    while (hy_id != lst_hy_id.end()){
        
        ret = _get_hymirror_percent_out(cmdOperate, *hy_id, query_percent);
        if (RETURN_OK != ret){
            consistency.error_info.code = OS_IToString(ERROR_INVALIDATE_HYMIRROR_STATUS);
            consistency.device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "query sync status of consistency info,get percent of ly_id fail %s", hy_id->c_str());

            return ret;
        }

        if ("--" == query_percent){
            ++hy_id;
            continue;
        }

        percent = atoi(query_percent.c_str());

        if (con_percent > percent){
            con_percent = percent;
        }

        ++hy_id;
    }

    if (SYNC_COMPLETE_FLAG == con_percent){
        consistency.device_sync_info.sync_status = COMPLETE_STATUS;
    }
    else{
        consistency.device_sync_info.sync_progress = OS_IToString(con_percent);
        consistency.device_sync_info.sync_status = INPROGRESS_STATUS;
        consistency.device_sync_info.sync_remainingtimeestimate = REMAIN_SYNC_TIME;
    }

    return RETURN_OK;
}

int QuerySyncStatus::_get_consistencyhm_info_out(CCmdOperate& cmdOperate, string cg_id,
                              list<HYPERMETROPAIR_INFO_STRU> &lstHMPairInfo, ConGroupInfo& consistency)
{
    int ret = RETURN_ERR;
    int con_percent = SYNC_COMPLETE_FLAG;
    int percent = 0;
    string query_percent;
    list<string> lst_hy_id;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHMPairInfo;
    
    for (itHMPairInfo = lstHMPairInfo.begin(); itHMPairInfo != lstHMPairInfo.end(); itHMPairInfo++){
        if (itHMPairInfo->strGroupID != cg_id){
            continue;
        }
        string hmpair_id = itHMPairInfo->strID;
        
        ret = _get_hmpair_percent_out(cmdOperate, hmpair_id, query_percent);
        if (RETURN_OK != ret){
            consistency.error_info.code = OS_IToString(ERROR_INVALIDATE_HYMIRROR_STATUS);
            consistency.device_sync_info.sync_id = "";
            COMMLOG(OS_LOG_WARN, "query sync status of consistency info,get percent of hyperemetroPair fail %s", 
                        hmpair_id.c_str());

            return ret;
        }

        if ("--" == query_percent){
            continue;
        }

        percent = atoi(query_percent.c_str());

        if (con_percent > percent){
            con_percent = percent;
        }
    }

    if (SYNC_COMPLETE_FLAG == con_percent){
        consistency.device_sync_info.sync_status = COMPLETE_STATUS;
    }
    else{
        consistency.device_sync_info.sync_progress = OS_IToString(con_percent);
        consistency.device_sync_info.sync_status = INPROGRESS_STATUS;
        consistency.device_sync_info.sync_remainingtimeestimate = REMAIN_SYNC_TIME;
    }

    return RETURN_OK;
}
