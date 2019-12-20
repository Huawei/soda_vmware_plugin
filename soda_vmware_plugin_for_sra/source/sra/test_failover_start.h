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

#ifndef TEST_FAILOVER_START_H
#define TEST_FAILOVER_START_H

#include "sra_basic.h"
#include <set>
#include "../common/xml_node/target_devices.h"
#include "../common/xml_node/target_groups.h"

int test_failover_start(XmlReader &reader);

class TestFailoverStart : public SraBasic
{
public:
    TestFailoverStart();
    virtual ~TestFailoverStart();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    void _split_target_info(TargetDevicesInfo& tg_devices);

    void _operate_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device);
    int _active_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device);
    int _recreate_snapshot_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device);
    int _get_snapshot_info_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device, int& errorCode);
    int _get_active_time_out(CCmdOperate& cmdOperate,HYIMAGE_INFO_STRU& snapshot, string& time_stamp);
    int _and_map_out(CCmdOperate& cmdOperate,TargetDeviceInfo& target_device);
    int _map_snapshot(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device);

    int _get_consist_devices_out(CCmdOperate& cmdOperate,TargetDevicesInfo& tg_devices);
    void _operate_snapshots_out(CCmdOperate& cmdOperate,TargetDevicesInfo& tg_devices);
    int _map_snapshot_to_host_out(CCmdOperate& cmdOperate,TargetDevicesInfo& tg_devices);
    int _get_snapshot_wwn_out(CCmdOperate& cmdOperate,TargetDevicesInfo& tg_devices);
    void _check_status_sync_out(CCmdOperate& cmdOperate);
    void _check_statushm_sync_out(CCmdOperate& cmdOperate);
    void _find_host_and_hostGroups_for_fusion(const list<MAP_INFO_STRU>& rlstMapInfo, const list<MAP_INFO_STRU>& rlstSnapMapInfo, \
        set<string>& hosts, set<string>& hostGroups);
private:
    char input_array_id[LENGTH_COMMON];

    TargetGroupsInfo tg_groups;
    TargetDevicesInfo tg_devices;
    StoragePortsInfo stroageports;

    string sn;
    string strid;
    string clonefsid;
    string hyper_id;

    list<CMDHOSTINFO_STRU> lsthost;

    list<DAR_HOST_INFO> lstDARHosts;
    map<string, string> accessGroupDict;
};

#endif
