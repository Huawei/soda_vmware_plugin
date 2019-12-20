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

#include "prepare_reverse.h"

int prepare_reverse(XmlReader &reader)
{
    PrepareReverse pre_reverse;

    COMMLOG(OS_LOG_INFO, "%s", "prepare_reverse begin.");
    print("%s", "prepare_reverse begin.");
    if (RETURN_OK != pre_reverse.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "failed to prepare reverse.");
        return RETURN_ERR;
    }
    
    COMMLOG(OS_LOG_INFO, "%s", "prepare_reverse end.");
    print("%s", "prepare_reverse end.");
    return RETURN_OK;
}



int PrepareReverse::_read_command_para(XmlReader &reader)
{
    char peer_array[LENGTH_COMMON] = {0};
    char curr_array_id[LENGTH_COMMON] = {0};

    CHECK_FALSE(reader.get_string("/Command/PrepareReverseReplicationParameters/ArrayId", array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/PrepareReverseReplicationParameters/ArrayId", curr_array_id, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string("/Command/PrepareReverseReplicationParameters/PeerArrayId", peer_array, 0), RETURN_ERR);

    peer_array_id = peer_array;
    input_array_id = string(curr_array_id);
    
    int cg_count = reader.get_count("/Command/PrepareReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup");
    for (int i = 0; i < cg_count; i++){
        char cg_id[LENGTH_COMMON] = {0};
        ConGroupInfo cg_info;

        reader.get_string("/Command/PrepareReverseReplicationParameters/ConsistencyGroups/ConsistencyGroup/@id", cg_id, i);
        
        
        cg_info.cg_id = cg_id;
        cg_info.is_success = true;
        consistency_group_info.lst_groups_info.push_back(cg_info);
    }

    int device_count = reader.get_count("/Command/PrepareReverseReplicationParameters/Devices/Device");
    for (int i = 0; i < device_count; i++){
        char dev_id[LENGTH_COMMON] = {0};
        DeviceInfo dev_info;

        reader.get_string("/Command/PrepareReverseReplicationParameters/Devices/Device/@id", dev_id, i);
        
        
        dev_info.id = dev_id;
        
        dev_info.is_success = true;
        devices_info.lst_device_info.push_back(dev_info);
    }

    return RETURN_OK;
}

PrepareReverse::PrepareReverse():
    ThreeDCLunHelper(this->input_array_id,this->consistency_group_info,this->devices_info)
{
}

PrepareReverse::~PrepareReverse()
{
}

int PrepareReverse::_outband_process()
{
    HYPER_STORAGE_STRU stStorageInfo;
    CCmdOperate cmdOperate;

    (void)setStorageInfo(stStorageInfo);

    int iRet = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, iRet);

    CHECK_UNEQ_LOG(RETURN_OK, _get_array_info(cmdOperate));
    string cur_array_id(array_id);
    CHECK_UNEQ_LOG(RETURN_OK,check_array_id_validate_out(cmdOperate,cur_array_id,peer_array_id));

    iRet = find_luns_from_consis(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);

    iRet = find_luns_from_devices(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);

    if(three_dc_luns.empty()){
        set_all_success();
    }
    else{
        iRet = deal_3dc_lun(cmdOperate);  
        CHECK_UNEQ(RETURN_OK, iRet);
    }

    
    dealHyperMirror(cmdOperate);
    dealConsistHyperMirror(cmdOperate);

    return RETURN_OK;
}

void PrepareReverse::dealHyperMirror(CCmdOperate& cmdOperate)
{
    int iRet = RETURN_OK;

    list<DeviceInfo>::iterator iter;
    for (iter = devices_info.lst_device_info.begin(); iter != devices_info.lst_device_info.end(); ++iter){
        string sn;
        string lunId;
        string lunFlag;
        string HymirrorId;

        iRet = get_device_info(iter->id, sn, lunId, lunFlag, HymirrorId);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            iter->is_success = false;
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->id.c_str());
            continue;
        }

        if (lunFlag.compare(MIRROR_LUN_TAG)){
            COMMLOG(OS_LOG_WARN, "the lunFlag of device(%s) is not RP.", iter->id.c_str());
            continue;
        }

        if (HymirrorId.empty()){
            COMMLOG(OS_LOG_WARN, "the hymirrorId of device(%s) is empty.", iter->id.c_str());
            continue;
        }

        is_Fs_by_lun(cmdOperate, lunId);

        
        string strTime;
        string key = sn + "." + lunId;
        strTime = OS_Time_tToStr(OS_Now());
        COMMLOG(OS_LOG_INFO,"Replica of [%s] with key [%s] on array [%s] is being promoted from Protect Point[%s]",
            HymirrorId.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());

        
        if (!isNFS && !g_bFusionStorage){
            iRet = execSparseHymirror(cmdOperate, lunId, HymirrorId, RESOURCE_ACCESS_READ_ONLY);

            if (iRet != RETURN_OK){
                iter->error_info.code = OS_IToString(iRet);
                iter->is_success = false;
                COMMLOG(OS_LOG_ERROR, "exec execSparseHymirror for lun_id[%s] failed, error code id [%d]", lunId.c_str(), iRet);
                continue;
            }
        }
    }
}

