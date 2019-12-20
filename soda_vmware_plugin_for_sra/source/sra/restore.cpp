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

#include "restore.h"

int restore(XmlReader &reader)
{
    RestoreReplication restore_rp;

    COMMLOG(OS_LOG_INFO, "%s", "restore begin.");
    print("%s", "restore begin.");
    if (RETURN_OK != restore_rp.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "restore failed.");
        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "restore end.");
    print("%s", "restore end.");
    return RETURN_OK;
}

int RestoreReplication::_read_command_para(XmlReader &reader)
{
    int i = 0;
    int cg_count = 0;
    int device_count = 0;
    char peer_array[LENGTH_COMMON] = {0};

    (void)reader.get_string("/Command/RestoreReplicationParameters/ArrayId", array_id, 0);
    (void)reader.get_string("/Command/RestoreReplicationParameters/ArrayId", input_array_id, 0);
    (void)reader.get_string("/Command/RestoreReplicationParameters/PeerArrayId", peer_array, 0);

    peer_array_id = peer_array;

    cg_count = reader.get_count("/Command/RestoreReplicationParameters/ConsistencyGroups/ConsistencyGroup");
    for (i = 0; i < cg_count; i++){
        char cg_id[LENGTH_COMMON];
        ConGroupInfo gp_info;

        memset_s(cg_id, sizeof(cg_id), 0, sizeof(cg_id));
        (void)reader.get_string("/Command/RestoreReplicationParameters/ConsistencyGroups/ConsistencyGroup/@id", cg_id, i);
        
        gp_info.cg_id = cg_id;
        group_info.lst_groups_info.push_back(gp_info);
    }

    device_count = reader.get_count("/Command/RestoreReplicationParameters/Devices/Device");
    for (i = 0; i < device_count; i++){
        char dev_id[LENGTH_COMMON];
        DeviceInfo device_info;

        memset_s(dev_id, sizeof(dev_id), 0, sizeof(dev_id));
        (void)reader.get_string("/Command/RestoreReplicationParameters/Devices/Device/@id", dev_id, i);
        
        device_info.id = dev_id;
        devices_info.lst_device_info.push_back(device_info);
    }

    return RETURN_OK;
}

int RestoreReplication::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "RestoreReplication::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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

void RestoreReplication::_write_response(XmlWriter &writer)
{
    if(!group_info.lst_groups_info.empty()){
        (void)writer.set_xml("/Response/RestoreReplicationResults/ConsistencyGroups", &group_info);
    }

    if (!devices_info.lst_device_info.empty()){
        (void)writer.set_xml("/Response/RestoreReplicationResults/Devices", &devices_info);
    }

    return;
}

int RestoreReplication::_outband_process()
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
    CHECK_UNEQ(RETURN_OK, iRet);

    (void)_is_slave_consistency_out(cmdOperate);
    if (g_bstretch){
        (void)_is_slave_consistencyhm_out(cmdOperate);
    }

    (void)_is_slave_device_out(cmdOperate);
    if (g_bstretch){
        (void)_is_slavehm_device_out(cmdOperate);
    }

    return RETURN_OK;
}

