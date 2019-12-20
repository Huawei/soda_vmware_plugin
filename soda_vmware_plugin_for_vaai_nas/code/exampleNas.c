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

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <rpc/pmap_clnt.h>
#include <errno.h>

#include "rpcb_prot.h"

#include "FileCloning.h"
#include "FileSpaceReservation.h"
#include "ExtendedStats.h"

static VixDiskLibNasPluginStartSession ExampleNas_StartSession;
static VixDiskLibNasPluginEndSession ExampleNas_EndSession;
static VixDiskLibNasPluginExecutePrimitive ExampleNas_ExecPrimitive;
static VixDiskLibNasPluginSupportStatus ExampleNas_SupportStatus;

static void NofsNasFreeSession(exNasPluginSession *pstSession);
static void NofsNasInsertSession(exNasPluginSession *pstSession);
static void NofsNasRemoveSession(exNasPluginSession *pstSession);

static VixError ExecPrimitiveCloneFile(exNasPluginSession *pstSession,
                                       const VixDiskLibNasPluginCloneFileParams *pstCloneParams);

static CLIENT *ExampleNas_ClntCreate(char *remoteIP,
                                     unsigned long prog,
                                     unsigned long vers,
                                     int *piSocketClnt);
static unsigned short ExampleNas_Rpcb_GetPort(struct sockaddr_in6 *addr,
                                              unsigned long prog,
                                              unsigned long vers);




// default timeout of 10 sec for the RPC client
static struct timeval exDefaultTimeout = {10, 0};

/* Buffer with zeroes */
char gZeroBuf[NFS_MAXDATA];

// global session list
pthread_mutex_t gSessionListLock = PTHREAD_MUTEX_INITIALIZER;
exNasPluginSession *gSessionHead = NULL;

/* The interface for the ExampleNas plugin as exported by this library. */
VixDiskLibNasPlugin ExampleNasPlugin = {
   {
      VIXDISKLIB_NASPLUGIN_MAJOR_VERSION,
      VIXDISKLIB_NASPLUGIN_MINOR_VERSION,
      VIXDISKLIB_PLUGIN_TYPE_NAS,
      "HuaweiVAAINasPlugin",
      ExampleNas_PluginInit,
      ExampleNas_PluginDone
   },
   ExampleNas_StartSession,
   ExampleNas_EndSession,
   ExampleNas_ExecPrimitive,
   ExampleNas_SupportStatus
};

/*
 * Functions exported by the function table describing the NAS plugin.
 */

VixError ExampleNas_MountResProcess(mountres3 stMountRes, 
                                    exNasPluginSession *pstSession, 
                                    CLIENT    *pstMountClnt,
                                    VixDiskLibNasPluginSessionID *sessionID,
                                    int iSocketClnt)
{
    VixError iRet = VIX_OK;
    
    if (0 == stMountRes.fhs_status)
    {
        pstSession->rootFH.fhandle3_len = stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len;
        if (stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len <= 0)
        {
            Log(LGPFX"NofsNas_StartSession: pstSession->rootFH.fhandle3_len is less than zero.\n");
        
            NofsNasFreeSession(pstSession);
            clnt_destroy(pstMountClnt);
            if (iSocketClnt != -1)
            {
                close(iSocketClnt);
            }
        
            return VIX_E_OUT_OF_MEMORY;            
        }

        pstSession->rootFH.fhandle3_val = (char *)malloc(stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len);
        if (NULL == pstSession->rootFH.fhandle3_val)
        {
            Log(LGPFX"NofsNas_StartSession: Does not have enough memory.\n");
        
            NofsNasFreeSession(pstSession);
            clnt_destroy(pstMountClnt);
            if (iSocketClnt != -1)
            {
                close(iSocketClnt);
            }
        
            return VIX_E_OUT_OF_MEMORY;
        }

        Log(LGPFX"NofsNas_StartSession: mounted %s on host %s \n",
            pstSession->remoteMountPoint, pstSession->remoteIP);

        memset_s(pstSession->rootFH.fhandle3_val, stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len,
            0, stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len);
        memcpy_s(pstSession->rootFH.fhandle3_val, stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len, 
            stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_val, 
            stMountRes.mountres3_u.mountinfo.fhandle.fhandle3_len);
    }
    else
    {
        Log(LGPFX"NofsNas_StartSession: could not mount %s on host %s; status: (NFS=%d), (iRet=%d).\n",
          pstSession->remoteMountPoint, pstSession->remoteIP, stMountRes.fhs_status, iRet);

        NofsNasFreeSession(pstSession);
        pstSession = NULL;

        iRet = NofsNasMakeVixError(iRet, NFS3_OK, stMountRes.fhs_status);

        Log(LGPFX"NofsNas_StartSession iRet=%d.\n",iRet);
    }

    /* Don't need the mount client anymore */
    clnt_freeres(pstMountClnt, (xdrproc_t)xdr_mountres3, (caddr_t)&stMountRes);
    clnt_destroy(pstMountClnt);
    if (iSocketClnt != -1)
    {
        close(iSocketClnt);
    }
    
    /*Use pstSession structure pointer as our sessionID  */
    if (pstSession)
    {
        *sessionID = MAKE_SESSION_ID(pstSession);
        NofsNasInsertSession(pstSession);
    }

    return iRet;    
}

