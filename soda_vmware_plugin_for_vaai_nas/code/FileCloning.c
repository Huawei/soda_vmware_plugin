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

#include "FileCloning.h"

static VixError NofsNasCloneAbort(exNasPluginSession* pstDstSession, nfs_fh3* pstFH);

static VixError NofsNasGetCloneProc(exNasPluginSession* pstDstSession,
                                    nfs_fh3* pstFH,
                                    FILECLONE_PROCINFO_S* pstCloneInfo);

static VixError NofsNasFileReport(exNasPluginSession* pstDstSession,
                                  const VixDiskLibNasPluginCloneFileParams* pstCloneParams,
                                  char* pszDstfilePath);

static VixError NofsNasHandingExcption(const VixDiskLibNasPluginCloneFileParams* pstCloneParams,
                                       VixError             status,
                                       exNasPluginSession*  pstDstSession,
                                       nfs_fh3*             pstdstFH,
                                       char*                pszDstPath,
                                       fattr3*              pstAttributes);


static VixError NofsNasCloneAbort(exNasPluginSession* pstDstSession, nfs_fh3* pstFH)
{

    EXTENDCMDargs         stExtCmdArgs;
    VAAI_PRIVATEDATA_S    stPrivateData;
    EXTENDCMDres          stExtres;
    VAAI_OUTPUT_DATA_S    *outputdata = NULL;
    VixError              status = 0;
    int                   iRet   = 0;

    CHECK_INPUT_NULL_POINT(pstDstSession, "NofsNasCloneAbort: pstDstSession is null.")
    CHECK_INPUT_NULL_POINT(pstFH,         "NofsNasCloneAbort: pstFH is null.")

    memset_s(&stExtCmdArgs, sizeof(stExtCmdArgs), 0, sizeof(stExtCmdArgs));
    memset_s(&stExtres, sizeof(stExtres), 0, sizeof(stExtres));
    memset_s(&stPrivateData, sizeof(stPrivateData), 0, sizeof(stPrivateData));

    stPrivateData.version              = VAAI_NAS_MSG_VERSION;
    stPrivateData.vaaiCmdType          = VAAICMD_FCLNABORT;
    stExtCmdArgs.src_fh.cmdtype        = EXTENDVAAI_OTHER;
    stExtCmdArgs.dis_fh.data.data_len  = pstFH->data.data_len;
    stExtCmdArgs.dis_fh.data.data_val  = (char*)malloc(NFS3_FHSIZE);

    CHECK_MALLOC_POINT_ERROR(stExtCmdArgs.dis_fh.data.data_val, "NofsNasCloneAbort: dis_fh does not have enough memory.")

    memset_s(stExtCmdArgs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(stExtCmdArgs.dis_fh.data.data_val, NFS3_FHSIZE, pstFH->data.data_val, pstFH->data.data_len);

    stExtCmdArgs.cmdarg.data.data_len  = sizeof(stPrivateData);
    stExtCmdArgs.cmdarg.data.data_val  = (char*)malloc(sizeof(stPrivateData));
    if (NULL == stExtCmdArgs.cmdarg.data.data_val)
    {
        Log(LGPFX"NofsNasCloneAbort: cmdarg does not have enough memory.\n");
        free(stExtCmdArgs.dis_fh.data.data_val);

        stExtCmdArgs.dis_fh.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(stExtCmdArgs.cmdarg.data.data_val, sizeof(stPrivateData), 0, sizeof(stPrivateData));
    memcpy_s(stExtCmdArgs.cmdarg.data.data_val, sizeof(stPrivateData), &stPrivateData, sizeof(stPrivateData));

    iRet = nfsproc3_extendvaai_3(&stExtCmdArgs, pstDstSession->clnt, &stExtres);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasCloneAbort: call function(nfsproc3_extendvaai_3) failed.\n");
        free(stExtCmdArgs.dis_fh.data.data_val);
        free(stExtCmdArgs.cmdarg.data.data_val);

        stExtCmdArgs.dis_fh.data.data_val = NULL;
        stExtCmdArgs.cmdarg.data.data_val = NULL;

        return VIX_E_FAIL;
    }

    free(stExtCmdArgs.dis_fh.data.data_val);
    free(stExtCmdArgs.cmdarg.data.data_val);

    stExtCmdArgs.dis_fh.data.data_val = NULL;
    stExtCmdArgs.cmdarg.data.data_val = NULL;

    if (VIX_OK != stExtres.status)
    {
        if (stExtres.EXTENDCMDres_u.resfail.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasCloneAbort: Output data len error.\n");
            clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtres);
            return VIX_E_FAIL;
        }

        outputdata = (VAAI_OUTPUT_DATA_S *)stExtres.EXTENDCMDres_u.resfail.cmdres.data.data_val;
        if (VIX_OK == outputdata->result)
        {
            Log(LGPFX"NofsNasCloneAbort: function(nfsproc3_extendvaai_3) failed (NFS=%d).\n",stExtres.status);
            status = NofsNasMakeVixError(0, (nfsstat3)stExtres.status, MOUNT_OK);

            clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtres);
            return status;
        }

        Log(LGPFX"NofsNasCloneAbort: function(nfsproc3_extendvaai_3) failed (NFS=%d), result(FS=%d).\n",
            stExtres.status, outputdata->result);

        status = NofsNasMakeVixErrorFromFileSys(outputdata->result);

        clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtres);
        return status;
     }

    clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtres);

    Log(LGPFX"NofsNasCloneAbort: successfully.\n");

    return VIX_OK;
}


