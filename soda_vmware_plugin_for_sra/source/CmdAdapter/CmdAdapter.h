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

#ifndef CMD_ADAPTER_H
#define CMD_ADAPTER_H

#include "InCLIImp.h"
#include "Enum_define.h"
#define   WWN_FILTER_CON   ":"
typedef struct tagHyperStorageInfo{
    string strHostName;
    string strArraySN;
    string strArrayUser;
    string strArrayPasswd;
    string strArrayName;
    string strArrayIP;
    int iCommuType;
    string strArrayModel;
    int iArrayStatus;  //2=Online, 1=Offline (This status exists for out-of-band devices)
    string strAppHostName;
    string strArrayVersion;

    void dump(ostringstream &oss) {
        oss <<"HostName=" <<strHostName 
            <<",ArraySN=" <<strArraySN
            <<",User=" <<strArrayUser
            <<",Password=" <<strArrayPasswd
            <<",ArrayName=" <<strArrayName
            <<",ArrayIP=" <<strArrayIP
            <<",CommuType=" <<iCommuType
            <<",ArrayMode=" <<strArrayModel
            <<",ArrayStatus=" <<iArrayStatus
            <<",AppHostName=" <<strAppHostName
            <<",ArrayVersion=" <<strArrayVersion;
    }

}HYPER_STORAGE_STRU;

struct tagLunInfo{
    string strLunID;
    string strLunName;
    string strLunCapacity;
};

struct tagInbandHost{
    string ip;
    bool isOnline;
};

typedef struct tag_TIME_ZONE_INFO{
    string strTimeZone; 
    string strTimeZoneName; 
    unsigned int uiStyle; //The display style of the time zone name, 0- short name (three-letter form) 1 - long name
    unsigned int uiDstEnable; //Whether daylight saving time is used in this area, 0-not used 1-use
} TIME_ZONE_INFO_STRU;

//Array information
typedef struct tag_SYSTEM_INFO{
    string strName; 
    string strWWN; 
    string strLocation;
    unsigned int uiProductMode;
    string strProductVersion;
    string strTime;
    string strProMode;
    string strDeviceSerialNumber;
    string striSCSIName; 
    string strProductMode;
    string patchVersion;
} SYSTEM_INFO_STRU;

//snapshot information
typedef struct tag_HYIMAGE_INFO{
    string strID; 
    string strName; 
    unsigned int uiSnapType;
    unsigned int uiStatus; 
    unsigned int uiState;
    unsigned int uiRollBackProgress; 
    string strTimeStamp;
    string strWWN;
    string vstoreID;
} HYIMAGE_INFO_STRU;

//luncopy information
typedef struct tag_LUNCOPY_INFO{
    string strID;
    string strName;
    string strSrcLUNID;
    unsigned int uiStatus;
    unsigned int uiState;
    unsigned int uiType;
    int iProgress;
} LUNCOPY_INFO_STRU;

typedef struct tag_LUNCOPY_MEMBER_INFO{
    string strID;
    string strName;
    string strLunCopyID;
    unsigned int uiType;
    unsigned int uiClassType;
    unsigned int uiLunScopeType;
} LUNCOPY_MEMBER_INFO_STRU;

// Remote replication for SAN information
typedef struct tag_HYMIRROR_INFO{
    string strID;
    string strName;
    unsigned int uiStatus;
    unsigned int uiState;
    string strPriLUNID;
    string strPriLUNName;
    unsigned int uilocalResType;
    unsigned int uiModel;
    bool bIsBelongGroup;
    string strGroupID;
    string strPriArrayWWN;
    string strPriArrayName;
    bool bIsPrimary;
    unsigned int uiSecResAccess;
    string vstorePairID;
    string strPriVstoreID;
} HYMIRROR_INFO_STRU;

