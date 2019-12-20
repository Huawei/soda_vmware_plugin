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

#include "ExtendedStats.h"

VixError ExtendStatusExcption(exNasPluginSession *session,
                        EXTENDCMDres         *extres,
                        VixDiskLibNasPluginStatXParams *params)
{
    VixError             status                 = 0;
    VAAI_OUTPUT_DATA_S   *outputdatafail        = NULL;

    if (extres->EXTENDCMDres_u.resfail.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
    {
        Log(LGPFX"NofsNasGetExtendStats: Output data len error.\n");
        clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)extres);
        return VIX_E_FAIL;
    }

    outputdatafail = (VAAI_OUTPUT_DATA_S *)extres->EXTENDCMDres_u.resfail.cmdres.data.data_val;

    if (VIX_OK == outputdatafail->result)
    {
        Log(LGPFX"NofsNasGetExtendStats: the File(%s) failed, Error(NFS=%d).\n",
            params->fileName, extres->status);

        status = NofsNasMakeVixError(0, extres->status, MOUNT_OK);
        clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)extres);
        return status;
    }

    Log(LGPFX"NofsNasGetExtendStats: the File(%s) failed, Error(NFS=%d), result(FS=%d).\n",
        params->fileName, extres->status, outputdatafail->result);

    status = NofsNasMakeVixErrorFromFileSys(outputdatafail->result);
    clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)extres);

    return status;
}

