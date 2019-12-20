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

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

/*********************error code ************************/
#define ERROR_COMMAND_NOT_SUPPORTED          100
#define ERROR_LOCALE_NOT_SUPPORTED           101

#define WARN_REPLICATION_IS_PROCESSING       500

#define ERROR_COMMON_XML_READ         0x08025E00    
#define ERROR_COMMON_XML_WRITE        0x08025E01   
#define ERROR_COMMON_XML_MEMORY_ALLOC 0x08025E02   
#define ERROR_COMMON_MEMORY_ALLOC     0x08025E03   
#define ERROR_COMMON_POINTER_NULL     0x08025E04  
#define ERROR_IC_CMD_FAIL             0x08025E06  
#define ERROR_GET_IC_SERVER_PATH      0x08025E07   
#define ERROR_IC_PARSE_RST_FILE       0x08025E08  
#define ERROR_IC_INVALIDATE_PARAM     0x08025E09 
#define ERRPR_IC_INTERNAL_ERROR       0x08025E0A 

 
#define ERRPR_COMMON_USERPWD_ERROR    0x40403285 
#define ERROR_COMMON_OVER_MAX_USER    0x1007212      

#define ERROR_INTERNAL_PROCESS_FAIL      1001
#define ERROR_NO_INBAND                  1002
#define ERROR_GET_HOSTID                 1003
#define ERROR_RECREATE_SNAPSHOT          1004
#define ERROR_ACTIVE_SNAPSHOT            1005
#define ERROR_CREATE_SNAPSHOT            1006
#define ERROR_GET_SNAPSHOT_INFO          1007
#define ERROR_GET_SNAPSHOT_WWN           1008
#define ERROR_INVALIDATE_DEVICE_ID       1009
#define ERROR_INVALIDATE_GROUP_ID        1010
#define ERROR_INVALIDATE_LOCAL_ARRAY_SN  1011
#define ERROR_INVALIDATE_PEER_ARRAY_SN   1012
#define ERROR_INVALIDATE_HYMIRROR_STATUS 1013
#define ERROR_INVALIDATE_SYNC_ID         1014
#define ERROR_SLAVELUN_NOT_MAP           1015
#define ERROR_SLAVELUN_IS_SYNCING        1016
#define ERROR_INVALIDATE_IPADDRESS       1017
#define ERROR_CONNECTSERVER_CODE_LIBCURL 7


#define DEVICE_NOT_EXIST                 2228225  
#define MAP_NULL_MORE_ONE                2228236  
#define ERROR_IC_SERVER_NOT_EXIST        134372869
#define ERROR_NET_SOURCE_INVALID         2228226 

#define ERROR_LUN_NOT_OPERATION                                    50331662 
#define ERROR_SYSTEM_BUSY                                          50331663 
#define ERROR_SYSTEM_ABNORMAL                                      50331671 
#define ERROR_EXCEED_SYSTEM_SPEC                                   50331693 
#define ERROR_SNAPSHOT_EXCEEDED                                    50337536 
#define ERROR_LUN_EXCEEDED                                         50337537 
#define ERROR_LUN_SNAPSHOT_EXCEEDED                                50337538 
#define ERROR_LUN_IN_ANOTHER_SESSION                               50337542 
#define ERROR_LUN_NOT_SNAPSHOT_LUN                                 50337543 
#define ERROR_LUN_NOT_SNAPSHOT_ANOTHER                             50337544 
#define ERROR_NUN_NOT_SOURCE_LUN                                   50337545 
#define ERROR_LUN_STATUS_NOT_OPERATION                             50337547 
#define ERROR_SYSTEM_BUSY_TRY_LATER                                50337548 
#define ERROR_SNAPSHOT_INVAILD_LICENSE                             50337549 
#define ERROR_LUN_STATUS_NOT_OPER                                  50337550 
#define ERROR_LUN_ABNORMAL                                         50337551 
#define ERROR_OBJECT_BUSY                                          50337552 
#define ERROR_DELETE_MAPPED_LUN                                    50337553 
#define ERROR_CHOSE_SAME_LUN                                       50337555 
#define ERROR_DELETE_SNAPSHOT_REFER_ONLINE                         50337561 
#define ERROR_LIMIT_NUMBER_SNAPSHOT                                50337601 
#define ERROR_HOST_HAVE_LUN_MAPPING                                50337799 
#define ERROR_DELETE_NOT_EXIST_MAP                                 50337800 
#define ERROR_REMOTE_LUN_NOT_EXIST                                 50338307 
#define ERROR_REMOTE_NOT_EXIST                                     50338308 
#define ERROR_REMOTE_CONSISTENT_GROUP_NOT_EXIST                    50338309 
#define ERROR_SLAVELUN_NOT_BELONG_REMOTE                           50338313 
#define ERROR_REMOTE_NOT_BELONG_CONSISTENT_GROUP                   50338314 
#define ERROR_MASTER_NOT_OPERATE                                   50338323 
#define ERROR_SLAVE_NOT_OPERATE                                    50338324 
#define ERROR_FORCE_OPERATE_INGOOD_CONDITION                       50338325 
#define ERROR_FORCE_OPERATE_INBAD_CONDITION                        50338326 
#define ERROR_REMOTE_INVALID_LICENSE                               50338327 
#define ERROR_CONTROLLER_STATUS_NOT_ALLOW_OPERATE                  50338328 
#define ERROR_DATA_STATUS_NOT_ALLOW_OPERATE                        50338329 
#define ERROR_REMOTE_STATUS_NOT_ALLOW_OPERATE                      50338330 
#define ERROR_REMOTE_CONSISTENT_GROUP_STATUS_NOT_ALLOW_OPERATE     50338331 
#define ERROR_NOT_OPERATE_MEMBER_IN_REMOTE_CONSISTENT_GROUP        50338332 
#define ERROR_ADD_REMOTE_BELONG_DIFF_ARRAY                         50338338 
#define ERROR_NOT_CONN_REMOTE_LUN                                  50338339 
#define ERROR_NOT_CONN_PEER_CONTROLLER                             50338340 
#define ERROR_LOCAL_ARRAY_BUSY                                     50338341 
#define ERROR_REMOTE_ARRAY_BUSY                                    50338342 
#define ERROR_MASTER_LUN_NOT_TEAM_MEMBER                           50338348 
#define ERROR_MASTER_LUN_ABNORMAL_NOT_OPERATE                      50338349 
#define ERROR_SLAVE_LUN_ABNORMAL_NOT_OPERATE                       50338350 
#define ERROR_MASTER_LUN_ROLLBACK_NOT_OPERATE                      50338352 
#define ERROR_SLAVE_LUN_ROLLBACK_NOT_OPERATE                       50338353 
#define ERROR_CONSISTET_GROUP_FAULT_NOT_OPERATE                    50338354 
#define ERROR_CONSISTET_GROUP_NO_MEMBER                            50338355 
#define ERROR_REMOTE_CONSIST_GROUP_NOT_EXIST                       50338390 
#define ERROR_ANOTHER_REMOTE_STATE_NOT_OPERATE                     50338395 
#define ERROR_REPEAT_MAPPING                                       50332704 
#define ERROR_DELETE_NFS_NOT_EXIST                                 1077939726 

#define RETURN_SSP(code)        (0x01000000 + code) 
#define RETURN_SMP(code)        (0x02000000 + code)
#define RETURN_CONTROL(code)    (0x03000000 + code)
#define RETURN_ANDES_ALPS(code) (0x04000000 + code) 
#define RETURN_PRODUCT(code)    (0x05000000 + code)

#define RETURN_MSG_CODE_ITEM_ERROR      RETURN_SSP(0x300)    

#define RETURN_ECONF_ITEM_FULL         RETURN_SSP(0x800)  

#define RETURN_DB_TABLE_NAME_INVALID    RETURN_SMP(0x0)    
#define RETURN_DB_FIELD_NAME_INVALID    RETURN_SMP(0x1)  
#define RETURN_DB_CAPACITY_NOT_ENOUGH   RETURN_SMP(0x2) 
#define RETURN_DB_FIELD_TYPE_INVALID    RETURN_SMP(0x3) 

#define RETURN_DB_NO_MEDIUM             RETURN_SMP(0xB1)    
#define RETURN_DB_START_MASTER_FAILED   RETURN_SMP(0xB2)    
#define RETURN_DB_START_SLAVE_FAILED    RETURN_SMP(0xB3)   

#define RETURN_DB_VERIFY_CRC_ERR              RETURN_SMP(0x9) 
#define RETURN_DB_VERIFY_DATA_INCOMPATIBLE    RETURN_SMP(0xA)

#define RETURN_DB_PID_REG_REPEAT        RETURN_SMP(0xB)      
#define RETURN_DB_CALLBACK_FUNC_NULL    RETURN_SMP(0xC)      
#define RETURN_DB_MOD_NAME_REG_REPEAT   RETURN_SMP(0xD)       
#define RETURN_DB_MOD_NAME_LEN_ERR      RETURN_SMP(0xE)     

#define RETURN_DB_MAGIC_ERROR            RETURN_SMP(0xA1)  
#define RETURN_DB_TABLE_NOT_FOUND        RETURN_SMP(0XA3)  
#define RETURN_INVALID_DISK_NO           RETURN_SMP(0xA4)  
#define RETURN_DB_IMPORT_CFG_UPGRADE_FAIL  RETURN_SMP(0xA5)  

#define RETURN_DB_BUTT                  RETURN_SMP(0xFF)     

enum RETURN_VALUE_E
{
     
    RETURN_VALUE_COMMON_BEGIN = 0x00010000,
    RETURN_PARAM_ERROR,              
    RETURN_VALUE_CACHE_BEGIN = 0x00020000,
    RETURN_VALUE_RAID_BEGIN = 0x00030000,
    RETURN_COMMUNICATION_ERR,                            
    RETURN_VALUE_DB_BEGIN = 0x00040000,
    RETURN_VALUE_TGT_BEGIN = 0x00050000,
    RETURN_LMP_TGT_NOT_START, 
    RETURN_DEV_PEER_RETURN_ERROR,  
    RETURN_DEV_FC_DRIVER_NO_LOAD_ERROR,  
    RETURN_DEV_NET_NO_SET,  
    RETURN_DEV_NOT_GET_IBC_CONF_MAN_NET_MSG,     
    RETURN_DEV_SERIAL_MSG_FAIL,                  
     
    RETURN_DEV_SEND_PRE_FAIL,                    
    RETURN_DEV_SEND_PACKET_FAIL,                 
    RETURN_DEV_BMC_PROGRAM_ERROR,                

    RETURN_DEV_MAINCPLD_ISP_JBC_ERR,        
    RETURN_DEV_MAINCPLD_ISP_PRO_ERR ,        
    RETURN_DEV_STBCPLD_ISP_JBC_ERR ,         
    RETURN_DEV_STBCPLD_ISP_PRO_ERR ,         
     
    RETURN_DEV_GET_IP_MAC_ERROR,                
    RETURN_DEV_SET_IP_MAC_ERROR ,               

    RETURN_SYS_UNSUPPORT_SWAP_FOR_SINGLE_CONTROL = 0x000c0005,                         
    RETURN_SYS_CMD_COM_STAUS_CONFLICT = 0x000c001d, 
     
    RETURN_SYS_SEND_OM_MSG_FAIL = 0x000c001f,  
    RETURN_SYS_SEND_OM_ACK_MSG_FAIL = 0x000c0020,  

    RETURN_SYS_IN_UPDATE_STATUS = 0x000c0022,   
    RETURN_SYS_NEED_NOT_UPDATE = 0x000c0023,   
    RETURN_SYS_SYSTEM_DISK_IS_ABNORMAL = 0x000c0031, 
    RETURN_SYS_MASTER_NOT_CONFIRM = 0x000c0032, 

    RETURN_ISCSI_CHAP_USER_HAS_BEEN_ADDED = 0x000d0100,                      
    RETURN_NIC_IP_FORMAT_BAD = 0x000d1000,                                               
    RETURN_NIC_NOT_EXIST,                                                            
    RETURN_NIC_MAC_BAD                                                      
     
};

