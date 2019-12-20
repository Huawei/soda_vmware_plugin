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

#ifndef _COMMF_H_
#define _COMMF_H_

#ifdef WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4267)
#pragma warning(disable:4311) 
#endif

#define _WINSOCKAPI_

#include "Type.h"

#define OS_HOSTNAME_LEN  260

#define NOW_TIME_LENGTH  200

#define MAX_TIMET_VALUE  0x7FFFFFFF

#define TIME_INVALID_VALUE "0000-00-00 00:00:00"

#define LENGTH_ARRAY_PASS_ASCII 400

#define ARG_TMP_BUF      1024

/*Initialization semaphore*/
int OS_MutexInit(MUTEX *pMutex);

/*Destroy semaphore*/
int OS_MutexDestroy(MUTEX *pMutex);

/*Lock the specified semaphore for the specified time*/
int OS_TimeLock(MUTEX *pMutex, unsigned int uiLockTime);

/*Unlock the semaphore that is locked at the specified time*/
int OS_TimeUnlock(MUTEX *pMutex);

/*Lock the specified semaphore*/
int OS_Lock(MUTEX *pMutex);

/*Unlock the specified semaphore*/
int OS_Unlock(MUTEX *pMutex);

/*Determine if the specified file exists*/
bool OS_FileExists(char *pszFilePath);

int OS_DeleteFile(char *pszFilePath);

int OS_DirExists(const char *pszPath);

int OS_DirCreate(const char *pszPath);

/*Execution system call*/
int OS_SystemExec(char *pszCmdBuf, list <string> *plstParse, unsigned long *ulRetCode);

/*Convert integers to strings*/
string OS_IToString(int iParam);

/*Split the string according to the specified separator and store the result in the linked list*/
void OS_StrToken(IN string pszToken,
                      IN string pszSeparator,
                      OUT list<string> *plstStr);

/*Read a line in the file and store it in the specified buffer*/
int OS_FileGets(FILE *pstFp, char *pszStr, int iBufLen);

/*Get the current time of the system*/
time_t OS_Now(void);

/*Time to convert time_t structure to tm structure type*/
struct tm *Os_LocalTime(const time_t *plTime, struct tm *pstrTime);

/*Convert time_t structure to string in "%Y-%m-%d %X" format*/
string OS_Time_tToStr(time_t lTime);

void OS_Sleep(int iMSec);

int  SafeWideCharToMultiByte(char* pszDest,wchar_t *pszSrc);

int SafeMultiByteToWideChar(wchar_t* pszDest,char *pszSrc);

#ifdef WIN32
#define OS_SYS_LOG(...)
#else
#ifdef AIX
extern struct syslog_data g_stSysLogData;
#define OS_SYS_LOG(...) syslog_r(LOG_ERR|LOG_USER, &g_stSysLogData, __VA_ARGS__)
#else
#define OS_SYS_LOG(...) syslog(LOG_ERR|LOG_USER, __VA_ARGS__)
#endif
#endif

#ifdef WIN32
#define OS_atoi64(str) _atoi64(str)
#else
#define OS_atoi64(str) atoll(str)
#endif

#ifdef WIN32
#define OS_assert(exp) assert(exp)
#else
#define OS_assert(exp) assert(exp)
#endif

int TimeDiff(IN string const &strSrcTime, IN string strDescTime);
void Os_Char2Ascii(char *pszDstAscii, const char *pszSrcChar);

void min_time(const string& new_value, string& old_value);

#endif

