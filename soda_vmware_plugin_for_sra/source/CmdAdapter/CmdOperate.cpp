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

#include "CmdOperate.h"

CCmdOperate::CCmdOperate(void)
{
    m_iCommType = RETURN_ERR;
    m_strSN = "";
    m_objAdapter = NULL;
}

CCmdOperate::~CCmdOperate(void)
{
    try{
        if (NULL != m_objAdapter){
            delete m_objAdapter;
            m_objAdapter = NULL;
        }
    }
    catch(...){};
}

void CCmdOperate::SetCommType(IN int iCommType)
{
    m_iCommType = iCommType;
}

const int CCmdOperate::GetCommType() const
{
    return m_iCommType;
}

void CCmdOperate::SetArraySN(string strSN)
{
    m_strSN = strSN;
}

const string CCmdOperate::GetArraySN() const
{
    return m_strSN;
}

int CCmdOperate::SetStorageInfo(HYPER_STORAGE_STRU &rstHyperStor)
{
    int iRtn = RETURN_OK;
    
    iRtn = CheckByType(rstHyperStor.iCommuType);
    if (RETURN_OK != iRtn){
        return iRtn;
    }

    if (NULL != m_objAdapter){
        return m_objAdapter->ConfigConnectInfo(rstHyperStor.strArraySN, rstHyperStor.strArrayIP, rstHyperStor.strArrayUser, rstHyperStor.strArrayPasswd);
    }

    return RETURN_ERR;
}

int CCmdOperate::CheckByType(IN int &riCommType)
{
    int iRtn = RETURN_OK;

    if (NULL != m_objAdapter){
        delete m_objAdapter;
        m_objAdapter = NULL;
    }

    //Create the corresponding object based on how it communicates with the array
    switch (riCommType){
    case COMMUNICATION_TYPE_INBAND:
        break;

    case COMMUNICATION_TYPE_TLV:
        break;

    case COMMUNICATION_TYPE_VIS:
        break;

    case COMMUNICATION_TYPE_REST:
        if (g_bFusionStorage){
            m_objAdapter = new CRESTFusionStorage();
        }
        else{
            m_objAdapter = new CRESTCmd();
        }

        break;
        
    default:
        COMMLOG(OS_LOG_ERROR, 
            "The communication type(%d) is wrong.", m_iCommType);

        m_iCommType = RETURN_ERR;

        iRtn = RETURN_ERR;

        break;
    }

    return iRtn;
}

int CCmdOperate::CheckArraySN(string &rstrArraySN)
{
    int iRtn = RETURN_OK;
    
    return iRtn;
}

