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

#include "rpcb_prot.h"
#include "securec.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

int rpcbproc_getaddr_3(rpcb *argp, CLIENT *clnt, char **clnt_res)
{
    return clnt_call (clnt, RPCBPROC_GETADDR,
		(xdrproc_t) xdr_rpcb, (caddr_t) argp,
		(xdrproc_t) xdr_wrapstring, (caddr_t)clnt_res,
		TIMEOUT);
}


