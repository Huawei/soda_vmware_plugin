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

#ifndef _CMD_REST_FUSIONSTORAGE_ADAPTER_H_
#define _CMD_REST_FUSIONSTORAGE_ADAPTER_H_


#include "RESTConn.h"


#define FUSION_SNAPSHOT  "snapshot"
#define FUSION_SNAPSHOTID "snapshotId"
#define FUSION_SNAPSHOTNAME "snapshotName"
#define FUSION_STATUS "status"
#define FUSION_SNAPSHOT_CREATETIME "createTime"
#define FUSION_WWN "wwn"
#define FUSION_SNAP_LIST "snapshotList"
#define FUSION_VOL_LIST "volumeList"
#define FUSION_VOL_NAME "volName"
#define FUSION_LUN_CREATE_TIME  "createTime"
#define FUSION_LUN_DETAIL_INFO "lunDetailInfo"
#define FUSION_LUN_LIST "volumeList"
#define FUSION_DATA "data"
#define FUSION_VOL_ID "volId"
#define FUSION_HOST_LIST "hostList"
#define FUSION_HOST_NAME "hostName"
#define FUSION_HOST_ID "hostId"
#define FUSION_HOST_LUN_ID "lunId"
#define FUSION_HOST_GROUP_LIST "hostGroupList"
#define FUSION_HOST_GROUP_NAME "hostGroupName"
#define FUSION_HOST_CREATETIME "createTime"
#define FUSION_REPLICATIONPAIR_TAG_REMOTEDEVICESN "system_wwn"
#define FUSION_REPLICATIONPAIR_REMOTEDEVICESN "REMOTEDEVICESN"
#define FUSION_PORT_ISFREE "ISFREE"
#define FUSION_PORT_ID "ID"
#define FUSION_PORT_STATUS "RUNNINGSTATUS"
#define FUSION_PORT_HOST_ID "PARENTID"
#define FUSION_PORT_HOST_NAME "PARENTNAME"
#define FUSION_COMM_ID "ID"
#define FUSION_RP_CONSIS_GP_NAME "NAME"
#define FUSION_HEALTHSTATUS "HEALTHSTATUS"
#define FUSION_RUNNINGSTATUS "RUNNINGSTATUS"


#define FUSION_URL_VOL_LIST "/volume/list"
#define FUSION_URL_QUERY_LUN_BY_NAME "/volume/queryByName?volName="
#define FUSION_URL_VOL_SNAP_LIST "/volume/snapshot/list"
#define RESTURL_URL_QUERY_BY_NAME "/queryByName"
#define RESTURL_URL_QUERY_BY_ID "/queryById"
#define FUSION_URL_VOL "/volume"
#define FUSION_URL_QUERY_HOST_MAP "/lun/host/list"
#define FUSION_URL_CREATE_LUN_BY_SNAP "/snapshot/volume/create"
#define FUSION_URL_DEL_LUN "/volume/delete"
#define FUSION_URL_QUERY_HOSTGROUP_MAP  "/volume/hostGroup/list"
#define FUSION_URL_HOST_LIST "/host/list"
#define FUSION_URL_QUERY_HOSTS_BY_GROUPNAME "/hostGroup/host/list"
#define FUSION_URL_MAP_LUN_TO_HOSTGROUP "/hostGroup/lun/add"
#define FUSION_URL_UNMAP_LUN_TO_HOSTGROUP "/hostGroup/lun/delete"
#define FUSION_URL_MAP_LUN_TO_HOST "/host/lun/add"
#define FUSION_URL_UNMAP_LUN_TO_HOST "/host/lun/delete"
#define FUSION_URL_PORT_LIST "/port/list"
#define FUSION_URL_PORT_INITIATOR_2_HOST "/host/port/add"
#define FUSION_URL_HOST_CREATE "/host/create"
#define FUSION_URL_RP_CONSIS_GROUP "/CONSISTENTGROUP"
#define FUSION_URL_SPLIT_CONSISTENCY_GROUP "/SPLIT_CONSISTENCY_GROUP"

#define FUSION_REQ_VOL_NAME "volName"
#define FUSION_REQ_SRC_SNAP_NAME "src"
#define FUSION_REQ_BATCH_NUM "batchNum"
#define FUSION_REQ_BATCH_LIMIT "batchLimit"
#define FUSION_REQ_POOLID "poolId"
#define FUSION_REQ_PAFE_NUM "pageNum"
#define FUSION_REQ_PAGE_SIZE "pageSize"
#define FUSION_REQ_VOLUME_NAME "volumeName"
#define FUSION_REQ_LUN_NAME "lunName"
#define FUSION_REQ_HOSTGROUP_NAME "hostGroupName"
#define FUSION_REQ_LUN_LIST "lunList"
#define FUSION_REQ_VOL_NAMES "volNames"
#define FUSION_REQ_HOST_NAME "hostName"
#define FUSION_REQ_LUN_NAMES "lunNames"
#define FUSION_REQ_PORT_NAME "portName"
#define FUSION_REQ_PORT_NAMES "portNames"

#define FUSION_FILTER "filters"
#define FUSION_FILTER_VOL_NAME "volumeName"



#define POOLID 0
#define PAGENUM 1
#define PAGESIZE 10

#define HM_CPTYPE_NONE "HC_CP_NONE"
#define HC_CP_SERVER "HC_CP_SERVER"
#define FUSION_HCRESOURCETYPE_FS "ST_FILE_SYSTEM"

#define FUSION_HEALTH_NORMAL 1
#define FUSION_HEALTH_FAULT 2
#define FUSION_HEALTH_DEGRADE 5