int CCmdOperate::CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showarrayinfo(rlstRemoteArrayInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayStretched)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showarraystretched(rlstRemoteArrayStretched);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showarrayclonefs()
{
    if(NULL != m_objAdapter){
        return m_objAdapter->CMD_showarrayclonefs();
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_maplun2host(const string& hostname, const list<string>& luns)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_maplun2host(hostname, luns);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_maplun2hostGroup(const string& hostGroupname, const list<string>& luns)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_maplun2hostGroup(hostGroupname, luns);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_unmaplun2host(const string& hostname, const list<string>& luns)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_unmaplun2host(hostname, luns);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_unmaplun2hostGroup(const string& hostGroupname, const list<string>& luns)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_unmaplun2hostGroup(hostGroupname, luns);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_showlun(list<LUN_INFO_STRU>& rlstLunInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showlun(rlstLunInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showlun(LUN_INFO_STRU& rstLunInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showlun(rstLunInfo);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_showfs(FS_INFO_STRU& rstFSInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showfs(rstFSInfo);
    }
    return RETURN_ERR;
}

int CCmdOperate::CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showconsistgrmember(strGroupID, rlstHyMirrorInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showvstorepairmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showvstorepairmember(strGroupID, rlstHyMirrorInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showconsistgrinfo(rstGroupInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo)
{
    if (NULL != m_objAdapter){
        return m_objAdapter->CMD_showconsistgrhminfo(rstGroupInfo);
    }

    return RETURN_ERR;
}

int CCmdOperate::CMD_showconsistgrinfo(OUT list<GROUP_INFO_STRU> &rlstGroupInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showconsistgrinfo(rlstGroupInfo);
}

int CCmdOperate::CMD_syncconsistgr(IN string strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_syncconsistgr(strGroupID);
}

int CCmdOperate::CMD_syncvstorepair(IN const string& strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_syncvstorepair(strGroupID);
}

int CCmdOperate::CMD_syncconsistgrhm(IN string strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_syncconsistgrhm(strGroupID);
}

int CCmdOperate::CMD_swapconsistgr(IN string strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_swapconsistgr(strGroupID);
}

int CCmdOperate::CMD_swapvstorepair(IN const string& strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_swapvstorepair(strGroupID);
}

int CCmdOperate::CMD_swapconsistgrhm(IN string strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_swapconsistgrhm(strGroupID);
}

int CCmdOperate::CMD_splitconsistgr(IN string strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_splitconsistgr(strGroupID);
}

int CCmdOperate::CMD_splitvstorepair(IN const string& strGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_splitvstorepair(strGroupID);
}

int CCmdOperate::CMD_stopconsistgrhm(IN string strGroupID,bool isPrimary)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_stopconsistgrhm(strGroupID,isPrimary);
}

int CCmdOperate::CMD_showlunsbyconsisid(const string& consis_id,vector<string>& local_lun_ids)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showlunsbyconsisid(consis_id,local_lun_ids);
}

int CCmdOperate::CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showmap(rlstMapInfo);
}

int CCmdOperate::CMD_shownfs(IN string vstoreID, OUT list<NFS_INFO_STRU> &rlstnfsInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_shownfs(vstoreID, rlstnfsInfo);
}

int CCmdOperate::CMD_shownfs_byfsid(IN const string& vstoreid, IN string fsid, OUT list<NFS_INFO_STRU> &rlstnfsInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_shownfs_byfsid(vstoreid, fsid, rlstnfsInfo);
}

int CCmdOperate::CMD_delclonefs(IN const string &strId)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_delclonefs(strId);
}

int CCmdOperate::CMD_addnfsclient(IN const string& vstoreid,IN string &strIP,IN string &strfsID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_addnfsclient(vstoreid, strIP,strfsID);
}

int CCmdOperate::CMD_delnfs(IN const string& vstoreid, IN const string& strId)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_delnfs(vstoreid, strId);
}

int CCmdOperate::CMD_addnfs(IN const string& vstoreid, IN string &strFSID,OUT string &rstrNFSID,OUT string &rstrsharPath)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_addnfs(vstoreid, strFSID,rstrNFSID,rstrsharPath);
}

int CCmdOperate::CMD_showLIF(OUT list<LIF_INFO_STRU> &rlstLifInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showLIF(rlstLifInfo);
}

int CCmdOperate::CMD_showhost(OUT list<CMDHOSTINFO_STRU> &rlstHostInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhost(rlstHostInfo);
}

int CCmdOperate::CMD_showhost(OUT CMDHOSTINFO_STRU &rHostInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhost(rHostInfo);
}

int CCmdOperate::CMD_showmap(IN HOST_ID_INFO_STRU &stHostIDInfo, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showmap(stHostIDInfo, rlstMapInfo);
}

int CCmdOperate::CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showmap(volID, rlstMapInfo);
}

int CCmdOperate::CMD_showmap(IN const string& volID, OUT list<MAP_INFO_STRU> &rlstMapInfo,LUN_INFO_STRU& rstLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showmap(volID, rlstMapInfo,rstLUNInfo);
}

int CCmdOperate::CMD_createhyimg(IN string &rstrSnapshotName, IN string &rstrSrcLUNID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_createhyimg(rstrSnapshotName, rstrSrcLUNID);
}

int CCmdOperate::CMD_unmountSlave(IN string &strMirrorID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_unmountSlave(strMirrorID);
}

int CCmdOperate::CMD_createCloneFS(IN string vstoreID, IN string &rstrSnapshotName, IN string &rstrSrcFSID, OUT CLONEFS_INFO_STRU &dstClonefsInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_createCloneFS(vstoreID, rstrSnapshotName, rstrSrcFSID, dstClonefsInfo);
}

int CCmdOperate::CMD_showClonefsOffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showClonefsOffs(strFSID, rlstHyImageInfo);
}

int CCmdOperate::CMD_createFShyimg(IN string &rstrSnapshotName, IN string &rstrSrcFSID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_createFShyimg(rstrSnapshotName, rstrSrcFSID);
}

int CCmdOperate::CMD_recreatehyimg(IN string &rstrSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_recreatehyimg(rstrSnapshotID);
}

