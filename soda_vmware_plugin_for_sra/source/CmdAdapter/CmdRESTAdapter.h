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

#ifndef _CMD_REST_ADAPTER_H_
#define _CMD_REST_ADAPTER_H_

#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "CmdAdapter.h"
#include "json/reader.h"
#include "../sra/sra.h"

using namespace std;

#define DEC_TO_HEX_BUFFER        256
#define SESSION_INVALID_TIME    600                        // session expiration time, more than 600 seconds, log in again to get a new session
#define STR_INVALID_CGID            "ffffffffffffffff"     // Return this string to indicate that the CGID is invalid
#define MAX_REPLICATIONPAIR_COUNT (64 * 1024)              // Remote copy full specification quantity
#define MAX_HYPERMETRO_COUNT (64 * 1024)                   // Double full size
#define MAX_MAPPVIEW_COUNT (64*1024)
#define RECOMMEND_RANGE_COUNT 150                          // Recommended interval span of the range parameter
#define SPECIAL_RANGE_COUNT 100                            // Range span of the range parameter
#define MAX_RANGE_COUNT 10000                              // The maximum number of range parameters
#define STR_NOT_EXIST               "----"

// REST Request address
#define RESTURL_REMOTE_DEVICE                "/remote_device"
#define RESTURL_LUN                            "/lun"
#define RESTURL_LUN_ASSOCIATE                RESTURL_LUN"/associate"
#define RESTURL_LUNGROUP                    "/lungroup"                                             // LUN group
#define RESTURL_LUNGROUP_ASSOCIATE            RESTURL_LUNGROUP"/associate"                          // the association query of LUN group 
#define RESTURL_HOSTGROUP                    "/hostgroup"                                           // host group
#define RESTURL_HOSTGROUP_ASSOCIATE            RESTURL_HOSTGROUP"/associate"                        // the association query of host group
#define RESTURL_LUN_CREATE_SNAPSHOT            RESTURL_LUN"/snapshot_lun_canuse"
#define RESTURL_SPLITMIRROR                    "/splitmirror"
#define    RESTURL_SPLITMIRRORTARGETLUN        RESTURL_SPLITMIRROR"/hyperclone_lun_info"
#define RESTURL_SPLIT_HYPERCLONE_SLAVE        RESTURL_SPLITMIRROR"/split_hyperclone_slave"          // splitmirror lun
#define RESTURL_SYNCHRONIZE_HYPERCLONE_SLAVE RESTURL_SPLITMIRROR"/synchronize_hyperclone_slave"     // synchronize the slave LUN
#define RESTURL_LUNGROUP_FILTER              RESTURL_LUNGROUP"?filter"                              // the filtering query of LUN group
#define RESTURL_HOSTGROUP_FILTER              RESTURL_HOSTGROUP"?filter"                            // the filtering query of host group

#define RESTURL_REPLICATIONPAIR                "/REPLICATIONPAIR"                                    // remote replication Pair
#define RESTURL_REPLICATIONPAIR_ASSOCIATE    "/REPLICATIONPAIR/associate"                            // the association query of remote replication
#define RESTURL_REMOTEREPLICATION_LUN        "/remotereplicationmemberlun"                           //  the lun of remote of replication
#define RESTURL_CONSISTENTGROUP                "/consistentgroup"                                    // Consistency group
#define RESTURL_VSTOREPAIR                  "/REPLICATION_VSTOREPAIR"                                // vstore PAIR
#define RESTURL_VSTOREPAIR_SYNC             RESTURL_VSTOREPAIR"/sync"                                // Synchronize tenant pair
#define RESTURL_VSTOREPAIR_SPLIT            RESTURL_VSTOREPAIR"/split"                               // split vstore pair
#define RESTURL_VSTOREPAIR_SWITCH           RESTURL_VSTOREPAIR"/switch"                              // vstore pair master-slave switch
#define RESTURL_CONSISTENTGROUP_SYNC        "/SYNCHRONIZE_CONSISTENCY_GROUP"                         // synchronize
#define RESTURL_CONSISTENTGROUP_SPLIT        "/SPLIT_CONSISTENCY_GROUP"                              // splite
#define RESTURL_CONSISTENTGROUP_SWITCH        "/SWITCH_GROUP_ROLE"                                   // Master-slave switching
#define RESTURL_CONSISTENTGROUP_DOSWITCH    RESTURL_CONSISTENTGROUP"/do_switch_group_role"           // Force master-slave switch
#define RESTURL_MAPPINGVIEW                    "/mappingview"                                        // mappingview
#define RESTURL_MAPPINGVIEW_ASSOCIATE        RESTURL_MAPPINGVIEW"/associate"                        
#define RESTURL_MAPPINGVIEW_ASSOCIATE_ADD    RESTURL_MAPPINGVIEW"/CREATE_ASSOCIATE"                  // Create mapping associations
#define RESTURL_MAPPINGVIEW_ASSOCIATE_DEL    RESTURL_MAPPINGVIEW"/REMOVE_ASSOCIATE"                  // Delete mapping association
#define RESTURL_HOST                        "/host"                                                  // host
#define RESTURL_HOST_ASSOCIATE                RESTURL_HOST"/associate"                               // Host management query
#define RESTURL_SNAPSHOT                    "/snapshot"                                              // snapshot
#define RESTURL_SNAPSHOT_ASSOCIATE            RESTURL_SNAPSHOT"/associate"                           // Snapshot association query
#define RESTURL_MAPPINGVIEW_FILTER            RESTURL_MAPPINGVIEW"?filter"                           // Filter view of the map view

