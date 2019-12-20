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

#include "Log.h"
#include <stdarg.h>
#ifdef WIN32
#include "Shlwapi.h"
#endif

OsLogger g_objLogger;
OsLogger::OsLogger()
{
    OS_MutexInit(&m_Mutex);
    m_iMaxSize = UNIT_M * 3;
    m_iMode = ROLLING_TIME;
    m_iLogLevel = OS_LOG_DEBUG;
    m_iLogCategory = OS_LOG_CATA_DEBUG;
    memset_s(m_acFile, sizeof(m_acFile), 0, sizeof(m_acFile));
    memset_s(m_acLogfile, sizeof(m_acLogfile), 0, sizeof(m_acLogfile));
    memset_s(m_acPath, sizeof(m_acPath), 0, sizeof(m_acPath));
    m_iLogCount = 0; 
    m_pFileLog = NULL;
}

OsLogger::~OsLogger()
{
    try{
        OS_MutexDestroy(&m_Mutex);
        if (m_pFileLog != NULL){
            fclose(m_pFileLog);
        }
    }catch(...){}
}

int OsLogger::SetFile(char *pszFileName)
{
    char acLogPath[MAX_PATH_LEN];
    char acFileFullName[MAX_FILENAME_LEN];

    OS_Lock(&m_Mutex);
    memset_s(acFileFullName, sizeof(acFileFullName), 0, sizeof(acFileFullName));
    memset_s(acLogPath, sizeof(acLogPath), 0, sizeof(acLogPath));
    SNPRINTF(m_acFile, MAX_FILENAME_LEN, MAX_FILENAME_LEN - 1, "%s", pszFileName);

    (void)GetLogFileName(acFileFullName);

    if (strcmp(m_acLogfile, acFileFullName)){
        if (m_pFileLog != NULL){
            fclose(m_pFileLog);
            SNPRINTF(acLogPath, MAX_PATH_LEN, MAX_PATH_LEN - 1, "%s%s", m_acPath, PATH_SEPARATOR);
            strcat_s(acLogPath, sizeof(acLogPath), acFileFullName);

            char ca_name[MAX_PATH + 1] = {0x00};
            char *lppath = ca_name;

#ifdef WIN32
            if (strlen(acLogPath) > MAX_PATH || FALSE == PathCanonicalize(lppath, acLogPath)){
                return ISSP_RTN_FAIL;
            }
#else
            if (strlen(acLogPath) > MAX_PATH ){
                return ISSP_RTN_FAIL;
            }
            else{
                strcpy_s(lppath, MAX_PATH + 1, acLogPath);
            }
#endif
            m_pFileLog = fopen(lppath, "a+");

            if (NULL == m_pFileLog){
                OS_Unlock(&m_Mutex);
                return ISSP_RTN_FAIL;
            }

            SNPRINTF(m_acLogfile, MAX_FILENAME_LEN, MAX_FILENAME_LEN - 1, "%s", acFileFullName);
        }
    }

    OS_Unlock(&m_Mutex);
    return ISSP_RTN_OK;
}

int OsLogger::SetMaxFileSize(unsigned int iSize)
{
    OS_Lock(&m_Mutex);
    m_iMaxSize = (int)iSize;
    OS_Unlock(&m_Mutex);
    return ISSP_RTN_OK;
}

int OsLogger::GetLogFileName(char *pszFileFullName)
{
    memmove_s(pszFileFullName, MAX_FILENAME_LEN, m_acFile, strlen(m_acFile));
    switch (m_iMode){
    case ROLLING_TIME:
        break;

    case ROLLING_SIZE:
        break;

    default:
        return ISSP_RTN_FAIL;
    }

    return ISSP_RTN_OK;
}

