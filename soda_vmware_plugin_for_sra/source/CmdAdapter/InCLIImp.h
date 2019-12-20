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

#ifndef INCLI_IMP_H
#define INCLI_IMP_H

#include "../common/cli/rslt_parser.h"

typedef struct tag_HOST_ID_INFO
{
    string strHostGroupID;
    bool IsMapToGroup;         //Whether to map to the host group ID, the identifier is passed in as lungroupID
} HOST_ID_INFO_STRU;

#define DEVICE_NOT_EXIST   2228225  //device does not exist.
#define MAP_NULL_MORE_ONE  2228236  //device does not exist or the specified array sn is not unique
#define VOLUME_NOT_EXIST   50150005 //Error code that does not exist when the volume is deleted

#define STATUS_IMAGE_CHAR_DISABLE         1  //Snapshot disabled state
#define STATUS_IMAGE_CHAR_ACTIVE          2  //Snapshot active state
#define STATUS_IMAGE_CHAR_ROLLBACK        3  //Snapshot rollback state
#define STATUS_IMAGE_CHAR_ACTIVE_FAULT    4  //Snapshot activation (fault) status
#define STATUS_IMAGE_CHAR_DISABLE_FAULT   5  //Snapshot disabled (fault) status

#define STATUS_IMAGE_CHAR_INVALID  0  //Invalid snapshot status

//Virtual snapshot type
#define TYPE_IMAGE_PUBLIC                 1
#define TYPE_IMAGE_PRIVATE_2              2
#define TYPE_IMAGE_PRIVATE_3              3
#define TYPE_IMAGE_TIME                   4
#define TYPE_IMAGE_INVALID                0

#define LINK_STATUS_UNCONNECT    0
#define LINK_STATUS_INVALID     -1 

#define MIRROR_LUN_RELATION_MASTER          1
#define MIRROR_LUN_RELATION_SLAVE           2

//Mirror relationship definition
#define MIRROR_RELATION_MASTER            "Primary"
#define MIRROR_RELATION_SLAVE             "Slave"

#define MIRROR_STATUS_NORMAL      1
#define MIRROR_STATUS_FAULT       0
#define MORROR_STATUS_NOT_EXIST   4

#define MIRROR_STATUS_CHAR_SYNCHRONIZED     24
#define MIRROR_STATUS_CHAR_CONSISTENT       22
#define MIRROR_STATUS_CHAR_SYNCHRONIZING    23
#define MIRROR_STATUS_CHAR_INITIALSYN       21
#define MIRROR_STATUS_CHAR_INCONSISTENT     0
#define MIRROR_STATUS_CHAR_NOT_EXIST        0

#define MIRROR_SLAVE_PAIR_STATUS_STR_NORMAL        "Normal"
#define MIRROR_SLAVE_PAIR_STATUS_STR_SYNCHRONIZING "Synchronizing"

#define MIRROR_SLAVE_PAIR_STATUS_STR_INTERRUPTED   "Interrupted"
#define MIRROR_SLAVE_PAIR_STATUS_STR_TOBERECOVERED "Toberecovered"
#define MIRROR_SLAVE_PAIR_STATUS_STR_SPLITED       "Splited"
#define MIRROR_SLAVE_PAIR_STATUS_STR_INVALID       "Invalid"

#define MIRROR_SLAVE_PAIR_STATUS_CHAR_NORMAL         1
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING  23
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED    34
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_TOBERECOVERED  33
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_SPLITED        26
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_INVALID        35
#define MIRROR_SLAVE_PAIR_STATUS_CHAR_NOT_EXIST      0

/*远程镜像一致性组状态宏定义*/
#define MIRROR_GROUP_STATUS_STR_NORAML         "Normal"
#define MIRROR_GROUP_STATUS_STR_SYNCHRONIZING  "Synchronizing"
#define MIRROR_GROUP_STATUS_STR_INTERRUPTED    "Interrupted"
#define MIRROR_GROUP_STATUS_STR_TOBERECOV      "ToBeRecov"
#define MIRROR_GROUP_STATUS_STR_SPLITED        "Splited"
#define MIRROR_GROUP_STATUS_STR_INVALID        "Invalid"
#define MIRROR_GROUP_STATUS_STR_FAULT          "Fault"

#define MIRROR_GROUP_STATUS_NORAML        1
#define MIRROR_GROUP_STATUS_SYNCHRONIZING 23
#define MIRROR_GROUP_STATUS_INTERRUPTED   34
#define MIRROR_GROUP_STATUS_TOBERECOV     33
#define MIRROR_GROUP_STATUS_SPLITED       26
#define MIRROR_GROUP_STATUS_INVALID       35
#define MIRROR_GROUP_STATUS_FAULT         0
#define MIRROR_GROUP_STATUS_NOT_EXIST     4

