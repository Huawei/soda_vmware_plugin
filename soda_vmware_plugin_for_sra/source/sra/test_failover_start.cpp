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

#include "test_failover_start.h"
#include <algorithm>

TestFailoverStart::TestFailoverStart() : SraBasic()
{
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
};

TestFailoverStart::~TestFailoverStart()
{

};

int test_failover_start(XmlReader &reader)
{
    TestFailoverStart test_fo_start;
    COMMLOG(OS_LOG_INFO, "%s", "TestFailoverStart begin.");
    print("%s", "TestFailoverStart begin.");
    if (RETURN_OK != test_fo_start.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "failed to test failover start.");
        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "TestFailoverStart end.");
    print("%s", "TestFailoverStart end.");
    return RETURN_OK;
}

int TestFailoverStart::_read_command_para(XmlReader &reader)
{
    int i = 0, j = 0;
    int tg_count = 0;
    int device_count = 0;
    int accessgroup_count = 0;
    int accessgroups_count = 0;
    int accessinitiator_count = 0;

    if (!reader.get_string("/Command/TestFailoverStartParameters/ArrayId", array_id, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    CHECK_FALSE(reader.get_string("/Command/TestFailoverStartParameters/ArrayId", input_array_id, 0), RETURN_ERR);

    tg_count = reader.get_count("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup");

    for (i = 0; i < tg_count; i++){
        char tg_id[LENGTH_COMMON] = {0};
        TargetGroupInfo tg_info;

        (void)reader.get_string("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup/@key", tg_id, i);

        tg_info.tg_key = tg_id;
        tg_groups.lst_groups_info.push_back(tg_info);

        
        device_count = reader.get_count("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice", i, 2);
        
        for(j = 0; j < device_count; j++){
            char target_key[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/@key", target_key, j);
            
            accessgroups_count = reader.get_count("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/AccessGroups/AccessGroup",j,2);
            
            string cg_accessgroups = "";
            for (int k = 0; k < accessgroups_count; k++){
                char group_id[LENGTH_COMMON] = {0};
                (void)reader.get_string("/Command/TestFailoverStartParameters/TargetGroups/TargetGroup/TargetDevices/TargetDevice/AccessGroups/AccessGroup/@id", group_id, k,j,2);
                
                cg_accessgroups = cg_accessgroups + group_id + DAR_HOST_SEPERATOR;
            }
            COMMLOG(OS_LOG_INFO, "AccessGroups of TargetDevice[%s] is [%s].", target_key, cg_accessgroups.c_str());
            accessGroupDict[target_key] = cg_accessgroups;
        }
    }

    device_count = reader.get_count("/Command/TestFailoverStartParameters/TargetDevices/TargetDevice");

    for (i = 0; i < device_count; i++){
        char target_key[LENGTH_COMMON] = {0};

        TargetDeviceInfo tg_device_info;

        (void)reader.get_string("/Command/TestFailoverStartParameters/TargetDevices/TargetDevice/@key", target_key, i);
        accessgroups_count = reader.get_count("/Command/TestFailoverStartParameters/TargetDevices/TargetDevice/AccessGroups/AccessGroup",i,2);
        for (j = 0; j < accessgroups_count; j++){
            char group_id[LENGTH_COMMON] = {0};
            (void)reader.get_string("/Command/TestFailoverStartParameters/TargetDevices/TargetDevice/AccessGroups/AccessGroup/@id", group_id, j,i,2);
            tg_device_info.cg_accessgroups = tg_device_info.cg_accessgroups + group_id + DAR_HOST_SEPERATOR;
        }
        tg_device_info.target_key = target_key;
        tg_devices.lst_target_devices.push_back(tg_device_info);
    }

    
    accessgroup_count = reader.get_count("/Command/TestFailoverStartParameters/AccessGroups/AccessGroup");
    for(j = 0; j < accessgroup_count; j++){
        char group_id[LENGTH_COMMON] = {0};
        (void)reader.get_string("/Command/TestFailoverStartParameters/AccessGroups/AccessGroup/@id", group_id, j);
        accessinitiator_count = reader.get_count("/Command/TestFailoverStartParameters/AccessGroups/AccessGroup/Initiator",j,1);
        for (i = 0; i < accessinitiator_count; i++){
            char initiator_id[LENGTH_COMMON] = {0};
            char initiator_type[LENGTH_COMMON] = {0};
            DAR_HOST_INFO tg_dar_host_info;
            (void)reader.get_string("/Command/TestFailoverStartParameters/AccessGroups/AccessGroup/Initiator/@id", initiator_id, i,j,1);
            (void)reader.get_string("/Command/TestFailoverStartParameters/AccessGroups/AccessGroup/Initiator/@type", initiator_type, i,j,1);
            tg_dar_host_info.strID = initiator_id;
            tg_dar_host_info.strType = initiator_type;
            tg_dar_host_info.strGroup = group_id;
            lstDARHosts.push_back(tg_dar_host_info);
        }
    }
    return RETURN_OK;
}

int TestFailoverStart::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "TestFailoverStart::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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

int TestFailoverStart::_active_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device)
{
    if (g_bFusionStorage){
        return RETURN_ERR; 
    }
    int ret = RETURN_ERR;

    ret = cmdOperate.CMD_activehyimg(target_device.snapshot_info.snap_id);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);
        COMMLOG(OS_LOG_ERROR, "failed to active snapshot(%s)", target_device.snapshot_info.snap_id.c_str());
    }

    return ret;
}

int TestFailoverStart::_recreate_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device)
{
    int ret = RETURN_ERR;

    ret = cmdOperate.CMD_recreatehyimg(target_device.snapshot_info.snap_id);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);      
        COMMLOG(OS_LOG_ERROR, "failed to recreate snapshot(%s)", target_device.snapshot_info.snap_id.c_str());
    }

    return ret;
}