int RestoreReplication::_is_slave_consistency_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string consisgr_status;
    list<string> lst_token;
    string lun_id;
    string cg_id;
    string cg_name;
    string arrayid;
    string lun_flag;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;

    list<ConGroupInfo>::iterator iter_group = group_info.lst_groups_info.begin();

    while (iter_group != group_info.lst_groups_info.end()){
        GROUP_INFO_STRU stGroupInfo;

        ret = get_device_info(iter_group->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            iter_group->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_group->cg_id.c_str());
            ++iter_group;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++iter_group;
            continue;
        }

        if (cg_id.empty()){
            ++iter_group;
            continue;
        }

        if ("--" == cg_id){
            iter_group->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter_group;
            continue;
        }
        print("Commence: Replica of ConsistencyGroup [%s] on array [%s] for restore replication request",
            iter_group->cg_id.c_str(), array_id);
        stGroupInfo.strID = cg_id;
        ret = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
        if (RETURN_OK != ret){
            iter_group->error_info.code = OS_IToString(ret);
            ++iter_group;
            continue;
        }

        cg_name = stGroupInfo.strName;

        if ( 0 == cg_name.length()){
            iter_group->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter_group;
            continue;
        }

        _convert_pair_status(stGroupInfo.uiState, consisgr_status);
        print("The ConsistencyGroup [%s] with GroupId [%s] has status [%s]",
            iter_group->cg_id.c_str(),cg_id.c_str(),consisgr_status.c_str());

        if (MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED == consisgr_status){
            ret = cmdOperate.CMD_changeCGSlaveLunRw(cg_id, RESOURCE_ACCESS_READ_ONLY);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "change consistgr SlaveLun RO ret(%d).", ret);
                iter_group->is_success = false;
                ++iter_group;
                continue;
            }
            print("The ConsistencyGroup [%s] with GroupId [%s] is being restored",
                iter_group->cg_id.c_str(),cg_id.c_str());

            if(!isNFS){
                ret = cmdOperate.CMD_showhymirrorlun_all(strHyID, lstHyMirrorLFInfo);
                if(ret != 0){
                    COMMLOG(OS_LOG_ERROR,"show hymirror lun err ret(%d)",ret);
                }
            }
            string slave;
            string perrArry;
            for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
                if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLFInfo->uiRelationType && !isNFS){
                    slave = "G_" + itHyMirrorLFInfo->strID;
                    perrArry = itHyMirrorLFInfo->strArrayWWN;
                }
            }
            print("After restore,the TargetGroup [%s] on array [%s] has attributes: [r/o]",
                slave.c_str(),
                peer_array_id.c_str());
            print("After restore,the ConsistencyGroup [%s] on array [%s] has attributes: [r/w]",
                iter_group->cg_id.c_str(),
                array_id);
            ret = cmdOperate.CMD_syncconsistgr(cg_id);
        }
        else{
            iter_group->war_info.code = OS_IToString(500);
        }
        print( "Complete: Promotion is now complete for ConsistencyGroup[%s] for restore replication request", iter_group->cg_id.c_str());
        iter_group->is_success = true;
        ++iter_group;
    }

    return RETURN_OK;
}

int RestoreReplication::_is_slave_consistencyhm_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string consisgr_status;
    list<string> lst_token;
    string lun_id;
    string cg_id;
    string cg_name;
    string arrayid;
    string lun_flag;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;

    list<ConGroupInfo>::iterator iter_group = group_info.lst_groups_info.begin();

    while (iter_group != group_info.lst_groups_info.end()){
        HYPERMETRODOMAIN_LF_INFO_STRU stHMDomainInfo;

        ret = get_device_info(iter_group->cg_id, arrayid, lun_id, lun_flag, cg_id);
        if (RETURN_OK != ret){
            iter_group->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_group->cg_id.c_str());
            ++iter_group;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++iter_group;
            continue;
        }
        
        if (cg_id.empty() ){
            ++iter_group;
            continue;
        }

        if ("--" == cg_id){
            iter_group->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter_group;
            continue;
        }
        print("Commence: Replica of hypermetro domain [%s] on array [%s] for restore replication request",
            iter_group->cg_id.c_str(), array_id);
        
        GROUP_INFO_STRU stGroupInfo;
        stGroupInfo.strID = cg_id;
        ret = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
        if (RETURN_OK != ret){
            iter_group->error_info.code = OS_IToString(ret);
            ++iter_group;
            continue;
        }

        cg_name = stGroupInfo.strName;

        if ( 0 == cg_name.length()){
            iter_group->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++iter_group;
            continue;
        }

        _convert_hmpair_status(stGroupInfo.uiState, consisgr_status);
        print("The hypermetro ConsistencyGroup [%s] with GroupId [%s] has status [%s]",
            iter_group->cg_id.c_str(),cg_id.c_str(),consisgr_status.c_str());

        if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == consisgr_status){
            print("The ConsistencyGroup [%s] with GroupId [%s] is being restored",
                iter_group->cg_id.c_str(),cg_id.c_str());

            ret = cmdOperate.CMD_showhypermetrolun_all(strHyID, lstHMPairLFInfo);
            if(ret != 0){
                COMMLOG(OS_LOG_ERROR,"show hymirror lun err ret(%d)",ret);
            }

            string slave;
            string perrArry;
            for (itHMPairLFInfo = lstHMPairLFInfo.begin(); itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
                if (HM_LUN_RELATION_MASTER == itHMPairLFInfo->uiRelationType && !isNFS){
                    slave = "G_" + itHMPairLFInfo->strID;
                    perrArry = itHMPairLFInfo->strArrayWWN;
                }
            }
            print("After restore,the TargetGroup [%s] on array [%s] has attributes: [r/o]",
                slave.c_str(),
                peer_array_id.c_str());
            print("After restore,the ConsistencyGroup [%s] on array [%s] has attributes: [r/w]",
                iter_group->cg_id.c_str(),
                array_id);
            ret = cmdOperate.CMD_syncconsistgrhm(cg_id);
            COMMLOG(OS_LOG_ERROR, "sync hypermetro consistgr ret(%d).", ret);  
        }
        else{
            iter_group->war_info.code = OS_IToString(500);
        }
        print( "Complete: Promotion is now complete for ConsistencyGroup[%s] for restore replication request", iter_group->cg_id.c_str());
        iter_group->is_success = true;
        ++iter_group;
    }

    return RETURN_OK;
}