static VixError
NofsNasGetCloneProc(exNasPluginSession* pstDstSession,
                    nfs_fh3* pstFH,
                    FILECLONE_PROCINFO_S* pstCloneInfo)
{
    EXTENDCMDargs        stExtCmdArgs;
    VAAI_PRIVATEDATA_S   stPrivateData;
    EXTENDCMDres         stExtCmdRes;
    VAAI_OUTPUT_DATA_S   *outputdata        = NULL;
    VAAI_OUTPUT_DATA_S   *outputdatafail    = NULL;
    VixError             status             = 0;
    int                  iRet               = 0;

    CHECK_INPUT_NULL_POINT(pstDstSession, "NofsNasGetCloneProc: pstDstSession is null.")
    CHECK_INPUT_NULL_POINT(pstFH,         "NofsNasGetCloneProc: pstFH is null.")
    CHECK_INPUT_NULL_POINT(pstCloneInfo,  "NofsNasGetCloneProc: pstCloneInfo is null.")

    memset_s(&stExtCmdArgs, sizeof(stExtCmdArgs), 0, sizeof(stExtCmdArgs));
    memset_s(&stExtCmdRes, sizeof(stExtCmdRes), 0, sizeof(stExtCmdRes));
    memset_s(&stPrivateData, sizeof(stPrivateData), 0, sizeof(stPrivateData));

    stPrivateData.version      = VAAI_NAS_MSG_VERSION;
    stPrivateData.vaaiCmdType  = VAAICMD_FCLNPROC;
    stExtCmdArgs.dis_fh.data.data_len = pstFH->data.data_len;
    stExtCmdArgs.dis_fh.data.data_val = (char*) malloc(NFS3_FHSIZE);

    CHECK_MALLOC_POINT_ERROR(stExtCmdArgs.dis_fh.data.data_val, "NofsNasGetCloneProc: dis_fh does not have enough memory.")

    memset_s(stExtCmdArgs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(stExtCmdArgs.dis_fh.data.data_val, NFS3_FHSIZE, pstFH->data.data_val, pstFH->data.data_len);

    stExtCmdArgs.src_fh.cmdtype       = EXTENDVAAI_OTHER;
    stExtCmdArgs.cmdarg.data.data_len = sizeof(stPrivateData);
    stExtCmdArgs.cmdarg.data.data_val = (char*)malloc(sizeof(stPrivateData));
    if (NULL == stExtCmdArgs.cmdarg.data.data_val)
    {
        Log(LGPFX"NofsNasGetCloneProc:  cmdarg does not have enough memory.\n");
        free(stExtCmdArgs.dis_fh.data.data_val);

        stExtCmdArgs.dis_fh.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(stExtCmdArgs.cmdarg.data.data_val, sizeof(stPrivateData), 0, sizeof(stPrivateData));
    memcpy_s(stExtCmdArgs.cmdarg.data.data_val, sizeof(stPrivateData), &stPrivateData, sizeof(stPrivateData));

    iRet = nfsproc3_extendvaai_3(&stExtCmdArgs, pstDstSession->clnt, &stExtCmdRes);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasGetCloneProc: call function(nfsproc3_extendvaai_3) failed.iRet=%d\n", iRet);
        free(stExtCmdArgs.dis_fh.data.data_val);
        free(stExtCmdArgs.cmdarg.data.data_val);

        stExtCmdArgs.dis_fh.data.data_val = NULL;
        stExtCmdArgs.cmdarg.data.data_val = NULL;

        return VIX_E_FAIL;
    }

    free(stExtCmdArgs.cmdarg.data.data_val);
    free(stExtCmdArgs.dis_fh.data.data_val);

    stExtCmdArgs.cmdarg.data.data_val = NULL;
    stExtCmdArgs.dis_fh.data.data_val = NULL;

    if (VIX_OK != stExtCmdRes.status)
    {
        if (stExtCmdRes.EXTENDCMDres_u.resfail.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasGetCloneProc: Output data len error.\n");
            clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtCmdRes);
            return VIX_E_FAIL;
        }

        outputdatafail= (VAAI_OUTPUT_DATA_S *)stExtCmdRes.EXTENDCMDres_u.resfail.cmdres.data.data_val;
        if (VIX_OK == outputdatafail->result)
        {
            Log(LGPFX"NofsNasGetCloneProc: failed call rpc Error (NFS=%d).\n", stExtCmdRes.status);

            status = NofsNasMakeVixError(0, (nfsstat3)stExtCmdRes.status, MOUNT_OK);
            clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtCmdRes);
            return status;
        }

        Log(LGPFX"NofsNasGetCloneProc: failed call rpc Error (NFS=%d), result(FS=%d).\n",
        stExtCmdRes.status, outputdatafail->result);

        status = NofsNasMakeVixErrorFromFileSys(outputdatafail->result);
        clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtCmdRes);
        return status;
     }

    if (stExtCmdRes.EXTENDCMDres_u.resok.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
    {
        Log(LGPFX"NofsNasGetCloneProc: resok Output data len error.\n");
        clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtCmdRes);
        return VIX_E_FAIL;
    }

    outputdata = (VAAI_OUTPUT_DATA_S *)stExtCmdRes.EXTENDCMDres_u.resok.cmdres.data.data_val;
    pstCloneInfo->cloneFlag   = outputdata->cloneinfo.cloneFlag;
    pstCloneInfo->cloneOffset = outputdata->cloneinfo.cloneOffset;

    Log(LGPFX"NofsNasGetCloneProc: CloneFlag(%llu), CloneOffset(%llu).\n",
        pstCloneInfo->cloneFlag,pstCloneInfo->cloneOffset);

    clnt_freeres(pstDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtCmdRes);

    return VIX_OK;
}