VixError
ExampleNas_StartSession(const VixDiskLibNasPluginDataStoreParams *nasConfigData,
                        const VixDiskLibNasPluginSessionParams *sessionParams,
                        VixDiskLibNasPluginSessionID *sessionID)
{
    int iRet = 0;
    exNasPluginSession *pstSession = NULL;
    CLIENT    *pstMountClnt   = NULL;
    mountres3 stMountRes;
    dirpath   remoteMP        = NULL;
    int       iSocketClnt     = -1;

    CHECK_INPUT_NULL_POINT(nasConfigData, "NofsNas_StartSession: nasConfigData is null.")

    /*only support files on NFS*/
    START_SEESION_IF_SUPPORT_NFS(nasConfigData, "NofsNas_StartSession: reject non-NFS params.")

    *sessionID = VIXDISKLIB_NASPLUGIN_INVALID_SESSION_ID;
    remoteMP = nasConfigData->remoteMountPoint;

    Log(LGPFX"NofsNas_StartSession: pstSession request for %s %s %s %s %d \n",
              nasConfigData->remoteIP, nasConfigData->remoteMountPoint,
              nasConfigData->localMountPoint, nasConfigData->fsType,
              nasConfigData->fsVersion);

    /*Create new pstSession and store pstSession parameters in it*/
    pstSession = (exNasPluginSession *)malloc(sizeof(exNasPluginSession));

    CHECK_MALLOC_POINT_ERROR(pstSession, "NofsNas_StartSession: no memory.")

    memset_s(pstSession, sizeof(exNasPluginSession), 0, sizeof(exNasPluginSession));
       
    pstSession->magic   = NOFS_NAS_PLUGIN_SESS_MAGIC;
    pstSession->timeout = exDefaultTimeout;

    if (sessionParams)
    {
      pstSession->timeout.tv_sec  = sessionParams->timeoutMS / MS_PER_SEC;
      pstSession->timeout.tv_usec = (sessionParams->timeoutMS -
                                       pstSession->timeout.tv_sec * MS_PER_SEC) * MS_PER_SEC;
    }
    pstSession->remoteIP            = strdup(nasConfigData->remoteIP);
    pstSession->remoteMountPoint    = strdup(nasConfigData->remoteMountPoint);
    pstSession->localMountPoint     = strdup(nasConfigData->localMountPoint);

    if ((NULL == pstSession->remoteIP)
        ||(NULL == pstSession->remoteMountPoint)
        ||(NULL == pstSession->localMountPoint))
    {
        Log(LGPFX"NofsNas_StartSession: pstSession->remoteMountPoint or pstSession->localMountPoint is null.\n");

        NofsNasFreeSession(pstSession);
        return VIX_E_INVALID_ARG;
    }

    pstSession->localMPLen          = strlen(pstSession->localMountPoint);
    pstSession->next                = NULL;
    pstSession->prev                = NULL;
    pstSession->rootFH.fhandle3_val = NULL;

    /*create a new RPC NFS pstSession*/
    pstSession->clnt = ExampleNas_ClntCreate(pstSession->remoteIP, NFS_PROGRAM, NFS_V3, &iSocketClnt);
    if (!pstSession->clnt)
    {
        Log(LGPFX"NofsNas_StartSession: could not create RPC client for the remote host %s %s\n",
            pstSession->remoteIP, clnt_spcreateerror("error:"));

        NofsNasFreeSession(pstSession);
        return VIX_E_FAIL;
    }
    pstSession->socket = iSocketClnt;

    /*Set authentication type to Unix*/
    auth_destroy(pstSession->clnt->cl_auth);
    pstSession->clnt->cl_auth = authunix_create_default();
    if (NULL == pstSession->clnt->cl_auth)
    {
        Log(LGPFX"NofsNas_StartSession: Could not set Auth flavor to Unix \n");

        NofsNasFreeSession(pstSession);
        return VIX_E_FAIL;
    }

    /* Ping NFS server with a NULL request just to make sure it exists*/
    iRet = clnt_call(pstSession->clnt, NFSPROC3_NULL,
                     (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL,
                     pstSession->timeout);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNas_StartSession: could not connect to the remote host %s; \n"
            "NofsNas_StartSession: prog %d version %d proc %d; status %d %s\n",
            pstSession->remoteIP, NFS3_PROGRAM, NFS_V3, NFSPROC3_NULL, iRet, clnt_sperror(pstSession->clnt, " "));

        NofsNasFreeSession(pstSession);
        return VIX_E_HOST_CONNECTION_LOST;
    }

    /*Get the file handle of the mount point via mount protocol */
    pstMountClnt = ExampleNas_ClntCreate(pstSession->remoteIP, MOUNTPROG, MOUNTVERS3, &iSocketClnt);
    if (!pstMountClnt)
    {
        Log(LGPFX"NofsNas_StartSession: could not create RPC mount client for the remote host %s %s\n",
               pstSession->remoteIP, clnt_spcreateerror("error:"));
        NofsNasFreeSession(pstSession);

        return VIX_E_FAIL;
    }

    //Set authentication type to Unix
    auth_destroy(pstMountClnt->cl_auth);
    pstMountClnt->cl_auth = authunix_create_default();
    if (NULL == pstMountClnt->cl_auth)
    {
        Log(LGPFX"NofsNas_StartSession: Could not set Auth flavor to Unix \n");

        clnt_destroy(pstMountClnt);

        if (iSocketClnt != -1)
        {
            close(iSocketClnt);
        }
        NofsNasFreeSession(pstSession);

        return VIX_E_FAIL;
    }

    memset_s(&stMountRes, sizeof(stMountRes), 0, sizeof(stMountRes));
    
    iRet = mountproc_mnt_3(&remoteMP, pstMountClnt, &stMountRes);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNas_StartSession: mount clnt_call failed.\n");

        clnt_destroy(pstMountClnt);
        if (iSocketClnt != -1)
        {
            close(iSocketClnt);
        }
        NofsNasFreeSession(pstSession);

        return VIX_E_FAIL;
    }

    iRet = ExampleNas_MountResProcess(stMountRes, pstSession, pstMountClnt, sessionID, iSocketClnt);

    return (VixError)iRet;
}

