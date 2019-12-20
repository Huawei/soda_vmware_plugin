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

#include "test_failover_stop.h"

int test_failover_stop(XmlReader &reader)
{
    TestFailoverStop tfs;
    COMMLOG(OS_LOG_INFO, "%s", "TestFailoverStop begin.");
    print("%s", "TestFailoverStop begin.");

    if (RETURN_OK != tfs.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "Test Failover Stop failed.");

        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "TestFailoverStop end.");
    print("%s", "TestFailoverStop end.");
    return RETURN_OK;
}

TestFailoverStop::TestFailoverStop() : SraBasic()
{
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
}

TestFailoverStop::~TestFailoverStop()
{

};

int TestFailoverStop::_read_command_para(XmlReader &reader)
{
    char obj_id_tmp[LENGTH_COMMON] = {0};

    int consistgr_count = 0;
    int devices_count = 0;
    int index;

    CHECK_FALSE(reader.get_string("/Command/TestFailoverStopParameters/ArrayId", array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/TestFailoverStopParameters/ArrayId", input_array_id, 0), RETURN_ERR);

    consistgr_count = reader.get_count("/Command/TestFailoverStopParameters/TargetGroups/TargetGroup");
    for (index = 0; index < consistgr_count; index++){
        TargetGroupInfo tg;
        memset_s(obj_id_tmp, sizeof(obj_id_tmp), 0, sizeof(obj_id_tmp));

        CHECK_FALSE(reader.get_string("/Command/TestFailoverStopParameters/TargetGroups/TargetGroup/@key", obj_id_tmp, index), ERROR_INTERNAL_PROCESS_FAIL);

        tg.tg_key = string(obj_id_tmp);
        targetconsisgrs_info.lst_groups_info.push_back(tg);
    }

    devices_count = reader.get_count("/Command/TestFailoverStopParameters/TargetDevices/TargetDevice");
    for (index = 0; index < devices_count; index++){
        memset_s(obj_id_tmp, sizeof(obj_id_tmp), 0, sizeof(obj_id_tmp));
        CHECK_FALSE(reader.get_string("/Command/TestFailoverStopParameters/TargetDevices/TargetDevice/@key", obj_id_tmp, index), ERROR_INTERNAL_PROCESS_FAIL);

        TargetDeviceInfo td;

        td.target_key = string(obj_id_tmp);
        tartgetdevs_info.lst_target_devices.push_back(td);
    }

    return RETURN_OK;
}

int TestFailoverStop::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (array_id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "TestFailoverStop::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_id.c_str(), array_id.c_str());
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

int TestFailoverStop::_do_failover_stop_fusion(CCmdOperate& cmdOperate)
{
    string strid;
    string hyperid;
    string tmp_id;
    string arrayid;
    string lun_flag;
    int ret;
    list<HYMIRROR_LF_INFO_STRU> rlstHyMirrorLUNInfo;
    list<HYIMAGE_INFO_STRU> rlstHyImageInfo;
    string snapname;
    list<TargetDeviceInfo>::iterator it_td = tartgetdevs_info.lst_target_devices.begin();
    for (;it_td != tartgetdevs_info.lst_target_devices.end(); it_td++){
        rlstHyMirrorLUNInfo.clear();
        rlstHyImageInfo.clear();
        ret = get_device_info(it_td->target_key, arrayid, strid, lun_flag, hyperid);
        if (ret != RETURN_OK){
            it_td->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "query deviceinfo error by target_key[%s]", it_td->target_key.c_str());
            continue;
        }

        if (lun_flag == "RP"){
            ret = cmdOperate.CMD_showhymirrorlun_all(hyperid, rlstHyMirrorLUNInfo);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "show mirror info error,mirrorId[%s]", hyperid.c_str());
                it_td->err_info.code = OS_IToString(ret);
                continue;
            }
        }
        else if(lun_flag == "MP"){
            list<HYPERMETRO_LF_INFO_STRU> rlstHMLUNInfo;
            ret = cmdOperate.CMD_showhypermetrolun_all(hyperid, rlstHMLUNInfo);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "show hyper pair info error,hyperId[%s]", hyperid.c_str());
                it_td->err_info.code = OS_IToString(ret);
                continue;
            }
        }

        ret = cmdOperate.CMD_showhyimgoflun(strid, rlstHyImageInfo);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "query snapshop error,lunId[%s]", strid.c_str());
            it_td->err_info.code = OS_IToString(ret);
            continue;
        }
        snapname = VSSRA_SNAPSHOT_PREFIX + strid;
        ret = _delete_snapshot_fusion(snapname, cmdOperate,rlstHyImageInfo);
        if (ret != RETURN_OK){
            if (ret == RETURN_WAR){
                COMMLOG(OS_LOG_WARN, "snapshot or clone_lun of lun(%s) is not exist.", strid.c_str());
                it_td->success = true;
            }
            else{
                COMMLOG(OS_LOG_ERROR, "delete snapshot error");
                it_td->err_info.code = OS_IToString(ret);
            }
            continue;
        }
        else{
            it_td->success = true;
        }
    }
    return RETURN_OK;
}