static VixError NofsNasFileReport(exNasPluginSession* pstDstSession,
                                  const VixDiskLibNasPluginCloneFileParams* pstCloneParams,
                                  char* pszDstfilePath)
{
    FILECLONE_PROCINFO_S stCloneInfo;
    nfs_fh3       stFH;
    fattr3        stAttributes;
    VixError      status       = 0;
    size3         ullProcCount = 0;
    time_t        curTime      = 0;
    time_t        beginTime    = 0;
    uint64        ulWriteSize  = 0;
    uint64        ulLaseWriteSize  = 0;
    uint64        ulWriteDiff  = 0;
    uint64        ulOffset     = 0;
    uint64        ulCheckSize  = 0;
    Bool          bContinue    = TRUE;

    CHECK_INPUT_NULL_POINT(pstDstSession,  "NofsNasCloneFileReport: pstDstSession is null.")
    CHECK_INPUT_NULL_POINT(pstCloneParams, "NofsNasCloneFileReport: pstCloneParams is null.")
    CHECK_INPUT_NULL_POINT(pszDstfilePath, "NofsNasCloneFileReport: pszDstfilePath is null.")

    memset_s(&stFH, sizeof(stFH), 0, sizeof(stFH));
    memset_s(&stCloneInfo, sizeof(stCloneInfo), 0, sizeof(stCloneInfo));
    memset_s(&stAttributes, sizeof(stAttributes), 0, sizeof(stAttributes));

    time(&beginTime);
    stFH.data.data_val = (char*)malloc(NFS3_FHSIZE);
    CHECK_MALLOC_POINT_ERROR(stFH.data.data_val, "NofsNasFileReport:  nfsFH does not have enough memory.")

    memset_s(stFH.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);

    status = NofsNasLookupFH(pstDstSession, &stFH, pszDstfilePath, &stAttributes);
    if (VIX_OK != status)
    {
        free(stFH.data.data_val);
        stFH.data.data_val = NULL;
        Log(LGPFX"NofsNasCloneFileReport:dst file(%s) can not detected.\n", pszDstfilePath);

        return VIX_E_FAIL;
    }

    while (TRUE)
    {
        /* Get Clone Proc*/
        status = NofsNasGetCloneProc(pstDstSession, &stFH, &stCloneInfo);
        if (VIX_OK != status)
        {
            if (ullProcCount < RETRYTIME)
            {
                ullProcCount++;
                sleep(SLEEPTIME);
                continue;
            }

            free(stFH.data.data_val);

            stFH.data.data_val = NULL;
            Log(LGPFX"NofsNasCloneFileReport:NofsNasGetCloneProc(%s) failed(%d).\n", pszDstfilePath, status);

            return VIX_E_FAIL;
         }

        if (CLONEFILE_PROC_FLAG_COMPLETE == stCloneInfo.cloneFlag)
        {
            Log(LGPFX"NofsNasCloneFileReport: file(%s) clone complete.\n", pszDstfilePath);
            break;
        }

        //update progress
        ulWriteSize = stCloneInfo.cloneOffset;
        ulCheckSize = ulWriteSize - ulOffset;
        ulWriteDiff = ulWriteSize - ulLaseWriteSize;
        if ((pstCloneParams->common.progressRecord->updateBytes > 0)
            && (ulCheckSize >= pstCloneParams->common.progressRecord->updateBytes))
        {
            pstCloneParams->common.progressRecord->progressBytes = ulCheckSize;

            Log(LGPFX"NofsNasFullCloneFileReport: ProgressBytes(%llu), UpdateBytes(%llu).\n",
                pstCloneParams->common.progressRecord->progressBytes,
                pstCloneParams->common.progressRecord->updateBytes);

            time(&beginTime);
            bContinue = pstCloneParams->common.progressRecord->callback(pstCloneParams->common.progressRecord);
            if (FALSE == bContinue)
            {
                Log(LGPFX"NofsNasCloneFileReport:Operation(%s) cancelled by caller ulWriteSize=%llu.\n", pszDstfilePath, ulWriteSize);
                status = NofsNasCloneAbort(pstDstSession, &stFH);
                if (VIX_OK != status)
                {
                    Log(LGPFX"NofsNasCloneFileReport:NofsNasCloneAbort(%s) failed(%d).\n", pszDstfilePath, status);
                }

                free(stFH.data.data_val);
                stFH.data.data_val = NULL;

                return VIX_E_CANCELLED;
            }

            ulOffset = ulWriteSize;
        }

        ulLaseWriteSize = ulWriteSize;

        time(&curTime);
        if ((0 == ulWriteDiff) && (CLONETIMEOUT < (curTime - beginTime)))
        {
            Log(LGPFX"NofsNasCloneFileReport:full clone is error, the timeout is %lld.\n", (curTime - beginTime));
            status = NofsNasCloneAbort(pstDstSession, &stFH);
            if (VIX_OK != status)
            {
                Log(LGPFX"NofsNasCloneFileReport:NofsNasCloneAbort failed.\n");
            }

            free(stFH.data.data_val);
            stFH.data.data_val = NULL;

            return VIX_E_FAIL;
        }

        ullProcCount = 0;
        sleep(SLEEPTIME);
    }

    free(stFH.data.data_val);
    stFH.data.data_val = NULL;

    return VIX_OK;
}

