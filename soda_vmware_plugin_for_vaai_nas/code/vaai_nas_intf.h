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

#ifndef _VAAI_NAS_INTF_H_
#define _VAAI_NAS_INTF_H_

#define VAAI_NAS_MSG_VERSION   1


typedef enum enVAAI_NAS_SUPPORT_STATUS
{
    VAAI_NAS_SUPPORT = 0,           /*Hardware acceleration support */
    VAAI_NAS_NOT_SUPPORT = 1       /*Hardware acceleration is not supported*/
}VAAI_NAS_SUPPORT_STATUS;

typedef enum FS_ERROR_CODE
{
    SNAPSHOT_HIERARCHY_TOODEEP = 1001,
  SNAPSHOT_MAXSNAPSHOTS = 1002,
    OBJECT_IS_BUSY = 1003,
    DISK_FULL = 1004,
    E_BUTT 
}FS_ERROR_CODE_ENUM;

typedef enum VAAICMDTYPE{
    VAAICMD_DEFAULT     = 0,        /*Do not use */
    VAAICMD_FCLONE      = 1,        /*File cloning*/ 
    VAAICMD_FRESERVE    = 2,        /*File reservation */ 
    VAAICMD_FSTAT       = 3,        /*File extension attributes*/
    VAAICMD_FCLNPROC    = 4,        /*Get cloning progress information*/ 
    VAAICMD_FCLNABORT   = 5,        /*End the clone operation */
    VAAICMD_CLN_SUPPORT = 6,        /*Clone hardware accelerated support switch*/
    VAAICMD_RSV_SUPPORT = 7,        /*File reserved hardware acceleration supported switch */
    VAAICMD_STAT_SUPPORT= 8,        /*Extended status query hardware acceleration supported switch*/
    VAAICMD_BUTT
}VAAI_CMDTYPE_ENUM;

typedef struct tagFILE_CLONEINFO
{
    uint32_t    cloneFlag;          /* clone flag */
    uint32_t    nameLen;            /* Destination file path length */
    char        cloneFileName[256]; /* File path: The format of the destination file path */
}FILE_CLONEINFO_S;

typedef struct tagVAAI_PRIVATEDATA
{
    uint64_t version;               /* Message version*/
    VAAI_CMDTYPE_ENUM vaaiCmdType;  /* Command type */
    uint32_t pad1;                  /* Align field */
    FILE_CLONEINFO_S cloneData;     /* Clone data */
    uint64_t reserve[2];
}VAAI_PRIVATEDATA_S;

typedef struct tagFILE_EXSTATINFO{
    uint64_t    totalBytes;         /* Logical size */
    uint64_t    allocedBytes;       /*Actual size*/
    uint64_t    uniqueBytes;        /* Exclusive size */
    uint64_t    reserved;           /*Reserved*/
}FILE_EXSTATINFO_S;

typedef enum tagCLONE_PROC_FLAG {
    CLONEFILE_PROC_FLAG_START = 0,  /* Start task */
    CLONEFILE_PROC_FLAG_DOING,      /* Start data copy */
    CLONEFILE_PROC_FLAG_COMPLETE,   /* Copy completed */
    CLONEFILE_PROC_FLAG_BUTT
}CLONE_PROC_FLAG_ENUM;

typedef struct tagFILECLONE_PROCINFO{     
    uint64_t    cloneFlag;          /* Clone Flag */
    uint64_t    cloneOffset;        /* clone process */
}FILECLONE_PROCINFO_S;

typedef struct tagVAAI_OUTPUT_DATA
{
    uint64_t              version;      /* Message version */
    VAAI_CMDTYPE_ENUM     cmdtype;      /* Command word type */
    int32_t               result;       /* Internal result, used by VAAI-NAS to pass internal error code */
    FILE_EXSTATINFO_S     statinfo;     /* Extended status query results */
    FILECLONE_PROCINFO_S  cloneinfo;    /* Progress query results */
    uint64_t reserve[2];
}VAAI_OUTPUT_DATA_S;

#endif

