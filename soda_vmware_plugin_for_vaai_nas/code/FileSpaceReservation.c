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

#include "FileSpaceReservation.h"

VixError
NofsNasFileReserveSpace(exNasPluginSession *session,
                        VixDiskLibNasPluginResvSpaceParams *reserveSpaceParams)
{
    VAAI_PRIVATEDATA_S      argPrivateData;
    nfs_fh3             nfsFH;
    fattr3              attributes;
    EXTENDCMDargs       extendCMDArgs;
    EXTENDCMDres        extendCMDres;
    char                *filePath       = NULL;
    VAAI_OUTPUT_DATA_S      *outputdata     = NULL;
    VixError            status          = 0;
    VixError                Supportstatus   = 0;
    int                     iRet            = 0;
    uint64_t                uiFSVersion     = 0;

    CHECK_INPUT_NULL_POINT(session,            "NofsNasFileReserveSpace: The parameter session is null.")
    CHECK_INPUT_NULL_POINT(reserveSpaceParams, "NofsNasFileReserveSpace: The parameter reserveSpaceParams is null.")

    Log(LGPFX"NofsNasFileReserveSpace: Prepare to reserve space for %s.\n", reserveSpaceParams->fileName);

    memset_s(&nfsFH, sizeof(nfsFH), 0, sizeof(nfsFH));
    memset_s(&attributes, sizeof(attributes), 0, sizeof(attributes));
    memset_s(&extendCMDArgs, sizeof(extendCMDArgs), 0, sizeof(extendCMDArgs));
    memset_s(&argPrivateData, sizeof(argPrivateData), 0, sizeof(argPrivateData));
    memset_s(&extendCMDres, sizeof(extendCMDres), 0, sizeof(extendCMDres));

    if (0 != strncmp(reserveSpaceParams->fileName, session->localMountPoint, session->localMPLen) )
    {
        Log(LGPFX"NofsNasFileReserveSpace: The mount point on ESXi Server doesn't match.\n");
        return VIX_E_DISK_INVAL;
    }

    //if not support,return VIX_OK but prints a warning
    Supportstatus = NofsNasIsSupport(session, VAAICMD_RSV_SUPPORT, &uiFSVersion);
    if (VIX_OK != Supportstatus)
    {
        Warning(LGPFX"NofsNasFileReserveSpace: FileReserveSpace is not support.\n");
        return VIX_OK;
    }

    /* Get path relative to the local mount point  */
    filePath = &(reserveSpaceParams->fileName[session->localMPLen + 1]);

    nfsFH.data.data_val = (char *)malloc(NFS3_FHSIZE);

    CHECK_MALLOC_POINT_ERROR(nfsFH.data.data_val, "NofsNasFileReserveSpace: nfsFH.data.data_val does not have enough memory.")
    memset_s(nfsFH.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);

    /* lookup file handle  */
    status = NofsNasLookupFH(session, &nfsFH, filePath, &attributes);
    if (status != VIX_OK)
    {
        Log(LGPFX"NofsNasFileReserveSpace: Lookup the path(%s) on NAS Server(%s) failed.\n", filePath, session->remoteIP);

        free(nfsFH.data.data_val);
        nfsFH.data.data_val = NULL;

        return status;
    }

    argPrivateData.version       = VAAI_NAS_MSG_VERSION;
    argPrivateData.vaaiCmdType   = VAAICMD_FRESERVE;
    extendCMDArgs.src_fh.cmdtype = EXTENDVAAI_OTHER;
    extendCMDArgs.dis_fh.data.data_len = nfsFH.data.data_len;
    extendCMDArgs.dis_fh.data.data_val = (char *)malloc(NFS3_FHSIZE);
    if (NULL == extendCMDArgs.dis_fh.data.data_val)
    {
        Log(LGPFX"NofsNasFileReserveSpace: dis_fh does not have enough memory.\n");
        free(nfsFH.data.data_val);
        nfsFH.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(extendCMDArgs.dis_fh.data.data_val, NFS3_FHSIZE, 0, NFS3_FHSIZE);
    memcpy_s(extendCMDArgs.dis_fh.data.data_val, NFS3_FHSIZE, nfsFH.data.data_val, nfsFH.data.data_len);

    extendCMDArgs.cmdarg.data.data_len = sizeof(argPrivateData);
    extendCMDArgs.cmdarg.data.data_val = (char *)malloc(sizeof(argPrivateData));
    if (NULL == extendCMDArgs.cmdarg.data.data_val)
    {
        Log(LGPFX"NofsNasFileReserveSpace: cmdarg does not have enough memory.\n");
        free(nfsFH.data.data_val);
        free(extendCMDArgs.dis_fh.data.data_val);
        nfsFH.data.data_val = NULL;
        extendCMDArgs.dis_fh.data.data_val = NULL;

        return VIX_E_OUT_OF_MEMORY;
    }

    memset_s(extendCMDArgs.cmdarg.data.data_val, sizeof(argPrivateData), 0, sizeof(argPrivateData));
    memcpy_s(extendCMDArgs.cmdarg.data.data_val, sizeof(argPrivateData), &argPrivateData, sizeof(argPrivateData));

    iRet = nfsproc3_extendvaai_3(&extendCMDArgs, session->clnt, &extendCMDres);
    if (RPC_SUCCESS != iRet)
    {
        Log(LGPFX"NofsNasFileReserveSpace: call function(nfsproc3_extendvaai_3) failed.\n");
        free(nfsFH.data.data_val);
        free(extendCMDArgs.dis_fh.data.data_val);
        free(extendCMDArgs.cmdarg.data.data_val);

        nfsFH.data.data_val = NULL;
        extendCMDArgs.dis_fh.data.data_val = NULL;
        extendCMDArgs.cmdarg.data.data_val = NULL;

        return VIX_E_FAIL;
    }

    free(nfsFH.data.data_val);
    free(extendCMDArgs.dis_fh.data.data_val);
    free(extendCMDArgs.cmdarg.data.data_val);

    nfsFH.data.data_val = NULL;
    extendCMDArgs.dis_fh.data.data_val = NULL;
    extendCMDArgs.cmdarg.data.data_val = NULL;

    if (VIX_OK != extendCMDres.status)
    {
        if (extendCMDres.EXTENDCMDres_u.resfail.cmdres.data.data_len != sizeof(VAAI_OUTPUT_DATA_S))
        {
            Log(LGPFX"NofsNasFileReserveSpace: Output data len error.\n");
            clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);
            return VIX_E_FAIL;
        }

        outputdata = (VAAI_OUTPUT_DATA_S *)extendCMDres.EXTENDCMDres_u.resfail.cmdres.data.data_val;

        if (VIX_OK == outputdata->result)
        {
            Log(LGPFX"NofsNasFileReserveSpace: Reserve space for File(%s) failed, Error (NFS=%d).\n",
                reserveSpaceParams->fileName, extendCMDres.status);

            status = NofsNasMakeVixError(0, (nfsstat3)extendCMDres.status, MOUNT_OK);
            clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);
            return status;
        }

        Log(LGPFX"NofsNasFileReserveSpace: Reserve space for File(%s) failed, Error (NFS=%d), result(FS=%d).\n",
            reserveSpaceParams->fileName, extendCMDres.status, outputdata->result);

        status = NofsNasMakeVixErrorFromFileSys(outputdata->result);
        clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);
        return status;
    }

    Log(LGPFX"NofsNasFileReserveSpace: Reserve space successfully.\n");
    clnt_freeres(session->clnt, (xdrproc_t)xdr_EXTENDCMDres, (caddr_t)&extendCMDres);

    return VIX_OK;
}