int TestFailoverStart::_get_snapshot_info_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device, int& errorCode)
{
    int ret = RETURN_ERR;
    list<string> lst_snapshot_ids ;
    string strActiveTime;
    list<HYIMAGE_INFO_STRU> rlstHyImageInfo;
    list<HYIMAGE_INFO_STRU>::iterator itHyImageInfo;
    string snapshot_name = VSSRA_SNAPSHOT_PREFIX + strid;

    if (g_bFusionStorage){
        string lun_name = snapshot_name + VSSRA_CLONE_LUN;
        LUN_INFO_STRU rstLUNInfo;
        cmdOperate.CMD_showlunByname(lun_name, rstLUNInfo);
        if (lun_name != rstLUNInfo.strName){
            COMMLOG(OS_LOG_ERROR, "failed to find the lun[%s]", lun_name.c_str());
            return RETURN_ERR;
        }

        target_device.snapshot_info.snap_id = rstLUNInfo.strID;
        target_device.snapshot_info.recoverypoint_info.rp_id = "rp_" + rstLUNInfo.strID;
        target_device.snapshot_info.recoverypoint_info.rp_name = "rp_" + rstLUNInfo.strName;
        strActiveTime = rstLUNInfo.strCreateTime;
        format_daytime(strActiveTime);
        target_device.snapshot_info.recoverypoint_info.rp_time = strActiveTime;
        return RETURN_OK;
    }

    if (target_device.isNfs){
        
        ret = cmdOperate.CMD_showhyimgoffs(clonefsid,rlstHyImageInfo);
    }
    else{
        ret = cmdOperate.CMD_showhyimgoflun(strid,rlstHyImageInfo);
    }
    if (RETURN_OK != ret){
        COMMLOG(OS_LOG_ERROR, "failed to send cmd to get snapshot info of lun(%s) retcode(%d).", strid.c_str(), ret);
        return ret;
    }

    for(itHyImageInfo = rlstHyImageInfo.begin();itHyImageInfo != rlstHyImageInfo.end();itHyImageInfo++){
        if (target_device.isNfs){
            lst_snapshot_ids.push_back(clonefsid);
            strActiveTime = itHyImageInfo->strTimeStamp;
            format_daytime(strActiveTime);
            break;
        }
        else{
            if (snapshot_name == itHyImageInfo->strName){
                lst_snapshot_ids.push_back(itHyImageInfo->strID);
                strActiveTime = itHyImageInfo->strTimeStamp;
                format_daytime(strActiveTime);
            }
        }
        
    }

    if (lst_snapshot_ids.empty()){
        
        string desc = "failed to create snapshot:" + snapshot_name + " for target device id:" + strid;
        string hint = "Please make sure there is no snapshot:" + snapshot_name + " built for other device in array."; 

        target_device.err_info.desc = desc;
        target_device.err_info.hint = hint;

        COMMLOG(OS_LOG_ERROR, "failed to get snapshot(%s) of lun(%s).", snapshot_name.c_str(), strid.c_str());
        return RETURN_ERR;
    }

    target_device.snapshot_info.snap_id = lst_snapshot_ids.front();
    target_device.snapshot_info.recoverypoint_info.rp_id = "rp_" + lst_snapshot_ids.front();
    target_device.snapshot_info.recoverypoint_info.rp_name = "rp_" + lst_snapshot_ids.front();
    target_device.snapshot_info.recoverypoint_info.rp_time = strActiveTime;

    
    if (RETURN_REPEAT_NAME == errorCode){
        return errorCode;
    }

    return RETURN_OK;
}

