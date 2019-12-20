// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.storage.vro.plugin.model;

public class RestContains {
    /**
     * Range max size
     */
    public static final int RANGE_MAX = 50;

    /**
     * LUN group apptype
     */
    public static final String APPTYPE_LUNGROUP = "0";

    /**
     * Product mode
     */
    public static final String PRODUCTMODE = "PRODUCTMODE";

    /**
     * Product mode string
     */
    public static final String PRODUCTMODESTRING = "productModeString";

    /**
     * Product version
     */
    public static final String PRODUCTVERSION = "PRODUCTVERSION";

    /**
     * Filter param name
     */
    public static final String FILTER_NAME_PARAMETER = "NAME::";

    /**
     * Fuzzy query filter param name
     */
    public static final String FILTER_NAME_PARAMETER_SPECIAL = "NAME:";

    /**
     * Filter param share path
     */
    public static final String FILTER_SHAREPATH_PARAMETER = "SHAREPATH::";

    /**
     * name
     */
    public static final String NAME = "NAME";

    /**
     * id
     */
    public static final String ID = "ID";

    /**
     * Point release
     */
    public static final String POINTRELEASE = "pointRelease";

    /**
     * description
     */
    public static final String DESCRIPTION = "DESCRIPTION";

    /**
     * Parent id
     */
    public static final String PARENTID = "PARENTID";

    /**
     * Host type enum value
     */
    public static final String PARENTTYPE_HOST = "21";

    /**
     * Filesystem type enum value
     */
    public static final String PARENTTYPE_FILESYSTEM = "40";

    /**
     * Parent type
     */
    public static final String PARENTTYPE = "PARENTTYPE";

    /**
     * capacity
     */
    public static final String CAPACITY = "CAPACITY";

    /**
     * Filter key
     */
    public static final String FILTER_KEY = "filter";

    /**
     * sortby
     */
    public static final String SORTBY_KEY = "sortby";

    /**
     * Vstore id
     */
    public static final String VSORTBY_ID = "vstoreId";

    /**
     * Sortby time
     */
    public static final String SORTBY_TIME = "TIMESTAMP,d";

    /**
     * range
     */
    public static final String RANGE = "range";

    /**
     * Default range
     */
    public static final String RANGE_DEFAULT = "[0-100]";

    /**
     * count
     */
    public static final String COUNT = "COUNT";

    /**
     * time
     */
    public static final String TIME = "t";

    /**
     * apptype
     */
    public static final String APPTYPE = "APPTYPE";

    /**
     * Associate object type
     */
    public static final String ASSOCIATEOBJTYPE = "ASSOCIATEOBJTYPE";

    /**
     * Associate object id
     */
    public static final String ASSOCIATEOBJID = "ASSOCIATEOBJID";

    /**
     * LUN enum value
     */
    public static final String ASSOCIATEOBJTYPE_LUN = "11";

    /**
     * ETH port enum value
     */
    public static final String ASSOCIATEOBJTYPE_ETH_PORT = "213";

    /**
     * Host Enum value
     */
    public static final String ASSOCIATEOBJTYPE_HOST = "21";

    /**
     * Initiator online enum value
     */
    public static final String INITIATOR_ONLIUNE = "27";

    /**
     * Normal enum value
     */
    public static final String NORMAL = "1";

    /**
     * Splited enum value
     */
    public static final String SPLITED = "26";

    /**
     * Link up enum value
     */
    public static final String LINK_UP = "10";

    /**
     * Snapshot Enum value
     */
    public static final String ASSOCIATEOBJTYPE_SANPSHOT = "27";

    /**
     * LUN group enum value
     */
    public static final String ASSOCIATEOBJTYPE_LUN_GROUP = "256";

    /**
     * Mappingview enum value
     */
    public static final String ASSOCIATEOBJTYPE_MAPPING_VIEW = "245";

    /**
     * Host group enum value
     */
    public static final String ASSOCIATEOBJTYPE_HOST_GROUP = "14";

