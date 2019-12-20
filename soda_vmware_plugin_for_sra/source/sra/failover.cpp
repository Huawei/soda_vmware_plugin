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

#include "failover.h"

FailOver::FailOver() : SraBasic()
    ,ThreeDCLunHelper(this->m_array_id,this->consistency_group_info,this->devices_info)
{
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
}

FailOver::~FailOver()
{

};

int failover(XmlReader &reader)
{
    FailOver fail_over;

    COMMLOG(OS_LOG_INFO, "%s", "FailOver begin.");
    print("%s", "FailOver begin.");
    if (RETURN_OK != fail_over.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "fail over failed.");
        return RETURN_ERR;
    }

    COMMLOG(OS_LOG_INFO, "%s", "FailOver end.");
    print("%s", "FailOver end.");
    return RETURN_OK;
}

int FailOver::_read_command_para(XmlReader &reader)
{
    int i = 0,j = 0;
    int tg_count = 0;
    int device_count = 0;
    int accessgroup_count = 0;
    int accessgroups_count = 0;
    int accessinitiator_count = 0;

    (void)reader.get_string("/Command/FailoverParameters/ArrayId", array_id, 0);
    (void)reader.get_string("/Command/FailoverParameters/ArrayId", input_array_id, 0);

    m_array_id = input_array_id;

    tg_count = reader.get_count("/Command/FailoverParameters/TargetGroups/TargetGroup");

    for (i = 0; i < tg_count; i++){
        char tg_key[LENGTH_COMMON] = {0};
        char tg_isolationRequired[LENGTH_COMMON] = {0};

        TargetGroupInfo tg_info;

        (void)reader.get_string("/Command/FailoverParameters/TargetGroups/TargetGroup/@key", tg_key, i);
        (void)reader.get_string("/Command/FailoverParameters/TargetGroups/TargetGroup/@isolationRequired", tg_isolationRequired, i);

        tg_info.tg_key = tg_key;
        tg_info.isolationRequired = tg_isolationRequired;

        ConGroupInfo cg_info;
        cg_info.cg_id = tg_key;
        consistency_group_info.lst_groups_info.push_back(cg_info);

        tg_groups.lst_groups_info.push_back(tg_info);
        
        device_count = reader.get_count("/Command/FailoverParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice", i, 2);
        
        for(j = 0; j < device_count; j++){
            char target_key[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/FailoverParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/@key", target_key, j);
            
            accessgroups_count = reader.get_count("/Command/FailoverParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/AccessGroups/AccessGroup",j,2);
            
            string cg_accessgroups = "";
            for (int k = 0; k < accessgroups_count; k++){
                char group_id[LENGTH_COMMON] = {0};
                (void)reader.get_string("/Command/FailoverParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/AccessGroups/AccessGroup/@id", group_id, k,j,2);
                
                cg_accessgroups = cg_accessgroups + group_id + DAR_HOST_SEPERATOR;
            }
            COMMLOG(OS_LOG_INFO, "AccessGroups of TargetDevice[%s] is [%s].", target_key, cg_accessgroups.c_str());
            accessGroupDict[target_key] = cg_accessgroups;
        }
    }

    device_count = reader.get_count("/Command/FailoverParameters/TargetDevices/TargetDevice");

    for (i = 0; i < device_count; i++){
        char target_key[LENGTH_COMMON] = {0};
        char target_isolationRequired[LENGTH_COMMON] = {0};

        TargetDeviceInfo tg_device_info;

        (void)reader.get_string("/Command/FailoverParameters/TargetDevices/TargetDevice/@key", target_key, i);
        (void)reader.get_string("/Command/FailoverParameters/TargetDevices/TargetDevice/@isolationRequired", target_isolationRequired, i);
        accessgroups_count = reader.get_count("/Command/FailoverParameters/TargetDevices/TargetDevice/AccessGroups/AccessGroup",i,2);
        for (j = 0; j < accessgroups_count; j++){
            char group_id[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/FailoverParameters/TargetDevices/TargetDevice/AccessGroups/AccessGroup/@id", group_id, j,i,2);
            tg_device_info.cg_accessgroups = tg_device_info.cg_accessgroups + group_id + ":";
        }
        tg_device_info.target_key = target_key;
        tg_device_info.isolationRequired = target_isolationRequired;
        tg_devices.lst_target_devices.push_back(tg_device_info);

        DeviceInfo dev_info;
        dev_info.id = target_key;
        
        devices_info.lst_device_info.push_back(dev_info);
    }
       
    accessgroup_count = reader.get_count("/Command/FailoverParameters/AccessGroups/AccessGroup");
    for(j = 0 ;j < accessgroup_count; j++){
        char group_id[LENGTH_COMMON] = {0};
        (void)reader.get_string("/Command/FailoverParameters/AccessGroups/AccessGroup/@id", group_id, j);
        accessinitiator_count = reader.get_count("/Command/FailoverParameters/AccessGroups/AccessGroup/Initiator",j,1);
        for (i = 0; i < accessinitiator_count; i++){
            char initiator_id[LENGTH_COMMON] = {0};
            char initiator_type[LENGTH_COMMON] = {0};
            DAR_HOST_INFO tg_dar_host_info;
            (void)reader.get_string("/Command/FailoverParameters/AccessGroups/AccessGroup/Initiator/@id", initiator_id, i,j,1);
            (void)reader.get_string("/Command/FailoverParameters/AccessGroups/AccessGroup/Initiator/@type", initiator_type, i,j,1);
            tg_dar_host_info.strID = initiator_id;
            tg_dar_host_info.strType = initiator_type;
            tg_dar_host_info.strGroup = group_id;
            lstDARHosts.push_back(tg_dar_host_info);
        }
    }

    return RETURN_OK;
}

int FailOver::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
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

void FailOver::_write_response(XmlWriter &writer)
{
    
    list<TargetGroupInfo>::iterator iter_target_group;
    list<TargetDeviceInfo>::iterator iter_target_device;

    iter_target_group = tg_groups.lst_groups_info.begin();
    while (iter_target_group != tg_groups.lst_groups_info.end()){
        if ((iter_target_group->cg_id.empty()) && (!iter_target_group->err_info.hasError())){
            iter_target_group->err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        }
        if ((iter_target_group->cg_id.empty()) && (!iter_target_group->war_info.hasWarning())){
            iter_target_group->war_info.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
        }
        ++iter_target_group;
    }

    iter_target_device = tg_devices.lst_target_devices.begin();
    while (iter_target_device != tg_devices.lst_target_devices.end()){
        if (iter_target_device->target_id.empty() && (!iter_target_device->err_info.hasError())){
            iter_target_device->err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        }
        if (iter_target_device->target_id.empty() && (!iter_target_device->war_info.hasWarning())){
            iter_target_device->war_info.code = OS_IToString(WARN_REPLICATION_IS_PROCESSING);
        }
        ++iter_target_device;
    }

    (void)writer.set_xml("/Response/FailoverResults", &tg_groups);
    (void)writer.set_xml("/Response/FailoverResults", &tg_devices);
    if (!storageports_info.lst_storageportsips.empty()){
        (void)writer.set_xml("/Response/FailoverResults", &storageports_info);
    }
}

int FailOver::_outband_process()
{
    int iRet = RETURN_ERR;
    HYPER_STORAGE_STRU stStorageInfo;
    CCmdOperate cmdOperate;
    list<LIF_INFO_STRU> lstLifInfo;
    list<LIF_INFO_STRU>::iterator itLifInfo;

    (void)setStorageInfo(stStorageInfo);

    iRet = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, iRet);
   
    iRet = _get_array_info(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);
   
    string array_id_temp(array_id);
    iRet = check_array_id_validate_out(cmdOperate, array_id_temp);
    CHECK_UNEQ(RETURN_OK, iRet);
  
    lstMapInfo.clear();
    iRet = cmdOperate.CMD_showmap(lstMapInfo);
    CHECK_UNEQ(RETURN_OK,iRet);
    
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
    
    if(g_bnfs == true){
        cmdOperate.CMD_showLIF(lstLifInfo);
        for (itLifInfo = lstLifInfo.begin(); itLifInfo != lstLifInfo.end(); ++itLifInfo){
            
            if(itLifInfo->role == SERVICE_ROLE || itLifInfo->role == MANAGEMENT_AND_SERVICE_ROLE){
                StoragePortsIPS storageportsips;
                storageportsips.source_IP = itLifInfo->strLIFIP;
                storageports_info.lst_storageportsips.push_back(storageportsips);
            }
            else{
                COMMLOG(OS_LOG_INFO, "Filtered logical port [%s], because it cannot be a service port", itLifInfo->strLIFIP.c_str());
            }                    
        }
    }
       
    deal_3dc_lun(cmdOperate);
    
    return RETURN_OK;
}

void FailOver::deal_3dc_lun(CCmdOperate& cmdOperate)
{
}

int FailOver::_outband_swap_consist(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cmd;
    string status;
    string role;
    string cgid;
    string arrayid;
    string lunid;
    string lun_flag;
    unsigned int uiResAccess = 0;
    list<TargetGroupInfo>::iterator iter;
    
    list<Log_Info>::iterator itLFLog;
    Log_Info TempLog;
    lstLogInfo.clear();
    string all_lun = "";
    string comma = "";
    string all_key = "";
    if (!tg_groups.lst_groups_info.empty()){
        for(iter = tg_groups.lst_groups_info.begin(); iter != tg_groups.lst_groups_info.end(); ++iter){
            iRet = get_device_info(iter->tg_key, arrayid, lunid, lun_flag, cgid);
            if (RETURN_OK != iRet){
                iter->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->tg_key.c_str());
                continue;
            }

            if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
                continue;
            }
            TempLog.strID = lunid;
            TempLog.strName = iter->tg_key;
            lstLogInfo.push_back(TempLog);
        }
        for(itLFLog = lstLogInfo.begin(); itLFLog != lstLogInfo.end(); ++itLFLog){
            all_lun += comma + itLFLog->strID;
            all_key += comma+ itLFLog->strName;
            comma = ",";
        }
        if (!lstLogInfo.empty()){
            COMMLOG(OS_LOG_INFO,"Commence: Suspending replication of [%s] on array [%s] for failover request",
                all_lun.c_str(),array_id);
            print("Commence: Suspending replication of [%s] on array [%s] for failover request",
                all_lun.c_str(),array_id);
            for(itLFLog = lstLogInfo.begin(); itLFLog != lstLogInfo.end(); ++itLFLog){
                COMMLOG(OS_LOG_INFO,"Replication session [%s] to TargetGroup [%s] on array [%s] is terminated. Target is now promotable.",
                    itLFLog->strID.c_str(),itLFLog->strName.c_str(),array_id);
                print("Replication session [%s] to TargetGroup [%s] on array [%s] is terminated. Target is now promotable.",
                    itLFLog->strID.c_str(),itLFLog->strName.c_str(),array_id);
            }
            COMMLOG(OS_LOG_INFO,"Complete: Suspension of replication of [%s] on array [%s] for failover request is now COMPLETE",
                all_lun.c_str(),array_id);
            print("Complete: Suspension of replication of [%s] on array [%s] for failover request is now COMPLETE",
                all_lun.c_str(),array_id);

            COMMLOG(OS_LOG_INFO,"Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(),array_id);
            print("Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(),array_id);
        }
    }
    
    for (iter = tg_groups.lst_groups_info.begin(); iter != tg_groups.lst_groups_info.end(); ++iter){
        iRet = get_device_info(iter->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->tg_key.c_str());
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            continue;
        }
       
        if (cgid.empty()){
            continue;
        }
        
        if ("--" == cgid){
            iter->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR, "id of consisgroup(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        is_Fs_by_lun(cmdOperate, lunid);
       
        iRet = _get_cg_status_out(cmdOperate, cgid, status, role, uiResAccess, isNFS);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "_get_cg_status_out faild(%d).", iRet);
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Replication TargetGroup [%s] with groupID [%s] status [%s] role [%s]",
            iter->tg_key.c_str(),
            cgid.c_str(),
            status.c_str(),
            role.c_str());
        print("Replication TargetGroup [%s] with groupID [%s] status [%s] role [%s]",
            iter->tg_key.c_str(),
            cgid.c_str(),
            status.c_str(),
            role.c_str());
        string TimeNowe = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Replica of [%s] with key [%s] is being promoted from Recovery Point [%s]",
            lunid.c_str(),iter->tg_key.c_str(),TimeNowe.c_str());
        print("Replica of [%s] with key [%s] is being promoted from Recovery Point [%s]",
            lunid.c_str(),iter->tg_key.c_str(),TimeNowe.c_str());
       
        iRet = _exec_failover_out(cmdOperate , cgid, status, role, uiResAccess, *iter, isNFS);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "_exec_failover_out faild(%d).", iRet);
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Promoted target [%s] has attributes: [r/w]",
            iter->tg_key.c_str());
        print("Promoted target [%s] has attributes: [r/w]",
            iter->tg_key.c_str());

        iRet = _get_consisgr_info_out(cmdOperate, cgid, *iter, isNFS);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "_get_consisgr_info_out faild(%d).", iRet);
            iter->err_info.code = OS_IToString(iRet);
        }
    
        if(!isNFS){
            
            list<MAP_INFO_STRU>::iterator itMapInfo;
            list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
            list<HOST_PORT_INFO_STRU>::iterator ithostPort;

            list<CMDHOSTINFO_STRU> rlstHostInfo;
            list<CMDHOSTINFO_STRU>::iterator itHostInfo;

            for(itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end();itMapInfo++){
                if(lunid == itMapInfo->strDevLUNID){
                    cmdOperate.CMD_showhostByhostGroup(itMapInfo->strHostGroupID,rlstHostInfo);
                    for(itHostInfo = rlstHostInfo.begin();itHostInfo !=rlstHostInfo.end();++itHostInfo){
                        string StrIQN("");
                        string strChapname("");
                        string strCom("");
                        unsigned int fcOriscsi = 0;
                        cmdOperate.CMD_showhostport(itHostInfo->strID,rlstHostPortInfo);
                        for(ithostPort = rlstHostPortInfo.begin();ithostPort != rlstHostPortInfo.end();ithostPort++){
                            if(!ithostPort->strIQN.empty() && ithostPort->strIQN.size() > 1){
                                StrIQN += strCom + ithostPort->strIQN;
                                strChapname += strCom + ithostPort->strChapName;
                                fcOriscsi = ithostPort->uifcOriscsi;
                                strCom = ",";
                            }
                        }
                        if( fcOriscsi == 0){
                            COMMLOG(OS_LOG_INFO,"Promoted TargetGroup [%s] is being exposed to host [%s] using chapName [%s]",
                                iter->tg_key.c_str(),
                                StrIQN.c_str(),
                                strChapname.c_str());
                            print("Promoted TargetGroup [%s] is being exposed to host [%s] using chapName [%s]",
                                iter->tg_key.c_str(),
                                StrIQN.c_str(),
                                strChapname.c_str());
                        }
                        else{
                            COMMLOG(OS_LOG_INFO,"Promoted TargetGroup [%s] is being exposed to host [%s]",
                                iter->tg_key.c_str(),
                                StrIQN.c_str());
                            print("Promoted TargetGroup [%s] is being exposed to host [%s]",
                                iter->tg_key.c_str(),
                                StrIQN.c_str());
                        }
                    }
                    break;
                }
            }
        }
    }
    if (!tg_groups.lst_groups_info.empty()){
        if (!all_key.empty()){
            COMMLOG(OS_LOG_INFO,"Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
            print("Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
        }
    }
    return RETURN_OK;
}

int FailOver::_outband_swap_consisthm(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cmd;
    string status;
    string role;
    string cgid;
    string arrayid;
    string lunid;
    string lun_flag;
    list<string>lun;
    unsigned int uiResAccess = 0;
    list<TargetGroupInfo>::iterator iter;
  
    list<Log_Info>::iterator itLFLog;
    Log_Info TempLog;
    lstLogInfo.clear();
    string all_lun = "";
    string comma = "";
    string all_key = "";
    if (!tg_groups.lst_groups_info.empty()){
        for(iter = tg_groups.lst_groups_info.begin(); iter != tg_groups.lst_groups_info.end(); ++iter){
            iRet = get_device_info(iter->tg_key, arrayid, lunid, lun_flag, cgid);
            if (RETURN_OK != iRet){
                iter->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->tg_key.c_str());
                continue;
            }

            if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
                continue;
            }
            TempLog.strID = lunid;
            TempLog.strName = iter->tg_key;
            lstLogInfo.push_back(TempLog);
        }
        for(itLFLog = lstLogInfo.begin(); itLFLog != lstLogInfo.end(); ++itLFLog){
            all_lun += comma + itLFLog->strID;
            all_key += comma+ itLFLog->strName;
            comma = ",";
        }
        if (!lstLogInfo.empty()){
            COMMLOG(OS_LOG_INFO,"Commence: Suspending hypermetro of [%s] on array [%s] for failover request",
                all_lun.c_str(),array_id);
            print("Commence: Suspending hypermetro of [%s] on array [%s] for failover request",
                all_lun.c_str(),array_id);
            for(itLFLog = lstLogInfo.begin(); itLFLog != lstLogInfo.end(); ++itLFLog){
                COMMLOG(OS_LOG_INFO,"Hypermetro session [%s] to TargetGroup [%s] on array [%s] is terminated. "
                    "Target is now promotable.",
                    itLFLog->strID.c_str(),itLFLog->strName.c_str(),array_id);
                print("Hypermetro session [%s] to TargetGroup [%s] on array [%s] is terminated. Target is now promotable.",
                    itLFLog->strID.c_str(),itLFLog->strName.c_str(),array_id);
            }
            COMMLOG(OS_LOG_INFO,"Complete: Suspension of hypermetro of [%s] on array [%s] for failover request is now COMPLETE",
                all_lun.c_str(),array_id);
            print("Complete: Suspension of hypermetro of [%s] on array [%s] for failover request is now COMPLETE",
                all_lun.c_str(),array_id);

            COMMLOG(OS_LOG_INFO,"Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(),array_id);
            print("Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(),array_id);
        }
    }
   
    for (iter = tg_groups.lst_groups_info.begin(); iter != tg_groups.lst_groups_info.end(); ++iter){
        iRet = get_device_info(iter->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->tg_key.c_str());
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            continue;
        }
        
        if (cgid.empty()){
            continue;
        }
        
        if ("--" == cgid){
            iter->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR, "id of hypermetro group(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }
       
        iRet = _get_cghm_status_out(cmdOperate, cgid, status, role, uiResAccess);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "_get_cg_status_out faild(%d).", iRet);
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Hypermetro TargetGroup [%s] with groupID [%s] status [%s] role [%s]",
            iter->tg_key.c_str(),
            cgid.c_str(),
            status.c_str(),
            role.c_str());
        print("Hypermetro TargetGroup [%s] with groupID [%s] status [%s] role [%s]",
            iter->tg_key.c_str(),
            cgid.c_str(),
            status.c_str(),
            role.c_str());
        string TimeNowe = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Hypermetro of [%s] with key [%s] is being promoted from Recovery Point [%s]",
            iter->lun_id.c_str(),iter->tg_key.c_str(),TimeNowe.c_str());
        print("Hypermetro of [%s] with key [%s] is being promoted from Recovery Point [%s]",
            iter->lun_id.c_str(),iter->tg_key.c_str(),TimeNowe.c_str());

        
        if (HM_ISOLATIONREQUIRED_TRUE == iter->isolationRequired){
            iRet = _exec_failover_hm_out(cmdOperate , cgid, status, role, *iter);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "_exec_failover_hm_out faild(%d).", iRet);
                continue;
            }
        }
        
        COMMLOG(OS_LOG_INFO,"Promoted target [%s] has attributes: [r/w]",
            iter->tg_key.c_str());
        print("Promoted target [%s] has attributes: [r/w]",
            iter->tg_key.c_str());

        iRet = _get_consisgrhm_info_out(cmdOperate, cgid, *iter);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "_get_consisgrhm_info_out faild(%d).", iRet);
            iter->err_info.code = OS_IToString(iRet);
        }
        
        list<MAP_INFO_STRU>::iterator itMapInfo;
        list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
        list<HOST_PORT_INFO_STRU>::iterator ithostPort;

        list<CMDHOSTINFO_STRU> rlstHostInfo;
        list<CMDHOSTINFO_STRU>::iterator itHostInfo;

        for(itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end();itMapInfo++){
            if(lunid == itMapInfo->strDevLUNID){
                cmdOperate.CMD_showhostByhostGroup(itMapInfo->strHostGroupID,rlstHostInfo);
                for(itHostInfo = rlstHostInfo.begin();itHostInfo !=rlstHostInfo.end();++itHostInfo){
                    string StrIQN("");
                    string strChapname("");
                    string strCom("");
                    unsigned int fcOriscsi = 0;
                    cmdOperate.CMD_showhostport(itHostInfo->strID,rlstHostPortInfo);
                    for(ithostPort = rlstHostPortInfo.begin();ithostPort != rlstHostPortInfo.end();ithostPort++){
                        if(!ithostPort->strIQN.empty() && ithostPort->strIQN.size() > 1){
                            StrIQN += strCom + ithostPort->strIQN;
                            strChapname += strCom + ithostPort->strChapName;
                            fcOriscsi = ithostPort->uifcOriscsi;
                            strCom = ",";
                        }
                    }
                    if( fcOriscsi == 0){
                        COMMLOG(OS_LOG_INFO,"Promoted TargetGroup [%s] is being exposed to host [%s] using chapName [%s]",
                            iter->tg_key.c_str(),
                            StrIQN.c_str(),
                            strChapname.c_str());
                        print("Promoted TargetGroup [%s] is being exposed to host [%s] using chapName [%s]",
                            iter->tg_key.c_str(),
                            StrIQN.c_str(),
                            strChapname.c_str());
                    }
                    else{
                        COMMLOG(OS_LOG_INFO,"Promoted TargetGroup [%s] is being exposed to host [%s]",
                            iter->tg_key.c_str(),
                            StrIQN.c_str());
                        print("Promoted TargetGroup [%s] is being exposed to host [%s]",
                            iter->tg_key.c_str(),
                            StrIQN.c_str());
                    }
                }
                break;
            }
        }
    }

    if (!tg_groups.lst_groups_info.empty()){
        if (!all_key.empty()){
            COMMLOG(OS_LOG_INFO,"Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
            print("Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
        }
    }
    return RETURN_OK;
}

int FailOver::_outband_swap_hypermirror(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    int iRes = RETURN_ERR;
    string cmd;
    string strid;
    string hymirror_id;
    string hostid;
    string all_id = "";
    string all_key = "";
    string comma = "";
    string sn;
    string key;
    string strNFSID;
    string strsharepath;
    string lun_flag;
    string cgid;
    Log_Info TempLog;

    list<string> lst_tmp;
    list<TargetDeviceInfo>::iterator iter;
    list<Log_Info>::iterator itFLLog;

    list<MAP_INFO_STRU>::iterator itMapInfo;

    list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
    list<HOST_PORT_INFO_STRU>::iterator ithostPort;

    list<CMDHOSTINFO_STRU> rlstHostInfo;
    list<CMDHOSTINFO_STRU>::iterator itHostInfo;

    list<NFS_INFO_STRU> lstnfsInfo;
    list<NFS_INFO_STRU>::iterator itNfsInfo;

    list<DAR_HOST_INFO>::iterator itDARHostInfo;
   
    if(!tg_devices.lst_target_devices.empty()){
        lstLogInfo.clear();
        for (iter = tg_devices.lst_target_devices.begin(); iter != tg_devices.lst_target_devices.end(); ++iter){
            iRet = get_device_info(iter->target_key, sn, strid, lun_flag, hymirror_id);
            if (RETURN_OK != iRet){
                iter->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
                continue;
            }

            if (lun_flag.compare(MIRROR_LUN_TAG)){
                continue;
            }

            TempLog.strID = strid;
            TempLog.strName = sn + "." + strid;
            lstLogInfo.push_back(TempLog);
        }
        for(itFLLog = lstLogInfo.begin(); itFLLog != lstLogInfo.end();itFLLog++){
            all_id += comma + itFLLog->strID;
            all_key += comma + itFLLog->strName;
            comma = ",";
        }
        if (!lstLogInfo.empty()){
            COMMLOG(OS_LOG_INFO, "Commence: Suspending replication of [%s] on array [%s] for failover request",
                all_id.c_str(), 
                sn.c_str());
            print("Commence: Suspending replication of [%s] on array [%s] for failover request",
                all_id.c_str(), 
                sn.c_str());
            for(itFLLog = lstLogInfo.begin(); itFLLog != lstLogInfo.end();itFLLog++){
                COMMLOG(OS_LOG_INFO,"Replication session [%s] to target [%s] on array [%s] is terminated. target is now promotable",
                    itFLLog->strID.c_str(),
                    itFLLog->strName.c_str(),
                    sn.c_str());
                print("Replication session [%s] to target [%s] on array [%s] is terminated. target is now promotable",
                    itFLLog->strID.c_str(),
                    itFLLog->strName.c_str(),
                    sn.c_str());
            }
            COMMLOG(OS_LOG_INFO,"Complete:Suspension of replication of [%s] on array [%s] for failover request is now COMPLETE",
                all_id.c_str(), 
                sn.c_str());
            print("Complete:Suspension of replication of [%s] on array [%s] for failover request is now COMPLETE",
                all_id.c_str(), 
                sn.c_str());
            COMMLOG(OS_LOG_INFO, "Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(), 
                sn.c_str());
            print("Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(), 
                sn.c_str());
        }
    }

    for (iter = tg_devices.lst_target_devices.begin(); iter != tg_devices.lst_target_devices.end(); ++iter){
        iRet = get_device_info(iter->target_key, sn, strid, lun_flag, hymirror_id);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            continue;
        }
        key = sn + "." + strid;

        if (hymirror_id.empty()){
            continue;
        }
        is_Fs_by_lun(cmdOperate, strid);

        if (!isNFS){
            
            if(g_bFusionStorage){
                
                iRet = cmdOperate.CMD_unmountSlave(hymirror_id);
                if (RETURN_OK != iRet){
                    iter->err_info.code = OS_IToString(iRet);
                    COMMLOG(OS_LOG_ERROR, "unMap slave lun pair[%s] to hosts failed.", hymirror_id.c_str());
                    return iRet;
                }
            }
            else{
                iRet = _add_map(cmdOperate, *iter, lstDARHosts, rlstHostInfo, strid, OBJ_LUN);
                if (RETURN_OK != iRet){
                    iter->err_info.code = OS_IToString(iRet);
                    COMMLOG(OS_LOG_ERROR, "Map remote lun [%s] to hosts failed.", strid.c_str());
                    return iRet;
                }
            }            
        }
        
        string strTime;
        strTime = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Replica of [%s] with key [%s] on array [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());
        print("Replica of [%s] with key [%s] on array [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());
        
        iRet = _exec_hymirror_failover_out(cmdOperate, *iter, strid, hymirror_id);
        if(g_bFusionStorage){
            
            iRes = _add_map(cmdOperate, *iter, lstDARHosts, rlstHostInfo, strid, OBJ_LUN);
            if (RETURN_OK != iRes){
                iter->err_info.code = OS_IToString(iRes);
                COMMLOG(OS_LOG_ERROR, "Map remote lun [%s] to hosts failed.", strid.c_str());
                return iRes;
            }
        }
        
        if (RETURN_OK != iRet){
            continue;
        }
        iter->target_id = iter->target_key;
        iter->target_state = "read-write";
        
        iter->recoverypoint_info.rp_time = OS_Time_tToStr(OS_Now());
        format_daytime(iter->recoverypoint_info.rp_time);
        iter->recoverypoint_info.rp_id = iter->target_key;
        iter->recoverypoint_info.rp_name = "RP_" + iter->target_key;  
        
        COMMLOG(OS_LOG_INFO,"Promoted target [%s] has attributes: [r/w] [multihost]",
            key.c_str());
        print("Promoted target [%s] has attributes: [r/w] [multihost]",
            key.c_str());
        if (!isNFS){
            lstMapInfo.clear();
            if (g_bFusionStorage){
                iRet = cmdOperate.CMD_showmap(strid, lstMapInfo);
            }
            else{
                iRet = cmdOperate.CMD_showmap(lstMapInfo);
            }

            IdentityInfo tmp_identity;
            for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
                if (strid == itMapInfo->strDevLUNID){
                    tmp_identity.source_wwn = itMapInfo->strLUNWWN;
                    break;
                }
            }
            
            if (!g_bFusionStorage){
                iRet = getLunWwn(cmdOperate, tmp_identity.source_wwn, strid);

                if (RETURN_OK != iRet){
                    iter->err_info.code = OS_IToString(iRet);
                }
            }

            iter->identity_info = tmp_identity;
            
            for(itHostInfo = rlstHostInfo.begin(); itHostInfo != rlstHostInfo.end(); itHostInfo++){
                string StrIQN("");
                string strChapName("");
                string strCom("");
                unsigned int fcOriscsi = 0;
                cmdOperate.CMD_showhostport(itHostInfo->strID, rlstHostPortInfo);
                for(ithostPort = rlstHostPortInfo.begin(); ithostPort != rlstHostPortInfo.end(); ithostPort++){
                    if(!ithostPort->strIQN.empty() && ithostPort->strIQN.size() > 1){
                        StrIQN += strCom + ithostPort->strIQN ;
                        strChapName += strCom + ithostPort->strChapName ;
                        fcOriscsi = ithostPort->uifcOriscsi;
                        strCom = ",";
                    }
                }
                if(fcOriscsi == 0){
                    COMMLOG(OS_LOG_INFO,"Promoted Target [%s] is being exposed to host [%s] using chapName [%s]",
                        key.c_str(),
                        StrIQN.c_str(),
                        strChapName.c_str());
                    print("Promoted Target [%s] is being exposed to host [%s] using chapName [%s]",
                        key.c_str(),
                        StrIQN.c_str(),
                        strChapName.c_str());
                }
                else{
                    COMMLOG(OS_LOG_INFO,"Promoted Target [%s] is being exposed to host [%s]",
                        key.c_str(),
                        StrIQN.c_str());
                    print("Promoted Target [%s] is being exposed to host [%s]",
                        key.c_str(),
                        StrIQN.c_str());
                }
            }

        }
        else{
            FS_INFO_STRU stFsInfo;
            stFsInfo.strID = strid;
            iRet = cmdOperate.CMD_showfs(stFsInfo);
            if (iRet != RETURN_OK){
                iter->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "Query fs (%s) wrong.", strid.c_str());
                continue;
            }

            iRet = cmdOperate.CMD_addnfs(stFsInfo.vstoreId, strid,strNFSID,strsharepath);
            if (iRet == 1077939724){
                cmdOperate.CMD_delnfs(stFsInfo.vstoreId, strNFSID);
                iRet = cmdOperate.CMD_addnfs(stFsInfo.vstoreId, strid,strNFSID,strsharepath);
                if(iRet != RETURN_OK){
                    iter->err_info.code = OS_IToString(iRet);
                    COMMLOG(OS_LOG_ERROR, "Add nfs for fs (%s) wrong.", strid.c_str());
                    continue;
                }
            }
            iter->identity_infofs.source_NfsName = strsharepath;
            for (itDARHostInfo = lstDARHosts.begin(); itDARHostInfo != lstDARHosts.end(); itDARHostInfo++){
                if (iter->cg_accessgroups.find(itDARHostInfo->strGroup) != string::npos){
                    if (itDARHostInfo->strType == "NFS"){
                        int ret = cmdOperate.CMD_addnfsclient(stFsInfo.vstoreId, itDARHostInfo->strID, strNFSID);
                        COMMLOG(OS_LOG_ERROR, "CMD_addnfsclient ret (%d)(%s)(%s)", ret, itDARHostInfo->strID.c_str(), itDARHostInfo->strType.c_str());
                    }
                }
            }
        }
    }
    
    if(!tg_devices.lst_target_devices.empty()){
        if (!all_key.empty()){
            COMMLOG(OS_LOG_INFO, "Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
            print("Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
        }
    }
    return RETURN_OK;
}

int FailOver::_outband_swap_hypermetro(CCmdOperate &cmdOperate)
{
    int iRet = RETURN_ERR;
    string cmd;
    string strid;
    string hmpair_id;
    string hostid;
    string all_id = "";
    string all_key = "";
    string comma = "";
    string sn;
    string key;
    string lun_flag;
    string strsharepath;
    Log_Info TempLog;

    list<string> lst_tmp;
    list<TargetDeviceInfo>::iterator iter;
    list<Log_Info>::iterator itFLLog;

    list<MAP_INFO_STRU> rlstMapInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;

    list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
    list<HOST_PORT_INFO_STRU>::iterator ithostPort;

    list<CMDHOSTINFO_STRU> rlstHostInfo;
    list<CMDHOSTINFO_STRU>::iterator itHostInfo;

    list<NFS_INFO_STRU> lstnfsInfo;
    list<NFS_INFO_STRU>::iterator itNfsInfo;

    
    if(!tg_devices.lst_target_devices.empty()){
        lstLogInfo.clear();
        for (iter = tg_devices.lst_target_devices.begin(); iter != tg_devices.lst_target_devices.end(); ++iter){
            iRet = get_device_info(iter->target_key, sn, strid, lun_flag, hmpair_id);
            if (RETURN_OK != iRet){
                iter->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
                continue;
            }

            if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
                continue;
            }

            TempLog.strID = strid;
            TempLog.strName = sn + "." + strid;
            lstLogInfo.push_back(TempLog);
        }
        for(itFLLog = lstLogInfo.begin(); itFLLog != lstLogInfo.end();itFLLog++){
            all_id += comma + itFLLog->strID;
            all_key += comma + itFLLog->strName;
            comma = ",";
        }
        if (!lstLogInfo.empty()){
            COMMLOG(OS_LOG_INFO, "Commence: Suspending hypermetro of [%s] on array [%s] for failover request",
                all_id.c_str(), 
                sn.c_str());
            print("Commence: Suspending hypermetro of [%s] on array [%s] for failover request",
                all_id.c_str(), 
                sn.c_str());
            for(itFLLog = lstLogInfo.begin(); itFLLog != lstLogInfo.end();itFLLog++){
                COMMLOG(OS_LOG_INFO,"Hypermetro session [%s] to target [%s] on array [%s] is terminated. target is now promotable",
                    itFLLog->strID.c_str(),
                    itFLLog->strName.c_str(),
                    sn.c_str());
                print("Hypermetro session [%s] to target [%s] on array [%s] is terminated. target is now promotable",
                    itFLLog->strID.c_str(),
                    itFLLog->strName.c_str(),
                    sn.c_str());
            }
            COMMLOG(OS_LOG_INFO,"Complete:Suspension of hypermetro of [%s] on array [%s] for failover request is now COMPLETE",
                all_id.c_str(), 
                sn.c_str());
            print("Complete:Suspension of hypermetro of [%s] on array [%s] for failover request is now COMPLETE",
                all_id.c_str(), 
                sn.c_str());
            COMMLOG(OS_LOG_INFO, "Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(), 
                sn.c_str());
            print("Commence: Promotion of devices [%s] on array [%s] for failover request",
                all_key.c_str(), 
                sn.c_str());
        }
    }

    for (iter = tg_devices.lst_target_devices.begin(); iter != tg_devices.lst_target_devices.end(); ++iter){
        iRet = get_device_info(iter->target_key, sn, strid, lun_flag, hmpair_id);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            continue;
        }
        key = sn + "." + strid;

        if (hmpair_id.empty()){
            continue;
        }


        
        string strTime;
        strTime = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Hypermetro of [%s] with key [%s] on array [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());
        print("Hypermetro of [%s] with key [%s] on array [%s] is being promoted from Recovery Point[%s]",
            strid.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());

        
        if (HM_ISOLATIONREQUIRED_TRUE == iter->isolationRequired){
            iRet = _exec_hmpair_failover_out(cmdOperate, *iter, strid, hmpair_id);
            if (RETURN_OK != iRet){
                continue;
            }
        }
        
        iter->target_id = iter->target_key;
        iter->target_state = "read-write";
        
        iter->recoverypoint_info.rp_time = OS_Time_tToStr(OS_Now());
        format_daytime(iter->recoverypoint_info.rp_time);
        iter->recoverypoint_info.rp_id = iter->target_key;
        iter->recoverypoint_info.rp_name = "RP_" + iter->target_key;  
        
        COMMLOG(OS_LOG_INFO,"Promoted target [%s] has attributes: [r/w] [multihost]",
            key.c_str());
        print("Promoted target [%s] has attributes: [r/w] [multihost]",
            key.c_str());

        IdentityInfo tmp_identity;  
        string strHostGroupID;

        if ( g_bFusionStorage){
            lstMapInfo.clear();
            if (cmdOperate.CMD_showmap(strid, lstMapInfo) != RETURN_OK){
                COMMLOG(OS_LOG_ERROR,"query map fail,lun_id[%s]",strid.c_str());
                return RETURN_ERR;
            }
        }

        for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
            if (strid == itMapInfo->strDevLUNID){
                tmp_identity.source_wwn = itMapInfo->strLUNWWN;
                strHostGroupID = itMapInfo->strHostGroupID;
                break;
            }
        }

        
        if (!g_bFusionStorage){
            iRet = getLunWwn(cmdOperate, tmp_identity.source_wwn, strid);

            if (RETURN_OK != iRet){
                iter->err_info.code = OS_IToString(iRet);
            }
        }

        iter->identity_info = tmp_identity;
        

        for(itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end();itMapInfo++){
            if(strid == itMapInfo->strDevLUNID){
                cmdOperate.CMD_showhostByhostGroup(itMapInfo->strHostGroupID,rlstHostInfo);
                for(itHostInfo = rlstHostInfo.begin();itHostInfo !=rlstHostInfo.end();itHostInfo++){
                    string StrIQN("");
                    string strChapName("");
                    string strCom("");
                    unsigned int fcOriscsi = 0;
                    cmdOperate.CMD_showhostport(itHostInfo->strID,rlstHostPortInfo);
                    for(ithostPort = rlstHostPortInfo.begin();ithostPort != rlstHostPortInfo.end();ithostPort++){
                        if(!ithostPort->strIQN.empty() && ithostPort->strIQN.size() > 1){
                            StrIQN += strCom + ithostPort->strIQN ;
                            strChapName += strCom + ithostPort->strChapName ;
                            fcOriscsi = ithostPort->uifcOriscsi;
                            strCom = ",";
                        }
                    }
                    if(fcOriscsi == 0){
                        COMMLOG(OS_LOG_INFO,"Promoted Target [%s] is being exposed to host [%s] using chapName [%s]",
                            key.c_str(),
                            StrIQN.c_str(),
                            strChapName.c_str());
                        print("Promoted Target [%s] is being exposed to host [%s] using chapName [%s]",
                            key.c_str(),
                            StrIQN.c_str(),
                            strChapName.c_str());
                    }
                    else{
                        COMMLOG(OS_LOG_INFO,"Promoted Target [%s] is being exposed to host [%s]",
                            key.c_str(),
                            StrIQN.c_str());
                        print("Promoted Target [%s] is being exposed to host [%s]",
                            key.c_str(),
                            StrIQN.c_str());
                    }
                }
                break;
            }
        }
    }
    
    if(!tg_devices.lst_target_devices.empty()){
        if (!all_key.empty()){
            COMMLOG(OS_LOG_INFO, "Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
            print("Complete: Promotion is now complete for devices [%s] for failover request",
                all_key.c_str());
        }
    }
    return RETURN_OK;
}