static VixError
ExampleNas_EndSession(VixDiskLibNasPluginSessionID sessionID)
{
    exNasPluginSession *pstSession = GET_SESSION_FROM_ID(sessionID);

    if (sessionID == VIXDISKLIB_NASPLUGIN_INVALID_SESSION_ID)
    {
        Log(LGPFX"NofsNas_EndSession: invalid session id.\n");
        return VIX_E_INVALID_ARG;
    }

    NofsNasFreeSession(pstSession);

    Log(LGPFX"NofsNas_EndSession OK.\n");
    return VIX_OK;
}

static void
ExampleNas_ExecPrimitive(VixDiskLibNasPluginSessionID sessionID,
                         const VixDiskLibNasPluginCommonParams *execParams)
{
    if (NULL == execParams)
    {
        Log(LGPFX"NofsNas_ExecPrimitive: execParams is null.\n");
        return;
    }

    exNasPluginSession *pstSession = GET_SESSION_FROM_ID(sessionID);
    ASSERT(NOFS_NAS_PLUGIN_SESS_MAGIC == pstSession->magic);

    switch(execParams->primitiveID)
    {
        case VIXDISKLIB_NASPLUGIN_PRIM_CLONEFILE:
        {
            Log(LGPFX"NofsNas_ExecPrimitive: case VIXDISKLIB_NASPLUGIN_PRIM_CLONEFILE.\n");

            VixDiskLibNasPluginCloneFileParams *pstCloneParams = (VixDiskLibNasPluginCloneFileParams*)execParams;
            execParams->result->status = ExecPrimitiveCloneFile(pstSession, pstCloneParams);

            break;
        }

        case VIXDISKLIB_NASPLUGIN_PRIM_STATX:
        {
            Log(LGPFX"NofsNas_ExecPrimitive: case VIXDISKLIB_NASPLUGIN_PRIM_STATX.\n");

            VixDiskLibNasPluginStatXParams *xstatsParams = (VixDiskLibNasPluginStatXParams*)execParams;
            execParams->result->status = NofsNasGetExtendStats(pstSession, xstatsParams);

            break;
        }

        case VIXDISKLIB_NASPLUGIN_PRIM_RESVSPACE:
        {
            Log(LGPFX"NofsNas_ExecPrimitive: case VIXDISKLIB_NASPLUGIN_PRIM_RESVSPACE.\n");

            VixDiskLibNasPluginResvSpaceParams *resSpaceParams = (VixDiskLibNasPluginResvSpaceParams *)execParams;
            execParams->result->status = NofsNasFileReserveSpace(pstSession, resSpaceParams);

            break;
        }

        default:
            Log(LGPFX"NofsNas_ExecPrimitive: invalid primitiveID:%d.\n", execParams->primitiveID);
            execParams->result->status =  VIX_E_NOT_SUPPORTED;

            break;
    }

    return;
}