int CCmdOperate::CMD_recreatehyimg(IN list<string> &rlstSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_recreatehyimg(rlstSnapshotID);
}

int CCmdOperate::CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhyimginfo(rstHyImageInfo);
}

int CCmdOperate::CMD_showhyimgoflun(IN string &rstrLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhyimgoflun(rstrLUNID, rlstHyImageInfo);
}

int CCmdOperate::CMD_showFShyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo)
{

    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showFShyimginfo(rstHyImageInfo);
}

int CCmdOperate::CMD_showhyimgoffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhyimgoffs(strFSID, rlstHyImageInfo);
}

int CCmdOperate::CMD_showlunforhyimg(OUT list<LUN_INFO_STRU> &rlstLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showlunforhyimg(rlstLUNInfo);
}

int CCmdOperate::CMD_synchymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_synchymirror(rstrMirrorID, rstrSlaveArraySN, rstrSlaveLUNID);
}

int CCmdOperate::CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_splithymirror(rstrMirrorID, rstrSlaveArraySN, rstrSlaveLUNID);
}

int CCmdOperate::CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID,bool isPrimary)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_disablehcpair(rstrHMPairID, rstrSlaveArraySN, rstrSlaveLUNID,isPrimary);
}

int CCmdOperate::CMD_swaphymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_swaphymirror(rstrMirrorID, rstrSlaveArraySN, rstrSlaveLUNID);
}

int CCmdOperate::CMD_swaphypermetro(IN string &rstrHMPairID){

    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_swaphypermetro(rstrHMPairID);
}

int CCmdOperate::CMD_synchmpair(IN string &rstrHMPairID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_synchmpair(rstrHMPairID);
}

int CCmdOperate::CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhymirrorinfo(rstHyMirrorInfo);
}

int CCmdOperate::CMD_showlunByname(const string& name, LUN_INFO_STRU &rstLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showlunByname(name, rstLUNInfo);
}

int CCmdOperate::CMD_dellun(const list<string> &lunsname)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_dellun(lunsname);
}

int CCmdOperate::CMD_showmapByName(IN const string& name, OUT list<MAP_INFO_STRU> &rlstMapInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showmapByName(name, rlstMapInfo);
}

int CCmdOperate::CMD_cloneSnap2Lun(const string& newLunname, const string& srcname)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_cloneSnap2Lun(newLunname, srcname);
}

int CCmdOperate::CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhymirrorinfo_all(rlstHyMirrorInfo);
}

int CCmdOperate::CMD_showhypermetroinfo(OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhypermetroinfo(rstHMPairInfo);
}

int CCmdOperate::CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showHyperMetroPair_all(rlstHyperMetroPairInfo);
}

int CCmdOperate::CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhyperMetroDomain_all(hmdomainid, rstHMDomainInfo);
}

int CCmdOperate::CMD_showhymirrorlun(IN OUT HYMIRROR_LF_INFO_STRU &rstHyMirrorLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhymirrorlun(rstHyMirrorLUNInfo);
}

int CCmdOperate::CMD_showhymirrorlun_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhymirrorlun_all(rstrMirrorID, rlstHyMirrorLUNInfo);
}

int CCmdOperate::CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhypermetrolun_all(hypermetroid, rlstHMLUNInfo);
}

int CCmdOperate::CMD_showhymirrorfs_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhymirrorfs_all(rstrMirrorID, rlstHyMirrorFSInfo);
}

int CCmdOperate::CMD_showhymirrorbyfs(IN string &strID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo,IN bool bismirrorID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showhymirrorbyfs(strID,rlstHyMirrorFSInfo,bismirrorID);
}


int CCmdOperate::CMD_delhyimg(IN string &rstrSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delhyimg(rstrSnapshotID);
}

int CCmdOperate::CMD_delFShyimg(IN string &strSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delFShyimg(strSnapshotID);

}

int CCmdOperate::CMD_disablehyimg(IN string &rstrSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_disablehyimg(rstrSnapshotID);
}

int CCmdOperate::CMD_activehyimg(IN string rstrSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_activehyimg(rstrSnapshotID);
}

int CCmdOperate::CMD_activehyimg(IN list<string> &rlstSnapshotID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_activehyimg(rlstSnapshotID);
}

int CCmdOperate::CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showsys(rstSysInfo);
}