//HyperMetroPair information
typedef struct tag_HyperMetroPair_INFO{
    string strID;
    string strName;
    string strLocalLUNID;
    string strLocalLUNName;
    string strRemoteLunID;
    string strRemoteLunName;
    string strDomainID;
    string strDomainName;
    string strPriArrayWWN;
    string strPriArrayName;
    string strGroupID;
    bool bIsBelongGroup;
    bool bIsPrimary;
    string stretched;
    string iProgress;
    unsigned int uiStatus;
    unsigned int uiState;
    unsigned int uiRelationType;
    unsigned int uiSecResAccess;
} HYPERMETROPAIR_INFO_STRU;

typedef struct tag_REMOTE_ARRAY_INFO{
    string strID;
    string strName;
    unsigned int uiRunState;
    string strWWN;
    string strSN;
    string strStretched;
}REMOTE_ARRAY_STRU;

//Remote replication for FS information
typedef struct tag_HYMIRROR_LF_INFO{
    string strID;
    string strName;
    string strMirrorID;
    unsigned int uiRelationType;
    unsigned int uilocalResType;
    string strArrayWWN;
    string strArrayName;
    string strSyncStartTime;
    unsigned int uiSyncType;
    int iProgress;
    unsigned int uiDataStatus;
    unsigned int uiPairStatus;
    unsigned int uiSecResAccess;
} HYMIRROR_LF_INFO_STRU;

typedef struct tag_VSTORE_PAIR_INFO{
    string strID;
    string strLocalVstoreid;
    string strRemoteArrayWWN;
    string strRemoteVstoreid;
    string strRemoteDeviceid;
    unsigned int uiIdentyReserve;
    bool isPrimary;
    unsigned int uiStatus;
    unsigned int uiState;
    unsigned int uiSecResAccess;
} VSTORE_PAIR_INFO_STRU;

typedef struct tag_HYPERMETRO_LF_INFO{
    string strID;
    string strName;
    string strHMpairID;
    unsigned int uiRelationType;
    string strArrayWWN;
    string strArrayName;
    string strSyncStartTime;
    string strstretched;
    string strCptype;
    string strIsinCg;
    string strDomainID;
    string strDomainName;
    unsigned int uiSyncType;
    int iProgress;
    unsigned int uiDataStatus;
    unsigned int uiPairStatus;
    unsigned int uiSecResAccess;
    unsigned int uiLocalAccess;
    unsigned int uiHealthStatus;
} HYPERMETRO_LF_INFO_STRU;

typedef struct tag_HYPERMETRODOMAIN_LF_INFO{
    string strID;
    string strName;
    string strRemoteArraySN;
    string strRemoteArrayName;
    string strCptype;
    unsigned int uiStatus;
} HYPERMETRODOMAIN_LF_INFO_STRU;

typedef struct tag_GROUP_INFO{
    string strID;
    string strName;
    unsigned int uiStatus;
    string strStatus;
    string hmdomain;
    unsigned int uiState;
    unsigned int uiModel;
    unsigned int uiSyncType;
    unsigned int uiIsPrimary;
    unsigned int uiSecResAccess;
    unsigned int uiSyncDirection;
    string strCptype;
} GROUP_INFO_STRU;

typedef struct tag_HYCLONE_INFO{
    string strID;
    string strName;
    unsigned int uiState;
    unsigned int uiStatus;
    string strPriLUNID;
    string strPriLUNName;
    string strSlaveLUNName;
    string strSlaveLUNID;
} HYCLONE_INFO_STRU;

typedef struct tag_HYCLONE_LUN_INFO{
    string strID;
    string strName;
    string strCloneID;
    unsigned int uiDataStatus;
    unsigned int uiPairStatus;
    int iProgress;
    string strSyncStartTime;
    string strSyncEndTime;
    string strSplitTime;
    unsigned int uiRelationType;
    string strPairID;
} HYCLONE_LUN_INFO_STRU;

typedef struct tag_CMD_HOST_INFO{
    string strID;
    string strName;
    string strHostGroupID;
    string strHostIP;
} CMDHOSTINFO_STRU;

typedef struct tag_HOST_PORT_INFO{
    string strHostID;
    string strIQN;
    string strChapName;
    unsigned int uiStatus;
    unsigned int uiState;
    unsigned int uifcOriscsi;        //0:issci,1:fc
} HOST_PORT_INFO_STRU;