#define RETURN_SLOT_NOT_ISOLATED  0x3000942   
#define RETURN_SLOT_ISOLATED   0x3000941   
#define RETURN_INVALID_IPV6_GATEWAY   50334042     
#define RETURN_DEV_FRAME_SLEEP_NOT_ALLOW_DELETE_SUBRACK   50334040  

enum  OLD_RETURN_VALUE
{     
    RETURN_NO_MEM =                                0x3000001,  
    RETURN_END_OF_TABLE =                          0x1,   
    RETURN_FILE_OPEN_FAILURE =                     0x3000004,  
    
    RETURN_ONLY_ONE_CONTROLLER_IN_RAID =           0x3000006,  
    RETURN_MORE_ONE_DISK_TYPE  =                   0x3000007,  
    RETURN_FILE_SAVE_FAILURE =                     0x3000008,  
    RETURN_DOUBT =                                 0x3000009,  
    RETURN_MAGIC_ERROR =                           0x300000A,  
    RETURN_VER_ERROR =                             0x300000B,  
    RETURN_NULL_PTR =                              0x300000C,  

    RETURN_SEND_MSG_TO_PEER_ERROR =                0x300000D,  
    RETURN_LUN_STATE_NOT_OPERATION =               0x300000E,  
    RETURN_LOCK_RESOURCE_FAIL =                    0x300000F,  
    RETURN_IO_BUSY_NEED_RETRY =                    0x3000010,  
    RETURN_LARGEST =                               0x3000011,  
    RETURN_SMALLEST =                              0x3000012,  
    RETURN_IBS_SEND_ERROR =                        0x3000013,  
    RETURN_IBC_SEND_ERROR =                        0x3000014,  
    RETURN_INVALID_LICENSE_OPCODE_ERROR =          0x3000015,  
    RETURN_COMMAND_SUCCESS =                       0x3000016,  
    RETURN_SYS_ABNORMAL_OPERATION_FAILED =         0x3000017,  

    RETURN_RAID_LEVEL_INVALID               =      0x3000020,  
    RETURN_PARAM_ALARM_INVALID              =      0x3000021,  
    RETURN_PARAM_NAME_INVALID               =      0x3000022,  
    RETURN_PARAM_SIZE_INVALID               =      0x3000023,  
    RETURN_PARAM_DISKLIST_INVALID           =      0x3000024,  
    RETURN_DISKNUM_EXCEED_SPECIFICATION     =      0x3000025,  
    RETURN_PARAM_DISK_REPEAT                =      0x3000026,  
    RETURN_PARAM_DISK_IS_VAULT              =      0x3000027,  
    RETURN_PARAM_DISK_UNFIT                 =      0x3000028,  
    RETURN_PARAM_WRITETYPE_INVALID          =      0x3000029,  
    RETURN_PARAM_MIRRORSWITCH_INVALID       =      0x300002A,  
    RETURN_PARAM_PREFETCHTYPE_INVALID       =      0x300002B,  
    RETURN_PARAM_PREFETCHVALUE_INVALID      =      0x300002C,  
    RETURN_PARAM_EXCEED_SPECIFICATION       =      0x300002D,  
     
    RETURN_OK_CM_WRITE_OK_AND_4K_ADDED =           0x3000100,  
    RETURN_WRITE_CONFLICT =                        0x3000101,  
    RETURN_HIT_ERROR =                             0x3000102,  
    RETURN_HIT_IN_PREFETCH_LIST =                  0x3000103,  
    RETURN_NO_HIT_IN_PREFETCH_LIST =               0x3000104,  
    RETURN_DATA_MAGIC_ERROR =                      0x3000105,  
    RETURN_ADDR_ERROR =                            0x3000106,  
    RETURN_PAGE_SUM_ERROR =                        0x3000107,  
    RETURN_RACK_OFFLINE =                          0x3000108,  
    RETURN_RETRY =                                 0x3000109,  
    RETURN_LUN_FAULT =                             0x300010A,  
    RETURN_OVER_RETRY =                            0x300010B,  
    RETURN_HIT_IN_CACHE =                          0x300010C,  
    RETURN_NO_HIT_IN_CACHE =                       0x300010D,  
    RETURN_DESTAGE_CONFLICT =                      0x300010E,  
    RETURN_WAIT =                                  0x300010F,  
    RETURN_HALF_FAIL =                             0x3000110,  
    RETURN_FINISH =                                0x3000111,  
    RETURN_WAIT_ALLOC =                            0x3000112,  
    RETURN_NO_PAGE =                               0x3000113,  
    RETURN_NO_SGL =                                0x3000114,  
    RETURN_NO_SLAB =                               0x3000115,  
    RETURN_DESTAGE_TOO_OFTEN =                     0x3000116,  
    RETURN_POWER_LOST =                            0x3000117,  
   
    RETURN_LUN_REPEAT_FMT =                        0x3000200,  
    RETURN_NO_EXIST_DISK =                         0x3000201,  
    RETURN_NO_EXIST_RAID =                         0x3000202,  
    RETURN_NO_EXIST_LUN =                          0x3000203,  
    RETURN_LUN_BUSY_CANNOT_DEL =                   0x3000204,  
    RETURN_LUN_CANCEL_INEXISTENCE_LUN =            0x3000205,  
    RETURN_ILLEGAL_COPYBACK_CMD =                  0x3000206,  
    RETURN_COPYBACK_ALREADY_IN_QUEUE =             0x3000207,  
    RETURN_ILLEGAL_RECON_CMD =                     0x3000208,  
    RETURN_RECON_ALREADY_IN_QUEUE =                0x3000209,  
    RETURN_DISK_TYPE_ERROR =                       0x300020A,  
    RETURN_RAID_STATUS_ERROR =                     0x300020B,  
    RETURN_DISK_STATUS_ERROR =                     0x300020C,  
    RETURN_SPARE_DISK_USED =                       0x300020D,  
    RETURN_DISK_SIZE_ERROR =                       0x300020E,  
    RETURN_RAID_HAS_RECON_OR_COPYBACK =            0x300020F,  
    RETURN_DISK_NOT_MATCH =                        0x3000210,  
    RETURN_DISK_CANNOT_COPYBACK =                  0x3000211,  
    RETURN_RAID_HAS_LUN_CANNOT_DEL =               0x3000212,  
    RETURN_LUN_STATUS_ERROR =                      0x3000213,  
    RETURN_ILLEGAL_OPCODE =                        0x3000214,  
    RETURN_RAID_USE_UP =                           0x3000215,  
    RETURN_LUN_USE_UP =                            0x3000216,  
    RETURN_SUBRACK_CONNOT_DOWN =                   0x3000217,  
    RETURN_RAID_HAS_RECON_OR_COPYBACK_CANNOT_DEL = 0x3000218,  
    RETURN_RAID_IN_POWEROFF_FRAME_CANNOT_DEL =     0x3000219,  
    RETURN_LUN_IN_POWEROFF_FRAME_CANNOT_DEL =      0x300021A,  
    RETURN_LUN_IS_EXTENDING =                      0x300021B,  
    RETURN_LUN_EXTEND_SPACE_NOT_ENOUGH =           0x300021C,  
    RETURN_REPEAT_RAIDNAME =                       0x300021D,  
    RETURN_LUN_FORMATED =                          0x300021E,  
    RETURN_LUN_FORMATED_FAULT =                    0x300021F,  
    RETURN_LUN_FORMATING =                         0x3000220,  
    RETURN_HAVE_HALFLIFE_DISK =                    0x3000221,  
    RETURN_GET_STRIPE_LOCK =                       0x3000222,  
    RETURN_NOT_GET_STRIPE_LOCK =                   0x3000223,  
    RETURN_LOCK_OK =                               0x3000224,  
    RETURN_MUTEX =                                 0x3000225,  
    RETURN_RAIDGROUP_ERROR =                       0x3000226,  
    RETURN_HASH_DUPLICATE =                        0x3000227,  
    RETURN_COPYBACK_CMD_UNEXIST =                  0x3000228,  
    RETURN_STRIPE_COPYBACK_COMPLETE =              0x3000229,  
    RETURN_STRIPE_COPYBACK_UNCOMPLETE =            0x300022A,  
    RETURN_RECON_CMD_UNEXIST =                     0x300022B,  
    RETURN_STRIPE_RECON_COMPLETE =                 0x300022C,  
    RETURN_STRIPE_RECON_UNCOMPLETE =               0x300022D,  
    RETURN_RAID_HAVED_FRAME_EVENT =                0x300022E,  
    RETURN_REDUNDANTCOPY_ALREADY_IN_QUEUE =        0x300022F,  
    RETURN_REDUNDANTCOPY_CMD_UNEXIST =             0x3000230,  
    RETURN_RAID_HIBERNATE_STATUS_ERROR =           0x3000231,  
    RERURN_RAID_NEARLINE_STATUS_ERROR =            0x3000232,  
    RETURN_TIME_PARAM_ERROR =                      0x3000233,  
    RETURN_WAKE_ALL_DISKS_FAILED =                 0x3000234,  
    RETURN_HIBERNATE_FAILED =                      0x3000235,  
    RETURN_APPCM_DISK_NUM_USE_UP =                 0x3000236,  
    RETURN_APPCM_DISK_CAPACITY_USE_UP =            0x3000237,  
    RETURN_CONTAIN_COFFER_DISK =                   0x3000238,  
    RETURN_LICENSE_LIMITED =                       0x3000239,  
    RETURN_RAID_STATUS_ABNORMAL =                  0x3000240,  

    RETURN_MEDIUM_ERROR =                          0x3000241,  
    RETURN_REPAIR_OK =                             0x3000242,  

    RETURN_BST_ERROR =                                0x3000243,
    RETURN_DIVIDE_STRIPE =                         0x3000244, 
    RETURN_INSERT_BST =                            0x3000245,
    RETURN_FORCE_WRITE =                           0x3000246,  
    RETURN_REPAIR_FAIL =                           0x3000247,    
    RETURN_CONTINUE =                              0x3000248,
    RETURN_FORMATTING_LUN_EXIST_NOT_ALLOW_UPGRADE = 0x3000249,  

    RETURN_CHG_VAULT_DISK_LOGIC_TYPE =              0x300026F,   
    RETURN_DISK_LOGIC_TYPE_ERROR =                   0x3000270,   
     
    RETURN_STATUS_RECONSTRUCT_ERROR =              0x300027B,   
    RETURN_STATUS_COPYBACK_ERROR =                 0x300027C,   
    RETURN_STATUS_REDUNDANT_COPY_ERROR =           0x300027D,   
    RETURN_STATUS_DYNAMIC_ERROR =                  0x300027E,   
    RETURN_STATUS_LUN_VERIFY_ERROR =               0x300027F,   
    RETURN_STATUS_BACKSCAN_ERROR =                 0x3000280,   
     
    RETURN_RAID_SPACE_NOT_ENOUGH =                 0x3000300,  
 
    RETURN_BSP_ETH_DEV_ERR =                       0x3000700,  
    RETURN_BSP_ETH_SEND_ERR =                      0x3000701,  
    RETURN_BSP_ETH_REC_ERR =                       0x3000702,  
    RETURN_BSP_ETH_LOOPBACK_ERR =                  0x3000703,  
    RETURN_BSP_NVRAM_WRITE_ERR =                   0x3000704,  
    RETURN_BSP_SYSTIME_ERR =                       0x3000705,  
    RETURN_BSP_RST_INFO_ERR =                      0x3000706,  

    RETURN_CLI_CMDFORMAT_ILLEGAL =                 0x3000800,  
    RETURN_CLI_CMDNAME_INVALID =                   0x3000801,  
    RETURN_CLI_CMDEXPLAIN_ERROR =                  0x3000802,  
    RETURN_CLI_SES_FILENAME_ILLEGAL =              0x3000803,  
    RETURN_CLI_OPEN_HELPFILE_FAILED =              0x3000804,  
    RETURN_CLI_IP_IN_SAME_SEGMENT =                0x3000805,  
    RETURN_CLI_IP_IN_DIFF_SEGMENT =                0x3000806,  