int TestFailoverStop::_delete_snapshot_fusion(string& snapname, CCmdOperate& cmdOperate,list<HYIMAGE_INFO_STRU>& snapshotlist)
{
    bool isExist = false;
    string snapshotId;
    int ret = RETURN_ERR;
    list<string> luns;
    
    for (list<HYIMAGE_INFO_STRU>::iterator it = snapshotlist.begin(); it != snapshotlist.end(); it++){
        if (snapname == it->strName){
            isExist = true;
            snapshotId = it->strID;
            luns.push_back(snapname + VSSRA_CLONE_LUN);
            break;
        }
    }

    if (isExist){
        list<MAP_INFO_STRU> rlstMapInfo;

        LUN_INFO_STRU rstLUNInfo;
        string name = snapname + VSSRA_CLONE_LUN;
        cmdOperate.CMD_showlunByname(name, rstLUNInfo);
        ret = cmdOperate.CMD_showmap(rstLUNInfo.strID, rlstMapInfo);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR,"query mapinfo error,snapshotname[%s]",snapname.c_str());
            return ret;
        }
        
        for (list<MAP_INFO_STRU>::iterator name = rlstMapInfo.begin(); name != rlstMapInfo.end(); name++){
            if (!name->strHostGroupName.empty()){
                ret = cmdOperate.CMD_unmaplun2hostGroup(name->strHostGroupName,luns);
                if (ret != RETURN_OK)
                    COMMLOG(OS_LOG_ERROR, "unmap error,hostgroupname[%s]", name->strHostGroupName.c_str());
            }
            if (name->strHostGroupName.empty() && !name->strHostName.empty()){
                ret = cmdOperate.CMD_unmaplun2host(name->strHostName,luns);
                if(ret != RETURN_OK)
                    COMMLOG(OS_LOG_ERROR, "unmap error,hostname[%s]", name->strHostName.c_str());
            }
        }
    }
    else{
        COMMLOG(OS_LOG_WARN, "can not find the snapshotName[%s]",snapname.c_str());
    }

    
    list<string> lunsname;
    lunsname.push_back(snapname + VSSRA_CLONE_LUN);
    ret = cmdOperate.CMD_dellun(lunsname);
    if (ret != RETURN_OK){
        if (ret == RETURN_WAR){
            COMMLOG(OS_LOG_WARN, "clone_lun_name[%s] is not exist.", lunsname.front().c_str());
        }
        else{
            COMMLOG(OS_LOG_ERROR, "delete snap_clone_lun error,clone_lun_name[%s]", lunsname.front().c_str());
            return ret;
        }  
    }

    OS_Sleep(1000);
    
    if( isExist ){
        ret = cmdOperate.CMD_delhyimg(snapname);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "delete snapshot error,snapshotName[%s]", snapname.c_str());
            return ret;
        }
    }
    else{
        ret = RETURN_WAR;
    }
    
    return ret;
}

void TestFailoverStop::_write_response(XmlWriter &writer)
{    
    (void)writer.set_xml("/Response/TestFailoverStopResults", &targetconsisgrs_info);
    (void)writer.set_xml("/Response/TestFailoverStopResults", &tartgetdevs_info);

    return;
}

