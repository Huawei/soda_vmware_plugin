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

#ifndef QUERY_SYNCSTATUS_H
#define QUERY_SYNCSTATUS_H

#include "sra_basic.h"
#include "../common/xml_node/source_devices.h"
#include "../common/xml_node/consistency_groups.h"

int query_syncstatus(XmlReader &reader);

#define INPROGRESS_STATUS "inProgress"
#define COMPLETE_STATUS "complete"

#define REMAIN_SYNC_TIME "100"

class QuerySyncStatus : public SraBasic
{
public:
    QuerySyncStatus() : SraBasic() 
    {
        memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
    }
    virtual ~QuerySyncStatus() {};

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");
private:
    int _query_consistency_info_out(CCmdOperate& cmdOperate);
    int _query_consistencyhm_info_out(CCmdOperate& cmdOperate);
    int _query_hymirror_info_out(CCmdOperate& cmdOperate);
    int _query_hypermetro_info_out(CCmdOperate& cmdOperate);
    int _get_consistency_info_out(CCmdOperate& cmdOperate, list<HYMIRROR_INFO_STRU>& lstMirrorInfo, ConGroupInfo& consistency);
    int _get_consistencyhm_info_out(CCmdOperate& cmdOperate, string cg_id, list<HYPERMETROPAIR_INFO_STRU> &lstHMPairInfo, ConGroupInfo& consistency);

private:
    string peer_array_id;
    char input_array_id[LENGTH_COMMON];

    ConGroupsInfo consistency_group_info;
    SourceDevicesInfo source_device_info;
    TargetDevicesInfo target_device_info;
};

#endif