    RETURN_DEV_NOT_EXIST =                         0x3000900,  
    RETURN_DEV_TYPE_NOT_CONSISTENT =               0x3000901,  
    RETURN_DEV_UNKNOWN_TYPE =                      0x3000902,  
    RETURN_DEV_END  =                              0x3000903,  
    RETURN_DEV_BE_UPGRADING =                      0x3000904,  
    RETURN_DEV_CANNOT_UPGRADE =                    0x3000905,  
    RETURN_DEV_UPGRADE_FAIL =                      0x3000906,  
    RETURN_DEV_SAME_VERSION =                      0x3000907,  
    RETURN_DEV_ILLEGAL_OPCODE =                    0x3000908,  
    RETURN_DEV_MUST_BE_MASTER =                    0x3000909,  
    RETURN_DEV_TEST_FAIL =                         0x300090A,  
    RETURN_DEV_FC_PORT_IN_USE =                    0x300090B,  
    RETURN_DEV_SUBRACK_NOEXIST =                   0x300090C,  
    RETURN_DEV_SUBRACK_REPEATED =                  0x300090D,  
    RETURN_DEV_UPGRADE_CONDITION_NOT_MEET =        0x300090E,  
    RETURN_DEV_SES_NOT_GOOD =                      0x300090F,  
    RETURN_DEV_RP_NOT_ALLOW_DELETE_SUBRACK =       0x3000910,  
    RETURN_DEV_TEST_IN_HANDLE =                    0x3000911,  
    RETURN_DEV_TEST_BURNIN_UNDO =                  0x3000912,  
    RETURN_DEV_TEST_BURNIN_NOT_COMPLETE =          0x3000913,  
    RETURN_DEV_FILE_SIZE_TOO_BIG =                 0x3000914,  
    RETURN_DEV_ENCRYPT_UNABLED =                   0x3000915,  
    RETURN_Demo_USERMGR_USER_NOT_FOUND =           0x3000916,  
    RETURN_Demo_USERMGR_CMDCODE_NOT_EXPECTED =     0x3000917,  
    RETURN_Demo_USERMGR_GET_MGR_INSTANCE_FAIL =    0x3000918,  
    RETURN_Demo_USERMGR_GET_USER_INFO_FAIL =       0x3000919,  
    RETURN_Demo_USERMGR_CMD_JUDGE_FAIL =           0x300091A,  
    RETURN_Demo_USERMGR_INVALID_PARAMETER =        0x300091B,  
    RETURN_Demo_USERMGR_USER_TO_ADD_EXIST =        0x300091C,  
    RETURN_Demo_USERMGR_USER_OPERATION_NOT_SUPPORTED = 0x300091D,  
    RETURN_Demo_USERMGR_USER_OPERATION_FAILED =    0x300091E,  
    RETURN_Demo_USERMGR_USER_LOGIN_FAILED =        0x300091F,  

    RETURN_DEV_FRAME_ONLINE_NOT_ALLOW_DELETE_SUBRACK =     0x3000925,  
    RETURN_DEV_SYNC_FAILED_NOT_ALLOW_DELETE_SUBRACK  =       0x3000926,    
    RETURN_SAME_AS_IBC    =   0x300092A,                
    RETURN_ETHCONF_DB_FULL  =     0x3000930,            
    RETURN_OPERATION_REFUSED =     0x3000931,        
    RETURN_DEV_IS_ABNORMAL = 0x3000932,  
    RETURN_CONTROLLER_NOT_EXIST = 0x3000933,   
    RETURN_DISK_IS_VAULT =  0x3000935, 

    RETURN_FC_PORT_MODE_CANNOT_MODIFIED = 0x300093A,  
    RETURN_SPEED_NOT_SUPPORTED = 0x300093B,  
    RETURN_EPL_LINK_BANDWIDTH_EXCEED = 0x300094B,     
     
    RETURN_SYS_SEND_IBCMSG_ERROR =                 0x3000B00,  
    RETURN_SYS_CANNOT_MODIFY =                     0x3000B01,  
    RETURN_SYS_UNSUPPORT_SWAP =                    0x3000B02,  
    RETURN_SYS_CONTROLLER_NOT_MASTER =             0x3000B03,  
    RETURN_SYS_NEED_SAFTMODE =                     0x3000B04,  
    RETURN_SYS_VERIFY_ERROR =                      0x3000B05,  
    RETURN_SYS_NEED_CANCEL =                       0x3000B06,  
    RETURN_SYS_UNZIP_CONFIG_FAIL =                 0x3000B07,  
    RETURN_SYS_OPEN_DB_DATA_FAIL =                 0x3000B08,  
    RETURN_SYS_CLOSE_FILE_FAIL =                   0x3000B09,  
    RETURN_SYS_VERIFY_DB_MAGIC_FAIL =              0x3000B0A,  
    RETURN_SYS_VERIFY_DB_LENGTH_FAIL =             0x3000B0B,  
    RETURN_SYS_WRITE_COFFER_FAIL =                 0x3000B0C,  
    RETURN_SYS_CALL_USER_SHELL_FAIL =              0x3000B0D,  
    RETURN_SYS_CONFIG_SAVE_FAIL =                  0x3000B0E,  
    RETURN_SYS_GET_DEV_CONFIG_FAILED =             0x3000B0F,  
    RETURN_SYS_GET_DB_CONFIG_FAILED =              0x3000B10,  
    RETURN_SYS_CREATE_DB_CONFIG_FILE_FAIL =        0x3000B11,  
    RETURN_SYS_WRITE_FILE_FAIL =                   0x3000B12,  
    RETURN_SYS_VAULT_READ_DB_FAIL =                0x3000B13,  
    RETURN_SYS_CONFIG_MODULE_INIT_FAIL =           0x3000B14,  
    RETURN_SYS_READ_FILE_FAIL =                    0x3000B15,  
    RETURN_SYS_SET_LUN_REPEAT =                    0x3000B16,  
    RETURN_SYS_SINGLE_CONTROLLER =                 0x3000B17,  
    RETURN_SYS_MASTER_CAN_NOT_UPGRADE =            0x3000B18,  
    RETURN_SYS_MIRROR_LINK_FAULT =                 0x3000B19,  
    RETURN_SYS_FORBID_MODIFY =                     0x3000B1A,  
    RETURN_SYS_CMD_SEND_FREQUENT =                 0x3000B1B,  
    RETURN_SYS_CMD_SYS_STAUS_CONFLICT =            0x3000B1C,  
    RETURN_SYS_SINGLE_CONNECTION =                 0x3000B1D,  

    RETURN_SYS_CTRLER_ALREADY_ONLINE =             0x3000B1E,  
    RETURN_SYS_SINGLE_FAN =                        0x3000B1F,  
    RETURN_SYS_SINGLE_POWER =                      0x3000B20,  
    RETURN_SYS_UPGRADE_FAULT =                     0x3000B21,  
    RETURN_SYS_IBC_FAULT =                         0x3000B22,  
    RETURN_SYS_SLAVE_CAN_NOT_UPGRADE_SYNC =        0x3000B23,  
    RETURN_UPDATE_VERIFY_PACKAGE_FAULT =           0x3000B24,  
    RETURN_UPDATE_SAME_VERSION =                   0x3000B25,  
    RETURN_UPDATE_SLAVE_POWERON =                  0x3000B26,  

    RETURN_SYS_COFIG_IMPORT_EXPORT_PROHIBITED =    0x3000B27,  
    RETURN_SYS_LICENSE_REBOOT =                    0x3000B28,  
    RETURN_SYS_LUN_ID_USE_UP =                     0x3000B29,  
    RETURN_SYS_WAIT_LOCK =                         0x3000B2A,  
    RETURN_SYS_CMD_NEED_RETRY =                    0x3000B2B,  

    RETURN_SYS_DEV_FAULT =                         0x3000B2C,  
    RETURN_SYS_UPGRADE =                           0x3000B2D,  

    RETURN_SYS_IP_USED =                           0x3000B2E,  
    RETURN_LUN_SLEEP =                             0x3000B2F,  
    RETURN_SYS_IO_HOLDED =                         0x3000B30,  
    RETURN_SYS_CONTROLLER_IS_DUAL =                0x3000B31,  
    RETURN_SYS_LUN_HAVE_IO =                       0x3000B36,  
    
     
    RETURN_ISCSI_IP_CONFLICT =                     0x3000C00,  
    RETURN_ISCSI_SUBNET_CONFLICT =                 0x3000C01,  
    RETURN_ISCSI_SUBNET_INCONSISTENT =             0x3000C02,  
    RETURN_ISCSI_PORT_IN_USE =                     0x3000C03,  
    RETURN_ISCSI_WAIT_IP_CONFIG =                  0x3000C04,  
    RETURN_ISCSI_CHAP_USER_NAME_ERROR =            0x3000C05,  
    RETURN_ISCSI_CHAP_PASSWORD_ERROR =             0x3000C06,  
    RETURN_ISCSI_CHAP_REACH_MAX_USER =             0x3000C07,  
    RETURN_ISCSI_CHAP_REACH_INITIATOR_MAX_USER =   0x3000C08,  
    RETURN_ISCSI_CHAP_USER_EXIST =                 0x3000C09,  
    RETURN_ISCSI_CHAP_USER_NOT_EXIST =             0x3000C0A,  
    RETURN_ISCSI_CHAP_INITIATOR_NOT_EXIST =        0x3000C0B,  
    RETURN_ISCSI_CHAP_NULL_ERROR =                 0x3000C0C,  
    RETURN_ISCSI_CHAP_TIME_ERROR =                 0x3000C0D,  
    RETURN_ISCSI_CHAP_INITIATOR_NAME_ERROR =       0x3000C0E,  
    RETURN_ISCSI_CHAP_INTIIATOR_ALREADY_LINKUP =   0x3000C0F,  
    RETURN_ISCSI_IP_NOT_EXIST =                    0x3000C10,  
    RETURN_ISCSI_DESTINATION_AND_MASK_INCONSISTENT  = 0x3000C11,  
    RETURN_ISCSI_IP_BROADCAST_IP =                 0x3000C12,  
    RETURN_ISCSI_IP_NETWORK_IP =                   0x3000C13,  
    RETURN_ISCSI_CONNECT_IP_ERROR =         0x3000C1C,  

     
    RETURN_FCDRV_NOT_FOUND_CARD =                  0x3000D00,  
    RETURN_FCDRV_CARD_ERROR_POSITION =             0x3000D01,  
    RETURN_FCDRV_NO_FCP_EXCHG =                    0x3000D02,  
    RETURN_FCDRV_NO_TD_EXCHG =                     0x3000D03,  
    RETURN_FCDRV_NO_TD_ESGL =                      0x3000D04,  
    RETURN_FCDRV_NO_TGT_CMD =                      0x3000D05,  
    RETURN_FCDRV_RUNING_ABNORMAL =                 0x3000D06,  
  
    RETURN_SOCKET_POWER_LOST =                     0x3000E00,  
    RETURN_INVALID_MODULE =                        0x3000E01,  
    RETURN_SOCKET_EXCUTE_TIME_OUT =                0x3000E02,  

    RETURN_USR_OTHERUSR_ALREADY_LOGIN =            0x3000500,  
    RETURN_USR_USRNAMEORPASSWD_ERROR =             0x3000501,  
    RETURN_USR_SYSTEM_LOCKED =                     0x3000502,  
    RETURN_USR_OVER_MAX_IPNUM =                    0x3000503,  