int CCmdOperate::CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showhostport(strHostID, rlstHostPortInfo);
}

int CCmdOperate::CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_changeSlaveLunRw(strHyMirrorID, accessMode);
}

int CCmdOperate::CMD_changeSlaveFsRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode) 
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_changeSlaveFsRw(strHyMirrorID,accessMode);
}

int CCmdOperate::CMD_changeCGSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_changeCGSlaveLunRw(strGroupID, accessMode);
}

int CCmdOperate::CMD_changeVstorePairSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_changeVstorePairSlaveLunRw(strGroupID, accessMode);
}

int CCmdOperate::InitConnectInfo(const string& strSN)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->InitConnectInfo(strSN);
}

int CCmdOperate::SetSN(const string& sn)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->SetSN(sn);
}

const string CCmdOperate::GetSN()
{
    if (NULL == m_objAdapter){
        return string();
    }
    return m_objAdapter->GetSN();
}

/*------------------------------------------------------------
Function Name: CMD_showtlvsysinfo()
Description  : Query VIS system information.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : rstStorageInfo:returns system structure information
Return       : Success or failure
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showtlvsysinfo(rstStorageInfo,strErrorMsg);
}

/*------------------------------------------------------------
Function Name: CMD_showctrinfo()
Description  : Query the controller information of the array.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : CONTROLLER_INFO_STRU:returns controller structure information
Return       : Success or failure
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showctrinfo(OUT list<CONTROLLER_INFO_STRU> &rlstControllerInfo,OUT string &strErrorMsg)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    //Call the general interface, which can query the system information of the array and the system information of the VIS (according to the specific implementation)
    return m_objAdapter->CMD_showctrinfo(rlstControllerInfo,strErrorMsg);//This method is called by both the VIS and the out-of-band of the array.
}

/*------------------------------------------------------------
Function Name: InitTlvConnectInfo()
Description  : Connect when initializing the scan.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : 
Return       : Success or failure
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/

void CCmdOperate::InitTlvConnectInfo(IN HYPER_STORAGE_STRU &rstStorageInfo,IN bool hasFlush)
{
    return;
}

/*------------------------------------------------------------
Function Name: CMD_flushsysinfo()
Description  : Refresh controller information.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : rstStorageInfo:returns system structure information
Return       : Success or failure
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_flushsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo)
{
    int iRet = RETURN_OK;
    string strTmpErrorMsg;

    if(rstStorageInfo.strArrayIP.empty() 
        || rstStorageInfo.strArrayUser.empty() 
        || rstStorageInfo.strArrayPasswd.empty()
        || rstStorageInfo.iCommuType > COMMUNICATION_TYPE_REST
        || rstStorageInfo.iCommuType < COMMUNICATION_TYPE_INBAND){
        COMMLOG(OS_LOG_ERROR,
            "%s","The parameters of query Storageinfo isn't correct");
        return RETURN_ERR;
    }

    //Determine the communication class based on the type passed in
    iRet = CheckByType(rstStorageInfo.iCommuType);

    if (RETURN_OK != iRet){
        return iRet;
    }
    
    //Initialize connection operation (refresh VIS system information according to all IP addresses)
    InitTlvConnectInfo(rstStorageInfo,true);

    //Query system information
    iRet = CMD_showtlvsysinfo(rstStorageInfo,strTmpErrorMsg);

    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "Query system Information of Storage failed. the errorCode is:(%d)", iRet);
        return iRet;
    }

    return iRet;
}

/*------------------------------------------------------------
Function Name: CMD_showrvginfo()
Description  : Query VIS RVG information..
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : lstRvgInfo:Return RVG list information
Return       : Success or failure.
Call         :
Called by    :
Created By   : 
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showrvginfo(OUT list<QUERY_RVG_INFO_STRU> &lstRvgInfo,OUT string &strErrorMsg)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showvisrvginfo(lstRvgInfo,strErrorMsg);
}

/*------------------------------------------------------------
Function Name: CMD_showrvgsecondaryhostinfo()
Description  : Query VIS RVG information.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : lstRvgInfo:Return RVG list information
Return       : Success or failure.
Call         :
Called by    :
Created By   : 
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showrvgsecondaryhostinfo(IN RVG_INFO_STRU &rstRvgInfo, OUT list<SECONDARY_HOST_INFO_STRU> &lstSecondaryHostInfo,OUT string &strErrorMsg)
{
    string strTmpDiskGroupName;
    string strTmpRvgName;

    strTmpDiskGroupName = rstRvgInfo.strReplicateDiskGroupName;
    strTmpRvgName = rstRvgInfo.strReplicateRvgName;
    
    if (strTmpDiskGroupName.empty() || strTmpRvgName.empty()){
        COMMLOG(OS_LOG_ERROR, 
            "%s","the Input Param is Empty.");
        return RETURN_ERR;
    }

    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showvissecondhostinfo(rstRvgInfo,lstSecondaryHostInfo,strErrorMsg);
}

/*------------------------------------------------------------
Function Name: CMD_showrvgsecondaryhostinfo()
Description  : Query VIS RVG information.
Data Accessed: None.
Data Updated : None.
Input        : None
Output       : lstRvgInfo:Return RVG list information
Return       : Success or failure.
Call         :
Called by    :
Created By   : 
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showrvgsecondaryhostinfo(IN RVG_INFO_STRU &rstRvgInfo, OUT SECONDARY_HOST_INFO_STRU &rstSecondaryHostInfo,OUT string &strErrorMsg)
{
    string strTmpSecondaryHost;
    string strTmpDiskGroupName;
    string strTmpRvgName;

    strTmpDiskGroupName = rstRvgInfo.strReplicateDiskGroupName;
    strTmpRvgName = rstRvgInfo.strReplicateRvgName;
    strTmpSecondaryHost = rstRvgInfo.strReplicateSecondaryHost;
    
    if (strTmpDiskGroupName.empty() || strTmpRvgName.empty() || strTmpSecondaryHost.empty()){
        COMMLOG(OS_LOG_ERROR, 
            "%s","the Input Param is Empty.");
        return RETURN_ERR;
    }

    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showvissecondhostinfo(rstRvgInfo,rstSecondaryHostInfo,strErrorMsg);
}

/*------------------------------------------------------------
Function Name: CMD_showhostByhostGroup()
Description  : Query the host through the host group
Data Accessed: None.
Data Updated : None.
Input        : strHostGroupID.
Output       : lstHosts.
Return       : Success or failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate::CMD_showhostByhostGroup(const string &strHostGroupID, list<CMDHOSTINFO_STRU> &lstHosts)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showHostByHostGroup(strHostGroupID, lstHosts);
}

/*------------------------------------------------------------
Function Name: CMD_showhostlink()
Description  : Query host link information through the host.
Data Accessed: None.
Data Updated : None.
Input        : strHostGroupID.
Output       : lstHosts.
Return       : Success or failure.
Call         :
Called by    :
Created By   :
Modification :
Others       :
-------------------------------------------------------------*/
int CCmdOperate:: CMD_showhostlink(IN string initatorWwn,IN unsigned int nFalg,list<HOST_LINK_INFO> &rlstHostLinkInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showhostlink(initatorWwn,nFalg,rlstHostLinkInfo);
}

