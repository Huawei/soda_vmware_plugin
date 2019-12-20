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

#ifndef _PLUGININT_H_
#define _PLUGININT_H_

#include <assert.h>
#include <rpc/rpc.h>
#include "nfs_prot.h"
#include "mount.h"
#include "vaai_nas.h"
#include <pthread.h>
#include "vixDiskLibNasPlugin.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tagFuuid
{
    uint64 ullID;
    uint64 ullFsId;
};
#define NOFS_NAS_PLUGIN_SESS_MAGIC 0x1A5B6C7E9F193832LL
#define MS_PER_SEC 1000

#define LGPFX "VAAINasPlugin--"
#define ASSERT assert

/*
 * Convert between session pointer and session ID
 * This is platform dependent
 */

struct exNasPluginSession;

#ifdef VM_X86_64
#define MAKE_SESSION_ID(session) ((VixDiskLibNasPluginSessionID) (session))
#define GET_SESSION_FROM_ID(sessionID) ((exNasPluginSession *) (sessionID))
#else
#define MAKE_SESSION_ID(session) ((VixDiskLibNasPluginSessionID) (uint32) (session))
#define GET_SESSION_FROM_ID(sessionID) ((exNasPluginSession *) (uint32) (sessionID))
#endif

// Mount protocol success
#define MOUNT_OK 0

extern void Log(const char *fmt, ...);
extern void Warning(const char *fmt, ...);
extern void Panic(const char *fmt, ...);

extern VixDiskLibNasPlugin ExampleNasPlugin;

extern VixDiskLibPluginInit ExampleNas_PluginInit;
extern VixDiskLibPluginDone ExampleNas_PluginDone;

// global session list
extern pthread_mutex_t gSessionListLock;
extern struct exNasPluginSession *gSessionHead;

extern char gZeroBuf[NFS_MAXDATA];

typedef struct exNasPluginSession {
   uint64 magic;
   CLIENT *clnt;
   char *remoteIP;
   char *remoteMountPoint;
   char *localMountPoint;
   int  localMPLen;
   int  socket;
   struct timeval timeout;
   fhandle3 rootFH;

   // global list linkage
   struct exNasPluginSession *next;
   struct exNasPluginSession *prev;

} exNasPluginSession;

#define CHECK_INPUT_NULL_POINT(ptr, str)    \
    if (NULL == (ptr))    \
    {    \
        Log(LGPFX"%s.\n", str);   \
        return VIX_E_INVALID_ARG;    \
    }

#define CHECK_MALLOC_POINT_ERROR(ptr, str)    \
    if (NULL == (ptr))    \
    {    \
        Log(LGPFX"%s.\n", str);   \
        return VIX_E_OUT_OF_MEMORY;    \
    }

/* DTS2016071405138 wangmeiqin/90003952 2016-7-15 modify static check*/
#define START_SEESION_IF_SUPPORT_NFS(nasConfigData, str) \
    if ((0 != (strcmp((nasConfigData)->fsType, VIXDISKLIB_NASPLUGIN_FSTYPE_NFS))) \
          || (NULL == (nasConfigData)->remoteIP) \
          || (NULL == (nasConfigData)->remoteMountPoint) \
          || (NULL == (nasConfigData)->localMountPoint)) \
    { \
        Log(LGPFX"%s.\n", (str));\
        return VIX_E_NOT_SUPPORTED; \
    }

#ifdef __cplusplus
}
#endif

#endif // _PLUGININT_H_

