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
#include "vaai_nas.h"

bool_t
xdr_vaaifilename3 (XDR *xdrs, vaaifilename3 *objp)
{
    if (!xdr_string (xdrs, objp, ~0))
        return FALSE;
    return TRUE;
}

bool_t
xdr_nfsfh3 (XDR *xdrs, nfsfh3 *objp)
{
    if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, FHSIZE3))
        return FALSE;
    return TRUE;
}

bool_t
xdr_cmdargdata3 (XDR *xdrs, cmdargdata3 *objp)
{
    if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, EXTENDCMDSIZE3))
        return FALSE;
    return TRUE;
}

bool_t
xdr_EXTENDCMDTYPE (XDR *xdrs, EXTENDCMDTYPE *objp)
{
    if (!xdr_enum (xdrs, (enum_t *) objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_vaai_extend_fh (XDR *xdrs, vaai_extend_fh *objp)
{
    if (!xdr_EXTENDCMDTYPE (xdrs, &objp->cmdtype))
        return FALSE;
    switch (objp->cmdtype) {
    case EXTENDVAAI_NEED_SRC_FH:
        if (!xdr_nfsfh3 (xdrs, &objp->vaai_extend_fh_u.ffh))
            return FALSE;
        break;
    default:
        break;
    }
    return TRUE;
}

bool_t
xdr_EXTENDCMDargs (XDR *xdrs, EXTENDCMDargs *objp)
{
    if (!xdr_nfsfh3 (xdrs, &objp->dis_fh))
        return FALSE;
    if (!xdr_vaai_extend_fh (xdrs, &objp->src_fh))
        return FALSE;
    if (!xdr_cmdargdata3 (xdrs, &objp->cmdarg))
        return FALSE;
    return TRUE;
}

bool_t
xdr_cmdresdata3 (XDR *xdrs, cmdresdata3 *objp)
{
    if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, VAAISIZE3))
        return FALSE;
    return TRUE;
}

bool_t
xdr_extendcmdresult (XDR *xdrs, extendcmdresult *objp)
{
    if (!xdr_cmdresdata3 (xdrs, &objp->cmdres))
        return FALSE;
    return TRUE;
}

bool_t
xdr_EXTENDCMDres (XDR *xdrs, EXTENDCMDres *objp)
{
    if (!xdr_int32_t (xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
    case 0:
        if (!xdr_extendcmdresult (xdrs, &objp->EXTENDCMDres_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_extendcmdresult (xdrs, &objp->EXTENDCMDres_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}