int RestoreReplication::_is_slave_device_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_slave_array_sn;
    list<string> lst_slave_id;
    list<string> lst_pair_status;
    list<string> lst_token;
    string strid;
    string hyper_id;
    string key;
    string arrayid;
    string lun_flag;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;
    list<DeviceInfo>::iterator iter_hymirror_id = devices_info.lst_device_info.begin();

    while (iter_hymirror_id != devices_info.lst_device_info.end()){
        lstHyMirrorLFInfo.clear();

        ret = get_device_info(iter_hymirror_id->id, arrayid, strid, lun_flag, hyper_id);
        if (RETURN_OK != ret){
            iter_hymirror_id->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hymirror_id->id.c_str());
            ++iter_hymirror_id;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++iter_hymirror_id;
            continue;
        }

        key = arrayid + "." + strid;
        print( "Commence: Replica of devices [%s] on array [%s] for restore replication request",
            key.c_str(), array_id);

        if (hyper_id.empty()){
            ++iter_hymirror_id;
            continue;
        }
        is_Fs_by_lun(cmdOperate, strid);
        if (isNFS){
            ret = cmdOperate.CMD_showhymirrorbyfs(strid, lstHyMirrorLFInfo);//mark
        } 
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(hyper_id, lstHyMirrorLFInfo);
        }
        if (RETURN_OK != ret){
            iter_hymirror_id->error_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hymirror_id->id.c_str());
            ++iter_hymirror_id;
            continue;
        }

        string slave;
        for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLFInfo->uiRelationType){
                string strPairStatus;

                lst_slave_array_sn.push_back(itHyMirrorLFInfo->strArrayWWN);
                lst_slave_id.push_back(itHyMirrorLFInfo->strID);
                slave = itHyMirrorLFInfo->strArrayWWN + "." + itHyMirrorLFInfo->strID;
                _convert_pair_status(itHyMirrorLFInfo->uiPairStatus, strPairStatus);
                lst_pair_status.push_back(strPairStatus);
            }
            if (MIRROR_LUN_RELATION_MASTER == itHyMirrorLFInfo->uiRelationType && isNFS){
                hyper_id = itHyMirrorLFInfo->strMirrorID;
            }
        }

        if (lst_slave_array_sn.empty() || lst_slave_id.empty()){
            iter_hymirror_id->error_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            ++iter_hymirror_id;
            continue;
        }

        print("The device [%s] with Remote Replica id [%s] has status [%s]",key.c_str(),hyper_id.c_str(),lst_pair_status.front().c_str());

        if (!lst_pair_status.empty() && MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED == lst_pair_status.front()){
            if (isNFS){
                ret = cmdOperate.CMD_changeSlaveFsRw(hyper_id,RESOURCE_ACCESS_READ_ONLY);
            }
            else{
                ret = cmdOperate.CMD_changeSlaveLunRw(hyper_id, RESOURCE_ACCESS_READ_ONLY);
            }
            print("The device [%s] with Remote Replica id [%s] is being restored",key.c_str(),hyper_id.c_str());
            if (RETURN_OK != ret){
                COMMLOG(OS_LOG_ERROR, "CMD_changeSlaveLunRw failed (%d).", ret);
                ++iter_hymirror_id;
                continue;
            }
            print("After restore,the source device [%s] has has attributes: [r/w]",key.c_str());
            print("After restore,the target device [%s] has has attributes: [r/o]",slave.c_str());
            ret = cmdOperate.CMD_synchymirror(hyper_id, lst_slave_array_sn.front(), lst_slave_id.front());
    
            COMMLOG(OS_LOG_ERROR, "sync device ret(%d).", ret); 
        }
        else{
            iter_hymirror_id->war_info.code = OS_IToString(500);
        }
        print( "Complete: Promotion is now complete for device [%s] for restore replication request", key.c_str());
        iter_hymirror_id->is_success = true;

        ++iter_hymirror_id;
    }

    return RETURN_OK;
}