#define RESTURL_ACITVIE_HYPERIMAGE            RESTURL_SNAPSHOT"/activate"                            // active snapshot
#define RESTURL_ROLLBACK_HYPERIMAGE            RESTURL_SNAPSHOT"/rollback"                           // rollback snapshot
#define RESTURL_DISABLE_HPERIMAGE            RESTURL_SNAPSHOT"/stop"                                 // stop snapshot
#define RESTURL_FSSNAPSHOT                    "/fssnapshot"                                          // FS snapshot
#define RESTURL_FSSNAPSHOT_ASSOCIATE        RESTURL_FSSNAPSHOT"/associate"                           // the association query of snapshot
#define RESTURL_ROLLBACK_FSHYPERIMAGE        RESTURL_FSSNAPSHOT"/rollback_fssnapshot"                // the rollback of FS snapshot
#define RESTURL_LUNCOPY                        "/luncopy"
#define RESTURL_LUNCOPY_START                RESTURL_LUNCOPY"/start_hypercopy"                       // start lun copy
#define RESTURL_SYNCRONIZE_HYPERMIRROR        RESTURL_REPLICATIONPAIR"/sync"                         // sync remote replication 
#define RESTURL_SPILT_HYPERMIRROR            RESTURL_REPLICATIONPAIR"/split"                         // spilt remote replication
#define RESTURL_SWITCHROLE_HYPERMIRROR        RESTURL_REPLICATIONPAIR"/switch"                       // switch master-slave for remote replication
#define RESTURL_SYSTEM_TIMEZONE                "/system_timezone"                                    // system_timezone
#define RESTURL_SYSTEM                        "/system/xx"                                           // system
#define RESTURL_ISCSI_INITIATOR                "/iscsi_initiator"                                    // iscsi initiator
#define RESTURL_FC_INITIATOR                "/fc_initiator"                                          // fc initiator
#define RESTURL_LUNCOPYMEMBERLUN_GETBATCH    "/luncopymemberlun/getbatch"                            
#define RESTURL_FILESYSTEM                    "/filesystem"                                          // FILESYSTEM
#define RESTURL_CLONEFS                     RESTURL_FILESYSTEM"?filter=ISCLONEFS::1"                 // Clone file system
#define RESTURL_HOST_LINK                    "/host_link"                                            // link information
#define RESTURL_SNAS_NFS_SHARE                "/nfshare"                                             // NFS share
#define RESTURL_LIF                            "/LIF"                                                // Logical port information
#define RESTURL_ACTIVE_LICENSE              "/license/activelicense"                                 // Activated license
#define RESTURL_HYPERMETROPAIR                "/HyperMetroPair"                                      
#define RESTURL_HYPERMETRODOMAIN            "/HyperMetroDomain"                                    
#define RESTURL_DISABLE_HYPERMETRO            RESTURL_HYPERMETROPAIR"/disable_hcpair"               
#define RESTURL_SWAP_HCPAIR                    RESTURL_HYPERMETROPAIR"/SWAP_HCPAIR"                
#define RESTURL_SYNC_HCPAIR                    RESTURL_HYPERMETROPAIR"/synchronize_hcpair"            
#define RESTURL_HYPERMETRO_CONSISTENTGROUP  "/HyperMetro_ConsistentGroup"                            
#define RESTURL_HYPERMETRO_CONSISTENTGROUP_STOP        RESTURL_HYPERMETRO_CONSISTENTGROUP"/stop"        
#define RESTURL_HYPERMETRO_CONSISTENTGROUP_SWITCH    RESTURL_HYPERMETRO_CONSISTENTGROUP"/switch"        
#define RESTURL_HYPERMETRO_CONSISTENTGROUP_SYNC     RESTURL_HYPERMETRO_CONSISTENTGROUP"/sync"       

// REST Request field
#define COMMON_TAG_TYPE                        "TYPE"
#define COMMON_TAG_ID                        "ID"
#define COMMON_TAG_NAME                        "NAME"
#define COMMON_TAG_PARENTTYPE                "PARENTTYPE"
#define COMMON_TAG_PARENTID                    "PARENTID"
#define COMMON_TAG_PARENTNAME                "PARENTNAME"
#define COMMON_TAG_LOCATION                    "LOCATION"
#define COMMON_TAG_HEALTHSTATUS                "HEALTHSTATUS"
#define COMMON_TAG_RUNNINGSTATUS            "RUNNINGSTATUS"
#define COMMON_TAG_DESCRIPTION                "DESCRIPTION"
#define COMMON_TAG_COUNT                    "COUNT"
#define COMMON_TAG_ASSOCIATEOBJTYPE            "ASSOCIATEOBJTYPE"
#define COMMON_TAG_ASSOCIATEOBJID            "ASSOCIATEOBJID"
#define COMMON_TAG_ASSOCIATEMETADATA        "ASSOCIATEMETADATA"
#define COMMON_TAG_APPLICATION                "APPLICATION"
#define COMMON_TAG_QUERYCONDITION            "QUERYCONDITION"
#define COMMON_TAG_ASSOCIATEOBJIDLIST        "QUERYCONDITION"
#define COMMON_TAG_CHAPNAME                    "CHAPNAME" //add by lwp
#define COMMON_TAG_STRETCHEDSTORAGE         "STRETCHEDSTORAGE"
#define COMMON_TAG_HCRESOURCETYPE            "HCRESOURCETYPE"
#define COMMON_TAG_LICENSEUSAGEINFO            "LicenseUsageInfo"
#define COMMON_TAG_LICENSE_FEATUREID        "FeatureId"
#define COMMON_TAG_LICENSE_OPENSTATUS        "OpenStatus"
#define COMMON_TAG_LICENSE_STATE            "State"
#define COMMON_TAG_ALLOCTYPE                "ALLOCTYPE"
#define COMMON_TAG_VSTOREID                 "vstoreId"