static void
NofsNasInsertSession(exNasPluginSession *pstSession)
{
    if (NULL == pstSession)
    {
        Log(LGPFX"NofsNasInsertSession: para is null.\n");
        return;
    }

    (void)pthread_mutex_lock(&gSessionListLock);

    // Insert at the head
    pstSession->next = gSessionHead;
    pstSession->prev = NULL;

    if (gSessionHead)
    {
        gSessionHead->prev = pstSession;
    }

    gSessionHead = pstSession;

    (void)pthread_mutex_unlock(&gSessionListLock);
}

static void
NofsNasRemoveSession(exNasPluginSession *pstSession)
{
    // Check if pstSession exist
    if (NULL == pstSession)
    {
        Log(LGPFX"NofsNasRemoveSession: para is null.\n");
        return;
    }

    (void)pthread_mutex_lock(&gSessionListLock);

    // pstSession could be unlinked
    if (pstSession->prev)
    {
        pstSession->prev->next = pstSession->next;
    }

    if (pstSession->next)
    {
        pstSession->next->prev = pstSession->prev;
    }

    if (pstSession == gSessionHead)
    {
        gSessionHead = pstSession->next;
    }

    pstSession->next = NULL;
    pstSession->prev = NULL;

    (void)pthread_mutex_unlock(&gSessionListLock);
}

static void
NofsNasFreeSession(exNasPluginSession *pstSession)
{
    if (NULL == pstSession)
    {
        Log(LGPFX"NofsNasFreeSession: pstSession is null.\n");
        return;
    }

    ASSERT(pstSession->magic == NOFS_NAS_PLUGIN_SESS_MAGIC);
    NofsNasRemoveSession(pstSession);

    if (pstSession->clnt)
    {
        clnt_destroy(pstSession->clnt);

        if (pstSession->socket != -1)
        {
             close(pstSession->socket);
             pstSession->socket = -1;
        }
    }

    if (NULL != pstSession->rootFH.fhandle3_val)
    {
        free(pstSession->rootFH.fhandle3_val);
        pstSession->rootFH.fhandle3_val = NULL;
    }

    free(pstSession->remoteIP);    
    free(pstSession->remoteMountPoint);    
    free(pstSession->localMountPoint); 
    pstSession->remoteIP = NULL;
    pstSession->remoteMountPoint = NULL;
    pstSession->localMountPoint = NULL;
    free(pstSession);
    pstSession = NULL;

    return;
}