    RETURN_EPL_ARRAY_NOT_EXIST =                   0x3000F00,  
    RETURN_EPL_ARRAY_NOT_CONFIG_FOR_HYPERMIRROR =  0x3000F01,  
    RETURN_EPL_ARRAY_CONFIG_FOR_HYPERMIRROR =      0x3000F02,  
    RETURN_EPL_LINK_CONFIG_FOR_HYPERMIRROR =       0x3000F03,  
    RETURN_EPL_LINK_NOT_EXIST =                    0x3000F04,  
    RETURN_EPL_LINK_IS_DOWN =                      0x3000F05,  
    RETURN_EPL_REACH_MAX_LINK =                    0x3000F06,  
    RETURN_EPL_LINK_NOT_MATCH_WITH_ARRAY =         0x3000F07,  
    RETURN_EPL_NOTMATCH_WITH_PRIOR =               0x3000F08,  
    RETURN_EPL_CONN_MODE_NOT_ACCORD =              0x3000F09,  
    RETURN_EPL_REMOTE_SETTING_FAILED =             0x3000F0A,  
    RETURN_EPL_REMOTE_DEL_FAILED =                 0x3000F0B,  
    RETURN_EPL_CONN_IS_BEING_USED =                0x3000F0C,  
    RETURN_EPL_REACH_MAX_CONN =                    0x3000F0D,  
    RETURN_EPL_NO_LICENSE =                        0x3000F0E,  
    RETURN_EPL_ARRAY_IS_DOWN =                     0x3000F0F,  
    RETURN_EPL_DEL_CONN_FAILURE =                  0x3000F10,  
    RETURN_EPL_LINK_NOT_CONFIG_FOR_HYPERMIRROR =   0x3000F11,  
    RETURN_EPL_LINK_TYPE_NOT_MATCH =               0x3000F12,  
    RETURN_EPL_ONE_LINK_CANNOT_DEL =               0x3000F13,  
    RETURN_EPL_3RD_LUN_NOT_EXITS =                 0x3000F14,  
    RETURN_EPL_LINKTYPE_NOTMATCH_WITH_PRIOR =      0x3000F15,  
    RETURN_EPL_REPEAT_CONFIG_ONE_CONTROLLER =      0x3000F16,  
    RETURN_NEED_PROCESS_IN_OTHER_SIDE =            0x3000F17,  
     
    RETURN_ISCSIINI_SET_NETCFG_ERROR =             0x3001000,  
    RETURN_ISCSIINI_NOT_STARTED =                  0x3001001,  
    RETURN_ISCSIINI_SAME_INFO =                    0x3001002,  
    RETURN_ISCSIINI_NAME_CLASH =                   0x3001003,  
    RETURN_ISCSIINI_ADDR_CLASH =                   0x3001004,  
    RETURN_ISCSIINI_CONN_ERROR =                   0x3001005,  
    RETURN_ISCSIINI_SPACE_NOT_ENOUGH =             0x3001006,  
    RETURN_ISCSIINI_TARGET_NOT_EXIST =             0x3001007,  
    RETURN_ISCSIINI_TARGET_BE_USED =               0x3001008,  
    RETURN_ISCSIINI_IBC_FUN_ID_ERROR =             0x3001009,  
    
    RETURN_DYNAMIC_CMD_OVERFLOW =                  0x3001100,  
    RETURN_DYNAMIC_COPYING_RUNNING =               0x3001101,  
    RETURN_DYNAMIC_RECONING_RUNNING =              0x3001102,  
    RETURN_DYNAMIC_FORMATING_RUNNING =             0x3001103,  
    RETURN_DYNAMIC_CAPACTITY_LACK =                0x3001104,  
    RETURN_DYNAMIC_NONEED =                        0x3001105,  
    RETURN_DYNAMIC_NOEXEC =                        0x3001106,  
    RETURN_DYNAMIC_QUERY_CMD =                     0x3001107,  
    RETURN_DYNAMIC_DISKS_OVERFLOW =                0x3001108,  
    RETURN_DYNAMIC_DISKSNUM_ERROR =                0x3001109,  
    RETURN_DYNAMIC_IS_RUNNING =                    0x3001110,  
    
    RETURN_VERIFY_LUN_INEXISTENCE =                0x3001200,  
    RETURN_VERIFY_LUN_VIRTUAL_TYPE =               0x3001201,  
    RETURN_VERIFY_ITEM_BAD_STATUS =                0x3001202,  
    RETURN_VERIFY_SERVICE_STOP =                   0x3001203,  
    RETURN_VERIFY_CANNOT_VERIFY =                  0x3001204,  
    RETURN_VERIFY_AUTO_EXIST =                     0x3001205,  
    RETURN_VERIFY_AUTO_STOP =                      0x3001206, 
    RETURN_VERIFY_NO_MANUAL =                      0x3001207,  
    RETURN_VERIFY_NO_RAIDLEVEL =                   0x3001208,  
    RETURN_VERIFY_RECORD_INVALID =                 0x3001209,  
    RETURN_VERIFY_EXIST =                          0x300120A,  
    RETURN_VERIFY_REPAIRE_OVERFLOW =               0x300120B,  
    RETURN_VERIFY_AUTOREPAIRED =                   0x300120C, 
    RETURN_VERIFY_INCONSISTENT =                   0x300120D,  
    RETURN_VERIFY_NOT_VERIFIED =                   0x300120E,  
    RETURN_VERIFY_NOT_COMPLETE =                   0x300120F,  
    RETURN_VERIFY_NOT_MANU_VERIFING =              0x3001210,  
     
    RETURN_COMMSRV_SYNC_FAIL =                     0x3001300,  
    RETURN_COMMSRV_NEED_WRITE_VAULT =              0x3001301,  
    RETURN_COMMSRV_SYNC_OK =                       0x3001302,  
    RETURN_COMMSRV_LUN_BITMAP_FORMATTED =          0x3001303,  
    RETURN_COMMSRV_LUN_BITMAP_UNFORMATTED =        0x3001304,  
    RETURN_COMMSRV_LUN_BITMAP_UNEXIST =            0x3001305,  
    RETURN_COMMSRV_LUN_FORMATTING_FORBID_IMPORT = 0x3001306,  
    RETURN_COMMSRV_RESOURCE_LACK_FORBID_EXPORT = 0x3001307,  
   
    RETURN_EXPAND_EXCEED_MAX_SLAVE_LUN_NUM =       0x3001400,  
    RETURN_EXPAND_NOT_JOINT_LUN =                  0x3001401,  
    RETURN_EXPAND_NOT_SLAVE_LUN =                  0x3001402,  
    RETURN_EXPAND_NOT_SLAVE_LUN_OF_THE_JOINT_LUN = 0x3001403,  
    RETURN_EXPAND_NOT_LAST_SLAVE_LUN =             0x3001404,  
   
    RETURN_VAULT_RECON_DUET =                      0x3001500,  
    RETURN_VAULT_BLOCK_NOT_HIT =                   0x3001501,  
    RETURN_VAULT_NO_SPACE =                        0x3001502,  
    RETURN_VAULT_REQ_SUSPENDED =                   0x3001503,  
    RETURN_VAULT_REQ_NOT_SUSPEND =                 0x3001504,  
     
    RETURN_RSS_NOT_ALLOW_PROCESS_ON_BSTLUN  =      0x300160C, 

    RETURN_SNAP_RECORD_ALREADY_EXIST =             0x300171B,  
    RETURN_SNAP_WINDOW_IS_FULL =                   0x300171C,  
    RETURN_SNAP_CHUNK_ROLLING =                    0x300171D,  
    RETURN_SNAP_CHUNK_NOT_ROLL =                   0x300171E,  
    RETURN_SNAP_CHUNK_ROLLED =                     0x300171F,  
    RETURN_SNAP_CHUNK_NOT_EXIST =                  0x3001720,  
    RETURN_SNAP_LATER =                            0x3001721,  
    RETURN_SNAP_EARLIER =                          0x3001722,  
    RETURN_SNAP_EQUAL =                            0x3001723,  
    RETURN_SNAP_COMPARE_GREAT =                    0x3001724,  
    RETURN_SNAP_COMPARE_LESS =                     0x3001725,  
    RETURN_SNAP_COMPARE_EQUAL =                    0x3001726,  
    RETURN_SNAP_NO_ENTRY_RESOURCE =                0x3001727,  
    RETURN_SNAP_POOL_LUN_INVALID =                 0x3001728,  
    RETURN_SNAP_NO_RESOURCE=                       0x3001729,  
    
    RETURN_DHA_INVALID_PARAM = 0x3001D01,  
    RETURN_DHA_SRV_CLOSED = 0x3001D02,  
    RETURN_DHA_NO_FILES_EXIST = 0x3001D03,   
    RETURN_DHA_EXPORT_USR_EXIST  = 0x3001D04,  
    
    RETURN_TP_DISKNUM_LIST_UNMATCH =               0x3002000,  
    RETURN_TP_DISKNUM_LEVEL_UNMATCH =              0x3002001,  
    RETURN_TP_RAID_MIRRORDISK_INVALID =            0x3002002,  
    RETURN_TP_MORE_ONE_DISK_TYPE =                 0x3002003,  
    RETURN_TP_POOL_STATE_INVALID =                 0x3002004,  
    RETURN_TP_POOL_HAS_LUN =                       0x3002005,  
    RETURN_TP_LUN_CREATE_NO_CAPACITY =             0x3002006,  
    RETURN_RAID_UNSET_SPIN_DOWN =                  0x3002007,  
    RETURN_TP_GET_LICENSE_STATE_FAILED  =          0x3002008,  
     
    RETURN_SSDC_DISK_IN_SAVEBOX =                  0xB000002,  
    RETURN_SSDC_DISK_NO_FREE =                     0xB000003,  
    RETURN_SSDC_DISK_PHY_TYPE_ERROR =              0xB000004,  
    RETURN_SSDC_DISK_NOT_IN_POOL =                 0xB000005,     
    RETURN_SSDC_LUN_TOOBIG_SIZE =                  0xB000006,  
    RETURN_SSDC_LUN_WITH_SMARTCACHE =              0xB000007,  
    RETURN_SSDC_LUN_WITHOUT_SMARTCACHE =           0xB000008,      
    RETURN_SSDC_DISK_IN_POOL =                     0xB000009,  
    RETURN_SSDC_LUN_NOT_OPERATION =                0xB00000A,  
    RETURN_SSDC_DEV_NOT_ALLOW_DELETE_SUBRACK =     0xB00000B,  
     
    RETURN_SSDC_DISK_EXCEED_MAX_SIZE =             0xB00000C,  
    RETURN_SSDC_PERFORMANCE_ANALYSIS_DISABLED =    0xB00000D,  

    RETURN_BUTT =                                  0xf000000
};
 
#define OM_AUTH_USER_NOT_FOUND                83887360    
#define OM_AUTH_USRNAMEORPASSWD_ERROR         83887373    

 
#define OM_UPGRADE_ERROR_CMD                  83892993   
#define OM_UPGRADE_HANDLED_FAILED             83892994   
#define RETURN_IN_UPDATE_STATUS               83892995   
#define RETURN_SINGLE_CONTROLLER              83892996   
#define RETURN_SINGLE_CONNECTION              83892997   
#define RETURN_UPGRADE_PARAM_ERROR            83892998   
#define RETURN_UPGRADE_NO_NEED_UPDATE         83892999  
#define RETURN_UPGRADE_SYSTEM_NOT_NORMAL      83893000  
#define RETURN_CMD_IN_SLAVE_CONTROLLER        83893001  

#define OM_MSG_DB_ERROR                       83886849    
#define OM_MSG_DB_INVALID_DISK_NO             83886850    
#define OM_MSG_DB_IMPORT_EXPORT_DISABLE       83886851    
#define OM_MSG_DB_NO_MEDIUM                   83886852    
#define OM_MSG_DB_MAGIC_ERROR                 83886853    
#define OM_MSG_DB_DATA_INVALID                83886854    
#define OM_MSG_DB_READ_CONFIG_ERROR           83886855    
#define OM_MSG_DB_NO_TXT_OUT_MOD_REG          83886856    
#define OM_MSG_IMPORT_CFG_UPGRADE_FAIL        83886857    
#define OM_MSG_DB_SYS_BUSY_TRY_LATER          83886858    