// Remote device
#define REMOTEDEVICE_TAG_WWN                "WWN"
#define REMOTEDEVICE_TAG_SN                    "SN"
#define REMOTEDEVICE_TAG_ARRAYTYPE            "ARRAYTYPE"

// LUN
#define LUN_TAG_CAPACITY                    "CAPACITY"
#define LUN_TAG_OWNINGCONTROLLER            "OWNINGCONTROLLER"
#define LUN_TAG_SNAPSHOTIDS                    "SNAPSHOTIDS"
#define LUN_TAG_LUNCOPYIDS                    "LUNCOPYIDS"
#define LUN_TAG_REMOTEREPLICATIONIDS        "REMOTEREPLICATIONIDS"
#define LUN_TAG_SPLITMIRRORIDS                "SPLITMIRRORIDS"
#define LUN_TAG_WWN                            "WWN"
#define LUN_TAG_DISGUISESTATUS                "DISGUISESTATUS"
#define LUN_TAG_TAKEOVERLUNWWN                "takeOverLunWwn"
#define LUNGRP_TAG_GROUPTYPE                "GROUPTYPE"
#define LUNGRP_TAG_APPTYPE                  "APPTYPE"

//FS
#define FS_TAG_PARENTFILESYSTEMID           "PARENTFILESYSTEMID"
#define FS_TAG_PARENTSNAPSHOTID             "PARENTSNAPSHOTID"

//Split mirror slave LUN information
#define SPLITMIRRORTARGETLUN_TAG_DATA_STATUS    "DATASTATUS"
#define SPLITMIRRORTARGETLUN_TAG_PROGRESS    "PROGRESS"
#define SPLITMIRRORTARGETLUN_TAG_STARTTIME    "STARTTIME"
#define SPLITMIRRORTARGETLUN_TAG_ENDTIME    "ENDTIME"
#define SPLITMIRRORTARGETLUN_TAG_SPLITTIME    "SPLITTIME"

// Split mirror
#define SPLITMIRROR_TAG_PRILUNID            "PRILUNID"
#define SPLITMIRROR_TAG_PRILUNNAME            "PRILUNNAME"

// Consistency group
#define CONSISTENTGROUP_TAG_RECOVERYPOLICY    "RECOVERYPOLICY"
#define CONSISTENTGROUP_TAG_REPLICATIONMODEL    "REPLICATIONMODEL"
#define CONSISTENTGROUP_TAG_SPEED            "SPEED"
#define CONSISTENTGROUP_TAG_SYNCHRONIZETYPE    "SYNCHRONIZETYPE"
#define CONSISTENTGROUP_TAG_RMLIST            "RMLIST"
#define CONSISTENTGROUP_TAG_ISPRIMARY        "ISPRIMARY"
#define CONSISTENTGROUP_TAG_PRIORITYSTATIONTYPE        "PRIORITYSTATIONTYPE"
#define CONSISTENTGROUP_TAG_TIMINGVAL        "TIMINGVAL"
#define CONSISTENTGROUP_TAG_SECRESACCESS    "SECRESACCESS"

// mappingview
#define MAPPINGVIEW_TAG_LUNMAPPING            "LUNMAPPING"
#define MAPPINGVIEW_TAG_SNAPSHOTMAPPING        "SNAPSHOTMAPPING"
#define MAPPINGVIEW_TAG_HOSTLUNID            "HOSTLUNID"
#define MAPPINGVIEW_TAG_ENABLEINBANDCOMMAND    "ENABLEINBANDCOMMAND"
#define MAPPINGVIEW_TAG_INBANDLUNWWN        "INBANDLUNWWN"
#define MAPPINGVIEW_TAG_WORKMODE            "WORKMODE"
#define MAPPINGVIEW_TAG_LUNWWN                "LUNWWN"
#define MAPPINGVIEW_TAG_FORFILESYSTEM        "FORFILESYSTEM"

// host
#define HOST_TAG_OPERATIONSYSTEM            "OPERATIONSYSTEM"
#define HOST_TAG_NETWORKNAME                "NETWORKNAME"
#define HOST_TAG_IP                            "IP"
#define HOST_TAG_MODEL                        "MODEL"

// snapshot
#define SNAPSHOT_TAG_SNAPSHOTUSER            "SNAPSHOTUSER"
#define SNAPSHOT_TAG_TIMESTAMP                "TIMESTAMP"
#define SNAPSHOT_TAG_ROLLBACKSTARTTIME        "ROLLBACKSTARTTIME"
#define SNAPSHOT_TAG_ROLLBACKENDTIME        "ROLLBACKENDTIME"
#define SNAPSHOT_TAG_ROLLBACKSPEED            "ROLLBACKSPEED"
#define SNAPSHOT_TAG_ROLLBACKRATE            "ROLLBACKRATE"
#define SNAPSHOT_TAG_EXPOSEDTOINITIATOR        "EXPOSEDTOINITIATOR"
#define SNAPSHOT_TAG_WWN                    "WWN"
#define SNAPSHOT_TAG_USERCAPACITY            "USERCAPACITY"
#define SNAPSHOT_TAG_CONSUMEDCAPACITY        "CONSUMEDCAPACITY"


// Batch operation snapshot
#define SNAPSHOTBATCHOPERATION_TAG_SNAPSHOTLIST    "SNAPSHOTLIST"