static VixError
ExampleNas_SupportStatus(VixDiskLibNasPluginSessionID sessionID,
                              VixDiskLibNasPluginPrimitiveID primitiveID)
{
    VixError status = 0;
    uint64_t uiFSVersion = 0;
    exNasPluginSession *session = GET_SESSION_FROM_ID(sessionID);

    ASSERT(NOFS_NAS_PLUGIN_SESS_MAGIC == session->magic);

    /*
    * The NofsNas plugin supports all primitives. We trivially return
    * SUPPORTED for all primitives. A real plugin should do the necessary
    * checks required to ensure that a primitive is supported on the Filer.
    */
    switch(primitiveID)
    {
        case VIXDISKLIB_NASPLUGIN_PRIM_CLONEFILE:
            Log(LGPFX"ExampleNas_SupportStatus: VIXDISKLIB_NASPLUGIN_PRIM_CLONEFILE.\n");
            status = NofsNasIsSupport(session, VAAICMD_CLN_SUPPORT, &uiFSVersion);
            break;

        case VIXDISKLIB_NASPLUGIN_PRIM_STATX:
            Log(LGPFX"ExampleNas_SupportStatus: VIXDISKLIB_NASPLUGIN_PRIM_STATX.\n");
            status = NofsNasIsSupport(session, VAAICMD_STAT_SUPPORT, &uiFSVersion);
            break;

        case VIXDISKLIB_NASPLUGIN_PRIM_RESVSPACE:
            Log(LGPFX"ExampleNas_SupportStatus: VIXDISKLIB_NASPLUGIN_PRIM_RESVSPACE.\n");
            status = NofsNasIsSupport(session, VAAICMD_RSV_SUPPORT, &uiFSVersion);
            break;

        default:
            status = VIX_E_NOT_SUPPORTED;
            break;
    }

    if (VIX_OK != status)
    {
        status = VIX_E_NOT_SUPPORTED;
        Log(LGPFX"ExampleNas_SupportStatus: status is not support.\n");
    }

    return status;
}

static VixError ExecPrimitiveCloneFile(exNasPluginSession *pstSession,
                                       const VixDiskLibNasPluginCloneFileParams *pstCloneParams)
{
    VixError status = 0;
    exNasPluginSession *pstSrcSession = NULL;

    VixDiskLibNasPluginSessionID srcSessionID = VIXDISKLIB_NASPLUGIN_INVALID_SESSION_ID;
    VixDiskLibNasPluginSessionParams stSrcSessionParams;

    CHECK_INPUT_NULL_POINT(pstSession,     "ExecPrimitiveCloneFile: pstSession is null.")
    CHECK_INPUT_NULL_POINT(pstCloneParams, "ExecPrimitiveCloneFile: pstCloneParams is null.")

    memset_s(&stSrcSessionParams, sizeof(stSrcSessionParams), 0, sizeof(stSrcSessionParams));      

    //src and dst in diff datastore
    if (pstCloneParams->cloneFlags & VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_SRCDATASTORE_VALID)
    {
        Log(LGPFX"ExecPrimitiveCloneFile: VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_SRCDATASTORE_VALID.\n");

        stSrcSessionParams.timeoutMS = pstSession->timeout.tv_usec / MS_PER_SEC +
                                       pstSession->timeout.tv_sec * MS_PER_SEC;

        status = ExampleNas_StartSession(pstCloneParams->srcDataStoreInfo,
                                          &stSrcSessionParams,
                                          &srcSessionID);
        if (VIX_OK != status)
        {
            Log(LGPFX"ExecPrimitiveCloneFile: start pstSrcSession failed.\n");

            return status;
        }

        pstSrcSession = GET_SESSION_FROM_ID(srcSessionID);
    }
    else
    {
        pstSrcSession = pstSession;
    }

    status = NofsNasCloneFile(pstSrcSession, pstSession, pstCloneParams);

    if (VIXDISKLIB_NASPLUGIN_INVALID_SESSION_ID != srcSessionID)
    {
        ExampleNas_EndSession(srcSessionID);
    }

    return status;
}

