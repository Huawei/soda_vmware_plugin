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

#ifndef DISCOVER_DEVICES_H
#define DISCOVER_DEVICES_H

#include "sra_basic.h"
#include "../common/xml_node/replicated_devices.h"

int discover_devices(XmlReader &reader);

class MirrorDetail : public SraBasic
{
public:
    virtual ~MirrorDetail(){};
    MirrorDetail()
    {
        uiResAcess = 0;
        localResType = 0;
    }


public:
    string hymirrorid;
    string cgid;
    string arrayid;
    string peerarrayid;
    string masterlun;
    string masterlunname;
    string masterlumwwn;
    string slavelun;
    string slavelunname;
    string slavelunwwn;
    string pairstatus;
    string relation;
    unsigned int localResType;
    unsigned int uiResAcess;
    SnapShotInfo snapshot_info;
};

class HyperMetroDetail : public SraBasic
{
public:
    virtual ~HyperMetroDetail(){};
    HyperMetroDetail()
    {
        uiResAcess = 0;
        localResType = 0;
    }


public:
    string hmpairid;
    string cgid;
    string hmdomaingid;
    string hmdomainname;
    string arrayid;
    string peerarrayid;
    string masterlun;
    string masterlunname;
    string masterlumwwn;
    string slavelun;
    string slavelunname;
    string slavelunwwn;
    string pairstatus;
    string relation;
    string strstretched;
    string strCptype;
    string strIsinCg;
    unsigned int localResType;
    unsigned int uiResAcess;
    SnapShotInfo snapshot_info;
};

class CgDetail : public SraBasic
{
public:
    virtual ~CgDetail(){};
    CgDetail()
    {
        uiResAcess = 0;
    }

public:
    string minihymirrorid;
    string cgid;
    string cgname;
    string arrayid;
    string peerarrayid;
    string masterlun;
    string slavelun;
    string pairstatus;
    string role;
    unsigned int uiResAcess;
    list<MirrorDetail> lst_mirror_info;
    CgSnapshot snapshot_info;
};


class CgHMDetail : public SraBasic
{
public:
    virtual ~CgHMDetail(){};
    CgHMDetail()
    {
        uiResAcess = 0;
    }

public:
    string hypermetroid;
    string cgid;
    string cgname;
    string hmdomaingid;
    string hmdomainname;
    string arrayid;
    string peerarrayid;
    string masterlun;
    string slavelun;
    string pairstatus;
    string role;
    string strstretched;
    string strCptype;
    unsigned int uiResAcess;
    list<HyperMetroDetail> lst_hypermetro_info;
    CgSnapshot snapshot_info;
};

class DiscoverDevices : public SraBasic
{
public:
    DiscoverDevices() : SraBasic()
    {
        memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
        memset_s(peer_array_id, sizeof(peer_array_id), 0, sizeof(peer_array_id));
        memset_s(input_array_id, sizeof(input_array_id), 0, sizeof(input_array_id));
    };
    virtual ~DiscoverDevices() {};

protected:
    virtual int _read_command_para(XmlReader &reader);
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();
    virtual int check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id = "");
    

private:
    void _write_mirror_response(list<MirrorDetail> &mirrorlist, SourceDevicesInfo &source_devs,TargetDevicesInfo &target_devs);
    void _write_hypermetro_response(list<HyperMetroDetail> &mirrorlist, SourceDevicesInfo &source_devs,TargetDevicesInfo &target_devs);
    void _get_tg_snapshot_info(CgDetail &it_cg);  //not use
    void _get_tghm_snapshot_info(CgHMDetail &it_cghm);
    void _write_detail(const MirrorDetail &it_mirror, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs);
    void _write_detailhm(const HyperMetroDetail &it_hm, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs);
    void _write_consist(CgDetail &it_cg);
    void _write_consisthm(CgHMDetail &ig_cghm);

    int _get_mirror_info(CCmdOperate& cmdOperate, string& hymirror_id, MirrorDetail& mirrorDetail);
    int _get_hypermetro_info(CCmdOperate& cmdOperate, string& hypermetro_id, HyperMetroDetail &hypermetrodetail);
    int _check_cgid_exist(CCmdOperate& cmdOperate,const string &cg_id, string &hymirror_id, const MirrorDetail &mirrorinfo);
    int _check_cghmid_exist(CCmdOperate& cmdOperate, const string &cghm_id,string &hypermetro_id,const HyperMetroDetail &hypermetroinfo);
    int _get_cgdetail_info(CCmdOperate& cmdOperate, const string& cg_id, const string& hymirror_id, CgDetail& cg_info);

    int _get_vstorepair_detail_info(CCmdOperate& cmdOperate, const string& cg_id, const string& hymirror_id, CgDetail& cg_info);
    int _get_cghmdetail_info(CCmdOperate& cmdOperate, const string& cg_id, const string& hypermetroid, CgHMDetail& cghm_info);
    int _get_mirror_detail_info(CCmdOperate& cmdOperate, list<HYMIRROR_LF_INFO_STRU>& lstMirrorLUNInfo, MirrorDetail& mirrorDetail);
    int _get_hm_detail_info(CCmdOperate& cmdOperate, list<HYPERMETRO_LF_INFO_STRU>& lstHMLUNInfo, HyperMetroDetail& hypermetroDetail);
    int _get_hm_detail_info(CCmdOperate& cmdOperate, list<HYMIRROR_LF_INFO_STRU>& lstMirrorLUNInfo, MirrorDetail& mirrorDetail);
    int _get_snapshot_info(CCmdOperate& cmdOperate, string& lun_id, SnapShotInfo& snapShotInfo);
    int _get_active_time(CCmdOperate& cmdOperate, const string& snapShot_id, const string& snapName,string& time_stamp);

private:
    char peer_array_id[LENGTH_COMMON];
    string peer_array_wwn;
    char input_array_id[LENGTH_COMMON];

    SourceDevicesInfo source_devices_info;
    TargetDevicesInfo target_devices_info;
    StoragePortsInfo  storageports_info;
    ReplicatedDevices rep_devices;

    list<MirrorDetail> lst_mirror_info;
    list<HyperMetroDetail> lst_hypermetro_info;
    list<CgDetail> lst_cg_info; 
    list<CgHMDetail> lst_hmcg_info;

};


#endif