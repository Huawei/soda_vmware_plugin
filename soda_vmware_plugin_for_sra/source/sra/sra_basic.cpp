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

#include "sra_basic.h"
#include <fstream>
#include <algorithm>

char LOG_LEVEL[10] = {0};

SraBasic::SraBasic() : error_code(RETURN_OK)
{
    memset_s(xmlns, sizeof(xmlns), 0, sizeof(xmlns));
    memset_s(output_file, sizeof(output_file), 0, sizeof(output_file));
    memset_s(status_file, sizeof(status_file), 0, sizeof(status_file));
    memset_s(log_dir, sizeof(log_dir), 0, sizeof(log_dir));
    memset_s(log_level, sizeof(log_level), 0, sizeof(log_level));

    memset_s(id, sizeof(id), 0, sizeof(id));
    memset_s(user_name, sizeof(user_name), 0, sizeof(user_name));
    memset_s(password, sizeof(password), 0, sizeof(password));
    memset_s(array_id, sizeof(array_id), 0, sizeof(array_id));
    memset_s(slaveid, sizeof(slaveid), 0, sizeof(slaveid));
    memset_s(array_stretched, sizeof(array_stretched), 0, sizeof(array_stretched));
    isNFS = false;
}

SraBasic::~SraBasic()
{

}

int SraBasic::_read_common_info(XmlReader &reader)
{

    int addressCount = 0;
    list<string> lstAddressInfo;
    string strTmpIP;
    list<string> lstMasterIPPort;
    list<string> lstSlaveIPPort;
    char *tmpstr = NULL;
    long portNum = 0;

    CHECK_FALSE(reader.get_string(XML_REQUSET_TITLE, xmlns, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_OUTPUTFILE, output_file, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_STATUSFILE, status_file, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_LOGDIRECTORY, log_dir, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_LOGLEVEL, log_level, 0), RETURN_ERR);

    
    if (INBAND_TYPE == g_cli_type){
        CHECK_FALSE(reader.get_string(XML_ARRAYSN, id, 0), RETURN_ERR);
    }
    else{
        addressCount = reader.get_count(XML_ARRAYSN);
        lstAddressInfo.clear();
        for (int i = 0; i< addressCount; i++){
            char tmpIP[LENGTH_COMMON] = {0};

            (void)reader.get_string(XML_ARRAYIP, tmpIP, i);
            lstAddressInfo.push_back(string(tmpIP));
        }
    }

    CHECK_FALSE(reader.get_string(XML_USERNAME, user_name, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_PASSWD, password, 0), RETURN_ERR);

    
    if (INBAND_TYPE != g_cli_type){
        if (lstAddressInfo.empty() || lstAddressInfo.size() != 2){
            COMMLOG(OS_LOG_ERROR, "%s", "Get Parameter failed.");
            
            return ERROR_INVALIDATE_IPADDRESS;
            
        }

        strTmpIP = lstAddressInfo.front();
        SNPRINTF(id, sizeof(id), sizeof(id) - 1, "%s", strTmpIP.c_str());
        strTmpIP = lstAddressInfo.back();
        SNPRINTF(slaveid, sizeof(slaveid), sizeof(slaveid) - 1, "%s", strTmpIP.c_str());

        
        OS_StrToken(id, ":", &lstMasterIPPort);
        OS_StrToken(slaveid, ":", &lstSlaveIPPort);

        
        if (lstMasterIPPort.empty() || lstSlaveIPPort.empty()){
            COMMLOG(OS_LOG_ERROR, "masterIP %s or slaveIP %s of OS_StrToken is empty", id, slaveid);
            return ERROR_INVALIDATE_IPADDRESS;
        }

        
        in_addr mastIP;
        in_addr slaveIP;
        if(0 == ACE_OS::inet_aton(lstMasterIPPort.front().c_str(),&mastIP) || 0 == ACE_OS::inet_aton(lstSlaveIPPort.front().c_str(),&slaveIP)){
            COMMLOG(OS_LOG_ERROR, "The Input IP is error, master ip is (%s), slave ip is (%s)", 
                lstMasterIPPort.front().c_str(), lstSlaveIPPort.front().c_str());
            
            return ERROR_INVALIDATE_IPADDRESS;
            
        }

        
        if (lstMasterIPPort.size() == 2){
            portNum = strtol(lstMasterIPPort.back().c_str(), &tmpstr, 10);

            if (portNum > 65535 || portNum <= 0) {
                COMMLOG(OS_LOG_ERROR, "The Input Port is error, [%s] master ip is (%s), port is (%s), portNum (%d)", 
                    id, lstMasterIPPort.front().c_str(), lstMasterIPPort.back().c_str(), portNum);
                
                return ERROR_INVALIDATE_IPADDRESS;
                
            }
        }

        if (lstSlaveIPPort.size() == 2){
            tmpstr = NULL;
            portNum = 0;
            portNum = strtol(lstSlaveIPPort.back().c_str(), &tmpstr, 10);

            if (portNum > 65535 || portNum <= 0){
                COMMLOG(OS_LOG_ERROR, "The Input Port is error, [%s] slave ip is (%s), port is (%s), portNum (%d)", 
                    slaveid, lstSlaveIPPort.front().c_str(), lstSlaveIPPort.back().c_str(),portNum);
                
                return ERROR_INVALIDATE_IPADDRESS;
                
            }
        }
    }

    return RETURN_OK;
}

int SraBasic::_read_command_para(XmlReader &reader)
{
    (void)reader;
    return RETURN_OK;
}

int SraBasic::_outband_process()
{
    return RETURN_OK;
}

void SraBasic::_write_response(XmlWriter &writer)
{
    (void)writer;

    return;
}

int SraBasic::_write_error_info(XmlWriter &writer)
{
    CHECK_FALSE(writer.set_int(XML_RESPONSE_ERROR, error_code), ERROR_INTERNAL_PROCESS_FAIL);

    return RETURN_OK;
}

void SraBasic::_write_progress(unsigned int percent)
{
    XmlWriter writer("ProgressUpdate");

    (void)writer.set_string(XML_PROGRESS_TITLE, xmlns);

    (void)writer.set_int(XML_PROGRESS, (int)percent);

    (void)writer.save_to_file(status_file);

}

