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

#ifndef TEST_FAILOVER_STOP_H
#define TEST_FAILOVER_STOP_H

#include "sra_basic.h"
#include "../common/xml_node/target_groups.h"

int test_failover_stop(XmlReader &reader);

class TestFailoverStop : public SraBasic
{
public:
    TestFailoverStop();
    virtual ~TestFailoverStop();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    void _deal_consistentgroup_out(CCmdOperate& cmdOperate);
    int _delete_snapshot_fusion(string& name, CCmdOperate& cmdOperate,list<HYIMAGE_INFO_STRU>& snapshotlist);
    int _do_failover_stop_fusion(CCmdOperate& cmdOperate);
    void _deal_consistentgrouphm_out(CCmdOperate& cmdOperate);
    void _deal_hymirror_out(CCmdOperate& cmdOperate);
    void _deal_hypermetro_out(CCmdOperate& cmdOperate);
    void _del_lun_snapshot_out(CCmdOperate& cmdOperate, list<CommonLunInfo>& lst_lun_info, ErrorInfo& errorinfo, WarnInfo& warn_info);
    int _stop_test_device_out(CCmdOperate& cmdOperate, string& lun_id, ErrorInfo& err_info, WarnInfo& warnInfo);
    void _delete_snapshot_out(CCmdOperate& cmdOperate, string& snap_id);

    int _del_map_from_dar(CCmdOperate& cmdOperate, const string& mapID, const string& lunGroupID,  const string& hostID, const string& snapID);

private:
    char input_array_id[LENGTH_COMMON];

    list<MAP_INFO_STRU> lstMapInfo;

    TargetGroupsInfo targetconsisgrs_info;
    TargetDevicesInfo tartgetdevs_info;

    string mSnapname;
};


#endif