void PrepareReverse::dealConsistHyperMirror(CCmdOperate& cmdOperate)
{
    int iRet = RETURN_OK;
    list<ConGroupInfo>::iterator iter;

    
    for (iter = consistency_group_info.lst_groups_info.begin(); iter != consistency_group_info.lst_groups_info.end(); ++iter){
        if (iter->error_info.hasError()){
            ++iter;
            continue;
        }
        
        string sn;
        string lunId;
        string lunFlag;
        string cgId;
        iRet = get_device_info(iter->cg_id, sn, lunId, lunFlag, cgId);
        if (RETURN_OK != iRet){
            iter->error_info.code = OS_IToString(iRet);
            iter->is_success = false;
            COMMLOG(OS_LOG_ERROR, "id of device(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        if (lunFlag.compare(MIRROR_LUN_CONSISTENT_TAG)){
            COMMLOG(OS_LOG_WARN, "the lunFlag of device(%s) is not RG.", iter->cg_id.c_str());
            continue;
        }

        
        if (cgId.empty()){
            COMMLOG(OS_LOG_WARN, "the cgId of device(%s) is empty.", iter->cg_id.c_str());
            continue;
        }

        
        if ("--" == cgId){
            iter->error_info.code = OS_IToString(ERROR_INVALIDATE_GROUP_ID);
            iter->is_success = false;
            COMMLOG(OS_LOG_ERROR, "id of consisgroup(%s) is wrong.", iter->cg_id.c_str());
            continue;
        }

        is_Fs_by_lun(cmdOperate, lunId);

        
        string strTime;
        string key = sn + "." + lunId;
        strTime = OS_Time_tToStr(OS_Now());

        COMMLOG(OS_LOG_INFO,"Consist Replica of [%s] with key [%s] on array [%s] is being promoted from Protect Point[%s]",
            cgId.c_str(),
            key.c_str(),
            sn.c_str(),
            strTime.c_str());

        
        if (!isNFS && !g_bFusionStorage){
            iRet = execSparseConsisgr(cmdOperate, cgId, RESOURCE_ACCESS_READ_ONLY);
            if(iRet != RETURN_OK){
                iter->error_info.code =  OS_IToString(iRet);
                iter->is_success = false;
                COMMLOG(OS_LOG_ERROR, "execSparseConsisgr by cgid [%s]  failed, the error code is [%d]", cgId.c_str(), iRet);
                continue;
            }
        }
    }
}


void PrepareReverse::set_all_success()
{
    for(list<DeviceInfo>::iterator it_device = devices_info.lst_device_info.begin();
        it_device != devices_info.lst_device_info.end();it_device++){
        it_device->is_success = true;
    }

    for(list<ConGroupInfo>::iterator it_consis = consistency_group_info.lst_groups_info.begin();
        it_consis != consistency_group_info.lst_groups_info.end();it_consis++){
        it_consis->is_success = true;
    }
}

void PrepareReverse::set_device_result(const string& pair_id,int err)
{
    for(list<DeviceInfo>::iterator it_device = devices_info.lst_device_info.begin();
        it_device != devices_info.lst_device_info.end();it_device++){
        string device_id = it_device->id;
        string lun_id,pair,flag;
        get_device_info_by_id(device_id,lun_id,flag,pair);
        if(pair == pair_id){
            if(err == RETURN_OK){
                it_device->is_success = true;
            }
            else{
                it_device->error_info.code = OS_IToString(err);
            }
            
        }
        else{
            it_device->is_success = true;
        }
    }
}

void PrepareReverse::set_consis_result(const string& consis_id,int err)
{
    for(list<ConGroupInfo>::iterator it_consis = consistency_group_info.lst_groups_info.begin();
        it_consis != consistency_group_info.lst_groups_info.end();it_consis++){
        string consis_id = it_consis->cg_id;
        string consis,flag;
        get_consis_info(consis_id,flag,consis);
        if(consis == consis_id){
            if(err == RETURN_OK){
                it_consis->is_success = true;
            }
            else{
                it_consis->error_info.code = OS_IToString(err);
            }
        }
        else{
            it_consis->is_success = true;
        }
    }
}

int PrepareReverse::do_deal_AB(CCmdOperate& cmdOperate,const ThreeDCLun::ABInfo& ab_info)
{
    string sn,slave_lun;
    string pair_id = ab_info.id;
    if(ab_info.relation == ThreeDCLun::PAIR_HYPER && ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_NORMAL){
         CHECK_UNEQ_LOG(RETURN_OK,cmdOperate.CMD_disablehcpair(pair_id,sn,slave_lun,!ab_info.is_primary));
    }
    else if(ab_info.relation == ThreeDCLun::CONSIS_HYPER && ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_NORMAL){
        CHECK_UNEQ_LOG(RETURN_OK,cmdOperate.CMD_stopconsistgrhm(ab_info.id,!ab_info.is_primary));
    }
    else if(ab_info.relation == ThreeDCLun::CONSIS_COPY || ab_info.relation == ThreeDCLun::PAIR_COPY){
        COMMLOG(OS_LOG_ERROR,"can not support the model[%d]",ab_info.relation);
        return RETURN_ERR;
    }
    else{
        COMMLOG(OS_LOG_ERROR,"unknown model[%d]",ab_info.relation);
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int PrepareReverse::deal_AB(CCmdOperate& cmdOperate,const ThreeDCLun::ABInfo& ab_info)
{
    int ret = RETURN_ERR;
    if(ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_NORMAL){
        ret = do_deal_AB(cmdOperate,ab_info);
        if(ab_info.access != RESOURCE_ACCESS_READ_WRITE){
            COMMLOG(OS_LOG_ERROR,"dataCenterB is not in read_write state[%d]",ab_info.access);
            ret = RETURN_ERR;
        }
    }
    else if(ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_PAUSE 
            || ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_ERROR || ab_info.status == HM_SLAVE_PAIR_STATUS_CHAR_INVALID){
        if(ab_info.access != RESOURCE_ACCESS_READ_WRITE){
            COMMLOG(OS_LOG_ERROR,"dataCenterB is not in read_write state[%d]",ab_info.access);
            ret = RETURN_ERR;
        }
        ret = RETURN_OK;
    }
    else{
        COMMLOG(OS_LOG_ERROR,"unknown hyper status[%d]",ab_info.status);
        ret = RETURN_ERR;
    }

    return ret;
}

int PrepareReverse::deal_3dc_lun(CCmdOperate& cmdOperate)
{
    COMMLOG(OS_LOG_INFO,"exec 3DC......");
    set<string> sync_consiss,sync_pairs;
    for(vector<ThreeDCLun>::iterator it_lun = three_dc_luns.begin();it_lun != three_dc_luns.end();it_lun++){
        COMMLOG(OS_LOG_INFO,"pair_id[%s],relation[%d]",it_lun->getABInfo().id.c_str(),it_lun->getABInfo().relation);
        ThreeDCLun& lun = *it_lun;
        ThreeDCLun::ABInfo& ab_info = lun.getABInfo();
        ThreeDCLun::BCInfo& bc_info = lun.getBCInfo();
        if(ab_info.relation == ThreeDCLun::PAIR_HYPER ){
            if(sync_pairs.find(ab_info.id) != sync_pairs.end()){
                continue;
            }
            
            int ret = deal_AB(cmdOperate,ab_info);

            
            set_device_result(bc_info.id,ret);
            if(ret == RETURN_OK){
                sync_pairs.insert(ab_info.id);
            }
        }
        else if(ab_info.relation == ThreeDCLun::CONSIS_HYPER ){
            if(sync_consiss.find(ab_info.id) != sync_consiss.end()){
                continue;
            }
            int ret = deal_AB(cmdOperate,ab_info);
            
            set_consis_result(bc_info.id,ret);
            if(ret == RETURN_OK){
                sync_consiss.insert(ab_info.id);
            }
        }
        else if(ab_info.relation == ThreeDCLun::CONSIS_COPY || ab_info.relation == ThreeDCLun::PAIR_COPY){
            COMMLOG(OS_LOG_WARN,"can not support the model[%d],id[%s]",ab_info.relation,ab_info.id.c_str());
        }
        else{
            COMMLOG(OS_LOG_ERROR,"error model[%d],id[%s]",ab_info.relation,ab_info.id.c_str());
            return RETURN_ERR;
        }
    }
    return RETURN_OK;
}

void PrepareReverse::_write_response(XmlWriter &writer)
{
    writer.set_xml("/Response/PrepareReverseReplicationResults", &consistency_group_info);
    
    writer.set_xml("/Response/PrepareReverseReplicationResults", &devices_info);
}
