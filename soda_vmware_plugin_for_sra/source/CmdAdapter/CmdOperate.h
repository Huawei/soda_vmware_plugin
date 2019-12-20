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

#ifndef CMD_OPERATE_H
#define CMD_OPERATE_H
#include "CmdRESTAdapter.h"
#include "CmdRESTFusionStorage.h"

class CCmdOperate{
public:
    CCmdOperate(void);
    ~CCmdOperate(void);

    void SetCommType(IN int iCommType);
    const int GetCommType() const;
    void SetArraySN(string strSN);
    const string GetArraySN() const;

public:
    int SetSN(const string& sn);
    const string GetSN();
    int SetStorageInfo(HYPER_STORAGE_STRU &rstHyperStor);
    int CMD_maplun2host(const string& hostname, const list<string>& luns);
    int CMD_maplun2hostGroup(const string& hostname, const list<string>& luns);
    int CMD_unmaplun2host(const string& hostname, const list<string>& luns);
    int CMD_unmaplun2hostGroup(const string& hostname, const list<string>& luns);
    int CheckByType(IN int &riCommType);
    int CheckArraySN(string &rstrArraySN);
    int CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo); 
    int CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStretched);
    int CMD_showlun(list<LUN_INFO_STRU>& rlstLunInfo);
    int CMD_showlun(LUN_INFO_STRU& rstLunInfo);
    int CMD_showfs(FS_INFO_STRU& rstFSInfo);
    int CMD_showlunsbyconsisid(const string& consis_id,vector<string>& local_lun_ids);
    int CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    int CMD_showvstorepairmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    int CMD_syncvstorepair(IN const string& strGroupID);
    int CMD_dellun(const list<string> &lunsname);
    int CMD_showmapByName(IN const string& name, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    int CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    int CMD_showconsistgrinfo(OUT list<GROUP_INFO_STRU> &rlstGroupInfo);
    int CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    int CMD_syncconsistgr(IN string strGroupID);
    int CMD_syncconsistgrhm(IN string strGroupID);
    int CMD_swapconsistgr(IN string strGroupID);
    int CMD_swapvstorepair(IN const string& strGroupID);
    int CMD_swapconsistgrhm(IN string strGroupID);
    int CMD_splitconsistgr(IN string strGroupID);
    int CMD_splitvstorepair(IN const string& strGroupID);
    int CMD_stopconsistgrhm(IN string strGroupID, bool isPrimary = false);
    int CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo);
    int CMD_shownfs(IN string vstoreID, OUT list<NFS_INFO_STRU> &rlstnfsInfo);
    int CMD_shownfs_byfsid(IN const string& vstoreid, IN string fsid, OUT list<NFS_INFO_STRU> &rlstnfsInfo);
    int CMD_cloneSnap2Lun(const string& newLunname, const string& srcname);
    int CMD_showlunByname(const string& name, LUN_INFO_STRU &rstLUNInfo);
    int CMD_addnfsclient(IN const string& vstoreid,IN string &strIP,IN string &strfsID);
    int CMD_delnfs(IN const string& vstoreid, IN const string& strId);
    int CMD_delclonefs(IN const string &strId);
    int CMD_addnfs(IN const string& vstoreid,IN string &strFSID,OUT string &rstrNFSID,OUT string &rstrsharPath);
    int CMD_showLIF(OUT list<LIF_INFO_STRU> &rlstLifInfo);
    int CMD_showhost(OUT list<CMDHOSTINFO_STRU> &rlstHostInfo);
    int CMD_showhost(OUT CMDHOSTINFO_STRU &rHostInfo);
    int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    int CMD_showmap(IN HOST_ID_INFO_STRU &stHostIDInfo, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo,LUN_INFO_STRU& rstLUNInfo);
    int CMD_unmountSlave(IN string &strMirrorID);
    int CMD_createhyimg(IN string &rstrSnapshotName, IN string &rstrSrcLUNID);
    int CMD_createCloneFS(IN string vstoreID, IN string &rstrSnapshotName, IN string &rstrSrcFSID, OUT CLONEFS_INFO_STRU &dstClonefsInfo);
    int CMD_showClonefsOffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    int CMD_createFShyimg(IN string &rstrSnapshotName, IN string &rstrSrcFSID);
    int CMD_recreatehyimg(IN string &rstrSnapshotID);
    int CMD_recreatehyimg(IN list<string> &rlstSnapshotID);
    int CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo);
    int CMD_showhyimgoflun(IN string &rstrLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    int CMD_showFShyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo);
    int CMD_showhyimgoffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    int CMD_showlunforhyimg(OUT list<LUN_INFO_STRU> &rlstLUNInfo);
    int CMD_synchymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    int CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    int CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID,bool isPrimary = false);
    int CMD_swaphymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    int CMD_swaphypermetro(IN string &strHMPairID);
    int CMD_synchmpair(IN string &strHMPairID);
    int CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo);
    int CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    int CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo);
    int CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo);
    int CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo);
    int CMD_showhymirrorlun(IN OUT HYMIRROR_LF_INFO_STRU &rstHyMirrorLUNInfo);
    int CMD_showhymirrorlun_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo);
    int CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo);
    int CMD_showhymirrorfs_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo);
    int CMD_showhymirrorbyfs(IN string &strID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo,IN bool bismirrorID = false);
    int CMD_delhyimg(IN string &rstrSnapshotID);
    int CMD_delFShyimg(IN string &strSnapshotID);
    int CMD_disablehyimg(IN string &rstrSnapshotID);
    int CMD_activehyimg(IN string rstrSnapshotID);
    int CMD_activehyimg(IN list<string> &rlstSnapshotID);
    int CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo);
    int CMD_showproductver(OUT string &rstrSysVer);
    int CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo);
    int CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode);        
    int CMD_changeSlaveFsRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode);        
    int CMD_changeCGSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode);            
    int CMD_changeVstorePairSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode);  
    int InitConnectInfo(const string& strSN);
    int CMD_flushsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo);
    int CMD_showrvginfo(OUT list<QUERY_RVG_INFO_STRU> &lstRvgInfo,OUT string &strErrorMsg);
    int CMD_showrvgsecondaryhostinfo(IN RVG_INFO_STRU &rstRvgInfo, OUT list<SECONDARY_HOST_INFO_STRU> &lstSecondaryHostInfo,OUT string &strErrorMsg);
    int CMD_showrvgsecondaryhostinfo(IN RVG_INFO_STRU &rstRvgInfo, OUT SECONDARY_HOST_INFO_STRU &rstSecondaryHostInfo,OUT string &strErrorMsg);
    int CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstSysInfo,OUT string &strErrorMsg);
    int CMD_showhostByhostGroup(const string &strHostGroupID, list<CMDHOSTINFO_STRU> &lstHosts);
    int CMD_showhostlink(IN string initatorWwn, IN unsigned int nFalg,list<HOST_LINK_INFO> &rlstHostLinkInfo);
    int CMD_showarrayclonefs();
    int CMD_showvstorepair_info(IN const string &pairid, OUT VSTORE_PAIR_INFO_STRU &vstorePair);
    int CMD_showLunGroupByLUN(const string &strLunID, list<string> &strLunGroupID, int objType);
    int CMD_showInitiatorInfo(const string &initiatorID, const string& type, INITIATOR_INFO& initiatorInfo);
    int CMD_removeHostFromHostGroup(const string& hostID, const string& groupID);
    int CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo);
    int CMD_CreateLunGroup(const string &lunGroupName, string &lunGroupID);
    int CMD_addLun2LunGroup(const string &strLunID, const string &strLunGroupID, int objType);
    int CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID);
    int CMD_removeInitiatorFromHost(const string &initiatorID, const string &type);
    int CMD_CreateHostGroup(const string& hostGroupName, string &hostGroupID);
    int CMD_addHostsToHostGroup(const string& hostGroupID, const string &host);
    int CMD_createMappingView(const string& name, string &mvID);
    int CMD_associateToMappingview(const string& mvID, const int &type, const string& ID);
    int CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo);
    int CMD_showMapByHostGroup(const string &strHostGrupID, string &strMapID);                        
    int CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID);
    int CMD_delLunFromLunGroup(const string &strLunID, const string &strLunGroupID, int objType);
    int CMD_showMapbyLunGroup(const string &strLunGroupID, list<string> &lstMapID); 
    int CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID);        
    int CMD_showHostbyHostGroup(const string &HostGroupID, list<string>& lstHostID); 
    int CMD_showHostGroupByHost(const string &strHostID, list<string> &lstHostGroupID); 
    int CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns); 
    int CMD_showSnapShotByLUNGroup(const string &strLunGroupID, list<HYIMAGE_INFO_STRU> &lstSnapShots); 
    int CMD_showHostByLUN(const string &lunID, list<string> &lstHosts, const int objType); 
    int CMD_showMapbyHostGroup(const string &strHostGroupID, list<string> &lstMapID); 
    int CMD_showLUNGroupByName(const string &strLunGroupName, string &strLunGroupID);  
    int CMD_showHostGroupByName(const string &strHostGroupName, string &strHostGroupID); 
    int CMD_showMappingByName(const string &strMappingName, string &mapID);   
    int CMD_removeLunGroupFromMap(const string &mapID, const string &lunGroupID);
    int CMD_removeHostGroupFromMap(const string &mapID, const string &hostGroupID);
    int CMD_delMappingByMapID(const string &mapID);
    int CMD_delMapping(const string& mapID, const string& hostGroupID, const string& lunGroupID); 
    int CMD_delLunGroup(const string& lunGroupID, const string& snapshotID, int objType = OBJ_SNAPSHOT);
    int CMD_delHostGroup(const string& hostGroupID, const list<string>& lstHostIDs); 
    int CMD_delHostFromHostGroup(const string& hostGroupID, const string& hostID); 
public:
    CCmdAdapter *m_objAdapter;

private:
    string m_strSN;
    int m_iCommType;

    void InitTlvConnectInfo(IN HYPER_STORAGE_STRU &rstStorageInfo,IN bool hasFlush);
    int CMD_showctrinfo(OUT list<CONTROLLER_INFO_STRU> &rlstControllerInfo,OUT string &strErrorMsg);
};

#endif
