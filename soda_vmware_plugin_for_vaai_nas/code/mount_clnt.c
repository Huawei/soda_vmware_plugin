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
#include "mount.h"
#ifndef lint

static const char rcsid[] =
 "$FreeBSD: src/include/rpcsvc/mount.x,v 1.6 1999/08/27 23:45:08 peter Exp $";
#endif /* not lint */

static struct timeval TIMEOUT = { 60, 0 };

int mountproc_mnt_3(dirpath *argp, CLIENT *clnt, mountres3 *clnt_res)
{

    return clnt_call (clnt, MOUNTPROC_MNT,
        (xdrproc_t) xdr_dirpath, (caddr_t) argp,
        (xdrproc_t) xdr_mountres3, (caddr_t) clnt_res, TIMEOUT);
}

