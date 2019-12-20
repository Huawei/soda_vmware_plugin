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

#ifndef _TYPE_H_
#define _TYPE_H_

#define _CRT_SECURE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <wchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <vector>

#include "securec.h"

#ifdef WIN32
#pragma warning(disable:4996)
#pragma warning(disable: 4819)  /* more than 64k source lines */

#pragma comment (linker,"/NODEFAULTLIB:libcd.lib")
#include <winsock2.h>
#include <windows.h>
#include <WinBase.h>
#include <process.h>
#include <io.h>
#include <direct.h>
#include <nb30.h>
#include <conio.h>
#include <Wincrypt.h>

#include <winioctl.h>
#include <ntddscsi.h>
#include <stddef.h>
#include <Shlwapi.h>
#include <devguid.h>
#include <Dbt.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <comdef.h>
#include <Wbemidl.h>

#include <Iphlpapi.h>
#include <Winternl.h>
#include "setupapi.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")
#pragma comment(lib, "shlwapi.lib")

#else
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <dlfcn.h>
#include <locale.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#ifdef SOLARIS
#include <kstat.h>
#include <sys/utsname.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <stropts.h>
#include <netdb.h>
#include <iconv.h>
#include <sys/sysinfo.h>
#include <syslog.h>
#endif
#ifdef HP_UX_C
#include <iconv.h>
#include <sys/pstat.h>
#include <syslog.h>
#endif
#ifdef AIX
#include <libperfstat.h>
#include <sys/syslog.h>
#include "/usr/include/sys/hbaapi.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <ctype.h>
#include <stropts.h>
#include <sys/scsi.h>
#include <sys/scsi_buf.h>
#include <sys/serdasd.h>
#include <sys/errno.h>
#include <standards.h>
#include <sys/ttychars.h>
#include <sys/ttydev.h>
#include <string.h>
#include <pwd.h>
#include <libperfstat.h>
# pragma comment(lib, "libperfstat.a")
#endif
#ifdef LINUX
#include <sys/vfs.h>
#include <sys/sysinfo.h>
#include <sys/syslog.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/scsi.h>
#include <ctype.h>
#include <iconv.h>

#endif
#endif

#if defined LINUX
#define MAX_SEND_SCSI_CMD_TIMEOUT 60 * 1000
#elif defined WIN32
#define MAX_SEND_SCSI_CMD_TIMEOUT 60
#elif defined AIX
#define MAX_SEND_SCSI_CMD_TIMEOUT 60
#endif

using namespace std;

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define WIN_PATH_SEPARATOR "\\"
#define UNIX_PATH_SEPARATOR "/"

#define UNIT_K 1024
#define UNIT_M 1048576

#define LENGTH_COMMON_IP   30 
#define LENGTH_COMMON_PORT 15 

#define MAX_PATH_LEN      260
#define MAX_FULL_PATH_LEN 300
#define MAX_FILENAME_LEN  80
#define MAX_IP_LEN   32
#define MAX_PORT_LEN 16
#define MAX_SERIAL_LEN 32
#define MAX_USER_LEN 128
#define MAX_PWD_LEN  128

#ifdef WIN32
#define PATH_SEPARATOR WIN_PATH_SEPARATOR
#define MUTEX     HANDLE
#define COND      HANDLE
#define THREAD    HANDLE
#define KEY       HANDLE
#else
#define PATH_SEPARATOR UNIX_PATH_SEPARATOR
#define MUTEX     pthread_mutex_t
#define COND      pthread_cond_t
#define THREAD    pthread_t
#define KEY       key_t
#endif


#define ISSP_RTN_OK     0
#define ISSP_RTN_FAIL  -1
#define ISSP_RTN_ERR    1

#ifdef bzero
#define BZERO(name, len) bzero(name, len)
#else
#define BZERO(name, len) memset_s(name, sizeof(name), 0, len)
#endif

#ifdef WIN32
#define SPRINTF(dest, destMax, length, pszformat,...) _snprintf_s(dest, destMax, length, pszformat, __VA_ARGS__)
#define SNPRINTF(dest, destMax, length, pszformat,...) _snprintf_s(dest, destMax, length, pszformat, __VA_ARGS__)
#define STRNCPY(dest, destLen, src, srcLen) strncpy_s(dest, destLen, src, srcLen)
#define ITOA(val, buf, size, radix) _itoa_s(val, buf, size, radix)
#define BITOA(val, buf, size, radix) _i64toa_s(val, buf, size, radix)
#else
#define SPRINTF(dest, destMax, length, pszformat,...) sprintf_s(dest, destMax, pszformat, __VA_ARGS__)
#define SNPRINTF(dest, destMax, length, pszformat,...) snprintf_s(dest, destMax, length, pszformat, __VA_ARGS__)
#define STRNCPY(dest, destLen, src, srcLen) strncpy_s(dest, destLen, src, srcLen)
#define ITOA(val, buf, size, radix) itoa(val, buf, radix)
#endif


#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#endif