int SraBasic::entry(XmlReader &reader)
{
    XmlWriter writer;

    COMMLOG(OS_LOG_INFO, "%s", SIGNATURE1);
    COMMLOG(OS_LOG_INFO, "%s", SIGNATURE2);
    COMMLOG(OS_LOG_INFO, "%s", SIGNATURE1);

    
    int error_read_info(RETURN_OK), error_read_param(RETURN_OK);
    error_read_info = _read_common_info(reader);
    error_read_param = _read_command_para(reader);

    if (RETURN_OK != error_read_info){
        COMMLOG(OS_LOG_ERROR, "%s", "failed to read commond info");
        error_code = error_read_info;
    }

    if (RETURN_OK != error_read_param){
        COMMLOG(OS_LOG_ERROR, "%s", "failed to read commond param");
        error_code = error_read_param;
    }
    

    if (RETURN_OK == error_code){
        process();
    }

    if (!writer.set_string(XML_RESPONSE_TITLE, xmlns)){
        error_code = ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (RETURN_OK == error_code){
        _write_response(writer);
    }
    else{
        (void)_write_error_info(writer);
    }

    if (!writer.save_to_file(output_file)){
        COMMLOG(OS_LOG_ERROR, "faild to save file(%s)", output_file);
    }

    ifstream infile(output_file);
	string outputStr;
	string strInfile;
	if (infile.is_open())
	{
		while (getline(infile, strInfile))
		{
			outputStr +=  strInfile;
#ifdef WIN32
			outputStr += "\r\n";
#else
			outputStr += "\n";
#endif
		}
	}
	else
	{
		COMMLOG(OS_LOG_WARN, "the output_file [%s] open failed ", output_file);
	}

    COMMLOG(OS_LOG_INFO, "%s", outputStr.c_str());
	infile.close();

    if(RETURN_OK == error_code){
        return RETURN_OK;
    }
    else{
        return RETURN_ERR;
    }
 
}

void print(const char *pszFormat, ...)
{
     va_list pszArgp;
     va_start(pszArgp, pszFormat);
     char *acMsg = new char[MAX_MSG_SIZE];
     if(acMsg == NULL){ 
         return;
     }
     memset_s(acMsg, MAX_MSG_SIZE, 0, MAX_MSG_SIZE);
#ifdef WIN32
     _vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#else
     (void)vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#endif
    string strTm = NowTime();
    fflush(stdout);
    printf("%s %s\n",strTm.c_str(),acMsg);
    va_end(pszArgp);
    delete []acMsg;
}

string  NowTime()
{
    struct tm *pstCurTime;
    struct tm stCurTime;
    time_t lLongTime;
    char acTime[128];
    memset_s(acTime, sizeof(acTime), 0, sizeof(acTime));
    lLongTime = OS_Now();
    pstCurTime = (struct tm *)Os_LocalTime(&lLongTime, &stCurTime);
    if (NULL == pstCurTime){
        return "[]";
    }
    (void)sprintf_s(acTime, sizeof(acTime), "[%4u-%02u-%02u %02u:%02u:%02u] [%s]:",
        pstCurTime->tm_year + 1900,
        pstCurTime->tm_mon + 1,
        pstCurTime->tm_mday,
        pstCurTime->tm_hour,
        pstCurTime->tm_min,
        pstCurTime->tm_sec,
        LOG_LEVEL);
    return string(acTime);

}

void filter_list(list<string>& rlist)
{
    vector<string> rvector;
    vector<string>::iterator iter;

    if(!rlist.empty() && rlist.size() != 0){
        size_t rlstsize = rlist.size();
        if(rlstsize >= 1){
            rvector.resize(rlstsize);
        }
    }
    (void)copy(rlist.begin(), rlist.end(), rvector.begin());
    sort(rvector.begin(), rvector.end());

    iter = unique(rvector.begin(), rvector.end());

    rvector.erase(iter, rvector.end());

    rlist.resize(rvector.size());

    (void)copy(rvector.begin(), rvector.end(), rlist.begin());

    return;
}

int check_success_out(const string& errorMsg, ErrorInfo& error_info)
{
    int ret = RETURN_OK;

    if (error_info.hasError()){
        return RETURN_ERR;
    }

    error_info.desc = errorMsg;
    error_info.hint = "Please check the information from array.";

    return ret;
}

int parse_device_id(const string& device_id, string& sn, string& lun_id, string& hyper_id)
{
    list<string> hyper_token_id;
    OS_StrToken(device_id, "_", &hyper_token_id);

    if (hyper_token_id.size() != 3){
        return RETURN_ERR;
    }

    sn = hyper_token_id.front();

    lun_id = *(++hyper_token_id.begin());

    hyper_id = hyper_token_id.back();

    return RETURN_OK;
}

void format_daytime(string& daytime)
{
    string date;
    string time;

    list<string> lst_splited_time;

    OS_StrToken(daytime, " ", &lst_splited_time);

    
    if (lst_splited_time.empty() || lst_splited_time.size() != 2){
        daytime = "";
        return;
    }

    date = lst_splited_time.front();
    time = *(++lst_splited_time.begin());

    daytime = date + "T" + time;

    return;
}

int get_lun_by_consisgr_out(CCmdOperate& cmdOperate, unsigned int role, string& consisgr_id, list<CommonLunInfo>& lst_lun_info, bool is_nfs=false)
{
    int ret = RETURN_ERR;

    string cmd;

    list<string> lst_mirror_ids;
    list<string> lst_slave_array_sn;
    list<string> lst_slave_lun_id;
    list<string> lst_slave_lun_name;

    list<string>::iterator it_mirror_id;
    list<HYMIRROR_INFO_STRU> lstHyMirrorInfo;
    list<HYMIRROR_INFO_STRU>::iterator itHyMirrorInfo;
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;

    if(is_nfs){
        ret = cmdOperate.CMD_showvstorepairmember(consisgr_id, lstHyMirrorInfo);
    }
    else{
        
        ret = cmdOperate.CMD_showconsistgrmember(consisgr_id, lstHyMirrorInfo);
    }
    CHECK_UNEQ(RETURN_OK, ret);

    itHyMirrorInfo = lstHyMirrorInfo.begin();
    while (itHyMirrorInfo != lstHyMirrorInfo.end()){
        
        lstHyMirrorLUNInfo.clear();
        lst_slave_array_sn.clear();
        lst_slave_lun_id.clear();
        lst_slave_lun_name.clear();
        ret = cmdOperate.CMD_showhymirrorlun_all(itHyMirrorInfo->strID, lstHyMirrorLUNInfo);
        CHECK_UNEQ(RETURN_OK, ret);
        
        for (itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin(); itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end();
            itHyMirrorLUNInfo++){
            if (role == itHyMirrorLUNInfo->uiRelationType){
                lst_slave_array_sn.push_back(itHyMirrorLUNInfo->strArrayWWN);
                lst_slave_lun_id.push_back(itHyMirrorLUNInfo->strID);
                lst_slave_lun_name.push_back(itHyMirrorLUNInfo->strName);
            }
        }

        if (lst_slave_array_sn.empty() || lst_slave_lun_id.empty() || lst_slave_lun_name.empty()){
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            return ERROR_INTERNAL_PROCESS_FAIL;
        }

        CommonLunInfo lun_info_tmp;
        lun_info_tmp.array_id = lst_slave_array_sn.front();
        lun_info_tmp.lun_id = lst_slave_lun_id.front();
        lun_info_tmp.lun_name = lst_slave_lun_name.front();
        lun_info_tmp.mirror_id = itHyMirrorInfo->strID;
        lun_info_tmp.is_slave = true;

        lst_lun_info.push_back(lun_info_tmp);

        ++itHyMirrorInfo;
    }

    return RETURN_OK;
}

int get_lun_by_consisgrhm_out(CCmdOperate& cmdOperate, unsigned int role, string& consisgr_id, list<CommonLunInfo>& lst_lun_info)
{
    int ret = RETURN_ERR;

    string cmd;

    list<string> lst_mirror_ids;
    list<string> lst_slave_array_sn;
    list<string> lst_slave_lun_id;
    list<string> lst_slave_lun_name;

    
    list<HYPERMETROPAIR_INFO_STRU> lstHMPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHMPairInfo;
    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLUNInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLUNInfo;

    ret = cmdOperate.CMD_showHyperMetroPair_all(lstHMPairInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    itHMPairInfo = lstHMPairInfo.begin();
    while (itHMPairInfo != lstHMPairInfo.end()){
        if (itHMPairInfo->strGroupID != consisgr_id){
            ++itHMPairInfo;
            continue;
        }
        lstHMPairLUNInfo.clear();
        lst_slave_array_sn.clear();
        lst_slave_lun_id.clear();
        lst_slave_lun_name.clear();

        ret = cmdOperate.CMD_showhypermetrolun_all(itHMPairInfo->strID, lstHMPairLUNInfo);
        CHECK_UNEQ(RETURN_OK, ret);

        
        for (itHMPairLUNInfo = lstHMPairLUNInfo.begin(); itHMPairLUNInfo != lstHMPairLUNInfo.end();
            itHMPairLUNInfo++){
            if (role == itHMPairLUNInfo->uiRelationType){
                lst_slave_array_sn.push_back(itHMPairLUNInfo->strArrayWWN);
                lst_slave_lun_id.push_back(itHMPairLUNInfo->strID);
                lst_slave_lun_name.push_back(itHMPairLUNInfo->strName);
            }
        }

        if (lst_slave_array_sn.empty() || lst_slave_lun_id.empty() || lst_slave_lun_name.empty()){
            COMMLOG(OS_LOG_ERROR, "%s", "slave info is empty.");
            return ERROR_INTERNAL_PROCESS_FAIL;
        }

        CommonLunInfo lun_info_tmp;
        lun_info_tmp.array_id = lst_slave_array_sn.front();
        lun_info_tmp.lun_id = lst_slave_lun_id.front();
        lun_info_tmp.lun_name = lst_slave_lun_name.front();
        lun_info_tmp.mirror_id = itHMPairInfo->strID;
        lun_info_tmp.is_slave = true;

        lst_lun_info.push_back(lun_info_tmp);

        ++itHMPairInfo;
    }

    return RETURN_OK;
}

int SraBasic::setStorageInfo(HYPER_STORAGE_STRU& stStorageInfo)
{
    stStorageInfo.iCommuType = g_cli_type;
    stStorageInfo.strArrayIP = string(id) + ";" + string(slaveid);
    stStorageInfo.strArrayUser = user_name;
    stStorageInfo.strArrayPasswd = password;

    return RETURN_OK;
}

int SraBasic::_get_array_info(CCmdOperate& cmdOperate)
{
    int ret = RETURN_ERR;
    string strErrorMsg;
    HYPER_STORAGE_STRU stStorageInfo;

    ret = cmdOperate.CMD_showtlvsysinfo(stStorageInfo, strErrorMsg);
    CHECK_UNEQ(RETURN_OK, ret);

    SNPRINTF(array_id, sizeof(array_id), sizeof(array_id) - 1, "%s", stStorageInfo.strArraySN.c_str());

    return RETURN_OK;
}

int SraBasic::_get_hymirror_percent_out(CCmdOperate& cmdOperate, string& hymirror_id, string& query_percent)
{
    int iRet = RETURN_ERR;

    list<string> lst_percent;
    list<string> lst_name_normal;
    list<string> lst_name_synchronizing;

    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLFInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLFInfo;

    
    if (isNFS){
        COMMLOG(OS_LOG_INFO, "resource is filesystem, CMD_showhymirrorbyfs is to be executed...");
        iRet = cmdOperate.CMD_showhymirrorbyfs(hymirror_id, lstHyMirrorLFInfo, true);
    } 
    else{
        COMMLOG(OS_LOG_INFO, "resource is LUN, CMD_showhymirrorlun_all is to be executed...");
        iRet = cmdOperate.CMD_showhymirrorlun_all(hymirror_id, lstHyMirrorLFInfo);
    }
    CHECK_UNEQ(RETURN_OK,iRet);

    for (itHyMirrorLFInfo = lstHyMirrorLFInfo.begin(); itHyMirrorLFInfo != lstHyMirrorLFInfo.end(); itHyMirrorLFInfo++){
        if (MIRROR_SLAVE_PAIR_STATUS_CHAR_NORMAL == itHyMirrorLFInfo->uiPairStatus){
            lst_name_normal.push_back(itHyMirrorLFInfo->strName);
        }

        if (MIRROR_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING == itHyMirrorLFInfo->uiPairStatus){
            lst_name_synchronizing.push_back(itHyMirrorLFInfo->strName);
        }

        if (MIRROR_LUN_RELATION_SLAVE == itHyMirrorLFInfo->uiRelationType){
            lst_percent.push_back(OS_IToString(itHyMirrorLFInfo->iProgress));
        }
        if (isNFS){
            hymirror_id = itHyMirrorLFInfo->strMirrorID;
        }
    }

    if (lst_name_normal.empty() && lst_name_synchronizing.empty()){
        return RETURN_ERR;
    }

    if (!lst_percent.empty()){
        query_percent = lst_percent.front();

        return RETURN_OK;
    }

    COMMLOG(OS_LOG_WARN, "query sync status,percent is null when slave status %s", hymirror_id.c_str());

    query_percent = "--";

    return RETURN_ERR;
}

int SraBasic::_get_hmpair_percent_out(CCmdOperate& cmdOperate, string& hymirror_id, string& query_percent)
{
    int iRet = RETURN_ERR;

    list<string> lst_percent;
    list<string> lst_name_normal;
    list<string> lst_name_synchronizing;

    list<HYPERMETRO_LF_INFO_STRU> lstHMPairLFInfo;
    list<HYPERMETRO_LF_INFO_STRU>::iterator itHMPairLFInfo;

    
    iRet = cmdOperate.CMD_showhypermetrolun_all(hymirror_id, lstHMPairLFInfo);
    CHECK_UNEQ(RETURN_OK,iRet);

    for (itHMPairLFInfo = lstHMPairLFInfo.begin(); itHMPairLFInfo != lstHMPairLFInfo.end(); itHMPairLFInfo++){
        if (MIRROR_SLAVE_PAIR_STATUS_CHAR_NORMAL == itHMPairLFInfo->uiPairStatus){
            lst_name_normal.push_back(itHMPairLFInfo->strName);
        }

        if (MIRROR_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING == itHMPairLFInfo->uiPairStatus){
            lst_name_synchronizing.push_back(itHMPairLFInfo->strName);
        }

        if (MIRROR_LUN_RELATION_SLAVE == itHMPairLFInfo->uiRelationType){
            lst_percent.push_back(OS_IToString(itHMPairLFInfo->iProgress));
        }
        if (isNFS){
            hymirror_id = itHMPairLFInfo->strHMpairID;
        }
    }

    if (lst_name_normal.empty() && lst_name_synchronizing.empty()){
        return RETURN_ERR;
    }

    if (!lst_percent.empty()){
        query_percent = lst_percent.front();

        return RETURN_OK;
    }

    COMMLOG(OS_LOG_WARN, "query sync status,percent is null when slave status %s", hymirror_id.c_str());

    query_percent = "--";

    return RETURN_ERR;
}

int SraBasic::check_array_id_validate_out(CCmdOperate& cmdOperate, string& array_id, const string& peer_array_id)
{
    int ret = RETURN_ERR;

    list<REMOTE_ARRAY_STRU> lstRemoteArrayInfo;
    list<REMOTE_ARRAY_STRU>::iterator itRemoteArrayInfo;

    list<string> lst_sn_ids;

    if (peer_array_id.empty()){
        return RETURN_OK;
    }

    
    ret = cmdOperate.CMD_showarrayinfo(lstRemoteArrayInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    for (itRemoteArrayInfo = lstRemoteArrayInfo.begin(); 
        itRemoteArrayInfo != lstRemoteArrayInfo.end(); itRemoteArrayInfo++){
        lst_sn_ids.push_back(itRemoteArrayInfo->strSN);
    }

    if (lst_sn_ids.end() == find(lst_sn_ids.begin(), lst_sn_ids.end(), peer_array_id)){
        return ERROR_INVALIDATE_PEER_ARRAY_SN;
    }

    return RETURN_OK;
}

int SraBasic::check_device_id_validate_out(CCmdOperate& cmdOperate, string& device_id, string& relation)
{
    int ret = RETURN_ERR;
    string sn;
    string lun_id;
    string hyper_id;
    string strRelation;

    list<HYMIRROR_LF_INFO_STRU> lstMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itMirrorLUNInfo;
    list<string> lst_lun_id;

    if (RETURN_OK != parse_device_id(device_id, sn, lun_id, hyper_id)){
        return RETURN_ERR;
    }

    
    ret = cmdOperate.CMD_showhymirrorlun_all(hyper_id, lstMirrorLUNInfo);
    CHECK_UNEQ(RETURN_OK, ret);

    lst_lun_id.clear();
    for (itMirrorLUNInfo = lstMirrorLUNInfo.begin(); itMirrorLUNInfo != lstMirrorLUNInfo.end(); itMirrorLUNInfo++){
        _convert_relation_type(itMirrorLUNInfo->uiRelationType, strRelation);
        if (strRelation == relation){
            lst_lun_id.push_back(itMirrorLUNInfo->strID);
        }
    }

    
    if (lst_lun_id.empty() || lst_lun_id.front() != lun_id){
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int SraBasic::get_hymirror_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hymirror_id)
{
    int iRet = RETURN_ERR;
    LUN_INFO_STRU stLUNInfo;
    stLUNInfo.strID = lun_id;

    iRet = cmdOperate.CMD_showlun(stLUNInfo);
    if (RETURN_OK != iRet){
        return iRet;
    }
    isNFS = false;
    hymirror_id = stLUNInfo.strMirrorID;
    lunName = stLUNInfo.strName;
    return RETURN_OK;
}

int SraBasic::get_hymirror_by_fs_out(CCmdOperate& cmdOperate, string& fs_id, string& hymirror_id)
{
    int iRet = RETURN_ERR;
    list<HYMIRROR_LF_INFO_STRU> lstFSInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator iterStInfo;            
    
    iRet = cmdOperate.CMD_showhymirrorbyfs(fs_id, lstFSInfo); 
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "CMD_showhymirrorbyfs Failed, ret=%d", iRet);
        return iRet;
    }

    if(!lstFSInfo.empty()){
        isNFS = true;
        iterStInfo = lstFSInfo.begin();
        hymirror_id = iterStInfo->strMirrorID;
        
        return RETURN_OK;
    }

    COMMLOG(OS_LOG_ERROR, "CMD_showhymirrorbyfs finished without mirror");
    return iRet;
}

int SraBasic::get_hypermetroid_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hypermetroid)
{
    int iRet = RETURN_ERR;

    list<HYPERMETROPAIR_INFO_STRU> lstHyperMetroPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHyperMetroPairInfo;

    iRet = cmdOperate.CMD_showHyperMetroPair_all(lstHyperMetroPairInfo);
    if (RETURN_OK != iRet){
        return iRet;
    }

    for (itHyperMetroPairInfo = lstHyperMetroPairInfo.begin(); 
            itHyperMetroPairInfo != lstHyperMetroPairInfo.end();itHyperMetroPairInfo++){

        if (lun_id == itHyperMetroPairInfo->strLocalLUNID){
            hypermetroid = itHyperMetroPairInfo->strID;
            lunName = itHyperMetroPairInfo->strLocalLUNName;
            break;
        }
    }

    return RETURN_OK;
}

void SraBasic::is_Fs_by_lun(CCmdOperate& cmdOperate, string& fs_id)
{
    int iRet = RETURN_ERR;
    FS_INFO_STRU stFSInfo;
    stFSInfo.strID = fs_id;
    iRet = cmdOperate.CMD_showfs(stFSInfo);
    if (RETURN_OK != iRet){
        isNFS = false;
    }
    else{
        isNFS = true;
    }
}

int SraBasic::get_remote_hypermetroid_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& hypermetroid)
{
    int iRet = RETURN_ERR;

    list<HYPERMETROPAIR_INFO_STRU> lstHyperMetroPairInfo;
    list<HYPERMETROPAIR_INFO_STRU>::iterator itHyperMetroPairInfo;

    iRet = cmdOperate.CMD_showHyperMetroPair_all(lstHyperMetroPairInfo);
    if (RETURN_OK != iRet){
        return iRet;
    }

    for (itHyperMetroPairInfo = lstHyperMetroPairInfo.begin(); 
        itHyperMetroPairInfo != lstHyperMetroPairInfo.end();itHyperMetroPairInfo++){

        if (lun_id == itHyperMetroPairInfo->strRemoteLunID){
            hypermetroid = itHyperMetroPairInfo->strID;
            break;
        }
    }

    return RETURN_OK;
}

