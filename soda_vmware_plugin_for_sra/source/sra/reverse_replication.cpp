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

#include "reverse_replication.h"

int reverse_replications(XmlReader &reader)
{
    ReverseReplication rev_replication;

    COMMLOG(OS_LOG_INFO, "%s", "ReverseReplication begin.");
    print("%s", "ReverseReplication begin.");

    if (RETURN_OK != rev_replication.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "reverse replication failed.");

        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "ReverseReplication end.");
    print("%s", "ReverseReplication end.");
    return RETURN_OK;
}

int ReverseReplication::_read_command_para(XmlReader &reader)
{
    int i, j, k;
    int cg_count = 0;
    int device_count = 0;
    int accessgroup_count = 0;
    int accessinitiator_count = 0;
    char peer_array[LENGTH_COMMON] = {0};

    CHECK_FALSE(reader.get_string("/Command/ReverseReplicationParameters/ArrayId", array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/ReverseReplicationParameters/ArrayId", input_array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/ReverseReplicationParameters/PeerArrayId", peer_array, 0), RETURN_ERR);

    peer_array_id = peer_array;
    m_array_id = input_array_id;
    cg_count = reader.get_count("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup");
    for (i = 0; i < cg_count; i++){
        char cg_id[LENGTH_COMMON];
        ConGroupInfo cg_info;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        (void)reader.get_string("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/@id", cg_id, i);
        
        
        cg_info.cg_id = cg_id;
        consistency_group_info.lst_groups_info.push_back(cg_info);

        
        device_count = reader.get_count("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/Devices/Device", i, 2);
        for(j = 0; j < device_count; j++){
            char device_id[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/Devices/Device/@id", device_id, j);
            
            accessgroup_count = reader.get_count("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/Devices/Device/AccessGroups/AccessGroup", j, 2);
            
            string cg_accessgroups = "";
            for (k = 0; k < accessgroup_count; k++){
                char group_id[LENGTH_COMMON] = {0};
                (void)reader.get_string("/Command/ReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/Devices/Device/AccessGroups/AccessGroup/@id", group_id, k, j, 2);
                
                cg_accessgroups = cg_accessgroups + group_id + DAR_HOST_SEPERATOR;
            }
            COMMLOG(OS_LOG_INFO, "AccessGroups of Device[%s] is [%s].", device_id, cg_accessgroups.c_str());
            accessGroupDict[device_id] = cg_accessgroups;
        }
    }

    device_count = reader.get_count("/Command/ReverseReplicationParameters/Devices/Device");
    for (i = 0; i < device_count; i++){
        char dev_id[LENGTH_COMMON];
        DeviceInfo dev_info;

        memset_s(dev_id, sizeof(dev_id), 0, sizeof(dev_id));
        (void)reader.get_string("/Command/ReverseReplicationParameters/Devices/Device/@id", dev_id, i);
        
        
        dev_info.id = dev_id;
        

        
        accessgroup_count = reader.get_count("/Command/ReverseReplicationParameters/Devices/Device/AccessGroups/AccessGroup", i, 2);
        for (j = 0; j < accessgroup_count; j++){
            char group_id[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/ReverseReplicationParameters/Devices/Device/AccessGroups/AccessGroup/@id", group_id, j, i, 2);
            dev_info.cg_accessgroups = dev_info.cg_accessgroups + group_id + ":";
        }
        devices_info.lst_device_info.push_back(dev_info);
    }

    
    accessgroup_count = reader.get_count("/Command/ReverseReplicationParameters/AccessGroups/AccessGroup");
    for(j = 0; j < accessgroup_count; j++){
        char group_id[LENGTH_COMMON] = {0};
        (void)reader.get_string("/Command/ReverseReplicationParameters/AccessGroups/AccessGroup/@id", group_id, j);
        accessinitiator_count = reader.get_count("/Command/ReverseReplicationParameters/AccessGroups/AccessGroup/Initiator", j, 1);
        for (i = 0; i < accessinitiator_count; i++){
            char initiator_id[LENGTH_COMMON] = {0};
            char initiator_type[LENGTH_COMMON] = {0};
            DAR_HOST_INFO tg_dar_host_info;
            (void)reader.get_string("/Command/ReverseReplicationParameters/AccessGroups/AccessGroup/Initiator/@id", initiator_id, i, j, 1);
            (void)reader.get_string("/Command/ReverseReplicationParameters/AccessGroups/AccessGroup/Initiator/@type", initiator_type, i, j, 1);
            tg_dar_host_info.strID = initiator_id;
            tg_dar_host_info.strType = initiator_type;
            tg_dar_host_info.strGroup = group_id;
            lstDARHosts.push_back(tg_dar_host_info);
        }
    }

    return RETURN_OK;
}

int ReverseReplication::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "ReverseReplication::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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
        COMMLOG(OS_LOG_ERROR, "find remote[%s] array fail", peer_array_id.c_str());
        return ERROR_INVALIDATE_PEER_ARRAY_SN;
    }

    return RETURN_OK;
}

