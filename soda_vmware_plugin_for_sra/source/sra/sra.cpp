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

#include "sra.h"
#include "query_info.h"
#include "query_strings.h"
#include "query_capacity.h"
#include "query_connection.h"
#include "query_error.h"
#include "discover_arrays.h"
#include "discover_devices.h"
#include "sync_once.h"
#include "prepare_failover.h"
#include "failover.h"
#include "test_failover_start.h"
#include "query_syncstatus.h"
#include "test_failover_stop.h"
#include "reverse_replication.h"
#include "restore.h"
#include "prepare_reverse.h"

#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#include <Psapi.h> 
#endif

map<string, SRA_ENTRY> g_mapFunc;

map<int, pair<string, string> > g_map_err_en;
map<int, pair<string, string> > g_map_err_cn;
map<int, pair<string, string> > g_map_warn_en;
map<int, pair<string, string> > g_map_warn_cn;

bool g_bIsThreadExit = false;

bool g_bnfs = false;
bool g_bstretch = false;
bool g_bFusionStorage = false;
bool g_testFusionStorageStretch = true;
std::ostream& operator<<(std::ostream& out, HYIMAGE_INFO_STRU& item)
{
    out << "strID: " << item.strID << std::endl;
    out << "strName: " << item.strName << std::endl;
    out << "uiSnapType: " << item.uiSnapType << std::endl;
    out << "uiStatus: " << item.uiStatus << std::endl;
    out << "uiState: " << item.uiState << std::endl;
    out << "uiRollBackProgress: " << item.uiRollBackProgress << std::endl;
    out << "strTimeStamp: " << item.strTimeStamp << std::endl;
    out << "strWWN: " << item.strWWN << std::endl;
    out << "vstoreID: " << item.vstoreID << std::endl;
    return out;
}

void sra_init_reg_function()
{
    register_fun("queryInfo", (SRA_ENTRY)query_info);
    register_fun("queryStrings", (SRA_ENTRY)query_strings);
    register_fun("queryCapabilities", (SRA_ENTRY)query_capacity);
    register_fun("queryErrorDefinitions", (SRA_ENTRY)query_error);
    register_fun("queryConnectionParameters", (SRA_ENTRY)query_connection);
    register_fun("discoverArrays", (SRA_ENTRY)discover_arrays);
    register_fun("discoverDevices", (SRA_ENTRY)discover_devices);
    register_fun("syncOnce", (SRA_ENTRY)sync_once);
    register_fun("prepareFailover", (SRA_ENTRY)prepare_failover);
    register_fun("failover", (SRA_ENTRY)failover);
    register_fun("testFailoverStart", (SRA_ENTRY)test_failover_start);
    register_fun("querySyncStatus", (SRA_ENTRY)query_syncstatus);
    register_fun("testFailoverStop", (SRA_ENTRY)test_failover_stop);
    register_fun("reverseReplication", (SRA_ENTRY)reverse_replications);
    register_fun("restoreReplication", (SRA_ENTRY)restore);
    register_fun("prepareReverseReplication", (SRA_ENTRY)prepare_reverse);
    return;
}


int make_matulDir(char *pstrPath)
{
    char *strPath = new char[strlen(pstrPath) +2];
    
    if (NULL == strPath){
        return -1;
    }
    
    memset_s(strPath, strlen(pstrPath) +2, 0, strlen(pstrPath) +2);
    memmove_s(strPath, strlen(pstrPath) +2, pstrPath, strlen(pstrPath)+1);

    char pathSeparator;

#ifdef WIN32
    pathSeparator = '\\';
#else
    pathSeparator = '/';
#endif

    int i = (int)strlen(strPath)-1;
    while(strPath[i] == pathSeparator && i >= 0){
        strPath[i] = '\0';
        i--;
    }
    if(OS_DirExists(strPath) == ISSP_RTN_OK || i <= 0){
        delete [] strPath;
        return 0;
    }
    if(OS_DirCreate(strPath) == ISSP_RTN_OK || i <= 0){
        delete [] strPath;
        return 0;
    }
    while(strPath[i] != pathSeparator && i >= 0){
        strPath[i] = '\0';
        i--;
    }
    (void)make_matulDir(strPath);
    
    if(OS_DirCreate(pstrPath) == 0){
        delete [] strPath;
        return 0;
    }
    if(strPath != NULL){
        delete [] strPath;
    }
    return -1;
}

std::string getProcessFileFullPath()
{
#if defined(WIN32)
    char modName[PATH_MAX] = {0};

    DWORD id = ::GetCurrentProcessId();
    HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, id);
    if (NULL != hProc){
        HMODULE hMod = NULL;
        DWORD cbNeeded;
        if (EnumProcessModules(hProc, &hMod, sizeof(hMod), &cbNeeded)){
            GetModuleFileNameExA(hProc, hMod, modName, sizeof(char) * PATH_MAX);
        }

        CloseHandle(hProc);
    }

    return modName;
#else
#endif
}

bool CISMConfig::open(const char* pFileName, const char* pSuffix, bool bCompletePath)
{
    std::string strFile;

    if(false == bCompletePath){
        const std::string& strPath = getHomePath();

        if(strPath.empty()){
            return false;
        }

        
        strFile = strPath + pFileName + pSuffix;
    }
    else{
        strFile = pFileName;
        strFile += pSuffix;
    }

    
    std::ifstream ifile;
    ifile.open(strFile.c_str());

    if (ifile.fail()){
        
        return false;
    }

    
    ifile.seekg(0, std::ios::end);
    std::streamoff len = ifile.tellg();
    if(len <= 0){
        return false;
    }

    
    ifile.seekg(0, std::ios::beg);
    
    m_oFile.resize((std::string::size_type)len);
    
    ifile.read((char*)m_oFile.c_str(), len);
    
    ifile.close();

    return true;
}