int TestFailoverStart::_get_active_time_out(CCmdOperate& cmdOperate,HYIMAGE_INFO_STRU& stHyImgInfo, string& time_stamp)
{
    int ret = RETURN_ERR;

    ret = cmdOperate.CMD_showhyimginfo(stHyImgInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    time_stamp = stHyImgInfo.strTimeStamp;
    if (time_stamp == "--"){
        COMMLOG(OS_LOG_WARN,"The snapshot(%s) is not activate time", stHyImgInfo.strID.c_str());

        return ERROR_ACTIVE_SNAPSHOT;
    }

    format_daytime(time_stamp);

    return RETURN_OK;
}

void TestFailoverStart::_operate_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device)
{
    int ret = RETURN_ERR;
    string snapshot_name;
    string active_time;
    string status;
    string testname;
    list<string> lst_snapshot_ids;
    list<string> lst_token;
    string LFid;
    string key;
    string g_peer_sn;
    string strNFSID;
    string strsharepath;
    string stretched;
    string arrayid;
    string lun_flag;
    string pairid;

    HYMIRROR_INFO_STRU rstHyMirrorInfo;
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLUNInfo;

    ret = get_device_info(target_device.target_key, arrayid, strid, lun_flag, pairid);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);
        COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", target_device.target_key.c_str());
    }
    key = arrayid + "." + strid;
    hyper_id = pairid;

    
    if (g_bstretch && HYPERMETRO_LUN_TAG == lun_flag){
        if (!hyper_id.empty()){
            ret = cmdOperate.CMD_showhypermetrolun_all(hyper_id,lstHMPairLUNInfo);
            if (RETURN_OK != ret){
                return;
            }
            for (itHMPairLUNInfo = lstHMPairLUNInfo.begin(); itHMPairLUNInfo != lstHMPairLUNInfo.end(); itHMPairLUNInfo++){
                if (MIRROR_LUN_RELATION_SLAVE == itHMPairLUNInfo->uiRelationType){
                    _convert_hmpair_status(itHMPairLUNInfo->uiPairStatus, status);
                    LFid = itHMPairLUNInfo->strID;
                    testname = itHMPairLUNInfo->strName;
                    g_peer_sn = itHMPairLUNInfo->strArrayWWN;
                    stretched = target_device.stretched;
                    break;
                }
            }
        }
    }

    if (MIRROR_LUN_TAG == lun_flag){
        if (target_device.isNfs){
            ret = cmdOperate.CMD_showhymirrorbyfs(strid, lstHyMirrorLUNInfo);
            if (RETURN_OK != ret){
                return;
            }
        }
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(hyper_id,lstHyMirrorLUNInfo);
            if (RETURN_OK != ret){
                return;
            }
        }
        
        for (itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin(); itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end(); itHyMirrorLUNInfo++){
            if (isNFS){
                hyper_id = itHyMirrorLUNInfo->strMirrorID;
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLUNInfo->uiRelationType){
                _convert_pair_status(itHyMirrorLUNInfo->uiPairStatus, status);
                LFid = itHyMirrorLUNInfo->strID;
                testname = itHyMirrorLUNInfo->strName;
                g_peer_sn = itHyMirrorLUNInfo->strArrayWWN;
                break;
            }
        }
    }

    if ((LFid.length() == 0) || (LFid != strid)){
        if (LFid != strid){
            COMMLOG(OS_LOG_ERROR, "the LUN ID is not match, please check the pair direction!.");
        }

        return;
    }
    snapshot_name = VSSRA_SNAPSHOT_PREFIX + strid;
    target_device.testname = key;
    
    COMMLOG(OS_LOG_INFO, "Commence: Temporary Snapshot Copy of [%s] is being created for testFailoverStart request.",
        key.c_str());

    
    if (target_device.isNfs){
        FS_INFO_STRU localFSInfo;
        localFSInfo.strID = strid;
        ret = cmdOperate.CMD_showfs(localFSInfo);
        if (RETURN_OK != ret){
            target_device.err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "failed to show fs(%s) info", strid.c_str());
            return;
        }

        CLONEFS_INFO_STRU clonefsInfo;
        ret = cmdOperate.CMD_createCloneFS(localFSInfo.vstoreId, snapshot_name, strid, clonefsInfo);

        

        if (RETURN_OK != ret){
            target_device.err_info.code = OS_IToString(ret);

            COMMLOG(OS_LOG_ERROR, "failed to create clonefs(%s) of fs(%s), the error code is (%s)",
                snapshot_name.c_str(), target_device.target_key.c_str(), OS_IToString(ret).c_str());
            return;
        }

        clonefsid = clonefsInfo.strID;

        
        ret = _get_snapshot_info_out(cmdOperate,target_device, ret);
        target_device.snapshot_info.vstore_id = localFSInfo.vstoreId;

        if (RETURN_OK != ret){
            target_device.err_info.code = OS_IToString(ret);
            return;
        }
    }
    else{
        if (g_bFusionStorage){
            LUN_INFO_STRU rstLUNInfo;
            rstLUNInfo.strID = strid;
            cmdOperate.CMD_showlun(rstLUNInfo);
            ret = cmdOperate.CMD_createhyimg(snapshot_name, rstLUNInfo.strName);

            if (ret != RETURN_OK){
                target_device.err_info.code = OS_IToString(ret);

                COMMLOG(OS_LOG_ERROR, "failed to create snapshot(%s) of lun(%s), the error code is (%s)",
                    snapshot_name.c_str(), target_device.target_key.c_str(), OS_IToString(ret).c_str());
                return;
            }

            int create_lun = cmdOperate.CMD_cloneSnap2Lun(snapshot_name + VSSRA_CLONE_LUN,snapshot_name);
            if (create_lun != RETURN_OK){
                target_device.err_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_ERROR, "create lun by snapname[%s] failed", snapshot_name.c_str());
                return ;
            }
            ret = _get_snapshot_info_out(cmdOperate,target_device, ret);
            if (ret != RETURN_OK){
               COMMLOG(OS_LOG_ERROR, "get activetime fail");
            }
            return;
        }
        else{
            ret = cmdOperate.CMD_createhyimg(snapshot_name, strid);
        }

        
        if (RETURN_OK != ret){
            target_device.err_info.code = OS_IToString(ret);

            COMMLOG(OS_LOG_ERROR, "failed to create snapshot(%s) of lun(%s), the error code is (%s)",
                snapshot_name.c_str(), target_device.target_key.c_str(), OS_IToString(ret).c_str());
            return;
        }
    
        
        ret = _get_snapshot_info_out(cmdOperate,target_device, ret);

        
        if (RETURN_REPEAT_NAME == ret){
            if (target_device.snapshot_info.recoverypoint_info.rp_time == ""){
                ret = _active_snapshot_out(cmdOperate,target_device);
                CHECK_UNEQRTN(RETURN_OK,ret);
            }    
        }
        
        else if (RETURN_OK == ret){
            
            ret = _active_snapshot_out(cmdOperate,target_device);
            CHECK_UNEQRTN(RETURN_OK,ret);
        }
        else{
            COMMLOG(OS_LOG_ERROR, "failed to query snapshot(%s) of lun(%s)",
                snapshot_name.c_str(), strid.c_str());
        }

        OS_Sleep(2000);

        HYIMAGE_INFO_STRU stHyImgInfo;
        if (g_bFusionStorage){
            stHyImgInfo.strName = snapshot_name;
        }
        else{
            stHyImgInfo.strID = target_device.snapshot_info.snap_id;
        }

        ret = _get_active_time_out(cmdOperate, stHyImgInfo, active_time);
        if (RETURN_OK != ret){
            target_device.err_info.code = OS_IToString(ret);
            return;
        }
        target_device.snapshot_info.recoverypoint_info.rp_time = active_time;
    }


    
    COMMLOG(OS_LOG_INFO, "Replica of [%s] with key [%s] is being cloned from Recovery Point [%s]",
        testname.c_str(),
        key.c_str(),
        target_device.snapshot_info.recoverypoint_info.rp_time.c_str());
    return;
}