int FailOver::getLunWwn(CCmdOperate &cmdOperate, string &sourceWwn, string strid)
{
    if (sourceWwn.empty()){
        int iRet = RETURN_ERR;
        COMMLOG(OS_LOG_WARN, "the lun [%s] is not in mappingview",  strid.c_str());
        LUN_INFO_STRU rstLUNInfo;
        rstLUNInfo.strID = strid;

        iRet = cmdOperate.CMD_showlun(rstLUNInfo);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "query lun [%s] info failed.", strid.c_str());
            return iRet;
        }

        sourceWwn = rstLUNInfo.strWWN;
    }

    return RETURN_OK;
}

int FailOver::_check_slave_lun_map(string& lun_id)
{
    string host_id;

    list<MAP_INFO_STRU>::iterator itMapInfo;

    if (lstMapInfo.empty()){
        return RETURN_OK;
    }

    for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
        if (itMapInfo->strDevLUNID == lun_id){
            host_id = itMapInfo->strHostLUNID;
            break;
        }
    }




    if (host_id.empty()){
        COMMLOG(OS_LOG_ERROR,"Slave LUN (%s) is not map to ESX",lun_id.c_str());

        return ERROR_SLAVELUN_NOT_MAP;
    }

    return RETURN_OK;
}

int FailOver::_get_cg_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess, bool is_nfs=false)
{
    int iRet = RETURN_ERR;

    
    if(is_nfs){
        VSTORE_PAIR_INFO_STRU vstorePairInfo;
        iRet = cmdOperate.CMD_showvstorepair_info(cgid, vstorePairInfo);
        CHECK_UNEQ(RETURN_OK,iRet);

        _convert_pair_status(vstorePairInfo.uiState, status);
        role = vstorePairInfo.isPrimary ? MIRROR_RELATION_MASTER : MIRROR_RELATION_SLAVE;
        uiResAccess = vstorePairInfo.uiSecResAccess;
    }
    else{
        GROUP_INFO_STRU stGroupInfo;
        stGroupInfo.strID = cgid;

        iRet = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
        CHECK_UNEQ(RETURN_OK,iRet);

        _convert_pair_status(stGroupInfo.uiState, status);
        _convert_relation_type(stGroupInfo.uiIsPrimary, role);
        uiResAccess = stGroupInfo.uiSecResAccess;
    }

    return RETURN_OK;
}