// LUNcopy
#define LUNCOPY_TAG_COPYSPEED                "COPYSPEED"
#define LUNCOPY_TAG_LUNCOPYTYPE                "LUNCOPYTYPE"
#define LUNCOPY_TAG_COPYSYNCFIRST            "COPYSYNCFIRST"
#define LUNCOPY_TAG_COPYPROGRESS            "COPYPROGRESS"
#define LUNCOPY_TAG_COPYMARKBOOL            "COPYMARKBOOL"
#define LUNCOPY_TAG_COPYMARKTIME            "COPYMARKTIME"
#define LUNCOPY_TAG_COPYSTARTTIME            "COPYSTARTTIME"
#define LUNCOPY_TAG_COPYSTOPTIME            "COPYSTOPTIME"
#define LUNCOPY_TAG_SOURCELUN                "SOURCELUN"
#define LUNCOPY_TAG_TARGETLUN                "TARGETLUN"


// SYSTEM_TIMEZONE
#define SYSTEM_TIMEZONE_TAG_CMO_SYS_TIME_ZONE_NAME            "CMO_SYS_TIME_ZONE_NAME"
#define SYSTEM_TIMEZONE_TAG_CMO_SYS_TIME_ZONE                "CMO_SYS_TIME_ZONE"
#define SYSTEM_TIMEZONE_TAG_CMO_SYS_TIME_ZONE_NAME_STYLE    "CMO_SYS_TIME_ZONE_NAME_STYLE"
#define SYSTEM_TIMEZONE_TAG_CMO_SYS_TIME_ZONE_USE_DST        "CMO_SYS_TIME_ZONE_USE_DST"

// SYSTEM
#define SYSTEM_TAG_PRODUCTMODE                                "PRODUCTMODE"
#define SYSTEM_TAG_PRODUCTVERSION                            "PRODUCTVERSION"
#define SYSTEM_TAG_DOMAINNAME                                "DOMAINNAME"
#define SYSTEM_TAG_HIGHWATERLEVEL                            "HIGHWATERLEVEL"
#define SYSTEM_TAG_LOWWATERLEVEL                            "LOWWATERLEVEL"
#define SYSTEM_TAG_CACHEWRITEQUOTA                            "CACHEWRITEQUOTA"
#define SYSTEM_TAG_MEMBERDISKSCAPACITY                        "MEMBERDISKSCAPACITY"
#define SYSTEM_TAG_HOTSPAREDISKSCAPACITY                    "HOTSPAREDISKSCAPACITY"
#define SYSTEM_TAG_FREEDISKSCAPACITY                        "FREEDISKSCAPACITY"
#define SYSTEM_TAG_UNAVAILABLEDISKSCAPACITY                    "UNAVAILABLEDISKSCAPACITY"
#define SYSTEM_TAG_STORAGEPOOLRAWCAPACITY                    "STORAGEPOOLRAWCAPACITY"
#define SYSTEM_TAG_STORAGEPOOLCAPACITY                        "STORAGEPOOLCAPACITY"
#define SYSTEM_TAG_STORAGEPOOLUSEDCAPACITY                    "STORAGEPOOLUSEDCAPACITY"
#define SYSTEM_TAG_STORAGEPOOLFREECAPACITY                    "STORAGEPOOLFREECAPACITY"
#define SYSTEM_TAG_STORAGEPOOLHOSTSPARECAPACITY                "STORAGEPOOLHOSTSPARECAPACITY"
#define SYSTEM_TAG_THICKLUNSALLOCATECAPACITY                "THICKLUNSALLOCATECAPACITY"
#define SYSTEM_TAG_THICKLUNSUSEDCAPACITY                    "THICKLUNSUSEDCAPACITY"
#define SYSTEM_TAG_THINLUNSMAXCAPACITY                        "THINLUNSMAXCAPACITY"
#define SYSTEM_TAG_THINLUNSALLOCATECAPACITY                    "THINLUNSALLOCATECAPACITY"
#define SYSTEM_TAG_THINLUNSUSEDCAPACITY                        "THINLUNSUSEDCAPACITY"
#define SYSTEM_TAG_SECTORSIZE                                "SECTORSIZE"
#define SYSTEM_TAG_VASA_SUPPORT_BLOCK                        "VASA_SUPPORT_BLOCK"
#define SYSTEM_TAG_VASA_SUPPORT_FILESYSTEM                    "VASA_SUPPORT_FILESYSTEM"
#define SYSTEM_TAG_VASA_ALTERNATE_NAME                        "VASA_ALTERNATE_NAME"
#define SYSTEM_TAG_VASA_SUPPORT_PROFILE                        "VASA_SUPPORT_PROFILE"
#define SYSTEM_TAG_WWN                                        "wwn"
#define SYSTEM_PATCH_VERSION                                "patchVersion"


// LUNCOPYMEMBERLUN
#define LUNCOPYMEMBERLUN_TAG_LUNCOPYID                        "LUNCOPYID"
#define LUNCOPYMEMBERLUN_TAG_LUNCOPYTYPE                    "LUNCOPYTYPE"
#define LUNCOPYMEMBERLUN_TAG_LUNCLASSTYPE                    "LUNCLASSTYPE"
#define LUNCOPYMEMBERLUN_TAG_CAPACITY                        "CAPACITY"
#define LUNCOPYMEMBERLUN_TAG_POOL                            "POOL"
#define LUNCOPYMEMBERLUN_TAG_LUNSCOPETYPE                    "LUNSCOPETYPE"
#define LUNCOPYMEMBERLUN_TAG_LUNDEVICEID                    "LUNDEVICEID"
#define LUNCOPYMEMBERLUN_TAG_LUNDEVICENAME                    "LUNDEVICENAME"
#define LUNCOPYMEMBERLUN_TAG_LUNWWW                            "LUNWWW"
#define LUNCOPYMEMBERLUN_TAG_REMOTEWWPN                        "REMOTEWWPN"
#define LUNCOPYMEMBERLUN_TAG_HOSTLUNID                        "HOSTLUNID"

