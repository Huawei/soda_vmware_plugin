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

#ifndef PREPARE_REVERSR_0923_H
#define PREPARE_REVERSR_0923_H

#include "sra_basic.h"
#include "../common/xml_node/device.h"
#include "../common/xml_node/consistency_groups.h"
#include "ThreeDCLun.h"
#include <vector>
int prepare_reverse(XmlReader &reader);



class PrepareReverse : public SraBasic
                     , ThreeDCLunHelper
{
public:
    PrepareReverse();
    virtual ~PrepareReverse();

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual int _outband_process();
    virtual void _write_response(XmlWriter &writer);

private:
    //int find_3DC
    int deal_3dc_lun(CCmdOperate& cmdOperate);

    int do_deal_AB(CCmdOperate& cmdOperate,const ThreeDCLun::ABInfo& ab_info);
    int deal_AB(CCmdOperate& cmdOperate,const ThreeDCLun::ABInfo& ab_info);
    void set_device_result(const string& pair_id,int err);
    void set_consis_result(const string& consis_id,int err);
    void set_all_success();

    void dealHyperMirror(CCmdOperate& cmdOperate);
    void dealConsistHyperMirror(CCmdOperate& cmdOperate);


private:
    string peer_array_id;
    string input_array_id;

    ConGroupsInfo consistency_group_info;
    DevicesInfo devices_info;
};

#endif