int TestFailoverStart::_map_snapshot_to_host_out(CCmdOperate& cmdOperate,TargetDevicesInfo& target_devices)
{
    int ret = RETURN_ERR;
    list<string> lst_token;
    list<CMDHOSTINFO_STRU> lstHosts;
    list<TargetDeviceInfo>::iterator iter_tg_device;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    
    list<DAR_HOST_INFO>::iterator itDARHostInfo;
    string snapname;
    string lun_flag;
    string pairid;
    string strNFSID;
    string strsharepath;

    iter_tg_device = target_devices.lst_target_devices.begin();
    while (iter_tg_device != target_devices.lst_target_devices.end()){
        if (iter_tg_device->err_info.hasError()){
            ++iter_tg_device;
            continue;
        }

        if (iter_tg_device->isNfs){
            
            ret = cmdOperate.CMD_addnfs(iter_tg_device->snapshot_info.vstore_id, iter_tg_device->snapshot_info.snap_id,strNFSID,strsharepath);
            if (ret == 1077939724){
                ret = cmdOperate.CMD_delnfs(iter_tg_device->snapshot_info.vstore_id, strNFSID);
                if(ret != RETURN_OK){
                    COMMLOG(OS_LOG_ERROR, "CMD_delnfs(%s, %s) ret (%d)",iter_tg_device->snapshot_info.vstore_id.c_str(), strNFSID.c_str(), ret);
                    return ret;
                }
                ret = cmdOperate.CMD_addnfs(iter_tg_device->snapshot_info.vstore_id, iter_tg_device->snapshot_info.snap_id,strNFSID,strsharepath);
            }
            CHECK_UNEQ(RETURN_OK,ret);
            COMMLOG(OS_LOG_INFO, "CMD_addnfs ret (%d)",ret);
            iter_tg_device->snapshot_info.snap_fspath = strsharepath;
            
            
            vector<string> darHosts = split(iter_tg_device->cg_accessgroups, DAR_HOST_SEPERATOR);
            for (itDARHostInfo = lstDARHosts.begin();itDARHostInfo != lstDARHosts.end(); itDARHostInfo++){
                if (itDARHostInfo->strType == "NFS"){
                    
                    if (find(darHosts.begin(), darHosts.end(), itDARHostInfo->strGroup) != darHosts.end()){                    
                        
                        ret = cmdOperate.CMD_addnfsclient(iter_tg_device->snapshot_info.vstore_id, itDARHostInfo->strID,strNFSID);
                    }
                }
            }
        }
        else{
            
            if (!g_bFusionStorage){
                ret = _and_map_out(cmdOperate, *iter_tg_device);
                CHECK_UNEQ(RETURN_OK, ret);
            }
            else{
                ret = _map_snapshot(cmdOperate, *iter_tg_device);
                CHECK_UNEQ(RETURN_OK, ret);
            }
        }
        ++iter_tg_device;
    }

    return RETURN_OK;
}

int TestFailoverStart::_get_snapshot_wwn_out(CCmdOperate& cmdOperate,TargetDevicesInfo& target_devices)
{
    int ret = RETURN_ERR;
    string device_wwn;
    list<TargetDeviceInfo>::iterator iter_tg_device;
    list<string> device_wwns;
    list<MAP_INFO_STRU> rlstMapInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    list<NFS_INFO_STRU> lstnfsInfo;
    list<NFS_INFO_STRU>::iterator itNfsInfo;
    list<string> lst_token;
    string strfsid;


    
    if (!g_bFusionStorage){
        ret = cmdOperate.CMD_showmap(rlstMapInfo);
        CHECK_UNEQ(RETURN_OK, ret);
    }
   
    iter_tg_device = target_devices.lst_target_devices.begin();
    while (iter_tg_device != target_devices.lst_target_devices.end()){
        if (iter_tg_device->err_info.hasError()){
            ++iter_tg_device;
            continue;
        }
        if (true == iter_tg_device->isNfs){
            ++iter_tg_device;
            continue;
        }

        if (g_bFusionStorage){
            rlstMapInfo.clear();
            string arrayid;
            string lun_flag;
            string pairid;
            ret = get_device_info(iter_tg_device->target_key, arrayid, strid, lun_flag, pairid);
            LUN_INFO_STRU rstLUNInfo;
            cmdOperate.CMD_showlunByname(VSSRA_SNAPSHOT_PREFIX + strid + VSSRA_CLONE_LUN, rstLUNInfo);
            ret = cmdOperate.CMD_showmap(rstLUNInfo.strID,rlstMapInfo);
            CHECK_UNEQ(RETURN_OK, ret);

            for (itMapInfo = rlstMapInfo.begin(); itMapInfo != rlstMapInfo.end(); itMapInfo++){
                if (rstLUNInfo.strID == itMapInfo->strDevLUNID){
                    device_wwn = itMapInfo->strLUNWWN;
                    break;
                }
            }
        }
        
        if (!g_bFusionStorage){
            for (itMapInfo = rlstMapInfo.begin(); itMapInfo != rlstMapInfo.end(); itMapInfo++){
                if (iter_tg_device->snapshot_info.snap_id == itMapInfo->strDevLUNID){
                    device_wwn = itMapInfo->strLUNWWN;
                    break;
                }
            }
        }
      

        if (device_wwn.empty()){
            iter_tg_device->err_info.code = OS_IToString(ERROR_GET_SNAPSHOT_WWN);
            COMMLOG(OS_LOG_ERROR, "failed to get wwn of snapshot(%s)", iter_tg_device->snapshot_info.snap_id.c_str());
        }
        else{
            iter_tg_device->snapshot_info.snap_wwn = device_wwn;
        }

        ++iter_tg_device;
    }

    return RETURN_OK;
}

void TestFailoverStart::_operate_snapshots_out(CCmdOperate& cmdOperate,TargetDevicesInfo& target_devices)
{
    list<TargetDeviceInfo>::iterator iter_tg_device = target_devices.lst_target_devices.begin();

    while (iter_tg_device != target_devices.lst_target_devices.end()){
        if (!iter_tg_device->err_info.code.empty()){
            ++iter_tg_device;
            continue;
        }

        _operate_snapshot_out(cmdOperate,*iter_tg_device);

        ++iter_tg_device;
    }
    return;
}