int OsLogger::SetPath(char *pszPath)
{
    char acLogPath[MAX_PATH_LEN];
    char acFileFullName[MAX_FILENAME_LEN];

    OS_Lock(&m_Mutex);

    if (!OS_DirExists(pszPath)){
        if (ISSP_RTN_FAIL == OS_DirCreate(pszPath)){
            OS_Unlock(&m_Mutex);
            return ISSP_RTN_FAIL;
        }
    }

    memset_s(acFileFullName, sizeof(acFileFullName), 0, sizeof(acFileFullName));
    memset_s(acLogPath, sizeof(acLogPath), 0, sizeof(acLogPath));
    SNPRINTF(m_acPath, MAX_FULL_PATH_LEN, MAX_FULL_PATH_LEN - 1, "%s", pszPath);
    (void)GetLogFileName(acFileFullName);

    if (m_pFileLog != NULL){
        fclose(m_pFileLog);
        m_pFileLog = NULL;
        
        sprintf_s(acLogPath, sizeof(acLogPath), "%s%s", m_acPath, PATH_SEPARATOR);
        
        strcat_s(acLogPath, sizeof(acLogPath), acFileFullName);

        char ca_name[MAX_PATH + 1] = {0x00};
        char *lppath = ca_name;

#ifdef WIN32
        if (strlen(acLogPath) > MAX_PATH || FALSE == PathCanonicalize(lppath, acLogPath)){
            return ISSP_RTN_FAIL;
        }
#else
        if (strlen(acLogPath) > MAX_PATH ){
            return ISSP_RTN_FAIL;
        }
        else{
            strcpy_s(lppath, MAX_PATH + 1, acLogPath);
        }
#endif      

        m_pFileLog = fopen(lppath, "a+");
        if (NULL == m_pFileLog){
            OS_Unlock(&m_Mutex);
            return ISSP_RTN_FAIL;
        }
        
        sprintf_s(m_acLogfile, sizeof(m_acLogfile), "%s", acFileFullName);
    }

    OS_Unlock(&m_Mutex);
    return ISSP_RTN_OK;
}

int OsLogger::SetRollingMode(int iMode)
{
    OS_Lock(&m_Mutex);
    switch (iMode){
    case ROLLING_TIME:
    case ROLLING_SIZE:
        m_iMode = iMode;

        break;

    default:
        break;
    }

    OS_Unlock(&m_Mutex);
    return ISSP_RTN_OK;
}

void OsLogger::SetLevel(int iLevel)
{
    OS_Lock(&m_Mutex);
    m_iLogLevel = iLevel;
    OS_Unlock(&m_Mutex);
}

void OsLogger::SetCategory(int iCategory)
{
    OS_Lock(&m_Mutex);
    m_iLogCategory = iCategory;
    OS_Unlock(&m_Mutex);
}

void OsLogger::MkHead(int iLevel, char *pszHeadBuf, int iBufLen)
{
    switch (iLevel){
    case OS_LOG_DEBUG:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "DBG");
        break;
    case OS_LOG_INFO:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "INFO");
        break;
    case OS_LOG_WARN:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "WARN");
        break;
    case OS_LOG_ERROR:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "ERR");
        break;
    case OS_LOG_CRI:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "CRI");
        break;
    default:
        SNPRINTF(pszHeadBuf, iBufLen, iBufLen - 1, "%s", "");
        break;
    }
}

