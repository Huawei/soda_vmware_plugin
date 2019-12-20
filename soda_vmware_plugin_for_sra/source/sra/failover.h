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

#ifndef FAIL_OVER_H
#define FAIL_OVER_H

#include "sra_basic.h"
#include "../common/xml_node/target_devices.h"
#include "../common/xml_node/target_groups.h"
#include "ThreeDCLun.h"

typedef struct tag_logInfo
{
    string strID;
    string strName;
    string strPoint;
}Log_Info;

int failover(XmlReader &reader);

class FailOver : public SraBasic
               , ThreeDCLunHelper
{
public:
    FailOver();
    virtual ~FailOver();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    int _outband_swap_consist(CCmdOperate& cmdOperate);
    int _outband_swap_consisthm(CCmdOperate& cmdOperate);
    int _outband_swap_hypermetro(CCmdOperate& cmdOperate);
    int _outband_swap_hypermirror(CCmdOperate& cmdOperate);
    int _check_slave_lun_map(string& lun_id);
    int _get_cg_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess, bool is_nfs);
    int _get_cghm_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess);
    int _exec_failover_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess, TargetGroupInfo& tginfo, bool is_nfs);
    int _exec_failover_hm_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, TargetGroupInfo& tginfo);
    int _exec_split_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo, bool is_nfs); 
    int _exec_split_hm_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo);
    int _get_consistency_percent_out(CCmdOperate& cmdOperate, list<HYMIRROR_INFO_STRU>& lstMirrorInfo, string& con_percent);
    int _get_consisgr_info_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo, bool is_nfs);
    int _get_consisgrhm_info_out(CCmdOperate& cmdOperate, string& cgid, TargetGroupInfo& tginfo);
    int _exec_hymirror_failover_out(CCmdOperate& cmdOperate, TargetDeviceInfo &targetinfo,string& lun_id,string& hymirror_id);
    int _exec_hmpair_failover_out(CCmdOperate& cmdOperate, TargetDeviceInfo &targetinfo,string& lun_id,string& hmpairid);
    int _exec_hymirror_split_out(CCmdOperate& cmdOperate, string& hymirror_id, TargetDeviceInfo& targetInfo);
    int _exec_hmpair_split_out(CCmdOperate& cmdOperate, string& hmpairid, TargetDeviceInfo& targetInfo);
    int getLunWwn(CCmdOperate &cmdOperate, string &sourceWwn, string strid);

    void deal_3dc_lun(CCmdOperate& cmdOperate);

private:
    char input_array_id[LENGTH_COMMON];
    string m_array_id;
    
    list<MAP_INFO_STRU> lstMapInfo;
    list<Log_Info>lstLogInfo;
    TargetGroupsInfo tg_groups;
    TargetDevicesInfo tg_devices;
    StoragePortsInfo storageports_info;
    //for 3dc
    ConGroupsInfo consistency_group_info;
    DevicesInfo devices_info;
    
    list<DAR_HOST_INFO> lstDARHosts;
    map<string, string> accessGroupDict; 
};

#endif
