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

#ifndef RESTORE_H
#define RESTORE_H

#include "sra_basic.h"
#include "../common/xml_node/device.h"
#include "../common/xml_node/consistency_groups.h"


int restore(XmlReader &reader);

class RestoreReplication: public SraBasic
{
public:
    RestoreReplication() : SraBasic()
    {
        memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
    }
    virtual ~RestoreReplication() {};

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");

private:
    int _is_slave_consistency_out(CCmdOperate& cmdOperate);
    int _is_slave_consistencyhm_out(CCmdOperate& cmdOperate);
    int _is_slave_device_out(CCmdOperate& cmdOperate);
    int _is_slavehm_device_out(CCmdOperate& cmdOperate);

private:
    string peer_array_id;
    char input_array_id[LENGTH_COMMON];
    ConGroupsInfo group_info;
    DevicesInfo devices_info;
};

#endif