void ReverseReplication::_write_response(XmlWriter &writer)
{
    (void)writer.set_xml("/Response/ReverseReplicationResults", &consistency_group_info);
    (void)writer.set_xml("/Response/ReverseReplicationResults", &devices_info);

    return;
}

int ReverseReplication::_outband_process()
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


    
    _outband_swap_hypermirror(cmdOperate);

    if (g_bFusionStorage && !g_testFusionStorageStretch){
        return RETURN_OK;
    }

    if (g_bstretch){
        _outband_swap_hypermetropair(cmdOperate);
    }

    if(g_bFusionStorage){
        return RETURN_OK;
    }

    
    _outband_swap_consist(cmdOperate);
    if (g_bstretch){
        _outband_swap_consisthm(cmdOperate);
    }

    
    find_luns_from_consis(cmdOperate);
    find_luns_from_devices(cmdOperate);
    deal_3dc_lun(cmdOperate);

    return RETURN_OK;
}


void ReverseReplication::deal_3dc_lun(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    COMMLOG(OS_LOG_INFO,"exec 3DC......");
    set<string> sync_consiss,sync_pairs;
    for(vector<ThreeDCLun>::iterator it_lun = three_dc_luns.begin();it_lun != three_dc_luns.end();it_lun++){
        ThreeDCLun::ABInfo& ab_info = it_lun->getABInfo();
        if(ab_info.relation == ThreeDCLun::CONSIS_HYPER){
            if((ab_info.status == RUNNING_STATUS_PAUSE || ab_info.status == RUNNING_STATUS_NORMAL)){
                if(sync_consiss.find(ab_info.id) != sync_consiss.end()){
                    continue;
                }
                ret = cmdOperate.CMD_syncconsistgrhm(ab_info.id);
                if(ret != RETURN_OK){
                    COMMLOG(OS_LOG_WARN,"sync consis hyper fail,id[%s]",ab_info.id.c_str());
                }
                sync_consiss.insert(ab_info.id);
            }
            else{
                COMMLOG(OS_LOG_WARN,"consis hyper status not [normal or pause],id[%s],status[%d]",ab_info.id.c_str(),ab_info.status);
            }
        }
        else if(ab_info.relation == ThreeDCLun::PAIR_HYPER){
            if((ab_info.status == RUNNING_STATUS_PAUSE || ab_info.status == RUNNING_STATUS_NORMAL)){
                if(sync_pairs.find(ab_info.id) != sync_pairs.end()){
                    continue;
                }
                ret = cmdOperate.CMD_synchmpair(ab_info.id);
                if(ret != RETURN_OK){
                    COMMLOG(OS_LOG_WARN,"sync pair hyper fail,id[%s], status[%d]",ab_info.id.c_str(),ab_info.status);
                }
                sync_pairs.insert(ab_info.id);
            }
            else{
                COMMLOG(OS_LOG_WARN,"hyper pair status not [normal or pause],id[%s]",ab_info.id.c_str());
            }
        }
        else if(ab_info.relation == ThreeDCLun::CONSIS_COPY || ab_info.relation == ThreeDCLun::PAIR_COPY){
            COMMLOG(OS_LOG_WARN,"can not support the model[%d]",ab_info.relation);
        }
        else{
            COMMLOG(OS_LOG_WARN,"unknown 3dc model[%d]",ab_info.relation);
        }
    }
}