static CLIENT *
ExampleNas_ClntCreate(char *remoteIP,
                      unsigned long prog,
                      unsigned long vers,
                      int *piSocketClnt)
{
    int sock = -1;
    char ipv6Addr[INET6_ADDRSTRLEN];
    char *pIF = NULL;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    char *hostIP = NULL;
    CLIENT *clnt = NULL;
    unsigned int ifIndex = 0;
    struct in6_pktinfo pktInfo;
    struct sockaddr_in6 *saddr6 = NULL;
    struct sockaddr_in *saddr = NULL;
    int len;
    unsigned short port;

    *piSocketClnt = -1;

    pIF = strchr(remoteIP, '%');
    if (pIF != NULL)
    {
        /* link local address format "<ipv6 addr>%vmk<interface number>" */
        if (sizeof(ipv6Addr) < pIF - remoteIP + 1)
        {
            Log(LGPFX" invalid IPv6 address %s.\n", remoteIP);
            return NULL;
        }

        strncpy_s(ipv6Addr, INET6_ADDRSTRLEN, remoteIP, pIF - remoteIP);
        ipv6Addr[pIF - remoteIP] = '\0';

        pIF++;
    }

    memset_s(&hints, sizeof(hints), 0, sizeof(hints));
   
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICHOST;

    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    if (pIF != NULL)
    {
        hostIP = ipv6Addr;
    }
    else
    {
        hostIP = remoteIP;
    }

    if (getaddrinfo(hostIP, NULL, &hints, &result) != 0)
    {
        Log(LGPFX" failed to get address information %s.\n", hostIP);
        return NULL;
    }

    sock = socket(result->ai_family, result->ai_socktype, 0);
    if (sock == -1)
    {
        Log(LGPFX" failed to create socket.\n");
        goto free;
    }

    if (pIF != NULL)
    {
        ifIndex = if_nametoindex(pIF);
        if (ifIndex == 0)
        {
            Log(LGPFX" failed to get index for interface %s.\n", pIF);
            close(sock);
            sock = -1;
            goto free;
        }

        memset_s(&pktInfo, sizeof(pktInfo), 0, sizeof(pktInfo));
                
        pktInfo.ipi6_ifindex = ifIndex;
        if (setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, &pktInfo, sizeof(pktInfo)) != 0)
        {
            Log(LGPFX" failed to set sockopt for interface %s, index %d.\n", pIF, pktInfo.ipi6_ifindex);
            close(sock);
            sock = -1;
            goto free;
        }
    }

    switch (result->ai_family)
    {
        case AF_INET6:
            saddr6 = (struct sockaddr_in6 *)result->ai_addr;
            saddr6->sin6_scope_id = ifIndex;
            port = ExampleNas_Rpcb_GetPort(saddr6, prog, vers);
            saddr6->sin6_port = htons(port);
            len = sizeof(struct sockaddr_in6);
            break;
        case AF_INET:
            saddr = (struct sockaddr_in *)result->ai_addr;
            port = pmap_getport(saddr, prog, vers, result->ai_protocol);
            saddr->sin_port = htons(port);
            len = sizeof(struct sockaddr_in);
            break;
        default:
            Log(LGPFX" invalid family %d.\n", result->ai_family);
            goto free;
    }

    Log(LGPFX" %s: socket info IP %s, family %d, socktype %d, "
            "protocol %d, port %hu, ifIndex %d.\n",
            __FUNCTION__, hostIP, result->ai_family, result->ai_socktype,
            result->ai_protocol, port, ifIndex);

    ASSERT(result->ai_protocol == IPPROTO_TCP);

   /*
    * IPv4 and IPv6 address structures are compatible up to the port
    * field. All clnttcp_create use is sin_port field if socket passed
    * in is not NULL.
    */
    if (connect(sock, result->ai_addr, len) == -1)
    {
        Log(LGPFX" failed to connect to socket.\n");
        close(sock);
        sock = -1;
        goto free;
    }

    clnt = clnttcp_create((struct sockaddr_in *)result->ai_addr,
                         prog, vers, &sock, 0,0);
    if (clnt == NULL)
    {
        Log(LGPFX" failed to create RPC client.\n");
        close(sock);        
        sock = -1;
        goto free;
    }

