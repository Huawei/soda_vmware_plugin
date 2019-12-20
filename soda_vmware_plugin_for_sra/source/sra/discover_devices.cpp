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

#include "discover_devices.h"
#include <ostream>

std::ostream& operator<<(std::ostream& out,const MirrorDetail& item)
{
    out << "hymirrorid: " << item.hymirrorid << endl;
    out << "cgid: " << item.cgid << endl;
    out << "arrayid: " << item.arrayid << endl;
    out << "peerarrayid: " << item.peerarrayid << endl;
    out << "masterlun: " << item.masterlun << endl;
    out << "masterlunname: " << item.masterlunname << endl;
    out << "masterlumwwn: " << item.masterlumwwn << endl;
    out << "slavelun: " << item.slavelun << endl;
    out << "slavelunname: " << item.slavelunname << endl;
    out << "slavelunwwn: " << item.slavelunwwn << endl;
    out << "pairstatus: " << item.pairstatus << endl;
    out << "relation: " << item.relation << endl;
    out << "uiResAcess: " << item.uiResAcess << endl;
    out << "localResType: " << item.localResType << endl;
    return out;
}

int discover_devices(XmlReader &reader)
{
    DiscoverDevices discover_dev;

    COMMLOG(OS_LOG_INFO, "%s", "DiscoverDevices begin.");
    print("%s", "DiscoverDevices begin.");
    if (RETURN_OK != discover_dev.entry(reader)){
        COMMLOG(OS_LOG_ERROR, "%s", "discover array failed.");

        return RETURN_ERR;
    }

    COMMLOG(OS_LOG_INFO, "%s", "DiscoverDevices end.");
    print("%s", "DiscoverDevices end.");
    return RETURN_OK;
}


int DiscoverDevices::_read_command_para(XmlReader &reader)
{
    (void)reader.get_string("/Command/DiscoverDevicesParameters/ArrayId", input_array_id, 0);
    (void)reader.get_string("/Command/DiscoverDevicesParameters/PeerArrayId", peer_array_id, 0);

    return RETURN_OK;
}


int DiscoverDevices::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_Id, const string& peer_array_Id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    if (array_Id != input_array_id){
        COMMLOG(OS_LOG_ERROR, "DiscoverDevices::check_array_id_validate_out input_array_id is %s, peer_array_id is %s. array_id is %s.", input_array_id, peer_array_Id.c_str(), array_Id.c_str());
        return ERROR_INVALIDATE_LOCAL_ARRAY_SN;
    }

    if (peer_array_Id.empty()){
        return RETURN_OK;
    }

    ret = cmdOperate.CMD_showarrayinfo(lstRemoteArrayInfo);
    CHECK_UNEQ(RETURN_OK, ret);
    COMMLOG(OS_LOG_INFO, "peer_array_Id:%s", peer_array_Id.c_str());
    bool findPeerArrayId = false;
    for (itRemoteArrayInfo = lstRemoteArrayInfo.begin(); 
        itRemoteArrayInfo != lstRemoteArrayInfo.end(); itRemoteArrayInfo++){
        COMMLOG(OS_LOG_INFO, "itRemoteArrayInfo:%s", itRemoteArrayInfo->strSN.c_str());
        if (peer_array_Id == itRemoteArrayInfo->strSN){
            findPeerArrayId = true;
            peer_array_wwn = itRemoteArrayInfo->strWWN;
            break;
        }
    }

    if (!findPeerArrayId){
        return ERROR_INVALIDATE_PEER_ARRAY_SN;
    }

    return RETURN_OK;
}

