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

#include "Commf.h"

char g_acHome[MAX_FULL_PATH_LEN + 1];

int g_cli_type = 0;


int g_iOsErrno = 0;

#ifdef WIN32
typedef void (*Init_Routine)(void);
#define UTIL_ONCE_UNINITIALIZED 0
#define UTIL_ONCE_INITIALIZING  1
#define UTIL_ONCE_INITIALIZED   2

#define UTIL_ONCE_WAIT   10

#define EXECUTE_ONCE_T long
#define DECLARE_ONCE_T(varname)  static long varname = UTIL_ONCE_UNINITIALIZED;

DWORD g_ulTlsInx;
#else
#define EXECUTE_ONCE_T pthread_once_t
#define DECLARE_ONCE_T(varname)  static pthread_once_t varname = PTHREAD_ONCE_INIT;
#define DECLARE_RETCODE(retcode) errno = retcode;
#endif

#ifdef WIN32
#else
static pthread_key_t os_tl_key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
#ifdef AIX
struct syslog_data g_stSysLogData = SYSLOG_DATA_INIT;
#endif

extern "C"{
    static void Thread_Init(void){
        (void)pthread_key_create(&os_tl_key, NULL);
    }
}

#endif

int OS_MutexInit(MUTEX *pMutex)
{
#ifdef WIN32
    *pMutex = CreateMutex(NULL, false, NULL);
    if (NULL == *pMutex){
        return ISSP_RTN_FAIL;
    }
#else
    if (0 != pthread_mutex_init(pMutex, NULL)){
        OS_SYS_LOG("OS_MutexInit error!\n");
        return ISSP_RTN_FAIL;
    }
#endif

    return ISSP_RTN_OK;
}

int OS_MutexDestroy(MUTEX *pMutex)
{
#ifdef WIN32
    if (NULL != *pMutex){
        CloseHandle(*pMutex);
    }
#else
    if (NULL != pMutex){
        pthread_mutex_destroy(pMutex);
    }
#endif

    return ISSP_RTN_OK;
}

int OS_TimeLock(MUTEX *pMutex, unsigned int uiLockTime)
{
#ifdef WIN32
    HANDLE pTempMutex;
    
    DWORD iRetCode;

    if (NULL == *pMutex){
        return ISSP_RTN_FAIL;
    }

    pTempMutex  = (HANDLE)(*pMutex);

    if (0 == uiLockTime){
        iRetCode = WaitForSingleObject(pTempMutex, INFINITE);
        if ((iRetCode == WAIT_FAILED)){
            return ISSP_RTN_FAIL;
        }

        return ISSP_RTN_OK;
    }

    iRetCode = WaitForSingleObject(pTempMutex, (DWORD)uiLockTime * 1000);
    if ((iRetCode == WAIT_FAILED) || (iRetCode == WAIT_TIMEOUT)){
        return ISSP_RTN_FAIL;
    }

    return ISSP_RTN_OK;
#else
    pthread_mutex_t *pTempMutex;

    if (NULL == pMutex){
        return ISSP_RTN_FAIL;
    }

    pTempMutex = (pthread_mutex_t *)pMutex;
    if (pthread_mutex_lock(pTempMutex)){
        return ISSP_RTN_FAIL;
    }
    else{
        return ISSP_RTN_OK;
    }
#endif
}

int OS_TimeUnlock(MUTEX *pMutex)
{
#ifdef WIN32
    HANDLE pTmpMutex;

    if (NULL == *pMutex){
        return ISSP_RTN_FAIL;
    }

    pTmpMutex = (HANDLE) (*pMutex);
    (void)ReleaseMutex(pTmpMutex);
#else
    pthread_mutex_t *pTmpMutex;

    if (NULL == pMutex){
        return ISSP_RTN_FAIL;
    }

    pTmpMutex = (pthread_mutex_t *)pMutex;
    if (pthread_mutex_unlock(pTmpMutex)){
        return ISSP_RTN_FAIL;
    }
#endif

    return ISSP_RTN_OK;
}

int OS_Lock(MUTEX *pMutex)
{
    return OS_TimeLock(pMutex, 0);
}