free:
    freeaddrinfo(result);

    *piSocketClnt = sock;
    
    return clnt;
}

static unsigned short
ExampleNas_Rpcb_GetPort(struct sockaddr_in6 *addr,
                        unsigned long prog,
                        unsigned long vers)
{
    int sock = -1;
    CLIENT *clnt = NULL;
    struct rpcb rpcbArg;
    char ipAddr[INET6_ADDRSTRLEN];
    char univAddr[INET6_ADDRSTRLEN];
    char *retAddr = NULL;
    char *portStr = NULL;
    unsigned long int port = 0;
    struct in6_pktinfo pktInfo;
    int iRet = 0;

    addr->sin6_port = htons(RPCB_PORT);

    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock == -1)
    {
        Log(LGPFX" failed to create socket.\n");
        goto exit;
    }

    if (addr->sin6_scope_id != 0)
    {
        memset_s(&pktInfo, sizeof(pktInfo), 0, sizeof(pktInfo));
                
        pktInfo.ipi6_ifindex = addr->sin6_scope_id;;
        if (setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, &pktInfo, sizeof(pktInfo)) != 0)
        {
            Log(LGPFX" failed to set sockopt for interface index %d.\n", pktInfo.ipi6_ifindex);
            goto close_sock;
        }
    }

    if (connect(sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        Log(LGPFX" failed to connect to socket.\n");
        goto close_sock;
    }


    clnt = clnttcp_create((struct sockaddr_in *)addr,
                         RPCBPROG, RPCBVERS, &sock, 0,0);

    if (clnt == NULL)
    {
        Log(LGPFX" failed to create RPC client.\n");
        goto close_sock;
    }

    if (inet_ntop(AF_INET6, &addr->sin6_addr, ipAddr, sizeof(ipAddr)) == NULL)
    {
        Log(LGPFX" failed in inet_ntop.\n");
        goto clnt;
    }

    port = ntohs(addr->sin6_port);

    snprintf_s(univAddr, sizeof(univAddr), sizeof(univAddr) - 1, "%s.%u.%u",
            ipAddr, (unsigned int)(port >> 8) & 0xFF,
            (unsigned int)port & 0xFF);

    port = 0;

    memset_s(&rpcbArg, sizeof(rpcbArg), 0, sizeof(rpcbArg));
   
    rpcbArg.r_prog = prog;
    rpcbArg.r_vers = vers;
    rpcbArg.r_netid = "tcp6";
    rpcbArg.r_addr = univAddr;
    rpcbArg.r_owner = "exampleNasPlugin";

    iRet = rpcbproc_getaddr_3(&rpcbArg, clnt, &retAddr);
    if ((iRet != RPC_SUCCESS) || (retAddr == NULL))
    {
        Log(LGPFX" RPCBIND getaddr failed,  prog %d, vers %d, netid %s, "
                " addr %s, owner %s, status %s.\n",
                rpcbArg.r_prog, rpcbArg.r_vers, rpcbArg.r_netid,
                rpcbArg.r_addr, rpcbArg.r_owner, clnt_sperror(clnt, ""));
        goto clnt;
    }


    Log(LGPFX" %s: prog %d, vers %d, univAddr %s, retAddr %s.\n ",
        __FUNCTION__, rpcbArg.r_prog, rpcbArg.r_vers, rpcbArg.r_addr, retAddr);

    if ((portStr = strrchr(retAddr, (int)'.')) == NULL)
    {
        goto getaddr;
    }

    port = strtoul(portStr + 1, NULL, 10) & 0xFF;
    
    *portStr = '\0';
    if ((portStr = strrchr(retAddr, (int)'.')) == NULL)
    {        
        goto getaddr;
    }

    port |= strtoul(portStr + 1, NULL, 10) << 8;    

getaddr:
        
    free(retAddr);
    retAddr = NULL;    

clnt:
    clnt_destroy(clnt);
    
close_sock:
   /* clnt_destroy closes socket */
    if (sock != -1)
    {
        close(sock);
    }

exit:    
    return (unsigned short) port;
}