// REPLICATIONPAIR
#define REPLICATIONPAIR_TAG_ISPRIMARY                        "ISPRIMARY"
#define REPLICATIONPAIR_TAG_LOCALRESID                        "LOCALRESID"
#define REPLICATIONPAIR_TAG_LOCALRESNAME                    "LOCALRESNAME"
#define REPLICATIONPAIR_TAG_LOCALRESTYPE                    "LOCALRESTYPE"
#define REPLICATIONPAIR_TAG_REMOTEDEVICEID                    "REMOTEDEVICEID"
#define REPLICATIONPAIR_TAG_REMOTEDEVICESN                    "REMOTEDEVICESN"
#define REPLICATIONPAIR_TAG_REMOTEDEVICENAME                "REMOTEDEVICENAME"
#define REPLICATIONPAIR_TAG_REMOTERESID                        "REMOTERESID"
#define REPLICATIONPAIR_TAG_REMOTERESNAME                    "REMOTERESNAME"
#define REPLICATIONPAIR_TAG_ISFIRSTSYNC                        "ISFIRSTSYNC"
#define REPLICATIONPAIR_TAG_SYNCHRONIZETYPE                    "SYNCHRONIZETYPE"
#define REPLICATIONPAIR_TAG_CAPACITY                        "CAPACITY"
#define REPLICATIONPAIR_TAG_RECOVERYPOLICY                    "RECOVERYPOLICY"
#define REPLICATIONPAIR_TAG_REPLICATIONPROGRESS                "REPLICATIONPROGRESS"
#define REPLICATIONPAIR_TAG_STARTTIME                        "STARTTIME"
#define REPLICATIONPAIR_TAG_ENDTIME                            "ENDTIME"
#define REPLICATIONPAIR_TAG_SPEED                            "SPEED"
#define REPLICATIONPAIR_TAG_TIMINGVAL                        "TIMINGVAL"
#define REPLICATIONPAIR_TAG_ISROLLBACK                        "ISROLLBACK"
#define REPLICATIONPAIR_TAG_SECRESDATASTATUS                "SECRESDATASTATUS"
#define REPLICATIONPAIR_TAG_POOLNAME                        "POOLNAME"
#define REPLICATIONPAIR_TAG_REPLICATIONMODEL                "REPLICATIONMODEL"
#define REPLICATIONPAIR_TAG_PRIRESDATASTATUS                "PRIRESDATASTATUS"
#define REPLICATIONPAIR_TAG_SECRESACCESS                    "SECRESACCESS"
#define REPLICATIONPAIR_TAG_ISSECRESBEWRITE                    "ISSECRESBEWRITE"
#define REPLICATIONPAIR_TAG_ISINCG                            "ISINCG"
#define REPLICATIONPAIR_TAG_CGID                            "CGID"
#define REPLICATIONPAIR_TAG_CGNAME                            "CGNAME"
#define REPLICATIONPAIR_TAG_ISDATASYNC                        "ISDATASYNC"
#define REPLICATIONPAIR_TAG_ISLOCALDELETE                    "ISLOCALDELETE"
#define REPLICATIONPAIR_TAG_TOSYNCSRWHENDELETE                "TOSYNCSRWHENDELETE"
#define REPLICATIONPAIR_TAG_VSTOREPAIRID                    "VSTOREPAIRID"
#define REPLICATIONPAIR_TAG_IDENTITYPERSERVE                "IDENTITYPERSERVE"
#define REPLICATIONPAIR_TAG_LOCALVSTOREID                   "LOCALVSTOREID"
#define REPLICATIONPAIR_TAG_REMOTEVSTOREID                  "REMOTEVSTOREID"
#define REPLICATIONPAIR_TAG_ROLE                            "ROLE"
#define REPLICATIONPAIR_TAG_SECVSTOREACCESS                 "SECVSTOREACCESS"

//hyperMetro
#define HYPERMETROPAIR_TAG_RESOURCEWWN                        "RESOURCEWWN"
#define HYPERMETROPAIR_TAG_LOCALOBJNAME                        "LOCALOBJNAME"
#define HYPERMETROPAIR_TAG_REMOTEOBJNAME                    "REMOTEOBJNAME"
#define HYPERMETROPAIR_TAG_DOMAINID                            "DOMAINID"
#define HYPERMETROPAIR_TAG_DOMAINNAME                        "DOMAINNAME"
#define HYPERMETROPAIR_TAG_ISINCG                            "ISINCG"
#define HYPERMETROPAIR_TAG_CGID                                "CGID"
#define HYPERMETROPAIR_TAG_ISPRIMARY                        "ISPRIMARY"
#define HYPERMETROPAIR_TAG_REPLICATIONPROGRESS                "SYNCPROGRESS"
#define HYPERMETROPAIR_TAG_LOCALDATASTATE                    "LOCALDATASTATE"
#define HYPERMETROPAIR_TAG_REMOTEDATASTATE                    "REMOTEDATASTATE"
#define HYPERMETROPAIR_TAG_STARTTIME                        "STARTTIME"
#define HYPERMETROPAIR_TAG_LOCALOBJID                        "LOCALOBJID"
#define HYPERMETROPAIR_TAG_REMOTEOBJID                        "REMOTEOBJID"
#define HYPERMETROPAIR_TAG_LOCALHOSTACCESSSTATE                "LOCALHOSTACCESSSTATE"
#define HYPERMETROPAIR_TAG_REMOTEHOSTACCESSSTATE            "REMOTEHOSTACCESSSTATE"
#define HYPERMETROPAIR_TAG_SYNCDIRECTION                    "SYNCDIRECTION"
#define HYPERMETROPAIR_TAG_RUNNINGSTATUS                    "RUNNINGSTATUS"
#define HYPERMETROPAIR_TAG_LICENSE_INDEX                    "63"            //63 in the license indicates information about the double activity
#define HYPERMETROPAIR_TAG_LICENSE_OPENSTATUS                "1"            //License openstatus value 0 - uninitialized, 1-open, 2-closed
#define HYPERMETROPAIR_TAG_LICENSE_STATE                    "3"             //The state value in the license is 1-valid, 2-expired, 3-invalid