void ReverseReplication::_outband_swap_consist(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_token;
    string lun_id;
    string cg_id;
    string cg_name;
    string role;
    string arrayid;
    string lun_flag;
    list<ConGroupInfo>::iterator iter;

    
    iter = consistency_group_info.lst_groups_info.begin();
    while (iter != consistency_group_info.lst_groups_info.end()){
        GROUP_INFO_STRU stGroupInfo;

        if (iter->error_info.hasError()){
            ++iter;
            continue;
        }

        ret = get_device_info(iter->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->cg_id.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++iter;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Commence: Replica of ConsistencyGroup [%s] on array [%s] for reverse replication request",
            iter->cg_id.c_str(), array_id);
        print("Commence: Replica of ConsistencyGroup [%s] on array [%s] for reverse replication request",
            iter->cg_id.c_str(), array_id);

        if (cg_id.empty()){
            ++iter;
            continue;
        }
        
        if ("--" == cg_id){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter;
            continue;
        }

        is_Fs_by_lun(cmdOperate, lun_id);
        if(isNFS){
            VSTORE_PAIR_INFO_STRU vstorePairInfo;
            ret = cmdOperate.CMD_showvstorepair_info(cg_id, vstorePairInfo);
            cg_name = vstorePairInfo.strID;
            role = vstorePairInfo.isPrimary ? MIRROR_RELATION_MASTER : MIRROR_RELATION_SLAVE;
        }
        else{
            
            stGroupInfo.strID = cg_id;
            ret = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
            
            cg_name = stGroupInfo.strName;
            _convert_relation_type(stGroupInfo.uiIsPrimary, role);
        }

        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);           
            ++iter;
            continue;
        }        
        
        if ( 0 == cg_name.length()){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"the lun [%s] of TargetGroup [%s] with role [%s]",lun_id.c_str(),iter->cg_id.c_str(),role.c_str());
        print("the lun [%s] of TargetGroup [%s] with role [%s]",lun_id.c_str(),iter->cg_id.c_str(),role.c_str());
        
        if (MIRROR_RELATION_MASTER == role){
            iter->war_info.code = OS_IToString(500);
            iter->is_success = true;
            ++iter;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"the ConsistencyGroup[%s] with TargetGroup [%s] promot begin",iter->cg_id.c_str(),cg_id.c_str());
        print("the ConsistencyGroup[%s] with GroupID [%s] is being promoted",iter->cg_id.c_str(),cg_id.c_str());
        
        if(isNFS){
            ret = cmdOperate.CMD_swapvstorepair(cg_id);
            if (RETURN_OK != ret){
                COMMLOG(OS_LOG_ERROR, "swap vstorepair ret(%d).", ret);
                iter->error_info.code = OS_IToString(ret);
                ++iter;
                continue;
            }

            ret = cmdOperate.CMD_changeVstorePairSlaveLunRw(cg_id, RESOURCE_ACCESS_READ_ONLY);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "change vstorepair Slave RO ret(%d).", ret);
                iter->error_info.code = OS_IToString(ret);
                ++iter;
                continue;
            }
        }
        else{
            
            ret = cmdOperate.CMD_swapconsistgr(cg_id);
            if (RETURN_OK != ret){
                COMMLOG(OS_LOG_ERROR, "swap consistgr ret(%d).", ret);
                iter->error_info.code = OS_IToString(ret);
                ++iter;
                continue;
            }

            OS_Sleep(500);

            
            ret = cmdOperate.CMD_changeCGSlaveLunRw(cg_id, RESOURCE_ACCESS_READ_ONLY);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "change consistgr SlaveLun RO ret(%d).", ret);
                iter->error_info.code = OS_IToString(ret);
                ++iter;
                continue;
            }
        }
        
        list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
        list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;
        string slave_key;
        string perrArry;
        if(isNFS){
            ret = cmdOperate.CMD_showhymirrorfs_all(strHyID, lstHyMirrorLFInfo);
        }
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(strHyID, lstHyMirrorLFInfo);
        }
        
        if(ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR,"show hymirror fs/lun err ret(%d)",ret);
        }

        for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); 
            itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
            if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLFInfo->uiRelationType && !isNFS){
                slave_key = "G_" + itHyMirrorLFInfo->strID;
                perrArry = itHyMirrorLFInfo->strArrayWWN;
            }
        }
        COMMLOG(OS_LOG_INFO,"After reverse the ConsistencyGroup [%s] has attributes: [r/w]", iter->cg_id.c_str());
        print("After reverse the ConsistencyGroup [%s] on array [%s] has attributes: [r/w]",
            iter->cg_id.c_str(),array_id);
        COMMLOG(OS_LOG_INFO,"After reverse the TargetGroup [%s] has attributes: [r/o]",slave_key.c_str());
        print("After reverse the TargetGroup [%s] on array [%s] has attributes: [r/o]",
            slave_key.c_str(),peer_array_id.c_str());
        
        COMMLOG(OS_LOG_INFO, "Complete: Promotion is now complete for group lun[%s] for reverse replication request", lun_id.c_str());
        print("Complete: Promotion is now complete for ConsistencyGroup [%s] for reverse replication request", iter->cg_id.c_str());
        iter->is_success = true;
        ++iter;
    }

    return;
}