void OsLogger::Log(int iLevel, const char *pszFormat, ...)
{
    struct tm *pstCurTime;
    struct tm stCurTime;
    time_t lLongTime;
    char acFilefullname[MAX_FILENAME_LEN];
    char acLogPath[MAX_FULL_PATH_LEN];
    char acMsgHead[10];
    
    va_list pszArgp;
    long lPos;

    memset_s(acFilefullname, sizeof(acFilefullname), 0, sizeof(acFilefullname));
    memset_s(acLogPath, sizeof(acLogPath), 0, sizeof(acLogPath));

    va_start(pszArgp, pszFormat);

    if (iLevel < m_iLogLevel){
        return;
    }

    lLongTime = OS_Now();
    pstCurTime = (struct tm *)Os_LocalTime(&lLongTime, &stCurTime);
    if (NULL == pstCurTime){
        return ;
    }

    OS_Lock(&m_Mutex);
    sprintf_s(acFilefullname, sizeof(acFilefullname), "%s",m_acFile);
    switch (m_iMode){
    case ROLLING_TIME:
        break;

    case ROLLING_SIZE:
        break;

    default:
        break;
    }

    if ((strcmp(m_acLogfile, acFilefullname)) && (m_pFileLog != NULL)){
        fclose(m_pFileLog);
        m_pFileLog = NULL;
    }

    if (NULL == m_pFileLog){
        sprintf_s(acLogPath, sizeof(acLogPath), "%s%s",m_acLogfile,PATH_SEPARATOR);

        strcat_s(acLogPath, sizeof(acLogPath), acFilefullname);
        char ca_name[MAX_PATH + 1] = {0x00};
        char *lppath = ca_name;

#ifdef WIN32
        if (strlen(acLogPath) > MAX_PATH || FALSE == PathCanonicalize(lppath, acLogPath)){
            return;
        }
#else
        if (strlen(acLogPath) > MAX_PATH ){
            return;
        }
        else{
            strcpy_s(lppath, MAX_PATH + 1, acLogPath);
        }
#endif
        m_pFileLog = fopen(lppath, "a+");
        if (NULL == m_pFileLog){
            OS_Unlock(&m_Mutex);
            return ;
        }
        
        sprintf_s(m_acLogfile, sizeof(m_acLogfile), "%s", acFilefullname);
    }
    
    if(fseek(m_pFileLog, 0, SEEK_END) != 0){
        OS_Unlock(&m_Mutex);
        return ;
    }
    
    lPos = ftell(m_pFileLog);
    
    if ((ROLLING_SIZE == m_iMode) && (lPos >= m_iMaxSize)){
        fclose(m_pFileLog);
        sprintf_s(acLogPath, sizeof(acLogPath), "%s%s", m_acPath, PATH_SEPARATOR);
        strcat_s(acLogPath, sizeof(acLogPath), acFilefullname);
        char ca_name[MAX_PATH + 1] = {0x00};
        char *lppath = ca_name;
#ifdef WIN32
        if (strlen(acLogPath) > MAX_PATH || FALSE == PathCanonicalize(lppath, acLogPath)){
            return;
        }
#else
        if (strlen(acLogPath) > MAX_PATH ){
            return;
        }
        else{
            strcpy_s(lppath, MAX_PATH + 1, acLogPath);
        }
#endif        
        m_pFileLog = fopen(lppath, "w+");
        if (NULL == m_pFileLog){
            OS_Unlock(&m_Mutex);
            return ;
        }
        
        sprintf_s(m_acLogfile, sizeof(m_acLogfile), "%s", acFilefullname);
    }
    
    char *acMsg = new char[MAX_MSG_SIZE];
    if(acMsg == NULL){ 
        OS_Unlock(&m_Mutex);
        return;
    }
        
    memset_s(acMsg, MAX_MSG_SIZE, 0, MAX_MSG_SIZE);

#ifdef WIN32
    _vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#else
    (void)vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#endif

    memset_s(acMsgHead, sizeof(acMsgHead), 0, sizeof(acMsgHead));
    MkHead(iLevel, acMsgHead, 10);

#ifndef WIN32
    pthread_t pt = pthread_self();

    fprintf(m_pFileLog, "%4u-%02u-%02u %02u:%02u:%02u %ld %s: %s; \n",
        pstCurTime->tm_year + 1900,
        pstCurTime->tm_mon + 1,
        pstCurTime->tm_mday,
        pstCurTime->tm_hour,
        pstCurTime->tm_min,
        pstCurTime->tm_sec,
        pt,
        acMsgHead,
        acMsg);
#else
    fprintf_s(m_pFileLog, "%4u-%02u-%02u %02u:%02u:%02u %s: %s; \n",
        pstCurTime->tm_year + 1900,
        pstCurTime->tm_mon + 1,
        pstCurTime->tm_mday,
        pstCurTime->tm_hour,
        pstCurTime->tm_min,
        pstCurTime->tm_sec,
        acMsgHead,
        acMsg);
#endif

    fflush(m_pFileLog);
    OS_Unlock(&m_Mutex);
    va_end(pszArgp);
    delete [] acMsg;
    return ;
}