int FailOver::_get_cghm_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess)
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

int FailOver::_exec_failover_out(CCmdOperate& cmdOperate, string &cgid, string& status, string& role, unsigned int &uiResAccess, TargetGroupInfo& tginfo, bool is_nfs=false)
{
    string cmd;
    int iRet = RETURN_ERR;

    
    if (!is_nfs && !g_bFusionStorage){
        iRet = execSparseConsisgr(cmdOperate, cgid, RESOURCE_ACCESS_READ_WRITE);
        if(iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "execSparseConsisgr by cgid [%s]  failed, the error code is [%d]", cgid.c_str(), iRet);
            return iRet;
        }
    }

    list<HYMIRROR_INFO_STRU> lstMirrorInfo;

    
    if ((MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED == status || MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED == status) 
        && RESOURCE_ACCESS_READ_WRITE == uiResAccess){
        tginfo.war_info.code = OS_IToString(500);
        COMMLOG(OS_LOG_INFO,"%s", "failover had excute some time before.");
        print("%s", "failover had excute some time before.");
        return RETURN_OK;
    }

    
    if (status != MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED && status != MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED){
        
        iRet = _exec_split_out(cmdOperate, cgid, tginfo, is_nfs);
        CHECK_UNEQ(RETURN_OK, iRet);
    }

    
    
    if (isNFS){
        return cmdOperate.CMD_changeVstorePairSlaveLunRw(cgid, RESOURCE_ACCESS_READ_WRITE);
    }
    else{
        return cmdOperate.CMD_changeCGSlaveLunRw(cgid, RESOURCE_ACCESS_READ_WRITE);
    }
}

