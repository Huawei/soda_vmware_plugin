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

#ifndef PREPARE_FAILOVER_H
#define PREPARE_FAILOVER_H

#include "sra_basic.h"
#include "../common/xml_node/consistency_groups.h"
#include "../common/xml_node/device.h"

int prepare_failover(XmlReader &reader);

class PrepareFailove : public SraBasic
{
public:
    PrepareFailove();
    virtual ~PrepareFailove();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual int _outband_process();
    virtual void _write_response(XmlWriter &writer);
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    int _operate_snapshot(const string& lun_id);
    int _create_snapshot(const string& lun_id);
    int _recreate_snapshot(const string& snapshot_id);
    int _active_snapshot(const string& snapshot_id);
    int _get_snapshot_id(const string& lun_id, string& snapshot_id);
    void _operate_lun_snapshot(const string& cg_id, list<CommonLunInfo>& lst_lun_info);

    int _outband_swap_consist(CCmdOperate& cmdOperate);
    int _outband_swap_consisthm(CCmdOperate& cmdOperate);
    int _outband_swap_hypermirror(CCmdOperate& cmdOperate);
    int _outband_swap_hypermetro(CCmdOperate& cmdOperate);
    int _get_cghm_status_out(CCmdOperate& cmdOperate, string& cgid, string& status, string& role, unsigned int &uiResAccess);


private:
    char array_id[LENGTH_COMMON];
    char input_array_id[LENGTH_COMMON];
    list<string> lst_cgs_id;
    list<string> lst_sources_id;
    list<string> lst_cgs_lun_id;

    ConGroupsInfo group_info;
    SourceDevicesInfo devices_info;

    map<string, string> map_cgs_errors;
    map<string, string> map_source_errors;
};


#endif