//hyperMetro consistency group
#define HYPERMETRO_CONSISTENTGROUP_TAG_MODEL                "REPLICATIONMODEL"
#define HYPERMETRO_CONSISTENTGROUP_TAG_SECRESACCESS            "SECRESACCESS"
#define HYPERMETRO_CONSISTENTGROUP_TAG_SYNCDIRECTION        "SYNCDIRECTION"

//hyperMetro Domain
#define HYPERMETRODOMAIN_TAG_REMOTEDEVICES                    "REMOTEDEVICES"
#define HYPERMETRODOMAIN_TAG_REMOTEDEVICES_SN                "devESN"
#define HYPERMETRODOMAIN_TAG_REMOTEDEVICES_DEVNAME            "devName"
#define HYPERMETRODOMAIN_TAG_CPTYPE                            "CPTYPE"
#define HYPERMETRODOMAIN_TAG_RUNNINGSTATUS                    "RUNNINGSTATUS"


//NFS share
#define NFS_TAG_FSID                                        "FSID"
#define NFS_TAG_SHAREPATH                                    "SHAREPATH"

//Host link
#define    HOSTLINK_INITIATOR_ID                                "INITIATOR_ID" 
#define HOSTLINK_INITIATOR_PORT_WWN                            "INITIATOR_PORT_WWN"
#define HOSTLINK_TARGET_ID                                    "TARGET_ID"
#define HOSTLINK_TARGET_PORT_WWN                            "TARGET_PORT_WWN"
#define HOSTLINK_INITIATOR_TYPE                                "INITIATOR_TYPE"
#define HOSTLINK_INITIATOR_NODE_WWN                            "INITIATOR_NODE_WWN"

//Logical port
#define LF_TAG_OPERATIONALSTATUS                            "OPERATIONALSTATUS"
#define LF_TAG_ROLE                                         "ROLE"

//initiator
#define INITIATOR_ISFREE                                    "ISFREE"

#define REST_PARAM_ERROR  50331651

#define HCRESOURCETYPE_FS "2"
#define ALLOCTYPE_THIN    1

// Parse JsonValue, if it is array, parse the content is not string, otherwise it is false
bool jsonValue2Array(Json::Value &, list<string> &);


class CRESTConn;

class CRESTCmd : public CCmdAdapter
{
public:
    CRESTCmd(const string& strSN = "");
    virtual ~CRESTCmd();

    // LUN
    virtual int CMD_showlun(list<LUN_INFO_STRU> &rlstLUNInfo);
    virtual int CMD_showlun(LUN_INFO_STRU &rstLUNInfo);
    
    // FS
     virtual int CMD_showfs(FS_INFO_STRU &rstFSInfo);