int TestFailoverStart::_get_consist_devices_out(CCmdOperate& cmdOperate,TargetDevicesInfo& target_devices)
{
    int ret = RETURN_ERR;
    list<CommonLunInfo> lst_lun_info;
    list<CommonLunInfo>::iterator iter_luninfo;
    list<TargetGroupInfo>::iterator iter_tg_group;
    list<string> lst_token;
    string lunid;
    string cgid;
    string arrayid;
    string lun_flag;
    string hymirrorid;
    string hypermetroid;

    iter_tg_group = tg_groups.lst_groups_info.begin();
    while (iter_tg_group != tg_groups.lst_groups_info.end()){
        lst_lun_info.clear();

        ret = get_device_info(iter_tg_group->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != ret){
            iter_tg_group->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_tg_group->tg_key.c_str());
            ++iter_tg_group;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++iter_tg_group;
            continue;
        }

        if (cgid.empty()){
            ++iter_tg_group;
            continue;
        }

        is_Fs_by_lun(cmdOperate, lunid);

        
        ret = get_lun_by_consisgr_out(cmdOperate, (unsigned int)MIRROR_LUN_RELATION_SLAVE, cgid, lst_lun_info, isNFS);       
        if (RETURN_OK != ret){
            iter_tg_group->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "failed to get slave lun info of cg(%s)", iter_tg_group->tg_key.c_str());

            ++iter_tg_group;
            continue;
        }

        iter_luninfo = lst_lun_info.begin();
        while (iter_luninfo != lst_lun_info.end()){
            TargetDeviceInfo target_device;
            string hymirror_id;
            target_device.cg_id = iter_tg_group->tg_key;
            if(isNFS){
                ret = get_hymirror_by_fs_out(cmdOperate, iter_luninfo->lun_id, hymirror_id);
            }
            else{
                ret = get_hymirror_by_lun_out(cmdOperate, iter_luninfo->lun_id, hymirror_id);
            }
            if (RETURN_OK != ret){
                COMMLOG(OS_LOG_ERROR, "failed to get lun info of LUN(%s)", iter_luninfo->lun_id.c_str());
                ++iter_luninfo;
                continue;
            }

            
            string pattern(":");
            std::vector<string> vHymirrorIDs = split(hymirror_id, pattern);

            string accessgroups;
            for (size_t i = 0; i < vHymirrorIDs.size(); i++){
                target_device.target_key = iter_luninfo->array_id + "_" + iter_luninfo->lun_id + "_" + MIRROR_LUN_TAG + "_" + vHymirrorIDs[i];
                if(accessGroupDict.find(target_device.target_key) != accessGroupDict.end()){
                    accessgroups = accessGroupDict[target_device.target_key];
                    break;
                }
            }

            if (accessgroups.empty()){
                target_device.target_key = iter_luninfo->array_id + "_" + iter_luninfo->lun_id + "_" + MIRROR_LUN_TAG + "_" + hymirror_id;
                COMMLOG(OS_LOG_ERROR, "Cannot find access group info of device(%s)", target_device.target_key.c_str());
                ++iter_luninfo;
                continue;
            }

            target_device.cg_accessgroups = accessgroups;
            target_device.isNfs = isNFS;
            target_device.err_info.code = iter_tg_group->err_info.code;
            target_devices.lst_target_devices.push_back(target_device);

            ++iter_luninfo;
        }
        ++iter_tg_group;
    }

    
    if (!g_bstretch){
        return RETURN_OK;
    }
    

    iter_tg_group = tg_groups.lst_groups_info.begin();
    while (iter_tg_group != tg_groups.lst_groups_info.end()){
        lst_lun_info.clear();

        ret = get_device_info(iter_tg_group->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != ret){
            iter_tg_group->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter_tg_group->tg_key.c_str());
            ++iter_tg_group;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++iter_tg_group;
            continue;
        }

        if (cgid.empty()){
            ++iter_tg_group;
            continue;
        }

        
        ret = get_lun_by_consisgrhm_out(cmdOperate, (unsigned int)MIRROR_LUN_RELATION_SLAVE, cgid, lst_lun_info);       
        if (RETURN_OK != ret){
            iter_tg_group->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "failed to get slave lun info of cg(%s)", iter_tg_group->tg_key.c_str());

            ++iter_tg_group;
            continue;
        }

        iter_luninfo = lst_lun_info.begin();
        while (iter_luninfo != lst_lun_info.end()){
            TargetDeviceInfo target_device;
            string hypermetro_id;
            
            target_device.stretched = "";
            target_device.cg_id = iter_tg_group->tg_key;
            if (RETURN_OK != get_hypermetroid_by_lun_out(cmdOperate, iter_luninfo->lun_id, hypermetro_id)){
                ++iter_luninfo;
                continue;
            }

            target_device.target_key = iter_luninfo->array_id + "_" + iter_luninfo->lun_id + "_" +HYPERMETRO_LUN_TAG + "_" + hypermetro_id;
            string accessgroups;
            if(accessGroupDict.find(target_device.target_key) != accessGroupDict.end()){
                accessgroups = accessGroupDict[target_device.target_key];
            }
            else{
                COMMLOG(OS_LOG_ERROR, "Cannot find access group info of device(%s)", target_device.target_key.c_str());
                ++iter_luninfo;
                continue;
            }

            target_device.cg_accessgroups = accessgroups;
            target_device.err_info.code = iter_tg_group->err_info.code;
            target_devices.lst_target_devices.push_back(target_device);

            ++iter_luninfo;
        }
        ++iter_tg_group;
    }

    return RETURN_OK;
}

void TestFailoverStart::_split_target_info(TargetDevicesInfo& target_devices)
{
    list<TargetDeviceInfo>::iterator iter_tg_devices;
    list<TargetGroupInfo>::iterator iter_tg_groups;

    iter_tg_devices = target_devices.lst_target_devices.begin();
    while (iter_tg_devices != target_devices.lst_target_devices.end()){
        iter_tg_groups = tg_groups.lst_groups_info.begin();
        while (iter_tg_groups != tg_groups.lst_groups_info.end()){
            
            if (iter_tg_devices->cg_id == iter_tg_groups->tg_key){
                
                iter_tg_devices->snapshot_info.recoverypoint_info.rp_id.clear();

                iter_tg_groups->snapshot_info.id = "snap_" + iter_tg_devices->cg_id;

                iter_tg_groups->snapshot_info.recover_info.rp_id = "rp_" + iter_tg_groups->snapshot_info.id;

                min_time(iter_tg_devices->snapshot_info.recoverypoint_info.rp_time, iter_tg_groups->snapshot_info.recover_info.rp_time);

                iter_tg_devices->snapshot_info.recoverypoint_info.rp_name = "rp_" + iter_tg_groups->snapshot_info.id;

                iter_tg_groups->snapshot_info.tg_devices_info.lst_target_devices.push_back(*iter_tg_devices);
                break;
            }
            ++iter_tg_groups;
        }

        
        if (iter_tg_groups == tg_groups.lst_groups_info.end()){
            tg_devices.lst_target_devices.push_back(*iter_tg_devices);
        }

        ++iter_tg_devices;
    }

    return;
}