enum RETURN_PCIE_STATUS_E
{
    RETURN_PCIE_STATUS_OK = 0,   
    RETURN_PCIE_STATUS_INCONSISTENT,   
    RETURN_PCIE_STATUS_FAULT   
};
 
enum tagDHA_RETURN_VALUE_E
{
    RETURN_DHA_TASK_QUEUE_FULL =                   0x3001D05,  
    RETURN_DHA_TASK_NOT_EXIST =                    0x3001D06,  
    RETURN_DHA_SYS_NO_NORMAL =                     0x3001D07,  
    RETURN_DHA_BUTT
};

enum tagSYSTEM_RETURN_VALUE_E
{
    RETURN_REPEAT_NAME               = 0x40403241,           
    RETURN_OBJ_NUM_REACH_MAX         = 0x40403242,           
    RETURN_ALLOC_MEM_FAIL            = 0x40403243,           
    RETURN_OBJ_NOT_EXIST             = 0x40403244,           
    RETURN_OBJ_ID_EXIST              = 0x40403245,           
    RETURN_SYSTEM_STATUS_CHANGING    = 0x40403246,           
    RETURN_SYSTEM_STARTING           = 0x40403247,           
    RETURN_SYSTEM_START_FAIL         = 0x40403248,           
    RETURN_OM_MSG_TIME_OUT           = 0x40403249,           
    RETURN_OM_MSG_LAWLESS_CMD        = 0x4040324A,           
    RETURN_OM_MSG_VERSION_ERROR      = 0x4040324B,           
    RETURN_OM_MSG_DEAL_FAIL           = 0x4040324C,           
    RETURN_OM_MSG_NOT_REACHABLE      = 0x4040324D,           

    RETURN_SYSTEM_BUSY                 = 0x4040324E,           
    RETURN_SYSTEM_TYPE_UNEXIST        = 0x4040324F,           
    RETURN_SYSTEM_NO_DATA              = 0x40403250,           
    OM_ECONF_ITEM_INEXIST              = 0x40403251,           
    OM_ECONF_ITEM_FULL                  = 0x40403252,           
    RETURN_SYSTEM_INNER_ERROR           = 1077949019,          
    RETURN_SNAPSHOT_ISEXIST              = 32150008,

    RETURN_SYSTEM_BUTT
};

enum tagOM_UPGRADE_RETURN_VALUE_E
{    
    UPGRADE_DISK_NUM_ERROR      = 0x4040325E,  
    UPGRADE_SINGLE_CONTROLLER   = 0x4040325F,  
    UPGRADE_SINGLE_LINK_CONNECTION  = 0x40403260,  
    UPGRADE_CUR_VERSION_NOT_IN_CONFIG_LIST     = 0x40403261,
    UPGRADE_CUR_VERSION_IN_BLACK_LIST          = 0x40403262,
    UPGRADE_EXIST_URGENT_ALARM                 = 0x40403263,
    UPGRADE_SYS_STATUS_ABNORMAL                = 0x40403264,
    UPGRADE_CPU_USEAGE_RATE_TOO_HIGH           = 0x40403265,
    UPGRADE_DISK_SPACE_NOT_ENOUGH_FOR_UPG      = 0x40403266,
    UPGRADE_FORBID_UPGRADE_IN_UPG_STATUS       = 0x40403267,
    UPGRADE_FORBID_UPGRADE_CMD_IN_SYNC_STATUS  = 0x40403268,
    UPGRADE_NO_ROLLBACK_VERSION_EXIST          = 0x40403269,
    UPGRADE_MANAGEMENT_BOARD_FAULT             = 0x4040326A,
    UPGRADE_CUR_VERSION_SAME_WHITH_UPG_PACKAGE = 0x4040326B,
    UPGRADE_PACKAG_VERIFY_FAIL                 = 0x4040326C,
    UPGRADE_BBU_UPG_FAIL                       = 0x4040326D,
    UPGRADE_MANAGECHIP_UPG_FAIL                = 0x4040326E,
    UPGRADE_SAS_UPG_FAIL                       = 0x4040326F,
    UPGRADE_CPLD_UPG_FAIL                      = 0x40403270,
    UPGRADE_FPAG_UPG_FAIL                      = 0x40403271,
    UPGRADE_SYS_UPG_FAIL                       = 0x40403272,
    UPGRADE_HOTPATCH_PACKAGE_UNCOMPATIBLE_WITH_CUR_VERSION = 0x40403273,
    UPGRADE_HOTPATCH_NOT_EXIST                 = 0x40403274,
    UPGRADE_HOTPATCH_MEMORY_NOT_ENOUGH         = 0x40403275,
    UPGRADE_HOTPATCH_UPG_FAIL                  = 0x40403276,
    UPGRADE_PREPARE_UPD_PROCESS_FAIL           = 0x40403277,

    RETURN_OM_UPGRADE_BUTT 
};

enum tagOM_USER_RETURN_VALUE_E
{  
    RETURN_OM_AUTH_USER_NOT_FOUND               = 0x40403281,   
    RETURN_OM_AUTH_CMD_JUDGE_FAIL               = 0x40403282,   
    RETURN_OM_AUTH_USER_EXISTED                 = 0x40403283,   
    RETURN_OM_AUTH_OPERATION_NOT_SUPPORTED     = 0x40403284,   
    RETURN_OM_AUTH_LOGIN_FAILED                 = 0x40403285,   
    RETURN_OM_AUTH_ALREADY_LOGIN_ATPSDINIT     = 0x40403286,   
    RETURN_OM_AUTH_ALREADY_LOGIN_ATLEVELALTER  = 0x40403287,   
    RETURN_OM_AUTH_ALREADY_LOGIN_ATDELUSER     = 0x40403288,   
    RETURN_OM_AUTH_USRNAMEORPASSWD_ERROR       = 0x40403289,   
    RETURN_OM_AUTH_OLDPSD_NOT_RIGHT             = 0x4040328A,   
    RETURN_OM_AUTH_TOO_MANY_USERS               = 0x4040328B,   
    RETURN_OM_AUTH_MODIFY_OTHERS_PASSWD        = 0x4040328C,   
    RETURN_OM_AUTH_CONNECT_FAILED               = 0x4040328D,   
    RETURN_OM_AUTH_USER_LOCK                     = 0x4040328E,   
    RETURN_OM_AUTH_IP_NOT_PERMIT                = 0x4040328F,   
    RETURN_OM_AUTH_MODIFY_LDAP_USER_NOT_PERMIT = 0x40403290,   
    RETURN_OM_AUTH_SERVER_NOT_CONFIG            = 0x40403291,   
    RETURN_OM_AUTH_SERVER_NOT_CONTACT           = 0x40403292,   
    RETURN_OM_AUTH_INVALID_DN_SYNTAX             = 0x40403293,   
    RETURN_OM_AUTH_USER_NOT_MAP_LOCAL           = 0x40403294,   
    RETURN_OM_AUTH_WAIT_RESPOSE_TIME_OUT        = 0x40403295,   
    RETURN_OM_AUTH_INVALID_USERNAME              = 0x40403296,    
    RETURN_OM_AUTH_INVALID_USERLEVEL             = 0x40403297,    
    RETURN_OM_AUTH_RESERVED_USERNAME_CONFLICT   = 0x40403298,    
    RETURN_OM_AUTH_LOCAL_LDAP_USERNAME_SAME     = 0x40403299,    
    RETURN_OM_AUTH_BUTT
};

 
enum tagOM_ALARM_RETURN_VALUE_E
{
    RETURN_OM_ALARM_MASK_ID_INVALID               = 83887400,                                        
    RETURN_OM_ALARM_BE_CLEARED                    = 83887401,                                      
    RETURN_OM_ALARM_CLEAR_NOT_EXITED             = 83887402,                                    
    RETURN_OM_ALARM_CLEAR_SAVED_FAILED           = 83887403,                                      
    RETURN_OM_ALARM_CLEAR_FAILED                  = 83887404,                                    
    RETURN_OM_ALARM_TRAP_CFG_ADD_FAILED          = 83887405,            
    RETURN_OM_ALARM_TRAP_CFG_OP_FAILED           = 83887406,                         
    RETURN_OM_ALARM_AFFIRM_FAILED                 = 83890180,                                      
    RETURN_OM_ALARM_BE_AFFIRMED                   = 83890181,                                      
    RETURN_OM_ALARM_TRAP_REPEAT                   = 83887407,                                   
    RETURN_OM_ALARM_CONFIG_ERROR                  = 83887408,                                              
    RETURN_OM_ALARM_PACKAGE                        = 83887409,    
    RETURN_OM_ALARM_SEND_TESTMAIL_FAILED         = 83887415,    

    RETURN_OM_ALARM_BUTT
};

enum tagOM_PERF_RETURN_VALUE_E
{ 
    OM_PERF_HANDLE_FAILED                 = 83890432,                                     
    OM_PERF_ERROR_CMD                     = 83890433,                                         
    OM_PERF_TYPE_NOEXIST                  = 1077949048,                                           
    OM_PERF_ERROR_PARAM                   = 83890435,         
    OM_PERF_QUERY_NOEXIST                 = 1077949049,                                       
    OM_PERF_OP_NOT_PERMITTED              = 1077949050,                                          
    OM_PERF_QUERY_HIS_NOEXIST             = 83890438,     
    OM_PERF_SWITCH_STATUS_NOT_PERMITTED  = 1077949051,
    RETURN_OM_PERF_BUTT
};

enum tagOM_ECONF_RETURN_VALUE_E
{  
    OM_SYS_COMMAND_SUCCESS                  = 0x3000016,      
    RETURN_OM_ECONF_BUTT
};

 
enum tagOM_SCM_RETURN_VALUE_E
{
    SCM_TIMEZONE_NO_DST         = 1077949052,   
    SCM_NTP_FORBIDDEN_MODIY     = 1077949053,   
    SCM_GET_NTP_TIME_FAIL       = 1077949054,   
    SCM_SET_TIME_ZONE_FAIL      = 1077949055,   

    RETURN_OM_SCM_BUTT
};

enum tagOM_LICENSE_RETURN_VALUE_E
{
    RETURN_LICENSE_WORKSPACE_CALC_ERROR =          0x40403D01,  
    RETURN_LICENSE_WORKSPACE_INIT_ERROR =          0x40403D02,  
    RETURN_LICENSE_ITEM_ERROR =                    0x40403D03,  
    RETURN_LICENSE_GET_ESN_ERROR =                 0x40403D04,  
    RETURN_LICENSE_GET_PRODUCT_NAME_ERROR =        0x40403D05,  
    RETURN_LICENSE_GET_SOFTWARE_VERSION_ERROR =    0x40403D06,  
    RETURN_LICENSE_UP_LOAD_LICENSE_FILE_ERROR =    0x40403D07,  
    RETURN_LICENSE_VERIFY_ERROR =                  0x40403D08,  
    RETURN_LICENSE_SEND_CONTRL_INFO_ERROR =        0x40403D09,  
    RETURN_LICENSE_INIT_LICENSE_THREAD_ERROR =     0x40403D0A,  
    RETURN_LICENSE_GET_LICENSE_FILE_INFO_ERROR =   0x40403D0B,  
    RETURN_LICENSE_READ_HISTORY_LICENSE_INFO_ERROR = 0x40403D0C,  
    RETURN_LICENSE_WRITE_HISTORY_LICENSE_INFO_ERROR= 0x40403D0D,  
    RETURN_LICENSE_OPEN_LICENSE_FILE_ERROR =       0x40403D0E,  
    RETURN_LICENSE_READ_LICENSE_FILE_ERROR =       0x40403D0F,  
    RETURN_LICENSE_WORKSPACE_MALLOC_ERROR =        0x40403D10,  
    RETURN_LICENSE_REACH_SW_MAINTAIN_DEAD_TIME =   0x40403D11,  
    RETURN_LICENSE_REACH_HW_MAINTAIN_DEAD_TIME =   0x40403D12,  
    RETURN_LICENSE_REACH_SW_UPDATE_DEAD_TIME =     0x40403D13,  
    RETURN_LICENSE_REACH_FEATURE_RUN_DEAD_TIME =   0x40403D14,  
    RETURN_LICENSE_GET_FILE_BASE_INFO_ERROR =      0x40403D15,  
    RETURN_LICENSE_GET_SYSTEM_TIME_ERROR =         0x40403D16,  
    RETURN_LICENSE_PARAMETER_FAIL =                0x40403D17,  
    RETURN_LICENSE_MALLOC_FAIL =                   0x40403D18,  
    RETURN_LICENSE_CMD_NOTEXIST =                  0x40403D19,  
    RETURN_LICENSE_UNKNOWN_ERROR =                 0x40403D1A,     
    RETURN_LICENSE_INVALID =                       0x40403D1B,  
    RETURN_LICENSE_OFF =                           0x40403D1C,  
    RETURN_LICENSE_EXPIRED =                       0x40403D1D,  
    RETURN_LICENCE_BUTT 
};