void ReverseReplication::_outband_swap_consisthm(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_token;
    string lun_id;
    string cg_id;
    string cg_name;
    string role;
    string arrayid;
    string lun_flag;
    list<ConGroupInfo>::iterator iter;

    
    iter = consistency_group_info.lst_groups_info.begin();
    while (iter != consistency_group_info.lst_groups_info.end()){
        if (iter->error_info.hasError()){
            ++iter;
            continue;
        }

        ret = get_device_info(iter->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->cg_id.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++iter;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Commence: hypermetro of hmdomain [%s] on array [%s] for reverse hypermetro request",
            iter->cg_id.c_str(), array_id);
        print("Commence: hypermetro of hmdomain [%s] on array [%s] for reverse hypermetro request",
            iter->cg_id.c_str(), array_id);

        if (cg_id.empty()){
            ++iter;
            continue;
        }

        
        if ("--" == cg_id){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter;
            continue;
        }

        
        GROUP_INFO_STRU stGroupInfo;
        stGroupInfo.strID = cg_id;
        ret = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);           
            ++iter;
            continue;
        }

        
        list<HYPERMETROPAIR_INFO_STRU> lstHMPairInfo;
        list<HYPERMETROPAIR_INFO_STRU>::iterator itHMPairInfo;
        ret = cmdOperate.CMD_showHyperMetroPair_all(lstHMPairInfo);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "CMD_showHyperMetroPair_all failed.");
            continue;
        }
        for (itHMPairInfo = lstHMPairInfo.begin(); itHMPairInfo != lstHMPairInfo.end(); itHMPairInfo++){
            if (cg_id != itHMPairInfo->strGroupID){
                continue;
            }

            arrayid = array_id;
            lun_id = itHMPairInfo->strLocalLUNID;
            TargetDeviceInfo tmp_tg;
            tmp_tg.target_id = arrayid + "_" + lun_id + "_" + HYPERMETRO_LUN_TAG + "_" + itHMPairInfo->strID;
            tmp_tg.target_key = tmp_tg.target_id;
            if (accessGroupDict.find(tmp_tg.target_id) != accessGroupDict.end()){
                tmp_tg.cg_accessgroups = accessGroupDict[tmp_tg.target_id];
            }
            else{
                iter->error_info.code = OS_IToString(RETURN_ERR);
                COMMLOG(OS_LOG_ERROR, "Cannot find access group info of device(%s)", tmp_tg.target_id.c_str());
                return;
            }

            list<CMDHOSTINFO_STRU> rlstHostInfo;
            ret = _add_map(cmdOperate, tmp_tg, lstDARHosts, rlstHostInfo, lun_id, OBJ_LUN);
            if (RETURN_OK != ret){
                iter->error_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "Map remote lun [%s] to hosts failed.", lun_id.c_str());
                return;
            }
        }

        
        cg_name = stGroupInfo.strName;
        _convert_relation_type(stGroupInfo.uiIsPrimary, role);
        if ( 0 == cg_name.length()){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"the lun [%s] of TargetGroup [%s] with role [%s]",lun_id.c_str(),iter->cg_id.c_str(),role.c_str());
        print("the lun [%s] of TargetGroup [%s] with role [%s]",lun_id.c_str(),iter->cg_id.c_str(),role.c_str());
        
        if (MIRROR_RELATION_MASTER == role){
            iter->is_success = true;
            ++iter;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO,"the Hypermetro ConsistencyGroup[%s] with TargetGroup [%s] promot begin",
            iter->cg_id.c_str(),cg_id.c_str());
        print("the Hypermetro ConsistencyGroup[%s] with GroupID [%s] is being promoted",
            iter->cg_id.c_str(),cg_id.c_str());
        
        COMMLOG(OS_LOG_INFO,"the lun [%s] of TargetGroup [%s] with role [%s]",
            lun_id.c_str(),iter->cg_id.c_str(),role.c_str());
        print("the lun [%s] of TargetGroup [%s] with role [%s]",
            lun_id.c_str(),iter->cg_id.c_str(),role.c_str());

        ret = cmdOperate.CMD_swapconsistgrhm(cg_id);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_ERROR, "swap consistgr ret(%d).", ret);
            iter->error_info.code = OS_IToString(ret);
            ++iter;
            continue;
        }

        
        list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
        list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;
        string slave_key;
        string perrArry;

        ret = cmdOperate.CMD_showhypermetrolun_all(strHyID, lstHMPairLFInfo);
        if(ret != 0){
            COMMLOG(OS_LOG_ERROR,"show hypermetro lun err ret(%d)",ret);
        }

        for (itHMPairLFInfo = lstHMPairLFInfo.begin(); 
            itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
            if (MIRROR_LUN_RELATION_MASTER == itHMPairLFInfo->uiRelationType && !isNFS){
                slave_key = "G_" + itHMPairLFInfo->strID;
                perrArry = itHMPairLFInfo->strArrayWWN;
            }
        }
        COMMLOG(OS_LOG_INFO,"After reverse the hmdomain [%s] has attributes: [r/w]", iter->cg_id.c_str());
        print("After reverse the hypermetro group [%s] on array [%s] has attributes: [r/w]",
            iter->cg_id.c_str(),array_id);
        COMMLOG(OS_LOG_INFO,"After reverse the TargetGroup [%s] has attributes: [r/o]",slave_key.c_str());
        print("After reverse the TargetGroup [%s] on array [%s] has attributes: [r/o]",
            slave_key.c_str(),peer_array_id.c_str());
        
        COMMLOG(OS_LOG_INFO, "Complete: Promotion is now complete for group lun[%s] for reverse hypermetro request", lun_id.c_str());
        print("Complete: Promotion is now complete for hmdomain [%s] for reverse hypermetro request", iter->cg_id.c_str());
        iter->is_success = true;
        ++iter;
    }

    return;
}