int TestFailoverStart::_outband_process()
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

    _write_progress(10);

    _check_status_sync_out(cmdOperate);

    if (g_bstretch){
        _check_statushm_sync_out(cmdOperate);
    }

    TargetDevicesInfo target_devices = tg_devices;

    
    
    if (!g_bFusionStorage){

        iRet = _get_consist_devices_out(cmdOperate, target_devices);
        CHECK_UNEQ(RETURN_OK, iRet);
    }
    _write_progress(20);

    
    _operate_snapshots_out(cmdOperate,target_devices);

    _write_progress(50);


    
    iRet = _map_snapshot_to_host_out(cmdOperate,target_devices);
    CHECK_UNEQ(RETURN_OK, iRet);

    _write_progress(70);

    
    iRet = _get_snapshot_wwn_out(cmdOperate,target_devices);
    CHECK_UNEQ(RETURN_OK, iRet);

    tg_devices.lst_target_devices.clear();

    
    _split_target_info(target_devices);
    
    if(g_bnfs == true){
        cmdOperate.CMD_showLIF(lstLifInfo);
        for (itLifInfo = lstLifInfo.begin(); itLifInfo != lstLifInfo.end(); ++itLifInfo){
            
            if(itLifInfo->role == SERVICE_ROLE || itLifInfo->role == MANAGEMENT_AND_SERVICE_ROLE){
                StoragePortsIPS storageportsips;
                storageportsips.source_IP = itLifInfo->strLIFIP;
                stroageports.lst_storageportsips.push_back(storageportsips);
            }
            else{
                COMMLOG(OS_LOG_INFO, "Filtered logical port [%s], because it cannot be a service port", itLifInfo->strLIFIP.c_str());
            }
            
        }
    }

   return RETURN_OK;
}

int TestFailoverStart::_map_snapshot(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device)
{
    int ret = RETURN_ERR;
    string snapname;
    string lun_id;
    string lun_flag;
    string pairid;
    string arrayid;

    ret = get_device_info(target_device.target_key, arrayid, lun_id, lun_flag, pairid);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);
        COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", target_device.target_key.c_str());
    }

    snapname = VSSRA_SNAPSHOT_PREFIX + lun_id + VSSRA_CLONE_LUN;

    
    COMMLOG(OS_LOG_INFO, "Replica of [%s] with key [%s] is being cloned from Recovery Point [%s]",
        lun_id.c_str(),
        target_device.testname.c_str(),
        target_device.snapshot_info.recoverypoint_info.rp_time.c_str());
    print("Replica of [%s] with key [%s] is being cloned from Recovery Point [%s]",
        lun_id.c_str(),
        target_device.testname.c_str(),
        target_device.snapshot_info.recoverypoint_info.rp_time.c_str());

    
    COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] has attributes:[r/w] [multihost]",
        snapname.c_str());
    print("Snapshot copy [%s] has attributes:[r/w] [multihost]",
        snapname.c_str());

    
    list<MAP_INFO_STRU> rlstMapInfo;
    list<MAP_INFO_STRU> rlstSnapMapInfo;
    ret = cmdOperate.CMD_showmap(lun_id,rlstMapInfo);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "query volume map failed,the id is [%s]", lun_id.c_str());
        return ret;
    }
    ret = cmdOperate.CMD_showmap(target_device.snapshot_info.snap_id, rlstSnapMapInfo);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "query snapshot map failed,the id is [%s]", target_device.snapshot_info.snap_id.c_str());
        return ret;
    }
    set<string> hosts, hostGroups;
    list<string> mapLuns;
    mapLuns.push_back(snapname);

    list<CMDHOSTINFO_STRU> lstHosts;
    ret = _add_map(cmdOperate, target_device, lstDARHosts, lstHosts, target_device.snapshot_info.snap_id, OBJ_SNAPSHOT);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "map snapshot[%s] to host fail", target_device.snapshot_info.snap_id.c_str());
        return ret;
    }

    return RETURN_OK;
}

void TestFailoverStart::_find_host_and_hostGroups_for_fusion(const list<MAP_INFO_STRU>& rlstMapInfo, const list<MAP_INFO_STRU>& rlstSnapMapInfo, \
    set<string>& hosts, set<string>& hostGroups)
{
    for (list<MAP_INFO_STRU>::const_iterator itVol = rlstMapInfo.begin(); itVol != rlstMapInfo.end(); ++itVol){
        if (itVol->strHostGroupName != ""){
            hostGroups.insert(itVol->strHostGroupName);
        }
        else{
            hosts.insert(itVol->strHostName);
        }
    }
    
    for (list<MAP_INFO_STRU>::const_iterator itSnap = rlstSnapMapInfo.begin(); itSnap != rlstSnapMapInfo.end(); ++itSnap){
        if(hostGroups.find(itSnap->strHostGroupName) != hostGroups.end()){
            hostGroups.erase(itSnap->strHostGroupName);
        }

        if (hosts.find(itSnap->strHostName) != hosts.end()){
            hosts.erase(itSnap->strHostName);
        }
    }
}