enum tagEMP_RETURN_VALUE_E
{
    RETURN_CHANGE_MAC_WHEN_SUBRACK_ONLINE                         =0x40403381,        
    RETURN_ENCLOSURE_ID_NOT_EXIST                                 =0x40403382,        
    RETURN_ENCLOSURE_TEMPERATURE_TOO_HIGH                         =0x40403383,        
    RETURN_CONTROLLER_ID_NOT_EXIST                                =0x404033C4,        
    RETURN_EXPBOARD_ID_NOT_EXIST                                  =0x40403445,        
    RETURN_INTF_ID_NOT_EXIST                                      =0x40403446,        
    RETURN_POWERON_FIX_INTF_OPERATION_NOT_SUPPORTED               =0x40403447,        
    RETURN_INTERFACE_NOT_ACTIVATED                                =0x40403448,        
    RETURN_INTERFACE_FAULTY                                       =0x40403449,        
    RETURN_BACKUP_POWER_ID_NOT_EXIST                              =0x4040348A,        
    RETURN_POWER_ID_NOT_EXIST                                     =0x404005CB,       
    RETURN_FAN_ID_NOT_EXIST                                       =0x404034CC,        
    RETURN_PORT_IS_INI_SPEED_CANNOT_MODIFIED                      =0x4040350D,        
    RETURN_FC_ID_NOT_EXIST                                        =0x4040350E,        
    RETURN_ETH_ID_NOT_EXIST                                       =0x4040354F,        
    RETURN_INVALID_GATEWAY                                        =0x40403550,        
    RETURN_GW_NOT_MATCH_IP                                        =0x40403551,        
    RETURN_GATEWY_SAME_AS_IP                                      =0x40403552,        
    RETURN_CTRL_IP_USED                                           =0x40403553,        
    RETURN_SAME_AS_MGT                                            =0x40403554,        
    RETURN_SAME_AS_SRV                                            =0x40403555,        
    RETURN_SAME_AS_MNTN                                           =0x40403556,        
    RETURN_IP_CONFLICT_WITH_ROUTE                                 =0x40403557,        
    RETURN_INVALID_IP                                             =0x40403558,        
    RETURN_SAS_ID_NOT_EXIST                                       =0x40403599,        
    RETURN_SERIES_ID_NOT_EXIST                                    =0x404035DA,        
    RETURN_CPU_ID_NOT_EXIST                                       =0x4040041B,        
    RETURN_MEMORY_ID_NOT_EXIST                                    =0x40403B5C,        
    RETURN_SAME_ROUTE                                             =0x4040355E ,       
    RETURN_ROUTE_DB_FULL                                          =0x4040355F,        
    RETURN_NO_IP_CONFIGURED                                       =0x40403560,        
    RETURN_DEST_NOT_MATCH_MASK                                    =0x40403561,        
    RETURN_ROUTE_CONFLICT_WITH_MGT                                =0x40403562,        
    RETURN_ROUTE_CONFLICT_WITH_MNTN                               =0x40403563,        
    RETURN_ROUTE_CONFLICT_WITH_SRV                                =0x40403564,        
    RETURN_BOND_PORT_ID_NOT_EXIST                                 =0x40403AE5,        
    RETURN_TOO_MANY_BOND                                          =0x40403AE6,        
    RETURN_BOND_TOO_FEW_MEMBER                                    =0x40403AE7,        
    RETURN_BOND_MEMBER_USED                                       =0x40403AE8,        
    RETURN_INCONSISTENT_PORT_TYPE                                 =0x40403AE9,       
    RETURN_BOND_MEMBER_ON_DIFFERENT_INTF                          =0x40403AEA,        
    RETURN_SFP_ID_NOT_EXIST                                       =0x40403AAB,        
    RETURN_IPV6_DEST_NOT_MATCH_PREFIX                             =0x40403AAC,        
    RETURN_INTERFACE_ALREADY_POWER_ON                             =0x40403AAD,        
    RETURN_INTERFACE_ALREADY_POWER_OFF                            =0x40403AAE,        
    RETURN_MGT_BOARD_POWERONOFF_NOT_SUPPORTED                     =0x40403AAF,        
    RETURN_CHANGE_CTRL_FRAME_MAC_NOT_SUPPORTED                    =0x40403AB0,        
    RETURN_NEW_USR_FRAME_ID_OVER_SIZE                             =0x40403AB1,        
    RETURN_NEW_USR_FRAM_ID_REPEATED                               =0x40403AB2,        
    RETURN_DISK_FRAME_POWERON_REPEAT                              =0x40403AB3,        
    RETURN_DISK_LIGHT_REPEAT                                      =0x40403AB4,        
    RETURN_BOND_NAME_REPEAT                                       =0x40403AB5,        
    RETURN_ADD_MTG_BOARD_PORT_ROUTE_NOT_SUPPORTED                 =0x40403AB6 ,       
    RETURN_SET_MAINTANCE_PORT_IPV6_NOT_SUPPORTED                  =0x40403AB7,        
    RETURN_DEL_MGT_BOARD_PORT_IP_NOT_SUPPORTED                    =0x40403AB8,        
    RETURN_CLOSE_SAS_PRI_PORT_PHY_SWITCH_NOT_SUPPORTED            =0x40403AB9,        
    RETURN_MODIFY_SPECIAL_FRAME_ID_IS_NOT_CTRL_FRAME              =0x40403ABA,        
    RETURN_FRAME_MAC_REPEATED                                     =0x40403ABB,        
    RETURN_MODIFY_FC_SPEED_NOT_SUPPORTED                          =0x40403ABC ,       
    RETURN_IP_DB_FULL                                             =0x40403ABD ,       
    RETURN_MGT_OR_MNTN_BOND_NOT_SUPPORTED                         =0x40403ABE,        
    RETURN_EMP_BUTT
};

enum tagQOS_RETURN_VALUE_E
{
    RETURN_LUN_HAVE_IN_IO_CLASS             = 0x40403981,  
    RETURN_FORBIT_MODIFY_IO_CLASS           = 0x40403982,  
    RETURN_FORBIT_DELETE_IO_CLASS           = 0x40403983,  
    RETURN_FORBIT_ACTIVE_EMPTY_IO_CLASS     = 0x40403984,  
    RETURN_FORBIT_ACTIVE_EXPIRED_IO_CLASS   = 0x40403985,  
    RETURN_QOS_BUTT
};

enum tagSTORAGEPOOL_RETURN_VALUE_E
{
    RETURN_DISK_NUM_IN_POOL_REACH_MAX = 0x40403601,   
    RETURN_POOL_IN_USE = 0x40403602, 
    RETURN_POOL_CAN_NOT_BE_OPERATED = 0x40403603, 
    RETURN_POOL_PREDICTION_DISABLED = 0x40403604, 
    RETURN_MIGRTE_ALREADY_RUNNING = 0x40403605, 
    RETURN_MIGRATE_TIME_IS_EMPTY = 0x40403606, 
    RETURN_HOTSPARE_SPACE_NOT_ENOUGH = 0x40403607, 
    RETURN_DISK_TYPE_MISS_MATCH = 0x40403608, 
    RETURN_DISK_AND_TIER_MISS_MATCH = 0x40403609, 
    RETURN_MAX_POOL_NUM_LIMIT = 0x4040360A, 
    RETURN_MAX_DISK_OF_POOL = 0x4040360B, 
    RETURN_DST_LICENSE_INVALID = 0x4040360C, 
    RETURN_POOL_ID_INVALID = 0x4040360D, 
    RETURN_POOL_DISK_STATUS_ABNORMAL = 0x4040360E, 
    RETURN_POOL_DISK_IN_USE = 0x4040360F, 
    RETURN_POOL_DISK_NUM_NOT_ENOUGH  = 0x40403610, 
    RETURN_POOL_FREE_DISK_NOT_ENOUGH = 0x40403611, 
    RETURN_POOL_MONITOR_TIME_INVALID = 0x40403612, 
    RETURN_POOL_HEALTH_STATUS_FORBIT_OPERATION = 0x40403613,  
    RETURN_POOL_OFFLINE_STATUS_FORBIT_OPERATION = 0x40403614, 
    RETURN_EXTENT_EXCEED = 0x40403615, 
    RETURN_CANNOT_FIND_DISK = 0x40403616, 
    RETURN_DST_ONLY_ONE_TIER = 0x40403617, 
    RETURN_DST_IS_PAUSED = 0x40403618, 
     
    RETURN_RAID_DISK_NUM_INVALID = 0x40403619, 
    RETURN_RAID0_NOT_SUPPORT_HOT_SPARE_LEVEL_MODIFY = 0x4040361A, 
    RETURN_META_SPACE_NOT_ENOUGH = 0x4040361E, 
     
    RETURN_STORAGEPOOL_BUTT
};

enum tagDISK_RETURN_VALUE_E
{
     
    RETURN_DISK_ID_NOT_EXIST = 0x40400281, 
    RETURN_DISK_BUTT
};
enum tagLUN_RETURN_VALUE_E
{
    RETURN_NOT_ENOUGH_SPACE_FOR_LUN = 0x404002C1, 
    RETURN_FORBIT_PROCESS_WITH_RSS = 0x404002C2, 
    RETURN_FORBID_DELETE_WITH_IOCLASS = 0x404002C3, 
    RETURN_FORBID_DELETE_WITH_MAPPING = 0x404002C4, 
    RETURN_SYS_WORM_CONFIGED_CLOSED_FAIL = 0x404002C5, 
    RETURN_FORBID_LUN_IS_FAUTL = 0x404002C6, 
    RETURN_FORBID_LUN_IS_OFFLINE = 0x404002C7, 
    RETURN_FORBIT_SET_WORM_WRITE = 0x404002C8, 
    RETURN_WORM_LUN_TERM_LESS_THAN_NOW = 0x404002C9, 
    RETURN_RSS_LUN_NOT_SUPPORT_EXPAND = 0x404002CA, 
    RETURN_FORBIT_CREATE_LUN = 0x404002CB,    
    RETURN_FORBIT_CHANGE_LUN_WORK_CONTROL = 0x404002CC, 
    RETURN_EXPAND_LUN_ONLY_ACCRATION = 0x404002CD,  
    RETURN_FORBIT_EXTEND_WORM_TIME = 0x404002CE, 
    RETURN_FORBIT_REPEAT_SET_WORM_LUN = 0x404002CF, 
    RETURN_WORM_LUN_SWITCH_CLOSE = 0x404002D0, 
    RETURN_WORM_LUN_FORBIT_EXPAND = 0x404002D1, 
    RETURN_WORM_LUN_FORBIT_MODIFY = 0x404002D2, 
    RETURN_WORM_LUN_FORBIT_DELETE = 0x404002D3, 
    RETURN_LUN_VALUE_BUTT    
};
enum tagMAPPINGVIEW_RETURN_VALUE_E
{
    RETURN_LMP_DEFAULT_MAPVIEW_CANNOT_ADD_HOST = 0x40403D41,  
    RETURN_LMP_DEFAULT_MAPVIEW_CANNOT_MODIFY = 0x40403D42, 
    RETURN_LMP_DEFAULT_MAPVIEW_CANNOT_DELETE = 0x40403D43, 
    RETURN_LMP_MAPVIEW_HAS_HOST = 0x40403D44, 
    RETURN_LMP_MAPVIEW_HAS_LUN = 0x40403D45, 
    RETURN_LMP_MAPVIEW_HAS_PORT = 0x40403D46, 
    RETURN_LMP_MAP_LUN_CANNOT_BE_MAP = 0x40403D47, 
    RETURN_LMP_MAP_LUN_NOT_EXIST = 0x40403D48, 
    RETUEN_LMP_MAP_HOST_LUN_BE_USED = 0x40403D49, 
    RETUEN_LMP_MAP_HOST_LUN_BE_USED_UP = 0x40403D4A, 
    RETURN_LMP_MAP_MAP_VIEW_NOT_EXIST = 0x40403D4B, 
    RETURN_LMP_MAP_NUM_EXCEED = 0x40403D4C, 
    RETURN_LMP_MAP_BUSY = 0x40403D4D, 
    RETURN_LMP_MAP_NOT_EXIST = 0x40403D4E, 
    RETURN_LMP_MAP_LUN_USED_TO_INBAND_CMD = 0x40403D52,  
    RETURN_LMP_MAP_LUN_REPEAT_MAP = 0x40403D53, 
    RETURN_LMP_MAPVIEW_SUPPORT_INBAND_CMD = 0x40403D54, 
    RETURN_LMP_HOST_ALREADY_BELONG_MAPVIEW = 0x4040054F, 
    RETURN_LMP_HOST_MAPPING_VIEW_BE_NOT_EXIST = 0x40400550, 
    RETURN_LMP_HOST_HAS_PORT = 0x40400551, 
    RETURN_LMP_HOST_BUSY = 0x40400555,  
    RETURN_LMP_HOST_NOT_BELONG_MAPVIEW = 0x40400556, 
    RETURN_LMP_REPEAT = 0x3000420,       
    RETURN_TGT_ESGL_NOT_ENOUGH = 0x3000410,  
    RETURN_INBAND_VERSION_UNCOMPATIBLE = 0x3000419,  
    RETURN_MAPPINGVIEW_BUTT
};