    /**
     * Operation system
     */
    public static final String OS = "OPERATIONSYSTEM";

    /**
     * ip
     */
    public static final String IP = "IP";

    /**
     * location
     */
    public static final String LOCATION = "LOCATION";

    /**
     * Mappingview prefix
     */
    public static final String MAPPING_VIEW_PREFIX = "mapping_view_";

    /**
     * Mappingview description
     */
    public static final String MAPPING_VIEW_DESCRIPTION = "Created By vRO workflow";

    /**
     * alloctype
     */
    public static final String ALLOCTYPE = "ALLOCTYPE";

    /**
     * sector size
     */
    public static final String SECTORSIZE = "SECTORSIZE";

    /**
     * GB to SECTOR
     */
    public static final int GB_TO_SECTOR = 1024 * 1024 * 1024 / 512;

    /**
     * Share path
     */
    public static final String SHAREPATH = "SHAREPATH";

    /**
     * Character encoding
     */
    public static final String CHARACTERENCODING = "CHARACTERENCODING";

    /**
     * Accessval
     */
    public static final String ACCESSVAL = "ACCESSVAL";

    /**
     * synchronize
     */
    public static final String SYNC = "SYNC";

    /**
     * all squash
     */
    public static final String ALLSQUASH = "ALLSQUASH";

    /**
     * root squash
     */
    public static final String ROOTSQUASH = "ROOTSQUASH";

    /**
     * Synchronize default
     */
    public static final String SYNC_DEFAULT = "0";

    /**
     * All squash default
     */
    public static final String ALLSQUASH_DEFAULT = "1";

    /**
     * Root squash default
     */
    public static final String ROOTSQUASH_DEFAULT = "0";

    /**
     * Enable oplock
     */
    public static final String ENABLEOPLOCK = "ENABLEOPLOCK";

    /**
     * Enable enotify
     */
    public static final String ENABLENOTIFY = "ENABLENOTIFY";

    /**
     * enable CA
     */
    public static final String ENABLECA = "ENABLECA";

    /**
     * Offline file mode
     */
    public static final String OFFLINEFILEMODE = "OFFLINEFILEMODE";

    /**
     * Enable ip control
     */
    public static final String ENABLEIPCONTROL = "ENABLEIPCONTROL";

    /**
     * permission
     */
    public static final String PERMISSION = "PERMISSION";

    /**
     * Domain type
     */
    public static final String DOMAINTYPE = "DOMAINTYPE";

    /**
     * Access name
     */
    public static final String ACCESSNAME = "ACCESSNAME";

    /**
     * Upband width
     */
    public static final String UPBANDWIDTH = "UPBANDWIDTH";

    /**
     * Downband width
     */
    public static final String DOWNBANDWIDTH = "DOWNBANDWIDTH";

    /**
     * subtype
     */
    public static final String SUBTYPE = "SUBTYPE";

    /**
     * Snapshot list
     */
    public static final String SNAPSHOTLIST = "SNAPSHOTLIST";

    /**
     * Rollback speed
     */
    public static final String ROLLBACKSPEED = "ROLLBACKSPEED";

    /**
     * Copy speed
     */
    public static final String COPYSPEED = "COPYSPEED";

    /**
     * Health status
     */
    public static final String HEALTHSTATUS = "HEALTHSTATUS";

    /**
     * Running status
     */
    public static final String RUNNINGSTATUS = "RUNNINGSTATUS";

    /**
     * Copy progress
     */
    public static final String COPYPROGRESS = "COPYPROGRESS";

    /**
     * Source LUN
     */
    public static final String SOURCELUN = "SOURCELUN";

    /**
     * Target LUN
     */
    public static final String TARGETLUN = "TARGETLUN";

    /**
     * LUN copy id
     */
    public static final String LUNCOPYID = "LUNCOPYID";

    /**
     * Use chap
     */
    public static final String USECHAP = "USECHAP";

    /**
     * Chap name
     */
    public static final String CHAPNAME = "CHAPNAME";

    /**
     * Chap pad
     */
    public static final String CHAPPAD = "CHAPPASSWORD";

