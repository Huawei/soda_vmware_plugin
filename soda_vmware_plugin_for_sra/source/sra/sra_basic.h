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

#ifndef SRA_BASIC_H
#define SRA_BASIC_H

#include "common.h"
#include "commond_info.h"
#include "CmdOperate.h"
#include "../common/xml_node/target_devices.h"
#include "../common/xml_node/target_groups.h"


#define VSSRA_SNAPSHOT_PREFIX               "snapshot_for_vssra_"
#define VSSRA_CLONE_LUN                     "_clone_lun"
#define VSSRA_BACKUP_PREFIX                 "backup_for_vssra_"
#define SYNC_PREFIX                         "sync_"

#define VSSRA_HOST_PREFIX                   "vssra_host_"
#define VSSRA_HOSTGROUP_PREFIX              "vssra_hostgrp_"
#define VSSRA_LUNGROUP_PREFIX               "vssra_lungrp_of_"
#define VSSRA_MAPPINGVIEW_PREFIX            "vssra_mv_of_"

extern char LOG_LEVEL[10];

class CommonLunInfo
{
public:
    CommonLunInfo() : is_slave(false){};
    ~CommonLunInfo() {};
public:
    string array_id;
    string lun_id;
    string lun_name;
    string mirror_id;
    bool is_slave;

};

class SraBasic
{
public:

    SraBasic();
    virtual ~SraBasic();

    int entry(XmlReader &reader);
    void read_xml(XmlReader &reader);
    void write_xml(XmlWriter &writer);
    void process();

    int get_consist_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& consist_id, list<HYMIRROR_INFO_STRU>& lstMirrorInfo);
    int get_consisthm_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& consist_id, list<HYPERMETROPAIR_INFO_STRU>& lstHMPairInfo);
    int get_device_info(string device_id, string &arrayid, string &lun_id, string &lun_flag, string &pair_id);

    int get_hymirror_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hymirror_id);
    int get_hypermetroid_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hypermetroid);
    int get_remote_hypermetroid_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hypermetroid);
    int get_hymirror_by_fs_out(CCmdOperate& cmdOperate, string& fs_id, string& hymirror_id);
    void is_Fs_by_lun(CCmdOperate& cmdOperate, string& fs_id);
    
    int check_device_id_validate_out(CCmdOperate& cmdOperate, string& device_id, string& relation);

    std::vector<std::string> split(std::string str, std::string pattern);
    int _get_lungroup_by_lun(CCmdOperate& cmdOperate, const string& lun_id, const int type, string& lungroup_id);
    int _get_host_by_initiator(CCmdOperate& cmdOperate, const string& initiator_id, const string& type, CMDHOSTINFO_STRU& hostInfo, const string& lun_id);
    int _create_hostgroup(CCmdOperate& cmdOperate, list<CMDHOSTINFO_STRU>& hosts, const string& lun_id, list<string>& hostGroupIDs);
    int _create_mappingview(CCmdOperate& cmdOperate, const string& mappingName, const string& lunGrpID, const string& hostGrpID, string mapID="");
    int _do_mapping(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, const string& hostGrpID, const int type);
    int _add_map(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, list<DAR_HOST_INFO>& lstDARHosts, list<CMDHOSTINFO_STRU>& lstHosts, string& lun_id, int obj_type);
    int _select_valid_initiator(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, list<DAR_HOST_INFO>& lstDARHosts, map<string, vector<CMDHOSTINFO_STRU>>& mapHostInfo, map<string, vector<DAR_HOST_INFO>>& mapInitiatorNoHost);
    int _query_lun_mapping_info(CCmdOperate& cmdOperate, const string& lun_id, const int obj_type, map<string, vector<CMDHOSTINFO_STRU>>& mapHostInfo, vector<string>& mappedInfo);
    int _query_and_create_host(CCmdOperate& cmdOperate, const string& lun_id, vector<string>& mappedInfo, map<string, vector<CMDHOSTINFO_STRU>>& mapHostInfo, map<string, vector<DAR_HOST_INFO>>& mapInitiatorNoHost);
    int _create_lun_group(CCmdOperate& cmdOperate, const string& lun_id, string& lunGroupID, int obj_type);
    int _create_host_group(CCmdOperate& cmdOperate, const string& lun_id, string& hostGroupID, list<CMDHOSTINFO_STRU> &lstHosts);
    int _create_mapping(CCmdOperate& cmdOperate, const string& lun_id, const string& lunGrpID, const string& hostGrpID);
    int execSparseHymirror(CCmdOperate &cmdOperate, string& lunId, string& hymirrorId, RESOURCE_ACCESS_E accessState);
    int changeSparseHymirrorAccessState(CCmdOperate &cmdOperate, string& hymirrorId, RESOURCE_ACCESS_E accessState);
    int execSparseConsisgr(CCmdOperate &cmdOperate, string& cgId, RESOURCE_ACCESS_E accessState);
    int querySparseHymirror(CCmdOperate &cmdOperate, string& lunId, std::vector<string> &vHymirrorIDs);
    int changeSparseConsisgrAccessState(CCmdOperate &cmdOperate, string& cgId, RESOURCE_ACCESS_E accessState);

protected:
    
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    int _get_array_info(CCmdOperate& cmdOperate);
    int _get_hymirror_percent_out(CCmdOperate& cmdOperate, string& hymirror_id, string& query_percent);
    int _get_hmpair_percent_out(CCmdOperate& cmdOperate, string& hymirror_id, string& query_percent);
    int setStorageInfo(HYPER_STORAGE_STRU& stStorageInfo);

    virtual void _write_progress(unsigned int percent);

    void _convert_relation_type(const unsigned int& uiRelationType, string& relation);
    void _convert_hmrelation_type(const unsigned int& uiRelationType, string& relation);
    void _convert_pair_status(const unsigned int& uiPairStatus, string& pair_status);
    void _convert_hmpair_status(const unsigned int& uiPairStatus, string& pair_status);
    
    char id[LENGTH_COMMON];
    char slaveid[LENGTH_COMMON];
    char user_name[LENGTH_COMMON];
    char password[LENGTH_COMMON];

    char array_id[LENGTH_COMMON];
    char array_stretched[LENGTH_COMMON];
    string lunName;
    string fsName;
    bool isNFS;

    string strHyID;

private:
    int _read_common_info(XmlReader &reader);
    int _write_error_info(XmlWriter &writer);

private:
    char xmlns[LENGTH_XMLNS];
    char output_file[LEGNTH_PATH];
    char status_file[LEGNTH_PATH];
    char log_dir[LEGNTH_PATH]; 
    char log_level[LENGTH_COMMON]; 

    int error_code;
};

string  NowTime();

void print(const char *pszFormat, ...);

void filter_list(list<string>& rlist);

int parse_device_id(const string& device_id, string& sn, string& lun_id, string& hyper_id);

int check_success_out(const string& errorMsg, ErrorInfo& error_info);

void format_daytime(string& daytime);

int get_lun_by_consisgr_out(CCmdOperate& cmdOperate, unsigned int role, string& consisgr_id, list<CommonLunInfo>& lst_lun_info, bool is_nfs);

int get_lun_by_consisgrhm_out(CCmdOperate& cmdOperate, unsigned int role, string& consisgr_id, list<CommonLunInfo>& lst_lun_info);

#endif