int OsLogger::SetLogCount(int iLogCount)
{
    OS_Lock(&m_Mutex);
    m_iLogCount = iLogCount;
    OS_Unlock(&m_Mutex);

    return ISSP_RTN_OK;
}

int OsLogger::CreateLogFile(char* pszLogFile)
{
    FILE* pFile = NULL;
    
    char ca_name[MAX_PATH + 1] = {0x00};
    char *lppath = ca_name;

#ifdef WIN32
    if (strlen(pszLogFile) > MAX_PATH || FALSE == PathCanonicalize(lppath, pszLogFile)){
        return ISSP_RTN_FAIL;
    }
#else
    if (strlen(pszLogFile) > MAX_PATH ){
        return ISSP_RTN_FAIL;
    }
    else{
        strcpy_s(lppath, MAX_PATH + 1, pszLogFile);
    }
#endif
    
    pFile = fopen(lppath, "w");
    if (NULL == pFile){
        return ISSP_RTN_FAIL;
    }

    fclose(pFile);

    return ISSP_RTN_OK;
}

int OsLogger::SwitchLogFile(const char* pszLogPath,const char* pszLogName, int iLogCount)
{
    int iRet = ISSP_RTN_OK;
    char acDestFile[MAX_FULL_PATH_LEN] = {0};
    
    char acBackFile[MAX_FULL_PATH_LEN] = {0};
    int i = iLogCount-1;

    std::string strTemp =pszLogPath + string(PATH_SEPARATOR) +pszLogName;
    std::string strback;
    std::ostringstream oss;

    oss<<i; 
    strback = strTemp + oss.str();
    memmove_s(acBackFile, MAX_FULL_PATH_LEN, strback.c_str(),sizeof(acBackFile) - 1);

    if (OS_FileExists(acBackFile)){
        (void)remove(acBackFile);
    }

    i--;
    for (; i >= 0; i--){
        oss.str("");
        if (0 == i){
            strback = strTemp;
            memmove_s(acBackFile, MAX_FULL_PATH_LEN, strback.c_str(), sizeof(acBackFile) - 1);
        }
        else{
            oss<<i;
            strback = strTemp + oss.str();
            memmove_s(acBackFile, MAX_FULL_PATH_LEN, strback.c_str(), sizeof(acBackFile) - 1);
        }

        if (!OS_FileExists(acBackFile)){
            continue;
        }

        oss.str("");
        oss<<i+1;
        strback = strTemp + oss.str();
        memmove_s(acDestFile, MAX_FULL_PATH_LEN, strback.c_str(), sizeof(acDestFile) - 1);
        iRet = rename(acBackFile, acDestFile);
        if( 0 != iRet){
            return iRet;
        }


    }

    iRet = CreateLogFile(acBackFile);
    if (ISSP_RTN_FAIL == iRet){
        return iRet;
    }

    return iRet;
}