void ReverseReplication::_outband_swap_hypermirror(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string strid;
    string hyper_id;
    string Slave_id;
    string Peer_array_id;
    string master_id;
    string all_LF = "";
    string key;
    string slave_key;
    string statu;
    string arrayid;
    string lun_flag;
    list<string> lst_token;
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;

    list<DeviceInfo>::iterator iter;

    iter = devices_info.lst_device_info.begin();
    while (iter != devices_info.lst_device_info.end()){
        
        lstHyMirrorLFInfo.clear();

        if (iter->error_info.hasError()){
            ++iter;
            continue;
        }

        ret = get_device_info(iter->id, arrayid, strid, lun_flag, hyper_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->id.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++iter;
            continue;
        }
        key = arrayid + "." + strid;

        
        COMMLOG(OS_LOG_INFO, "Commence: Replica of devices [%s] on array [%s] for reverse replication request",
            key.c_str(), array_id);
        print( "Commence: Replica of devices [%s] on array [%s] for reverse replication request",
            key.c_str(), array_id);

        if (hyper_id.empty()){
            ++iter;
            continue;
        }
        is_Fs_by_lun(cmdOperate, strid);
        
        COMMLOG(OS_LOG_INFO,"The Remote Replica id of lun [%s] is [%s]",strid.c_str(),hyper_id.c_str());
        print("The Remote Replica id of lun [%s] is [%s]",strid.c_str(),hyper_id.c_str());
        
        if (isNFS){
            ret = cmdOperate.CMD_showhymirrorbyfs(strid, lstHyMirrorLFInfo);
        } 
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(hyper_id, lstHyMirrorLFInfo);
        }
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            ++iter;
            continue;
        }

        
        for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); 
            itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
            if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLFInfo->uiRelationType){
                master_id = itHyMirrorLFInfo->strArrayWWN + "_" + itHyMirrorLFInfo->strID + "_" + MIRROR_LUN_TAG + "_" + hyper_id; 
                slave_key = itHyMirrorLFInfo->strArrayWWN + "." + itHyMirrorLFInfo->strID + "_" + MIRROR_LUN_TAG + "_" + hyper_id; 
            }
            if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLFInfo->uiRelationType && isNFS){
                hyper_id = itHyMirrorLFInfo->strMirrorID;
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLFInfo->uiRelationType){
                Slave_id = itHyMirrorLFInfo->strID;
                _convert_pair_status(itHyMirrorLFInfo->uiPairStatus, statu);

            }
        }

        if ((0 == master_id.length())){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++iter;
            continue;
        }

        
        
        if (master_id == iter->id || statu == MIRROR_SLAVE_PAIR_STATUS_STR_NORMAL){
            
            iter->is_success = true;
            ++iter;
            continue;
        }
        
        
        COMMLOG(OS_LOG_INFO, "Replica dvice [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        print("Replica device [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        
        ret = cmdOperate.CMD_swaphymirror(hyper_id, Peer_array_id, Slave_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            ++iter;
            COMMLOG(OS_LOG_ERROR, "CMD_swaphymirror failed (%d).", ret);
            continue;
        }

        
        if (isNFS){
            ret = cmdOperate.CMD_changeSlaveFsRw(hyper_id,RESOURCE_ACCESS_READ_ONLY);
        }
        else{
            ret = cmdOperate.CMD_changeSlaveLunRw(hyper_id, RESOURCE_ACCESS_READ_ONLY);
        }
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_ERROR, "CMD_changeSlaveRw failed (%d).", ret);
            iter->is_success = false;
            ++iter;
            continue;
        }
    

        
        COMMLOG(OS_LOG_INFO, "After reverse the promoted source  [%s] has attributes:[r/w]", key.c_str());
        print("After reverse the promoted source  [%s] has attributes:[r/w]", key.c_str());
        COMMLOG(OS_LOG_INFO, "After reverse the demoted target [%s] has attributes:[r/o]",slave_key.c_str());
        print("After reverse the demoted target [%s] has attributes:[r/o]",slave_key.c_str());

        
        COMMLOG(OS_LOG_INFO, "Complete: Replica is now complete for devices [%s] for reverse replication request.", key.c_str());
        print( "Complete: Replica is now complete for devices [%s] for reverse replication request.", key.c_str());
        iter->is_success = true;
        ++iter;
    }

    return;
}