    // vstore pair group
    virtual int CMD_showvstorepairmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);

    // consist group
    virtual int CMD_showconsistgrmember(IN string strGroupID, OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    virtual int CMD_showconsistgrinfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    virtual int CMD_showconsistgrinfo(OUT list<GROUP_INFO_STRU> &rlstGroupInfo);
    virtual int CMD_showlunsbyconsisid(const string& consis_id,vector<string>& local_lun_ids);
    virtual int CMD_showconsistgrhminfo(IN OUT GROUP_INFO_STRU &rstGroupInfo);
    virtual int CMD_syncconsistgr(IN string strGroupID);
    virtual int CMD_syncvstorepair(IN const string& strGroupID);
    virtual int CMD_syncconsistgrhm(IN string strGroupID);
    virtual int CMD_swapconsistgr(IN string strGroupID);
    virtual int CMD_swapconsistgrhm(IN string strGroupID);
    virtual int CMD_swapvstorepair(IN const string& strGroupID);
    virtual int CMD_splitconsistgr(IN string strGroupID);
    virtual int CMD_splitvstorepair(IN const string& strGroupID);
    virtual int CMD_stopconsistgrhm(IN string strGroupID,bool isParimary);
    virtual int CMD_changeCGSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode);
    virtual int CMD_changeVstorePairSlaveLunRw(string &strGroupID, enum RESOURCE_ACCESS_E accessMode);

    // mappingview
    virtual int CMD_showmap(OUT list<MAP_INFO_STRU> &rlstMapInfo);
    virtual int CMD_showmap(IN HOST_ID_INFO_STRU &stHostIDInfo, OUT list<MAP_INFO_STRU> &rlstMapInfo);
    
    //NFS share FS share CIFS share
    virtual int CMD_addnfs(IN const string& vstoreid, IN string &strFSID,OUT string &rstrNFSID,OUT string &rstrsharPath);
    virtual int CMD_delnfs(IN const string& vstoreid, IN const string& strId);
    virtual int CMD_shownfs(IN string vstoreID, OUT list<NFS_INFO_STRU> &rlstnfsInfo);
    
    //Add NFS share permission control
    virtual int CMD_addnfsclient(IN const string& vstoreid, IN string &strIP,IN string &strfsID);

    // snapshot
    virtual int CMD_createhyimg(IN string &strSnapshotName, IN string &strSrcLUNID);
    virtual int CMD_createCloneFS(IN string vstoreID, IN string &rstrSnapshotName, IN string &rstrSrcFSID, OUT CLONEFS_INFO_STRU &dstClonefsInfo);
    virtual int CMD_showClonefsOffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    virtual int CMD_shownfs_byfsid(IN const string& vstoreid, IN string fsid, OUT list<NFS_INFO_STRU> &rlstnfsInfo);
    virtual int CMD_delclonefs(IN const string &strId);
    virtual int CMD_showvstorepair_info(IN const string &pairid, OUT VSTORE_PAIR_INFO_STRU &vstorePair);
    virtual int CMD_recreatehyimg(IN string &strSnapshotID);
    virtual int CMD_recreatehyimg(IN list<string> &rlstSnapshotID);
    virtual int CMD_delhyimg(IN string &strSnapshotID);
    virtual int CMD_disablehyimg(IN string &strSnapshotID);
    virtual int CMD_activehyimg(IN string rstrSnapshotID);
    virtual int CMD_activehyimg(IN list<string> &rlstSnapshotID);
    virtual int CMD_showhyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo);
    virtual int CMD_showhyimgoflun(IN string &strLUNID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    virtual int CMD_showlunforhyimg(OUT list<LUN_INFO_STRU> &rlstLUNInfo);
    //FS
    virtual int CMD_createFShyimg(IN string &strSnapshotName, IN string &strSrcFSID);
    virtual int CMD_delFShyimg(IN string &strSnapshotID);

    virtual int CMD_showFShyimginfo(IN OUT HYIMAGE_INFO_STRU &rstHyImageInfo);
    virtual int CMD_showhyimgoffs(IN string &strFSID, OUT list<HYIMAGE_INFO_STRU> &rlstHyImageInfo);
    virtual int CMD_showLIF(OUT list<LIF_INFO_STRU> &rlstLifInfo);
    virtual int CMD_showarrayclonefs();

    // remote replication
    virtual int CMD_synchymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID);
    virtual int CMD_splithymirror(IN string &rstrMirrorID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID);
    virtual int CMD_disablehcpair(IN string &rstrHMPairID, IN string &rstrSlaveArraySN, IN string &rstrSlaveLUNID,bool isPrimary);
    virtual int CMD_swaphymirror(IN string &strMirrorID, IN string &strSlaveArraySN, IN string &strSlaveLUNID);
    virtual int CMD_swaphypermetro(IN string &strHMPairID);
    virtual int CMD_synchmpair(IN string &strHMPairID);
    virtual int CMD_showhymirrorinfo(IN OUT HYMIRROR_INFO_STRU &rstHyMirrorInfo);
    virtual int CMD_showhymirrorinfo_all(OUT list<HYMIRROR_INFO_STRU> &rlstHyMirrorInfo);
    virtual int CMD_showhypermetroinfo(IN OUT HYPERMETROPAIR_INFO_STRU &rstHMPairInfo);
    virtual int CMD_showHyperMetroPair_all(OUT list<HYPERMETROPAIR_INFO_STRU> &rlstHyperMetroPairInfo);
    virtual int CMD_showhyperMetroDomain_all(IN string &hmdomainid, OUT HYPERMETRODOMAIN_LF_INFO_STRU &rstHMDomainInfo);
    virtual int CMD_showhymirrorlun(IN OUT HYMIRROR_LF_INFO_STRU &rstHyMirrorLUNInfo);
    virtual int CMD_showhymirrorlun_all(IN string &strMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorLUNInfo);
    virtual int CMD_showhypermetrolun_all(IN string &hypermetroid, OUT list<HYPERMETRO_LF_INFO_STRU> &rlstHMLUNInfo);
    virtual int CMD_showhymirrorfs_all(IN string &rstrMirrorID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo);
    virtual int CMD_showhymirrorbyfs(IN string &strID, OUT list<HYMIRROR_LF_INFO_STRU> &rlstHyMirrorFSInfo,IN bool bismirrorID = false);
    virtual int CMD_changeSlaveLunRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode);
    virtual int CMD_changeSlaveFsRw(string &strHyMirrorID, enum RESOURCE_ACCESS_E accessMode);

    // system
    virtual int CMD_showarrayinfo(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo);
    virtual int CMD_showarraystretched(list<REMOTE_ARRAY_STRU>& rlstRemoteArrayInfo);
    virtual int CMD_showhost(OUT list<CMDHOSTINFO_STRU> &rlstHostInfo);
    virtual int CMD_showhost(OUT CMDHOSTINFO_STRU &rHostInfo);
    virtual int CMD_showsys(OUT SYSTEM_INFO_STRU &rstSysInfo);
    virtual int CMD_showhostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo);
    virtual int CMD_showarraymodelinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo);    
    virtual int CMD_showhostlink(IN string initatorWwn, IN unsigned int nFlag, OUT list<HOST_LINK_INFO> &rlstHostLinkInfo);
   
    virtual int InitConnectInfo(const string& strSN);
    virtual int ConfigConnectInfo(const string& strSN, const string& strIP, const string& strUser, const string& strPwd);
    virtual int CMD_showtlvsysinfo(IN OUT HYPER_STORAGE_STRU &rstStorageInfo,OUT string &strErrorMsg);

    virtual int CMD_showvisrvginfo(OUT list<QUERY_RVG_INFO_STRU> &,OUT string &){return RETURN_OK;};
    virtual int CMD_showvissecondhostinfo(IN RVG_INFO_STRU &,OUT list<SECONDARY_HOST_INFO_STRU> &,OUT string &){return RETURN_OK;};
    virtual int CMD_showvissecondhostinfo(IN RVG_INFO_STRU &,OUT SECONDARY_HOST_INFO_STRU &,OUT string &){return RETURN_OK;};
    virtual int CMD_stoprvg(IN RVG_INFO_STRU &,OUT string &){return RETURN_OK;};
    virtual int CMD_startrvg(IN RVG_INFO_STRU &,OUT string &){return RETURN_OK;};
    virtual int CMD_showctrinfo(OUT list<CONTROLLER_INFO_STRU> &,OUT string &){return RETURN_OK;};
    
    virtual int CMD_showHostByHostGroup(const string &strHostGroupID, list<CMDHOSTINFO_STRU> &lstHosts);

    //add to support DAR
    virtual int CMD_showInitiatorInfo(const string &initiatorID, const string& type, INITIATOR_INFO& initiatorInfo);
    virtual int CMD_removeHostFromHostGroup(const string& hostID, const string& groupID);
    virtual int CMD_createHost(const string& hostName, CMDHOSTINFO_STRU& hostInfo);
    virtual int CMD_addInitiatorToHost(const string &initiatorID, const string &type, const string &strHostID);
    virtual int CMD_removeInitiatorFromHost(const string &initiatorID, const string &type);
    virtual int CMD_CreateHostGroup(const string& hostGroupName, string &hostGroupID);
    virtual int CMD_addHostsToHostGroup(const string& hostGroupID, const string &host);
    virtual int CMD_createMappingView(const string& name, string &mvID);
    virtual int CMD_associateToMappingview(const string& mvID, const int &type, const string& ID);
    virtual int CMD_showhost(const string& name, OUT CMDHOSTINFO_STRU &rHostInfo);
    
    virtual int CMD_showLunGroupByLUN(const string &strLunID, list<string> &strLunGroupID, int objType);    
    virtual int CMD_showMapbyLunGroup(const string &strLunGroupID, list<string> &lstMapID);
    virtual int CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID);
    virtual int CMD_showHostbyHostGroup(const string &HostGroupID, list<string> &lstHostID);
    virtual int CMD_showHostGroupByHost(const string &strHostID, list<string> &lstHostGroupID);   
    virtual int CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns);
    virtual int CMD_showSnapShotByLUNGroup(const string &strLunGroupID, list<HYIMAGE_INFO_STRU> &lstSnapShots);
    virtual int CMD_showHostByLUN(const string &lunID, list<string> &lstHosts, const int objType); 

    virtual int CMD_showMapbyHostGroup(const string &strHostGroupID, list<string> &lstMapID);
    virtual int CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID);

    virtual int CMD_showLUNGroupByName(const string &strLunGroupName, string &strLunGroupID);
    virtual int CMD_showHostGroupByName(const string &strHostGroupName, string &strHostGroupID);
    virtual int CMD_showMappingByName(const string &strMappingName, string &mapID); 
    virtual int CMD_removeLunGroupFromMap(const string &mapID, const string &lunGroupID);
    virtual int CMD_removeHostGroupFromMap(const string &mapID, const string &hostGroupID);
    virtual int CMD_delMappingByMapID(const string &mapID);

    virtual int CMD_delMapping(const string& mapID, const string& hostGroupID, const string& lunGroupID); 
    virtual int CMD_delLunGroup(const string& lunGroupID, const string& snapshotID, int objType = OBJ_SNAPSHOT);
    virtual int CMD_delHostGroup(const string& hostGroupID, const list<string>& lstHostIDs); 
    virtual int CMD_delHostFromHostGroup(const string& hostGroupID, const string& hostID);

