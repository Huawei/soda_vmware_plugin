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

#include "ThreeDCLun.h"

int ThreeDCLun::full_ABInfo(CCmdOperate& cmdOperate,const string& lunId)
{
    id = lunId;
    
    //query ab relation info:
    //1 query all copy pair(with consis) todo
    //2 query all hypers(with consis)
    list<HYPERMETROPAIR_INFO_STRU> rlstHyperMetroPairInfo;
    CHECK_UNEQ_LOG(RETURN_OK,cmdOperate.CMD_showHyperMetroPair_all(rlstHyperMetroPairInfo));
    deal_hypers(cmdOperate,rlstHyperMetroPairInfo);
    return RETURN_OK;
}

bool ThreeDCLun::is3DC()
{
    return is3dc;
}

ThreeDCLun::ABInfo& ThreeDCLun::getABInfo()
{
    return ab_info;
}

ThreeDCLun::BCInfo& ThreeDCLun::getBCInfo()
{
    return bc_info;
}

void ThreeDCLun::assign_BCInfo(const BCInfo& bcinfo)
{
    bc_info = bcinfo;
}

void ThreeDCLun::ABInfo::dump()
{
    COMMLOG(OS_LOG_INFO,"id[%s],relation[%d],status[%d],access[%d]",id.c_str(),relation,status,access);
}

bool ThreeDCLun::is_nfs_lun(CCmdOperate& cmdOperate, string& fs_id)
{
    int iRet = RETURN_ERR;
    FS_INFO_STRU stFSInfo;
    stFSInfo.strID = fs_id;
    iRet = cmdOperate.CMD_showfs(stFSInfo);
    if (RETURN_OK != iRet){
        return false;
    }
    else{
        return true;
    }
}

void ThreeDCLun::deal_hypers(CCmdOperate& cmdOperate,list<HYPERMETROPAIR_INFO_STRU>& rlstHyperMetroPairInfo)
{
    for(list<HYPERMETROPAIR_INFO_STRU>::iterator it = rlstHyperMetroPairInfo.begin();it != rlstHyperMetroPairInfo.end();it++){
        list<HYPERMETRO_LF_INFO_STRU> rlstHMLUNInfo;
        cmdOperate.CMD_showhypermetrolun_all(it->strID, rlstHMLUNInfo);
        HYPERMETRO_LF_INFO_STRU& local = rlstHMLUNInfo.front();
        
        COMMLOG(OS_LOG_INFO,"expected lun id[%s],query lun_id[%s],expected array_id[%s],query array_id[%s]"
            ,id.c_str(),local.strID.c_str(),array_id.c_str(),local.strArrayWWN.c_str());
        if(local.strID == id && array_id == local.strArrayWWN){
            list<HYPERMETRO_LF_INFO_STRU>::iterator r_it = rlstHMLUNInfo.begin();
            HYPERMETRO_LF_INFO_STRU& remote = *(++r_it);
            
            
            if(local.strIsinCg == "true"){
                ab_info.id = it->strGroupID;
                ab_info.relation = Relation::CONSIS_HYPER;
                COMMLOG(OS_LOG_INFO,"find 3dc lun[consis hyper,consisId[ %s ]] in DataCenterB",ab_info.id.c_str());
            }
            else{
                ab_info.id = local.strHMpairID;
                ab_info.relation = Relation::PAIR_HYPER;
                COMMLOG(OS_LOG_INFO,"find 3dc lun[pair hyper,pairId[ %s ]] in DataCenterB",ab_info.id.c_str());
            }
            is3dc = true;
            ab_info.is_primary = (local.uiRelationType == LUN_RELATION_MASTER);
            ab_info.access = local.uiLocalAccess;
            ab_info.status = local.uiPairStatus;
            COMMLOG(OS_LOG_INFO,"the 3dc lun in DataCenterB read_write status[%d],running status[%d]",ab_info.access,ab_info.status);
            break;
        }

    }
}


int ThreeDCLunHelper::get_consis_info(const string& cg_id, string &lun_flag, string &consis_id)
{
    int iRet = RETURN_ERR;
    string hypermetroid;
    list<string> lst_token;
    list<string>::iterator token;

    OS_StrToken(cg_id, "_", &lst_token);
    if ((lst_token.size()) != 4){ 
        iRet = ERROR_INVALIDATE_DEVICE_ID;
        return iRet;
    } 

    token = lst_token.end();
    consis_id = *(--token);
    lun_flag = *(--token);

    return RETURN_OK;
}

int ThreeDCLunHelper::get_device_info_by_id(const string& device_id, string &lun_id, string &lun_flag, string &pair_id)
{
    int iRet = RETURN_ERR;
    string hypermetroid;
    list<string> lst_token;
    list<string>::iterator token;

    OS_StrToken(device_id, "_", &lst_token);
    if ((lst_token.size()) != 4){ 
        iRet = ERROR_INVALIDATE_DEVICE_ID;
        return iRet;
    } 

    token = lst_token.begin();
    lun_id = *(++token);
    lun_flag = *(++token);
    pair_id = *(++token);

    return RETURN_OK;
}

