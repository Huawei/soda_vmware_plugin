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

#ifndef _COMMON_H_
#define _COMMON_H_

#include "pluginInt.h"
#include "vaai_nas_intf.h"
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "securec.h"

#define RESERVETIMEOUT 300
#define CLONETIMEOUT   300
#define RETRYTIME      60
#define RESERVERETRYTIME 2
#define SLEEPTIME      1
#define REPORTCOUNT    30
#define FILEMINSIZE    512
#define CHECKPROGRESSCOUNT 15

VixError NofsNasLookupFHExistChk(exNasPluginSession *pstSession,
                                  char *pszPathName,
                                  char **ppszItr,
                                  nfs_fh3 *pstParentFH,
                                  fattr3 *pstAttr);

VixError NofsNasCreateAndLookupFile(exNasPluginSession *pstSession,
                                    nfs_fh3 *pstParentFH,
                                    char *pszPathName,
                                    fattr3 *pstAttributes);

VixError NofsNasLookupFH(exNasPluginSession *pstSession,
                         nfs_fh3 *pstNfsFH,
                         char *pszPathName,
                         fattr3 *pstAttributes);

VixError NofsNasRemoveFile(exNasPluginSession *pstSession,
                           nfs_fh3 *pstParentFH,
                           char *pszPathName);

VixError NofsNasMakeVixError(int iRpcError,
                             nfsstat3 enNfsStatus,
                             int iMountError);

VixError NofsNasMakeVixErrorFromNfs(nfsstat3 enNfsStatus);

VixError NofsNasMakeVixErrorFromFileSys(int32_t iresult);

VixError NofsNasIsSupport(exNasPluginSession *session, uint32_t uiVaaiProcType, uint64_t *uiFSVersion);

#endif