int SraBasic::get_consist_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& consist_id, list<HYMIRROR_INFO_STRU>& lstMirrorInfo)
{
    int iRet = RETURN_ERR;
    string hymirrorid;

    list<HYMIRROR_INFO_STRU>::iterator itHyMirrorInfo;

    iRet = get_hymirror_by_lun_out(cmdOperate,lun_id,hymirrorid);
    if (RETURN_OK != iRet){
        return iRet;
    }

    
    for (itHyMirrorInfo = lstMirrorInfo.begin(); itHyMirrorInfo != lstMirrorInfo.end(); itHyMirrorInfo++){
        if (itHyMirrorInfo->strID == hymirrorid){
            strHyID = hymirrorid;
            consist_id = itHyMirrorInfo->strGroupID;
            isNFS = false;
            break;
        }
    }

    return RETURN_OK;
}

int SraBasic::get_consisthm_by_lun_out(CCmdOperate& cmdOperate, string& lun_id, string& consist_id, list<HYPERMETROPAIR_INFO_STRU>& lstHMPairInfo)
{
    int iRet = RETURN_ERR;
    string hypermetroid;

    list<HYPERMETROPAIR_INFO_STRU>::iterator itHMPairInfo;

    iRet = get_hypermetroid_by_lun_out(cmdOperate,lun_id,hypermetroid);
    if (RETURN_OK != iRet){
        return iRet;
    }

    
    for (itHMPairInfo = lstHMPairInfo.begin(); itHMPairInfo != lstHMPairInfo.end(); itHMPairInfo++){
        if (itHMPairInfo->strID == hypermetroid){
            strHyID = hypermetroid;
            consist_id = itHMPairInfo->strGroupID;
            isNFS = false;
            break;
        }
    }

    return RETURN_OK;
}

int SraBasic::get_device_info(string device_id, string &arrayid, string &lun_id, string &lun_flag, string &pair_id)
{
    int iRet = RETURN_ERR;
    string hypermetroid;
    list<string> lst_token;
    list<string>::iterator token;

    
    OS_StrToken(device_id, "_", &lst_token);
    if ((lst_token.size()) != 4){ 
        iRet = ERROR_INVALIDATE_DEVICE_ID;
        return iRet;
    } 

    token = lst_token.begin();
    arrayid = *token;
    lun_id = *(++token);
    lun_flag = *(++token);
    pair_id = *(++token);

    return RETURN_OK;
}

