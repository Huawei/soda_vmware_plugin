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

#ifndef _COMMON_H
#define _COMMON_H

#include "./xmlserial/xmlreader.h"
#include "./xmlserial/xmlwriter.h"
#include "./error_code.h"
#include "os/BaseException.h"
#include "./os/Commf.h"
#include "./XmlInterface.h"
#include "./os/Log.h"

using namespace std;

#ifndef RETURN_OK
#define RETURN_OK 0
#endif

#ifndef RETURN_ERR
#define RETURN_ERR -1
#endif

#ifndef FALSE
#define FALSE    0
#endif

#ifndef TRUE
#define TRUE     1
#endif

enum CLI_TYPE
{
    INBAND_TYPE,
    OUTBAND_TYPE,
    VISOUT_TYPE,
    OUTBAND_REST_TYPE
};

#define CHECK_NULL(ptr, ret) \
{ \
    if ( NULL == (ptr) ) \
    { \
        return ret; \
    } \
}

#define CHECK_FALSE(ptr, ret) \
{ \
    if ( false == (ptr) ) \
    { \
        return ret; \
    } \
}

#define CHECK_MORETHAN(ptr1, ptr2, ret) \
{ \
    if ( (ptr1) > (ptr2) ) \
    { \
        return ret; \
    } \
}

#define CHECK_UNEQ(ptr, value)\
{ \
    if ((value) != (ptr))\
    { \
        return (value);\
    } \
}

#define CHECK_UNEQ_LOG(ptr, value)\
{ \
    if ((value) != (ptr))\
    { \
    COMMLOG(OS_LOG_ERROR, "assert fail[%s]!!!",#value);\
        return (value);\
    } \
}

#define CHECK_SIZE(ptr)\
{ \
    if (((ptr).size()) != 2)\
    { \
        return RETURN_ERR;\
    } \
}

#define CHECK_UNEQRTN(ptr, value)\
{ \
    if ((value) != (ptr))\
    { \
        return;\
    } \
}

#define OUT_RANGE(SRC, RAN_MIN, RAN_MAX) \
    (((RAN_MIN) > (SRC)) || ((RAN_MAX) < (SRC))) \


#define IN_RANGE(SRC, RAN_MIN, RAN_MAX) \
    (((RAN_MIN) <= (SRC)) && ((RAN_MAX) >= (SRC))) \


#define UNEQ_TO_FOUR(SRC, DESO, DESS, DEST, DESF) \
    (((DESO) != (SRC))&& \
    ((DESS) != (SRC))&& \
    ((DEST) != (SRC))&& \
    ((DESF) != (SRC)) )\

#define UNEQ_TO_THREE(SRC, DESO, DESS, DEST) \
    (((DESO) != (SRC))&& \
    ((DESS) != (SRC))&& \
    ((DEST) != (SRC))) \

#define UNEQ_TO_TWO(SRC, DESO, DESS) \
    (((DESO) != (SRC))&& \
    ((DESS) != (SRC))) \

#ifndef MIN_IN_PARA_NUM
#define MIN_IN_PARA_NUM 2
#endif

#define LENGTH_MESSAGE 1024

#define LENGTH_XMLNS                256

#define LEGNTH_PATH                 1024

#define LENGTH_COMMON               256

#define LENGTH_PROGRESS             10

#define LENGTH_COMMON_NAME 50

#define LENGTH_HOST_NAME   100

#define LENGTH_SQL_SENTENCE_MAX 128

#define MAX_TIME_LENGTH 100

#define LOG_DIR_NAME "log"

#define LOG_FILE_NAME "log.txt"

#define SRA_VERSION  "2.1.16"

#define COMMUNICATION_TYPE_INBAND   0
#define COMMUNICATION_TYPE_TLV      1
#define COMMUNICATION_TYPE_VIS      2
#define COMMUNICATION_TYPE_REST     3

#define NAME_STR_SEPARATOR ";"

#define THREAD_MAX_SIZE            20

#define ONE_SECOND_MSUNIT        1000    
#define MONITOR_ARRARY_LINK_STATUS    60  

#define DAR_HOST_SEPERATOR        ":"

extern int g_cli_type; 
extern char g_acHome[MAX_FULL_PATH_LEN + 1];

extern map<int, pair<string, string> > g_map_err_en;
extern map<int, pair<string, string> > g_map_err_cn;
extern map<int, pair<string, string> > g_map_warn_en;
extern map<int, pair<string, string> > g_map_warn_cn;

extern bool g_bIsThreadExit;                 
extern bool g_bnfs;                              
extern bool g_bstretch;                           
extern bool g_bFusionStorage;                    
extern bool g_testFusionStorageStretch;     
#endif