int OS_Unlock(MUTEX *pMutex)
{
    return OS_TimeUnlock(pMutex);
}

int OS_SystemExec(char *pszCmdBuf, list<string> *plstParse, unsigned long *ulRetCode)
{
    if(NULL == pszCmdBuf){
        return ISSP_RTN_FAIL;
    }
#ifdef WIN32
    STARTUPINFO stStartupInfo;
    PROCESS_INFORMATION stProcessInfo;
    DWORD dwCode = 0;

    memset_s(&stStartupInfo, sizeof(stStartupInfo), 0, sizeof(stStartupInfo));
    stStartupInfo.cb = sizeof(stStartupInfo);
    stStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    stStartupInfo.wShowWindow = SW_HIDE;
    memset_s(&stProcessInfo, sizeof(stProcessInfo), 0, sizeof(stProcessInfo));

    if(!CreateProcess(NULL,   // No module name (use command line).
        
        (LPTSTR)pszCmdBuf,
        NULL,             // Process handle not inheritable.
        NULL,             // Thread handle not inheritable.
        false,            // Set handle inheritance to false.
        0,                // No creation flags.
        NULL,             // Use parent's environment block.
        NULL,             // Use parent's starting directory.
        &stStartupInfo,              // Pointer to STARTUPINFO structure.
        &stProcessInfo)             // Pointer to PROCESS_INFORMATION structure.
        ){
        return ISSP_RTN_FAIL;
    }

    DWORD iRetCode = WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
    if ((iRetCode == WAIT_FAILED)){
        return ISSP_RTN_FAIL;
    }

    GetExitCodeProcess(stProcessInfo.hProcess, &dwCode);
    *ulRetCode = dwCode;

    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);

#else
    int iReturn;
    FILE *pStream = NULL;
    char TempBuf[1000];
    if (NULL == plstParse){
        iReturn = system(pszCmdBuf);
        if (iReturn != 0){
            return ISSP_RTN_FAIL;
        }
    }
    else{
        pStream = popen(pszCmdBuf, "r");
        if (NULL == pStream){
            return ISSP_RTN_FAIL;
        }

        while (!feof(pStream)){
            memset_s(TempBuf, sizeof(TempBuf), 0, sizeof(TempBuf));
            OS_FileGets(pStream, TempBuf, sizeof(TempBuf));
            if ((0 == TempBuf[0])
                || ('\n' == TempBuf[0])){
                continue;
            }

            plstParse->push_back(string(TempBuf));
        }

        pclose (pStream);
    }
#endif

    return ISSP_RTN_OK;
}

/* convert time_t structure to tm structure type */
struct tm *Os_LocalTime(const time_t *plTime, struct tm *pstrTime)
{
    struct tm *pTime;

    if ((plTime == NULL) || (pstrTime == NULL)){
        return NULL;
    }

    memset_s(pstrTime, sizeof(struct tm), 0, sizeof(struct tm));

    tzset();

#ifdef WIN32
    pTime = localtime(plTime);
    if(pTime){
        memcpy_s(pstrTime, sizeof(struct tm), pTime, sizeof(struct tm));
    }
    return pstrTime;
#else
    return localtime_r(plTime, pstrTime);
#endif
}

bool OS_FileExists(char *pszFilePath)
{
    if(NULL == pszFilePath){
        return false;
    }
#ifdef WIN32
    HANDLE pvFile = NULL;
    WIN32_FIND_DATA stFindFileData;

    pvFile = FindFirstFileA((LPCSTR)pszFilePath, &stFindFileData);
    if (INVALID_HANDLE_VALUE == pvFile) {
        return false;
    }
    
    if(!FindClose(pvFile)){
        return false;
    }

    return true;
#else
    int iRet;
    iRet = access(pszFilePath, F_OK);
    if (0 == iRet){
        return true;
    }
    return false;
#endif
}

int OS_DeleteFile(char *pszFilePath)
{
    if(NULL == pszFilePath){
        return false;
    }
#ifdef WIN32
    DeleteFileA((LPCSTR)pszFilePath);
    return ISSP_RTN_OK;
#else
    char acCmd[512];
    memset_s(acCmd, sizeof(acCmd), 0, sizeof(acCmd));
    SNPRINTF(acCmd, sizeof(acCmd) , sizeof(acCmd) - 1, "rm %s >/dev/null 2>&1", pszFilePath);
    if (!system(acCmd)){
        return ISSP_RTN_OK;
    }

    return ISSP_RTN_FAIL;
#endif
}