int FailOver::_exec_failover_hm_out(CCmdOperate& cmdOperate, string &cgid, string& status, string& role, TargetGroupInfo& tginfo)
{
    string cmd;
    int iRet = RETURN_ERR;

    list<HYPERMETROPAIR_INFO_STRU> lstHMPairInfo;

    
    if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == status){
        COMMLOG(OS_LOG_INFO,"%s", "failover had excute some time before.");
        print("%s", "failover had excute some time before.");
        return RETURN_OK;
    }


    if (HM_SLAVE_PAIR_STATUS_STR_NORMAL == status && MIRROR_RELATION_SLAVE == role){
        iRet = cmdOperate.CMD_swapconsistgrhm(cgid);
        if (RETURN_OK != iRet){
            tginfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "swap hypermetro consisgroup(%s) is failed.", cgid.c_str());
            return RETURN_ERR;
        }
    }

    
    if (status == HM_SLAVE_PAIR_STATUS_STR_NORMAL){
        
        if (HM_ISOLATIONREQUIRED_TRUE == tginfo.isolationRequired){
            iRet = _exec_split_hm_out(cmdOperate, cgid, tginfo);
            CHECK_UNEQ(RETURN_OK, iRet);
        }
        
        return RETURN_OK;
    }

    return RETURN_OK;
}