void SraBasic::process()
{    
    if ((OUTBAND_REST_TYPE == g_cli_type) || (OUTBAND_TYPE == g_cli_type) || (VISOUT_TYPE == g_cli_type)){
        error_code = _outband_process();
    }
    else{
        COMMLOG(OS_LOG_ERROR, "unknown cli type[%d]", g_cli_type);
    }

    return;
}

void SraBasic::_convert_relation_type(const unsigned int& uiRelationType, string& relation)
{
    if ( LUN_RELATION_MASTER == uiRelationType){
        relation = MIRROR_RELATION_MASTER;
    }
    else if (LUN_RELATION_SLAVE == uiRelationType){
        relation = MIRROR_RELATION_SLAVE;
    }
    else{
        relation = "--";
    }
}

void SraBasic::_convert_hmrelation_type(const unsigned int& uiRelationType, string& relation)
{
    if ( LUN_RELATION_MASTER == uiRelationType){
        relation = HM_RELATION_MASTER;
    }
    else if (LUN_RELATION_SLAVE == uiRelationType){
        relation = HM_RELATION_SLAVE;
    }
    else{
        relation = "--";
    }
}

void SraBasic::_convert_pair_status(const unsigned int& uiPairStatus, string& pair_status)
{
    if ( MIRROR_SLAVE_PAIR_STATUS_CHAR_NORMAL == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_NORMAL;
    }
    else if (MIRROR_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_SYNCHRONIZING;
    }
    else if (MIRROR_SLAVE_PAIR_STATUS_CHAR_TOBERECOVERED == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_TOBERECOVERED;
    }
    else if (MIRROR_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED;
    }
    else if (MIRROR_SLAVE_PAIR_STATUS_CHAR_SPLITED == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED;
    }
    else if (MIRROR_SLAVE_PAIR_STATUS_CHAR_INVALID == uiPairStatus){
        pair_status = MIRROR_SLAVE_PAIR_STATUS_STR_INVALID;
    }
    
    else if (MIRROR_GROUP_STATUS_FAULT == uiPairStatus){
        pair_status = MIRROR_GROUP_STATUS_STR_FAULT;
    }
    
    else{
        pair_status = "--";
    }
}

void SraBasic::_convert_hmpair_status(const unsigned int& uiPairStatus, string& pair_status)
{
    if ( HM_SLAVE_PAIR_STATUS_CHAR_NORMAL == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_NORMAL;
    }
    else if (HM_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_SYNCHRONIZING;
    }
    else if (HM_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_INTERRUPTED;
    }
    else if (HM_SLAVE_PAIR_STATUS_CHAR_INVALID == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_INVALID;
    }
    else if (MIRROR_GROUP_STATUS_FAULT == uiPairStatus){
        pair_status = MIRROR_GROUP_STATUS_STR_FAULT;
    }
    else if (HM_SLAVE_PAIR_STATUS_CHAR_PAUSE == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_PAUSE;
    }
    else if (HM_SLAVE_PAIR_STATUS_CHAR_TOBESYNCHRONIZED == uiPairStatus){
        pair_status = HM_SLAVE_PAIR_STATUS_STR_TOBESYNCHRONIZED;
    }

    else{
        pair_status = "--";
    }
}

std::vector<std::string> SraBasic::split(std::string str, std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;
    int size = str.size();

    for (int i=0; i<size; i++){
        pos = str.find(pattern, i);
        if (pos < size){
            std::string s = str.substr(i, pos-i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

int SraBasic::_get_lungroup_by_lun(CCmdOperate& cmdOperate, const string& lun_id, const int type, string& lungroup_id)
{
    lungroup_id.clear();
    int iRet = RETURN_OK;

    
    COMMLOG(OS_LOG_INFO, "Create LunGroup of lun/snapshot [%s]", lun_id.c_str());
    string lungroup_name = VSSRA_LUNGROUP_PREFIX + lun_id;
    iRet = cmdOperate.CMD_CreateLunGroup(lungroup_name, lungroup_id);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "Create LunGroup [%s] error", lungroup_name.c_str());
        return iRet;
    }

    if (!lungroup_id.empty()){
        iRet = cmdOperate.CMD_addLun2LunGroup(lun_id, lungroup_id, type);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Add snapshot/lun[%s] to LunGroup [%s] error", lun_id.c_str(), lungroup_name.c_str());
            lungroup_id.clear();
            return iRet;
        }
    }
    
    return RETURN_OK;
}
    
int SraBasic::_get_host_by_initiator(CCmdOperate& cmdOperate, const string& initiator_id, const string& type, CMDHOSTINFO_STRU& hostInfo, const string& lun_id)
{
    
    INITIATOR_INFO initiator_info;
    int ret = cmdOperate.CMD_showInitiatorInfo(initiator_id, type, initiator_info);
    if (ret != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "Failed to get host by initiator[%s], please check the connection between array and host", initiator_id.c_str());
        return ret;
    }

    if (initiator_info.uiStatus != 27){
        COMMLOG(OS_LOG_WARN, "Initiator[%s] status is not online.", initiator_id.c_str());
        return RETURN_OK;
    }

    
    if (initiator_info.isFree){
        string hostName = VSSRA_HOST_PREFIX + lun_id;
        ret = cmdOperate.CMD_showhost(hostName, hostInfo);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "Failed to get host info [%s]", hostName.c_str());
            return ret;
        }

        
        if (hostInfo.strID.empty()){
            ret = cmdOperate.CMD_createHost(hostName, hostInfo);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "Failed to create host [%s].", hostName.c_str());
                return ret;
            }
        }

        
        ret = cmdOperate.CMD_addInitiatorToHost(initiator_id, type, hostInfo.strID);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "Failed to associate initiator[%s] to host[%s]...", initiator_id.c_str(), hostInfo.strID.c_str());
            return ret;
        }
    }
    else{
        if (initiator_info.strHostID.empty()){
            COMMLOG(OS_LOG_ERROR, "Internal Error:host id of initiator[%s] is empty", initiator_id.c_str());
            return RETURN_ERR;
        }
        else{
            if (!hostInfo.strID.empty() && hostInfo.strID != initiator_info.strHostID){
                COMMLOG(OS_LOG_ERROR, "Initiator[%s] is associated to other host[%s]", initiator_id.c_str(), initiator_info.strHostID.c_str());
                return RETURN_ERR;
            }
            COMMLOG(OS_LOG_INFO, "Initiator[%s] is associated to host[%s]", initiator_id.c_str(), initiator_info.strHostID.c_str());
            hostInfo.strID = initiator_info.strHostID;
            ret = cmdOperate.CMD_showhost(hostInfo);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "Failed to get host[%s] info", initiator_info.strHostID.c_str());
                return ret;
            }
        }
    }

    return RETURN_OK;
}

int SraBasic::_create_hostgroup(CCmdOperate& cmdOperate, list<CMDHOSTINFO_STRU>& hosts, const string& lun_id, list<string>& hostGroupIDs)
{
    COMMLOG(OS_LOG_INFO, "Create host group.");
    int iRet = RETURN_OK;
    list<CMDHOSTINFO_STRU>::iterator itListHosts;
    string hostGroupID;
    hostGroupIDs.clear();

    for (itListHosts = hosts.begin(); itListHosts != hosts.end(); ++itListHosts){
        COMMLOG(OS_LOG_INFO, "HostInfo:ID=%s, Name=%s, HostGroup=%s.", itListHosts->strID.c_str(), itListHosts->strName.c_str(), itListHosts->strHostGroupID.c_str());
        
        list<string> lsthostGroupID;
        iRet = cmdOperate.CMD_showHostGroupByHost(itListHosts->strID, lsthostGroupID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "get host group by host [%s] failed", itListHosts->strID.c_str());
            return iRet;
        }

        if (!lsthostGroupID.empty()){
            hostGroupIDs.push_back(lsthostGroupID.front());
        }
        else{
            hostGroupID.clear();
            string strHostGroupName = VSSRA_HOSTGROUP_PREFIX + lun_id;
            iRet = cmdOperate.CMD_CreateHostGroup(strHostGroupName, hostGroupID);
            if (RETURN_OK != iRet || hostGroupID.empty()){
                COMMLOG(OS_LOG_ERROR, "Create Host Group [%s] error", strHostGroupName.c_str());
                return iRet;
            }

            iRet = cmdOperate.CMD_addHostsToHostGroup(hostGroupID, itListHosts->strID);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Add host[%s] to HostGroup [%s] error", itListHosts->strID.c_str(), hostGroupID.c_str());
                hostGroupID.clear();
                return iRet;
            }
            hostGroupIDs.push_back(hostGroupID);
        }
    }

    return RETURN_OK;
}

int SraBasic::_create_mappingview(CCmdOperate& cmdOperate, const string& mappingName, const string& lunGrpID, const string& hostGrpID, string mapID)
{
    COMMLOG(OS_LOG_INFO, "Create MappingView of lungroup[%s] and hostgroup[%s].", lunGrpID.c_str(), hostGrpID.c_str());
    
    int iRet = RETURN_OK;
    string mvID;
    mvID.clear();

    
    if (mapID.empty()){
        string strMappingViewName = mappingName;
        iRet = cmdOperate.CMD_createMappingView(strMappingViewName, mvID);
        if (RETURN_OK != iRet || mvID.empty()){
            COMMLOG(OS_LOG_ERROR, "Create Mapping View [%s] error", strMappingViewName.c_str());
            return iRet;
        }
    }
    else{
        mvID = mapID;
    }

    if (!hostGrpID.empty()){
        iRet = cmdOperate.CMD_associateToMappingview(mvID, OBJ_HOSTGROUP, hostGrpID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Failed to associate hostgroup[%s] to mapping view[%s]", hostGrpID.c_str(), mvID.c_str());
            return iRet;
        }
    }

    if (!lunGrpID.empty()){
        iRet = cmdOperate.CMD_associateToMappingview(mvID, OBJ_LUNGROUP, lunGrpID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Failed to associate lungroup[%s] to mapping view[%s]", lunGrpID.c_str(), mvID.c_str());
            return iRet;
        }
    }

    return RETURN_OK;
}