int RestoreReplication::_is_slavehm_device_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_slave_array_sn;
    list<string> lst_slave_id;
    list<string> lst_pair_status;
    list<string> lst_token;
    string strid;
    string hypermetro_id;
    string key;
    string master_id;
    string arrayid;
    string lun_flag;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;
    list<DeviceInfo>::iterator iter_hmpair_id = devices_info.lst_device_info.begin();

    while (iter_hmpair_id != devices_info.lst_device_info.end()){
        lstHMPairLFInfo.clear();

        ret = get_device_info(iter_hmpair_id->id, arrayid, strid, lun_flag, hypermetro_id);
        if (RETURN_OK != ret){
            iter_hmpair_id->error_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hmpair_id->id.c_str());
            ++iter_hmpair_id;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++iter_hmpair_id;
            continue;
        }

        key = arrayid + "." + strid;
        print( "Commence: Hypermetro of devices [%s] on array [%s] for restore replication request",
            key.c_str(), array_id);

        if (hypermetro_id.empty()){
            ++iter_hmpair_id;
            continue;
        }

        ret = cmdOperate.CMD_showhypermetrolun_all(hypermetro_id, lstHMPairLFInfo);
        if (RETURN_OK != ret){
            iter_hmpair_id->error_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_hmpair_id->id.c_str());
            ++iter_hmpair_id;
            continue;
        }

        string slave;
        for (itHMPairLFInfo = lstHMPairLFInfo.begin(); itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
            if (HM_LUN_RELATION_SLAVE == itHMPairLFInfo->uiRelationType){
                string strPairStatus;

                lst_slave_array_sn.push_back(itHMPairLFInfo->strArrayWWN);
                lst_slave_id.push_back(itHMPairLFInfo->strID);
                slave = itHMPairLFInfo->strArrayWWN + "." + itHMPairLFInfo->strID;
                _convert_hmpair_status(itHMPairLFInfo->uiPairStatus, strPairStatus);
                lst_pair_status.push_back(strPairStatus);
            }
            if (HM_LUN_RELATION_MASTER == itHMPairLFInfo->uiRelationType){
                hypermetro_id = itHMPairLFInfo->strHMpairID;
                master_id = itHMPairLFInfo->strArrayWWN + "_" + itHMPairLFInfo->strID + "_" + MIRROR_LUN_TAG + "_" + hypermetro_id; 
            }
        }

        if (lst_slave_array_sn.empty() || lst_slave_id.empty()){
            iter_hmpair_id->error_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            ++iter_hmpair_id;
            continue;
        }

        if (0 != master_id.length() && master_id == iter_hmpair_id->id){
            COMMLOG(OS_LOG_INFO, "Protect LUN(%s) is primary, now swap recover LUN to primary." ,master_id.c_str());
            ret = cmdOperate.CMD_swaphypermetro(hypermetro_id);
            if (RETURN_OK != ret){
                iter_hmpair_id->error_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "CMD_swaphypermetro failed, ret is %d.", ret);
                ++iter_hmpair_id;
                continue;
            }
        }

        print("The device [%s] with hypermetro id [%s] has status [%s]",
            key.c_str(),hypermetro_id.c_str(),lst_pair_status.front().c_str());

        if (!lst_pair_status.empty() && HM_SLAVE_PAIR_STATUS_STR_PAUSE == lst_pair_status.front()){
            print("The device [%s] with hypermetro id [%s] is being restored",key.c_str(),hypermetro_id.c_str());
            
            ret = cmdOperate.CMD_synchmpair(hypermetro_id);
       
            COMMLOG(OS_LOG_ERROR, "sync device ret(%d).", ret); 
        }
        else{
            iter_hmpair_id->war_info.code = OS_IToString(500);
        }
        print( "Complete: Promotion is now complete for device [%s] for restore hypermetro request", key.c_str());
        iter_hmpair_id->is_success = true;

        ++iter_hmpair_id;
    }

    return RETURN_OK;
}