typedef struct tag_HOST_LINK_INFO{
    string    host_id;
    string initiator_id;
    string initiator_port_wwn;
    string target_id;
    string target_port_wwn;
}HOST_LINK_INFO;                


typedef struct tag_LUN_INFO{
    string strID;
    string strName;
    unsigned int uiState;
    unsigned int uiStatus;
    string strCapacity;
    string strWWN;
    string strOwnerControlID;

    string strImgID;
    string strCopyID;
    string strMirrorID;
    string strCloneID;
    string strVstoreID;
    string strCreateTime;
} LUN_INFO_STRU;

typedef struct tag_FS_INFO{
    string strID;
    string strName;
    unsigned int uiState;
    unsigned int uiStatus;
    string strCapacity;
    string strOwnerControlID;
    string strImgID;
    string strNFSID;
    string strPath;
    string vstoreId;
} FS_INFO_STRU;

typedef struct tag_CLONEFS_INFO{
    string strID;
    string strName;
    string strParentSnapID;
    string strParentFSID;
    string vstoreId;
} CLONEFS_INFO_STRU;

typedef struct tag_MAP_INFO{
    string strMapID;
    string strHostGroupID;
    string strHostGroupName;
    string strLunGroupID;
    string strLUNWWN;
    string strDevLUNID;
    string strHostLUNID;
    string strHostID;
    string strHostName;
    unsigned int uiIsCmdLUN;
    unsigned int uiIsSnap;
} MAP_INFO_STRU;

typedef struct tag_MAP_IDINFO{
    string strHostGroupID;
    string strLunGroupID;
} MAP_IDINFO;

typedef struct tag_NFS_INFO{
    string strNfsID;
    string strSharePath;
    string strFsID;
    string strVstoreID;
} NFS_INFO_STRU;

typedef struct tag_LIF_INFO{
    string strLIFIP;
    string vstoreID;
    string operationalStatus;
    unsigned int role;
}LIF_INFO_STRU;

typedef struct tagTLVLogInInfo{
    list<string> lstArrayIP;
    string strArrayUser;
    string strArrayPwd;
} TLV_LOGIN_INFO_STRU;

typedef struct tag_VIS_SYSTEM_INFO{
    string strSystemSerialNumber;
    unsigned int uiSystemModel;
    string strSystemName;
    string strSystemLocation;
    string strSystemTime;
    string strSystemELabel;
    string strSystemEnsList;        //ESN list for applying for a license
    string strSystemVersion;
} VIS_SYSTEM_INFO_STRU;

typedef struct tag_RVG_SIMPLE_INFO{
    string strReplicateDiskGroupName;
    string strReplicateRvgName;
    string strReplicateSecondaryHost;
} RVG_INFO_STRU;

typedef struct tag_BATCH_RVG_TOTAL_INFO{
    string strRvgDiskGroupName;
    string strRvgName;
    string strRvgVersion;
    string strRvgLogowner;
    int iRvgStatus;
    string strReplicationSecondaryHost;
} QUERY_RVG_INFO_STRU;

typedef struct tag_SECONDARY_HOST_INFO{
    string strSecondaryHostIp;
    string strReplicateModel;
    string strReplicateStatus;
    string strDataStatus;
    string strLogStatus;
    string strHasSync;
    string strBandWidth;
} SECONDARY_HOST_INFO_STRU;

typedef struct tag_CONTROLLER_INFO{
    unsigned int uiControllerId;
    unsigned int uiControllerFrameId;                      /*Node's BSN uint32, used as the frame numbe*/
    unsigned int uiControllerSlotId;                       /*Slot number of the node, which is 0 and 1.*/
    string strControllerIp;
    unsigned int uiControllerIsMaster;
    unsigned int uiControllerStatus; 
    unsigned int uiBuzzerStatus;                           /* Buzzer status on the controller uint32 */
    unsigned int uiAlarmLightStatus;
    string strControllerELabel;
    string strVersionSoftware;
    string strVersionPCB;
    string strVersionBackboardPCB;
    string strVersionBIOS;
    string strVersionOS;
    string strVersionBMC;
    string strVersionOperationSoftware;
    string strVersionMasterCPLD;
    string strVersionStandbyCPLD; 
    unsigned int uiControllerTeperatureStatus;
    unsigned int uiControllerVoltageStatus;
}VIS_CONTROLLER_INFO_STRU;