string OS_IToString(int iParam)
{
    char acTemp[20];

    SNPRINTF(acTemp, sizeof(acTemp), sizeof(acTemp) - 1, "%d", iParam);
    return string(acTemp);
}

void OS_StrToken(IN string strToken,
    IN string strSeparator,
    OUT list<string> *plstStr){
    if (NULL == plstStr){
        return;
    }
    plstStr->clear();

    if (strToken.empty() || strSeparator.empty()){
        return;
    }

    string::size_type strStart = 0;
    string::size_type strIndex = strToken.find(strSeparator);
    string::size_type strLen = strIndex - strStart;

    while (strIndex != string::npos){
        plstStr->push_back(strToken.substr(strStart, strLen));
        strStart = ++strIndex;
        strIndex = strToken.find(strSeparator, strStart);
        strLen = strIndex - strStart;

        if (strIndex == string::npos){
            plstStr->push_back(strToken.substr(strStart, strToken.length()));
        }
    }

    if (plstStr->empty()){
        plstStr->push_back(strToken);
    }

    return;
}

int OS_DirExists(const char *pszPath)
{
    if ((NULL == pszPath) || (0 == strlen(pszPath))){
        return ISSP_RTN_FAIL;
    }

#ifdef WIN32
    HANDLE pvFile = NULL;
    char acTmpPath[MAX_PATH_LEN];
    WIN32_FIND_DATA stFindFileData;
    memset_s(&stFindFileData, sizeof(stFindFileData), 0, sizeof(stFindFileData));

    memset_s(acTmpPath, sizeof(acTmpPath), 0, sizeof(acTmpPath));
    STRNCPY(acTmpPath, sizeof(acTmpPath), pszPath, strlen(pszPath));

    if ('\\' == acTmpPath[strlen(acTmpPath) - 1]){
        acTmpPath[strlen(acTmpPath) - 1] = '\0';
    }

    pvFile = FindFirstFile(acTmpPath, &stFindFileData);
    if (INVALID_HANDLE_VALUE == pvFile){
        return false;
    }

    if(stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
        return FindClose(pvFile);
    }
    else{
        if(!FindClose(pvFile))
            return false;
    }

#else
    DIR *pDirectoryPointer = opendir(pszPath);
    if (pDirectoryPointer != NULL){
        closedir(pDirectoryPointer);
        return true;
    }
#endif

    return false;
}

int OS_DirCreate(const char *pszPath)
{
#ifdef WIN32
    int iRet;

    if ((NULL == pszPath) || (0 == strlen(pszPath))){
        return ISSP_RTN_FAIL;
    }

    iRet = _mkdir(pszPath);
    if (ISSP_RTN_OK == iRet){
        return ISSP_RTN_OK;
    }
#else
    char acMkdirCmd[MAX_PATH_LEN];
    memset_s(acMkdirCmd, MAX_PATH_LEN, 0, MAX_PATH_LEN);
    SNPRINTF(acMkdirCmd, sizeof(acMkdirCmd), sizeof(acMkdirCmd) - 1, "mkdir %s >/dev/null 2>&1", pszPath);
    if (0 == system(acMkdirCmd)){
        return ISSP_RTN_OK;
    }
#endif
    return ISSP_RTN_FAIL;
}

string OS_Time_tToStr(time_t lTime)
{
    string strNowTime;
    struct tm stTime;
    struct tm *pT;
    char acTime[NOW_TIME_LENGTH] = {0};

    if ((lTime < 0)||(lTime >= MAX_TIMET_VALUE)){
        strNowTime = TIME_INVALID_VALUE;

        return strNowTime;
    }

    tzset();
    pT = Os_LocalTime(&lTime, &stTime);
    if(pT != NULL)
        strftime(acTime, sizeof(acTime), "%Y-%m-%d %X",pT);
    
    strNowTime = acTime;

    return strNowTime;
}