int DiscoverDevices::_outband_process()
{
    int iRet = RETURN_ERR;

    HYPER_STORAGE_STRU stStorageInfo;
    list<HYMIRROR_INFO_STRU> lstMirrorInfo;
    list<HYMIRROR_INFO_STRU>::iterator itMirrorInfo;
    list<HYPERMETROPAIR_INFO_STRU> lstHyperMetroPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHyperMetroPairInfo;
    list<LIF_INFO_STRU> lstLifInfo;
    list<LIF_INFO_STRU>::iterator itLifInfo;
    CCmdOperate cmdOperate;
    int countDevice = 0;
    string lunID;

    (void)setStorageInfo(stStorageInfo);

    iRet = cmdOperate.SetStorageInfo(stStorageInfo);
    CHECK_UNEQ(RETURN_OK, iRet);

    iRet = _get_array_info(cmdOperate);
    CHECK_UNEQ(RETURN_OK, iRet);

    string array_id_temp(array_id);
    iRet = check_array_id_validate_out(cmdOperate, array_id_temp, peer_array_id);
    CHECK_UNEQ(RETURN_OK, iRet);

    COMMLOG(OS_LOG_INFO, "Commence: discover the device on array [%s]", string(array_id).c_str());
    print("Commence: discover the device on array [%s]", string(array_id).c_str());

    COMMLOG(OS_LOG_INFO, "Commence: Check whether nfs is supported on the array [%s]", string(array_id).c_str());
    print("Commence: Check whether nfs is supported on the array [%s]", string(array_id).c_str());
    iRet = cmdOperate.CMD_showarrayclonefs();
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "execute CMD_showarrayclonefs failed, ret = %d", iRet);
    }
     

     
    iRet = cmdOperate.CMD_showhymirrorinfo_all(lstMirrorInfo);
    CHECK_UNEQ(RETURN_OK, iRet);
    COMMLOG(OS_LOG_DEBUG, "Number of availabel remote replication is %d", lstMirrorInfo.size());

    for (itMirrorInfo = lstMirrorInfo.begin(); itMirrorInfo != lstMirrorInfo.end(); ++itMirrorInfo){
         
        if (OBJ_FILESYSTEM == itMirrorInfo->uilocalResType){
            isNFS = true;
        }
        else{
            isNFS = false;
        }
        MirrorDetail mirrorinfo;
        iRet = _get_mirror_info(cmdOperate, itMirrorInfo->strID, mirrorinfo);
         

        if(RETURN_OK != iRet){
             
            COMMLOG(OS_LOG_ERROR, "exclude the replicated device [%s] because found error [%d]", string(itMirrorInfo->strID).c_str(), iRet);
            continue;
        }
         
        if(!mirrorinfo.masterlun.empty()){
            lunID = mirrorinfo.masterlun;
        }
        else{
            lunID = mirrorinfo.slavelun;
        }
         
         
        if (!itMirrorInfo->bIsBelongGroup){
             
            lst_mirror_info.push_back(mirrorinfo);
             
            COMMLOG(OS_LOG_INFO, "discover the the remote replicated device [%s]",lunID.c_str());
            print("discover the the remote replicated device [%s]",lunID.c_str());
        }
        else{
            if (!g_bFusionStorage){
                 
                iRet = _check_cgid_exist(cmdOperate, itMirrorInfo->strGroupID, itMirrorInfo->strID, mirrorinfo);
                CHECK_UNEQ(RETURN_OK, iRet);
                 
                COMMLOG(OS_LOG_INFO, "discover the consistency group  device [%s] ", lunID.c_str());
                print("discover the consistency group  device [%s] ", lunID.c_str());
            }
        }
        countDevice++;
    }

    if (g_bFusionStorage && !g_testFusionStorageStretch){
        return RETURN_OK;
    }

    list<REMOTE_ARRAY_STRU> lstRemoteStretchedInfo;

    iRet = cmdOperate.CMD_showarraystretched(lstRemoteStretchedInfo);
    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "execute CMD_showarraystretched failed, ret = %d", iRet);
    }

    if (g_bstretch){
         
        iRet = cmdOperate.CMD_showHyperMetroPair_all(lstHyperMetroPairInfo);
        CHECK_UNEQ(RETURN_OK, iRet);
        for (itHyperMetroPairInfo = lstHyperMetroPairInfo.begin(); 
            itHyperMetroPairInfo != lstHyperMetroPairInfo.end(); ++itHyperMetroPairInfo){
            HyperMetroDetail hypermetroinfo;
            iRet = _get_hypermetro_info(cmdOperate, itHyperMetroPairInfo->strID, hypermetroinfo);

            if(RETURN_OK != iRet){
                 
                COMMLOG(OS_LOG_ERROR, "exclude the hypermetro device [%s] because found error [%d]", 
                    string(itHyperMetroPairInfo->strID).c_str(), iRet);
                continue;
            }
             
            if(!hypermetroinfo.masterlun.empty()){
                lunID = hypermetroinfo.masterlun;
            }
            else{
                lunID = hypermetroinfo.slavelun;
            }
             
            if (!itHyperMetroPairInfo->bIsBelongGroup){
                 
                lst_hypermetro_info.push_back(hypermetroinfo);
                 
                COMMLOG(OS_LOG_INFO, "discover the the remote hypermetro device [%s]",lunID.c_str());
                print("discover the the remote hypermetro device [%s]",lunID.c_str());
            }
            else{           
                if (!g_bFusionStorage){
                     
                    iRet = _check_cghmid_exist(cmdOperate, itHyperMetroPairInfo->strGroupID, 
                        itHyperMetroPairInfo->strID, hypermetroinfo);
                    CHECK_UNEQ(RETURN_OK, iRet);
                     
                    COMMLOG(OS_LOG_INFO, "discover the consistency group  device [%s] ", lunID.c_str());
                    print("discover the consistency group  device [%s] ", lunID.c_str());
                }
            }
            countDevice++;
        }
    }


    if (g_bFusionStorage){
        return RETURN_OK;
    }

     
    if(g_bnfs == true){
        cmdOperate.CMD_showLIF(lstLifInfo);
        for (itLifInfo = lstLifInfo.begin(); itLifInfo != lstLifInfo.end(); ++itLifInfo){                                         
             
            if(itLifInfo->role == SERVICE_ROLE || itLifInfo->role == MANAGEMENT_AND_SERVICE_ROLE){
                StoragePortsIPS storageportsips;
                storageportsips.source_IP = itLifInfo->strLIFIP;
                storageports_info.lst_storageportsips.push_back(storageportsips);
            }
            else{
                COMMLOG(OS_LOG_INFO, "Filtered logical port [%s], because it cannot be a service port", itLifInfo->strLIFIP.c_str());
            }
        }    
    }
     
    COMMLOG(OS_LOG_INFO, "Complete: discover [%d] devices on array [%s]", countDevice ,string(array_id).c_str());
    print("Complete: discover [%d] devices on array [%s]", countDevice ,string(array_id).c_str());
    return RETURN_OK;
}

int DiscoverDevices::_get_mirror_info(CCmdOperate& cmdOperate, string& hymirror_id, MirrorDetail& mirrorDetail)
{
    int iRet = RETURN_ERR;
    list<string> lst_local_ids;
    list<HYMIRROR_LF_INFO_STRU> lstMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itMirrorLUNInfo;

     
    if (isNFS){
        iRet = cmdOperate.CMD_showhymirrorfs_all(hymirror_id, lstMirrorLUNInfo);
    } 
    else{
        iRet = cmdOperate.CMD_showhymirrorlun_all(hymirror_id, lstMirrorLUNInfo);
    }
    CHECK_UNEQ(RETURN_OK,iRet);

     
    iRet = _get_mirror_detail_info(cmdOperate, lstMirrorLUNInfo, mirrorDetail);
    CHECK_UNEQ(RETURN_OK,iRet);

     
    if((peer_array_id != mirrorDetail.peerarrayid) && (peer_array_id != mirrorDetail.arrayid)){
        return RETURN_ERR;
    }

    mirrorDetail.hymirrorid = hymirror_id;

     
    lst_local_ids.clear();
    for (itMirrorLUNInfo = lstMirrorLUNInfo.begin(); itMirrorLUNInfo != lstMirrorLUNInfo.end(); itMirrorLUNInfo++){
        if ( string(array_id) == itMirrorLUNInfo->strArrayWWN){
            lst_local_ids.push_back(itMirrorLUNInfo->strID);
        }

        mirrorDetail.uiResAcess = itMirrorLUNInfo->uiSecResAccess;
    }
     
    if ( (MIRROR_RELATION_SLAVE == mirrorDetail.relation) && !lst_local_ids.empty()){
        if (RETURN_OK != _get_snapshot_info(cmdOperate, lst_local_ids.front(), mirrorDetail.snapshot_info)){
            mirrorDetail.snapshot_info.snap_id = "";
        }
    }

    return RETURN_OK; 
}