int TestFailoverStop::_outband_process()
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

    

    if (g_bFusionStorage){
        iRet = _do_failover_stop_fusion(cmdOperate);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR,"failover_stop error,ret[%d]",iRet);
            return iRet;
        }
        return RETURN_OK;
    }

    iRet = cmdOperate.CMD_showmap(lstMapInfo);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "showmap is failure,error code is %d.", iRet);
        return ERROR_INTERNAL_PROCESS_FAIL;
    }
    
    _deal_consistentgroup_out(cmdOperate);
    
    if (g_bstretch){
        _deal_consistentgrouphm_out(cmdOperate);
    }

    _deal_hymirror_out(cmdOperate);
    
    if (g_bstretch){
        _deal_hypermetro_out(cmdOperate);
    }

    return RETURN_OK;
}

void TestFailoverStop::_deal_consistentgroup_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<TargetGroupInfo>::iterator it_tg;
    list<string> lst_tmp;
    list<CommonLunInfo> lst_lun_info;
    list<string> lst_token;
    string lunid;
    string cgid;
    string cgname;
    string arrayid;
    string lun_flag;

    it_tg = targetconsisgrs_info.lst_groups_info.begin();
    while (it_tg != targetconsisgrs_info.lst_groups_info.end()){
        GROUP_INFO_STRU stGroupInfo;
        VSTORE_PAIR_INFO_STRU vstorePairInfo;

        lst_lun_info.clear();

        ret = get_device_info(it_tg->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it_tg->tg_key.c_str());
            ++it_tg;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            ++it_tg;
            continue;
        }

        if (cgid.empty()){
            ++it_tg;
            continue;
        }
        
        if ("--" == cgid){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }

        is_Fs_by_lun(cmdOperate, lunid);

        if(isNFS){
            ret = cmdOperate.CMD_showvstorepair_info(cgid, vstorePairInfo);
            cgname = vstorePairInfo.strID;
        }
        else{
            stGroupInfo.strID = cgid;
            ret = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
            cgname = stGroupInfo.strName;
        }

        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"the lun [%s] belong to group [%s]",lunid.c_str(),cgid.c_str());
        print("the lun [%s] belong to group [%s]",lunid.c_str(),cgid.c_str());
        if (cgname.length() == 0){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }

        
        ret = get_lun_by_consisgr_out(cmdOperate, (unsigned int)MIRROR_LUN_RELATION_SLAVE, cgid, lst_lun_info, isNFS);
        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ret);
            ++it_tg;
            continue;
        }

        
        _del_lun_snapshot_out(cmdOperate, lst_lun_info,it_tg->err_info, it_tg->war_info);

        if (RETURN_OK == ret){
            it_tg->success = true;
        }

        ++it_tg;
    }

    return;
}

void TestFailoverStop::_deal_consistentgrouphm_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<TargetGroupInfo>::iterator it_tg;
    list<string> lst_tmp;
    list<CommonLunInfo> lst_lun_info;
    list<string> lst_token;
    string lunid;
    string cgid;
    string cgname;
    string arrayid;
    string lun_flag;

    it_tg = targetconsisgrs_info.lst_groups_info.begin();
    while (it_tg != targetconsisgrs_info.lst_groups_info.end()){
        GROUP_INFO_STRU stGroupInfo;

        lst_lun_info.clear();

        ret = get_device_info(it_tg->tg_key, arrayid, lunid, lun_flag, cgid);
        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it_tg->tg_key.c_str());
            ++it_tg;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_CONSISTENT_TAG)){
            ++it_tg;
            continue;
        }

        if (cgid.empty()){
            ++it_tg;
            continue;
        }

        
        if ("--" == cgid){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }

        stGroupInfo.strID = cgid;
        ret = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"the lun [%s] belong to group [%s]",lunid.c_str(),cgid.c_str());
        print("the lun [%s] belong to group [%s]",lunid.c_str(),cgid.c_str());
        cgname = stGroupInfo.strName;
        if (cgname.length() == 0){
            it_tg->err_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            ++it_tg;
            continue;
        }

        
        ret = get_lun_by_consisgrhm_out(cmdOperate, (unsigned int)MIRROR_LUN_RELATION_SLAVE, cgid, lst_lun_info);
        if (RETURN_OK != ret){
            it_tg->err_info.code = OS_IToString(ret);
            ++it_tg;
            continue;
        }

        _del_lun_snapshot_out(cmdOperate, lst_lun_info, it_tg->err_info, it_tg->war_info);

        if (RETURN_OK == ret){
            it_tg->success = true;
        }

        ++it_tg;
    }

    return;
}

