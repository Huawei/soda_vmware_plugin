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

#include "Common.h"

VixError NofsNasMakeVixErrorFromFileSys(int32_t iresult)
{
    switch (iresult)
    {
        case SNAPSHOT_HIERARCHY_TOODEEP:
            return VIX_E_SNAPSHOT_HIERARCHY_TOODEEP;

        case SNAPSHOT_MAXSNAPSHOTS:
            return VIX_E_SNAPSHOT_MAXSNAPSHOTS;

        case OBJECT_IS_BUSY:
            return VIX_E_OBJECT_IS_BUSY;

        case DISK_FULL:
            return VIX_E_DISK_FULL;

        default:
            return VIX_E_FAIL;
    }
}

VixError NofsNasMakeVixErrorFromNfs(nfsstat3 enNfsStatus)
{
    switch (enNfsStatus)
    {
        case NFS3_OK:
            return VIX_OK;

        case NFS3ERR_PERM :
        case NFS3ERR_ACCES:
            return VIX_E_FILE_ACCESS_ERROR;

        case NFS3ERR_NOENT:
        case NFS3ERR_NODEV:
            return VIX_E_FILE_NOT_FOUND;

        case NFS3ERR_EXIST:
            return VIX_E_FILE_ALREADY_EXISTS;

        case NFS3ERR_ISDIR:
            return VIX_E_INCORRECT_FILE_TYPE;

        case NFS3ERR_FBIG:
            return VIX_E_FILE_TOO_BIG;

        case NFS3ERR_NOSPC:
            return VIX_E_DISK_FULL;

        case NFS3ERR_ROFS:
            return VIX_E_FILE_READ_ONLY;

        case NFS3ERR_NAMETOOLONG:
            return VIX_E_FILE_NAME_INVALID;

        default:
            return VIX_E_FILE_ERROR;
    }
}

VixError NofsNasMakeVixError(int iRpcError, nfsstat3 enNfsStatus, int iMountError)
{
    switch (iRpcError)
    {
        case RPC_SUCCESS:
            if (iMountError != MOUNT_OK)
            {
                return VIX_E_OBJECT_NOT_FOUND;
            }

            return NofsNasMakeVixErrorFromNfs(enNfsStatus);

        case RPC_AUTHERROR:/* authentication error */
            return VIX_E_AUTHENTICATION_FAIL;

        case RPC_VERSMISMATCH:/* rpc versions not compatible */
        case RPC_PROGUNAVAIL:/* program not available */
        case RPC_PROGVERSMISMATCH:/* program version mismatched */
        case RPC_PROCUNAVAIL:/* procedure unavailable */
        case RPC_CANTDECODEARGS:/* decode arguments error */
        case RPC_SYSTEMERROR:/* generic "other problem" */
        case RPC_NOBROADCAST:/* Broadcasting not supported */

            return VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT;

        case RPC_UNKNOWNHOST:/* unknown host name */
        case RPC_UNKNOWNADDR:/* Remote address unknown */

            return VIX_E_HOST_NOT_CONNECTED;

        case RPC_UNKNOWNPROTO:/* unknown protocol */
            return VIX_E_NOT_SUPPORTED;

        default:
            return VIX_E_FAIL;
    }
}

