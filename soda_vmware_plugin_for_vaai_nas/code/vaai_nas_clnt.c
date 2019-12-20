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
#include "vaai_nas.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 60, 0 };


int nfsproc3_extendvaai_3(EXTENDCMDargs *argp, CLIENT *clnt, EXTENDCMDres *clnt_res)
{
    return clnt_call(clnt, NFSPROC3_EXTENDVAAI,
                    (xdrproc_t) xdr_EXTENDCMDargs, (caddr_t) argp,
                    (xdrproc_t) xdr_EXTENDCMDres, (caddr_t) clnt_res, TIMEOUT);
}