typedef struct tag_TLV_CONTROLLER_INFO{
    unsigned int uiControllerId;
    string strControllerIp;
    unsigned int uiControllerIsMaster;
    unsigned int uiControllerStatus; 
}CONTROLLER_INFO_STRU;

//add to support DAR
typedef struct tag_DAR_HOST_INFO{
    string strID;
    string strType;
    string strGroup;
}DAR_HOST_INFO;

typedef struct tag_Initiator_INFO{
    string strID;
    bool isFree;
    unsigned int uiStatus;
    unsigned int uiType;
    string strHostID;
    string strHostName;
}INITIATOR_INFO;

class CCmdAdapter{
public:
    CCmdAdapter(const string& strSN = "") : m_strSN(strSN) {m_strUser=""; m_strPwd="";};
    virtual ~CCmdAdapter() {};

    int SetSN(const string& sn);
    const string GetSN() {return m_strSN;};

    int SetUser(const string& user);
    const string GetUser() {return m_strUser;};

    int SetPwd(const string& pwd);
    const string GetPwd() {return m_strPwd;};

    void SetLoginInfo(const TLV_LOGIN_INFO_STRU& rstLoginIfo );
    const TLV_LOGIN_INFO_STRU GetLoginInfo(){return m_stLoginInfo;};