enum tagINITIATOR_RETURN_VALUE_E
{ 
    RETURN_LMP_PORT_HOST_NOT_EXIST = 0x40403781, 
    RETURN_LMP_PORT_ALREADY_BELONG_HOST = 0x40403782, 
    RETURN_LMP_PORT_BUSY = 0x40403783, 
    RETURN_LMP_PORT_BE_LINK_UP = 0x40403784, 
    RETURN_LMP_PORT_BELONG_HOST = 0x40403785, 
    RETURN_LMP_PORT_NOT_BELONG_HOST = 0x40403786,  
    RETURN_INITIATOR_BUTT
};

enum tagIBHAGT_RETURN_VALUE_E
{
    RETURN_IBHAGT_CMD_ERROR = 0x3001800,  
    RETURN_IBHAGT_DEVICE_NO_EXIST = 0x3001801,  
    RETURN_IBHAGT_NETWORK_FAIL = 0x3001802,  
    RETURN_IBHAGT_DEVICE_NAME_FAIL = 0x3001803,  
    RETURN_IBAGT_TEST_LINK_OK  = 0x3001804,                    
    RETURN_IBAGT_GET_MSG_FAIL  = 0x3001805,                    
    RETURN_IBAGT_USRINFO_FAIL  = 0x3001806,                    
    RETURN_IBAGT_EXIST_MAPPING = 0x3001807,  
    RETURN_IBAGT_NOT_EXIST_MAPPING   = 0x3001808,              
    RETURN_IBAGT_NOT_EXIST_HOSTGROUP = 0x3001809,  
    RETURN_IBAGT_NOT_EXIST_HOST = 0x300180A,  
    RETURN_IBAGT_NOT_INBAND_VIRTUAL_MAPPING = 0x300180B,  
    RETURN_IBHAGT_DEVICE_MAP_ERROR = 0x300180C  
};

#if 1
enum tagSNAP_CTRL_RETURN_VALUE_E
{
    RETURN_SNAP_REACH_SNAPSHOTLUN_MAX_NUM =            0x404006C1,  
    RETURN_SNAP_REACH_ORIGINLUN_MAX_NUM =              0x404006C2,  
    RETURN_SNAP_REACH_SNAPSHOT_PER_ORIGINLUN_MAX_NUM = 0x404006C3,  
    RETURN_SNAP_NOT_SNAPSHOTLUN =                      0x404006C4,  
    RETURN_SNAP_SNAPSHOTLUN_BELONGTO_OTHER =           0x404006C5,  
    RETURN_SNAP_CMD_SEND_FREQUENT =                    0x404006C6,  
    RETURN_SNAP_NO_TASK_RESOURCE =                     0x404006C7,  
    RETURN_SNAP_GET_LICENSE_STATE_FAILED =             0x404006C8,  
    RETURN_SNAP_GET_RESOURCE_FAILED =                  0x404006C9,  
    RETURN_SNAP_SNAPSHOTLUN_IS_BUSY =                  0x404006CA,  
    RETURN_SNAP_NOT_ALLOW_DELETE_MAPPINGLUN =          0x404006CB,  
    RETURN_SNAP_SPEED_PARAM_ERROR =                    0x404006CC,  
    RETURN_SNAP_NOT_ALLOW_ACTIVE_SAME_OIRGIN =         0x404006CD,  
    RETURN_SNAP_NOT_ALLOW_DELETE_SNAPLUN =             0x404006CE,  
    RETURN_SNAP_NOT_ALLOW_ROLLBACK_SNAPLUN =           0x404006CF,  
    RETURN_SNAP_PARAM_ERROR =                          0x404006D0,  
    RETURN_SNAP_SNAPSHOTLUN_IS_ABNORMAL =              0x404006D1,  
    RETURN_SNAP_GET_ORIGINLUN_LOCK_FAILED =            0x404006D2,  
    RETURN_SNAP_END_SNAPSHOTLUN =                      0x404006D3,  
    RETURN_SNAP_NOT_ALLOW_WHEN_ACTIVE =                0x404006D4,  
    RETURN_SNAP_NOT_ALLOW_WHEN_DISABLE =               0x404006D5,  
    RETURN_SNAP_NOT_ALLOW_WHEN_ROLLBACK =              0x404006D6,  
    RETURN_SNAP_NOT_ALLOW_WHEN_SNAPSHOTLUN_FAULT =     0x404006D7,  
    
    RETURN_SNAP_SNAPSHOTLUN_NOT_EXIST =                0x404006D8,  
    RETURN_SNAP_NOT_ALLOW_WHEN_NOT_ROLLBACK =          0x404006D9,  
    RETURN_SNAP_REACH_BATCH_OPERATE_MAX_NUM =          0x404006DA,  
    RETURN_SNAP_EXEC_CMD_ERR =                         0x404006DB,  
    RETURN_SNAP_BUTT 
};
#endif

enum tagRM_RETURN_VALUE_E
{
    RETURN_RM_SERIAL_NOT_EXIST =                   0x40400701,  
    RETURN_RM_LUN_NOT_EXIST =                      0x40400702,  
    RETURN_RM_TEAM_NOT_EXIST =                     0x40400703,  
    RETURN_RM_CG_ID_NOT_EXIST =                    0x40400704,  
    RETURN_RM_LUN_ALREADY_USED =                   0x40400705,  
    RETURN_RM_SLV_LUN_NOT_MEMBER_OF_TEAM =         0x40400706,  
    RETURN_RM_TEAM_NOT_MEMBER_OF_CG =              0x40400707,  
    RETURN_RM_MIRROR_NUM_EXCEED =                  0x40400708,  
    RETURN_RM_SYNC_MIRROR_NUM_EXCEED =             0x40400709,  
    RETURN_RM_ASYNC_MIRROR_NUM_EXCEED =            0x4040070A,  
    RETURN_RM_CG_NUM_EXCEED =                      0x4040070B,  
    RETURN_RM_CG_MIRROR_NUM_EXCEED =               0x4040070C,  
    RETURN_RM_SLV_LUN_NUM_EXCEED =                 0x4040070D,  
    RETURN_RM_ILLEGAL_MIRROR_TYPE =                0x4040070E,  
    RETURN_RM_ILLEGAL_OPCODE_TYPE =                0x4040070F,  
    RETURN_RM_CAN_NOT_EXECUTE_AT_MST_NODE =        0x40400710,  
    RETURN_RM_CAN_NOT_EXECUTE_AT_SLV_NODE =        0x40400711,  
    RETURN_RM_CAN_NOT_EXECUTE_WHEN_LINK_NORMAL =   0x40400712,  
    RETURN_RM_CAN_NOT_EXECUTE_WHEN_LINK_ERROR =    0x40400713,  
    RETURN_RM_LICENSE_NOT_ALLOW_OPERATION =        0x40400714,  
    RETURN_RM_CONTROLLER_NOT_ALLOW_OPERATION =     0x40400715,  
    RETURN_RM_CG_MEMBER_CAN_NOT_EXECUTE =          0x40400716,  
    RETURN_RM_SLV_LUN_EXIST_NOT_DELETE_MIRROR =    0x40400717,  
    RETURN_RM_CG_MEMBER_EXIST_NOT_DELETE =         0x40400718,  
    RETURN_RM_LUN_ARRAY_IS_CONSISTENT =            0x40400719,  
    RETURN_RM_ARRAY_NOT_CONSISTENT =               0x4040071A,  
    RETURN_RM_REMOTE_LINK_BAD =                    0x4040071B,  
    RETURN_RM_PEER_CTLR_LINK_BAD =                 0x4040071C,  
    RETURN_RM_SYS_BUSY =                           0x4040071D,  
    RETURN_RM_REMOTE_SYS_BUSY =                    0x4040071E,  
    RETURN_RM_OBJ_BUSY =                           0x4040071F,  
    RETURN_RM_COMMUNICATE_ERROR =                  0x40400720,  
    RETURN_RM_MST_SLV_LUN_SIZE_NOT_SAME =          0x40400721,  
    RETURN_RM_MST_LUN_NOT_MEMBER_OF_TEAM =         0x40400722,  
    RETURN_RM_MST_LUN_HEALTH_NOT_OPERATION =       0x40400723,  
    RETURN_RM_SLV_LUN_HEALTH_NOT_OPERATION =       0x40400724,  
    RETURN_RM_MST_LUN_ROLLBACK_NOT_OPERATION =     0x40400725,  
    RETURN_RM_SLV_LUN_ROLLBACK_NOT_OPERATION =     0x40400726,  
    RETURN_RM_CG_FAULT_NOT_OPERATION =              0x40400727,  
    RETURN_RM_CG_MEMBER_NULL_CAN_NOT_EXECUTE =     0x40400728,  
    RETURN_RM_REMOTE_LINK_NOT_EXIST =              0x40400729,  
    RETURN_RM_TASK_ERROR =                         0x4040072A,  
    RETURN_RM_SLV_LUN_NUM_NOT_ONLYONE =            0x4040072B,  
    RETURN_RM_MIRRORTYPE_NOT_CONSISTENT =          0x4040072C,  
    RETURN_RM_PEER_CTLR_NOT_EXIST =                0x4040072D,  
    RETURN_RM_CONN_NOT_EXIST =                     0x4040072E,  
    RETURN_RM_SEND_MSG_REMOTE_ERR =                0x4040072F,  
    RETURN_RM_COMMUNICATE_REMOTE_ERR =             0x40400730,  
    RETURN_RM_DELIVER_MSG_ERR =                    0x40400731,  
    RETURN_RM_DLV_DEST_CTLR_ERR_SINGLE =           0x40400732,  
    RETURN_RM_OBJ_SERV_STOPED =                    0x40400733,  
    RETURN_RM_REMOTE_TEAM_EXIST =                  0x40400734, 
    RETURN_RM_REMOTE_TEAM_NOT_EXIST =              0x40400735,  
    RETURN_RM_REMOTE_LICENSE_NOT_ALLOW_OPERATION = 0x40400736,  
    RETURN_RM_REMOTE_CG_NOT_EXIST =                0x40400737,  
    RETURN_RM_REMOTE_CG_NUM_EXCEED =               0x40400738,  
    RETURN_RM_REMOTE_CG_INVALID_NOT_OPERATION =    0x40400739,  
    RETURN_RM_MUTEX_NOT_ALLOW_CREATE_TEAM =        0x4040073A, 
    RETURN_RM_MUTEX_NOT_ALLOW_ADD_SLVLUN =         0x4040073B, 
    RETURN_RM_OTHER_PAIR_STATE_NOT_OPERATION =     0x4040073C, 
    RETURN_RM_MST_LUN_ROLLBACK_FORCE_OR_NOT =      0x4040073D, 
    RETURN_RM_SLV_LUN_ROLLBACK_FORCE_OR_NOT =      0x4040073E,   
    RETURN_RM_PAIR_SYNCING_NOT_OPERATION =         0x4040073F, 
    RETURN_RM_PAIR_SPLITED_NOT_OPERATION =         0x40400740, 
    RETURN_RM_PAIR_INTERRUPTED_NOT_OPERATION =    0x40400741, 
    RETURN_RM_PAIR_INVALID_NOT_OPERATION =         0x40400742, 
    RETURN_RM_PAIR_TO_BE_RECOVERD_NOT_OPERATION = 0x40400743, 
    RETURN_RM_PAIR_SYNCED_NOT_OPERATION =          0x40400744, 
    RETURN_RM_SLV_DATA_INCONSISTENCY_NOT_OPERATION = 0x40400745, 
    RETURN_RM_CG_STATUS_FAULT_NOT_OPERATION =      0x40400746, 
    RETURN_RM_CG_SYNCING_NOT_OPERATION =            0x40400747, 
    RETURN_RM_CG_SPLITED_NOT_OPERATION =            0x40400748, 
    RETURN_RM_CG_TO_BE_RECOVERD_NOT_OPERATION =    0x40400749, 
    RETURN_RM_CG_INTERRUPTED_NOT_OPERATION =        0x4040074A, 
    RETURN_RM_CG_INVALID_NOT_OPERATION =            0x4040074B, 
    RETURN_RM_CONN_BAD_FORCE_OR_NOT =            0x4040074C,
    RETURN_RM_BUTT
};