bool CISMConfig::find(const std::string& strKey, std::string& str, size_t offset)
{
    std::string::size_type pos = m_oFile.find(strKey.c_str(), offset);

    if(std::string::npos == pos){
        return false;
    }

    
    std::string::size_type head = pos;
    const char* ptr = m_oFile.c_str();
    while((0 < head) && ('\n' != ptr[head])){
        head--;
    }

    if('\n' == ptr[head]){
        
        if (head < m_oFile.size() - 1){
            ++head;
        }
        
    }

    if('#' == ptr[head]){
        return find(strKey, str, pos + strKey.size());
    }

    std::string::size_type tail = strKey.size() + head;
    std::string::size_type eq_pos = tail;   

    while(('\r' != ptr[tail])
        && ('\n' != ptr[tail])
        && (tail < m_oFile.size())){
        tail++;
    }
    if(0 < (tail - eq_pos)){
        str.assign(&ptr[eq_pos], tail - eq_pos);
        return true;
    }

    return false;
}
void CISMConfig::split( const std::string & str,const std::string & delm_str,std::vector<std::string> & parts )
{
    if( str.empty() || delm_str.empty() ){
        return;
    }

    std::size_t b = 0;
    std::size_t e = str.find( delm_str );
    while( std::string::npos != e ){
        std::string sub = str.substr( b,e-b );
        if(!sub.empty()){
            parts.push_back(sub);
        }

        b = e + delm_str.size();
        e = str.find( delm_str,b );
    }

    
    parts.push_back( str.substr( b,e ) );
}
std::string CISMConfig::getHomePath()
{
    std::string homePath;

#if defined(WIN32)
    
    std::string fullPath = getProcessFileFullPath();

    const char * pExePath = ACE::dirname(fullPath.c_str());
    if (NULL != pExePath){
        std::string exeFilePath = pExePath;
        homePath = exeFilePath + ACE_DIRECTORY_SEPARATOR_STR_A;
    }
    else{
        return homePath;
    }

#else


    char link[MAX_PATH + 1] = {0x00};
    char path[MAX_PATH + 1] = {0x00};

    sprintf_s(link, MAX_PATH + 1, "/proc/%d/exe", getpid()); 
    int i = readlink(link, path, sizeof(path) );

    if (i < MAX_PATH + 1){
        path[i] = '\0';
    }
    else{
        return homePath;
    }

    homePath = string(path);
    homePath =  homePath.substr(0, homePath.rfind('/') + 1);
#endif

    return homePath;
}

bool CISMConfig::modify(const std::string & strKey, const std::string &strVaule, 
            const char* pFileName, const char* pSuffix, bool bCompletePath)
			{
    std::string strFile;
    string line;
    int offset = 0;

    if(false == bCompletePath){
        const std::string& strPath = getHomePath();

        if(strPath.empty()){
            return false;
        }

        
        strFile = strPath + pFileName + pSuffix;
    }
    else{
        strFile = pFileName;
        strFile += pSuffix;
    }

    
    std::fstream ifile;
    
    ifile.open(strFile.c_str(), ios::in| ios:: out | ios::binary);

    if (ifile.fail()){
        
        return false;
    }

    
    ifile.seekg(0, std::ios::end);
    std::streamoff len = ifile.tellg();
    if(len <= 0){
        return false;
    }

    
    ifile.seekg(0, std::ios::beg);
    
    m_oFile.resize((std::string::size_type)len);
    
    ifile.read((char*)m_oFile.c_str(), len);
    std::string::size_type pos = m_oFile.find(strKey.c_str(), 0);

    if(std::string::npos == pos){
        return false;
    }
    offset = pos + strKey.size() + 1;
    ifile.seekp(offset, std::ios::beg);
    ifile.write(strVaule.c_str(), strVaule.size());;
    
    ifile.close();

    return true;
}

void sra_init_log(char* program_name)
{
    char acPath[MAX_FULL_PATH_LEN + 1] = {0};

    if(NULL == program_name){
        printf("failed to load log file.");
        return;
    }
    
    
    SNPRINTF(acPath, MAX_FULL_PATH_LEN + 1, MAX_FULL_PATH_LEN, "%s", program_name);

    make_matulDir(program_name);
    (void)g_objLogger.SetPath(acPath);
    (void)g_objLogger.SetFile(LOG_FILE_NAME);
    (void)g_objLogger.SetMaxFileSize(100*1024*1024); 
    (void)g_objLogger.SetLogCount(5); 

    g_objLogger.SetLevel(OS_LOG_INFO);
    g_objLogger.SetCategory(OS_LOG_CATA_DEBUG);

    (void)g_objLogger.SetRollingMode(ROLLING_SIZE);

    return;

    
}