#define FUSION_DOMAIN_RUNING_NORMAL 1
#define FUSION_DOMAIN_RUNING_INVALID 35
#define FUSION_DOMAIN_RUNING_BE_RECOVERED 33

#define GET_JSON_VALUE(object,returnObj,name)\
Json::Value returnObj; \
do{\
returnObj = object[name]; \
if (returnObj == Json::nullValue)\
{\
COMMLOG(OS_LOG_ERROR, "the jsonObject does not have this field [%s].", name); \
return RETURN_ERROR; \
}\
}while (0);

class CRESTFusionStorage : public CCmdAdapter
{
public:
    CRESTFusionStorage(const string& strSN = "");
    virtual ~CRESTFusionStorage();

    //System query related
    virtual int CMD_showarraymodelinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo);    
    virtual int CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo);
    virtual int CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg);
    virtual int CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo);
    virtual int CMD_maplun2host(const string& hostname, const list<string>& luns);
    virtual int CMD_maplun2hostGroup(const string& hostname, const list<string>& luns);
    virtual int CMD_unmaplun2host(const string& hostname, const list<string>& luns);
    virtual int CMD_unmaplun2hostGroup(const string& hostname, const list<string>& luns);
    virtual int CMD_dellun(const list<string> &lunsname);

    //Remote replication information related
    virtual int CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    virtual int CMD_showhymirrorlun_all(IN string &strMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo);

    virtual int CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    virtual int CMD_showlunByname(const string& name, LUN_INFO_STRU &rstLUNInfo);
    virtual int CMD_showInitiatorInfo(const string &initiatorName, const string& type, INITIATOR_INFO& initiatorInfo);
       virtual int CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode);
    virtual int CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo);
    virtual int CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo);

    virtual int CMD_synchymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID);
    virtual int CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo);
    virtual int CMD_swaphymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID);
    virtual int CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID);

    //LUN
    virtual int CMD_showlun(LUN_INFO_STRU &rstLUNInfo);

    //snapshot
    virtual int CMD_showhyimgoflun(IN string &strLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    virtual int CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo);

    virtual int CMD_createhyimg(IN string &strSnapshotName, IN string &strSrcLUNID);
    virtual int CMD_cloneSnap2Lun(const string& newLunname,const string& srcname);
    virtual int CMD_delhyimg(IN string &strSnapshotID);

    //mappingview
    virtual int CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo);
    virtual int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    virtual int CMD_showmapByName(IN const string& name, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    virtual int CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo, LUN_INFO_STRU& rstLUNInfo);
    virtual int InitConnectInfo(const string& strSN);
    virtual int ConfigConnectInfo(const string& strSN, const string& strIP, const string& strUser, const string& strPwd);
    virtual int CMD_unmountSlave(IN string &strMirrorID);
    
    //hyperMetro
    virtual int CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStrecthed);
    virtual int CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    virtual int CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo);
    virtual int CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo);
    virtual int CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo);
    virtual int CMD_swaphypermetro(IN string &strHMPairID);
    virtual int CMD_stopconsistgrhm(IN string strGroupID);
    virtual int CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    virtual int CMD_showconsistgrinfo(IN OUT list<GROUP_INFO_STRU> &rstGroupInfo);
    virtual int CMD_splitconsistgr(IN string strGroupID);
    virtual int CMD_swapconsistgrhm(IN string strGroupID);
    virtual int CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID);
    virtual int CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo);
    virtual int CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    virtual int CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    virtual int CMD_syncconsistgrhm(IN string strGroupID);
    virtual void ConvertHMSlaveLUNStatus(IN OUT unsigned int &ruiIntState);
    virtual void ConvertHMPairStatus(IN OUT unsigned int &ruiIntState);
    virtual void UpdateGroupStatus(IN OUT unsigned int &ruiIntStatus);
    virtual int CMD_synchmpair(IN string &strHMPairID);
    virtual void ConvertHostPortStatus(IN OUT unsigned int &ruiIntStatus);
    virtual int CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo);
    
private:
    enum MAP_METHOD{MAP,UNMAP};
    int QueryHostsByHostGroupName(const string& groupName, vector<string>& hosts);
    int QueryHostInfoByHostName(const string& hostname,string& createTime, long long & hostId);
    int QueryHostMapByVolName(const LUN_INFO_STRU& lun, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    int QueryHostGroupMapByVolName(const LUN_INFO_STRU& lun, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    void TimeChg(IN unsigned long long ullTime,OUT string &strTime);
    void UpdateModel(IN OUT unsigned int &ruiMode);
    void SnapshotConvertStatus(IN const unsigned int uiHealthStatus, IN OUT unsigned int &ruiStatus);
    void SnapshotConvertType(IN OUT unsigned int &ruiType);
    void UpdateMirrorStatus(IN OUT unsigned int &ruiIntStatus);
    void ConvertMirrorPairStatus(IN OUT unsigned int &ruiIntState);
    void ConvertMirrorSlaveLUNStatus(IN OUT unsigned int &ruiIntState);
    void ConvertSrcLUNStatus(IN const unsigned int uiHealthStatus, IN const unsigned int uiRunStatus, IN OUT unsigned int &ruiIntState);
    int operateHostMapping(MAP_METHOD method, const string& hostname, const list<string>& luns);
    int operateHostGroupMapping(MAP_METHOD method, const string& hostGroupname, const list<string>& luns);
    int fullRestPkg(const string& url, REST_REQUEST_MODE reqMode,const string& body ,string& strIp, CRestPackage& restPkg);
    int findRemoteArraySn(const string& sn, REMOTE_ARRAY_STRU& remote_dev);

private:
    list<pair<string, CRESTConn *>> m_connList;
    //Get connected
    CRESTConn *getConn(string &, string &, string &);
};

#endif