void TestFailoverStop::_deal_hymirror_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_token;
    list<TargetDeviceInfo>::iterator it_td;
    list<string> lst_tmp;
    string strid;
    string hyperid;
    string cmd;
    string tmp_id;
    string key="";
    string slave_array_sn;
    string arrayid;
    string lun_flag;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;

    it_td = tartgetdevs_info.lst_target_devices.begin();
    while (it_td != tartgetdevs_info.lst_target_devices.end()){
        ret = get_device_info(it_td->target_key, arrayid, strid, lun_flag, hyperid);
        if (RETURN_OK != ret){
            it_td->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it_td->target_key.c_str());
            ++it_td;
            continue;
        }

        if (lun_flag.compare(MIRROR_LUN_TAG)){
            ++it_td;
            continue;
        }

        key = arrayid + "." + strid;

        if (hyperid.empty()){
            ++it_td;
            continue;
        }

        
        is_Fs_by_lun(cmdOperate, strid);
        if (isNFS){
            ret = cmdOperate.CMD_showhymirrorbyfs(strid, lstHyMirrorLFInfo);
        } 
        else{
            ret = cmdOperate.CMD_showhymirrorlun_all(hyperid, lstHyMirrorLFInfo);
        }
        if (RETURN_OK != ret){
            it_td->err_info.code = OS_IToString(ret);
            ++it_td;
            continue;
        }

        
        for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
            if (isNFS){
                hyperid = itHyMirrorLFInfo->strMirrorID;
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLFInfo->uiRelationType){
                tmp_id = itHyMirrorLFInfo->strID;
                slave_array_sn = itHyMirrorLFInfo->strArrayWWN;
                break;
            }
        }
        
        if ((0 ==tmp_id.length()) || (tmp_id != strid)){
            it_td->err_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++it_td;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request.",
            key.c_str());
        print("Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request.",
            key.c_str());
        ret = _stop_test_device_out(cmdOperate, strid, it_td->err_info, it_td->war_info);

        if (RETURN_OK == ret){
            it_td->success = true;
        }
        
        COMMLOG(OS_LOG_INFO, "Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request.",
            key.c_str());
        print("Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request.",
            key.c_str());
        ++it_td;
    }

    return;
}

void TestFailoverStop::_deal_hypermetro_out(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    list<string> lst_token;
    list<TargetDeviceInfo>::iterator it_td;
    list<string> lst_tmp;
    string strid;
    string hypermetroid;
    string cmd;
    string tmp_id;
    string key="";
    string slave_array_sn;
    string arrayid;
    string lun_flag;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;

    it_td = tartgetdevs_info.lst_target_devices.begin();
    while (it_td != tartgetdevs_info.lst_target_devices.end()){
        ret = get_device_info(it_td->target_key, arrayid, strid, lun_flag, hypermetroid);
        if (RETURN_OK != ret){
            it_td->err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", it_td->target_key.c_str());
            ++it_td;
            continue;
        }

        if (lun_flag.compare(HYPERMETRO_LUN_TAG)){
            ++it_td;
            continue;
        }

        key = arrayid + "." + strid;

        if (hypermetroid.empty()){
            ++it_td;
            continue;
        }

        ret = cmdOperate.CMD_showhypermetrolun_all(hypermetroid, lstHMPairLFInfo);

        if (RETURN_OK != ret){
            it_td->err_info.code = OS_IToString(ret);
            ++it_td;
            continue;
        }

        
        for (itHMPairLFInfo = lstHMPairLFInfo.begin(); itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
            if (isNFS){
                hypermetroid = itHMPairLFInfo->strHMpairID;
            }
            if (MIRROR_LUN_RELATION_SLAVE == itHMPairLFInfo->uiRelationType){
                tmp_id = itHMPairLFInfo->strID;
                slave_array_sn = itHMPairLFInfo->strArrayWWN;
                break;
            }
        }
        
        if ((0 ==tmp_id.length()) || (tmp_id != strid)){
            it_td->err_info.code = OS_IToString(ERROR_INVALIDATE_DEVICE_ID);
            ++it_td;
            continue;
        }

        
        COMMLOG(OS_LOG_INFO, "Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request.",
            key.c_str());
        print("Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request.",
            key.c_str());
        ret = _stop_test_device_out(cmdOperate, strid, it_td->err_info, it_td->war_info);

        if (RETURN_OK == ret){
            it_td->success = true;
        }
        
        COMMLOG(OS_LOG_INFO, "Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request.",
            key.c_str());
        print("Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request.",
            key.c_str());
        ++it_td;
    }

    return;
}