int SraBasic::_do_mapping(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, const string& hostGrpID, const int type)
{
    int iRet = RETURN_OK;
    
    string mapID;
    mapID.clear();
    iRet = cmdOperate.CMD_showMapByHostGroup(hostGrpID, mapID);
    if (RETURN_OK != iRet){
        target_device.err_info.code = OS_IToString(iRet);
        COMMLOG(OS_LOG_ERROR, "Failed to get mapping view by hostgroup[%s].", hostGrpID.c_str());
        return iRet;
    }

    string lun_id;
    if (type == OBJ_SNAPSHOT){
        lun_id = target_device.snapshot_info.snap_id;
    }
    else if (type == OBJ_LUN){
        string arrayid, lun_flag, cgid;
        get_device_info(target_device.target_key, arrayid, lun_id, lun_flag, cgid);
    }
    else{
        COMMLOG(OS_LOG_ERROR, "Unknown type[%d].", type);
        return RETURN_ERR;
    }
    
    if (mapID.empty()){
        
        string lunGroupID;
        iRet = _get_lungroup_by_lun(cmdOperate, lun_id, type, lunGroupID);
        if (RETURN_OK != iRet || lunGroupID.empty()){
            target_device.err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "Failed to create lungroup by snapshot/LUN [%s].", lun_id.c_str());
        }

        
        iRet = _create_mappingview(cmdOperate, lun_id, lunGroupID, hostGrpID);
        if (RETURN_OK != iRet){
            target_device.err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "Failed to create mapping between lungroup[%s] and hostgroup[%s].", lunGroupID.c_str(), hostGrpID.c_str());
            return iRet;
        }
    }
    
    else{
        
        string lunGrpOfMap;
        iRet = cmdOperate.CMD_showLunGroupByMap(mapID, lunGrpOfMap);
        if (RETURN_OK != iRet){
            target_device.err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "Failed to show lungroup by mappingview [%s].", mapID.c_str());
            return iRet;
        }

        
        list<string> lunGrpOfSnap;
        lunGrpOfSnap.clear();        

        iRet = cmdOperate.CMD_showLunGroupByLUN(lun_id, lunGrpOfSnap, type);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Failed to show lungroup by snapshot/lun [%s] error.", lun_id.c_str());
            return iRet;
        }

        
        if (lunGrpOfSnap.empty()){
            
            if (lunGrpOfMap.empty()){
                COMMLOG(OS_LOG_INFO, "Create LunGroup of snapshot/lun [%s]", lun_id.c_str());
                string lungroup_name = VSSRA_LUNGROUP_PREFIX + lun_id;
                iRet = cmdOperate.CMD_CreateLunGroup(lungroup_name, lunGrpOfMap);
                if (RETURN_OK != iRet){
                    COMMLOG(OS_LOG_ERROR, "Create LunGroup [%s] error", lungroup_name.c_str());
                    return iRet;
                }
            }
            
            
            iRet = cmdOperate.CMD_addLun2LunGroup(lun_id, lunGrpOfMap, type);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Add snapshot/lun[%s] to LunGroup [%s] error", lun_id.c_str(), lunGrpOfMap.c_str());
                return iRet;
            }
        }
        else{
            
            if (lunGrpOfMap.empty()){
                string lunGroupID;

                iRet = _get_lungroup_by_lun(cmdOperate, lun_id, type, lunGroupID);
                if (RETURN_OK != iRet || lunGroupID.empty()){
                    target_device.err_info.code = OS_IToString(iRet);
                    COMMLOG(OS_LOG_ERROR, "Failed to create lungroup by snapshot/LUN [%s].", lun_id.c_str());
                }
                
                
                iRet = _create_mappingview(cmdOperate, lun_id, lunGroupID, "", mapID);
                if (RETURN_OK != iRet){
                    target_device.err_info.code = OS_IToString(iRet);
                    COMMLOG(OS_LOG_ERROR, "Failed to create mapping between lungroup[%s].", lunGroupID.c_str());
                    return iRet;
                }
            }
            else{
                
                if (find(lunGrpOfSnap.begin(), lunGrpOfSnap.end(), lunGrpOfMap) == lunGrpOfSnap.end()){
                    COMMLOG(OS_LOG_INFO, "Add snapshot/lun[%s] to LunGroup [%s]", lun_id.c_str(), lunGrpOfMap.c_str());
                    iRet = cmdOperate.CMD_addLun2LunGroup(lun_id, lunGrpOfMap, type);
                    if (RETURN_OK != iRet){
                        target_device.err_info.code = OS_IToString(iRet);
                        COMMLOG(OS_LOG_ERROR, "Failed to Add snapshot/lun[%s] to LunGroup [%s]", lun_id.c_str(), lunGrpOfMap.c_str());
                        return iRet;
                    }
                }
            }
        }
    }
       
    return RETURN_OK;
}

int SraBasic::_add_map(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, list<DAR_HOST_INFO>& lstDARHosts, list<CMDHOSTINFO_STRU>& lstHosts, string& lun_id, int obj_type)
{
    int iRet = RETURN_ERR;
    map<string, vector<CMDHOSTINFO_STRU>> mapHostInfo;
    map<string, vector<DAR_HOST_INFO>> mapInitiatorNoHost; 

    
    iRet = _select_valid_initiator(cmdOperate, target_device,  lstDARHosts, mapHostInfo, mapInitiatorNoHost);
    if(RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "select valid initiator failed");
        return RETURN_ERR;
    }

    
    vector<string> mappedInfo;
    iRet = _query_lun_mapping_info(cmdOperate, lun_id, obj_type, mapHostInfo, mappedInfo);
    if(RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "query lun %s mapping info failed.", lun_id.c_str());
        return RETURN_ERR;
    }

    
    iRet = _query_and_create_host(cmdOperate, lun_id, mappedInfo, mapHostInfo, mapInitiatorNoHost);
    if(RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "query and create host failed.");
        return RETURN_ERR;
    }

    
    lstHosts.clear();
    map<string, vector<CMDHOSTINFO_STRU>>::iterator itrMapHostInfo = mapHostInfo.begin();
    for (; itrMapHostInfo != mapHostInfo.end(); itrMapHostInfo++){
        vector<CMDHOSTINFO_STRU> &vMapInfo =  itrMapHostInfo->second;
        for (size_t id = 0; id < vMapInfo.size(); id++){
            bool isAddHost = true; 
            list<CMDHOSTINFO_STRU>::iterator itrLstHosts = lstHosts.begin();
            for (; itrLstHosts != lstHosts.end(); itrLstHosts++){
                 if(itrLstHosts->strID == vMapInfo[id].strID){
                     isAddHost = false;
                     break;
                 }
            }

            if(isAddHost){
                lstHosts.push_back(vMapInfo[id]);
            }         
        }
    }

    if (lstHosts.empty()){
        COMMLOG(OS_LOG_INFO, "the lstHosts is empty, the lun[%s] is mapped to  host.", lun_id.c_str());
        return RETURN_OK;
    }

    if (!g_bFusionStorage){
        
        string hostGroupID;
        iRet = _create_host_group(cmdOperate, lun_id, hostGroupID, lstHosts);
        if (RETURN_OK != iRet || hostGroupID.empty()){
            COMMLOG(OS_LOG_ERROR, "Create Host Group for lun id [%s] error", lun_id.c_str());
            return iRet;
        }



        
        string lunGroupID;
        iRet = _create_lun_group(cmdOperate, lun_id, lunGroupID, obj_type);
        if (RETURN_OK != iRet || lunGroupID.empty()){
            COMMLOG(OS_LOG_ERROR, "Create LunGroup for lun id [%s] error", lun_id.c_str());
            return iRet;
        }



        
        iRet = _create_mapping(cmdOperate, lun_id, lunGroupID, hostGroupID);;
        if (RETURN_OK != iRet){
            target_device.err_info.code = OS_IToString(iRet);
            COMMLOG(OS_LOG_ERROR, "Failed to create mapping between lungroup[%s] and hostgroup[%s].", lunGroupID.c_str(), hostGroupID.c_str());
            return iRet;
        }
    }
    else{
        for (list<CMDHOSTINFO_STRU>::iterator it = lstHosts.begin(); it != lstHosts.end(); it++){
            LUN_INFO_STRU rstLUN;
            rstLUN.strID = lun_id;
            int ret = cmdOperate.CMD_showlun(rstLUN);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "query lun[%s] fail", lun_id.c_str());
                return ret;
            }
            list<string> lunnames;
            lunnames.push_back(rstLUN.strName);
            ret = cmdOperate.CMD_maplun2host(it->strName, lunnames);
            if (ret != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "map lun[%s] to host[%s] fail", lun_id.c_str(),it->strName.c_str());
                return ret;
            }
        }
    }
    return RETURN_OK;
}