#define HM_SLAVE_PAIR_STATUS_CHAR_NORMAL            1
#define HM_SLAVE_PAIR_STATUS_CHAR_SYNCHRONIZING        23
#define HM_SLAVE_PAIR_STATUS_CHAR_INTERRUPTED        34
#define HM_SLAVE_PAIR_STATUS_CHAR_PAUSE                41
#define HM_SLAVE_PAIR_STATUS_CHAR_INVALID            35
#define HM_SLAVE_PAIR_STATUS_CHAR_FORCEDSTART        93
#define HM_SLAVE_PAIR_STATUS_CHAR_ERROR                94
#define HM_SLAVE_PAIR_STATUS_CHAR_TOBESYNCHRONIZED  100
#define HM_SLAVE_PAIR_STATUS_CHAR_NOT_EXIST            4
#define HM_SLAVE_PAIR_STATUS_CHAR_UNKNOW            0

//hypermetro health status
#define HM_STATUS_NORMAL           1
#define HM_STATUS_FAULT            2
#define HM_STATUS_SYNCHRONIZING    23

//hypermetro access permission
#define HM_ACCESS_DENY 1
#define HM_ACCESS_READ_ONLY 2
#define HM_ACCESS_READ_WRITE 3
#define HM_ACCESS_ERROE -1

#define HC_DATA_INTEGRITY 1
#define HC_DATA_INCOMPLETENESS 2

//hypermetro running status
#define HM_STATUS_CHAR_NORMAL           "1"
#define HM_STATUS_CHAR_FAULT            "0"
#define HM_STATUS_CHAR_SYNCHRONIZED     24
#define HM_STATUS_CHAR_CONSISTENT       22
#define HM_STATUS_CHAR_SYNCHRONIZING    23
#define HM_STATUS_CHAR_INITIALSYN       21
#define HM_STATUS_CHAR_INCONSISTENT     0
#define HM_STATUS_CHAR_NOT_EXIST        4
#define HM_STATUS_CHAR_TOBESYNCHRONIZED 100

#define HM_SLAVE_PAIR_STATUS_STR_NORMAL                "Normal"
#define HM_SLAVE_PAIR_STATUS_STR_SYNCHRONIZING        "Synchronizing"
#define HM_SLAVE_PAIR_STATUS_STR_INTERRUPTED        "Interrupted"
#define HM_SLAVE_PAIR_STATUS_STR_INVALID            "Invalid"
#define HM_SLAVE_PAIR_STATUS_STR_PAUSE                "Pause"
#define HM_SLAVE_PAIR_STATUS_STR_TOBESYNCHRONIZED    "Tobesynchronized"

#define HM_ISOLATIONREQUIRED_TRUE                "true"
#define HM_ISOLATIONREQUIRED_FALSE                "false"

#define HM_RELATION_MASTER            "Primary"
#define HM_RELATION_SLAVE             "Slave"

#define HM_CPTYPE_NULL                    "3"

#define HM_LUN_RELATION_MASTER          1
#define HM_LUN_RELATION_SLAVE           2

#define HM_GROUP_RELATION_MASTER          1
#define HM_GROUP_RELATION_SLAVE           2

#define HM_SUPPORT_STRETCHED            "true"
#define HM_SUPPORT_NOT_STRETCHED        "false"

#define HYPERMETRO_LUN_TAG                    "MP"
#define HYPERMETRO_LUN_CONSISTENT_TAG        "MG"
#define MIRROR_LUN_TAG                        "RP"
#define MIRROR_LUN_CONSISTENT_TAG            "RG"

#define HM_CONSISGROUP_DATA_FROM_LOCAL_TO_REMOTE    1
#define HM_CONSISGROUP_DATA_FROM_REMOTE_TO_LOCAL    2

#define  MODE_SYNCHRONOUS        1
#define  MODE_ASYNCHRONOUS       2

#define INT_LUN_STATUS_NORMAL                  1
#define INT_LUN_STATUS_FAULT                   2
#define INT_LUN_STATUS_NOT_FORMATTED           3
#define INT_LUN_STATUS_NOT_FORMATTING          4
#define INT_LUN_STATUS_NOT_EXIST               0

#define CONSISTENTGROUP_PRE "G_"
#define HM_SPLITED              "Splited"
#define HM_SYNCING              "Synchronizing"
#define HM_INTERRUPT            "Interrupted"
#define SYNC_COMPLETE "100"
#define SYNC_COMPLETE_FLAG 100
#endif