void TestFailoverStop::_del_lun_snapshot_out(CCmdOperate& cmdOperate, list<CommonLunInfo>& lst_lun_info, ErrorInfo& errorinfo, WarnInfo& warninfo)
{
    int ret = RETURN_ERR;

    list<CommonLunInfo>::iterator it_lun_info;

    it_lun_info = lst_lun_info.begin();
    while (it_lun_info != lst_lun_info.end()){
        if (string(array_id) != it_lun_info->array_id){
            ++it_lun_info;
            continue;
        }
        
        string key;
        key = it_lun_info->array_id + "." + it_lun_info->lun_id;
        COMMLOG(OS_LOG_INFO,"Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request",
            key.c_str());    
        print("Commence: Temporary Snapshot Copy of [%s] is being destroyed for testFailoverStop request",
            key.c_str());
        ret = _stop_test_device_out(cmdOperate, it_lun_info->lun_id, errorinfo, warninfo);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_ERROR,"_stop_test_device_out with key [%s]error[%d]",
                key.c_str(),ret);
            ++it_lun_info;
            continue;
        }
        
        COMMLOG(OS_LOG_INFO,"Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request",
            key.c_str());
        print("Complete: Temporary Snapshot Copy of [%s] has been removed from service and destroyed for testFailoverStop request",
            key.c_str());
        ++it_lun_info;
    }

    return;
}