    virtual int CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo){return RETURN_OK;}
    virtual int CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStrecthed){return RETURN_OK;}
    virtual int CMD_showarrayclonefs()
    {
        g_bnfs = false;
        return RETURN_OK;
    }
    virtual int CMD_showlun(list<LUN_INFO_STRU>& rlstLunInfo) {return RETURN_OK;}
    virtual int CMD_dellun(const list<string> &lunsname) { return RETURN_OK; }
    virtual int CMD_showlunByname(const string& name, LUN_INFO_STRU &rstLUNInfo) { return RETURN_OK; }
    virtual int CMD_cloneSnap2Lun(const string& newLunname, const string& srcname) { return RETURN_OK; }
    virtual int CMD_maplun2host(const string& hostId, const list<string>& luns) { return RETURN_OK; }
    virtual int CMD_maplun2hostGroup(const string& hostId, const list<string>& luns) { return RETURN_OK; }
    virtual int CMD_unmaplun2host(const string& hostId, const list<string>& luns) { return RETURN_OK; }
    virtual int CMD_unmaplun2hostGroup(const string& hostId, const list<string>& luns) { return RETURN_OK; }
    virtual int CMD_showlun(LUN_INFO_STRU& rstLunInfo) {return RETURN_OK;}
    virtual int CMD_showfs(FS_INFO_STRU& rstFSInfo) {return RETURN_ERR;}
    virtual int CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo) {return RETURN_OK;}
    virtual int CMD_showvstorepairmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo) {return RETURN_OK;}
    virtual int CMD_showmapByName(IN const string& name, OUT list<MAP_INFO_STRU> &rlstMapInfo) { return RETURN_OK; }
    virtual int CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo) {return RETURN_OK;}
    virtual int CMD_showconsistgrinfo(OUT list<GROUP_INFO_STRU> &rlstGroupInfo) {return RETURN_OK;}
    virtual int CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo) {return RETURN_OK;}
    virtual int CMD_syncconsistgr(IN string strGroupID) {return RETURN_OK;}
    virtual int CMD_syncvstorepair(IN const string& strGroupID) {return RETURN_OK; }
    virtual int CMD_syncconsistgrhm(IN string strGroupID) {return RETURN_OK;}
    virtual int CMD_swapconsistgr(IN string strGroupID) {return RETURN_OK;}
    virtual int CMD_swapvstorepair(IN const string& strGroupID) {return RETURN_OK;}
    virtual int CMD_swapconsistgrhm(IN string strGroupID) {return RETURN_OK;}
    virtual int CMD_splitconsistgr(IN string strGroupID) {return RETURN_OK;}
    virtual int CMD_splitvstorepair(IN const string& strGroupID) {return RETURN_OK;}
    virtual int CMD_showlunsbyconsisid(const string& consis_id,vector<string>& local_lun_ids){return RETURN_OK;}
    virtual int CMD_stopconsistgrhm(IN string strGroupID,bool isPrimary) {return RETURN_OK;}
    virtual int CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo) {return RETURN_OK;}
    virtual int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo) { return RETURN_OK; }
    virtual int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo,LUN_INFO_STRU& rstLUNInfo) { return RETURN_OK; }
    virtual int CMD_showhost(OUT list<CMDHOSTINFO_STRU> &rlstHostInfo) {return RETURN_OK;}
    virtual int CMD_showhost(OUT CMDHOSTINFO_STRU &rHostInfo) { return RETURN_OK;}
    virtual int CMD_shownfs(IN string vstoreID, OUT list<NFS_INFO_STRU> &rlstnfsInfo) {return RETURN_OK;}
    virtual int CMD_addnfsclient(IN const string& vstoreid, IN string &strIP,IN string &strfsID) {return RETURN_OK;}
    virtual int CMD_delnfs(IN const string& vstoreid, IN const string& strId) {return RETURN_OK;}
    virtual int CMD_addnfs(IN const string& vstoreid, IN string &strFSID,OUT string &rstrNFSID,OUT string &rstrsharPath){return RETURN_OK;}
    virtual int CMD_showLIF(OUT list<LIF_INFO_STRU> &rlstLifInfo) {return RETURN_OK;}
    virtual int CMD_showmap(IN HOST_ID_INFO_STRU &stHostIDInfo, OUT list<MAP_INFO_STRU> &rlstMapInfo){return RETURN_OK;}
    virtual int CMD_unmountSlave(IN string &strMirrorID){return RETURN_OK;}
    virtual int CMD_createhyimg(IN string &rstrSnapshotName, IN string &rstrSrcLUNID) {return RETURN_OK;}
    virtual int CMD_createCloneFS(IN string vstoreID, IN string &rstrSnapshotName, IN string &rstrSrcFSID, OUT CLONEFS_INFO_STRU &dstClonefsInfo){return RETURN_OK;}
    virtual int CMD_showvstorepair_info(IN const string &pairid, OUT VSTORE_PAIR_INFO_STRU &vstorePair){return RETURN_OK;}
    virtual int CMD_createFShyimg(IN string &rstrSnapshotName, IN string &rstrSrcFSID) {return RETURN_OK;}
    virtual int CMD_recreatehyimg(IN string &rstrSnapshotID){return RETURN_OK;}
    virtual int CMD_recreatehyimg(IN list<string> &rlstSnapshotID){return RETURN_OK;}
    virtual int CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo){return RETURN_OK;}
    virtual int CMD_showhyimgoflun(IN string &rstrLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo) {return RETURN_OK;}
    virtual int CMD_showlunforhyimg(OUT list<LUN_INFO_STRU> &rlstLUNInfo){return RETURN_OK;}
    virtual int CMD_synchymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID) {return RETURN_OK;}
    virtual int CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID){return RETURN_OK;}
    virtual int CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID,bool isPrimary){return RETURN_OK;}
    virtual int CMD_swaphymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID) {return RETURN_OK;}
    virtual int CMD_swaphypermetro(IN string &strHMPairID) {return RETURN_OK;}
    virtual int CMD_synchmpair(IN string &strHMPairID){return RETURN_OK;}
    virtual int CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo) {return RETURN_OK;}
    virtual int CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo) {return RETURN_OK;}
    virtual int CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo) {return RETURN_OK;}
    virtual int CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo){return RETURN_OK;}
    virtual int CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo) {return RETURN_OK;}
    virtual int CMD_showhymirrorlun(IN OUT HYMIRROR_LF_INFO_STRU &rstHyMirrorLUNInfo){return RETURN_OK;}
    virtual int CMD_showhymirrorlun_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo){return RETURN_OK;}
    virtual int CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo) {return RETURN_OK;}
    virtual int CMD_showhymirrorfs_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo) {return RETURN_OK;}
    virtual int CMD_showhymirrorbyfs(IN string &strID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo,IN bool bismirrorID) {return RETURN_OK;}
    virtual int CMD_showFShyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo) {return RETURN_OK;}
    virtual int CMD_showhyimgoffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo) {return RETURN_OK;}
    //add to support nas
    virtual int CMD_showClonefsOffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo){return RETURN_OK;}
    virtual int CMD_shownfs_byfsid(IN const string& vstoreid, IN string fsid, OUT list<NFS_INFO_STRU> &rlstnfsInfo){return RETURN_OK;}
    virtual int CMD_delclonefs(IN const string &strId){return RETURN_OK;}
    virtual int CMD_delhyimg(IN string &rstrSnapshotID){return RETURN_OK;}
    virtual int CMD_delFShyimg(IN string &strSnapshotID){return RETURN_OK;}
    virtual int CMD_disablehyimg(IN string &rstrSnapshotID) {return RETURN_OK;}
    virtual int CMD_activehyimg(IN string rstrSnapshotID){return RETURN_OK;}
    virtual int CMD_activehyimg(IN list<string> &rlstSnapshotID) {return RETURN_OK;}
    virtual int CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo){return RETURN_OK;}
    virtual int CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo) {return RETURN_OK;}

    virtual int CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode){return RETURN_OK;}
    virtual int CMD_changeSlaveFsRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode){return RETURN_OK;}
    virtual int CMD_changeCGSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode) {return RETURN_OK;}
    virtual int CMD_changeVstorePairSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode) {return RETURN_OK;}

    virtual int InitConnectInfo(const string& strSN) {return RETURN_OK;}
    virtual int ConfigConnectInfo(const string& strSN, const string& strIP, const string& strUser, const string& strPwd){return RETURN_OK;}

    virtual int CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_showvisrvginfo(OUT list<QUERY_RVG_INFO_STRU> &lstQryRvgInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_showvissecondhostinfo(IN RVG_INFO_STRU &rstRvgInfo,OUT list<SECONDARY_HOST_INFO_STRU> &lstSecondHostInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_showvissecondhostinfo(IN RVG_INFO_STRU &rstRvgInfo,OUT SECONDARY_HOST_INFO_STRU &rstSecondHostInfo,OUT string &strErrorMsg){return RETURN_OK;}
    virtual int CMD_stoprvg(IN RVG_INFO_STRU &rstRvgInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_startrvg(IN RVG_INFO_STRU &rstRvgInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_showctrinfo(OUT list<CONTROLLER_INFO_STRU> &rlstControllerInfo,OUT string &strErrorMsg) {return RETURN_OK;}
    virtual int CMD_showHostByHostGroup(const string &strHostGroupID, list<CMDHOSTINFO_STRU> &lstHosts){return RETURN_OK;}
    virtual int CMD_showhostlink(IN string strHostID,IN unsigned int nFlag,OUT list<HOST_LINK_INFO> &rlstHostLinkInfo){return RETURN_OK;}

    virtual int CMD_showarraymodelinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo){return RETURN_OK;}
    virtual string CMD_getctrip(){return "";};

    virtual int CMD_showLunGroupByLUN(const string &strLunID, list<string> &strLunGroupID, int objType) {return RETURN_OK;}
    virtual int CMD_showInitiatorInfo(const string &initiatorID, const string& type, INITIATOR_INFO& initiatorInfo) {return RETURN_OK;}
    virtual int CMD_removeHostFromHostGroup(const string& hostID, const string& groupID) {return RETURN_OK;}
    virtual int CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo) {return RETURN_OK;}
    virtual int CMD_addLun2LunGroup(const string &strLunID, const string &strLunGroupID, int objType) {return RETURN_OK;}
    virtual int CMD_CreateLunGroup(const string &lunGroupName, string &lunGroupID) {return RETURN_OK;}
    virtual int CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID) {return RETURN_OK;}
    virtual int CMD_removeInitiatorFromHost(const string &initiatorID, const string &type) {return RETURN_OK;}
    virtual int CMD_CreateHostGroup(const string& hostGroupName, string &hostGroupID) {return RETURN_OK;}
    virtual int CMD_addHostsToHostGroup(const string& hostGroupID, const string &host) {return RETURN_OK;}
    virtual int CMD_createMappingView(const string& name, string &mvID) {return RETURN_OK;}
    virtual int CMD_associateToMappingview(const string& mvID, const int &type, const string& ID) {return RETURN_OK;}
    virtual int CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo) {return RETURN_OK;}
    virtual int CMD_showMapByHostGroup(const string &strHostGrupID, string &strMapID) {return RETURN_OK;}
    virtual int CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID) {return RETURN_OK;}
    virtual int CMD_delLunFromLunGroup(const string &strLunID, const string &strLunGroupID, int objType) {return RETURN_OK;}

    virtual int CMD_showMapbyLunGroup(const string &strLunGroupID, list<string> &lstMapID) {return RETURN_OK;}
    virtual int CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID) {return RETURN_OK;}
    virtual int CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID) {return RETURN_OK;} 
    virtual int CMD_showHostGroupByHost(const string &strHostID, list<string> &lstHostGroupID) {return RETURN_OK;} 
    virtual int CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns)  {return RETURN_OK;} 
    virtual    int CMD_showSnapShotByLUNGroup(const string &strLunGroupID, list<HYIMAGE_INFO_STRU> &lstSnapShots) {return RETURN_OK;}
    virtual int CMD_showHostByLUN(const string &lunID, list<string> &lstHosts, const int objType) {return RETURN_OK;}

    virtual int CMD_showMapbyHostGroup(const string &strHostGroupID, list<string> &lstMapID) {return RETURN_OK;}
    virtual int CMD_showLUNGroupByName(const string &strLunGroupName, string &strLunGroupID) {return RETURN_OK;}
    virtual int CMD_showHostGroupByName(const string &strHostGroupName, string &strHostGroupID) {return RETURN_OK;} 
    virtual int CMD_showMappingByName(const string &strMappingName, string &mapID){return RETURN_OK;}
    virtual int CMD_removeLunGroupFromMap(const string &mapID, const string &lunGroupID) {return RETURN_OK;}  
    virtual int CMD_removeHostGroupFromMap(const string &mapID, const string &hostGroupID) {return RETURN_OK;}
    virtual int CMD_delMappingByMapID(const string &mapID) {return RETURN_OK;}

    virtual int CMD_delMapping(const string& mapID, const string& hostGroupID, const string& lunGroupID) {return RETURN_OK;} 
    virtual int CMD_delLunGroup(const string& lunGroupID, const string& snapshotID, int objType = OBJ_SNAPSHOT) {return RETURN_OK;}  
    virtual int CMD_delHostGroup(const string& hostGroupID, const list<string> &lstHostIDs) {return RETURN_OK;}    
    virtual int CMD_delHostFromHostGroup(const string& hostGroupID, const string& hostID) {return RETURN_OK;}  

protected:
    void CMD_translatemodel(IN unsigned int &uiProductModel,OUT string &strProductModel);
    void CMD_addFilterToWWN(IN string& strSourceWWN, OUT string& strTargetWWN);
    void CMD_delFrontZero(IN string& strSourceID, OUT string& strTargetID);
    void CMD_delFilterFromWWN(IN string strSourceWWN, OUT string& strTargetWWN);

protected:
    string m_strSN;
    string m_strUser;
    string m_strPwd;
    TLV_LOGIN_INFO_STRU m_stLoginInfo; //Connection array information
};
#endif