int SraBasic::_select_valid_initiator(CCmdOperate& cmdOperate, TargetDeviceInfo& target_device, list<DAR_HOST_INFO>& lstDARHosts, map<string, vector<CMDHOSTINFO_STRU>>& mapHostInfo, map<string, vector<DAR_HOST_INFO>>& mapInitiatorNoHost)
{
    int iRet = RETURN_ERR;
    mapHostInfo.clear();
    mapInitiatorNoHost.clear();
    vector<string> lunAceessGroup = split(target_device.cg_accessgroups, DAR_HOST_SEPERATOR);
    
    vector<string>::iterator itrLunAceessGroup = lunAceessGroup.begin();
    for (; itrLunAceessGroup != lunAceessGroup.end(); itrLunAceessGroup++ ){
        if (itrLunAceessGroup->empty()){
            continue;
        }

        bool isValidInitiator = false;
        list<DAR_HOST_INFO>::iterator itDARHostInfo =  lstDARHosts.begin();
        for (; itDARHostInfo != lstDARHosts.end(); itDARHostInfo++){
            if (itDARHostInfo->strType == "FC" || itDARHostInfo->strType == "iSCSI"){
                if (*itrLunAceessGroup == itDARHostInfo->strGroup){                    
                    
                    INITIATOR_INFO initiator_info;
                    iRet = cmdOperate.CMD_showInitiatorInfo(itDARHostInfo->strID, itDARHostInfo->strType, initiator_info);
                    if (iRet != RETURN_OK){
                        COMMLOG(OS_LOG_WARN, "Failed to get host by initiator[%s], please check the connection between array and host", itDARHostInfo->strID.c_str());
                        continue;;
                    }

                    
                    if (!g_bFusionStorage){
                        if (initiator_info.uiStatus != 27){
                            COMMLOG(OS_LOG_WARN, "Initiator[%s] status is not online.", itDARHostInfo->strID.c_str());
                            continue;
                        }
                    }

                    isValidInitiator = true;
                    
                    if (initiator_info.isFree){
                        mapInitiatorNoHost[itDARHostInfo->strGroup].push_back(*itDARHostInfo);                        
                        COMMLOG(OS_LOG_INFO, "Initiator[%s] is valid, but not in host.", itDARHostInfo->strID.c_str());
                        continue;
                    }
                    else{
                        if (initiator_info.strHostID.empty()){
                            COMMLOG(OS_LOG_ERROR, "Internal Error:host id of initiator[%s] is empty", itDARHostInfo->strID.c_str());
                            continue;
                        }
                        else{    
                            
                            CMDHOSTINFO_STRU hostInfo;
                            if (g_bFusionStorage){
                                hostInfo.strName = initiator_info.strHostName;
                                hostInfo.strID = initiator_info.strHostID;
                            }
                            else{
                                hostInfo.strID = initiator_info.strHostID;
                                iRet = cmdOperate.CMD_showhost(hostInfo);
                            }
                            

                            if (iRet != RETURN_OK){
                                COMMLOG(OS_LOG_ERROR, "Failed to get host[%s] info", initiator_info.strHostID.c_str());
                                return iRet;
                            }

                            
                            if (mapHostInfo.find(itDARHostInfo->strGroup) != mapHostInfo.end()){
                                vector<CMDHOSTINFO_STRU> &vHostInfo = mapHostInfo[itDARHostInfo->strGroup];

                                bool isStoreHost = false;
                                for (size_t vHostInfoId = 0; vHostInfoId < vHostInfo.size(); vHostInfoId++){
                                    if (vHostInfo[vHostInfoId].strID == initiator_info.strHostID){
                                        isStoreHost = true;
                                        break;
                                    }
                                }

                                if (!isStoreHost){
                                    mapHostInfo[itDARHostInfo->strGroup].push_back(hostInfo) ;
                                }
                            }
                            else{
                                mapHostInfo[itDARHostInfo->strGroup].push_back(hostInfo) ;
                            }                            
                        }                    
                    }
                }
            }
        }

        
        if (!isValidInitiator){
            COMMLOG(OS_LOG_ERROR, "no valid initiator for accessGroup[%s] ", (*itrLunAceessGroup).c_str());
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

int SraBasic::_query_lun_mapping_info(CCmdOperate& cmdOperate, const string& lun_id, const int obj_type, map<string, vector<CMDHOSTINFO_STRU>> &mapHostInfo, vector<string> &mappedInfo)
{    
    if (mapHostInfo.empty()){
        COMMLOG(OS_LOG_INFO, "no valid host exsit for initator,  the lun id is [%s] .", lun_id.c_str());
        return RETURN_OK;
    }
    
    int iRet = RETURN_ERR;
    list<string> lstAllHostID;

    if (!g_bFusionStorage){
        
        list<string> strLunGroupID;
        iRet = cmdOperate.CMD_showLunGroupByLUN(lun_id, strLunGroupID, obj_type);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Failed to get lun group id by lun id [%s].", lun_id.c_str());
            return iRet;
        }

        if (strLunGroupID.empty()){
            COMMLOG(OS_LOG_INFO, "the lun id [%s] is not in lun group.", lun_id.c_str());
        }
 
        list<string>::iterator itrStrLunGroupID = strLunGroupID.begin();
        for(; itrStrLunGroupID != strLunGroupID.end(); itrStrLunGroupID++){
            
            list<string> lstMapID;
            iRet = cmdOperate.CMD_showMapbyLunGroup(*itrStrLunGroupID, lstMapID);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Failed to get map id by lun group id [%s].", itrStrLunGroupID->c_str());
                return iRet;
            }

            if (lstMapID.empty()){
                COMMLOG(OS_LOG_INFO, "the lun group id [%s] is not mapping", itrStrLunGroupID->c_str());
                continue;
            }

            
            list<string>::iterator itrlstMapID = lstMapID.begin();        
            for(; itrlstMapID != lstMapID.end(); itrlstMapID++){
                string hostGroupID;
                iRet = cmdOperate.CMD_showHostGroupByMap(*itrlstMapID, hostGroupID);

                if (RETURN_OK != iRet){
                    COMMLOG(OS_LOG_ERROR, "Failed to get host group id by map id [%s].", itrlstMapID->c_str());
                    return iRet;
                }

                if (hostGroupID.empty()){
                    COMMLOG(OS_LOG_INFO, "the host group id is empty by map id [%s]", itrlstMapID->c_str());
                    continue;
                }

                
                list<string> lstHostID;
                iRet = cmdOperate.CMD_showHostbyHostGroup(hostGroupID, lstHostID);

                if (RETURN_OK != iRet){
                    COMMLOG(OS_LOG_ERROR, "Failed to get host id by host group id [%s].", hostGroupID.c_str());
                    return iRet;
                }

                if (lstHostID.empty()){
                    COMMLOG(OS_LOG_INFO, "the host id is empty by host group id [%s]", hostGroupID.c_str());
                    continue;
                }

                lstAllHostID.insert(lstAllHostID.end(), lstHostID.begin(), lstHostID.end());
            }
        }

        
        list<string> lstHostIDByLun;
        iRet = cmdOperate.CMD_showHostByLUN(lun_id, lstHostIDByLun, obj_type);

        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Failed to get host id by lun id [%s].", lun_id.c_str());
        }

        if (!lstHostIDByLun.empty()){
            lstAllHostID.insert(lstAllHostID.end(), lstHostIDByLun.begin(), lstHostIDByLun.end());
        }

        if (lstAllHostID.empty()){
            COMMLOG(OS_LOG_INFO, "the lun id [%s] is not mapped.", lun_id.c_str());
            return RETURN_OK;
        }
    }
    else{
        
        list<MAP_INFO_STRU> rlstMapInfo;
        int ret = cmdOperate.CMD_showmap(lun_id, rlstMapInfo);
        if (ret != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "query lun[%s] maphost fail.", lun_id.c_str());
            return ret;
        }
        else{
            lstAllHostID.clear();
            for (list<MAP_INFO_STRU>::iterator it = rlstMapInfo.begin(); it != rlstMapInfo.end(); it++){
                lstAllHostID.push_back(it->strHostID);
            }
        }
    }

    
    mappedInfo.clear();
    map<string, vector<CMDHOSTINFO_STRU>>::iterator itrMapHostInfo = mapHostInfo.begin();
    for (; itrMapHostInfo != mapHostInfo.end(); itrMapHostInfo++){
        bool isMapping = false;
        string hostID = "";
        vector<CMDHOSTINFO_STRU> &vCmdHostInfo = itrMapHostInfo -> second;

        for (size_t cmdHostId = 0; cmdHostId < vCmdHostInfo.size(); cmdHostId++){
            list<string>::iterator itrlstAllHostID = lstAllHostID.begin();
            for(; itrlstAllHostID != lstAllHostID.end(); itrlstAllHostID++){                
                if (vCmdHostInfo[cmdHostId].strID == *itrlstAllHostID){
                    hostID = *itrlstAllHostID;
                    isMapping = true;
                    break;
                }
            }

            if (isMapping){
                break;
            }
        }

        if (isMapping){
            COMMLOG(OS_LOG_INFO, "the lun  [%s] is mapped by host[%s] ", lun_id.c_str(), hostID.c_str());
            mappedInfo.push_back(itrMapHostInfo -> first);
        }
    }

    if(mappedInfo.empty()){
        COMMLOG(OS_LOG_INFO, "the lun  [%s] is not mapped", lun_id.c_str());
    }

    return RETURN_OK;
}

