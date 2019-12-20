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

#ifndef THREE_DC_LUN_0110_H
#define THREE_DC_LUN_0110_H

#include "sra_basic.h"
#include "../common/xml_node/device.h"
#include "../common/xml_node/consistency_groups.h"
#include <vector>

struct ThreeDCLun
{
    /*
    DataCenterA                         DataCenterB                          DataCenterC
    lunA(ConsisGroup)                   lunB(ConsisGroup)                    lunC(ConsisGroup)
    */
    enum Relation{PAIR_COPY,PAIR_HYPER,CONSIS_COPY,CONSIS_HYPER};
    struct ABInfo
    {
        ABInfo():is_primary(false),status(-1),access(-1),relation(PAIR_COPY){}
        string id; //pair_id or consis_id
        Relation relation;
        bool is_primary;
        int status;
        int access;
        void dump();
    };
    struct BCInfo
    {
        Relation relation;
        string id; //pair_id or consis_id
    };

    ThreeDCLun(const string& array_id):array_id(array_id),is3dc(false){}

    int full_ABInfo(CCmdOperate& cmdOperate,const string& lun_id);
    void assign_BCInfo(const BCInfo& bcinfo);
    bool is_nfs_lun(CCmdOperate& cmdOperate, string& fs_id);
    bool is3DC();
    ABInfo& getABInfo();
    BCInfo& getBCInfo();
private:
    void deal_hypers(CCmdOperate& cmdOperate,list<HYPERMETROPAIR_INFO_STRU>& rlstHyperMetroPairInfo);
private:
    bool is3dc;
    string id; //lunid

    string array_id; 
    ABInfo ab_info;
    BCInfo bc_info;
};

struct ThreeDCLunHelper
{
    ThreeDCLunHelper(string& input_array_id,ConGroupsInfo& consistency_group_info,DevicesInfo& devices_info):
        m_input_array_id(input_array_id),m_consistency_group_info(consistency_group_info),m_devices_info(devices_info){}
    virtual ~ThreeDCLunHelper(){}
    int get_device_info_by_id(const string& device_id, string &lun_id, string &lun_flag, string &pair_id);
    int get_consis_info(const string& cg_id, string &lun_flag, string &consis_id);
    int find_luns_from_consis(CCmdOperate& cmdOperate);
    int find_luns_from_devices(CCmdOperate& cmdOperate);
    int query_all_luns_by_consis_id(CCmdOperate& cmdOperate,const string& flag,const string& consis_id,vector<string>& lun_ids);
private:
    const string& m_input_array_id;
    ConGroupsInfo& m_consistency_group_info;
    DevicesInfo& m_devices_info;
protected:
    vector<ThreeDCLun> three_dc_luns;
};


#endif