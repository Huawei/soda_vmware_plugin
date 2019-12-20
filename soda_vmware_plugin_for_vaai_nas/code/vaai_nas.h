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

#ifndef _VAAI_NAS_H_RPCGEN
#define _VAAI_NAS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef char *vaaifilename3;
#define FHSIZE3 64
#define EXTENDCMDSIZE3 1024
#define VAAISIZE3 1024

struct nfsfh3 {
    struct {
        u_int data_len;
        char *data_val;
    } data;
};
typedef struct nfsfh3 nfsfh3;

struct cmdargdata3 {
    struct {
        u_int data_len;
        char *data_val;
    } data;
};
typedef struct cmdargdata3 cmdargdata3;

enum EXTENDCMDTYPE {
    EXTENDVAAI_NEED_SRC_FH = 0,
    EXTENDVAAI_OTHER = 1,
    EXTENDVAAI_BUTT = 1 + 1,
};
typedef enum EXTENDCMDTYPE EXTENDCMDTYPE;

struct vaai_extend_fh {
    EXTENDCMDTYPE cmdtype;
    union {
        nfsfh3 ffh;
    } vaai_extend_fh_u;
};
typedef struct vaai_extend_fh vaai_extend_fh;

struct EXTENDCMDargs {
    nfsfh3 dis_fh;
    vaai_extend_fh src_fh;
    cmdargdata3 cmdarg;
};
typedef struct EXTENDCMDargs EXTENDCMDargs;

struct cmdresdata3 {
    struct {
        u_int data_len;
        char *data_val;
    } data;
};
typedef struct cmdresdata3 cmdresdata3;

struct extendcmdresult {
    cmdresdata3 cmdres;
};
typedef struct extendcmdresult extendcmdresult;

struct EXTENDCMDres {
    int32_t status;
    union {
        extendcmdresult resok;
        extendcmdresult resfail;
    } EXTENDCMDres_u;
};
typedef struct EXTENDCMDres EXTENDCMDres;

#define NFSPROGRAM 100003
#define NFSV3 3

#if defined(__STDC__) || defined(__cplusplus)
#define NFSPROC3_EXTENDVAAI 22
extern  int nfsproc3_extendvaai_3(EXTENDCMDargs *, CLIENT *, EXTENDCMDres *);
extern  EXTENDCMDres * nfsproc3_extendvaai_3_svc(EXTENDCMDargs *, struct svc_req *);
extern int nfsprogram_3_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define NFSPROC3_EXTENDVAAI 22
extern  EXTENDCMDres * nfsproc3_extendvaai_3();
extern  EXTENDCMDres * nfsproc3_extendvaai_3_svc();
extern int nfsprogram_3_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_vaaifilename3 (XDR *, vaaifilename3*);
extern  bool_t xdr_nfsfh3 (XDR *, nfsfh3*);
extern  bool_t xdr_cmdargdata3 (XDR *, cmdargdata3*);
extern  bool_t xdr_EXTENDCMDTYPE (XDR *, EXTENDCMDTYPE*);
extern  bool_t xdr_vaai_extend_fh (XDR *, vaai_extend_fh*);
extern  bool_t xdr_EXTENDCMDargs (XDR *, EXTENDCMDargs*);
extern  bool_t xdr_cmdresdata3 (XDR *, cmdresdata3*);
extern  bool_t xdr_extendcmdresult (XDR *, extendcmdresult*);
extern  bool_t xdr_EXTENDCMDres (XDR *, EXTENDCMDres*);

#else /* K&R C */
extern bool_t xdr_vaaifilename3 ();
extern bool_t xdr_nfsfh3 ();
extern bool_t xdr_cmdargdata3 ();
extern bool_t xdr_EXTENDCMDTYPE ();
extern bool_t xdr_vaai_extend_fh ();
extern bool_t xdr_EXTENDCMDargs ();
extern bool_t xdr_cmdresdata3 ();
extern bool_t xdr_extendcmdresult ();
extern bool_t xdr_EXTENDCMDres ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_VAAI_NAS_H_RPCGEN */