int SraBasic::_query_and_create_host(CCmdOperate& cmdOperate, const string& lun_id, vector<string>& mappedInfo, map<string, vector<CMDHOSTINFO_STRU>>& mapHostInfo, map<string, vector<DAR_HOST_INFO>>& mapInitiatorNoHost)
{
    int iRet = RETURN_ERR;

    
    if (!mappedInfo.empty()){
        for (size_t id = 0; id < mappedInfo.size(); id++){
            string strAccessGroup = mappedInfo[id];
            map<string, vector<CMDHOSTINFO_STRU>>::iterator itrMapHostInfo;
            itrMapHostInfo = mapHostInfo.find(strAccessGroup);
            if (itrMapHostInfo != mapHostInfo.end()){
                mapHostInfo.erase(itrMapHostInfo);
            }

            map<string, vector<DAR_HOST_INFO>>::iterator itrMapInitiatorNoHost;
            itrMapInitiatorNoHost = mapInitiatorNoHost.find(strAccessGroup);
            if (itrMapInitiatorNoHost != mapInitiatorNoHost.end()){
                mapInitiatorNoHost.erase(itrMapInitiatorNoHost);
            }
        }
    }

    
    if (!mapInitiatorNoHost.empty()){
        map<string, vector<DAR_HOST_INFO>>::iterator itrMapInitiatorNoHost = mapInitiatorNoHost.begin();
        for (; itrMapInitiatorNoHost != mapInitiatorNoHost.end();itrMapInitiatorNoHost++){
            string strAccessGroup = itrMapInitiatorNoHost->first;
            vector<DAR_HOST_INFO> &vInitiatorNoHost = itrMapInitiatorNoHost->second;

            if(vInitiatorNoHost.empty()){
                continue;
            }

            map<string, vector<CMDHOSTINFO_STRU>>::iterator itrMapHostInfo;
            itrMapHostInfo = mapHostInfo.find(strAccessGroup);

            if (itrMapHostInfo != mapHostInfo.end()){
                if(!itrMapHostInfo->second.empty()){
                    string hostId = itrMapHostInfo->second.front().strID;
                    string hostName = itrMapHostInfo->second.front().strName;
                    
                    for (size_t id = 0; id < vInitiatorNoHost.size(); id++){                
                        if (g_bFusionStorage){
                            iRet = cmdOperate.CMD_addInitiatorToHost(vInitiatorNoHost[id].strID, vInitiatorNoHost[id].strType, hostName);
                        }
                        else{
                            iRet = cmdOperate.CMD_addInitiatorToHost(vInitiatorNoHost[id].strID, vInitiatorNoHost[id].strType, hostId);
                        }
                        if (iRet != RETURN_OK){
                            COMMLOG(OS_LOG_ERROR, "Failed to associate initiator[%s] to host[%s]...", vInitiatorNoHost[id].strID.c_str(), hostId.c_str());
                            return iRet;
                        }
                    }

                    continue;
                }
            }

            
            string hostName = VSSRA_HOST_PREFIX + lun_id + "_" + strAccessGroup;
            CMDHOSTINFO_STRU hostInfo;

            iRet = cmdOperate.CMD_showhost(hostName, hostInfo);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "Failed to get host info [%s]", hostName.c_str());
                return iRet;
            }

            
            if (hostInfo.strID.empty()){
                iRet = cmdOperate.CMD_createHost(hostName, hostInfo);
                if (iRet != RETURN_OK){
                    COMMLOG(OS_LOG_ERROR, "Failed to create host [%s].", hostName.c_str());
                    return iRet;
                }

                if ( hostInfo.strID.empty()){
                    COMMLOG(OS_LOG_ERROR, "create host [%s] success, but host id is empty.", hostName.c_str());
                    return RETURN_ERR;
                }
            }

            
            for (size_t id = 0; id < vInitiatorNoHost.size(); id++){    
                if (g_bFusionStorage){
                    iRet = cmdOperate.CMD_addInitiatorToHost(vInitiatorNoHost[id].strID, vInitiatorNoHost[id].strType, hostInfo.strName);
                }
                else{
                    iRet = cmdOperate.CMD_addInitiatorToHost(vInitiatorNoHost[id].strID, vInitiatorNoHost[id].strType, hostInfo.strID);
                }
                if (iRet != RETURN_OK){
                    COMMLOG(OS_LOG_ERROR, "Failed to associate initiator[%s] to host[%s]...", vInitiatorNoHost[id].strID.c_str(),  hostInfo.strID.c_str());
                    return iRet;
                }
            }

            mapHostInfo[strAccessGroup].push_back(hostInfo);
        }
    }

    return RETURN_OK;
}

int SraBasic::_create_lun_group(CCmdOperate& cmdOperate, const string& lun_id, string& lunGroupID, int obj_type)
{
    int iRet = RETURN_OK;
    int suffixId = 0;

    while(true){
        lunGroupID.clear();
        suffixId++;
        stringstream sStream;
        sStream << suffixId;
        string strSuffixId;
        sStream >> strSuffixId;
        
        string lungroup_name = VSSRA_LUNGROUP_PREFIX + lun_id + "_" + strSuffixId;

        
        iRet = cmdOperate.CMD_showLUNGroupByName(lungroup_name, lunGroupID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "query LunGroup by Name [%s] error", lungroup_name.c_str());
            break;
        }

        if (lunGroupID.empty()){
            
            iRet = cmdOperate.CMD_CreateLunGroup(lungroup_name, lunGroupID);
            if (RETURN_OK != iRet || lunGroupID.empty()){
                COMMLOG(OS_LOG_ERROR, "Create LunGroup [%s] error", lungroup_name.c_str());
                break;
            }

            
            iRet = cmdOperate.CMD_addLun2LunGroup(lun_id, lunGroupID, obj_type);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Add snapshot/lun[%s] to LunGroup [%s] error", lun_id.c_str(), lunGroupID.c_str());
                break;;
            }

            break;
        }

        
        list<LUN_INFO_STRU> lstLunIDs;
        iRet = cmdOperate.CMD_showLunByLunGroup(lunGroupID, lstLunIDs);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "get lun from lunGroup[%s] failed", lunGroupID.c_str());
            break;
        }

        
        list<HYIMAGE_INFO_STRU> lstSnapShot;
        iRet = cmdOperate.CMD_showSnapShotByLUNGroup(lunGroupID, lstSnapShot);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "get snapshot from lunGroup[%s] failed", lunGroupID.c_str());
            break;
        }

        
        if (lstLunIDs.empty() && lstSnapShot.empty()){
            continue;
        }

        
        bool isBreak = false;

        if (obj_type == OBJ_SNAPSHOT){
            if (0 == lstLunIDs.size() && 1 == lstSnapShot.size() && lun_id == lstSnapShot.front().strID){
                isBreak = true;
            }
        }

        if (obj_type == OBJ_LUN){
            if (1 == lstLunIDs.size() && 0 == lstSnapShot.size() && lun_id == lstLunIDs.front().strID){
                isBreak = true;
            }
        }

        if (isBreak){
            break;
        }
    }

    return iRet;
}

int SraBasic::_create_host_group(CCmdOperate& cmdOperate, const string& lun_id, string& hostGroupID, list<CMDHOSTINFO_STRU> &lstHosts)
{
    int iRet = RETURN_OK;
    int suffixId = 0;

    while (true){
        hostGroupID.clear();
        suffixId++;
        stringstream sStream;
        sStream << suffixId;
        string strSuffixId;
        sStream >> strSuffixId;

        string strHostGroupName = VSSRA_HOSTGROUP_PREFIX + lun_id + "_" + strSuffixId;

        
        iRet = cmdOperate.CMD_showHostGroupByName(strHostGroupName, hostGroupID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "Get Host Group by name [%s] error", strHostGroupName.c_str());
            break;
        }

        if (hostGroupID.empty()){
            
            iRet = cmdOperate.CMD_CreateHostGroup(strHostGroupName, hostGroupID);
            if (RETURN_OK != iRet || hostGroupID.empty()){
                COMMLOG(OS_LOG_ERROR, "Create Host Group [%s] error", strHostGroupName.c_str());
                break;
            }

            
            list<CMDHOSTINFO_STRU>::iterator itrLstHosts = lstHosts.begin();
            for (; itrLstHosts != lstHosts.end(); itrLstHosts++){
                iRet = cmdOperate.CMD_addHostsToHostGroup(hostGroupID, itrLstHosts->strID);
                if (RETURN_OK != iRet){
                    COMMLOG(OS_LOG_ERROR, "Add host[%s] to HostGroup [%s] error", itrLstHosts->strID.c_str(), hostGroupID.c_str());
                    return RETURN_ERR;
                }
            }

            break;
        }

        
        list<string> lstHostID;
        iRet = cmdOperate.CMD_showHostbyHostGroup(hostGroupID, lstHostID);
        if (RETURN_OK != iRet){
            COMMLOG(OS_LOG_ERROR, "get host by Host Group [%s] error", hostGroupID.c_str());
            break;
        }

        
        bool isBreak = true;

        if (lstHostID.size() == lstHosts.size()){
            list<CMDHOSTINFO_STRU>::iterator itrLstHosts = lstHosts.begin();
            for (; itrLstHosts != lstHosts.end(); itrLstHosts++){
                if (find(lstHostID.begin(), lstHostID.end(), itrLstHosts->strID) == lstHostID.end()){
                    isBreak = false;
                    break;
                }
            }
        }
        else{
            isBreak = false;
        }

        if (isBreak){        
            break;
        }
    }

    return iRet;
}

int SraBasic::_create_mapping(CCmdOperate& cmdOperate, const string& lun_id, const string& lunGrpID, const string& hostGrpID)
{
    int iRet = RETURN_OK;
    bool isMapped = false;

    
    list<string> lstMapID;
    iRet = cmdOperate.CMD_showMapbyLunGroup(lunGrpID, lstMapID);
    if (RETURN_OK != iRet){
        COMMLOG(OS_LOG_ERROR, "Failed to get map id by lun group id [%s].", lunGrpID.c_str());
        return iRet;
    }

    if (!lstMapID.empty()){
        
        list<string>::iterator itrlstMapID = lstMapID.begin();        
        for(; itrlstMapID != lstMapID.end(); itrlstMapID++){
            string hostGroupID;
            iRet = cmdOperate.CMD_showHostGroupByMap(*itrlstMapID, hostGroupID);

            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Failed to get host group id by map id [%s].", itrlstMapID->c_str());
                return iRet;
            }

            if (hostGroupID.empty()){
                continue;
            }

            if (hostGrpID == hostGroupID){
                isMapped = true;
            }
        }
    }

    if (!isMapped){
        int suffixId = 0;
        while (true){
            suffixId++;
            stringstream sStream;
            sStream << suffixId;
            string strSuffixId;
            sStream >> strSuffixId;

            string strMappingViewName = VSSRA_MAPPINGVIEW_PREFIX + lun_id + "_" + strSuffixId;

            string mapID;
            iRet = cmdOperate.CMD_showMappingByName(strMappingViewName, mapID);
            if (RETURN_OK != iRet){
                COMMLOG(OS_LOG_ERROR, "Get Mapping View by Name[%s] error", strMappingViewName.c_str());
                return iRet;
            }

            if (mapID.empty()){
                iRet =_create_mappingview(cmdOperate, strMappingViewName, lunGrpID,  hostGrpID, mapID);
                if (RETURN_OK != iRet){
                    COMMLOG(OS_LOG_ERROR, "Create Mapping View  error");
                    return iRet;
                }

                break;
            }
        }
    }

    return iRet;
}

