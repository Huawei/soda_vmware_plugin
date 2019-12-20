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

#ifndef SYNC_ONCE
#define SYNC_ONCE

#include "sra_basic.h"
#include "../common/xml_node/consistency_groups.h"

#define SYNC_PROCESSING 2

#define ERROR_CODE_MIRROR_PROCESSING 50338330
#define ERROR_CODE_CONSISTGR_PROCESSING 50338331

#define SYNC_PROCESSING_STATUS         "inProgress"
#define SYNC_PROCESSING_RATE         "0"
#define SYNC_COMPLETE_RATE            "100"
#define SYNC_REMAIN_TIME             "60"
#define COMPLETE_STATUS                "complete"

int sync_once(XmlReader &reader);

class SyncOnce : public SraBasic
{
public:
    SyncOnce();
    virtual ~SyncOnce();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    int _sync_mirror_out(CCmdOperate& cmdOperate);
    int _sync_hypermetro_out(CCmdOperate& cmdOperate);
    int _sync_consistgroup_out(CCmdOperate& cmdOperate);
    int _sync_consistgrouphm_out(CCmdOperate& cmdOperate);

    void _write_consistgroup_result(XmlWriter &writer);
    void _write_mirror_result(XmlWriter &writer);

private:
    char peer_array_id[LENGTH_COMMON];
    char input_array_id[LENGTH_COMMON];

    ConGroupsInfo consisgrs_info;
    SourceDevicesInfo devices_info;
};

#endif

