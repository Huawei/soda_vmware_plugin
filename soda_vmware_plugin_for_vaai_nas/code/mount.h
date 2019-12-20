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

#ifndef _MOUNT_H_RPCGEN
#define _MOUNT_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define MNTPATHLEN 1024
#define MNTNAMLEN 255
#define FHSIZE 32
#define FHSIZE3 64

typedef char fhandle[FHSIZE];

typedef struct {
    u_int fhandle3_len;
    char *fhandle3_val;
} fhandle3;

struct fhstatus {
    u_int fhs_status;
    union {
        fhandle fhs_fhandle;
    } fhstatus_u;
};
typedef struct fhstatus fhstatus;

enum mountstat3 {
    MNT3_OK = 0,
    MNT3ERR_PERM = 1,
    MNT3ERR_NOENT = 2,
    MNT3ERR_IO = 5,
    MNT3ERR_ACCES = 13,
    MNT3ERR_NOTDIR = 20,
    MNT3ERR_INVAL = 22,
    MNT3ERR_NAMETOOLONG = 63,
    MNT3ERR_NOTSUPP = 10004,
    MNT3ERR_SERVERFAULT = 10006,
};
typedef enum mountstat3 mountstat3;

struct mountres3_ok {
    fhandle3 fhandle;
    struct {
        u_int auth_flavors_len;
        int *auth_flavors_val;
    } auth_flavors;
};
typedef struct mountres3_ok mountres3_ok;

struct mountres3 {
    mountstat3 fhs_status;
    union {
        mountres3_ok mountinfo;
    } mountres3_u;
};
typedef struct mountres3 mountres3;

typedef char *dirpath;

typedef char *name;

typedef struct mountbody *mountlist;

struct mountbody {
    name ml_hostname;
    dirpath ml_directory;
    mountlist ml_next;
};
typedef struct mountbody mountbody;

typedef struct groupnode *groups;

struct groupnode {
    name gr_name;
    groups gr_next;
};
typedef struct groupnode groupnode;

typedef struct exportnode *exports;

struct exportnode {
    dirpath ex_dir;
    groups ex_groups;
    exports ex_next;
};
typedef struct exportnode exportnode;

#define MOUNTPROG 100005
#define MOUNTVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define MOUNTPROC_NULL 0
#define MOUNTPROC_MNT 1
#define MOUNTPROC_DUMP 2
#define MOUNTPROC_UMNT 3
#define MOUNTPROC_UMNTALL 4
#define MOUNTPROC_EXPORT 5
#define MOUNTPROC_EXPORTALL 6

#else /* K&R C */
#define MOUNTPROC_NULL 0
#define MOUNTPROC_MNT 1
#define MOUNTPROC_DUMP 2
#define MOUNTPROC_UMNT 3
#define MOUNTPROC_UMNTALL 4
#define MOUNTPROC_EXPORT 5
#define MOUNTPROC_EXPORTALL 6
#endif /* K&R C */
#define MOUNTVERS3 3

#if defined(__STDC__) || defined(__cplusplus)
extern  int mountproc_mnt_3(dirpath *, CLIENT *, mountres3 *);
#else /* K&R C */
extern  mountres3 * mountproc_mnt_3();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_fhandle3 (XDR *, fhandle3*);
extern  bool_t xdr_mountstat3 (XDR *, mountstat3*);
extern  bool_t xdr_mountres3_ok (XDR *, mountres3_ok*);
extern  bool_t xdr_mountres3 (XDR *, mountres3*);
extern  bool_t xdr_dirpath (XDR *, dirpath*);

#else /* K&R C */
extern bool_t xdr_fhandle3 ();
extern bool_t xdr_mountstat3 ();
extern bool_t xdr_mountres3_ok ();
extern bool_t xdr_mountres3 ();
extern bool_t xdr_dirpath ();
#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_MOUNT_H_RPCGEN */

