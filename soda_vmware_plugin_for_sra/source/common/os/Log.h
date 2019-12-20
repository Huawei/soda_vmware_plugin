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

#ifndef _LOG_H_
#define _LOG_H_

#include "Commf.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

/*Print level*/
#define OS_LOG_DEBUG 0
#define OS_LOG_INFO  1
#define OS_LOG_WARN  2
#define OS_LOG_ERROR 3
#define  OS_LOG_CRI  4

/*Print signature*/
#define SIGNATURE1 "--------------------------------------------------------------------"
#define SIGNATURE2 "OpenSDS_Storage_SRA_v2.1.16 (build date 2019/10/09)"

#define OS_LOG_CATA_DEBUG   0
#define OS_LOG_CATA_RELEASE 1

#define ROLLING_TIME 0
#define ROLLING_SIZE 1

#define MAX_HEAD_SIZE 16
#define MAX_MSG_SIZE  30960

#define MODEL_NAME "HWSRA"

#define ADDLOG(x) ADDLLU(x)
#define ADDLLU(x) x##LLU

#ifdef WIN32
#define COMMLOG(iLevel, pszFormat, ...) g_objLogger.Log(iLevel, __LINE__, __FUNCTION__,  pszFormat, __VA_ARGS__)
#else
#define COMMLOG(iLevel, pszFormat, args...) g_objLogger.Log(iLevel, __LINE__, __FUNCTION__, pszFormat, ##args)
#endif

#ifndef WIN32
#define MAX_PATH  260
#define FALSE  false
#endif


class OsLogger
{
public:
    OsLogger();
    ~OsLogger();

    int SetFile(char *pszFileName);
    int SetPath(char *pszPath);
    void  SetLevel(int iLevel);
    void  SetCategory(int iCategory);
    int SetMaxFileSize(unsigned int iSize);
    int SetRollingMode(int iMode);
    int SetLogCount(int iLogCount);
    int SwitchLogFile(const char* pszLogPath,const char* pszLogName, int iLogCount);
    int CreateLogFile(char* pszLogFile);
    void  Log(int iLevel, const char *pszFormat, ...);
    void  Log(int iLevel, const int iFileLine, const char* pszFuncName, const char * pszFormat, ...);

private:
    MUTEX  m_Mutex;
    char m_acFile[MAX_FILENAME_LEN];  /* Log file name */
    char m_acLogfile[MAX_FILENAME_LEN]; /* The full name of the log file, scrolled by time with timestamp */
    char m_acPath[MAX_FULL_PATH_LEN];  /* Log file path */
    int m_iMode;
    int m_iMaxSize;
    FILE *m_pFileLog;
    int m_iLogLevel;
    int m_iLogCategory; /*Log type, in debug mode or release mode*/
    int m_iLogCount;//Number of journal files

    void MkHead(int iLevel, char *pszHeadBuf, int iBufLen);
    int GetLogFileName(char *pszFileFullName);
};

extern OsLogger g_objLogger;
#endif