private:
    int CMD_showiscsihostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo);
    int CMD_showfchostport(IN string strHostID, OUT list<HOST_PORT_INFO_STRU> &rlstHostPortInfo);

    int CMD_showLunGroupByMap(const string &strMapID, string &strLunGroupID,CRESTConn *restConn);    
    int CMD_showHostGroupByMap(const string &strMapID, string &strHostGroupID,CRESTConn *restConn);    
    
    int CMD_showLunByLunGroup(const string &strLunGroupID, list<LUN_INFO_STRU> &lstLuns,CRESTConn *restConn);  
    int CMD_showHostGroupByHost(const string &strHostID, string &strHostGrupID); 
    int CMD_showMapByHostGroup(const string &strHostGrupID, string &strMapID);

    int CMD_addLun2LunGroup(const string &strLunID, const string &strLunGroupID, int objType = OBJ_LUN);
    int CMD_CreateLunGroup(const string &lunGroupName, string &lunGroupID);

    int CMD_delLunFromLunGroup(const string &strLunID, const string &strLunGroupID, int objType = OBJ_LUN);

    void TimeChg(IN unsigned long long ullTime,OUT string &strTime);
    void UpdateGroupStatus(IN OUT unsigned int &ruiIntStatus);
    void UpdateModel(IN OUT unsigned int &ruiMode);
    void SnapshotConvertStatus(IN const unsigned int uiHealthStatus, IN OUT unsigned int &ruiStatus);
    void SnapshotConvertType(IN OUT unsigned int &ruiType);
    void UpdateMirrorStatus(IN OUT unsigned int &ruiIntStatus);
    void ConvertMirrorPairStatus(IN OUT unsigned int &ruiIntState);
    void ConvertHMPairStatus(IN OUT unsigned int &ruiIntState);
    void ConvertMirrorSlaveLUNStatus(IN OUT unsigned int &ruiIntState);
    void ConvertHMSlaveLUNStatus(IN OUT unsigned int &ruiIntState);
    void ConvertSrcLUNStatus(IN const unsigned int uiHealthStatus, IN const unsigned int uiRunStatus, IN OUT unsigned int &ruiIntState);
    void ConvertHostPortStatus(IN OUT unsigned int &ruiIntStatus);

private:
    list<pair<string, CRESTConn *>> m_connList;
    CRESTConn *getConn(string &, string &, string &);
};

#endif