int DiscoverDevices::_get_hypermetro_info(CCmdOperate& cmdOperate, string& hypermetro_id, HyperMetroDetail &hypermetrodetail)
{
    int iRet = RETURN_ERR;
    list<string> lst_local_ids;
    list<HYPERMETRO_LF_INFO_STRU> lstHMLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMLUNInfo;
    
     
    iRet = cmdOperate.CMD_showhypermetrolun_all(hypermetro_id, lstHMLUNInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    iRet = _get_hm_detail_info(cmdOperate, lstHMLUNInfo, hypermetrodetail);
    CHECK_UNEQ(RETURN_OK,iRet);

    if((peer_array_id != hypermetrodetail.peerarrayid) && (peer_array_id != hypermetrodetail.arrayid)){
        COMMLOG(OS_LOG_ERROR, "peer_array_id [%s] hypermetrodetail.peerarrayid [%s],hypermetrodetail.arrayid",
            peer_array_id, hypermetrodetail.peerarrayid.c_str(), hypermetrodetail.arrayid.c_str());
        return RETURN_ERR;
    }

    hypermetrodetail.hmpairid = hypermetro_id;

     
    lst_local_ids.clear();
    for (itHMLUNInfo = lstHMLUNInfo.begin(); itHMLUNInfo != lstHMLUNInfo.end(); itHMLUNInfo++){
        if ( string(array_id) == itHMLUNInfo->strArrayWWN){
            lst_local_ids.push_back(itHMLUNInfo->strID);
        }
    }
     
    if ( (HM_RELATION_SLAVE == hypermetrodetail.relation) && !lst_local_ids.empty()){
        if (RETURN_OK != _get_snapshot_info(cmdOperate, lst_local_ids.front(), hypermetrodetail.snapshot_info)){
            hypermetrodetail.snapshot_info.snap_id = "";
        }
    }

    return RETURN_OK; 
}

int DiscoverDevices::_check_cgid_exist(CCmdOperate& cmdOperate, const string &cg_id,
                                       string &hymirror_id,const MirrorDetail &mirrorinfo)
{
    list<CgDetail>::iterator itcgdetail;
    int flag = 0;
    int iRet = RETURN_ERR;
    CgDetail cg_info;
    int mirrorslavelunid = 0;
    int cgslavelunid = 0;
    int mirrormasterlunid = 0;
    int cgmasterlunid = 0;

     
    for (itcgdetail = lst_cg_info.begin(); itcgdetail != lst_cg_info.end(); ++itcgdetail){
        if (itcgdetail->cgid != cg_id){   
            continue;
        }

        mirrorslavelunid = atoi(mirrorinfo.slavelun.c_str());
        cgslavelunid = atoi(itcgdetail->slavelun.c_str());
        mirrormasterlunid = atoi(mirrorinfo.masterlun.c_str());
        cgmasterlunid = atoi(itcgdetail->masterlun.c_str());

        if (mirrorslavelunid < cgslavelunid){
            itcgdetail->slavelun = mirrorinfo.slavelun;
        }

        if (mirrormasterlunid < cgmasterlunid){
            itcgdetail->masterlun = mirrorinfo.masterlun;
        }

        itcgdetail->lst_mirror_info.push_back(mirrorinfo);
        flag = 1;
        break;
    }

     
    if (0 == flag){
        if (isNFS){
             
            iRet = _get_vstorepair_detail_info(cmdOperate, cg_id, hymirror_id, cg_info);
        }
        else{
            iRet = _get_cgdetail_info(cmdOperate, cg_id, hymirror_id, cg_info);
        }
        CHECK_UNEQ(RETURN_OK,iRet);

        cg_info.masterlun = mirrorinfo.masterlun;
        cg_info.slavelun = mirrorinfo.slavelun;
        cg_info.lst_mirror_info.push_back(mirrorinfo);

        lst_cg_info.push_back(cg_info);
    }

    return RETURN_OK;
}

int DiscoverDevices::_check_cghmid_exist(CCmdOperate& cmdOperate, const string &cghm_id,
    string &hypermetroid,const HyperMetroDetail &hypermetroinfo)
{
    list<CgHMDetail>::iterator itcghmdetail;
    int flag = 0;
    int iRet = RETURN_ERR;
    CgHMDetail cghm_info;
    int hmslavelunid = 0;
    int cghmslavelunid = 0;
    int hmmasterlunid = 0;
    int cghmmasterlunid = 0;

     
    for (itcghmdetail = lst_hmcg_info.begin(); itcghmdetail != lst_hmcg_info.end(); ++itcghmdetail){
        if (itcghmdetail->cgid != cghm_id){   
            continue;
        }

        hmslavelunid = atoi(hypermetroinfo.slavelun.c_str());
        cghmslavelunid = atoi(itcghmdetail->slavelun.c_str());
        hmmasterlunid = atoi(hypermetroinfo.masterlun.c_str());
        cghmmasterlunid = atoi(itcghmdetail->masterlun.c_str());

        if (hmslavelunid < cghmslavelunid){
            itcghmdetail->slavelun = hypermetroinfo.slavelun;
        }

        if (hmmasterlunid < cghmmasterlunid){
            itcghmdetail->masterlun = hypermetroinfo.masterlun;
        }

        itcghmdetail->lst_hypermetro_info.push_back(hypermetroinfo);
        flag = 1;
        break;
    }

     
    if (0 == flag){
        iRet = _get_cghmdetail_info(cmdOperate, cghm_id, hypermetroid, cghm_info);
        CHECK_UNEQ(RETURN_OK,iRet);

        cghm_info.strstretched = hypermetroinfo.strstretched;
        cghm_info.strCptype = hypermetroinfo.strCptype;
        cghm_info.hmdomaingid = hypermetroinfo.hmdomaingid;
        cghm_info.hmdomainname = hypermetroinfo.hmdomainname;
        cghm_info.masterlun = hypermetroinfo.masterlun;
        cghm_info.slavelun = hypermetroinfo.slavelun;
        cghm_info.lst_hypermetro_info.push_back(hypermetroinfo);

        lst_hmcg_info.push_back(cghm_info);
    }

    return RETURN_OK;
}

int DiscoverDevices::_get_cgdetail_info(CCmdOperate& cmdOperate,
                                       const string& cg_id, const string& hymirror_id, CgDetail& cg_info)
{
    int iRet = RETURN_ERR;
    
    GROUP_INFO_STRU stGroupInfo;
    list<HYMIRROR_INFO_STRU> lstCgHyMirrorInfo;
    bool bAllRW(true);

    string cgname;
    string pairstatus; 
    string role;

    stGroupInfo.strID = cg_id;
    iRet = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    cgname = stGroupInfo.strName;
    _convert_pair_status(stGroupInfo.uiState, pairstatus);
    _convert_relation_type(stGroupInfo.uiIsPrimary, role);

    if (MIRROR_RELATION_MASTER == role){
        cg_info.arrayid = array_id;
        cg_info.peerarrayid = peer_array_id;
    }
    else{
        cg_info.arrayid = peer_array_id;
        cg_info.peerarrayid = array_id;
    }

    cg_info.cgid = cg_id;
    cg_info.cgname = cgname;
    cg_info.minihymirrorid = hymirror_id;
    cg_info.pairstatus = pairstatus;
    cg_info.role = role;

    
     
    iRet = cmdOperate.CMD_showconsistgrmember(cg_id, lstCgHyMirrorInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

     
    for (list<HYMIRROR_INFO_STRU>::const_iterator constIt = lstCgHyMirrorInfo.begin();
        constIt != lstCgHyMirrorInfo.end(); ++constIt){
         
        bAllRW = bAllRW && (RESOURCE_ACCESS_READ_WRITE == constIt->uiSecResAccess);
         
    }

    if (bAllRW){
        cg_info.uiResAcess = RESOURCE_ACCESS_READ_WRITE;
    }
    else{
        cg_info.uiResAcess = RESOURCE_ACCESS_READ_ONLY;
    }

    return iRet;
}

int DiscoverDevices::_get_vstorepair_detail_info(CCmdOperate& cmdOperate,
                                       const string& cg_id, const string& hymirror_id, CgDetail& cg_info)
{
    int iRet = RETURN_ERR;
    
    GROUP_INFO_STRU stGroupInfo;
    VSTORE_PAIR_INFO_STRU stVstorePairInfo;
    list<HYMIRROR_INFO_STRU> lstCgHyMirrorInfo;
    bool bAllRW(true);

    string cgname;
    string pairstatus; 
    string role;

    iRet = cmdOperate.CMD_showvstorepair_info(cg_id, stVstorePairInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    cg_info.cgid = cg_id;
    cgname = stVstorePairInfo.strID;
    _convert_pair_status(stVstorePairInfo.uiState, pairstatus);

    role = stVstorePairInfo.isPrimary ? MIRROR_RELATION_MASTER : MIRROR_RELATION_SLAVE;

    if (MIRROR_RELATION_MASTER == role){
        cg_info.arrayid = array_id;
        cg_info.peerarrayid = peer_array_id;
    }
    else{
        cg_info.arrayid = peer_array_id;
        cg_info.peerarrayid = array_id;
    }

    cg_info.cgid = cg_id;
    cg_info.cgname = cgname;
    cg_info.minihymirrorid = hymirror_id;
    cg_info.pairstatus = pairstatus;
    cg_info.role = role;

    
     
     
    iRet = cmdOperate.CMD_showvstorepairmember(cg_id, lstCgHyMirrorInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

     
    for (list<HYMIRROR_INFO_STRU>::const_iterator constIt = lstCgHyMirrorInfo.begin();
        constIt != lstCgHyMirrorInfo.end(); ++constIt){
         
        bAllRW == bAllRW && (RESOURCE_ACCESS_READ_WRITE == constIt->uiSecResAccess);
         
    }

    if (bAllRW){
        cg_info.uiResAcess = RESOURCE_ACCESS_READ_WRITE;
    }
    else{
        cg_info.uiResAcess = RESOURCE_ACCESS_READ_ONLY;
    }

    return iRet;
}

int DiscoverDevices::_get_cghmdetail_info(CCmdOperate& cmdOperate,
                                       const string& cg_id, const string& hypermetroid, CgHMDetail& cghm_info)
{
    int iRet = RETURN_ERR;
    list<HYPERMETROPAIR_INFO_STRU> lstHyperMetroPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHyperMetroPairInfo;
    GROUP_INFO_STRU stGroupInfo;

    string cgname;
    string pairstatus; 
    string role;

    stGroupInfo.strID = cg_id;
    iRet = cmdOperate.CMD_showconsistgrhminfo(stGroupInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    cgname = stGroupInfo.strName;
    _convert_hmpair_status(stGroupInfo.uiState, pairstatus);
    _convert_relation_type(stGroupInfo.uiIsPrimary, role);

    if (MIRROR_RELATION_MASTER == role){
        cghm_info.arrayid = array_id;
        cghm_info.peerarrayid = peer_array_id;
    }
    else{
        cghm_info.arrayid = peer_array_id;
        cghm_info.peerarrayid = array_id;
    }

    cghm_info.cgid = cg_id;
    cghm_info.cgname = cgname;
    cghm_info.hmdomaingid = stGroupInfo.hmdomain;
    cghm_info.hypermetroid = hypermetroid;
    cghm_info.pairstatus = pairstatus;
    cghm_info.role = role;
    cghm_info.uiResAcess = stGroupInfo.uiSecResAccess;

    return iRet;
}

int DiscoverDevices::_get_mirror_detail_info(CCmdOperate& cmdOperate,
                                             list<HYMIRROR_LF_INFO_STRU>& lstMirrorLUNInfo, MirrorDetail& mirrorDetail)
{
    int iRet = RETURN_ERR;
    
    LUN_INFO_STRU stLUNInfo;
    FS_INFO_STRU  stFSInfo;
    
    list<HYMIRROR_LF_INFO_STRU>::iterator itMirrorLUNInfo;

    for (itMirrorLUNInfo = lstMirrorLUNInfo.begin(); itMirrorLUNInfo != lstMirrorLUNInfo.end(); itMirrorLUNInfo++){
         
        if ( LUN_RELATION_MASTER == itMirrorLUNInfo->uiRelationType){
            mirrorDetail.arrayid = itMirrorLUNInfo->strArrayWWN;
            mirrorDetail.masterlun = itMirrorLUNInfo->strID;
            mirrorDetail.masterlunname = itMirrorLUNInfo->strName;
            mirrorDetail.localResType = itMirrorLUNInfo->uilocalResType;
        }
        else if ( LUN_RELATION_SLAVE == itMirrorLUNInfo->uiRelationType){
            mirrorDetail.peerarrayid = itMirrorLUNInfo->strArrayWWN;
            mirrorDetail.slavelun = itMirrorLUNInfo->strID;
            mirrorDetail.slavelunname = itMirrorLUNInfo->strName;
            _convert_pair_status(itMirrorLUNInfo->uiPairStatus, mirrorDetail.pairstatus);
        }

         
        if (string(array_id) == itMirrorLUNInfo->strArrayWWN){
            _convert_relation_type(itMirrorLUNInfo->uiRelationType, mirrorDetail.relation);
        }
    }
    
    if (string(array_id) == mirrorDetail.arrayid){
         
        if (isNFS){
            stFSInfo.strID = mirrorDetail.masterlun;
            iRet = cmdOperate.CMD_showfs(stFSInfo);
            CHECK_UNEQ(RETURN_OK, iRet);
            mirrorDetail.masterlumwwn = stFSInfo.strPath;
        } 
        else{
            stLUNInfo.strID = mirrorDetail.masterlun;
            stLUNInfo.strName = mirrorDetail.masterlunname;
            iRet = cmdOperate.CMD_showlun(stLUNInfo);
            CHECK_UNEQ(RETURN_OK, iRet);
            mirrorDetail.masterlumwwn = stLUNInfo.strWWN;
        }
    }
    else{
         
        if (isNFS){
            stFSInfo.strID = mirrorDetail.slavelun;
            iRet = cmdOperate.CMD_showfs(stFSInfo);
            CHECK_UNEQ(RETURN_OK, iRet);
            mirrorDetail.slavelunwwn = stFSInfo.strPath;
        } 
        else{
            stLUNInfo.strID = mirrorDetail.slavelun;
            iRet = cmdOperate.CMD_showlun(stLUNInfo);
            CHECK_UNEQ(RETURN_OK, iRet);
            mirrorDetail.slavelunwwn = stLUNInfo.strWWN;
        }
    }

    return RETURN_OK;
}

int DiscoverDevices::_get_hm_detail_info(CCmdOperate& cmdOperate,
                                list<HYPERMETRO_LF_INFO_STRU>& lstHMLUNInfo, HyperMetroDetail& hypermetroDetail)
{
    int iRet = RETURN_ERR;
    LUN_INFO_STRU stLUNInfo;
    FS_INFO_STRU  stFSInfo;
    
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMLUNInfo;

    for (itHMLUNInfo = lstHMLUNInfo.begin(); itHMLUNInfo != lstHMLUNInfo.end(); itHMLUNInfo++){
         
        if ( LUN_RELATION_MASTER == itHMLUNInfo->uiRelationType){
            hypermetroDetail.arrayid = itHMLUNInfo->strArrayWWN;
            hypermetroDetail.masterlun = itHMLUNInfo->strID;
            hypermetroDetail.masterlunname = itHMLUNInfo->strName;
            hypermetroDetail.strstretched = itHMLUNInfo->strstretched;
            hypermetroDetail.strCptype = itHMLUNInfo->strCptype;
            hypermetroDetail.hmdomaingid = itHMLUNInfo->strDomainID;
            hypermetroDetail.hmdomainname = itHMLUNInfo->strDomainName;
            hypermetroDetail.uiResAcess = itHMLUNInfo->uiSecResAccess;
            hypermetroDetail.strIsinCg = itHMLUNInfo->strIsinCg;
        }
        else if ( LUN_RELATION_SLAVE == itHMLUNInfo->uiRelationType){
            hypermetroDetail.peerarrayid = itHMLUNInfo->strArrayWWN;
            hypermetroDetail.slavelun = itHMLUNInfo->strID;
            hypermetroDetail.slavelunname = itHMLUNInfo->strName;
            hypermetroDetail.strstretched = itHMLUNInfo->strstretched;
            hypermetroDetail.strCptype = itHMLUNInfo->strCptype;
            hypermetroDetail.hmdomaingid = itHMLUNInfo->strDomainID;
            hypermetroDetail.hmdomainname = itHMLUNInfo->strDomainName;
            hypermetroDetail.uiResAcess = itHMLUNInfo->uiSecResAccess;
            hypermetroDetail.strIsinCg = itHMLUNInfo->strIsinCg;
            _convert_hmpair_status(itHMLUNInfo->uiPairStatus, hypermetroDetail.pairstatus);
        }

         
        if (string(array_id) == itHMLUNInfo->strArrayWWN){
            _convert_hmrelation_type(itHMLUNInfo->uiRelationType, hypermetroDetail.relation);
        }
    }
    
    if (string(array_id) == hypermetroDetail.arrayid){
         
        stLUNInfo.strID = hypermetroDetail.masterlun;
        iRet = cmdOperate.CMD_showlun(stLUNInfo);
        CHECK_UNEQ(RETURN_OK, iRet);
        hypermetroDetail.masterlumwwn = stLUNInfo.strWWN;
    }
    else{
         
        stLUNInfo.strID = hypermetroDetail.slavelun;
        iRet = cmdOperate.CMD_showlun(stLUNInfo);
        CHECK_UNEQ(RETURN_OK, iRet);
        hypermetroDetail.slavelunwwn = stLUNInfo.strWWN;
    }

    return RETURN_OK;
}

int DiscoverDevices::_get_snapshot_info(CCmdOperate& cmdOperate, string& strID, SnapShotInfo& snapShotInfo)
{
    int iRet = RETURN_ERR;
    string wwn;
    string sharpath;
    string snapshot_name = VSSRA_SNAPSHOT_PREFIX + strID;
    list<string> lst_snapshot_ids;
    list<HYIMAGE_INFO_STRU> lstHyperImgInfo;
    list<HYIMAGE_INFO_STRU>::iterator itHyperImgInfo;
    list<MAP_INFO_STRU> lstMapInfo;
    list<MAP_INFO_STRU>::iterator itMapInfo;
    list<NFS_INFO_STRU> lstNfsInfo;
    list<NFS_INFO_STRU>::iterator itNfsInfo;
    string vstore_id = STR_NOT_EXIST;
     
    if (isNFS){
        iRet = cmdOperate.CMD_showhyimgoffs(strID, lstHyperImgInfo);
    } 
    else{
        iRet = cmdOperate.CMD_showhyimgoflun(strID, lstHyperImgInfo);
    }
    
    CHECK_UNEQ(RETURN_OK,iRet);

     
    lst_snapshot_ids.clear();
    for (itHyperImgInfo = lstHyperImgInfo.begin(); itHyperImgInfo != lstHyperImgInfo.end(); itHyperImgInfo++){
        if (snapshot_name == itHyperImgInfo->strName){
            lst_snapshot_ids.push_back(itHyperImgInfo->strID);
            vstore_id = itHyperImgInfo->vstoreID;
            break;
        }
    }

    if (lst_snapshot_ids.empty()){
         
        return RETURN_OK;
    }

     
    iRet = _get_active_time(cmdOperate, lst_snapshot_ids.front(), snapshot_name, snapShotInfo.recoverypoint_info.rp_time);
    CHECK_UNEQ(RETURN_OK,iRet);
    if (isNFS){
        iRet = cmdOperate.CMD_shownfs(vstore_id, lstNfsInfo);
        COMMLOG(OS_LOG_INFO, "CMD_shownfs(%d)", iRet);
                CHECK_UNEQ(RETURN_OK,iRet);
        for (itNfsInfo = lstNfsInfo.begin(); itNfsInfo != lstNfsInfo.end(); itNfsInfo++){
            if (itNfsInfo->strFsID == strID){
                sharpath = itNfsInfo->strSharePath;
                COMMLOG(OS_LOG_INFO, "itNfsInfo->strSharePath(%s)", itNfsInfo->strSharePath.c_str());
                break;
            }
        }
    }
    else{
         
        if (g_bFusionStorage){
            iRet = cmdOperate.CMD_showmap(lst_snapshot_ids.front(),lstMapInfo);
            CHECK_UNEQ(RETURN_OK, iRet);
            COMMLOG(OS_LOG_INFO,"the listMapInfo size[%d]", lstMapInfo.size());
            if (!lstMapInfo.empty()){
                wwn = (*lstMapInfo.begin()).strLUNWWN;
            }
        }
        else{
            iRet = cmdOperate.CMD_showmap(lstMapInfo);
            CHECK_UNEQ(RETURN_OK, iRet);

             
            for (itMapInfo = lstMapInfo.begin(); itMapInfo != lstMapInfo.end(); itMapInfo++){
                if (itMapInfo->strDevLUNID == lst_snapshot_ids.front()){
                    wwn = itMapInfo->strLUNWWN;
                    break;
                }
            }
        }
    }
 

     
    if ((wwn.empty() && sharpath.empty()) || (!wwn.empty() && !sharpath.empty())){
        snapShotInfo.snap_id.clear();
    }
    else{
        snapShotInfo.snap_id = lst_snapshot_ids.front();
        if (isNFS){
            snapShotInfo.snap_fspath = sharpath;
        }
        else{
            snapShotInfo.snap_wwn = wwn;
        }
        snapShotInfo.recoverypoint_info.rp_id = "rp_" + lst_snapshot_ids.front();
        snapShotInfo.recoverypoint_info.rp_name = "rp_" + lst_snapshot_ids.front();
    }

    return RETURN_OK;
}

int DiscoverDevices::_get_active_time(CCmdOperate& cmdOperate, const string& snapShot_id, const string& snapName, string& time_stamp)
{
    int iRet = RETURN_ERR;
    
    HYIMAGE_INFO_STRU stHyImgInfo;
    stHyImgInfo.strID = snapShot_id;
    stHyImgInfo.strName = snapName;
    if (isNFS){
        iRet = cmdOperate.CMD_showFShyimginfo(stHyImgInfo);
    } 
    else{
        iRet = cmdOperate.CMD_showhyimginfo(stHyImgInfo);
    }

    CHECK_UNEQ(RETURN_OK,iRet);
        
    time_stamp = stHyImgInfo.strTimeStamp;
    if (time_stamp == "--"){
        COMMLOG(OS_LOG_WARN,"The snapshot(%s) is not activate time", snapShot_id.c_str());

        return ERROR_ACTIVE_SNAPSHOT;
    }

    format_daytime(time_stamp);

    return RETURN_OK;
}

void DiscoverDevices::_write_response(XmlWriter &writer)
{   
    list<CgDetail>::iterator it_cg;
    list<CgHMDetail>::iterator it_cghm;

    _write_mirror_response(lst_mirror_info, source_devices_info, target_devices_info);  

    if (g_bstretch){
        _write_hypermetro_response(lst_hypermetro_info, source_devices_info, target_devices_info);  
    }
    
    rep_devices.sources_info = source_devices_info;
    rep_devices.targets_info = target_devices_info;
    rep_devices.storageports_info = storageports_info;
    for (it_cg =  lst_cg_info.begin(); it_cg != lst_cg_info.end(); it_cg++){
        _write_consist(*it_cg);
    }
    if(g_bstretch){
        for (it_cghm =  lst_hmcg_info.begin(); it_cghm != lst_hmcg_info.end(); it_cghm++){
            _write_consisthm(*it_cghm);
        }
    }

    (void)writer.set_xml("/Response", &rep_devices);  

    return;
}

void DiscoverDevices::_write_mirror_response(list<MirrorDetail> &mirrorlist, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs)
{
    list<MirrorDetail>::iterator it_mirror;

    for (it_mirror = mirrorlist.begin(); it_mirror != mirrorlist.end(); it_mirror++){
        _write_detail(*it_mirror,source_devs,target_devs);
    }

    return;
}

void DiscoverDevices::_write_hypermetro_response(list<HyperMetroDetail> &hmlist, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs)
{
    list<HyperMetroDetail>::iterator it_hm;

    for (it_hm = hmlist.begin(); it_hm != hmlist.end(); it_hm++){
        _write_detailhm(*it_hm,source_devs,target_devs);
    }

    return;
}

void DiscoverDevices:: _get_tg_snapshot_info(CgDetail &it_cg)
{
    list<MirrorDetail>::iterator itmirrordetail;   

    for(itmirrordetail = it_cg.lst_mirror_info.begin();itmirrordetail != it_cg.lst_mirror_info.end(); ++itmirrordetail){
        if (!(itmirrordetail->snapshot_info.snap_id.empty())){
            TargetDeviceInfo tgdev;

            tgdev.target_key = string(array_id) + "_" + itmirrordetail->slavelun;
            tgdev.snapshot_info.snap_id = itmirrordetail->snapshot_info.snap_id;
            tgdev.snapshot_info.snap_wwn = itmirrordetail->snapshot_info.snap_wwn;
             
            min_time(itmirrordetail->snapshot_info.recoverypoint_info.rp_time, it_cg.snapshot_info.recover_info.rp_time);

            it_cg.snapshot_info.tg_devices_info.lst_target_devices.push_back(tgdev);

            itmirrordetail->snapshot_info.snap_id.clear();
            itmirrordetail->snapshot_info.snap_wwn.clear();
        }  
    }

    return;
}

void DiscoverDevices:: _get_tghm_snapshot_info(CgHMDetail &it_cghm)
{
    list<HyperMetroDetail>::iterator ithypermetrodetail;   

    for(ithypermetrodetail = it_cghm.lst_hypermetro_info.begin();
        ithypermetrodetail != it_cghm.lst_hypermetro_info.end(); ++ithypermetrodetail){
        if (!(ithypermetrodetail->snapshot_info.snap_id.empty())){
            TargetDeviceInfo tgdev;

            tgdev.target_key = string(array_id) + "_" + ithypermetrodetail->slavelun;
            tgdev.snapshot_info.snap_id = ithypermetrodetail->snapshot_info.snap_id;
            tgdev.snapshot_info.snap_wwn = ithypermetrodetail->snapshot_info.snap_wwn;
             
            min_time(ithypermetrodetail->snapshot_info.recoverypoint_info.rp_time, it_cghm.snapshot_info.recover_info.rp_time);

            it_cghm.snapshot_info.tg_devices_info.lst_target_devices.push_back(tgdev);

            ithypermetrodetail->snapshot_info.snap_id.clear();
            ithypermetrodetail->snapshot_info.snap_wwn.clear();
        }  
    }

    return;
}

void DiscoverDevices::_write_detail(const MirrorDetail &it_mirror, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs)
{
    string source_dev_id = it_mirror.arrayid + "_" + it_mirror.masterlun + "_" + MIRROR_LUN_TAG + "_" + it_mirror.hymirrorid;
    string target_dev_id = it_mirror.peerarrayid + "_" + it_mirror.slavelun + "_" + MIRROR_LUN_TAG + "_" + it_mirror.hymirrorid;

    if (it_mirror.arrayid == array_id)  {
         
        if (HM_SPLITED == it_mirror.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_mirror.uiResAcess){
            SourceDeviceInfo sd;
            sd.source_id = source_dev_id;
            sd.source_name = it_mirror.masterlunname;
            sd.status = "read-only";

            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            sd.target_info = td;

            source_devs.lst_source_devices.push_back(sd);
        }
        else   {
            SourceDeviceInfo sd;
            sd.source_id = source_dev_id;
            sd.source_name = it_mirror.masterlunname;
            sd.status = "read-write";
            if (it_mirror.localResType == OBJ_FILESYSTEM){
                IdentityInfoFS ideninfofs;
                ideninfofs.source_NfsName = it_mirror.masterlumwwn;
                sd.identity_infofs = ideninfofs;
            } 
            else{
                IdentityInfo ideninfo;
                ideninfo.source_wwn = it_mirror.masterlumwwn;
                sd.identity_info = ideninfo;
            }

            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            sd.target_info = td;

            source_devs.lst_source_devices.push_back(sd);
        }
    }
    else         {
         
        if ((HM_SPLITED == it_mirror.pairstatus || HM_INTERRUPT == it_mirror.pairstatus) && (int)RESOURCE_ACCESS_READ_WRITE == it_mirror.uiResAcess){
            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            td.target_id = target_dev_id;
            td.target_name = it_mirror.slavelunname;
            td.target_state = "read-write";
            if (it_mirror.localResType == OBJ_FILESYSTEM){
                IdentityInfoFS ideninfofs;
                ideninfofs.source_NfsName = it_mirror.slavelunwwn; 
                td.identity_infofs = ideninfofs;
            }
            else{
                IdentityInfo ideninfo;
                ideninfo.source_wwn = it_mirror.slavelunwwn;
                td.identity_info = ideninfo;
            }

            target_devs.lst_target_devices.push_back(td);
        }
        else  {
            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            td.target_name = it_mirror.slavelunname;
            td.target_state = "read-only";

            td.snapshot_info = it_mirror.snapshot_info;

            target_devs.lst_target_devices.push_back(td);
        }
    }
}

void DiscoverDevices::_write_detailhm(const HyperMetroDetail &it_hm, SourceDevicesInfo &source_devs, TargetDevicesInfo &target_devs)
{
    string source_dev_id = it_hm.arrayid + "_" + it_hm.masterlun + "_" + HYPERMETRO_LUN_TAG + "_" + it_hm.hmpairid;
    string target_dev_id = it_hm.peerarrayid + "_" + it_hm.slavelun + "_"+HYPERMETRO_LUN_TAG + "_" + it_hm.hmpairid;

    if (it_hm.arrayid == array_id)  {
         
        if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == it_hm.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_hm.uiResAcess){
            SourceDeviceInfo sd;
            sd.source_id = source_dev_id;
            sd.source_name = it_hm.masterlunname;
            sd.status = "read-only";
            if (it_hm.strIsinCg.compare("true") == 0){
                sd.stretched = "";
                sd.cptype = "";
            }
            else{
                sd.stretched = it_hm.strstretched;
                sd.cptype = it_hm.strCptype;
            }

            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            sd.target_info = td;

            source_devs.lst_source_devices.push_back(sd);
        }
        else   {
            SourceDeviceInfo sd;
            sd.source_id = source_dev_id;
            sd.source_name = it_hm.masterlunname;
            sd.status = "read-write";
            if (it_hm.strIsinCg.compare("true") == 0){
                sd.stretched = "";
                sd.cptype = "";
            }
            else{
                sd.cptype = it_hm.strCptype;
                sd.stretched = it_hm.strstretched;
            }

            IdentityInfo ideninfo;
            ideninfo.source_wwn = it_hm.masterlumwwn;
            sd.identity_info = ideninfo;

            TargetDeviceInfo td;
            td.target_key = target_dev_id;
            sd.target_info = td;

            source_devs.lst_source_devices.push_back(sd);
        }
    }
    else         {
         
        if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == it_hm.pairstatus){
            if ("true" == it_hm.strCptype){
                TargetDeviceInfo td;
                td.target_key = target_dev_id;
                td.target_id = target_dev_id;
                td.target_name = it_hm.slavelunname;
                if ((int)RESOURCE_ACCESS_READ_WRITE == it_hm.uiResAcess){
                    td.target_state = "read-write";
                }
                else{
                    td.target_state = "read-only";
                }

                if (it_hm.strIsinCg.compare("true") == 0){
                    td.stretched = "";
                }
                else{
                    td.stretched = it_hm.strstretched;
                }
                td.cptype = "";

                target_devs.lst_target_devices.push_back(td);
            }
            else{
                SourceDeviceInfo sd;
                sd.source_id = target_dev_id;
                sd.source_name = it_hm.slavelunname;
                if ((int)RESOURCE_ACCESS_READ_WRITE == it_hm.uiResAcess){
                    sd.status = "read-write";
                }
                else{
                    sd.status = "read-only";
                }
                if (it_hm.strIsinCg.compare("true") == 0){
                    sd.stretched = "";
                    sd.cptype = "";
                }
                else{
                    sd.stretched = it_hm.strstretched;
                    sd.cptype = it_hm.strCptype;
                }

                TargetDeviceInfo td;
                td.target_key = source_dev_id;
                sd.target_info = td;

                source_devs.lst_source_devices.push_back(sd);
            }
        }
        else  {
            if ("true" == it_hm.strCptype){
                TargetDeviceInfo td;
                td.target_key = target_dev_id;
                td.target_name = it_hm.slavelunname;
                td.target_state = "read-write";
                if (it_hm.strIsinCg.compare("true") == 0){
                    td.stretched = "";
                }
                else{
                    td.stretched = it_hm.strstretched;
                }
                td.cptype = "";

                IdentityInfo ideninfo;
                ideninfo.source_wwn = it_hm.slavelunwwn;
                td.identity_info = ideninfo;

                td.snapshot_info = it_hm.snapshot_info;

                target_devs.lst_target_devices.push_back(td);
            }
            else{
                SourceDeviceInfo sd;
                sd.source_id = target_dev_id;
                sd.source_name = it_hm.slavelunname;
                sd.status = "read-write";
                if (it_hm.strIsinCg.compare("true") == 0){
                    sd.stretched = "";
                    sd.cptype = "";
                }
                else{
                    sd.cptype = it_hm.strCptype;
                    sd.stretched = it_hm.strstretched;
                }

                IdentityInfo ideninfo;
                ideninfo.source_wwn = it_hm.slavelunwwn;
                sd.identity_info = ideninfo;

                TargetDeviceInfo td;
                td.target_key = source_dev_id;
                sd.target_info = td;

                source_devs.lst_source_devices.push_back(sd);
            }
        }
    }
}

void DiscoverDevices::_write_consist(CgDetail &it_cg)
{
    string source_cg_id = CONSISTENTGROUP_PRE + it_cg.masterlun + "_" + MIRROR_LUN_CONSISTENT_TAG + "_" + it_cg.cgid;
    string target_cg_id = CONSISTENTGROUP_PRE + it_cg.slavelun + "_" + MIRROR_LUN_CONSISTENT_TAG + "_" + it_cg.cgid;

     
    if (it_cg.arrayid == array_id){
        ConGroupInfo cg;
        TargetDevicesInfo targetdev;

        cg.cg_id = source_cg_id;
        cg.cg_name = it_cg.cgname;

        TargetGroupInfo tg;
        tg.tg_key = target_cg_id;
        cg.target_group = tg;

        if (HM_SPLITED == it_cg.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_cg.uiResAcess){
            cg.status = "read-only";
        }
        else{
            cg.status = "read-write";
        }

        _write_mirror_response(it_cg.lst_mirror_info, cg.source_devices_info, targetdev);
        rep_devices.cgs_info.lst_groups_info.push_back(cg);
    }
    else{
        TargetGroupInfo tg;
        SourceDevicesInfo sourcedev;

        tg.tg_key = target_cg_id;
        tg.cg_name = it_cg.cgname;

        if (HM_SPLITED == it_cg.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_cg.uiResAcess){
            tg.cg_id = target_cg_id;
            tg.status = "read-write";
        }
        else{
            tg.status = "read-only";

            _get_tg_snapshot_info(it_cg);

            tg.snapshot_info = it_cg.snapshot_info;
            if (tg.snapshot_info.tg_devices_info.lst_target_devices.size() > 0){
                 
                tg.snapshot_info.id = "snap_" + tg.tg_key;
                tg.snapshot_info.recover_info.rp_id = "rp_" + tg.snapshot_info.id;
                tg.snapshot_info.recover_info.rp_name = "rp_" + tg.snapshot_info.id;
            }
        }
        _write_mirror_response(it_cg.lst_mirror_info, sourcedev, tg.target_devices_info);
        rep_devices.tgs_info.lst_groups_info.push_back(tg);
    }

    return;
}

void DiscoverDevices::_write_consisthm(CgHMDetail &it_cghm)
{
    string source_cg_id = CONSISTENTGROUP_PRE + it_cghm.masterlun + "_" + HYPERMETRO_LUN_CONSISTENT_TAG + "_" + it_cghm.cgid;
    string target_cg_id = CONSISTENTGROUP_PRE + it_cghm.slavelun + "_" + HYPERMETRO_LUN_CONSISTENT_TAG + "_" + it_cghm.cgid;

     
    if (it_cghm.arrayid == array_id){
        HMConGroupInfo cghm;
        TargetDevicesInfo targetdev;

        cghm.cghm_id = source_cg_id;
        cghm.cghm_name = it_cghm.cgname;
        cghm.stretched = it_cghm.strstretched;
        cghm.cptype = it_cghm.strCptype;

        TargetGroupInfo tg;
        tg.tg_key = target_cg_id;
        cghm.target_group = tg;

        if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == it_cghm.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_cghm.uiResAcess){
            cghm.status = "read-only";
        }
        else{
            cghm.status = "read-write";
        }

        _write_hypermetro_response(it_cghm.lst_hypermetro_info, cghm.source_devices_info, targetdev);
        rep_devices.hmcgs_info.lst_hmgroups_info.push_back(cghm);
    }
    else{
        if ("true" == it_cghm.strCptype){
            TargetGroupInfo tg;
            SourceDevicesInfo sourcedev;

            tg.tg_key = target_cg_id;
            tg.cg_name = it_cghm.cgname;
            tg.stretched = it_cghm.strstretched;
            tg.cptype = "";

            if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == it_cghm.pairstatus){
                tg.cg_id = target_cg_id;
                if ((int)RESOURCE_ACCESS_READ_WRITE == it_cghm.uiResAcess){
                    tg.status = "read-write";
                }
                else{
                    tg.status = "read-only";
                }
            }
            else{
                tg.status = "read-write";

                _get_tghm_snapshot_info(it_cghm);

                tg.snapshot_info = it_cghm.snapshot_info;
                if (tg.snapshot_info.tg_devices_info.lst_target_devices.size() > 0){
                     
                    tg.snapshot_info.id = "snap_" + tg.tg_key;
                    tg.snapshot_info.recover_info.rp_id = "rp_" + tg.snapshot_info.id;
                    tg.snapshot_info.recover_info.rp_name = "rp_" + tg.snapshot_info.id;
                }
            }
            _write_hypermetro_response(it_cghm.lst_hypermetro_info, sourcedev, tg.target_devices_info);
            rep_devices.tgs_info.lst_groups_info.push_back(tg);
        }
        else{
            HMConGroupInfo cghm;
            TargetDevicesInfo targetdev;

            cghm.cghm_id = target_cg_id;
            cghm.cghm_name = it_cghm.cgname;
            cghm.stretched = it_cghm.strstretched;
            cghm.cptype = it_cghm.strCptype;

            TargetGroupInfo tg;
            tg.tg_key = source_cg_id;
            cghm.target_group = tg;

            if (HM_SLAVE_PAIR_STATUS_STR_PAUSE == it_cghm.pairstatus && (int)RESOURCE_ACCESS_READ_WRITE == it_cghm.uiResAcess){
                cghm.status = "read-only";
            }
            else{
                cghm.status = "read-write";
            }

            _write_hypermetro_response(it_cghm.lst_hypermetro_info, cghm.source_devices_info, targetdev);
            rep_devices.hmcgs_info.lst_hmgroups_info.push_back(cghm);
        }
    }

    return;
}