int CCmdOperate::CMD_showvstorepair_info(IN const string &pairid, OUT VSTORE_PAIR_INFO_STRU &vstorePair)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showvstorepair_info(pairid, vstorePair);
}

int CCmdOperate::CMD_showLunGroupByLUN(const string &strLunID, list<string> &strLunGroupID, int objType)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showLunGroupByLUN(strLunID, strLunGroupID, objType);
}

int CCmdOperate::CMD_showMapbyLunGroup(const string &strLunGroupID, list<string> &lstMapID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showMapbyLunGroup(strLunGroupID, lstMapID);
}

int CCmdOperate::CMD_showMapbyHostGroup(const string &strHostGroupID, list<string> &lstMapID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showMapbyHostGroup(strHostGroupID, lstMapID);
}

int CCmdOperate::CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showHostGroupByMap(strMapID, strHostGroupID);
}

int CCmdOperate::CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showHostbyHostGroup(HostGroupID, lstHostID);
}

int CCmdOperate::CMD_showHostGroupByHost(const string &strHostID, list<string> &lstHostGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_showHostGroupByHost(strHostID, lstHostGroupID);
}

 int CCmdOperate::CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showLunByLunGroup(strLunGroupID, lstLuns);
 }

 int CCmdOperate::CMD_showSnapShotByLUNGroup(const string &strLunGroupID, list<HYIMAGE_INFO_STRU> &lstSnapShots)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showSnapShotByLUNGroup(strLunGroupID, lstSnapShots);
 }

 int CCmdOperate::CMD_showHostByLUN(const string &lunID, list<string> &lstHosts, const int objType)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showHostByLUN(lunID, lstHosts, objType);
 }

 int CCmdOperate::CMD_showLUNGroupByName(const string &strLunGroupName, string &strLunGroupID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showLUNGroupByName(strLunGroupName, strLunGroupID);
 }

 int CCmdOperate::CMD_showHostGroupByName(const string &strHostGroupName, string &strHostGroupID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showHostGroupByName(strHostGroupName, strHostGroupID);
 }

 int CCmdOperate::CMD_showMappingByName(const string &strMappingName, string &mapID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_showMappingByName(strMappingName, mapID);
 }

 int CCmdOperate::CMD_removeLunGroupFromMap(const string &mapID, const string &lunGroupID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_removeLunGroupFromMap(mapID, lunGroupID);
 }

 int CCmdOperate::CMD_removeHostGroupFromMap(const string &mapID, const string &hostGroupID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_removeHostGroupFromMap(mapID, hostGroupID);
 }

 int CCmdOperate::CMD_delMappingByMapID(const string &mapID)
 {
     if (NULL == m_objAdapter){
         return RETURN_ERR;
     }

     return m_objAdapter->CMD_delMappingByMapID(mapID);
 }