int SraBasic::execSparseHymirror(CCmdOperate &cmdOperate, string& lunId, string& hymirrorId, RESOURCE_ACCESS_E accessState)
{
    int iRet = RETURN_OK;

    if (lunId.empty()){
        COMMLOG(OS_LOG_ERROR, "execSparseHymirror failed, the lunId is empty.");
        return RETURN_ERR;
    }

    
    string allHymirrorId;
    iRet = get_hymirror_by_lun_out(cmdOperate, lunId, allHymirrorId);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get all hymirror id by lun id[%s] failed, error code id [%d]", lunId.c_str(), iRet);
        return RETURN_ERR;
    }

    if (allHymirrorId.empty()){
        COMMLOG(OS_LOG_WARN, "get all hymirror id by lun id[%s] failed, allHymirrorId is empty!", lunId.c_str(), iRet);
        return RETURN_ERR;
    }

    
    string pattern(":");
    std::vector<string> vHymirrorIDs = split(allHymirrorId, pattern);

    
    for(size_t i = 0; i < vHymirrorIDs.size(); i++){
        
        if (hymirrorId == vHymirrorIDs[i]){
            continue;
        }

        iRet = changeSparseHymirrorAccessState(cmdOperate, vHymirrorIDs[i], accessState);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "exec changeSparseHymirrorAccessState for SparseHymirror[%s] failed, error code id [%d]", vHymirrorIDs[i].c_str(), iRet);
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}


int SraBasic::changeSparseHymirrorAccessState(CCmdOperate &cmdOperate, string& hymirrorId, RESOURCE_ACCESS_E accessState)
{
    int iRet = RETURN_OK;

    if (hymirrorId.empty()){
        COMMLOG(OS_LOG_ERROR, "changeSparseHymirrorAccessState failed, the hymirrorId is empty.");
        return RETURN_ERR;
    }

    
    list<HYMIRROR_LF_INFO_STRU> lstHyMirrorLUNInfo;
    list<HYMIRROR_LF_INFO_STRU>::iterator itHyMirrorLUNInfo;
    iRet = cmdOperate.CMD_showhymirrorlun_all(hymirrorId, lstHyMirrorLUNInfo);

    
    for (itHyMirrorLUNInfo = lstHyMirrorLUNInfo.begin(); itHyMirrorLUNInfo != lstHyMirrorLUNInfo.end(); itHyMirrorLUNInfo++){
        if (accessState != itHyMirrorLUNInfo->uiSecResAccess){
            iRet = cmdOperate.CMD_changeSlaveLunRw(hymirrorId, accessState);
            if (iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "exec CMD_changeSlaveLunRw for SparseHymirror[%s] failed, error code id [%d]", hymirrorId.c_str(), iRet);
                return RETURN_ERR;
            }
            break;
        }
    }

    return RETURN_OK;
}

int SraBasic::execSparseConsisgr(CCmdOperate &cmdOperate, string& cgId, RESOURCE_ACCESS_E accessState)
{
    int iRet = RETURN_OK;
    std::vector<string> vSparseHymirrorIds;
    std::vector<string> vSparseConsisGrpIds;

    if (cgId.empty()){
        COMMLOG(OS_LOG_INFO, "execSparseConsisgr failed, the cgId is empty");
        return RETURN_ERR;
    }

    
    list<HYMIRROR_INFO_STRU> lstHyMirrorInfo;

    iRet = cmdOperate.CMD_showconsistgrmember(cgId, lstHyMirrorInfo);
    if(iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get the Hymirror by cgid [%s]  failed, the error code is [%d]", cgId.c_str(), iRet);
        return iRet;
    }

    
    list<HYMIRROR_INFO_STRU>::iterator itHyMirrorInfo;
    for (itHyMirrorInfo = lstHyMirrorInfo.begin(); itHyMirrorInfo != lstHyMirrorInfo.end(); itHyMirrorInfo++){
        string lunId = itHyMirrorInfo->strPriLUNID;

        std::vector<string> vHymirrorIDs;
        iRet = querySparseHymirror(cmdOperate, lunId, vHymirrorIDs);

        if(iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "get Sparese Hymirror by lun id[%s]  failed, the error code is [%d]", lunId.c_str(), iRet);
            return iRet;
        }

        for(size_t i = 0; i < vHymirrorIDs.size(); i++){
            if (vHymirrorIDs[i] == itHyMirrorInfo->strID){
                continue;
            }

            HYMIRROR_INFO_STRU rstHyMirrorInfo;
            rstHyMirrorInfo.strID = vHymirrorIDs[i];

            iRet = cmdOperate.CMD_showhymirrorinfo(rstHyMirrorInfo);
            if(iRet != RETURN_OK){
                COMMLOG(OS_LOG_ERROR, "get Sparese Hymirror info by mirror id[%s]  failed, the error code is [%d]", rstHyMirrorInfo.strID.c_str(), iRet);
                return iRet;
            }

            if (!rstHyMirrorInfo.bIsBelongGroup){
                COMMLOG(OS_LOG_WARN, "the Sparese Hymirror[%s] is not in consist group", rstHyMirrorInfo.strID.c_str());

                if (rstHyMirrorInfo.uiSecResAccess == accessState){
                    COMMLOG(OS_LOG_WARN, "the Sparese Hymirror[%s] SECRESACCESS is read-write or read-only", rstHyMirrorInfo.strID.c_str());
                }
                else{
                    vSparseHymirrorIds.push_back(rstHyMirrorInfo.strID);
                }
            }
            else{
                if (find(vSparseConsisGrpIds.begin(), vSparseConsisGrpIds.end(), rstHyMirrorInfo.strGroupID) == vSparseConsisGrpIds.end()){
                    COMMLOG(OS_LOG_INFO, "add the sparse consistgr id [%s]", rstHyMirrorInfo.strGroupID.c_str());
                    vSparseConsisGrpIds.push_back(rstHyMirrorInfo.strGroupID);
                }
            }
        }
    }

    
    for (size_t i = 0; i < vSparseHymirrorIds.size(); i++){
        iRet = changeSparseHymirrorAccessState(cmdOperate, vSparseHymirrorIds[i], accessState);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "changeSparseHymirrorAccessState by hymirror id[%s] failed, error code id [%d]", vSparseHymirrorIds[i].c_str(), iRet);
            return RETURN_ERR;
        }
    }

    
    for (size_t i = 0; i < vSparseConsisGrpIds.size(); i++){
        COMMLOG(OS_LOG_INFO, "changeSparseConsisgrAccessState, the consistgr id is %s",vSparseConsisGrpIds[i].c_str());
        iRet = changeSparseConsisgrAccessState(cmdOperate, vSparseConsisGrpIds[i], accessState);
        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "changeSparseConsisgrAccessState by consist id[%s] failed, error code id [%d]", vSparseConsisGrpIds[i].c_str(), iRet);
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

int SraBasic::changeSparseConsisgrAccessState(CCmdOperate &cmdOperate, string& cgId, RESOURCE_ACCESS_E accessState)
{
    int iRet = RETURN_OK;

    if (cgId.empty()){
        COMMLOG(OS_LOG_ERROR, "changeSparseConsisgrAccessState failed, the cgId is empty");
        return RETURN_ERR;
    }
    
    GROUP_INFO_STRU stGroupInfo;
    stGroupInfo.strID = cgId;

    iRet = cmdOperate.CMD_showconsistgrinfo(stGroupInfo);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get the consisGrp info  by consist id[%s] failed, error code id [%d]", stGroupInfo.strID.c_str(), iRet);
        return RETURN_ERR;
    }

    int uiResAccess = stGroupInfo.uiSecResAccess;
    if (accessState != uiResAccess){
        iRet = cmdOperate.CMD_changeCGSlaveLunRw(stGroupInfo.strID , accessState);

        if (iRet != RETURN_OK){
            COMMLOG(OS_LOG_ERROR, "get the consisGrp Read-Write  by consist id[%s] failed, error code id [%d]", stGroupInfo.strID.c_str(), iRet);
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

int SraBasic::querySparseHymirror(CCmdOperate &cmdOperate, string& lunId, std::vector<string> &vHymirrorIDs)
{
    int iRet = RETURN_OK;

    if (lunId.empty()){
        COMMLOG(OS_LOG_ERROR, "querySparseHymirror failed, the lunId is empty");
        return RETURN_ERR;
    }

    
    string allHymirrorId;
    iRet = get_hymirror_by_lun_out(cmdOperate, lunId, allHymirrorId);

    if (iRet != RETURN_OK){
        COMMLOG(OS_LOG_ERROR, "get all hymirror id by lun id[%s] failed, error code id [%d]", lunId.c_str(), iRet);
        return RETURN_ERR;
    }

    if (allHymirrorId.empty()){
        COMMLOG(OS_LOG_WARN, "get all hymirror id by lun id[%s] failed, allHymirrorId is empty!", lunId.c_str(), iRet);
        return RETURN_ERR;
    }

    
    vHymirrorIDs.clear();
    string pattern(":");
    vHymirrorIDs = split(allHymirrorId, pattern);

    return RETURN_OK;
}