VixError NofsNasSessionIsSupport(exNasPluginSession* pszSrcSession,
                                        exNasPluginSession* pszDstSession)
{
    VixError      Supportstatus    = 0;
    uint64_t      uiFSVersion      = 0;

    Supportstatus = NofsNasIsSupport(pszSrcSession, VAAICMD_CLN_SUPPORT, &uiFSVersion);
    if (VIX_OK != Supportstatus)
    {
        Log(LGPFX"NofsNasCloneFile: NofsNasCloneFile pszSrcSession is not support.\n");
        return VIX_E_FAIL;
    }

    Supportstatus = NofsNasIsSupport(pszDstSession, VAAICMD_CLN_SUPPORT, &uiFSVersion);
    if (VIX_OK != Supportstatus)
    {
        Log(LGPFX"NofsNasCloneFile: NofsNasCloneFile pszDstSession is not support.\n");
        return VIX_E_FAIL;
    }

    return VIX_OK;
}

VixError NofsNasCloneFile(exNasPluginSession* pszSrcSession,
                          exNasPluginSession* pszDstSession,
                          const VixDiskLibNasPluginCloneFileParams* pszCloneParams)
{

    nfs_fh3       stSrcFH, stDstFH;
    fattr3        stAttributes;
    EXTENDCMDargs stExtendArgs;
    EXTENDCMDres  stExtendres;
    int           iRet             = 0;
    VixError      status           = 0;
    VixError      handleExpStatus  = 0;
    VixError      Supportstatus    = 0;
    char*                 pszSrcPath       = NULL;
    char*                 pszDstPath       = NULL;
    VAAI_PRIVATEDATA_S    *pszPrivateData  = NULL;
    VAAI_OUTPUT_DATA_S    *outputdata      = NULL;


    CHECK_INPUT_NULL_POINT(pszSrcSession,  "NofsNasCloneFile: pszSrcSession is NULL .")
    CHECK_INPUT_NULL_POINT(pszDstSession,  "NofsNasCloneFile: pszDstSession is NULL .")
    CHECK_INPUT_NULL_POINT(pszCloneParams, "NofsNasCloneFile: pszCloneParams is NULL .")

    Bool bLazyClone = (Bool)pszCloneParams->cloneFlags & VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_LAZY;
    Bool bDryrun    = (Bool)pszCloneParams->cloneFlags & VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_DRYRUN;

    memset_s(&stSrcFH, sizeof(stSrcFH), 0, sizeof(stSrcFH));
    memset_s(&stDstFH, sizeof(stDstFH), 0, sizeof(stDstFH));
    memset_s(&stExtendArgs, sizeof(stExtendArgs), 0, sizeof(stExtendArgs));
    memset_s(&stExtendres, sizeof(stExtendres), 0, sizeof(stExtendres));
    memset_s(&stAttributes, sizeof(stAttributes), 0, sizeof(stAttributes));

    Log(LGPFX"NofsNasCloneFile,request to copy %s to %s flags %x \n",
        pszCloneParams->srcFileName,
        pszCloneParams->dstFileName,
        pszCloneParams->cloneFlags);

    NAS_CLONE_FILE_SRCDST_MOUNT_MATCH(pszCloneParams, pszSrcSession, pszDstSession,
        "NofsNasCloneFile: the mount point is not match.")

    Supportstatus = NofsNasSessionIsSupport(pszSrcSession, pszDstSession);
    if (VIX_OK != Supportstatus)
    {
        return Supportstatus;
    }

    if (bDryrun)
    {
        Log(LGPFX"NofsNasCloneFile: Src and Dst is support & Dryrun.\n");
        return VIX_OK;
    }

    /*local mount*/
    pszSrcPath = &(pszCloneParams->srcFileName[pszSrcSession->localMPLen + 1]);
    pszDstPath = &(pszCloneParams->dstFileName[pszDstSession->localMPLen + 1]);

    /*find srcFH*/
    stSrcFH.data.data_val = (char*)malloc(NFS3_FHSIZE);
    CHECK_MALLOC_POINT_ERROR(stSrcFH.data.data_val,"NofsNasCloneFile:  srcFH does not have enough memory.")

    memset_s(stSrcFH.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);

    status = NofsNasLookupFH(pszSrcSession, &stSrcFH, pszSrcPath, &stAttributes);
    if (VIX_OK != status)
    {
        Log(LGPFX"NofsNasCloneFile: lookup srcFH error (%d).\n", status);
        free(stSrcFH.data.data_val);
        stSrcFH.data.data_val = NULL;

        return status;
    }

    /*find DstFH*/
    stDstFH.data.data_val = (char*)malloc(NFS3_FHSIZE);
    if (NULL == stDstFH.data.data_val)
    {
        Log(LGPFX"NofsNasCloneFile:  dstFH does not have enough memory.\n");
        free(stSrcFH.data.data_val);
        stSrcFH.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(stDstFH.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memset_s(&stAttributes, sizeof(stAttributes), 0, sizeof(stAttributes));

    status = NofsNasLookupFH(pszDstSession, &stDstFH, pszDstPath, &stAttributes);
    handleExpStatus = NofsNasHandingExcption(pszCloneParams, status, pszDstSession, &stDstFH, pszDstPath, &stAttributes);
    if (VIX_OK != handleExpStatus)
    {
        Log(LGPFX"NofsNasCloneFile:  NofsNasHandingExcption %d.\n", handleExpStatus);
        free(stSrcFH.data.data_val);
        free(stDstFH.data.data_val);

        stSrcFH.data.data_val = NULL;
        stDstFH.data.data_val = NULL;

        return handleExpStatus;
    }

    /* rpc transfer*/
    stExtendArgs.src_fh.cmdtype       = EXTENDVAAI_NEED_SRC_FH;
    stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_len = stSrcFH.data.data_len;
    stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val = (char*)malloc(NFS3_FHSIZE);
    if (NULL == stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val)
    {
        Log(LGPFX"NofsNasCloneFile:  src_fh does not have enough memory.\n");
        free(stSrcFH.data.data_val);
        free(stDstFH.data.data_val);

        stSrcFH.data.data_val = NULL;
        stDstFH.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val, NFS3_FHSIZE,
        stSrcFH.data.data_val, stSrcFH.data.data_len);

    stExtendArgs.dis_fh.data.data_len = stDstFH.data.data_len;
    stExtendArgs.dis_fh.data.data_val = (char*)malloc(NFS3_FHSIZE);
    if (NULL == stExtendArgs.dis_fh.data.data_val)
    {
        Log(LGPFX"NofsNasCloneFile:  dis_fh does not have enough memory.\n");
        free(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val);
        free(stDstFH.data.data_val);
        free(stSrcFH.data.data_val);

        stDstFH.data.data_val = NULL;
        stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val = NULL;
        stSrcFH.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(stExtendArgs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(stExtendArgs.dis_fh.data.data_val, NFS3_FHSIZE, stDstFH.data.data_val, stDstFH.data.data_len);

    stExtendArgs.cmdarg.data.data_len = sizeof(VAAI_PRIVATEDATA_S);
    pszPrivateData = (VAAI_PRIVATEDATA_S*)malloc(sizeof(VAAI_PRIVATEDATA_S));
    if (NULL == pszPrivateData)
    {
        Log(LGPFX"NofsNasCloneFile: Failed malloc pPrivateData.");
        free(stExtendArgs.dis_fh.data.data_val);
        free(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val);
        free(stDstFH.data.data_val);
        free(stSrcFH.data.data_val);

        stExtendArgs.dis_fh.data.data_val = NULL;
        stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val = NULL;
        stDstFH.data.data_val = NULL;
        stSrcFH.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(pszPrivateData, sizeof(VAAI_PRIVATEDATA_S), 0, sizeof(VAAI_PRIVATEDATA_S));

    stExtendArgs.cmdarg.data.data_val       = (char*)pszPrivateData;
    pszPrivateData->version                 = VAAI_NAS_MSG_VERSION;
    pszPrivateData->vaaiCmdType             = VAAICMD_FCLONE;
    pszPrivateData->cloneData.cloneFlag     = pszCloneParams->cloneFlags;
    pszPrivateData->cloneData.nameLen       = strlen(pszDstPath) + 1;

    memcpy_s(pszPrivateData->cloneData.cloneFileName, sizeof(pszPrivateData->cloneData.cloneFileName),
        pszDstPath, strlen(pszDstPath) + 1);

    iRet = nfsproc3_extendvaai_3(&stExtendArgs, pszDstSession->clnt, &stExtendres);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasCloneFile:nfsproc3_extendvaai_3 returned NULL. \n");
        free(pszPrivateData);
        free(stExtendArgs.dis_fh.data.data_val);
        free(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val);
        free(stDstFH.data.data_val);
        free(stSrcFH.data.data_val);

        pszPrivateData = NULL;
        stExtendArgs.dis_fh.data.data_val = NULL;
        stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val = NULL;
        stDstFH.data.data_val = NULL;
        stSrcFH.data.data_val = NULL;

        return VIX_E_FAIL;
    }

    free(pszPrivateData);
    free(stExtendArgs.dis_fh.data.data_val);
    free(stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val);
    free(stDstFH.data.data_val);
    free(stSrcFH.data.data_val);

    pszPrivateData = NULL;
    stExtendArgs.dis_fh.data.data_val = NULL;
    stExtendArgs.src_fh.vaai_extend_fh_u.ffh.data.data_val = NULL;
    stDstFH.data.data_val = NULL;
    stSrcFH.data.data_val = NULL;

    if (VIX_OK != stExtendres.status)
    {
        if (stExtendres.EXTENDCMDres_u.resfail.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasCloneFile: Output data len error.\n");
            clnt_freeres(pszDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtendres);
            return VIX_E_FAIL;
        }

        outputdata = (VAAI_OUTPUT_DATA_S*)stExtendres.EXTENDCMDres_u.resfail.cmdres.data.data_val;

        if (VIX_OK == outputdata->result)
        {
            Log(LGPFX"nfsproc3_extendvaai_3: Error (NFS=%d).\n",  stExtendres.status);

            status = NofsNasMakeVixError(0, (nfsstat3)stExtendres.status, MOUNT_OK);
            clnt_freeres(pszDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtendres);
            return status;
        }

        Log(LGPFX"nfsproc3_extendvaai_3: Error (NFS=%d), result(FS=%d).\n",
        stExtendres.status, outputdata->result);

        status = NofsNasMakeVixErrorFromFileSys(outputdata->result);
        clnt_freeres(pszDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtendres);
        return status;
     }

    if (!bLazyClone)
    {
        status = NofsNasFileReport(pszDstSession, pszCloneParams, pszDstPath);
        Log(LGPFX"NofsNasCloneFile:Full Clone file report status is (%d).\n", status);
    }

    clnt_freeres(pszDstSession->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&stExtendres);

    return status;
}

static VixError
NofsNasLazyCloneExcption(Bool bLazyClone,
                                Bool bGuarded,
                                exNasPluginSession* pszDstSession,
                                nfs_fh3*            pszDstFH,
                                char*               pszDstPath,
                                fattr3*             pszAttributes)
{
    VixError localStatus = 0;

    if (!bLazyClone)
    {
        return VIX_OK;
    }

    if (bGuarded)
    {
        Log(LGPFX"NofsNasHandingExcption: The file '%s' already exists but the GUARDED flag is set.\n", pszDstPath);
        return VIX_E_FILE_ALREADY_EXISTS;
    }

    localStatus = NofsNasRemoveFile(pszDstSession, pszDstFH, pszDstPath);
    if (VIX_OK != localStatus)
    {
        Log(LGPFX"NofsNasHandingExcption: Remove file '%s' error.\n", pszDstPath);
        return localStatus;
    }

    localStatus = NofsNasCreateAndLookupFile(pszDstSession, pszDstFH, pszDstPath, pszAttributes);
    if (VIX_OK != localStatus)
    {
        Log(LGPFX"NofsNasHandingExcption: Create dst file '%s' error.\n", pszDstPath);
        return localStatus;
    }

    return VIX_OK;
}

static VixError
NofsNasHandingExcption(const VixDiskLibNasPluginCloneFileParams* pszCloneParams,
                       VixError            status,
                       exNasPluginSession* pszDstSession,
                       nfs_fh3*            pszDstFH,
                       char*               pszDstPath,
                       fattr3*             pszAttributes)

{
    VixError localStatus = 0;

    CHECK_INPUT_NULL_POINT(pszCloneParams,  "NofsNasHandingFileExcption: pszCloneParams is NULL .")
    CHECK_INPUT_NULL_POINT(pszDstSession,   "NofsNasHandingFileExcption: pszDstSession is NULL .")
    CHECK_INPUT_NULL_POINT(pszDstFH,        "NofsNasHandingFileExcption: pszDstFHis NULL .")
    CHECK_INPUT_NULL_POINT(pszDstPath,      "NofsNasHandingFileExcption: pszDstPath is NULL .")
    CHECK_INPUT_NULL_POINT(pszAttributes,   "NofsNasHandingFileExcption: pszAttributes is NULL .")

    Bool bGuarded = (Bool)pszCloneParams->cloneFlags & VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_GUARDED;
    Bool bLazyClone = (Bool)pszCloneParams->cloneFlags & VIXDISKLIB_NASPLUGIN_CLONEFILE_FLAG_LAZY;

    if (VIX_OK == status)
    {
        localStatus = NofsNasLazyCloneExcption(bLazyClone, bGuarded, pszDstSession, pszDstFH, pszDstPath, pszAttributes);
        if (VIX_OK != localStatus)
        {
            return localStatus;
        }
    }
    else if (VIX_E_FILE_NOT_FOUND == status)
    {
        if (!bLazyClone)
        {
            localStatus = NofsNasCreateAndLookupFile(pszDstSession, pszDstFH, pszDstPath, pszAttributes);
            if (VIX_OK != localStatus)
            {
                Log(LGPFX"NofsNasHandingExcption: Create dst file '%s' error.\n", pszDstPath);

                return status;
            }
        }
    }
    else
    {
        Log(LGPFX"NofsNasHandingExcption: Look up dstFH '%s' error.\n", pszDstPath);
        return status;
    }

    Log(LGPFX"NofsNasHandingExcption: status is OK.\n");
    return VIX_OK;
}