int FailOver::_exec_split_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo, bool is_nfs=false)
{
    int iRet = RETURN_ERR;
    string con_percent;
   
    list<HYMIRROR_INFO_STRU> lstMirrorInfo;

    if(is_nfs){
        iRet = cmdOperate.CMD_splitvstorepair(cgid);
    }
    else{
        
        iRet = cmdOperate.CMD_splitconsistgr(cgid);
    }

    if (RETURN_OK != iRet){
        tginfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "spliting consisgroup(%s) is failed.", cgid.c_str());
        return RETURN_ERR;
    } 

    return RETURN_OK;
}

int FailOver::_exec_split_hm_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo)
{
    int iRet = RETURN_ERR;

    
    iRet = cmdOperate.CMD_stopconsistgrhm(cgid);
    if (RETURN_OK != iRet){
        tginfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "stop hypermetro consisgroup(%s) is failed.", cgid.c_str());
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int FailOver::_get_consistency_percent_out(CCmdOperate& cmdOperate, list<HYMIRROR_INFO_STRU>& lstMirrorInfo, string& con_percent)
{
    int ret = RETURN_ERR;
    int percent = 0;
    int hundredpercent = SYNC_COMPLETE_FLAG;

    string query_percent;

    list<string>::iterator hy_id;
    list<string> lst_hy_id;
    list<HYMIRROR_INFO_STRU>::iterator itMirrorInfo;

    if (lstMirrorInfo.empty()){
        con_percent = "0";
        return RETURN_OK;
    }
  
    for (itMirrorInfo = lstMirrorInfo.begin(); itMirrorInfo != lstMirrorInfo.end(); itMirrorInfo++){
        lst_hy_id.push_back(itMirrorInfo->strID);
    }

    hy_id = lst_hy_id.begin();
    while (hy_id != lst_hy_id.end()){
        
        ret = _get_hymirror_percent_out(cmdOperate, *hy_id, query_percent);
        CHECK_UNEQ(RETURN_OK,ret);

        if ("--" == query_percent){
            ++hy_id;
            continue;
        }

        percent = atoi(query_percent.c_str());

        if (hundredpercent > percent){
            hundredpercent = percent;
        }

        ++hy_id;
    }

    con_percent = OS_IToString(hundredpercent);

    return RETURN_OK;
}

int FailOver::_get_consisgr_info_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo, bool is_nfs=false)
{
    int ret;
    string cmd;
    string tmp_lunid;
    string tmp_arrayid;
    list<HYMIRROR_INFO_STRU> lstHyMirrorInfo;
    list<HYMIRROR_INFO_STRU>::iterator itHyMirrorInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    list<string> lst_hymirrorid;
    list<string>::iterator iter;
    list<DAR_HOST_INFO>::iterator itDARHostInfo;

    tginfo.target_devices_info.lst_target_devices.clear();
    
    if (is_nfs){
        
        ret = cmdOperate.CMD_showvstorepairmember(cgid, lstHyMirrorInfo);
    }
    else{
        
        ret = cmdOperate.CMD_showconsistgrmember(cgid, lstHyMirrorInfo);
    }
    CHECK_UNEQ(RETURN_OK, ret);

    for (itHyMirrorInfo = lstHyMirrorInfo.begin(); itHyMirrorInfo != lstHyMirrorInfo.end(); itHyMirrorInfo++){
        list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
        list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;

        if(is_nfs){
            ret = cmdOperate.CMD_showhymirrorfs_all(itHyMirrorInfo->strID, lstHyMirrorLUNInfo);
        }
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(itHyMirrorInfo->strID, lstHyMirrorLUNInfo);
        }
        CHECK_UNEQ(RETURN_OK, ret);

        itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin();
        while (itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end()){
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLUNInfo->uiRelationType){
                tmp_arrayid = itHyMirrorLUNInfo->strArrayWWN;
                tmp_lunid = itHyMirrorLUNInfo->strID;
                break;
            }

            itHyMirrorLUNInfo++;
        }

        if (array_id != tmp_arrayid){
            COMMLOG(OS_LOG_ERROR, "Slave array(%s) is not current array(%s)", tmp_arrayid.c_str(), array_id);
            return ERROR_INTERNAL_PROCESS_FAIL;
        }

        TargetDeviceInfo tmp_tg;
        tmp_tg.target_id = tmp_arrayid + "_" + tmp_lunid + "_" + MIRROR_LUN_TAG + "_" + itHyMirrorInfo->strID;
        tmp_tg.target_key = tmp_tg.target_id;
        if(accessGroupDict.find(tmp_tg.target_id) != accessGroupDict.end()){
            tmp_tg.cg_accessgroups = accessGroupDict[tmp_tg.target_id];
        }
        else{
            COMMLOG(OS_LOG_ERROR, "Cannot find access group info of device(%s)", tmp_tg.target_id.c_str());
            return RETURN_ERR;
        }
         

        if(is_nfs){
            
            FS_INFO_STRU stFsInfo;
            stFsInfo.strID = tmp_lunid;
            ret = cmdOperate.CMD_showfs(stFsInfo);
            if(ret != RETURN_OK){
                
                tginfo.err_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "Query fs (%s) wrong.", tmp_lunid.c_str());
                continue;
            }

            string strNFSID;
            string strsharepath;
            ret = cmdOperate.CMD_addnfs(stFsInfo.vstoreId, tmp_lunid, strNFSID, strsharepath);
            if (ret == 1077939724){
                cmdOperate.CMD_delnfs(stFsInfo.vstoreId, strNFSID);
                ret = cmdOperate.CMD_addnfs(stFsInfo.vstoreId, tmp_lunid,strNFSID,strsharepath);
                if(ret != RETURN_OK){
                    
                    tginfo.err_info.code = OS_IToString(ret);
                    COMMLOG(OS_LOG_ERROR, "Add nfs for fs (%s) wrong.", tmp_lunid.c_str());
                    continue;
                }
            }

            
            vector<string> darHosts = split(tmp_tg.cg_accessgroups, DAR_HOST_SEPERATOR);
            for (itDARHostInfo = lstDARHosts.begin();itDARHostInfo != lstDARHosts.end(); itDARHostInfo++){
                if (itDARHostInfo->strType == "NFS"){
                    
                    if (find(darHosts.begin(), darHosts.end(), itDARHostInfo->strGroup) != darHosts.end()){                    
                        
                        ret = cmdOperate.CMD_addnfsclient(stFsInfo.vstoreId, itDARHostInfo->strID,strNFSID);
                    }
                }
            }
        }
        else{
            
            list<CMDHOSTINFO_STRU> rlstHostInfo;
            ret = _add_map(cmdOperate, tmp_tg, lstDARHosts, rlstHostInfo, tmp_lunid, OBJ_LUN);
            if (RETURN_OK != ret){
                tmp_tg.err_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "Map remote lun [%s] to hosts failed.", tmp_lunid.c_str());
                return ret;
            }
        }

        tmp_tg.target_state = "read-write";

        
        if(is_nfs){
            IdentityInfoFS tmp_identityfs;
            FS_INFO_STRU fsInfo;
            fsInfo.strID = itHyMirrorLUNInfo->strID;
            ret = cmdOperate.CMD_showfs(fsInfo);
            CHECK_UNEQ(RETURN_OK, ret);
            tmp_identityfs.source_NfsName = fsInfo.strPath;
            tmp_tg.identity_infofs = tmp_identityfs;
        }
        else{
            lstMapInfo.clear();
            ret = cmdOperate.CMD_showmap(lstMapInfo);
            CHECK_UNEQ(RETURN_OK,ret);

            IdentityInfo tmp_identity;
            for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
                if (tmp_lunid == itMapInfo->strDevLUNID){
                    tmp_identity.source_wwn = itMapInfo->strLUNWWN;
                    break;
                }
            }

            
            if (!g_bFusionStorage){
                ret = getLunWwn(cmdOperate, tmp_identity.source_wwn, tmp_lunid);

                if (RETURN_OK != ret){
                    COMMLOG(OS_LOG_ERROR, "get lun wwn [%s] info failed.", tmp_lunid.c_str());
                    return ret;
                }
            }

            tmp_tg.identity_info = tmp_identity;
        }    
        
        tginfo.target_devices_info.lst_target_devices.push_back(tmp_tg);
    }

    tginfo.cg_id = tginfo.tg_key;
    tginfo.status = "read-write";
    tginfo.repoint_info.rp_time = OS_Time_tToStr(OS_Now());
    format_daytime(tginfo.repoint_info.rp_time);
    tginfo.repoint_info.rp_id = tginfo.tg_key;
    tginfo.repoint_info.rp_name = "RP_" + tginfo.tg_key;

    return RETURN_OK;
}

