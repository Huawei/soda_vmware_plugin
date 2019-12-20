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

#ifndef _RPCB_PROT_H_RPCGEN
#define _RPCB_PROT_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define RPCB_PORT 111

struct rpcb {
	u_long r_prog;
	u_long r_vers;
	char *r_netid;
	char *r_addr;
	char *r_owner;
};
typedef struct rpcb rpcb;

#define RPCBPROG 100000
#define RPCBVERS 3

#if defined(__STDC__) || defined(__cplusplus)
#define RPCBPROC_GETADDR 3
extern int rpcbproc_getaddr_3(rpcb *argp, CLIENT *clnt, char **clnt_res);
#else /* K&R C */
#define RPCBPROC_GETADDR 3

extern  int rpcbproc_getaddr_3();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_rpcb (XDR *, rpcb*);
#else /* K&R C */
extern bool_t xdr_rpcb ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPCB_PROT_H_RPCGEN */
