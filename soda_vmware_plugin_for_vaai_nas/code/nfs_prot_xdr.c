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

#include "nfs_prot.h"
#ifndef lint
static const char rcsid[] =
 "$FreeBSD: src/include/rpcsvc/nfs_prot.x,v 1.7 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

bool_t
xdr_uint64 (XDR *xdrs, uint64 *objp)
{
    if (!xdr_u_quad_t (xdrs, objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_uint32 (XDR *xdrs, uint32 *objp)
{
    if (!xdr_u_long (xdrs, (u_long *)objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_filename3 (XDR *xdrs, filename3 *objp)
{
    if (!xdr_string (xdrs, objp, ~0))
        return FALSE;
    return TRUE;
}

bool_t
xdr_fileid3 (XDR *xdrs, fileid3 *objp)
{
    if (!xdr_uint64 (xdrs, objp))
        return FALSE;
    return TRUE;
}


bool_t
xdr_createverf3 (XDR *xdrs, createverf3 objp)
{
    if (!xdr_opaque (xdrs, objp, NFS3_CREATEVERFSIZE))
        return FALSE;
    return TRUE;
}


bool_t
xdr_uid3 (XDR *xdrs, uid3 *objp)
{
    if (!xdr_uint32 (xdrs, objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_gid3 (XDR *xdrs, gid3 *objp)
{
    if (!xdr_uint32 (xdrs, objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_size3 (XDR *xdrs, size3 *objp)
{
    if (!xdr_uint64 (xdrs, objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_mode3 (XDR *xdrs, mode3 *objp)
{
    if (!xdr_uint32 (xdrs, objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_nfsstat3 (XDR *xdrs, nfsstat3 *objp)
{
    if (!xdr_enum (xdrs, (enum_t *) objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_ftype3 (XDR *xdrs, ftype3 *objp)
{
    if (!xdr_enum (xdrs, (enum_t *) objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_specdata3 (XDR *xdrs, specdata3 *objp)
{
    if (!xdr_uint32 (xdrs, &objp->specdata1))
        return FALSE;
    if (!xdr_uint32 (xdrs, &objp->specdata2))
        return FALSE;
    return TRUE;
}

bool_t
xdr_nfs_fh3 (XDR *xdrs, nfs_fh3 *objp)
{
    if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, NFS3_FHSIZE))
        return FALSE;
    return TRUE;
}

bool_t
xdr_nfstime3 (XDR *xdrs, nfstime3 *objp)
{
    if (!xdr_uint32 (xdrs, &objp->seconds))
        return FALSE;
    if (!xdr_uint32 (xdrs, &objp->nseconds))
        return FALSE;
    return TRUE;
}

bool_t
xdr_fattr3 (XDR *xdrs, fattr3 *objp)
{
     if (!xdr_ftype3 (xdrs, &objp->type))
        return FALSE;
    if (!xdr_mode3 (xdrs, &objp->mode))
        return FALSE;
    if (!xdr_uint32 (xdrs, &objp->nlink))
        return FALSE;
    if (!xdr_uid3 (xdrs, &objp->uid))
        return FALSE;
    if (!xdr_gid3 (xdrs, &objp->gid))
        return FALSE;
    if (!xdr_size3 (xdrs, &objp->size))
        return FALSE;
    if (!xdr_size3 (xdrs, &objp->used))
        return FALSE;
    if (!xdr_specdata3 (xdrs, &objp->rdev))
        return FALSE;
    if (!xdr_uint64 (xdrs, &objp->fsid))
        return FALSE;
    if (!xdr_fileid3 (xdrs, &objp->fileid))
        return FALSE;
    if (!xdr_nfstime3 (xdrs, &objp->atime))
        return FALSE;
    if (!xdr_nfstime3 (xdrs, &objp->mtime))
        return FALSE;
    if (!xdr_nfstime3 (xdrs, &objp->ctime))
        return FALSE;
    return TRUE;
}

bool_t
xdr_post_op_attr (XDR *xdrs, post_op_attr *objp)
{
    if (!xdr_bool (xdrs, &objp->attributes_follow))
        return FALSE;
    switch (objp->attributes_follow)
    {
        case TRUE:
            if (!xdr_fattr3 (xdrs, &objp->post_op_attr_u.attributes))
                return FALSE;
            break;
        case FALSE:
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

bool_t
xdr_wcc_attr (XDR *xdrs, wcc_attr *objp)
{
    if (!xdr_size3 (xdrs, &objp->size))
        return FALSE;
    if (!xdr_nfstime3 (xdrs, &objp->mtime))
        return FALSE;
    if (!xdr_nfstime3 (xdrs, &objp->ctime))
        return FALSE;
    return TRUE;
}

bool_t
xdr_pre_op_attr (XDR *xdrs, pre_op_attr *objp)
{
    if (!xdr_bool (xdrs, &objp->attributes_follow))
        return FALSE;
    switch (objp->attributes_follow)
    {
        case TRUE:
            if (!xdr_wcc_attr (xdrs, &objp->pre_op_attr_u.attributes))
            return FALSE;
            break;
        case FALSE:
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

bool_t
xdr_wcc_data (XDR *xdrs, wcc_data *objp)
{
    if (!xdr_pre_op_attr (xdrs, &objp->before))
        return FALSE;
    if (!xdr_post_op_attr (xdrs, &objp->after))
        return FALSE;
    return TRUE;
}

bool_t
xdr_post_op_fh3 (XDR *xdrs, post_op_fh3 *objp)
{
    if (!xdr_bool (xdrs, &objp->handle_follows))
        return FALSE;
    switch (objp->handle_follows)
    {
        case TRUE:
            if (!xdr_nfs_fh3 (xdrs, &objp->post_op_fh3_u.handle))
            return FALSE;
            break;
        case FALSE:
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

bool_t
xdr_time_how (XDR *xdrs, time_how *objp)
{
    if (!xdr_enum (xdrs, (enum_t *) objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_set_mode3 (XDR *xdrs, set_mode3 *objp)
{
    if (!xdr_bool (xdrs, &objp->set_it))
        return FALSE;
    switch (objp->set_it)
    {
        case TRUE:
            if (!xdr_mode3 (xdrs, &objp->set_mode3_u.mode))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_set_uid3 (XDR *xdrs, set_uid3 *objp)
{
    if (!xdr_bool (xdrs, &objp->set_it))
        return FALSE;
    switch (objp->set_it)
    {
        case TRUE:
            if (!xdr_uid3 (xdrs, &objp->set_uid3_u.uid))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_set_gid3 (XDR *xdrs, set_gid3 *objp)
{
    if (!xdr_bool (xdrs, &objp->set_it))
    return FALSE;
    switch (objp->set_it)
    {
        case TRUE:
            if (!xdr_gid3 (xdrs, &objp->set_gid3_u.gid))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_set_size3 (XDR *xdrs, set_size3 *objp)
{
    if (!xdr_bool (xdrs, &objp->set_it))
        return FALSE;
    switch (objp->set_it)
    {
        case TRUE:
        if (!xdr_size3 (xdrs, &objp->set_size3_u.size))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_set_atime (XDR *xdrs, set_atime *objp)
{
    if (!xdr_time_how (xdrs, &objp->set_it))
        return FALSE;
    switch (objp->set_it)
    {
        case SET_TO_CLIENT_TIME:
            if (!xdr_nfstime3 (xdrs, &objp->set_atime_u.atime))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_set_mtime (XDR *xdrs, set_mtime *objp)
{
    if (!xdr_time_how (xdrs, &objp->set_it))
        return FALSE;
    switch (objp->set_it)
    {
        case SET_TO_CLIENT_TIME:
            if (!xdr_nfstime3 (xdrs, &objp->set_mtime_u.mtime))
            return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_sattr3 (XDR *xdrs, sattr3 *objp)
{
    if (!xdr_set_mode3 (xdrs, &objp->mode))
        return FALSE;
    if (!xdr_set_uid3 (xdrs, &objp->uid))
        return FALSE;
    if (!xdr_set_gid3 (xdrs, &objp->gid))
        return FALSE;
    if (!xdr_set_size3 (xdrs, &objp->size))
        return FALSE;
    if (!xdr_set_atime (xdrs, &objp->atime))
        return FALSE;
    if (!xdr_set_mtime (xdrs, &objp->mtime))
        return FALSE;
    return TRUE;
}

bool_t
xdr_diropargs3 (XDR *xdrs, diropargs3 *objp)
{
    if (!xdr_nfs_fh3 (xdrs, &objp->dir))
        return FALSE;
    if (!xdr_filename3 (xdrs, &objp->name))
        return FALSE;
    return TRUE;
}

bool_t
xdr_GETATTR3args (XDR *xdrs, GETATTR3args *objp)
{
    if (!xdr_nfs_fh3 (xdrs, &objp->object))
        return FALSE;
    return TRUE;
}

bool_t
xdr_GETATTR3resok (XDR *xdrs, GETATTR3resok *objp)
{
    if (!xdr_fattr3 (xdrs, &objp->obj_attributes))
        return FALSE;
    return TRUE;
}

bool_t
xdr_GETATTR3res (XDR *xdrs, GETATTR3res *objp)
{
    if (!xdr_nfsstat3 (xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
    case NFS3_OK:
        if (!xdr_GETATTR3resok (xdrs, &objp->GETATTR3res_u.resok))
            return FALSE;
        break;
    default:
        break;
    }
    return TRUE;
}


bool_t
xdr_LOOKUP3args (XDR *xdrs, LOOKUP3args *objp)
{
    if (!xdr_diropargs3 (xdrs, &objp->what))
        return FALSE;
    return TRUE;
}

bool_t
xdr_LOOKUP3resok (XDR *xdrs, LOOKUP3resok *objp)
{
    if (!xdr_nfs_fh3 (xdrs, &objp->object))
        return FALSE;
    if (!xdr_post_op_attr (xdrs, &objp->obj_attributes))
        return FALSE;
    if (!xdr_post_op_attr (xdrs, &objp->dir_attributes))
        return FALSE;
    return TRUE;
}

bool_t
xdr_LOOKUP3resfail (XDR *xdrs, LOOKUP3resfail *objp)
{
    if (!xdr_post_op_attr (xdrs, &objp->dir_attributes))
        return FALSE;
    return TRUE;
}

bool_t
xdr_LOOKUP3res (XDR *xdrs, LOOKUP3res *objp)
{
    if (!xdr_nfsstat3 (xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
    case NFS3_OK:
        if (!xdr_LOOKUP3resok (xdrs, &objp->LOOKUP3res_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_LOOKUP3resfail (xdrs, &objp->LOOKUP3res_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}

bool_t
xdr_createmode3 (XDR *xdrs, createmode3 *objp)
{
    if (!xdr_enum (xdrs, (enum_t *) objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_createhow3 (XDR *xdrs, createhow3 *objp)
{
    if (!xdr_createmode3 (xdrs, &objp->mode))
        return FALSE;
    switch (objp->mode) {
    case UNCHECKED:
    case GUARDED:
        if (!xdr_sattr3 (xdrs, &objp->createhow3_u.obj_attributes))
            return FALSE;
        break;
    case EXCLUSIVE:
        if (!xdr_createverf3 (xdrs, objp->createhow3_u.verf))
            return FALSE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

bool_t
xdr_CREATE3args (XDR *xdrs, CREATE3args *objp)
{
    if (!xdr_diropargs3 (xdrs, &objp->where))
        return FALSE;
    if (!xdr_createhow3 (xdrs, &objp->how))
        return FALSE;
    return TRUE;
}

bool_t
xdr_CREATE3resok (XDR *xdrs, CREATE3resok *objp)
{
    if (!xdr_post_op_fh3 (xdrs, &objp->obj))
        return FALSE;
    if (!xdr_post_op_attr (xdrs, &objp->obj_attributes))
        return FALSE;
    if (!xdr_wcc_data (xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool_t
xdr_CREATE3resfail (XDR *xdrs, CREATE3resfail *objp)
{
    if (!xdr_wcc_data (xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool_t
xdr_CREATE3res (XDR *xdrs, CREATE3res *objp)
{
    if (!xdr_nfsstat3 (xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
    case NFS3_OK:
        if (!xdr_CREATE3resok (xdrs, &objp->CREATE3res_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_CREATE3resfail (xdrs, &objp->CREATE3res_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}


bool_t
xdr_REMOVE3args (XDR *xdrs, REMOVE3args *objp)
{
    if (!xdr_diropargs3 (xdrs, &objp->object))
        return FALSE;
    return TRUE;
}

bool_t
xdr_REMOVE3resok (XDR *xdrs, REMOVE3resok *objp)
{
    if (!xdr_wcc_data (xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool_t
xdr_REMOVE3resfail (XDR *xdrs, REMOVE3resfail *objp)
{
    if (!xdr_wcc_data (xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool_t
xdr_REMOVE3res (XDR *xdrs, REMOVE3res *objp)
{
    if (!xdr_nfsstat3 (xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
    case NFS3_OK:
        if (!xdr_REMOVE3resok (xdrs, &objp->REMOVE3res_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_REMOVE3resfail (xdrs, &objp->REMOVE3res_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}