void ReverseReplication::_outband_swap_hypermetropair(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string strid;
    string hyper_id;
    string Slave_id;
    string Peer_array_id;
    string master_id;
    string all_LF = "";
    string key;
    string slave_key;
    string statu;
    string arrayid;
    string lun_flag;
    list<string> lst_token;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;

    list<DeviceInfo>::iterator iter;

    iter = devices_info.lst_device_info.begin();
    while (iter != devices_info.lst_device_info.end()){
        
        lstHMPairLFInfo.clear();

        if (iter->error_info.hasError()){
            ++iter;
            continue;
        }

        ret = get_device_info(iter->id, arrayid, strid, lun_flag, hyper_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->id.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++iter;
            continue;
        }
        key = arrayid + "." + strid;

        
        COMMLOG(OS_LOG_INFO, "Commence: Hypermetro of devices [%s] on array [%s] for reverse hypermetro request",
            key.c_str(), array_id);
        print( "Commence: Hypermetro of devices [%s] on array [%s] for reverse hypermetro request",
            key.c_str(), array_id);

        if (hyper_id.empty()){
            ++iter;
            continue;
        }

        
        list<CMDHOSTINFO_STRU> rlstHostInfo;
        TargetDeviceInfo tmp_tg;
        tmp_tg.target_id = iter->id;
        tmp_tg.target_key = tmp_tg.target_id;
        tmp_tg.cg_accessgroups = iter->cg_accessgroups;

        if (!g_bFusionStorage){
            ret = _add_map(cmdOperate, tmp_tg, lstDARHosts, rlstHostInfo, strid, OBJ_LUN);
            if (RETURN_OK != ret){
                iter->error_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "Map remote lun [%s] to hosts failed.", strid.c_str());
                return;
            }
        }

        
        COMMLOG(OS_LOG_INFO,"The Hypermetro id of lun [%s] is [%s]",strid.c_str(),hyper_id.c_str());
        print("The Hypermetro id of lun [%s] is [%s]",strid.c_str(),hyper_id.c_str());
        
        ret = cmdOperate.CMD_showhypermetrolun_all(hyper_id, lstHMPairLFInfo);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            ++iter;
            continue;
        }

        
        for (itHMPairLFInfo = lstHMPairLFInfo.begin(); 
            itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
            if (MIRROR_LUN_RELATION_MASTER == itHMPairLFInfo->uiRelationType){
                master_id = itHMPairLFInfo->strArrayWWN + "_" + itHMPairLFInfo->strID + "_" + HYPERMETRO_LUN_TAG + "_" + hyper_id; 
                slave_key = itHMPairLFInfo->strArrayWWN + "." + itHMPairLFInfo->strID + "_" + HYPERMETRO_LUN_TAG + "_" + hyper_id; 
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHMPairLFInfo->uiRelationType){
                Slave_id = itHMPairLFInfo->strID;
                _convert_hmpair_status(itHMPairLFInfo->uiPairStatus, statu);

            }
        }

        if ((0 == master_id.length())){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++iter;
            continue;
        }

        
        
        if (master_id == iter->id){
            iter->is_success = true;
            ++iter;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Hypermetro dvice [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        print("Hypermetro device [%s] of Remote device [%s] is being promoted",
            key.c_str(),slave_key.c_str());
        
        ret = cmdOperate.CMD_swaphypermetro(hyper_id);
        if (RETURN_OK != ret){
            iter->error_info.code = OS_IToString(ret);
            ++iter;
            COMMLOG(OS_LOG_ERROR, "CMD_swaphypermetro failed (%d).", ret);
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "After reverse the promoted source  [%s] has attributes:[r/w]", key.c_str());
        print("After reverse the promoted source  [%s] has attributes:[r/w]", key.c_str());
        COMMLOG(OS_LOG_INFO, "After reverse the demoted target [%s] has attributes:[r/o]",slave_key.c_str());
        print("After reverse the demoted target [%s] has attributes:[r/o]",slave_key.c_str());

        
        COMMLOG(OS_LOG_INFO, "Complete: Hypermetro is now complete for devices [%s] for reverse hypermetro request.", key.c_str());
        print( "Complete: Hypermetro is now complete for devices [%s] for reverse hypermetro request.", key.c_str());
        iter->is_success = true;
        ++iter;
    }

    return;
}