    /**
     * Chap OLD PAD
     */
    public static final String CHAPOLDPAD = "CHAPOLDPASSWORD";

    /**
     * Chap OLD PAD initiatorver target
     */
    public static final String CHAPOLDPADINITATORVERTARGET = "CHAPOLDPASSWORDINITATORVERTARGET";

    /**
     * Chap name initiatorver target
     */
    public static final String CHAPNAMEINITATORVERTARGET = "CHAPNAMEINITATORVERTARGET";

    /**
     * Chap PAD initiatorver target
     */
    public static final String CHAPPADINITATORVERTARGET = "CHAPPASSWORDINITATORVERTARGET";

    /**
     * Doscoveryver mode
     */
    public static final String DISCOVERYVERMODE = "DISCOVERYVERMODE";

    /**
     * Normalver mode
     */
    public static final String NORMALVERMODE = "NORMALVERMODE";

    /**
     * Failover mode
     */
    public static final String FAILOVERMODE = "FAILOVERMODE";

    /**
     * Path type
     */
    public static final String PATHTYPE = "PATHTYPE";

    /**
     * multipathtype
     */
    public static final String MULTIPATHTYPE = "MULTIPATHTYPE";

    /**
     * Owning controller
     */
    public static final String OWNINGCONTROLLER = "OWNINGCONTROLLER";

    /**
     * Read cache policy
     */
    public static final String READCACHEPOLICY = "READCACHEPOLICY";

    /**
     * Write cache policy
     */
    public static final String WRITECACHEPOLICY = "WRITECACHEPOLICY";

    /**
     * Prefectch policy
     */
    public static final String PREFETCHPOLICY = "PREFETCHPOLICY";

    /**
     * Prefectch value
     */
    public static final String PREFETCHVALUE = "PREFETCHVALUE";

    /**
     * Data transfer policy
     */
    public static final String DATATRANSFERPOLICY = "DATATRANSFERPOLICY";

    /**
     * I/O proority
     */
    public static final String IOPRIORITY = "IOPRIORITY";

    /**
     * Local resource id
     */
    public static final String LOCALRESID = "LOCALRESID";

    /**
     * Local resource type
     */
    public static final String LOCALRESTYPE = "LOCALRESTYPE";

    /**
     * Remote device id
     */
    public static final String REMOTEDEVICEID = "REMOTEDEVICEID";

    /**
     * Remote resource id
     */
    public static final String REMOTERESID = "REMOTERESID";

    /**
     * Synchronize type
     */
    public static final String SYNCHRONIZETYPE = "SYNCHRONIZETYPE";

    /**
     * Recovery policy
     */
    public static final String RECOVERYPOLICY = "RECOVERYPOLICY";

    /**
     * speed
     */
    public static final String SPEED = "SPEED";

    /**
     * Timing value
     */
    public static final String TIMINGVAL = "TIMINGVAL";

    /**
     * Replication model
     */
    public static final String REPLICATIONMODEL = "REPLICATIONMODEL";

    /**
     * isDataSync
     */
    public static final String ISDATASYNC = "ISDATASYNC";

    /**
     * RM list
     */
    public static final String RMLIST = "RMLIST";

    /**
     * Remtimeoutperiod
     */
    public static final String REMTIMEOUTPERIOD = "REMTIMEOUTPERIOD";

    /**
     * Synchronize schedule
     */
    public static final String SYNCHRONIZESCHEDULE = "SYNCHRONIZESCHEDULE";

    /**
     * isLocalDelete
     */
    public static final String ISLOCALDELETE = "ISLOCALDELETE";

    /**
     * toSyncsrWhenDelete
     */
    public static final String TOSYNCSRWHENDELETE = "TOSYNCSRWHENDELETE";

    /**
     * enable compress
     */
    public static final String ENABLECOMPRESS = "ENABLECOMPRESS";

    /**
     * Timing value insec
     */
    public static final String TIMINGVALINSEC = "TIMINGVALINSEC";

}