void sra_init_err_code()
{
    
    g_map_err_en[ERROR_COMMAND_NOT_SUPPORTED] = make_pair("The command requested by the SRM does not exist.", "Please contact the administrator.");
    g_map_err_en[ERROR_LOCALE_NOT_SUPPORTED] = make_pair("The locale is not supported.", "Please select simplified Chinese or English.");
    g_map_err_en[RETURN_ERR] = make_pair("Internal error.", "Please contact the administrator.");
    g_map_err_en[ERROR_INTERNAL_PROCESS_FAIL] = make_pair("Internal error.", "Please contact the administrator.");
    g_map_err_en[ERROR_NO_INBAND] = make_pair("The InbandAgent service is not found.", "Please configure the InbandAgent service correctly.");
    
    g_map_err_en[ERROR_GET_HOSTID] = make_pair("Failed to obtain the information about the host to which the target device is mapped.", "Please check that the target device is mapped to the ESX host.");
    g_map_err_en[ERROR_RECREATE_SNAPSHOT] = make_pair("Failed to re-create snapshots.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_ACTIVE_SNAPSHOT] = make_pair("Failed to activate the snapshots.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_CREATE_SNAPSHOT] = make_pair("Failed to create the snapshot.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_GET_SNAPSHOT_INFO] = make_pair("Failed to obtain the snapshot information.", "Please query the storage array information for help.");
    
    g_map_err_en[ERROR_GET_SNAPSHOT_WWN] = make_pair("Failed to obtain the snapshot WWN.", "Please check that the snapshot of the target device is mapped to the ESX host.");
    g_map_err_en[ERROR_INVALIDATE_DEVICE_ID] = make_pair("Invalid device ID.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_INVALIDATE_GROUP_ID] = make_pair("Invalid consistency group ID.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_INVALIDATE_LOCAL_ARRAY_SN] = make_pair("Invalid local storage array ID.", "Please query the storage array information for help.");
    g_map_err_en[ERROR_INVALIDATE_PEER_ARRAY_SN] = make_pair("Invalid peer storage array ID.", "Please query the storage array information for help.");
    
    g_map_err_en[ERROR_INVALIDATE_HYMIRROR_STATUS] = make_pair("Failed to query the synchronization progress.", "Please query the status of the remote replication or consistency group on the storage array.");
    g_map_err_en[ERROR_INVALIDATE_SYNC_ID] = make_pair("Invalid synchronization ID.", "Please contact the administrator.");
    g_map_err_en[ERROR_IC_SERVER_NOT_EXIST] = make_pair("The InbandAgent service does not exist.", "Please configure the InbandAgent service correctly.");
    g_map_err_en[DEVICE_NOT_EXIST] = make_pair("The command device does not exist.", "Please map the command device to the SRM server.");
    g_map_err_en[MAP_NULL_MORE_ONE] = make_pair("The command device does not exist.", "Please map the command device to the SRM server.");
    
    g_map_err_en[ERROR_NET_SOURCE_INVALID] = make_pair("The Network Resouce is invalid.", "Please map the command device to the SRM server.");
    g_map_warn_en[WARN_REPLICATION_IS_PROCESSING] = make_pair("The requested remote replication or consistency group is being synchronized.", "Please check the status of the remote replication or consistency group.");
    g_map_err_en[ERROR_LUN_NOT_OPERATION] = make_pair("Operation prohibited under the current status of the LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SYSTEM_BUSY] = make_pair("The system is busy. The operation cannot be performed for the moment. Please try again later.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SYSTEM_ABNORMAL] = make_pair("The system operating status is abnormal and the operation failed.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_EXCEED_SYSTEM_SPEC] = make_pair("Exceeds what is defined in the system specifications.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SNAPSHOT_EXCEEDED] = make_pair("Maximum of LUN snapshots exceeded.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_EXCEEDED] = make_pair("Maximum of source LUNs exceeded.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_SNAPSHOT_EXCEEDED] = make_pair("Maximum of LUN snapshots of each source LUN exceeded.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_IN_ANOTHER_SESSION] = make_pair("Source LUN is being used in another session.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_LUN_NOT_SNAPSHOT_LUN] = make_pair("Current LUN is not a LUN snapshot.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_NOT_SNAPSHOT_ANOTHER] = make_pair("Current LUN is a LUN snapshot of another user.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_NUN_NOT_SOURCE_LUN] = make_pair("Current LUN is not a source LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_STATUS_NOT_OPERATION] = make_pair("Operation prohibited under the current status of the LUN,Please check this lun's status.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SYSTEM_BUSY_TRY_LATER] = make_pair("The system is busy. Please try later.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_SNAPSHOT_INVAILD_LICENSE] = make_pair("No valid license file for snapshot. Please import a valid license file.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_STATUS_NOT_OPER] = make_pair("Operation prohibited under the current status of the LUN,Please check this lun's status.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LUN_ABNORMAL] = make_pair("Snapshot is abnormal.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_OBJECT_BUSY] = make_pair("The current object is busy,Please try again later.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_DELETE_MAPPED_LUN] = make_pair("The current LUN has been mapped,can't be delete.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_CHOSE_SAME_LUN] = make_pair("You have chosen the same LUN ,please check and try again.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_DELETE_SNAPSHOT_REFER_ONLINE] = make_pair("Deleting not allowed, please refer to help On-line.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LIMIT_NUMBER_SNAPSHOT] = make_pair("The number of virtual snapshot source LUNs has reached the upper limit.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_HOST_HAVE_LUN_MAPPING] = make_pair("The host or host group already has virtual LUN mapping.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_DELETE_NOT_EXIST_MAP] = make_pair("Map for the host or host group does not exist.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_REMOTE_LUN_NOT_EXIST] = make_pair("Remote replication LUN does not exist.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_NOT_EXIST] = make_pair("Remote replication does not exist.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_CONSISTENT_GROUP_NOT_EXIST] = make_pair("Remote replication consistent group does not exist.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SLAVELUN_NOT_BELONG_REMOTE] = make_pair("Slave LUN does not belong to the remote replication.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_NOT_BELONG_CONSISTENT_GROUP] = make_pair("Remote replication does not belong to the consistent group.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_MASTER_NOT_OPERATE] = make_pair("Master array cannot be operated.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SLAVE_NOT_OPERATE] = make_pair("Slave array cannot be operated.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_FORCE_OPERATE_INGOOD_CONDITION] = make_pair("Forced operation cannot be performed when connection is in good condition.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_FORCE_OPERATE_INBAD_CONDITION] = make_pair("Forced operation cannot be performed when connection is not in good condition.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_INVALID_LICENSE] = make_pair("No valid license file for remote replication. Please import a valid license file.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_CONTROLLER_STATUS_NOT_ALLOW_OPERATE] = make_pair("Operation prohibited due to the status of the current controller.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_DATA_STATUS_NOT_ALLOW_OPERATE] = make_pair("Operation prohibited due to the status of the data in the slave LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_STATUS_NOT_ALLOW_OPERATE] = make_pair("Operation prohibited due to the connection between mirrors in the remote replication.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_CONSISTENT_GROUP_STATUS_NOT_ALLOW_OPERATE] = make_pair("Operation prohibited due to the connection between mirrors in the remote replication consistent group.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_NOT_OPERATE_MEMBER_IN_REMOTE_CONSISTENT_GROUP] = make_pair("Member of the remote replication consistent group cannot be operated directly.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_ADD_REMOTE_BELONG_DIFF_ARRAY] = make_pair("Added remote replication and the consistent group belong to different arrays.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_NOT_CONN_REMOTE_LUN] = make_pair("Connection to the remote LUN is not in good condition.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_NOT_CONN_PEER_CONTROLLER] = make_pair("Connection to the peer controller is not in good condition.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_LOCAL_ARRAY_BUSY] = make_pair("Local array busy.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_ARRAY_BUSY] = make_pair("Remote array busy.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_MASTER_LUN_NOT_TEAM_MEMBER] = make_pair("Master LUN is not a member of team.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_MASTER_LUN_ABNORMAL_NOT_OPERATE] = make_pair("Operation prohibited due to the abnormal health status of the master LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SLAVE_LUN_ABNORMAL_NOT_OPERATE] = make_pair("Operation prohibited due to the abnormal health status of the slave LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_MASTER_LUN_ROLLBACK_NOT_OPERATE] = make_pair("Operation prohibited due to the rollback status of the master LUN.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SLAVE_LUN_ROLLBACK_NOT_OPERATE] = make_pair("Operation prohibited due to the rollback status of the slave LUN.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_CONSISTET_GROUP_FAULT_NOT_OPERATE] = make_pair("Consistency group is fault. The operation can not be executed.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_CONSISTET_GROUP_NO_MEMBER] = make_pair("There is no member in the consistency group. Operation failed. ", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_REMOTE_CONSIST_GROUP_NOT_EXIST] = make_pair("Consist group does not exist on remote array.  .", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_ANOTHER_REMOTE_STATE_NOT_OPERATE] = make_pair("Operation prohibited due to the state of another remote replication team.", "Please query the storage array information for help.");    
    g_map_err_en[ERROR_SLAVELUN_NOT_MAP] = make_pair("Slave LUN does not map to ESX.", "Please query the storage array information for help.");    
    
    g_map_err_en[ERROR_REPEAT_MAPPING] = make_pair("This LUN is mapped repeatedly.", "Please check that the LUN is mapped to the ESX host.");    
    g_map_err_en[ERROR_SLAVELUN_IS_SYNCING] = make_pair("This LUN is syncing", "Please query the storage array information for help.");

    
    
    
    g_map_err_en[RETURN_REPEAT_NAME] = make_pair("The object name already exists.","Please query the storage array information for help.");
    g_map_err_en[RETURN_OBJ_NUM_REACH_MAX] = make_pair("The number of created objects has reached the upper limit.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SYSTEM_INNER_ERROR] = make_pair("The system has an internal error.","Please query the storage array information for help.");
    g_map_err_en[RETURN_LMP_HOST_MAPPING_VIEW_BE_NOT_EXIST] = make_pair("The specified mapping view is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_LMP_HOST_BUSY] = make_pair("Services are running on the service.  ","Please query the storage array information for help.");

    g_map_err_en[RETURN_LMP_MAP_BUSY] = make_pair("Services are running on the LUN. The LUN mapping cannot be removed.","Please query the storage array information for help.");
    g_map_err_en[RETURN_LMP_MAP_LUN_REPEAT_MAP] = make_pair("The LUN has been added to the mapping view. ","Please query the storage array information for help.");
    g_map_err_en[RETURN_META_SPACE_NOT_ENOUGH] = make_pair("The storage pool does not have enough space for storing metadata.","Please query the storage array information for help.");
    g_map_err_en[RETURN_OM_AUTH_LOGIN_FAILED] = make_pair("The user name or password is incorrect.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SNAP_REACH_SNAPSHOTLUN_MAX_NUM] = make_pair("The number of snapshots has reached the upper limit.","Please query the storage array information for help.");

    g_map_err_en[RETURN_SNAP_REACH_SNAPSHOT_PER_ORIGINLUN_MAX_NUM] = make_pair("The number of snapshot LUNs of the source LUN has reached the upper limit.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SNAP_GET_LICENSE_STATE_FAILED] = make_pair("The snapshot license file is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SNAP_NOT_ALLOW_DELETE_SNAPLUN] = make_pair("The snapshot LUN has been applied to other services.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SNAP_PARAM_ERROR] = make_pair("The specified snapshot is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_SNAP_SNAPSHOTLUN_NOT_EXIST] = make_pair("The snapshot LUN does not exist. ","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_SERIAL_NOT_EXIST] = make_pair("The specified remote device is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_LUN_NOT_EXIST] = make_pair("The specified LUN is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_TEAM_NOT_EXIST] = make_pair("The specified remote replication is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_ID_NOT_EXIST] = make_pair("The specified remote replication consistency group is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_TEAM_NOT_MEMBER_OF_CG] = make_pair("The specified remote replication is not a member of the consistency group.","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_CAN_NOT_EXECUTE_AT_MST_NODE] = make_pair("This operation cannot be performed on the active end.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CAN_NOT_EXECUTE_AT_SLV_NODE] = make_pair("This operation cannot be performed on the standby end.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_LICENSE_NOT_ALLOW_OPERATION] = make_pair("The remote replication license file is unavailable.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CONTROLLER_NOT_ALLOW_OPERATION] = make_pair("The system is busy.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_REMOTE_LINK_BAD] = make_pair("The connection with the remote device is abnormal.","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_PEER_CTLR_LINK_BAD] = make_pair("The system is busy.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_SYS_BUSY] = make_pair("The system is busy.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_REMOTE_SYS_BUSY] = make_pair("The system is busy.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_OBJ_BUSY] = make_pair("The system is busy.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_PAIR_SYNCING_NOT_OPERATION] = make_pair("The pair of the remote replication is in the synchronizing state.","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_PAIR_SPLITED_NOT_OPERATION] = make_pair("The pair of the remote replication is in the split state.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_PAIR_INTERRUPTED_NOT_OPERATION] = make_pair("The pair of the remote replication is interrupted.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_PAIR_INVALID_NOT_OPERATION] = make_pair("The remote replication is in the faulty state.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_PAIR_TO_BE_RECOVERD_NOT_OPERATION] = make_pair("The remote replication is waiting for recovery.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_PAIR_SYNCED_NOT_OPERATION] = make_pair("The remote replication is in the synchronizing state.","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_SLV_DATA_INCONSISTENCY_NOT_OPERATION] = make_pair("The slave LUN data of the remote replication is in the inconsistent state. ","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_STATUS_FAULT_NOT_OPERATION] = make_pair("The consistency group is in the faulty state.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_SYNCING_NOT_OPERATION] = make_pair("The consistency group is in the synchronizing state.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_SPLITED_NOT_OPERATION] = make_pair("The consistency group is in the split state.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_TO_BE_RECOVERD_NOT_OPERATION] = make_pair("The consistency group is waiting for recovery.","Please query the storage array information for help.");

    g_map_err_en[RETURN_RM_CG_INTERRUPTED_NOT_OPERATION] = make_pair("The consistency group is abnormally interrupted.","Please query the storage array information for help.");
    g_map_err_en[RETURN_RM_CG_INVALID_NOT_OPERATION] = make_pair("The consistency group is in the faulty state.","Please query the storage array information for help.");
    
    
    
    g_map_err_cn[ERROR_COMMAND_NOT_SUPPORTED] = make_pair("SRM请求的命令不存在。", "请联系管理员。");
    g_map_err_cn[ERROR_LOCALE_NOT_SUPPORTED] = make_pair("不能支持的区域语言。", "请使用简体中文或英文。");
    g_map_err_cn[RETURN_ERR] = make_pair("内部错误。", "请联系管理员。");
    g_map_err_cn[ERROR_INTERNAL_PROCESS_FAIL] = make_pair("内部错误。", "请联系管理员。");
    g_map_err_cn[ERROR_NO_INBAND] = make_pair("没有发现InbandAgent服务。", "请正确配置InbandAgent服务。");
    
    g_map_err_cn[ERROR_GET_HOSTID] = make_pair("获得目标设备所映射的主机信息失败。", "请确认目标设备已经被映射给ESX主机。");
    g_map_err_cn[ERROR_RECREATE_SNAPSHOT] = make_pair("重新创建快照失败。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_ACTIVE_SNAPSHOT] = make_pair("激活快照失败。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_CREATE_SNAPSHOT] = make_pair("创建快照失败。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_GET_SNAPSHOT_INFO] = make_pair("获得快照信息失败.", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_GET_SNAPSHOT_WWN] = make_pair("获得快照WWN失败。", "请确认目标设备的快照已经被映射给ESX主机。");
    g_map_err_cn[ERROR_INVALIDATE_DEVICE_ID] = make_pair("输入设备ID非法。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_INVALIDATE_GROUP_ID] = make_pair("输入一致性组ID非法。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_INVALIDATE_LOCAL_ARRAY_SN] = make_pair("输入本地阵列ID非法。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_INVALIDATE_PEER_ARRAY_SN] = make_pair("输入对端阵列ID非法。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_INVALIDATE_HYMIRROR_STATUS] = make_pair("查询同步进度失败。", "请检查阵列远程复制或一致性组状态。");
    g_map_err_cn[ERROR_INVALIDATE_SYNC_ID] = make_pair("输入同步ID非法。", "请联系管理员。");
    g_map_err_cn[ERROR_IC_SERVER_NOT_EXIST] = make_pair("InbandAgent服务不存在。", "请正确配置InbandAgent服务。");
    g_map_err_cn[DEVICE_NOT_EXIST] = make_pair("命令设备不存在。", "请将命令设备映射给SRM主机。");
    g_map_err_cn[MAP_NULL_MORE_ONE] = make_pair("命令设备不存在。", "请将命令设备映射给SRM主机。");

    g_map_err_cn[ERROR_NET_SOURCE_INVALID] = make_pair("网络资源非法。", "请将命令设备映射给SRM主机。");
    g_map_warn_cn[WARN_REPLICATION_IS_PROCESSING] = make_pair("请求的远程复制或一致性组正在同步中。", "请检查远程复制或一致性组状态。");
    g_map_err_cn[ERROR_LUN_NOT_OPERATION] = make_pair("当前LUN状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SYSTEM_BUSY] = make_pair("系统正忙，该操作暂时不能执行，请稍后再试。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SYSTEM_ABNORMAL] = make_pair("系统状态异常，操作失败", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_EXCEED_SYSTEM_SPEC] = make_pair("超过系统规格。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SNAPSHOT_EXCEEDED] = make_pair("超出允许创建的最大快照LUN个数。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_EXCEEDED] = make_pair("超出允许的源LUN个数。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_SNAPSHOT_EXCEEDED] = make_pair("超出每个源LUN允许创建的最大快照LUN个数。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_IN_ANOTHER_SESSION] = make_pair("源LUN已经被其他会话使用。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_LUN_NOT_SNAPSHOT_LUN] = make_pair("当前LUN不是快照LUN。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_NOT_SNAPSHOT_ANOTHER] = make_pair("当前LUN是其他用户的快照LUN。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_NUN_NOT_SOURCE_LUN] = make_pair("当前LUN不是源LUN。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_STATUS_NOT_OPERATION] = make_pair("当前LUN状态不允许进行该操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SYSTEM_BUSY_TRY_LATER] = make_pair("系统繁忙，请稍后重试。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_SNAPSHOT_INVAILD_LICENSE] = make_pair("快照License无效，请导入有效的License。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_STATUS_NOT_OPER] = make_pair("当前LUN状态不允许进行该操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LUN_ABNORMAL] = make_pair("快照LUN异常。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_OBJECT_BUSY] = make_pair("当前对象忙。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_DELETE_MAPPED_LUN] = make_pair("不允许删除已添加映射的LUN。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_CHOSE_SAME_LUN] = make_pair("选择了相同的LUN。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_DELETE_SNAPSHOT_REFER_ONLINE] = make_pair("删除快照操作无法进行，请查询联机帮助。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LIMIT_NUMBER_SNAPSHOT] = make_pair("已达到系统虚拟快照规格上限。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_HOST_HAVE_LUN_MAPPING] = make_pair("为已存在映射的主机(组)添加映射。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_DELETE_NOT_EXIST_MAP] = make_pair("删除不存在的映射。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_REMOTE_LUN_NOT_EXIST] = make_pair("远程复制LUN不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_NOT_EXIST] = make_pair("远程复制不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_CONSISTENT_GROUP_NOT_EXIST] = make_pair("远程复制一致性组不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SLAVELUN_NOT_BELONG_REMOTE] = make_pair("从LUN不是远程复制的成员。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_NOT_BELONG_CONSISTENT_GROUP] = make_pair("远程复制不是一致性组的成员。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_MASTER_NOT_OPERATE] = make_pair("主端无法进行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SLAVE_NOT_OPERATE] = make_pair("从端无法进行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_FORCE_OPERATE_INGOOD_CONDITION] = make_pair("连接正常时无法进行强制操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_FORCE_OPERATE_INBAD_CONDITION] = make_pair("连接异常时无法操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_INVALID_LICENSE] = make_pair("远程复制License无效，请导入有效的License。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_CONTROLLER_STATUS_NOT_ALLOW_OPERATE] = make_pair("当前控制器状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_DATA_STATUS_NOT_ALLOW_OPERATE] = make_pair("从LUN的数据状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_STATUS_NOT_ALLOW_OPERATE] = make_pair("远程复制的镜像关系状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_CONSISTENT_GROUP_STATUS_NOT_ALLOW_OPERATE] = make_pair("远程复制一致性组的镜像关系状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_NOT_OPERATE_MEMBER_IN_REMOTE_CONSISTENT_GROUP] = make_pair("不能直接操作远程复制一致性组的小组成员。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_ADD_REMOTE_BELONG_DIFF_ARRAY] = make_pair("被添加的远程复制与一致性组属于不同的阵列。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_NOT_CONN_REMOTE_LUN] = make_pair("与远端LUN的连接异常。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_NOT_CONN_PEER_CONTROLLER] = make_pair("与对端控制器连接故障。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_LOCAL_ARRAY_BUSY] = make_pair("本端阵列系统繁忙。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_ARRAY_BUSY] = make_pair("远端阵列系统繁忙。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_MASTER_LUN_NOT_TEAM_MEMBER] = make_pair("主LUN不是Team的成员。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_MASTER_LUN_ABNORMAL_NOT_OPERATE] = make_pair("主LUN健康状态故障不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SLAVE_LUN_ABNORMAL_NOT_OPERATE] = make_pair("从LUN健康状态故障不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_MASTER_LUN_ROLLBACK_NOT_OPERATE] = make_pair("主LUN正在回滚状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SLAVE_LUN_ROLLBACK_NOT_OPERATE] = make_pair("从LUN正在回滚状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_CONSISTET_GROUP_FAULT_NOT_OPERATE] = make_pair("远程复制一致性组的故障状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_CONSISTET_GROUP_NO_MEMBER] = make_pair("一致性组没有成员，不允许执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_REMOTE_CONSIST_GROUP_NOT_EXIST] = make_pair("远端阵列一致性组不存在 。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_ANOTHER_REMOTE_STATE_NOT_OPERATE] = make_pair("另外一个Pair镜像关系状态不允许进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[ERROR_SLAVELUN_NOT_MAP] = make_pair("从LUN未映射给ESX主机。", "请查询阵列信息以获得更多帮助。");
    
    g_map_err_cn[ERROR_REPEAT_MAPPING] = make_pair("LUN重复映射。", "请确认目标设备已经被映射给ESX主机。");    
    g_map_err_cn[ERROR_SLAVELUN_IS_SYNCING] = make_pair("LUN正在同步中。", "请查询阵列信息以获得更多帮助。");

    
    
    g_map_err_cn[RETURN_REPEAT_NAME] = make_pair("名称重复。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_OBJ_NUM_REACH_MAX] = make_pair("创建的对象数量已达最大规格。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SYSTEM_INNER_ERROR] = make_pair("系统内部错误。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_LMP_HOST_MAPPING_VIEW_BE_NOT_EXIST] = make_pair("指定的映射视图不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_LMP_HOST_BUSY] = make_pair("主机有业务运行，不能移除或者替换映射视图。  ", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_LMP_MAP_BUSY] = make_pair("LUN有业务运行，不能解除映射。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_LMP_MAP_LUN_REPEAT_MAP] = make_pair("LUN已经添加给此映射视图", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_META_SPACE_NOT_ENOUGH] = make_pair("存储池元数据容量不足，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_OM_AUTH_LOGIN_FAILED] = make_pair("用户登录失败。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SNAP_REACH_SNAPSHOTLUN_MAX_NUM] = make_pair("系统中快照的数量已达最大规格。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_SNAP_REACH_SNAPSHOT_PER_ORIGINLUN_MAX_NUM] = make_pair("源LUN的快照LUN数量已达最大规格。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SNAP_GET_LICENSE_STATE_FAILED] = make_pair("快照license不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SNAP_NOT_ALLOW_DELETE_SNAPLUN] = make_pair("快照已应用于其他业务，不能执行删除操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SNAP_PARAM_ERROR] = make_pair("指定的快照不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_SNAP_SNAPSHOTLUN_NOT_EXIST] = make_pair("快照LUN不存在。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_SERIAL_NOT_EXIST] = make_pair("指定的远端设备不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_LUN_NOT_EXIST] = make_pair("远程镜像(主/从)LUN(ID)不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_TEAM_NOT_EXIST] = make_pair("指定的远程复制不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_ID_NOT_EXIST] = make_pair("指定的远程复制一致性组不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_TEAM_NOT_MEMBER_OF_CG] = make_pair("指定的远程复制不是该一致性组的成员。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_CAN_NOT_EXECUTE_AT_MST_NODE] = make_pair("主端无法进行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CAN_NOT_EXECUTE_AT_SLV_NODE] = make_pair("从端无法进行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_LICENSE_NOT_ALLOW_OPERATION] = make_pair("远程复制License不存在。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CONTROLLER_NOT_ALLOW_OPERATION] = make_pair("系统繁忙。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_REMOTE_LINK_BAD] = make_pair("远端设备连接异常。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_PEER_CTLR_LINK_BAD] = make_pair("与对端控制器连接异常。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_SYS_BUSY] = make_pair("本阵列系统繁忙。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_REMOTE_SYS_BUSY] = make_pair("远端阵列系统繁忙。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_OBJ_BUSY] = make_pair("前操作对象正在处理事务，无法进行操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_PAIR_SYNCING_NOT_OPERATION] = make_pair("远程复制pair的运行状态为同步中，不能执行此操作。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_PAIR_SPLITED_NOT_OPERATION] = make_pair("远程复制的运行状态为分裂，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_PAIR_INTERRUPTED_NOT_OPERATION] = make_pair("远程复制的运行状态为异常断开，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_PAIR_INVALID_NOT_OPERATION] = make_pair("远程复制的运行状态为失效，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_PAIR_TO_BE_RECOVERD_NOT_OPERATION] = make_pair("远程复制的运行状态为待恢复，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_PAIR_SYNCED_NOT_OPERATION] = make_pair("远程复制的运行状态为同步，不能执行此操作。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_SLV_DATA_INCONSISTENCY_NOT_OPERATION] = make_pair("远程复制的从LUN数据状态为不一致，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_STATUS_FAULT_NOT_OPERATION] = make_pair("一致性组的健康状态为故障，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_SYNCING_NOT_OPERATION] = make_pair("一致性组的运行状态为同步中，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_SPLITED_NOT_OPERATION] = make_pair("一致性组的运行状态为分裂，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_TO_BE_RECOVERD_NOT_OPERATION] = make_pair("一致性组的运行状态为待恢复，不能执行此操作。", "请查询阵列信息以获得更多帮助。");

    g_map_err_cn[RETURN_RM_CG_INTERRUPTED_NOT_OPERATION] = make_pair("一致性组的运行状态为异常断开，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    g_map_err_cn[RETURN_RM_CG_INVALID_NOT_OPERATION] = make_pair("一致性组的运行状态为失效，不能执行此操作。", "请查询阵列信息以获得更多帮助。");
    


    
    g_map_err_en[ERRPR_COMMON_USERPWD_ERROR] = make_pair("The user name or password is incorrect.","Check the user name and password, and try again.");
    g_map_err_cn[ERRPR_COMMON_USERPWD_ERROR] = make_pair("用户名或密码错误。","请检查用户名与密码后重试。");
    

    
    g_map_err_en[ERROR_INVALIDATE_IPADDRESS] = make_pair("The IP address is incorrect.","Check the IP address, and try again.");
    g_map_err_cn[ERROR_INVALIDATE_IPADDRESS] = make_pair("IP地址输入错误。","请检查IP地址后重试。");
    g_map_err_en[ERROR_CONNECTSERVER_CODE_LIBCURL] = make_pair("Couldn't connect to server.","Check the IP address, and try again.");
    g_map_err_cn[ERROR_CONNECTSERVER_CODE_LIBCURL] = make_pair("连接服务端失败。","请检查IP地址后重试。");
    

    g_map_err_en[ERROR_COMMON_OVER_MAX_USER] = make_pair("Exceeding maximum of users.", "The number of users has reached the upper limit. Please operate until one user exits.");
    g_map_err_cn[ERROR_COMMON_OVER_MAX_USER] = make_pair("已达到最大用户数。", "系统登录用户已达到上限，请耐心等待其他用户退出再进行操作。");
    return;
    
}

int dispatch(XmlReader &reader)
{
    map<string, SRA_ENTRY>::iterator it;

    char commander[LENGTH_COMMON] = {0};

    if (!reader.get_string("/Command/Name", commander, 0)){
        COMMLOG(OS_LOG_ERROR, "%s", "read /Command/Name failed.");
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    it = g_mapFunc.find(commander);
    if (it == g_mapFunc.end()){
        COMMLOG(OS_LOG_ERROR, "No registed function found for commander:%s", commander);
        return ERROR_COMMAND_NOT_SUPPORTED;
    }

    print("Receive command from SRM:%s",commander);
    
    if (RETURN_OK != (*it).second(reader)){
        COMMLOG(OS_LOG_ERROR, "failed to exec commander:%s", commander);
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    return RETURN_OK;
}

void register_fun(string key, SRA_ENTRY value)
{
    (void)g_mapFunc.insert(make_pair(key, value));

    return;
}

int send_error_info(XmlReader &reader, int error_code)
{
    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    XmlWriter writer;

    CHECK_FALSE(reader.get_string(XML_REQUSET_TITLE, xmlns, 0), RETURN_ERR);
    CHECK_FALSE(reader.get_string(XML_OUTPUTFILE, output_file, 0), RETURN_ERR);

    CHECK_FALSE(writer.set_int(XML_RESPONSE_ERROR, error_code), RETURN_ERR);
    CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, xmlns), RETURN_ERR);

    if (!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }

    return RETURN_OK;
}


int main(int argc, char* argv[])
{
    int ret = RETURN_ERR;

    string input("");
    string str_line;

    XmlReader reader;

    sra_init_reg_function();
    

    sra_init_err_code();

    g_cli_type = OUTBAND_REST_TYPE;

    if (argc > 1){
        char* pszOp = argv[1];

        if (0 == strcmp(pszOp, "-v")){
            printf("OpenSDS_Storage_SRA %s.\n",SRA_VERSION);
            return ISSP_RTN_OK;
        }
        else if (0 == strcmp(pszOp, "-h")){
            printf("./sra.exe -v : Inquire OpenSDS_Storage_SRA Version.\n");
            return ISSP_RTN_OK;
        }
        else{
            printf("Usage: ./sra.exe -v Inquire OpenSDS_Storage_SRA Version.\n");
            printf("Usage: ./sra.exe -h for more information\n");
            return ISSP_RTN_OK;
        }
    }

    
    

    while(getline(cin, str_line)){
#ifdef WIN32
        input += str_line + "\r\n";
#else
        input += str_line + "\n";
#endif
    }

    
    
    if (!reader.load_from_string(input, TIXML_DEFAULT_ENCODING)){
        print("%s","load_from_string err");
        return -1;
    }

    
    char Log_dir[1024] = {0};
    if(!reader.get_string(XML_LOGDIRECTORY, Log_dir, 0)){
        print("%s","get log directory err");
        return -1;
    }
    sra_init_log(Log_dir);
    if(!reader.get_string(XML_LOGLEVEL, LOG_LEVEL, 0)){
        print("%s","get log level err");
        return -1;
    }
    print("Version:%s",SIGNATURE2);

    

    
    CISMConfig oConfig;

#ifdef WIN32
    if (oConfig.open("config",".txt") == false){
         COMMLOG(OS_LOG_ERROR, "open config.txt failed!");
         return -1;
    }
#else
    if (oConfig.open("/srm/sra/conf/config",".txt", true) == false){
        COMMLOG(OS_LOG_ERROR, "open config.txt failed!");
        return -1;
    }
#endif

    {
        int isupportNfs = -1;
        if(oConfig.getIntValue("isSupportNfs", isupportNfs) != false){
            if (isupportNfs == 1){
                g_bnfs = true;
            }
            else{
                 g_bnfs = false;
            }
        }
        int isupportStretch = -1;
        if(oConfig.getIntValue("isSupportStretched", isupportStretch) != false){
            if (isupportStretch == 1){
                g_bstretch = true;

            }
            else{
                COMMLOG(OS_LOG_INFO, "config.txt is not support Stretched. isupportStretch = %d", isupportStretch);
                g_bstretch = false;
            }
        }

        int isFusionStorage = -1;
        if(oConfig.getIntValue("isFusionStorage", isFusionStorage) != false){
            if (isFusionStorage == 1){
                COMMLOG(OS_LOG_INFO, "config.txt is FusionStorage. isFusionStorage = %d", isFusionStorage);
                g_bFusionStorage = true;

            }
            else{
                g_bFusionStorage = false;
            }
        }
    }
    
    ret = dispatch(reader);

    if (RETURN_OK != ret){
        (void)send_error_info(reader, ret);
        return -1;
    }

    return 0;
}
