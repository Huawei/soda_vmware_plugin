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

import java.util.List;

import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;

public class LunAndHostService {
    private LUNManager lunManager = new LUNManager();

    private StoragePoolManager poolManager = new StoragePoolManager();

    private HostManager hostManager = new HostManager();

    private MappingViewManager mappingViewManager = new MappingViewManager();

    private iSCSIManager iSCSIManager = new iSCSIManager();

    private FCInitiatorManager fcInitiatorManager = new FCInitiatorManager();

    private HostService hostService = new HostService();

    /**
     *
     * @param storagearrayinfo Oceanstore info
     * @param lunname LUN name
     * @param lundescription LUN descreption
     * @param capacity capacity
     * @param storagepoolname Storage pool name
     * @param alloctype  Space allocation type
     * @param owningcontroller Owning controller
     * @param readcachepolicy Read cache policy
     * @param writecachepolicy Write cache policy.
     * @param prefetchpolicy intelligent prefetch
     * @param prefetchvalue Prefetch policy value
     * @param datatransferpolicy Data migration policy
     * @param iopriority I/O priority
     * @param lungroupname LUN group name
     * @param lungroupdescription LUN group description
     * @param hostname Host name
     * @param hostgroupname Host group name
     * @param hostgroupdescription Host group description
     * @param mappingviewname Mappingview name
     * @param mappingviewdescription Mappingview description
     * @return LUN
     * @throws StorageCommonException
     * @throws RestException
     */
    public LUN createLunAndHostMappingView(StorageArrayInfo storagearrayinfo, String lunname, String lundescription,
        int capacity, String storagepoolname, String alloctype, String owningcontroller, String readcachepolicy,
        String writecachepolicy, String prefetchpolicy, String prefetchvalue, String datatransferpolicy,
        String iopriority, String lungroupname, String lungroupdescription, String hostname, String hostgroupname,
        String hostgroupdescription, String mappingviewname, String mappingviewdescription)
        throws StorageCommonException, RestException {
        LUN lun = null;
        LUNGroup lunGroup = null;
        boolean flagCreateLunGroup = false;
        Host host = null;
        HostGroup hostGroup = null;
        boolean flagCreateHostGroup = false;
        MappingView mappingView = null;
        boolean flagCreateMappingview = false;
        try {
            String poolId = poolManager.getPoolIdByName(storagepoolname, storagearrayinfo);
            lunManager.createLUN(storagearrayinfo, lunname, lundescription, capacity, poolId, alloctype,
                    owningcontroller, readcachepolicy, writecachepolicy, prefetchpolicy, prefetchvalue,
                    datatransferpolicy, iopriority);
            lun = lunManager.getLUNByName(storagearrayinfo, lunname);
            try {
                lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lungroupname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateLunGroup = true;
                    lunManager.createLUNGroup(storagearrayinfo, lungroupname, lungroupdescription,
                            RestContains.APPTYPE_LUNGROUP);
                    lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lungroupname);
                }
            }
            host = hostManager.getHostByName(storagearrayinfo, hostname);
            try {
                hostGroup = hostManager.getHostGroupByName(storagearrayinfo, hostgroupname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateHostGroup = true;
                    hostManager.createHostGroup(storagearrayinfo, hostgroupname, hostgroupdescription);
                    hostGroup = hostManager.getHostGroupByName(storagearrayinfo, hostgroupname);
                }
            }
            lunManager.createLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
            hostManager.createHostGroupAssociation(storagearrayinfo, hostGroup.getReal_id(), host.getReal_id());
            try {
                mappingView = mappingViewManager.getMappingViewByName(storagearrayinfo, mappingviewname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateMappingview = true;
                    mappingViewManager.createMappingView(storagearrayinfo, mappingviewname, mappingviewdescription);
                    mappingView = mappingViewManager.getMappingViewByName(storagearrayinfo, mappingviewname);
                }
            }
            mappingViewManager.createMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                    hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
            mappingViewManager.createMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                    lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        } catch (RestException e) {
            if (mappingView != null) {
                mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                        lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
                mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                        hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
                if (flagCreateMappingview) {
                    mappingViewManager.deleteMappingViewById(storagearrayinfo, mappingView.getReal_id());
                }
            }
            if (null != hostGroup && flagCreateHostGroup) {
                hostManager.deleteHostGroupAssociation(storagearrayinfo, hostGroup.getReal_id(), host.getReal_id());
                hostManager.deleteHostGroupById(storagearrayinfo, hostGroup.getReal_id());
            }
            if (null != lunGroup) {
                lunManager.deleteLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
                if (flagCreateLunGroup) {
                    lunManager.deleteLUNGroupById(storagearrayinfo, lunGroup.getReal_id());
                }
            }
            throw e;
        }
        return lun;
    }

    /**
     * Create LUN and host MappingView
     * @param storagearrayinfo Oceanstor info
     * @param lunname LUN name
     * @param lungroupname LUN group name
     * @param lungroupdescription LUN group description
     * @param hostname Host name
     * @param hostgroupname Host group name
     * @param hostgroupdescription Host group description
     * @param mappingviewname Mappingview name
     * @param mappingviewdescription Mappingview description
     * @return LUN
     * @throws StorageCommonException
     * @throws RestException
     */
    public LUN createLunAndHostMappingView(StorageArrayInfo storagearrayinfo, String lunname,
        String lungroupname, String lungroupdescription, String hostname, String hostgroupname,
        String hostgroupdescription, String mappingviewname, String mappingviewdescription)
        throws StorageCommonException, RestException {
        LUN lun = null;
        LUNGroup lunGroup = null;
        boolean flagCreateLunGroup = false;
        Host host = null;
        HostGroup hostGroup = null;
        boolean flagCreateHostGroup = false;
        MappingView mappingView = null;
        boolean flagCreateMappingview = false;
        try {
            lun = lunManager.getLUNByName(storagearrayinfo, lunname);
            try {
                lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lungroupname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateLunGroup = true;
                    lunManager.createLUNGroup(storagearrayinfo, lungroupname, lungroupdescription,
                            RestContains.APPTYPE_LUNGROUP);
                    lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lungroupname);
                }
            }
            host = hostManager.getHostByName(storagearrayinfo, hostname);
            try {
                hostGroup = hostManager.getHostGroupByName(storagearrayinfo, hostgroupname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateHostGroup = true;
                    hostManager.createHostGroup(storagearrayinfo, hostgroupname, hostgroupdescription);
                    hostGroup = hostManager.getHostGroupByName(storagearrayinfo, hostgroupname);
                }
            }
            lunManager.createLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());

            hostManager.createHostGroupAssociation(storagearrayinfo, hostGroup.getReal_id(), host.getReal_id());
            try {
                mappingView = mappingViewManager.getMappingViewByName(storagearrayinfo, mappingviewname);
            } catch (StorageCommonException e) {
                if (e.getMessage().startsWith("can not find")) {
                    flagCreateMappingview = true;
                    mappingViewManager.createMappingView(storagearrayinfo, mappingviewname, mappingviewdescription);
                    mappingView = mappingViewManager.getMappingViewByName(storagearrayinfo, mappingviewname);
                }
            }
            mappingViewManager.createMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                    hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
            mappingViewManager.createMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                    lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        } catch (RestException e) {
            if (mappingView != null) {
                mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                        lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
                mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                        hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
                if (flagCreateMappingview) {
                    mappingViewManager.deleteMappingViewById(storagearrayinfo, mappingView.getReal_id());
                }
            }
            if (null != hostGroup && flagCreateHostGroup) {
                hostManager.deleteHostGroupAssociation(storagearrayinfo, hostGroup.getReal_id(), host.getReal_id());
                hostManager.deleteHostGroupById(storagearrayinfo, hostGroup.getReal_id());
            }

            if (null != lunGroup) {
                lunManager.deleteLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
                if (flagCreateLunGroup) {
                    lunManager.deleteLUNGroupById(storagearrayinfo, lunGroup.getReal_id());
                }
            }
            throw e;
        }
        return lun;
    }

    /**
     * Delete LUN and host mappingview
     * @param storagearrayinfo Oceanstor info
     * @param lunname LUN name
     * @param lungroupname LUN group name
     * @param hostname Host name
     * @param hostgroupname Host group name
     * @param mappingviewname Mappingview name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLunAndHostMappingView(StorageArrayInfo storagearrayinfo, String lunname, String lungroupname,
        String hostname, String hostgroupname, String mappingviewname) throws RestException, StorageCommonException {
        LUN lun = null;
        LUNGroup lunGroup = null;
        Host host = null;
        HostGroup hostGroup = null;
        lun = lunManager.getLUNByName(storagearrayinfo, lunname);
        lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lungroupname);
        host = hostManager.getHostByName(storagearrayinfo, hostname);
        hostGroup = hostManager.getHostGroupByName(storagearrayinfo, hostgroupname);
        MappingView mappingView = null;
        mappingView = mappingViewManager.getMappingViewByName(storagearrayinfo, mappingviewname);

        mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(), lunGroup.getReal_id(),
                RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        mappingViewManager.deleteMappingViewAssociate(storagearrayinfo, mappingView.getReal_id(),
                hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
        mappingViewManager.deleteMappingViewById(storagearrayinfo, mappingView.getReal_id());
        hostManager.deleteHostGroupAssociation(storagearrayinfo, hostGroup.getReal_id(), host.getReal_id());
        hostManager.deleteHostGroupById(storagearrayinfo, hostGroup.getReal_id());
        lunManager.deleteLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
        lunManager.deleteLUNGroupById(storagearrayinfo, lunGroup.getReal_id());
        lunManager.deleteLUNById(storagearrayinfo, lun.getReal_id());
    }

    /**
     * delete LUN
     * @param storagearrayinfo OceanStor info
     * @param lunname LUN name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLun(StorageArrayInfo storagearrayinfo, String lunname)
        throws RestException, StorageCommonException {
        LUN lun = null;
        lun = lunManager.getLUNByName(storagearrayinfo, lunname);
        List<LUNGroup> lunGroups = lunManager.getLUNGroupByLunId(storagearrayinfo, lun.getReal_id());
        for (LUNGroup lunGroup : lunGroups) {
            lunManager.deleteLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
        }
        lunManager.deleteLUNById(storagearrayinfo, lun.getReal_id());
    }

    /**
     * Delete LUN by wwn
     * @param storagearrayinfo Oceanstor info
     * @param wwn wwn
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLunBywwn(StorageArrayInfo storagearrayinfo, String wwn)
        throws RestException, StorageCommonException {
        LUN lunByWwn = lunManager.getLUNByWwn(storagearrayinfo, wwn);
        List<LUNGroup> lunGroups = lunManager.getLUNGroupByLunId(storagearrayinfo, lunByWwn.getReal_id());
        for (LUNGroup lunGroup : lunGroups) {
            List<MappingView> mappingViewByLUNGroup = mappingViewManager.getMappingViewByLUNGroup(storagearrayinfo,
            lunGroup.getReal_id());
            if (mappingViewByLUNGroup.size() == 1) {
                List<HostGroup> hostGroupByMappingView = hostManager.getHostGroupByMappingView(storagearrayinfo,
                    mappingViewByLUNGroup.get(0).getReal_id());
                if (hostGroupByMappingView.size() == 1) {
                    List<Host> hostByHostGroup = hostManager.getHostByHostGroup(storagearrayinfo,
                        hostGroupByMappingView.get(0).getReal_id());
                    if (hostByHostGroup.size() == 1) {
                        mappingViewManager.deleteMappingViewAssociate(storagearrayinfo,
                            mappingViewByLUNGroup.get(0).getReal_id(),
                                hostGroupByMappingView.get(0).getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
                        hostManager.deleteHostGroupAssociation(storagearrayinfo,
                            hostGroupByMappingView.get(0).getReal_id(), hostByHostGroup.get(0).getReal_id());
                        hostManager.deleteHostGroupById(storagearrayinfo, hostGroupByMappingView.get(0).getReal_id());
                    }
                }
                List<LUN> luNsByLungroup = lunManager.getLUNsByLungroup(storagearrayinfo, lunGroup.getReal_id());
                if (luNsByLungroup.size() == 1) {
                    mappingViewManager.deleteMappingViewAssociate(storagearrayinfo,
                        mappingViewByLUNGroup.get(0).getReal_id(),
                        lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
                    lunManager.deleteLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(),
                        lunByWwn.getReal_id());
                    lunManager.deleteLUNGroupById(storagearrayinfo, lunGroup.getReal_id());
                    lunManager.deleteLUNById(storagearrayinfo, lunByWwn.getReal_id());
                }
            }
        }
    }

    /**
     * Create LUN and host mappingview by initiator
     * @param storagearrayinfo Oceanstor info
     * @param lunname LUN name
     * @param capacity LUN capacity
     * @param storagepoolname Storage pool name
     * @param alloctype  Space allocation type
     * @param owningcontroller Owning controller
     * @param readcachepolicy Read cache policy
     * @param writecachepolicy Write cache policy.
     * @param prefetchpolicy intelligent prefetch
     * @param prefetchvalue Prefetch policy value
     * @param datatransferpolicy Data migration policy
     * @param iopriority I/O priority
     * @param hostIqns Host iqns
     * @param hostWwns Host wwns
     * @return MappingInfo
     * @throws StorageCommonException
     * @throws RestException
     */
    public MappingInfo createLunAndHostMappingViewByESX(StorageArrayInfo storagearrayinfo, String lunname,
        int capacity, String storagepoolname, String alloctype, String owningcontroller, String readcachepolicy,
        String writecachepolicy, String prefetchpolicy, String prefetchvalue, String datatransferpolicy,
        String iopriority, String[] hostIqns, String[] hostWwns) throws StorageCommonException, RestException {
        MappingInfo info = null;
        LUN lun = null;
        String hostName = null;
        String lunGroupName = null;
        String hostGroupName = null;
        String mappingViewName = null;
        String hostId = null;
        if (null != hostIqns) {
            for (String iqn : hostIqns) {
                iSCSIInitiator iscsi = iSCSIManager.getISCSIInitiatorById(storagearrayinfo, iqn);
                if (null != iscsi && RestContains.INITIATOR_ONLIUNE.equals(iscsi.getRunningstatus()) &&
                        null != iscsi.getParentid()) {
                    hostId = iscsi.getParentid();
                    hostName = iscsi.getParentname();
                    info = new MappingInfo();
                    info.setId(iqn);
                    break;
                }
            }
        }
        if (null != hostWwns) {
            for (String wwn : hostWwns) {
                FCInitiator fc = fcInitiatorManager.getFCInitiatorById(storagearrayinfo, wwn);
                if (null != fc && RestContains.INITIATOR_ONLIUNE.equals(fc.getRunningstatus()) &&
                        null != fc.getParentid()) {
                    hostId = fc.getParentid();
                    hostName = fc.getParentname();
                    info = new MappingInfo();
                    info.setId(wwn);
                    break;
                }
            }
        }
        if (null == hostName) {
            return info;
        }
        List<HostGroup> hostGroups = hostService.getHostGroupByHost(storagearrayinfo, hostName);
        if (null == hostGroups || hostGroups.size() == 0) {
            lunGroupName = "VRO_LUN_GROUP_" + hostId;
            hostGroupName = "VRO_HOST_GROUP_" + hostId;
            mappingViewName = "VRO_MAP_VIEW_" + hostId;
            lun = createLunAndHostMappingView(storagearrayinfo, lunname, null,
                    capacity, storagepoolname, alloctype, owningcontroller, readcachepolicy,
                    writecachepolicy, prefetchpolicy, prefetchvalue, datatransferpolicy,
                    iopriority, lunGroupName, null, hostName, hostGroupName, null,
                    mappingViewName, null);
        } else {
            HostGroup hostGroup = hostGroups.get(0);
            hostGroupName = hostGroup.getName();
            List<MappingView> mappingViews = mappingViewManager.getMappingViewByHostGroup(storagearrayinfo,
                    hostGroup.getReal_id());
            if (null == mappingViews || mappingViews.size() == 0) {
                mappingViewName = "VRO_MAP_VIEW_" + hostId;
                lunGroupName = "VRO_LUN_GROUP_" + hostId;
                hostManager.deleteHostGroupAssociation(storagearrayinfo, hostGroups.get(0).getReal_id(), hostId);
                lun = createLunAndHostMappingView(storagearrayinfo, lunname, null,
                        capacity, storagepoolname, alloctype, owningcontroller, readcachepolicy,
                        writecachepolicy, prefetchpolicy, prefetchvalue, datatransferpolicy,
                        iopriority, lunGroupName, null, hostName, hostGroupName, null,
                        mappingViewName, null);
            } else {
                mappingViewName = mappingViews.get(0).getName();
                List<LUNGroup> lunGroups = lunManager.getLUNGroupsByMappingview(storagearrayinfo,
                        mappingViews.get(0).getReal_id());
                LUNGroup lunGroup = null;
                if (null == lunGroups || lunGroups.size() == 0) {
                    lunGroupName = "VRO_LUN_GROUP_" + hostId;
                    lunManager.createLUNGroup(storagearrayinfo, lunGroupName, null,
                            RestContains.APPTYPE_LUNGROUP);
                    lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lunGroupName);
                    mappingViewManager.createMappingViewAssociate(storagearrayinfo, mappingViews.get(0).getReal_id(),
                            lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
                } else {
                    lunGroup = lunGroups.get(0);
                    lunGroupName = lunGroups.get(0).getName();
                }
                String poolId = poolManager.getPoolIdByName(storagepoolname, storagearrayinfo);
                lunManager.createLUN(storagearrayinfo, lunname, null, capacity, poolId, alloctype,
                        owningcontroller, readcachepolicy, writecachepolicy, prefetchpolicy, prefetchvalue,
                        datatransferpolicy, iopriority);
                lun = lunManager.getLUNByName(storagearrayinfo, lunname);
                lunManager.createLUNGroupAssociation(storagearrayinfo, lunGroup.getReal_id(), lun.getReal_id());
            }
        }
        info.setLunname(lunname);
        info.setHostname(hostName);
        info.setLungroupname(lunGroupName);
        info.setHostgroupname(hostGroupName);
        info.setMappingviewname(mappingViewName);
        info.setLunwwn(lun.getWwn());
        return info;
    }

    /**
     * Attach LUN to LUN group by initiator
     * @param storagearrayinfo Oceanstor info
     * @param hostIqns Host iqns
     * @param hostWwns Host wwns
     * @param lunname LUN name
     * @throws StorageCommonException
     * @throws RestException
     */
    public void attachLUNToLUNGroupByInitiator(StorageArrayInfo storagearrayinfo, String[] hostIqns,
        String[] hostWwns, String lunname) throws StorageCommonException, RestException {
        LUN lun = null;
        String hostName = null;
        String lunGroupName = null;
        String hostGroupName = null;
        String mappingViewName = null;
        String hostId = null;
        if (null != hostIqns) {
            for (String iqn : hostIqns) {
                iSCSIInitiator iscsi = iSCSIManager.getISCSIInitiatorById(storagearrayinfo, iqn);
                if (null != iscsi && RestContains.INITIATOR_ONLIUNE.equals(iscsi.getRunningstatus()) &&
                        null != iscsi.getParentid()) {
                    hostId = iscsi.getParentid();
                    hostName = iscsi.getParentname();
                    break;
                }
            }
        }
        if (null != hostWwns) {
            for (String wwn : hostWwns) {
                FCInitiator fc = fcInitiatorManager.getFCInitiatorById(storagearrayinfo, wwn);
                if (null != fc && RestContains.INITIATOR_ONLIUNE.equals(fc.getRunningstatus()) &&
                        null != fc.getParentid()) {
                    hostId = fc.getParentid();
                    hostName = fc.getParentname();
                    break;
                }
            }
        }
        if (null != hostName) {
            List<HostGroup> hostGroups = hostService.getHostGroupByHost(storagearrayinfo, hostName);
            if (null == hostGroups || hostGroups.size() == 0) {
                lunGroupName = "VRO_LUN_GROUP_" + hostId;
                hostGroupName = "VRO_HOST_GROUP_" + hostId;
                mappingViewName = "VRO_MAP_VIEW_" + hostId;
                lun = createLunAndHostMappingView(storagearrayinfo, lunname, lunGroupName, null,
                        hostName, hostGroupName, null, mappingViewName, null);
            } else {
                HostGroup hostGroup = hostGroups.get(0);
                hostGroupName = hostGroup.getName();
                List<MappingView> mappingViews = mappingViewManager.getMappingViewByHostGroup(storagearrayinfo,
                        hostGroup.getReal_id());
                if (null == mappingViews || mappingViews.size() == 0) {
                    mappingViewName = "VRO_MAP_VIEW_" + hostId;
                    lunGroupName = "VRO_LUN_GROUP_" + hostId;
                    hostManager.deleteHostGroupAssociation(storagearrayinfo, hostGroups.get(0).getReal_id(), hostId);
                    lun = createLunAndHostMappingView(storagearrayinfo, lunname,
                            lunGroupName, null, hostName, hostGroupName, null,
                            mappingViewName, null);
                } else {
                    List<LUNGroup> lunGroups = lunManager.getLUNGroupsByMappingview(storagearrayinfo,
                            mappingViews.get(0).getReal_id());
                    LUNGroup lunGroup = null;
                    if (null == lunGroups || lunGroups.size() == 0) {
                        lunGroupName = "VRO_LUN_GROUP_" + hostId;
                        lunManager.createLUNGroup(storagearrayinfo, lunGroupName, null,
                                RestContains.APPTYPE_LUNGROUP);
                        lunGroup = lunManager.getLUNGroupByName(storagearrayinfo, lunGroupName);
                        mappingViewManager.createMappingViewAssociate(storagearrayinfo,
                            mappingViews.get(0).getReal_id(),
                            lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
                    } else {
                        lunGroup = lunGroups.get(0);
                    }
                    lun = lunManager.getLUNByName(storagearrayinfo, lunname);
                    lunManager.createLUNGroupAssociation(storagearrayinfo,
                        lunGroups.get(0).getReal_id(), lun.getReal_id());
                }
            }
        }
    }

    /**
     * Delete LUN by LUN id and storage id
     * @param info Storage info and LUN id
     * @throws StorageCommonException
     * @throws RestException
     */
    public void deleteLUNByStorageArrayAndLUNInfo(StorageArrayAndLUNInfo info)
            throws StorageCommonException, RestException {
        StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfoById(info.getStorageid());
        deleteLun(storageArrayInfo, info.getLunname());
    }
}
