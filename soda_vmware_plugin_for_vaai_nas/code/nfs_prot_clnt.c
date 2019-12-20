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

#include <memory.h> /* for memset */
#include "nfs_prot.h"
#ifndef lint

static const char rcsid[] =
 "$FreeBSD: src/include/rpcsvc/nfs_prot.x,v 1.7 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int nfsproc3_getattr_3(GETATTR3args *argp, CLIENT *clnt, GETATTR3res *clnt_res)
{
    return clnt_call(clnt, NFSPROC3_GETATTR,
                    (xdrproc_t) xdr_GETATTR3args, (caddr_t) argp,
                    (xdrproc_t) xdr_GETATTR3res, (caddr_t) clnt_res, TIMEOUT);
}



int nfsproc3_lookup_3(LOOKUP3args *argp, CLIENT *clnt, LOOKUP3res *clnt_res)
{
    return clnt_call(clnt, NFSPROC3_LOOKUP,
                    (xdrproc_t) xdr_LOOKUP3args, (caddr_t) argp,
                    (xdrproc_t) xdr_LOOKUP3res, (caddr_t) clnt_res, TIMEOUT);
}

int nfsproc3_create_3(CREATE3args *argp, CLIENT *clnt, CREATE3res * clnt_res)
{
    return clnt_call(clnt, NFSPROC3_CREATE,
                    (xdrproc_t) xdr_CREATE3args, (caddr_t) argp,
                    (xdrproc_t) xdr_CREATE3res, (caddr_t) clnt_res, TIMEOUT);
}

int nfsproc3_remove_3(REMOVE3args *argp, CLIENT *clnt, REMOVE3res *clnt_res)
{
    return clnt_call(clnt, NFSPROC3_REMOVE,
                    (xdrproc_t) xdr_REMOVE3args, (caddr_t) argp,
                    (xdrproc_t) xdr_REMOVE3res, (caddr_t) clnt_res, TIMEOUT);
}