void OsLogger::Log(int iLevel, 
                        const int iFileLine,
                        const char *pszFuncName,
                        const char *pszFormat, ...)
                        {
    struct tm *pstCurTime;
    struct tm stCurTime;
    time_t lLongTime;
    char acLogPath[MAX_FULL_PATH_LEN];
    char acMsgHead[10];
    
    va_list   pszArgp;
    long lPos;
    int iRet = 0;
    
    memset_s(acLogPath, sizeof(acLogPath), 0, sizeof(acLogPath));
    SNPRINTF(acLogPath, MAX_FULL_PATH_LEN, MAX_FULL_PATH_LEN - 1, "%s%s%s",m_acPath, PATH_SEPARATOR, m_acFile);

    va_start(pszArgp, pszFormat);

    if (iLevel < m_iLogLevel){
        return ;
    }

    lLongTime = OS_Now();
    pstCurTime = (struct tm *)Os_LocalTime(&lLongTime, &stCurTime);
    if (NULL == pstCurTime){
        return ;
    }

    OS_Lock(&m_Mutex);
    switch (m_iMode){
    case ROLLING_TIME:
        break;

    case ROLLING_SIZE:
        break;

    default:
        break;
    }

    char ca_name[MAX_PATH + 1] = {0x00};
    char *lppath = ca_name;
    
#ifdef WIN32
    if (strlen(acLogPath) > MAX_PATH || FALSE == PathCanonicalize(lppath, acLogPath)){
        return;
    }
#else
    if (strlen(acLogPath) > MAX_PATH ){
        return;
    }
    else{
        strcpy_s(lppath, MAX_PATH + 1, acLogPath);
    }
#endif

    if(!OS_FileExists(lppath)){
        iRet=CreateLogFile(lppath);
        if(ISSP_RTN_FAIL==iRet){
            OS_Unlock(&m_Mutex);
            va_end(pszArgp);
            return;
        }
    }
    else{
        if( NULL!=m_pFileLog ){
            fclose(m_pFileLog);
            m_pFileLog = NULL;
        }

        m_pFileLog = fopen(lppath, "a+");
        if (NULL == m_pFileLog){
            OS_Unlock(&m_Mutex);
            va_end(pszArgp);
            return ;
        }

        if(fseek(m_pFileLog, 0, SEEK_END) != 0){
            OS_Unlock(&m_Mutex);
            va_end(pszArgp);
            return ;
        }
        lPos = ftell(m_pFileLog);
        fclose(m_pFileLog);

        if ((ROLLING_SIZE == m_iMode) && (lPos >= m_iMaxSize)){
            iRet = SwitchLogFile(m_acPath, m_acFile, m_iLogCount);
            if (ISSP_RTN_FAIL == iRet){
                OS_Unlock(&m_Mutex);
                va_end(pszArgp);
                return;
            }
        }
    }

    m_pFileLog = fopen(lppath, "a+");
    if (NULL == m_pFileLog){
        OS_Unlock(&m_Mutex);
        va_end(pszArgp);
        return ;
    }
    char *acMsg = new char[MAX_MSG_SIZE];    
    if(acMsg == NULL){
        OS_Unlock(&m_Mutex);
        va_end(pszArgp);
        return;
    }
    memset_s(acMsg, MAX_MSG_SIZE, 0, MAX_MSG_SIZE);

#ifdef WIN32
    _vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#else
    (void)vsnprintf_s(acMsg, MAX_MSG_SIZE, MAX_MSG_SIZE - 1, pszFormat, pszArgp);
#endif

    memset_s(acMsgHead, sizeof(acMsgHead), 0, sizeof(acMsgHead));
    MkHead(iLevel, acMsgHead, 10);

#ifdef WIN32
    fprintf_s(m_pFileLog, "[%4u-%02u-%02u %02u:%02u:%02u][%s][HWSRA][%s,%ld]%s\n",
        pstCurTime->tm_year + 1900,
        pstCurTime->tm_mon + 1,
        pstCurTime->tm_mday,
        pstCurTime->tm_hour,
        pstCurTime->tm_min,
        pstCurTime->tm_sec,
        acMsgHead,
        pszFuncName,
        iFileLine,
        acMsg);
#else
    fprintf(m_pFileLog, "[%4u-%02u-%02u %02u:%02u:%02u][%s][HWSRA][%s,%ld]%s\n",
        pstCurTime->tm_year + 1900,
        pstCurTime->tm_mon + 1,
        pstCurTime->tm_mday,
        pstCurTime->tm_hour,
        pstCurTime->tm_min,
        pstCurTime->tm_sec,
        acMsgHead,
        pszFuncName,
        iFileLine,
        acMsg);
#endif

    fflush(m_pFileLog);
    OS_Unlock(&m_Mutex);
    va_end(pszArgp);
    delete [] acMsg;
    acMsg = NULL;

    return ;
}