int ThreeDCLunHelper::find_luns_from_consis(CCmdOperate& cmdOperate)
{
    for(list<ConGroupInfo>::iterator it = m_consistency_group_info.lst_groups_info.begin();
        it != m_consistency_group_info.lst_groups_info.end();it++){
        string flag,consis_id;
        int ret = get_consis_info(it->cg_id,flag,consis_id);
        if(RETURN_OK != ret){
            COMMLOG(OS_LOG_INFO, "Get device id failed!");
            it->is_success = false;
            return ret;
        }

        COMMLOG(OS_LOG_INFO,"hanlde consis[%s] result: flag[%s],consis_id[%s]",it->cg_id.c_str(),flag.c_str(),consis_id.c_str());

        ThreeDCLun::BCInfo bc_info;
        bc_info.id = consis_id;
        if(flag == "RG"){
            bc_info.relation = ThreeDCLun::CONSIS_COPY;
        }
        else if(flag == "MG"){
            bc_info.relation = ThreeDCLun::CONSIS_HYPER;
        }
        else{
            COMMLOG(OS_LOG_ERROR,"error flag[%s]",flag.c_str());
            return RETURN_ERR;
        }

        //query all lun in the consis
        vector<string> lun_ids;
        CHECK_UNEQ_LOG(RETURN_OK,query_all_luns_by_consis_id(cmdOperate,flag,consis_id,lun_ids));

        COMMLOG(OS_LOG_INFO,"query all pair in consis[ consis_id[%s],luns_size[%d] ]"
            ,consis_id.c_str(),lun_ids.size());

        for(vector<string>::iterator lun = lun_ids.begin(); lun != lun_ids.end();lun++){
            ThreeDCLun three_dc_lun(m_input_array_id);
            three_dc_lun.assign_BCInfo(bc_info);
            if(three_dc_lun.full_ABInfo(cmdOperate,*lun) != RETURN_OK){
                COMMLOG(OS_LOG_ERROR,"handle ABInfo fail");
                return RETURN_ERR;
            }
            if(three_dc_lun.is3DC()&& !three_dc_lun.is_nfs_lun(cmdOperate,*lun)){
                three_dc_luns.push_back(three_dc_lun);
                COMMLOG(OS_LOG_INFO,"add consis[ consis_id[%s],lun_id[%s] ] 3DC luns",consis_id.c_str(),lun->c_str());
            }
        }
    }

    return RETURN_OK;
}

int ThreeDCLunHelper::find_luns_from_devices(CCmdOperate& cmdOperate)
{
    for(list<DeviceInfo>::iterator it_device = m_devices_info.lst_device_info.begin();
        it_device != m_devices_info.lst_device_info.end();it_device++){
        string flag,pair_id,lun_id;
        int ret = get_device_info_by_id(it_device->id,lun_id,flag,pair_id);

        if(RETURN_OK != ret){
            COMMLOG(OS_LOG_INFO, "Get device id failed!");
            it_device->is_success = false;
            return ret;
        }

        COMMLOG(OS_LOG_INFO,"handle device[%s] result: lun_id[%s],flag[%s],pair_id[%s]"
            ,it_device->id.c_str(),lun_id.c_str(),flag.c_str(),pair_id.c_str());

        ThreeDCLun::BCInfo bc_info;
        bc_info.id = pair_id;
        if(flag == "RP"){
            bc_info.relation = ThreeDCLun::PAIR_COPY;
        }
        else if(flag == "MP"){
            bc_info.relation = ThreeDCLun::PAIR_HYPER;
        }
        else{
            COMMLOG(OS_LOG_ERROR,"error flag[%s]",flag.c_str());
            return RETURN_ERR;
        }

        ThreeDCLun three_dc_lun(m_input_array_id);
        three_dc_lun.assign_BCInfo(bc_info);
        if(three_dc_lun.full_ABInfo(cmdOperate,lun_id) != RETURN_OK){
            COMMLOG(OS_LOG_ERROR,"handle ABInfo fail");
            return RETURN_ERR;
        }
        if(three_dc_lun.is3DC() && !three_dc_lun.is_nfs_lun(cmdOperate,lun_id)){
            three_dc_luns.push_back(three_dc_lun);
            COMMLOG(OS_LOG_INFO,"add device[%s] to 3DC luns",it_device->id.c_str());
        }
    }

    return RETURN_OK;
}

int ThreeDCLunHelper::query_all_luns_by_consis_id(CCmdOperate& cmdOperate,const string& flag,const string& consis_id,vector<string>& lun_ids)
{
    if(flag == "MG"){
        COMMLOG(OS_LOG_WARN,"can not support this model,just support[A--hyper--B--copy--C]");
        return RETURN_OK;
    }
    else if(flag == "RG"){
        return cmdOperate.CMD_showlunsbyconsisid(consis_id,lun_ids);
    }
    else{
        COMMLOG(OS_LOG_ERROR,"error flag[%s]",flag.c_str());
        return RETURN_ERR;
    }

    return RETURN_OK;
}