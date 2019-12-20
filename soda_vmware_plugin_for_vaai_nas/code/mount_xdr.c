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

#include "mount.h"
#ifndef lint
static const char rcsid[] =
 "$FreeBSD: src/include/rpcsvc/mount.x,v 1.6 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

bool_t
xdr_fhandle3 (XDR *xdrs, fhandle3 *objp)
{
    if (!xdr_bytes (xdrs, (char **)&objp->fhandle3_val, (u_int *)&objp->fhandle3_len, FHSIZE3))
        return FALSE;
    return TRUE;
}

bool_t
xdr_mountstat3 (XDR *xdrs, mountstat3 *objp)
{
    if (!xdr_enum (xdrs, (enum_t *)objp))
        return FALSE;
    return TRUE;
}

bool_t
xdr_mountres3_ok (XDR *xdrs, mountres3_ok *objp)
{
    if (!xdr_fhandle3 (xdrs, &objp->fhandle))
        return FALSE;
    if (!xdr_array (xdrs, (char **)&objp->auth_flavors.auth_flavors_val, (u_int *) &objp->auth_flavors.auth_flavors_len, ~0,
        sizeof(int), (xdrproc_t)xdr_int))
        return FALSE;
    return TRUE;
}

bool_t
xdr_mountres3 (XDR *xdrs, mountres3 *objp)
{
    if (!xdr_mountstat3 (xdrs, &objp->fhs_status))
        return FALSE;
    switch (objp->fhs_status)
    {
        case 0:
            if (!xdr_mountres3_ok (xdrs, &objp->mountres3_u.mountinfo))
                return FALSE;
            break;
        default:
            break;
    }
    return TRUE;
}

bool_t
xdr_dirpath (XDR *xdrs, dirpath *objp)
{
    if (!xdr_string (xdrs, objp, MNTPATHLEN))
        return FALSE;
    return TRUE;
}