VixError NofsNasLookupFH(exNasPluginSession *pstSession,
                         nfs_fh3 *pstNfsFH,
                         char *pszPathName,
                         fattr3 *pstAttributes)
{
    VixError status = 0;

    char *pszTmpPathName = NULL;
    char *pszItr         = NULL;

    CHECK_INPUT_NULL_POINT(pstSession,    "NofsNasLookupFH: pstSession is null.")
    CHECK_INPUT_NULL_POINT(pstNfsFH,      "NofsNasLookupFH: pstNfsFH is null.")
    CHECK_INPUT_NULL_POINT(pszPathName,   "NofsNasLookupFH: pszPathName is null.")
    CHECK_INPUT_NULL_POINT(pstAttributes, "NofsNasLookupFH: pstAttributes is null.")
    CHECK_INPUT_NULL_POINT(pstNfsFH->data.data_val, "NofsNasLookupFH: pstNfsFH->data.data_val is null.")

    pstNfsFH->data.data_len = pstSession->rootFH.fhandle3_len;

    memset_s(pstNfsFH->data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(pstNfsFH->data.data_val, NFS3_FHSIZE, pstSession->rootFH.fhandle3_val, pstSession->rootFH.fhandle3_len);

    pszTmpPathName = strdup(pszPathName);
    if (NULL == pszTmpPathName)
    {
        Log(LGPFX"NofsNasLookupFH: pszTmpPathName is NULL.\n");
        return VIX_E_FAIL;
    }

    status = NofsNasLookupFHExistChk(pstSession, pszTmpPathName, &pszItr, pstNfsFH, pstAttributes);
    free(pszTmpPathName);
    pszTmpPathName = NULL;

    if (VIX_E_FAIL == status)
    {
        Log(LGPFX"NofsNasLookupFH: Lookup file failed.\n");
        return VIX_E_FAIL;
    }

    if (VIX_OK == status)
    {
        Log(LGPFX"NofsNasLookupFH: NofsNasLookupFHExistChk path (%s) successfully.\n", pszPathName);
        return VIX_OK;
    }
    else if (VIX_E_FILE_NOT_FOUND == status)
    {
        Log(LGPFX"NofsNasLookupFH: NofsNasLookupFHExistChk path (%s) not found.\n", pszPathName);
        return VIX_E_FILE_NOT_FOUND;
    }

    Log(LGPFX"NofsNasLookupFH: NofsNasLookupFHExistChk error, return code:%d.\n", status);
    return VIX_E_FILE_ERROR;
 }

VixError  NofsNasLookupFHExistChk(exNasPluginSession *pstSession,
                                  char *pszPathName,
                                  char **ppszItr,
                                  nfs_fh3 *pstParentFH,
                                  fattr3 *pstAttr)
{
    VixError status      = 0;
    nfsstat3 enNfsStatus = 0;
    Bool     bLookupOK   = FALSE;

    char        *pszFileName   = NULL;
    LOOKUP3res  stLookupRes;
    LOOKUP3args stLookupArgs;
    int         iRet = 0;

    memset_s(&stLookupArgs, sizeof(stLookupArgs), 0, sizeof(stLookupArgs));

    CHECK_INPUT_NULL_POINT(pstSession,  "NofsNasLookupFHExistChk: pstSession is null.")
    CHECK_INPUT_NULL_POINT(pszPathName, "NofsNasLookupFHExistChk: pszPathName is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH, "NofsNasLookupFHExistChk: pstParentFH is null.")
    CHECK_INPUT_NULL_POINT(ppszItr,     "NofsNasLookupFHExistChk: ppszItr is null.")
    CHECK_INPUT_NULL_POINT(pstAttr,     "NofsNasLookupFHExistChk: pstAttr is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH->data.data_val, "NofsNasLookupFHExistChk: pstParentFH->data.data_val is null.")

    Log(LGPFX"NofsNasLookupFHExistChk: path=%s.\n", pszPathName);

    for (pszFileName = strtok_r(pszPathName, "/", ppszItr);
         pszFileName;
         pszFileName = strtok_r(NULL, "/", ppszItr))
    {
        stLookupArgs.what.dir  = *pstParentFH;
        stLookupArgs.what.name = pszFileName;

        memset_s(&stLookupRes, sizeof(LOOKUP3res), 0, sizeof(LOOKUP3res));

        iRet = nfsproc3_lookup_3(&stLookupArgs, pstSession->clnt, &stLookupRes);
        if (RPC_SUCCESS != iRet)
        {
            Log(LGPFX"NofsNasLookupFHExistChk: call nfsproc3_lookup_3 failed.\n");
            return VIX_E_FAIL;
        }

        enNfsStatus = stLookupRes.status;
        status = NofsNasMakeVixError(0, enNfsStatus, MOUNT_OK);
        if (VIX_OK != status)
        {
            Log(LGPFX"NofsNasLookupFHExistChk: look up file=%s failed.\n", pszFileName);
            clnt_freeres(pstSession->clnt, (xdrproc_t)xdr_LOOKUP3res, (caddr_t)&stLookupRes);

            break;
        }

        bLookupOK = TRUE;
        pstParentFH->data.data_len = stLookupRes.LOOKUP3res_u.resok.object.data.data_len;
        *pstAttr = stLookupRes.LOOKUP3res_u.resok.obj_attributes.post_op_attr_u.attributes;

        memset_s(pstParentFH->data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
        memcpy_s(pstParentFH->data.data_val, NFS3_FHSIZE,
            stLookupRes.LOOKUP3res_u.resok.object.data.data_val, stLookupRes.LOOKUP3res_u.resok.object.data.data_len);

        clnt_freeres(pstSession->clnt, (xdrproc_t)xdr_LOOKUP3res, (caddr_t)&stLookupRes);
    }

    if (FALSE == bLookupOK)
    {
        Log(LGPFX"NofsNasLookupFHExistChk: Lookup file failed.\n");
        return VIX_E_FAIL;
    }

    return status;
}

VixError NofsNasCreateAndLookupFile(exNasPluginSession *pstSession,
                                    nfs_fh3 *pstParentFH,
                                    char *pszPathName,
                                    fattr3 *pstAttributes)
{
    VixError status      = 0;
    nfsstat3 enNfsStatus = 0;
    int      iRet        = 0;
    int      iLookupRet  = 0;
    CREATE3args stCreateArgs;
    LOOKUP3args stLookupArgs;
    CREATE3res  stCreateRes;
    LOOKUP3res  stLookupRes;
    char       *pszFileName    = NULL;
    char       *pszItr         = NULL;
    char       *pszTmpPathName = NULL;
    char       *pszTmpName     = NULL;

    CHECK_INPUT_NULL_POINT(pstSession,    "NofsNasCreateAndLookupFile: pstSession is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH,   "NofsNasCreateAndLookupFile: pstParentFH is null.")
    CHECK_INPUT_NULL_POINT(pszPathName,   "NofsNasCreateAndLookupFile: pszPathName is null.")
    CHECK_INPUT_NULL_POINT(pstAttributes, "NofsNasCreateAndLookupFile: pstAttributes is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH->data.data_val, "NofsNasCreateAndLookupFile: pstParentFH->data.data_val is null.")

    memset_s(&stCreateRes, sizeof(stCreateRes), 0, sizeof(stCreateRes));
    memset_s(&stCreateArgs, sizeof(stCreateArgs), 0, sizeof(stCreateArgs));
    memset_s(&stLookupArgs, sizeof(stLookupArgs), 0, sizeof(stLookupArgs));
    memset_s(&stLookupRes, sizeof(stLookupRes), 0, sizeof(stLookupRes));

    pszTmpPathName = strdup(pszPathName);
    if (NULL == pszTmpPathName)
    {
        Log(LGPFX"NofsNasCreateAndLookupFile: pszTmpPathName is NULL.\n");
        return VIX_E_FAIL;
    }

    for (pszTmpName = strtok_r(pszTmpPathName, "/", &pszItr);
         pszTmpName;
         pszTmpName = strtok_r(NULL, "/", &pszItr))
    {
        pszFileName = pszTmpName;
    }

    stCreateArgs.where.dir  = *pstParentFH;
    stCreateArgs.where.name = pszFileName;

    stCreateArgs.how.createhow3_u.obj_attributes.mode.set_mode3_u.mode = pstAttributes->mode;
    stCreateArgs.how.createhow3_u.obj_attributes.gid.set_gid3_u.gid    = pstAttributes->gid;
    stCreateArgs.how.createhow3_u.obj_attributes.uid.set_uid3_u.uid    = pstAttributes->uid;
    stCreateArgs.how.createhow3_u.obj_attributes.size.set_size3_u.size = 0;

    stCreateArgs.how.createhow3_u.obj_attributes.atime.set_atime_u.atime.seconds  = -1;
    stCreateArgs.how.createhow3_u.obj_attributes.atime.set_atime_u.atime.nseconds = -1;
    stCreateArgs.how.createhow3_u.obj_attributes.mtime.set_mtime_u.mtime.seconds  = -1;
    stCreateArgs.how.createhow3_u.obj_attributes.mtime.set_mtime_u.mtime.nseconds = -1;

    iRet = nfsproc3_create_3(&stCreateArgs, pstSession->clnt, &stCreateRes);
    if (RPC_SUCCESS != iRet)
    {
        free(pszTmpPathName);

        pszTmpPathName = NULL;
        Log(LGPFX"NofsNasCreateAndLookupFile: call nfsproc3_create_3 failed.\n");

        return VIX_E_FAIL;
    }

    enNfsStatus = stCreateRes.status;
    status = NofsNasMakeVixError(0, enNfsStatus, MOUNT_OK);

    clnt_freeres(pstSession->clnt, (xdrproc_t)xdr_CREATE3res, (caddr_t)&stCreateRes);

    if (VIX_OK != status)
    {
        free(pszTmpPathName);
        Log(LGPFX"NofsNasCreateAndLookupFile: Create file (%s) failed. Error (NFS=%d). \n",
            pszFileName, enNfsStatus);

        return status;
    }

    Log(LGPFX"NofsNasCreateAndLookupFile: Create file (%s) successfully.\n", pszFileName);

    stLookupArgs.what.dir  = *pstParentFH;
    stLookupArgs.what.name = pszFileName;

    iLookupRet = nfsproc3_lookup_3(&stLookupArgs, pstSession->clnt, &stLookupRes);
    if (RPC_SUCCESS != iLookupRet)
    {
        free(pszTmpPathName);

        pszTmpPathName = NULL;
        Log(LGPFX"NofsNasCreateAndLookupFile: lookup call failed.\n");

        return VIX_E_FAIL;
    }

    enNfsStatus = stLookupRes.status;
    status = NofsNasMakeVixError(0, enNfsStatus, MOUNT_OK);

    if (VIX_OK == status)
    {
        pstParentFH->data.data_len = stLookupRes.LOOKUP3res_u.resok.object.data.data_len;
        *pstAttributes = stLookupRes.LOOKUP3res_u.resok.obj_attributes.post_op_attr_u.attributes;

        memset_s(pstParentFH->data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
        memcpy_s(pstParentFH->data.data_val, NFS3_FHSIZE,
            stLookupRes.LOOKUP3res_u.resok.object.data.data_val, stLookupRes.LOOKUP3res_u.resok.object.data.data_len);

        Log(LGPFX"NofsNasCreateAndLookupFile: Lookup file (%s) successfully.\n", pszFileName);
    }
    else
    {
        Log(LGPFX"NofsNasCreateAndLookupFile: Lookup file (%s) failed, error code (%d).\n", pszFileName, status);
    }

    free(pszTmpPathName);

    pszTmpPathName = NULL;
    clnt_freeres(pstSession->clnt, (xdrproc_t)xdr_LOOKUP3res, (caddr_t)&stLookupRes);

    return status;
}

VixError NofsNasRemoveFile(exNasPluginSession *pstSession,
                           nfs_fh3 *pstParentFH,
                           char *pszPathName)
{
    VixError status      = 0;
    nfsstat3 enNfsStatus = 0;
    int      iRet        = 0;

    REMOVE3args stRemoveArgs;
    REMOVE3res  stRemoveRes;
    fattr3      stAttributes;

    char        *pszFileName  = NULL;
    char        *pszRemoveItr = NULL;
    char        *pszTmpPath   = NULL;
    char        *pszTmpName   = NULL;

    CHECK_INPUT_NULL_POINT(pstSession,  "NofsNasRemoveFile: pstSession is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH, "NofsNasRemoveFile: pstParentFH is null.")
    CHECK_INPUT_NULL_POINT(pszPathName, "NofsNasRemoveFile: pszPathName is null.")
    CHECK_INPUT_NULL_POINT(pstParentFH->data.data_val, "NofsNasRemoveFile: pstParentFH->data.data_val is null.")

    memset_s(&stAttributes, sizeof(stAttributes), 0, sizeof(stAttributes));
    memset_s(&stRemoveArgs, sizeof(stRemoveArgs), 0, sizeof(stRemoveArgs));
    memset_s(&stRemoveRes, sizeof(stRemoveRes), 0, sizeof(stRemoveRes));

    pszTmpPath = strdup(pszPathName);
    if (NULL == pszTmpPath)
    {
        Log(LGPFX"NofsNasRemoveFile: pszTmpPath is NULL.\n");
        return VIX_E_FAIL;
    }

    for (pszTmpName = strtok_r(pszTmpPath, "/", &pszRemoveItr);
         pszTmpName;
         pszTmpName = strtok_r(NULL, "/", &pszRemoveItr))
    {
        pszFileName = pszTmpName;
    }

    free(pszTmpPath);
    pszTmpPath = NULL;

    stRemoveArgs.object.dir  = *pstParentFH;
    stRemoveArgs.object.name = pszFileName;

    iRet = nfsproc3_remove_3(&stRemoveArgs, pstSession->clnt, &stRemoveRes);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasRemoveFile: call nfsproc3_remove_3 failed.\n");
        return VIX_E_FAIL;
    }

    enNfsStatus = stRemoveRes.status;
    status = NofsNasMakeVixError(0, enNfsStatus, MOUNT_OK);

    clnt_freeres(pstSession->clnt, (xdrproc_t)xdr_REMOVE3res, (caddr_t)&stRemoveRes);

    if (VIX_OK != status)
    {
        Log(LGPFX"NofsNasRemoveFile: Remove file (%s) failed. Error (NFS=%d).\n",
            pszFileName, enNfsStatus);

        return status;
    }

    Log(LGPFX"NofsNasRemoveFile: Remove file (%s) successfully.\n", pszFileName);

    return VIX_OK;
}

VixError
NofsNasIsSupport(exNasPluginSession *session, uint32_t uiVaaiProcType, uint64_t *uiFSVersion)
{
    VAAI_PRIVATEDATA_S      argPrivateData;
    EXTENDCMDargs       extendCMDArgs;
    EXTENDCMDres        extendCMDres;
    VAAI_OUTPUT_DATA_S  *outputdata = NULL;
    fattr3              attributes;
    int                 iRet        = 0;

    CHECK_INPUT_NULL_POINT(session->localMountPoint, "NofsNasIsSupport: localMountPoint is null.")
    CHECK_INPUT_NULL_POINT(session->remoteMountPoint,"NofsNasIsSupport: remoteMountPoint is null.")
    CHECK_INPUT_NULL_POINT(session->remoteIP, "NofsNasIsSupport: remoteIP is null.")

    memset_s(&extendCMDArgs, sizeof(extendCMDArgs), 0, sizeof(extendCMDArgs));
    memset_s(&extendCMDres, sizeof(extendCMDres), 0, sizeof(extendCMDres));
    memset_s(&argPrivateData, sizeof(argPrivateData), 0, sizeof(argPrivateData));
    memset_s(&attributes, sizeof(attributes), 0, sizeof(attributes));

    //cmdtype
    extendCMDArgs.src_fh.cmdtype = EXTENDVAAI_OTHER;

    //dis_fh
    extendCMDArgs.dis_fh.data.data_len = session->rootFH.fhandle3_len;
    extendCMDArgs.dis_fh.data.data_val = (char *)malloc(NFS3_FHSIZE);

    CHECK_MALLOC_POINT_ERROR(extendCMDArgs.dis_fh.data.data_val, "NofsNasIsSupport: dis_fh not have enough memory.")

    memset_s(extendCMDArgs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(extendCMDArgs.dis_fh.data.data_val, NFS3_FHSIZE,
        session->rootFH.fhandle3_val, session->rootFH.fhandle3_len);

    /* is support */
    argPrivateData.version     = VAAI_NAS_MSG_VERSION;
    argPrivateData.vaaiCmdType = uiVaaiProcType;

    extendCMDArgs.cmdarg.data.data_len = sizeof(argPrivateData);
    extendCMDArgs.cmdarg.data.data_val = (char *)malloc(sizeof(argPrivateData));
    if (NULL == extendCMDArgs.cmdarg.data.data_val)
    {
        Log(LGPFX"NofsNasIsSupport: extendCMDArgs does not have enough memory.\n");
        free(extendCMDArgs.dis_fh.data.data_val);

        extendCMDArgs.dis_fh.data.data_val = NULL;
        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(extendCMDArgs.cmdarg.data.data_val, sizeof(argPrivateData), 0, sizeof(argPrivateData));
    memcpy_s(extendCMDArgs.cmdarg.data.data_val, sizeof(argPrivateData),
        &argPrivateData, sizeof(argPrivateData));

    iRet = nfsproc3_extendvaai_3(&extendCMDArgs, session->clnt, &extendCMDres);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasIsSupport: call function(nfsproc3_extendvaai_3) failed.\n");
        free(extendCMDArgs.cmdarg.data.data_val);
        free(extendCMDArgs.dis_fh.data.data_val);

        extendCMDArgs.cmdarg.data.data_val = NULL;
        extendCMDArgs.dis_fh.data.data_val = NULL;

        return VIX_E_FAIL;
    }

    free(extendCMDArgs.dis_fh.data.data_val);
    free(extendCMDArgs.cmdarg.data.data_val);

    extendCMDArgs.dis_fh.data.data_val = NULL;
    extendCMDArgs.cmdarg.data.data_val = NULL;

    //only NFS OK and output OK show support
    if (VIX_OK == extendCMDres.status)
    {
        if (extendCMDres.EXTENDCMDres_u.resok.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasIsSupport: resok Output data len error.\n");
            clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);
            return VIX_E_FAIL;
        }

        outputdata = (VAAI_OUTPUT_DATA_S *)extendCMDres.EXTENDCMDres_u.resok.cmdres.data.data_val;
        *uiFSVersion = outputdata->version;

        Log(LGPFX"NofsNasIsSupport: extendCMDres.status=(%d), outputdate->result=(%d), outputdate->version=(%llu)\n",
        extendCMDres.status, outputdata->result, outputdata->version);

        if (VAAI_NAS_SUPPORT == outputdata->result)
        {
            Log(LGPFX"NofsNasIsSupport: Support CMDType %d.\n", uiVaaiProcType);
            clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);

            return VIX_OK;
        }
    }

    clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);

    return VIX_E_FAIL;
}