enum tagLINKCFG_RETURN_VALUE_E
{
    RETURN_LINKCFG_ARRAY_NOT_EXIST          = 0x40403841,        
    RETURN_LINKCFG_LINK_CONFIGED            = 0x40403842,          
    RETURN_LINKCFG_LINK_NOT_EXIST           = 0x40403843,        
    RETURN_LINKCFG_LINK_IS_DOWN             = 0x40403844,          
    RETURN_LINKCFG_REACH_MAX_LINK           = 0x40403845,       
    RETURN_LINKCFG_LINK_NOT_BELONG_TO_ARRAY = 0x40403846,       
    RETURN_LINKCFG_SAME_CTRL_AS_PRIOR       = 0x40403847,          
    RETURN_LINKCFG_REMOTE_DEL_FAILED        = 0x40403848,         
    RETURN_LINKCFG_CONN_IS_USING            = 0x40403849,         
    RETURN_LINKCFG_REACH_MAX_CONN           = 0x4040384A,        
    RETURN_LINKCFG_CONN_IS_DOWN             = 0x4040384B,       
    RETURN_LINKCFG_DEL_CONN_FAILURE         = 0x4040384C,        
    RETURN_LINKCFG_LINK_NOT_CONFIGED        = 0x4040384D,       
    RETURN_LINKCFG_LAST_LINK_CANNOT_DEL     = 0x4040384E,         
    RETURN_LINKCFG_REMOTE_ADD_FAILED        = 0x4040384F,      
    RETURN_LINKCFG_ARRAY_TYPE_ERROR         = 0x40403850,        
    RETURN_LINKCFG_UTILIZATION_INVALID      = 0x40403851,       
    RETURN_LINKCFG_ACTUAL_BANDWIDTH_INVALID = 0x40403852,       
    RETURN_LINKCFG_BANDWIDTH_LARGER_THAN_PORT_SPEED = 0x40403853,       
    RETURN_LINKCFG_BUTT
};                      

enum tagCLONE_RETURN_VALUE_E
{
    RETURN_CLN_MAX_SYS_SLAVE_LUN_NUM                        = 0x40403701,   
    RETURN_CLN_MAX_SYS_CLONE_NUM                            = 0x40403702,   
    RETURN_CLN_MAX_CLONE_SLAVE_NUM                          = 0x40403703,   
    RETURN_CLN_LUN_ALREADY_PRIMARY_LUN                      = 0x40403704,   
    RETURN_CLN_LUN_ALREADY_SLAVE_LUN                        = 0x40403705,   
    RETURN_CLN_QUERY_SLAVE_LUN_NOT_EXIST                    = 0x40403706,   
    RETURN_CLN_DEL_CLONE_FAILED                             = 0x40403707,   
    RETURN_CLN_DEL_SLAVE_LUN_NOT_EXIST                      = 0x40403708,   
    RETURN_CLN_CREATE_CLONE_FAILED                          = 0x40403709,   
    RETURN_CLN_ADD_SLAVE_LUN_NOT_EXIST                      = 0x4040370A,   
    RETURN_CLN_SYNC_FAILED                                  = 0x4040370B,   
    RETURN_CLN_RESYNC_FAILED                                = 0x4040370C,   
    RETURN_CLN_SPLIT_FAILED                                 = 0x4040370D,   
    RETURN_CLN_MASTER_SLAVE_LUN_SIZE_NOT_EQUAL              = 0x4040370E,   
    RETURN_CLN_OBTAIN_WORK_CTRL_FAILED                      = 0x4040370F,   
    RETURN_CLN_WORK_CTRL_NOT_EQUAL_CUR_CTRL                 = 0x40403710,   
    RETURN_CLN_LUN_STATUS_NOT_NOMAL                         = 0x40403711,   
    RETURN_CLN_CLONE_NOT_EXIST                              = 0x40403712,   
    RETURN_CLN_SPLIT_SLAVE_LUN_NOT_EXSIT                    = 0x40403713,   
    RETURN_CLN_PROHIBIT_MODIFY_PROTECT_MODE_IN_CUR_STATUS   = 0x40403714,   
    RETURN_CLN_MASTER_LUN_CONFLICT_WITH_OTHER_FEAUTURE      = 0x40403715,   
    RETURN_CLN_SLAVE_LUN_CONFLICT_WITH_OTHER_FEAUTURE       = 0x40403716,   
    RETURN_CLN_PROHIBIT_SPLIT_IN_CUR_STATUS                 = 0x40403717,   
    RETURN_CLN_PROHIBIT_RESYNC_IN_CUR_STATUS                = 0x40403718,   
    RETURN_CLN_INVALID_LICENSE                              = 0x40403719,   
    RETURN_CLN_PROHIBIT_RESYNC_DUE_TO_DB                    = 0x4040371A,   
    RETURN_CLN_PROHIBIT_OPS_IN_PAIR_SYNC                    = 0x4040371B,   
    RETURN_CLN_PROHIBIT_OPS_IN_PAIR_RESYNC                  = 0x4040371C,   
    RETURN_CLN_PROHIBIT_OPS_IN_PAIR_SPLIT                   = 0x4040371D,   
    RETURN_CLN_PROHIBIT_OPS_IN_PAIR_NOMAL                   = 0x4040371E,   
    RETURN_CLN_PROHIBIT_OPS_IN_PAIR_FAULT                   = 0x4040371F,   
    RETURN_CLN_PROHIBIT_OPS_IN_DATA_INCONSISTENT            = 0x40403720,   
    RETURN_CLN_PROHIBIT_OPS_IN_OTHER_PAIR_SYNC              = 0x40403721,   
    RETURN_CLN_PROHIBIT_OPS_IN_OTHER_PAIR_RESYNC            = 0x40403722,   
    RETURN_CLN_PROHIBIT_OPS_IN_OTHER_PAIR_QUEUE             = 0x40403723,   

    RETURN_CLN_BUTT
};

enum tagCPY_RETURN_VALUE_E
{
    RETURN_CPY_CREATED_TOOMUCH                     = 0x404036C1,   
    RETURN_CPY_COPYING_STATE_CANT_MODIFY_NAME    = 0x404036C2,   
    RETURN_CPY_FULLCPY_CANT_LABEL                 = 0x404036C3,   
    RETURN_CPY_COPYING_STATE_CANT_DELETE         = 0x404036C4,   
    RETURN_CPY_FAIL_TO_CREATE_PAIR                = 0x404036C5,   
    RETURN_CPY_ILLEGAL_OPERATION                  = 0x404036C6,   
    RETURN_CPY_FAIL_TO_MODIFY                     = 0x404036C7,   
    RETURN_CPY_NOT_PAUSE_STATE_CANT_CONTINUE    = 0x404036C8,   
    RETURN_CPY_FULLCPY_CANT_TAG                   = 0x404036C9,   
    RETURN_CPY_SEND_TO_PEER_OPERATION_FAILED    = 0x404036CA,   
    RETURN_CPY_PAUSE_STATE_CANT_PAUSE            = 0x404036CB,   
    RETURN_CPY_HAVE_NOT_DST                     = 0x404036CC,   
    RETURN_CPY_LUN_STATE_ERROR                   = 0x404036CD,                    
    RETURN_CPY_DSTLUN_TOOMUCH                    = 0x404036CE,   
    RETURN_CPY_ADD_DST_CONTROLLER_OFFLINE        = 0x404036CF,   
    RETURN_CPY_DEL_DST_CONTROLLER_OFFLINE        = 0x404036D0,   
    RETURN_CPY_ALREADY_CPY_MEMBER_LUN            = 0x404036D1,   
    RETURN_CPY_CANNOT_DEL_ONLY_DST               = 0x404036D2,   
    RETURN_CPY_SRCID_EQUAL_DSTID                 = 0x404036D3,   
    RETURN_CPY_SRC_LARGER_THAN_DST               = 0x404036D4,   
    RETURN_CPY_ABNORMAL_LUN                       = 0x404036D5,   
    RETURN_CPY_BOTH_EXTLUN                        = 0x404036D6,   
    RETURN_CPY_TYPE_ERROR                         = 0x404036D7,   
    RETURN_CPY_SPEED_ERROR                        = 0x404036D8,   
    RETURN_CPY_SYN_ERROR                          = 0x404036D9,   
    RETURN_CPY_STATUS_ERROR             = 0x404036DA,   
    RETURN_CPY_FAIL_TO_MODIFY_SPEED             = 0x404036DB,   
    RETURN_CPY_FAIL_TO_MODIFY_NAME              = 0x404036DC,   
     
    RETURN_CPY_INC_CANT_SHARE_SRCID            = 0x404036DD,   
    RETURN_CPY_CANT_SHARE_DSTID                 = 0x404036DE,   
    RETURN_CPY_OPERATION_FAILED          = 0x404036DF,   
    RETURN_CPY_DST_LUN_DOESNOT_EXIST           = 0x404036E0,   
    RETURN_CPY_COPYING_STATE_CANT_START        = 0x404036E1,   
    RETURN_CPY_STOP_STATE_CANT_PAUSE           = 0x404036E2,   
    RETURN_CPY_CREATE_STATE_CANT_PAUSE         = 0x404036E3,   
    RETURN_CPY_COMPLETE_STATE_CANT_START       = 0x404036E4,   

    RETURN_CPY_NO_LICENSE                       = 0x404036E5,   
    RETURN_CPY_LUN_DOESNOT_EXIST                = 0x404036E6,   
    RETURN_CPY_PAIRID_NOT_EXIST                 = 0x404036E7,   

    RETURN_CPY_BUTT
};
 

#endif