int TestFailoverStop::_stop_test_device_out(CCmdOperate& cmdOperate, string& strid, ErrorInfo& err_info, WarnInfo& warn_info)
{
    int ret = RETURN_ERR;

    string desc;
    string cmd;
    string snap_name = VSSRA_SNAPSHOT_PREFIX + strid;
    string snap_id;
    string map_id;

    list<string> lst_map_id;
    list<string> lst_snapshot_id;
    list<string> lst_hostgroup_id;
    list<MAP_INFO_STRU> lst_delete_map;

    list<string>::iterator it_map_id;
    list<string>::iterator it_hostgroup_id;

    list<HYIMAGE_INFO_STRU> lstHyImgInfo;
    list<HYIMAGE_INFO_STRU>::iterator itHyImgInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    list<CMDHOSTINFO_STRU> lsthost;
    list<CMDHOSTINFO_STRU>::iterator ithost;
    list<HOST_PORT_INFO_STRU> rlstHostPortInfo;
    list<HOST_PORT_INFO_STRU>::iterator itHostPortInfo;
    list<NFS_INFO_STRU> lstNfsInfo;
    string vstore_id = STR_NOT_EXIST;

    COMMLOG(OS_LOG_INFO, "Commence: Query Temporary Snapshot Copy of [%s] for testFailoverStop request.",
        strid.c_str());
    print("Commence: Query Temporary Snapshot Copy of [%s] for testFailoverStop request.",
        strid.c_str());

    if (isNFS){
        ret = cmdOperate.CMD_showClonefsOffs(strid, lstHyImgInfo);
    } 
    else{
        ret = cmdOperate.CMD_showhyimgoflun(strid, lstHyImgInfo);
    }
    if (RETURN_OK != ret){
        err_info.code = OS_IToString(ret);
        return ret;
    }

    for (itHyImgInfo = lstHyImgInfo.begin(); itHyImgInfo != lstHyImgInfo.end(); itHyImgInfo++){
        if (snap_name == itHyImgInfo->strName){
            lst_snapshot_id.push_back(itHyImgInfo->strID);
            vstore_id = itHyImgInfo->vstoreID;
            break;
        }
    }

    
    if (lst_snapshot_id.empty()){
        
        COMMLOG(OS_LOG_WARN, "can not find snapshot by source lun(%s)", strid.c_str());
        return RETURN_OK;
    }
    snap_id = lst_snapshot_id.front();
    if (isNFS){
        
        COMMLOG(OS_LOG_INFO, "Commence: Deleting Nfs Share of Temporary Snapshot [%s] for testFailoverStop request.",
            snap_name.c_str());
        print("Commence: Deleting Nfs Share of Temporary Snapshot [%s] for testFailoverStop request.",
            snap_name.c_str());
        ret = cmdOperate.CMD_shownfs_byfsid(vstore_id, snap_id, lstNfsInfo);

        if (RETURN_OK != ret){
            err_info.code = OS_IToString(ret);
            return ret;
        }

        if (lstNfsInfo.empty()){
            COMMLOG(OS_LOG_WARN, "can not find NFS share by snapshot(%s)", snap_id.c_str());
        }
        else{     
            NFS_INFO_STRU nfsInfo = lstNfsInfo.front();
            ret = cmdOperate.CMD_delnfs(vstore_id, nfsInfo.strNfsID);
            if (RETURN_OK != ret){
                err_info.code = OS_IToString(ret);
                COMMLOG(OS_LOG_INFO, "Commence: Delete NFS Share of Temporary Snapshot %s fail, Error code: %s.",
                    snap_name.c_str(), err_info.code.c_str());
                return ret;
            }
        }

        
        COMMLOG(OS_LOG_INFO, "Commence: Deleting Temporary Snapshot [%s] of [%s] for testFailoverStop request.",
            snap_name.c_str(), strid.c_str());
        print("Commence: Deleting Temporary Snapshot [%s] of [%s] for testFailoverStop request.",
            snap_name.c_str(), strid.c_str());
        ret = cmdOperate.CMD_delclonefs(snap_id);
        if (RETURN_OK != ret){
            err_info.code = OS_IToString(ret);
            COMMLOG(OS_LOG_INFO, "Commence: Delete Temporary Snapshot %s fail, Error code: %s.",
                snap_name.c_str(), err_info.code.c_str());
            return ret;
        }
    }
    else{
        for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
            
            if (snap_id == itMapInfo->strDevLUNID){
                lst_map_id.push_back(itMapInfo->strMapID);
                lst_delete_map.push_back(*itMapInfo);
                
                lsthost.clear();
                cmdOperate.CMD_showhostByhostGroup(itMapInfo->strHostGroupID,lsthost);
                for(ithost  = lsthost.begin();ithost != lsthost.end();++ithost){
                    string strIQN = "";
                    string strCom = "";
                    cmdOperate.CMD_showhostport(ithost->strID,rlstHostPortInfo);
                    for(itHostPortInfo = rlstHostPortInfo.begin();itHostPortInfo != rlstHostPortInfo.end();itHostPortInfo++){
                        if(!itHostPortInfo->strIQN.empty() && itHostPortInfo->strIQN.size() > 1){
                            strIQN += strCom + itHostPortInfo->strIQN;
                            strCom = ",";
                        }
                    }
                    COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being masked from host [%s]",
                        snap_name.c_str(),
                        strIQN.c_str());
                    print( "Snapshot copy [%s] is being masked from host [%s]",
                        snap_name.c_str(),
                        strIQN.c_str());
                }
                
                lst_hostgroup_id.push_back(itMapInfo->strHostGroupID);
            }
        }

        for (itMapInfo = lst_delete_map.begin(); itMapInfo != lst_delete_map.end(); itMapInfo++){
            ret = _del_map_from_dar(cmdOperate, itMapInfo->strMapID, itMapInfo->strLunGroupID,  itMapInfo->strHostGroupID, itMapInfo->strDevLUNID);
            if (RETURN_OK != ret){
                (void)check_success_out("", err_info);
                return ret;
            }
        }

        mSnapname = snap_name;
        _delete_snapshot_out(cmdOperate, snap_id);

        if (lst_map_id.empty()){
            err_info.desc = "The snapshot is not mapped to ESX host.";
            err_info.hint = "Please check the information from array.";
            COMMLOG(OS_LOG_WARN, "can not find map id by lun snapshot(%s),maybe the snapshot is not mapped.", strid.c_str());

            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

void TestFailoverStop::_delete_snapshot_out(CCmdOperate& cmdOperate, string& snap_id)
{
    int ret = RETURN_ERR; 
    if (isNFS){
        COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being deleted", mSnapname.c_str());
        print("Snapshot copy [%s] is being deleted", mSnapname.c_str());
        ret = cmdOperate.CMD_delFShyimg(snap_id);
        if (RETURN_OK != ret){
            COMMLOG(OS_LOG_WARN, "can not delete snapshot(%s)", mSnapname.c_str());
            return;
        }

        return;
    }
    
    COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being offlined", mSnapname.c_str());
    print("Snapshot copy [%s] is being offlined", mSnapname.c_str());
    ret = cmdOperate.CMD_disablehyimg(snap_id);
    if (RETURN_OK != ret){
        COMMLOG(OS_LOG_WARN, "can not disable snapshot:(%s)", mSnapname.c_str());
        return;
    }

    OS_Sleep(5000);
    COMMLOG(OS_LOG_INFO, "Snapshot copy [%s] is being deleted", mSnapname.c_str());
    print("Snapshot copy [%s] is being deleted", mSnapname.c_str());
    ret = cmdOperate.CMD_delhyimg(snap_id);
    if (RETURN_OK != ret){

        COMMLOG(OS_LOG_WARN, "can not delete snapshot(%s)", mSnapname.c_str());
        return;
    }

    return;
}

int TestFailoverStop::_del_map_from_dar(CCmdOperate& cmdOperate, const string& mapID, const string& lunGroupID,  const string& hostGroupID, const string& snapID)
{
    int ret = RETURN_ERR;
  
    list<LUN_INFO_STRU> lstLunIDs;
    ret = cmdOperate.CMD_showLunByLunGroup(lunGroupID, lstLunIDs);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get lun from lunGroup[%s] failed", lunGroupID.c_str());
    }

    list<HYIMAGE_INFO_STRU> lstSnapShot;
    ret = cmdOperate.CMD_showSnapShotByLUNGroup(lunGroupID, lstSnapShot);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get snapshot from lunGroup[%s] failed", lunGroupID.c_str());
    }

    if (lstLunIDs.size() != 0 || lstSnapShot.size() != 1){
        COMMLOG(OS_LOG_WARN, "the Lun Number is [%u], the snapshot number is [%u] included by lunGroup[%s]  ", lstLunIDs.size(), lstSnapShot.size(), lunGroupID.c_str());
    }

    if (lstLunIDs.size() != 0 && lstSnapShot.size() != 0){
        list<HYIMAGE_INFO_STRU>::iterator itrLstSnapShot = lstSnapShot.begin();
        for (; itrLstSnapShot != lstSnapShot.end(); itrLstSnapShot++){
            if (snapID == itrLstSnapShot->strID){
                ret = cmdOperate.CMD_delLunFromLunGroup(snapID, lunGroupID, OBJ_SNAPSHOT);
                if ( ret != RETURN_OK){
                    COMMLOG(OS_LOG_ERROR, "snapshot [%s] can not remove from lunGroup [%s].", lstSnapShot.front().strID.c_str(), lunGroupID.c_str());
                    return ret;
                }

                return RETURN_OK;
            }
        }    
    }

    if (lstLunIDs.size() == 0 && lstSnapShot.size() == 1){
        ret = cmdOperate.CMD_delMapping(mapID, hostGroupID, lunGroupID);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "delete mapping faild by mapID[%s]", mapID.c_str());
            return ret;
        }

        ret = cmdOperate.CMD_delLunGroup(lunGroupID, lstSnapShot.front().strID);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "delete lunGroup faild by LunGroupID[%s], snapshotID[%s]", lunGroupID.c_str(), lstSnapShot.front().strID.c_str());
            return ret;
        }

        list<string> lstHostIDs;
        ret = cmdOperate.CMD_showHostbyHostGroup(hostGroupID, lstHostIDs);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "get host from hostGroup[%s] failed", hostGroupID.c_str());
            return ret;
        }

        ret = cmdOperate.CMD_delHostGroup(hostGroupID, lstHostIDs);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "delete hostGroup faild by hostGroupID[%s]", hostGroupID.c_str());
            return ret;
        }
    }

    return RETURN_OK;
}