int CCmdOperate::CMD_delMapping(const string& mapID, const string& hostGroupID, const string& lunGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delMapping(mapID, hostGroupID, lunGroupID);
}

int CCmdOperate::CMD_delLunGroup(const string& lunGroupID, const string& snapshotID, int objType)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delLunGroup(lunGroupID, snapshotID, objType);
}

int CCmdOperate::CMD_delHostGroup(const string& hostGroupID, const list<string>& lstHostIDs)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delHostGroup(hostGroupID, lstHostIDs);
}

int CCmdOperate::CMD_delHostFromHostGroup(const string& hostGroupID, const string& hostID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }

    return m_objAdapter->CMD_delHostFromHostGroup(hostGroupID, hostID);
}

int CCmdOperate::CMD_showInitiatorInfo(const string &initiatorID, const string& type, INITIATOR_INFO& initiatorInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showInitiatorInfo(initiatorID, type, initiatorInfo);
}

int CCmdOperate::CMD_removeHostFromHostGroup(const string& hostID, const string& groupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_removeHostFromHostGroup(hostID, groupID);
}

int CCmdOperate::CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_createHost(hostName, hostInfo);
}

int CCmdOperate::CMD_CreateLunGroup(const string &lunGroupName, string &lunGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_CreateLunGroup(lunGroupName, lunGroupID);
}

int CCmdOperate::CMD_addLun2LunGroup(const string &strLunID, const string &strLunGroupID, int objType)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_addLun2LunGroup(strLunID, strLunGroupID, objType);
}

int CCmdOperate::CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_addInitiatorToHost(initiatorID, type, strHostID);
}

int CCmdOperate::CMD_CreateHostGroup(const string& hostGroupName, string &hostGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_CreateHostGroup(hostGroupName, hostGroupID);
}
    
int CCmdOperate::CMD_addHostsToHostGroup(const string& hostGroupID, const string &host)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_addHostsToHostGroup(hostGroupID, host);
}

int CCmdOperate::CMD_createMappingView(const string& name, string &mvID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_createMappingView(name, mvID);
}

int CCmdOperate::CMD_associateToMappingview(const string& mvID, const int &type, const string& ID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_associateToMappingview(mvID, type, ID);
}

int CCmdOperate::CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showhost(name, rHostInfo);
}

int CCmdOperate::CMD_removeInitiatorFromHost(const string &initiatorID, const string &type)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_removeInitiatorFromHost(initiatorID, type);
}

int CCmdOperate::CMD_showMapByHostGroup(const string &strHostGrupID, string &strMapID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showMapByHostGroup(strHostGrupID, strMapID);
}

int CCmdOperate::CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_showLunGroupByMap(strMapID, strLunGroupID);
}

int CCmdOperate::CMD_delLunFromLunGroup(const string &strLunID, const string &strLunGroupID, int objType)
{
    if (NULL == m_objAdapter){
        return RETURN_ERR;
    }
    return m_objAdapter->CMD_delLunFromLunGroup(strLunID, strLunGroupID, objType);
}