int FailOver::_get_consisgrhm_info_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo)
{
    int ret;
    string cmd;
    string tmp_lunid;
    string tmp_arrayid;
    list<HYPERMETROPAIR_INFO_STRU> lstHMPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHMPairInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    list<string> lst_hmpairid;
    list<string>::iterator iter;

    tginfo.target_devices_info.lst_target_devices.clear();

    
    ret = cmdOperate.CMD_showHyperMetroPair_all(lstHMPairInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    for (itHMPairInfo = lstHMPairInfo.begin(); itHMPairInfo != lstHMPairInfo.end(); itHMPairInfo++){
        list<HYPERMETRO_LF_INFO_STRU> lstHMPairLUNInfo;
        list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLUNInfo;

        if (cgid != itHMPairInfo->strGroupID){
            continue;
        }

        ret = cmdOperate.CMD_showhypermetrolun_all(itHMPairInfo->strID, lstHMPairLUNInfo);
        CHECK_UNEQ(RETURN_OK, ret);

        itHMPairLUNInfo = lstHMPairLUNInfo.begin();
        while (itHMPairLUNInfo != lstHMPairLUNInfo.end()){
            if (HM_LUN_RELATION_MASTER == itHMPairLUNInfo->uiRelationType){
                tmp_arrayid = itHMPairLUNInfo->strArrayWWN;
                tmp_lunid = itHMPairLUNInfo->strID;
                break;
            }

            itHMPairLUNInfo++;
        }

        if (array_id != tmp_arrayid){
            COMMLOG(OS_LOG_ERROR, "Master array(%s) is not current array(%s)", tmp_arrayid.c_str(), array_id);
            return ERROR_INTERNAL_PROCESS_FAIL;
        }

        TargetDeviceInfo tmp_tg;
        tmp_tg.target_id = tmp_arrayid + "_" + tmp_lunid + "_" + HYPERMETRO_LUN_TAG + "_" + itHMPairInfo->strID;
        tmp_tg.target_key = tmp_tg.target_id;
        tmp_tg.target_state = "read-write";

        IdentityInfo tmp_identity;

        for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
            if (tmp_lunid == itMapInfo->strDevLUNID){
                tmp_identity.source_wwn = itMapInfo->strLUNWWN;
                break;
            }
        }

        
        if (!g_bFusionStorage){
            ret = getLunWwn(cmdOperate, tmp_identity.source_wwn, tmp_lunid);

            if (RETURN_OK != ret){
                COMMLOG(OS_LOG_ERROR, "get wwn [%s] info failed.", tmp_lunid.c_str());
                return ret;
            }
        }

        tmp_tg.identity_info = tmp_identity;
        tginfo.target_devices_info.lst_target_devices.push_back(tmp_tg);
    }

    tginfo.cg_id = tginfo.tg_key;
    tginfo.status = "read-write";
    tginfo.repoint_info.rp_time = OS_Time_tToStr(OS_Now());
    format_daytime(tginfo.repoint_info.rp_time);
    tginfo.repoint_info.rp_id = tginfo.tg_key;
    tginfo.repoint_info.rp_name = "RP_" + tginfo.tg_key;

    return RETURN_OK;
}