int TestFailoverStart::_and_map_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device)
{
    int ret = RETURN_ERR;
    string snapname;
    string lun_id;
    string lun_flag;
    string pairid;
    string arrayid;
    int nCount = 0;

    list<CMDHOSTINFO_STRU> lstHosts;
    list<CMDHOSTINFO_STRU>::iterator ithost;
    list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
    list<HOST_PORT_INFO_STRU>::iterator itHostPortInfo;

    ret = get_device_info(target_device.target_key, arrayid, lun_id, lun_flag, pairid);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);
        COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", target_device.target_key.c_str());
    }

    snapname = VSSRA_SNAPSHOT_PREFIX + lun_id;

    
    COMMLOG(OS_LOG_INFO, "Replica of [%s] with key [%s] is being cloned from Recovery Point [%s]",
        lun_id.c_str(),
        target_device.testname.c_str(),
        target_device.snapshot_info.recoverypoint_info.rp_time.c_str());
    print("Replica of [%s] with key [%s] is being cloned from Recovery Point [%s]",
        lun_id.c_str(),
        target_device.testname.c_str(),
        target_device.snapshot_info.recoverypoint_info.rp_time.c_str());

    
    COMMLOG(OS_LOG_INFO,"Snapshot copy [%s] has attributes:[r/w] [multihost]",
        snapname.c_str());    
    print("Snapshot copy [%s] has attributes:[r/w] [multihost]",
        snapname.c_str());

    
    COMMLOG(OS_LOG_INFO, "the lun id is [%s], the snapshot id is [%s]", lun_id.c_str(), target_device.snapshot_info.snap_id.c_str());
    ret = _add_map(cmdOperate, target_device, lstDARHosts, lstHosts, target_device.snapshot_info.snap_id, OBJ_SNAPSHOT);
    if (RETURN_OK != ret){
        target_device.err_info.code = OS_IToString(ret);
        COMMLOG(OS_LOG_ERROR, "Map snapshot [%s] to hosts failed.", lun_id.c_str());
        return ret;
    }

    
    for (ithost = lstHosts.begin(); ithost != lstHosts.end(); ++ithost){
        string strIQN = "";
        string strCom = "";
        string strChapName = "";
        list<HOST_LINK_INFO> rlstHostLinkInfo;
        list<HOST_LINK_INFO>::iterator itHostLinkInfo;
        string strPortWwn("");
        string strHostCom("");
        unsigned int fcOriscsi = 0;
        
        cmdOperate.CMD_showhostport(ithost->strID, rlstHostPortInfo);
        for (itHostPortInfo = rlstHostPortInfo.begin();itHostPortInfo != rlstHostPortInfo.end(); itHostPortInfo++){
            if (!itHostPortInfo->strIQN.empty() && itHostPortInfo->strIQN.size() > 1){
                strIQN += strCom + itHostPortInfo->strIQN;
                strChapName += strCom + itHostPortInfo->strChapName;
                strCom = ",";
                fcOriscsi = itHostPortInfo->uifcOriscsi;
                cmdOperate.CMD_showhostlink(itHostPortInfo->strIQN, itHostPortInfo->uifcOriscsi, rlstHostLinkInfo);
                for (itHostLinkInfo = rlstHostLinkInfo.begin(); itHostLinkInfo != rlstHostLinkInfo.end(); itHostLinkInfo++){
                    if (!itHostLinkInfo->target_port_wwn.empty() && itHostLinkInfo->target_port_wwn.size() > 1){
                        strPortWwn += strHostCom + itHostLinkInfo->target_port_wwn ;
                        strHostCom = ",";
                    }
                }
            }
        }            
        if (fcOriscsi == 0){
            nCount++;
            COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being exposed to host [%s] on target[%s] using chapName [%s] ",
                snapname.c_str(),
                strIQN.c_str(),
                strPortWwn.c_str(),
                strChapName.c_str());
            print("Snapshot copy [%s] is being exposed to host [%s] on target[%s] using chapName [%s] ",
                snapname.c_str(),
                strIQN.c_str(),
                strPortWwn.c_str(),
                strChapName.c_str());
        }
        else{
            nCount++;
            COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being exposed to host [%s] on target[%s]",
                snapname.c_str(),
                strIQN.c_str(),
                strPortWwn.c_str());
            print("Snapshot copy [%s] is being exposed to host [%s] on target[%s]",
                snapname.c_str(),
                strIQN.c_str(),
                strPortWwn.c_str());
        }
    }

    
    COMMLOG(OS_LOG_INFO, "Complete: Temporary Snapshot Copy of [%s] has been created and presented to (%d) hosts for testFailoverStart request.",
        target_device.testname.c_str(),
        nCount);
    print("Complete: Temporary Snapshot Copy of [%s] has been created and presented to (%d) hosts for testFailoverStart request.",
        target_device.testname.c_str(),
        nCount);

    return RETURN_OK;
}

void TestFailoverStart::_check_status_sync_out(CCmdOperate& cmdOperate)
{
    int iRet = RETURN_ERR;
    string cgid;
    string hymirror_id;
    string status;
    string lun_flag;
    string arrayid;
    list<TargetGroupInfo>::iterator itertg;
    list<TargetDeviceInfo>::iterator iter;
    list<string> lst_token;
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;

    
    if (!g_bFusionStorage){
        itertg = tg_groups.lst_groups_info.begin();
        while (itertg != tg_groups.lst_groups_info.end()){
            iRet = get_device_info(itertg->tg_key, arrayid, strid, lun_flag, cgid);
            if (RETURN_OK != iRet){
                itertg->err_info.code = OS_IToString(iRet);
                COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", itertg->tg_key.c_str());
                ++itertg;
                continue;
            }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++itertg;
            continue;
        }

        if (cgid.empty()){
            ++itertg;
            continue;
        }

        if ("--" == cgid){
            itertg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR,"consisgroup with lun [%s] is [%s]",strid.c_str(),cgid.c_str());
            ++itertg;
            continue;
        }

        is_Fs_by_lun(cmdOperate, strid); 

        if(isNFS){
            COMMLOG(OS_LOG_INFO,"vstorepair with fs [%s] is [%s]",strid.c_str(),cgid.c_str());
            VSTORE_PAIR_INFO_STRU vstorePairInfo;
            iRet = cmdOperate.CMD_showvstorepair_info(cgid, vstorePairInfo);
            if (RETURN_OK != iRet){
                itertg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
                COMMLOG(OS_LOG_ERROR, "id of vstorepair(%s) is wrong.", cgid.c_str());
                ++itertg;
                continue;
            }
            _convert_pair_status(vstorePairInfo.uiStatus,status);
        }
        else{
            
            COMMLOG(OS_LOG_INFO,"consisgroup with lun [%s] is [%s]",strid.c_str(),cgid.c_str());
            GROUP_INFO_STRU stGroupInfo;
            stGroupInfo.strID = cgid;
            iRet = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
            if (RETURN_OK != iRet){
                itertg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
                COMMLOG(OS_LOG_ERROR, "id of consisgroup(%s) is wrong.", cgid.c_str());
                ++itertg;
                continue;
            }
            _convert_pair_status(stGroupInfo.uiStatus,status);
        }        
        
        if(HM_SYNCING == status){
            itertg->err_info.code = OS_IToString(ERROR_SLAVELUN_IS_SYNCING);
            COMMLOG(OS_LOG_INFO, "consisgroup(%s)'s status is Synchronizing.", cgid.c_str());
        }
        
        COMMLOG(OS_LOG_INFO,"the status of lun [%s] is [%s]",strid.c_str(),status.c_str());
        ++itertg;

        }
    }

    iter = tg_devices.lst_target_devices.begin();
    while(iter != tg_devices.lst_target_devices.end()){
        iRet = get_device_info(iter->target_key, arrayid, strid, lun_flag, hymirror_id);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++iter;
            continue;
        }

        if (hymirror_id.empty()){
            ++iter;
            continue;
        }

        lstHyMirrorLFInfo.clear();
        is_Fs_by_lun(cmdOperate, strid);
        iter->isNfs = isNFS;
        
        COMMLOG(OS_LOG_INFO,"hymirror with lun [%s] is [%s]",strid.c_str(),hymirror_id.c_str());
        if (isNFS){
            iRet = cmdOperate.CMD_showhymirrorbyfs(strid, lstHyMirrorLFInfo);
        }
        else{
            iRet = cmdOperate.CMD_showhymirrorlun_all(hymirror_id, lstHyMirrorLFInfo);
        }
        if (RETURN_OK != iRet){        
            iter->err_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++iter;
            continue;       
        }


        itHyMirrorLFInfo = lstHyMirrorLFInfo.begin();
        while (itHyMirrorLFInfo != lstHyMirrorLFInfo.end()){
            if (isNFS){
                hyper_id = itHyMirrorLFInfo->strMirrorID;
            }
            if (MIRROR_LUN_RELATION_SLAVE== itHyMirrorLFInfo->uiRelationType){
                _convert_pair_status(itHyMirrorLFInfo->uiPairStatus,status);
                break;
            }

            itHyMirrorLFInfo++;
        }

        if(HM_SYNCING == status){
            iter->err_info.code = OS_IToString(ERROR_SLAVELUN_IS_SYNCING);
            COMMLOG(OS_LOG_INFO,"the status of lun [%s] with hymirror [%s] is [%s]",
                strid.c_str(),hymirror_id.c_str(),status.c_str());
            print("the status of lun [%s] with hymirror [%s] is [%s]",
                strid.c_str(),hymirror_id.c_str(),status.c_str());
        }
        
        else{
            COMMLOG(OS_LOG_INFO,"the status of lun [%s] with hymirror [%s] is [%s]",
                strid.c_str(),hymirror_id.c_str(),status.c_str());
            print("the status of lun [%s] with hymirror [%s] is [%s]",
                strid.c_str(),hymirror_id.c_str(),status.c_str());
        }
        ++iter;
    }

    return;
}