VixError
NofsNasGetExtendStats(exNasPluginSession *session,
                      VixDiskLibNasPluginStatXParams *params)
{
    VAAI_PRIVATEDATA_S   privatedata;
    EXTENDCMDargs        extargs;
    EXTENDCMDres         extres;
    fattr3               attributes;
    nfs_fh3              fh;
    char                 *filePath              = NULL;
    VAAI_OUTPUT_DATA_S   *outputdata            = NULL;
    VixError             status                 = 0;
    VixDiskLibNasPluginStatXResult *statXResult = NULL;
    GETATTR3res          stGetAttrres;
    GETATTR3args         stGetAttr;
    VixError             Supportstatus          = 0;
    int                  iRet                   = 0;
    uint64_t             uiFSVersion            = 0;

    CHECK_INPUT_NULL_POINT(session, "NofsNasGetExtendStats: The Parameter session is null.")
    CHECK_INPUT_NULL_POINT(params,  "NofsNasGetExtendStats: The Parameter params is null.")

    Log(LGPFX"NofsNasGetExtendStats: request for stats of %s\n", params->fileName);

    if (0 != strncmp(params->fileName, session->localMountPoint, session->localMPLen))
    {
        Log(LGPFX"NofsNasGetExtendStats: The local mount point on the Esxi Server is not match.\n");
        return VIX_E_INVALID_ARG;
    }

    memset_s(&attributes, sizeof(attributes), 0, sizeof(attributes));
    memset_s(&fh, sizeof(nfs_fh3), 0, sizeof(nfs_fh3));
    memset_s(&extargs, sizeof(EXTENDCMDargs), 0, sizeof(EXTENDCMDargs));
    memset_s(&extres, sizeof(EXTENDCMDres), 0, sizeof(EXTENDCMDres));
    memset_s(&privatedata, sizeof(privatedata), 0, sizeof(privatedata));
    memset_s(&stGetAttr, sizeof(GETATTR3args), 0, sizeof(GETATTR3args));
    memset_s(&stGetAttrres, sizeof(GETATTR3res), 0, sizeof(GETATTR3res));

    statXResult = (VixDiskLibNasPluginStatXResult *)params->common.result;

    filePath = &(params->fileName[session->localMPLen + 1]);

    fh.data.data_val = (char *)malloc(NFS3_FHSIZE);

    CHECK_MALLOC_POINT_ERROR(fh.data.data_val, "NofsNasGetExtendStats: fh.data.data_val not have enough memory.")

    memset_s(fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    status = NofsNasLookupFH(session, &fh, filePath, &attributes);
    if (VIX_OK != status)
    {
        Log(LGPFX"NofsNasGetExtendStats: Lookup the path(%s) on NAS Server(%s) failed.\n", filePath, session->remoteIP);
        free(fh.data.data_val);

        fh.data.data_val = NULL;

        return status;
    }

    //if not support,invoke NFS getattr; if support,invoke VAAI-NAS
    Supportstatus = NofsNasIsSupport(session, VAAICMD_STAT_SUPPORT, &uiFSVersion);
    if (VIX_OK == Supportstatus)
    {
        //cmdtype
        extargs.src_fh.cmdtype = EXTENDVAAI_OTHER;

        //dis_fh
        extargs.dis_fh.data.data_len = fh.data.data_len;
        extargs.dis_fh.data.data_val = (char *)malloc(NFS3_FHSIZE);
        if (NULL == extargs.dis_fh.data.data_val)
        {
            Log(LGPFX"NofsNasGetExtendStats: dis_fh not have enough memory.\n");
            free(fh.data.data_val);

            fh.data.data_val = NULL;

            return VIX_E_OUT_OF_MEMORY;
        }

        memset_s(extargs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
        memcpy_s(extargs.dis_fh.data.data_val, NFS3_FHSIZE, fh.data.data_val, fh.data.data_len);

        //cmdarg
        privatedata.version     = VAAI_NAS_MSG_VERSION;
        privatedata.vaaiCmdType = VAAICMD_FSTAT;

        extargs.cmdarg.data.data_len = sizeof(privatedata);
        extargs.cmdarg.data.data_val = (char *)malloc(sizeof(privatedata));
        if (NULL == extargs.cmdarg.data.data_val)
        {
            Log(LGPFX"NofsNasGetExtendStats: cmdarg not have enough memory.\n");

            free(extargs.dis_fh.data.data_val);
            free(fh.data.data_val);

            extargs.dis_fh.data.data_val = NULL;
            fh.data.data_val = NULL;

            return VIX_E_OUT_OF_MEMORY;
        }

        memset_s(extargs.cmdarg.data.data_val, sizeof(privatedata), 0, sizeof(privatedata));
        memcpy_s(extargs.cmdarg.data.data_val, sizeof(privatedata), &privatedata, sizeof(privatedata));

        iRet = nfsproc3_extendvaai_3(&extargs, session->clnt, &extres);
        if (RPC_SUCCESS != iRet)
        {
            Log(LGPFX"NofsNasGetExtendStats: call function(nfsproc3_extendvaai_3) failed.\n");

            free(extargs.dis_fh.data.data_val);
            free(extargs.cmdarg.data.data_val);
            free(fh.data.data_val);

            extargs.dis_fh.data.data_val = NULL;
            extargs.cmdarg.data.data_val = NULL;
            fh.data.data_val = NULL;

            return VIX_E_FAIL;
        }

        free(extargs.dis_fh.data.data_val);
        free(extargs.cmdarg.data.data_val);
        free(fh.data.data_val);

        extargs.dis_fh.data.data_val = NULL;
        extargs.cmdarg.data.data_val = NULL;
        fh.data.data_val = NULL;

        if (VIX_OK != extres.status)
        {
            return ExtendStatusExcption(session, &extres, params);
        }

        if (extres.EXTENDCMDres_u.resok.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasGetExtendStats: resok Output data len error.\n");
            clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extres);
            return VIX_E_FAIL;
        }

        outputdata = (VAAI_OUTPUT_DATA_S *)extres.EXTENDCMDres_u.resok.cmdres.data.data_val;

        statXResult->totalBytes   = outputdata->statinfo.totalBytes;
        statXResult->allocedBytes = outputdata->statinfo.allocedBytes;
        statXResult->uniqueBytes  = outputdata->statinfo.uniqueBytes;

        clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extres);
    }
    else
    {
        stGetAttr.object.data.data_len = fh.data.data_len;
        stGetAttr.object.data.data_val = (char *)malloc(NFS3_FHSIZE);
        if (NULL == stGetAttr.object.data.data_val)
        {
            Log(LGPFX"NofsNasGetExtendStats: object not have enough memory.\n");
            free(fh.data.data_val);
            fh.data.data_val = NULL;
            return VIX_E_OUT_OF_MEMORY;
        }

        memset_s(stGetAttr.object.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
        memcpy_s(stGetAttr.object.data.data_val, NFS3_FHSIZE, fh.data.data_val, fh.data.data_len);

        iRet= nfsproc3_getattr_3(&stGetAttr, session->clnt, &stGetAttrres);
        if (RPC_SUCCESS != iRet)
        {
            Log(LGPFX"NofsNasGetExtendStats: call nfsproc3_getattr_3 failed.\n");
            free(fh.data.data_val);
            free(stGetAttr.object.data.data_val);

            fh.data.data_val = NULL;
            stGetAttr.object.data.data_val = NULL;

            return VIX_E_FAIL;
        }

        status = NofsNasMakeVixError(0, stGetAttrres.status, MOUNT_OK);
        if (VIX_OK != status)
        {
            Log(LGPFX"NofsNasGetExtendStats: nfsproc3_getattr_3 failed status=%s.\n", status);
            free(fh.data.data_val);
            free(stGetAttr.object.data.data_val);

            fh.data.data_val = NULL;
            stGetAttr.object.data.data_val = NULL;
            clnt_freeres(session->clnt, (xdrproc_t)xdr_GETATTR3res, (caddr_t)&stGetAttrres);

            return status;
        }

        free(fh.data.data_val);
        free(stGetAttr.object.data.data_val);

        fh.data.data_val = NULL;
        stGetAttr.object.data.data_val = NULL;

        statXResult->totalBytes   = stGetAttrres.GETATTR3res_u.resok.obj_attributes.size;
        statXResult->allocedBytes = stGetAttrres.GETATTR3res_u.resok.obj_attributes.used;
        statXResult->uniqueBytes  = statXResult->allocedBytes;

        clnt_freeres(session->clnt, (xdrproc_t)xdr_GETATTR3res, (caddr_t)&stGetAttrres);
    }

    Log(LGPFX"NofsNasGetExtendStats: The File(%s)'s statistic information : totalBytes=%lld, allocedBytes= %lld, uniqueBytes=%lld.\n",
        params->fileName, statXResult->totalBytes, statXResult->allocedBytes, statXResult->uniqueBytes);

    return VIX_OK;
}