int FailOver::_exec_hymirror_failover_out(CCmdOperate& cmdOperate, TargetDeviceInfo &targetinfo,string& lun_id,string& hymirror_id)
{
    string g_slavelun;
    string g_peer_sn;
    string status;
    string key;
    string lstLunname;
    string timepoint;
    int iRet = RETURN_ERR;
    string cmd;
    RESOURCE_ACCESS_E secLunrw = RESOURCE_ACCESS_NOACCESS;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;
    list<Log_Info>::iterator itLunLog;

    
    if (isNFS){
        iRet = cmdOperate.CMD_showhymirrorbyfs(lun_id, lstHyMirrorLUNInfo);
    }
    else{
        iRet = cmdOperate.CMD_showhymirrorlun_all(hymirror_id, lstHyMirrorLUNInfo);
    }
    if (RETURN_OK != iRet){
        targetinfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "getting status of hymirror(%s) is failed.", hymirror_id.c_str());
        return RETURN_ERR;
    }

    
    
    if (!isNFS && !g_bFusionStorage){
        iRet = execSparseHymirror(cmdOperate, lun_id, hymirror_id, RESOURCE_ACCESS_READ_WRITE);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "exec execSparseHymirror for lun_id[%s] failed, error code id [%d]", lun_id.c_str(), iRet);
            return RETURN_ERR;
        }
    }
    
    
    for (itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin(); itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end(); itHyMirrorLUNInfo++){
        if (isNFS){
            hymirror_id = itHyMirrorLUNInfo->strMirrorID;
        }

        if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLUNInfo->uiRelationType){
            _convert_pair_status(itHyMirrorLUNInfo->uiPairStatus, status);
            g_slavelun = itHyMirrorLUNInfo->strID;
            g_peer_sn = itHyMirrorLUNInfo->strArrayWWN;
            secLunrw = (RESOURCE_ACCESS_E)itHyMirrorLUNInfo->uiSecResAccess;
            timepoint = itHyMirrorLUNInfo->strSyncStartTime;
            break;
        }

    }

    
    
    if ((status == MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED || status == MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED) 
        && RESOURCE_ACCESS_READ_WRITE == secLunrw){
        targetinfo.war_info.code = OS_IToString(500);
        COMMLOG(OS_LOG_INFO,"%s", "failover had excute some time before.");
        print("%s", "failover had excute some time before.");
        return RETURN_OK;
    }

    
    if (status != MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED && status != MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED){
        
        iRet = _exec_hymirror_split_out(cmdOperate, hymirror_id, targetinfo);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "_exec_hymirror_split_out is failed, code is [%d].", iRet);
            return RETURN_ERR;
        }
    }

    
    
    if (isNFS){
        return cmdOperate.CMD_changeSlaveFsRw(hymirror_id,RESOURCE_ACCESS_READ_WRITE);
    }

    return cmdOperate.CMD_changeSlaveLunRw(hymirror_id, RESOURCE_ACCESS_READ_WRITE);
}