int OS_FileGets(FILE *pstFp, char *pszStr, int iBufLen)
{
    char acBuf[1000];

    memset_s(acBuf, sizeof(acBuf), 0, sizeof(acBuf));
    (void)fgets(acBuf, sizeof(acBuf), pstFp);
    if (!strlen(acBuf)){
        return ISSP_RTN_FAIL;
    }

    acBuf[strlen(acBuf) - 1] = 0;
    STRNCPY(pszStr, iBufLen, acBuf, strlen(acBuf));

    return ISSP_RTN_OK;
}

void OS_Sleep(int iMSec)
{
#ifdef WIN32
    if (iMSec < 0){
        return;
    }

    Sleep(iMSec);
#else
    struct timeval stTimeOut;
    if (iMSec < 0){
        return;
    }

    stTimeOut.tv_sec  = iMSec / 1000;
    stTimeOut.tv_usec = (iMSec % 1000) * 1000;
    (void)select(0, NULL, NULL, NULL, &stTimeOut);
#endif
}

time_t OS_Now()
{
    time_t lParTime;
    time_t lRetTime;

    tzset();
    lRetTime = time(&lParTime);

    return lRetTime;
}

int SafeWideCharToMultiByte(char *pszDest,wchar_t *pszSrc)
{
    int dwNum = 0;

#ifdef WIN32
    if ((NULL != pszSrc)&&(NULL != pszDest)){
        dwNum = WideCharToMultiByte(CP_UTF8, 0, pszSrc, -1, NULL, 0, NULL, FALSE);
        if (dwNum <= 0 ){
            return ISSP_RTN_FAIL;
        }

        dwNum = WideCharToMultiByte(CP_UTF8, 0, pszSrc, -1, pszDest ,dwNum, NULL, FALSE);
        if (0 == dwNum){
            return ISSP_RTN_FAIL;
        }

        return ISSP_RTN_OK;
    }
#else
    if ((NULL != pszSrc)&&(NULL != pszDest)){
        size_t uiLen = wcslen(pszSrc);

        if (-1 == wcstombs(pszDest, pszSrc, uiLen)){
            return ISSP_RTN_FAIL;
        }

        pszDest[uiLen] = 0;
        return ISSP_RTN_OK;
    }
#endif

    return ISSP_RTN_FAIL;
}

int SafeMultiByteToWideChar(wchar_t* pszDest,char *pszSrc)
{
    int dwNum = 0;

#ifdef WIN32
    if ((NULL != pszSrc)&&(NULL != pszDest)){
        dwNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
        if (dwNum <= 0 ){
            return ISSP_RTN_FAIL;
        }

        dwNum = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pszDest,dwNum);
        if (0 == dwNum){
            return ISSP_RTN_FAIL;
        }

        return ISSP_RTN_OK;
    }
#else
    if ((NULL != pszSrc)&&(NULL != pszDest)){
        size_t uiLen = strlen(pszSrc);

        if (-1 == mbstowcs(pszDest, pszSrc, uiLen)){
            return ISSP_RTN_FAIL;
        }

        pszDest[uiLen] = 0;
        return ISSP_RTN_OK;
    }
#endif
    return ISSP_RTN_OK;
}

int TimeDiff(IN string const &strSrcTime, IN string strDescTime)
{
    int iRet;

    iRet = strcmp(strDescTime.c_str(), strSrcTime.c_str());

    return iRet;
}

void Os_Char2Ascii(char *pszDstAscii, const char *pszSrcChar)
{
    if ((NULL == pszDstAscii)
        || (NULL == pszSrcChar)
        || (0 == strlen(pszSrcChar))){
        return;
    }

    int pszStrCharLength = (int)strlen(pszSrcChar);
    for (int i = 0; i < pszStrCharLength; i++){
        SPRINTF(pszDstAscii, LENGTH_ARRAY_PASS_ASCII, LENGTH_ARRAY_PASS_ASCII - 1, "%s%03d", pszDstAscii, pszSrcChar[i]);
    }
}

void min_time(const string& new_value, string& old_value)
{
    if (old_value.empty() || old_value > new_value){
        old_value = new_value;
    }
}