void TestFailoverStart::_check_statushm_sync_out(CCmdOperate& cmdOperate)
{
    int iRet = RETURN_ERR;
    string cgid;
    string hypermetroid;
    string status;
    string arrayid;
    string lun_flag;
    list<TargetGroupInfo>::iterator itertg;
    list<TargetDeviceInfo>::iterator iter;
    list<string> lst_token;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;

    
    itertg = tg_groups.lst_groups_info.begin();
    while(itertg != tg_groups.lst_groups_info.end()){
        iRet = get_device_info(itertg->tg_key, arrayid, strid, lun_flag, cgid);
        if (RETURN_OK != iRet){
            itertg->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", itertg->tg_key.c_str());
            ++itertg;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++itertg;
            continue;
        }

        if (cgid.empty()){
            ++itertg;
            continue;
        }

        if ("--" == cgid){
            itertg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR,"hypermetro group with lun [%s] is [%s]",strid.c_str(),cgid.c_str());
            ++itertg;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"hypermetro group with lun [%s] is [%s]",strid.c_str(),cgid.c_str());

        GROUP_INFO_STRU stGroupInfo;
        stGroupInfo.strID = cgid;
        iRet = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
        if (RETURN_OK != iRet){
            itertg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            COMMLOG(OS_LOG_ERROR, "id of consisgroup(%s) is wrong.", cgid.c_str());
            ++itertg;
            continue;
        }
        _convert_hmpair_status(stGroupInfo.uiStatus,status);
        
        if(HM_SYNCING == status){
            itertg->err_info.code = OS_IToString(ERROR_SLAVELUN_IS_SYNCING);
            COMMLOG(OS_LOG_INFO, "hypermetro group(%s)'s status is Synchronizing.", cgid.c_str());
        }
        
        COMMLOG(OS_LOG_INFO,"the status of lun [%s] is [%s]",strid.c_str(),status.c_str());
        ++itertg;

    }

    iter = tg_devices.lst_target_devices.begin();
    while(iter != tg_devices.lst_target_devices.end()){
        iRet = get_device_info(iter->target_key, arrayid, strid, lun_flag, hypermetroid);
        if (RETURN_OK != iRet){
            iter->err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->target_key.c_str());
            ++iter;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++iter;
            continue;
        }

        if (hypermetroid.empty()){
            ++iter;
            continue;
        }

        lstHMPairLFInfo.clear();

        
        COMMLOG(OS_LOG_INFO,"hypermetro with lun [%s] is [%s]",strid.c_str(),hypermetroid.c_str());

        iRet = cmdOperate.CMD_showhypermetrolun_all(hypermetroid,lstHMPairLFInfo);

        if (RETURN_OK != iRet){        
            iter->err_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++iter;
            continue;       
        }

        itHMPairLFInfo = lstHMPairLFInfo.begin();
        while (itHMPairLFInfo != lstHMPairLFInfo.end()){
            if (MIRROR_LUN_RELATION_SLAVE== itHMPairLFInfo->uiRelationType){
                _convert_hmpair_status(itHMPairLFInfo->uiPairStatus,status);
                break;
            }

            itHMPairLFInfo++;
        }

        if(HM_SYNCING == status){
            iter->err_info.code = OS_IToString(ERROR_SLAVELUN_IS_SYNCING);
            COMMLOG(OS_LOG_INFO,"the status of lun [%s] with hypermetro [%s] is [%s]",
                strid.c_str(),hypermetroid.c_str(),status.c_str());
            print("the status of lun [%s] with hypermetro [%s] is [%s]",
                strid.c_str(),hypermetroid.c_str(),status.c_str());
        }
        
        else{
            COMMLOG(OS_LOG_INFO,"the status of lun [%s] with hymirror [%s] is [%s]",
                strid.c_str(),hypermetroid.c_str(),status.c_str());
            print("the status of lun [%s] with hypermetro [%s] is [%s]",
                strid.c_str(),hypermetroid.c_str(),status.c_str());
        }
        ++iter;
    }

    return;
}

void TestFailoverStart::_write_response(XmlWriter &writer)
{

    (void)writer.set_xml("/Response/TestFailoverStartResults", &tg_groups);
    (void)writer.set_xml("/Response/TestFailoverStartResults", &tg_devices);
    (void)writer.set_xml("/Response/TestFailoverStartResults", &stroageports);
    return;
}