int FailOver::_exec_hmpair_failover_out(CCmdOperate& cmdOperate, TargetDeviceInfo &targetinfo,string& lun_id, string& hmpairid)
{
    string status;
    int iRet = RETURN_ERR;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLUNInfo;
    list<Log_Info>::iterator itLunLog;

    
    iRet = cmdOperate.CMD_showhypermetrolun_all(hmpairid, lstHMPairLUNInfo);
    if (RETURN_OK != iRet){
        targetinfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "getting status of hmpairid(%s) is failed.", hmpairid.c_str());
        return RETURN_ERR;
    }

    
    for (itHMPairLUNInfo = lstHMPairLUNInfo.begin(); itHMPairLUNInfo != lstHMPairLUNInfo.end(); itHMPairLUNInfo++){
        if (MIRROR_LUN_RELATION_SLAVE == itHMPairLUNInfo->uiRelationType){
            _convert_hmpair_status(itHMPairLUNInfo->uiPairStatus, status);
            break;
        }
    }

    
    if (status == HM_SLAVE_PAIR_STATUS_STR_PAUSE) {
        COMMLOG(OS_LOG_INFO,"%s", "failover had excute some time before.");
        print("%s", "failover had excute some time before.");
        return RETURN_OK;
    }

    
    for (itHMPairLUNInfo = lstHMPairLUNInfo.begin(); itHMPairLUNInfo != lstHMPairLUNInfo.end(); itHMPairLUNInfo++){
        if (array_id == itHMPairLUNInfo->strArrayWWN && lun_id == itHMPairLUNInfo->strID){
            if (MIRROR_LUN_RELATION_SLAVE == itHMPairLUNInfo->uiRelationType){
                iRet = cmdOperate.CMD_swaphypermetro(hmpairid);
                if (RETURN_OK != iRet){
                    targetinfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
                    COMMLOG(OS_LOG_ERROR, "CMD_swaphypermetro is failed, code is [%d].", iRet);
                    return RETURN_ERR;
                }
            }
        }
    }

    
    if (status == HM_SLAVE_PAIR_STATUS_STR_NORMAL){
        
        iRet = _exec_hmpair_split_out(cmdOperate, hmpairid, targetinfo);
        if (RETURN_OK != iRet){
            targetinfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
            COMMLOG(OS_LOG_ERROR, "_exec_hmpair_split_out is failed, code is [%d].", iRet);
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

int FailOver::_exec_hymirror_split_out(CCmdOperate& cmdOperate, string& hymirror_id, TargetDeviceInfo& targetInfo)
{
    int iRet = RETURN_ERR;
    string g_slavelun;
    string g_peer_sn;
    string status;
    string percent;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;
  

    
    if (isNFS){
        iRet = cmdOperate.CMD_showhymirrorbyfs(hymirror_id, lstHyMirrorLUNInfo,true);
    }
    else{
        iRet = cmdOperate.CMD_showhymirrorlun_all(hymirror_id, lstHyMirrorLUNInfo);
    }
    if (RETURN_OK != iRet){
        targetInfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "getting status of hymirror(%s) is failed.", hymirror_id.c_str());
        return RETURN_ERR;
    }
    
    
    for (itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin(); itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end(); itHyMirrorLUNInfo++){
        if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLUNInfo->uiRelationType){
            _convert_pair_status(itHyMirrorLUNInfo->uiPairStatus, status);
            g_slavelun = itHyMirrorLUNInfo->strID;
            g_peer_sn = itHyMirrorLUNInfo->strArrayWWN;
            break;
        }
    }

    
    iRet = cmdOperate.CMD_splithymirror(hymirror_id, g_peer_sn, g_slavelun);
    if (RETURN_OK != iRet){
        targetInfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "spliting hymirror(%s) is failed.", hymirror_id.c_str());
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int FailOver::_exec_hmpair_split_out(CCmdOperate& cmdOperate, string& hmpairid, TargetDeviceInfo& targetInfo)
{
    int iRet = RETURN_ERR;
    string g_slavelun;
    string g_peer_sn;
    string status;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLUNInfo;
  

    
    iRet = cmdOperate.CMD_showhypermetrolun_all(hmpairid, lstHMPairLUNInfo);

    if (RETURN_OK != iRet){
        targetInfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "getting status of hmpairid(%s) is failed.", hmpairid.c_str());
        return RETURN_ERR;
    }
    
    
    for (itHMPairLUNInfo = lstHMPairLUNInfo.begin(); itHMPairLUNInfo != lstHMPairLUNInfo.end(); itHMPairLUNInfo++){
        if (MIRROR_LUN_RELATION_SLAVE == itHMPairLUNInfo->uiRelationType){
            _convert_hmpair_status(itHMPairLUNInfo->uiPairStatus, status);
            g_slavelun = itHMPairLUNInfo->strID;
            g_peer_sn = itHMPairLUNInfo->strArrayWWN;
            break;
        }
    }

    
    iRet = cmdOperate.CMD_disablehcpair(hmpairid, g_peer_sn, g_slavelun);
    if (RETURN_OK != iRet){
        targetInfo.err_info.code = OS_IToString(ERROR_INTERNAL_PROCESS_FAIL);
        COMMLOG(OS_LOG_ERROR, "split hmpair(%s) is failed.", hmpairid.c_str());
        return RETURN_ERR;
    }

    return RETURN_OK;
}
