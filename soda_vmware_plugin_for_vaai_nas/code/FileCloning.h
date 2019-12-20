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

#ifndef _FILECLONE_H_
#define _FILECLONE_H_

#include "Common.h"

#define NAS_CLONE_FILE_SRCDST_MOUNT_MATCH(CloneParams, SrcSession, DstSession, str)  \
    if ((strncmp((CloneParams)->srcFileName, (SrcSession)->localMountPoint, (SrcSession)->localMPLen) != 0)\
        || (strncmp((CloneParams)->dstFileName, (DstSession)->localMountPoint, (DstSession)->localMPLen) != 0)) \
    { \
        Log(LGPFX"%s.\n", (str));\
        return VIX_E_INVALID_ARG; \
    }

VixError NofsNasCloneFile(exNasPluginSession*  srcSession,
                          exNasPluginSession*  dstSession,
                          const VixDiskLibNasPluginCloneFileParams* cloneParams);

#endif

