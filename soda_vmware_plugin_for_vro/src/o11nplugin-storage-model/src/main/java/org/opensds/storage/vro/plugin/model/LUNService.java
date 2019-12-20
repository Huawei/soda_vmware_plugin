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
import java.util.Vector;

import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;
import org.apache.log4j.Logger;

public class LUNService {
    private static final Logger log = Logger.getLogger(LUNService.class);

    private LUNManager lunManager = new LUNManager();

    private StoragePoolManager poolManager = new StoragePoolManager();

    /**
     * Get LUN id by LUN name or LUN id
     * @param storageArrayInfo OceanStor info
     * @param id LUN id
     * @param name LUN name
     * @return LUN id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            LUN lun = lunManager.getLUNByName(storageArrayInfo, name);
            real_id = lun.getReal_id();
        }
        return real_id;
    }

    /**
     * Get LUN group by LUN group name or LUN group id
     * @param storageArrayInfo OceanStor info
     * @param id LUN group id
     * @param name LUN group name
     * @return LUN group id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustLUNGroupId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            LUNGroup lungroup = lunManager.getLUNGroupByName(storageArrayInfo,
                    name);
            real_id = lungroup.getReal_id();
        }
        return real_id;
    }

    /**
     * Get LUN by id
     * @param storageArrayInfo OceanStor info
     * @param LUNId LUN id
     * @return LUN
     * @throws StorageCommonException
     * @throws RestException
     */
    public LUN getLUNById(StorageArrayInfo storageArrayInfo, String LUNId)
        throws StorageCommonException, RestException {
        LUN lun = lunManager.getLUNById(storageArrayInfo, LUNId);

        return lun;
    }

    /**
     * Create LUN
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @param Description LUN description
     * @param Capacity LUN capacity
     * @param StoragePoolName Storage pool name
     * @param alloctype  Space allocation type
     * @param owningcontroller Owning controller
     * @param readcachepolicy Read cache policy
     * @param writecachepolicy Write cache policy.
     * @param prefetchpolicy intelligent prefetch
     * @param prefetchvalue Prefetch policy value
     * @param datatransferpolicy Data migration policy
     * @param iopriority I/O priority
     * @throws RestException
     */
    public void createLUN(StorageArrayInfo storageArrayInfo, String LUNName, String Description, int Capacity,
        String StoragePoolName, String alloctype, String owningcontroller, String readcachepolicy,
        String writecachepolicy, String prefetchpolicy, String prefetchvalue, String datatransferpolicy,
        String iopriority) throws RestException, StorageCommonException {
        String poolId = poolManager.getPoolIdByName(StoragePoolName,
                storageArrayInfo);
        lunManager.createLUN(storageArrayInfo, LUNName, Description, Capacity, poolId, alloctype, owningcontroller,
                readcachepolicy, writecachepolicy, prefetchpolicy, prefetchvalue, datatransferpolicy, iopriority);
    }

    /**
     * MOdify LUN
     * @param storageArrayInfo OceanStor info
     * @param currentName LUN current name
     * @param newName LUN new name
     * @param Description LUN description
     * @param owningcontroller Owning controller
     * @param readcachepolicy Read cache policy
     * @param writecachepolicy Write cache policy.
     * @param prefetchpolicy intelligent prefetch
     * @param prefetchvalue Prefetch policy value
     * @param datatransferpolicy Data migration policy
     * @param iopriority I/O priority
     * @throws RestException
     */
    public void modifyLUN(StorageArrayInfo storageArrayInfo, String currentName, String newName, String Description,
        String owningcontroller, String readcachepolicy, String writecachepolicy, String prefetchpolicy,
        String prefetchvalue, String datatransferpolicy, String iopriority)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, currentName);
        lunManager.modifyLUN(storageArrayInfo, real_id, newName, Description, owningcontroller, readcachepolicy,
                writecachepolicy, prefetchpolicy, prefetchvalue, datatransferpolicy, iopriority);
    }

    /**
     * Get LUN id by LUN name
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @return LUN id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String getIdByName(StorageArrayInfo storageArrayInfo, String LUNName)
        throws RestException, StorageCommonException {
        return getLUNByName(storageArrayInfo, LUNName).getReal_id();
    }

    /**
     * Get LUN by LUN name
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @return LUN
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUN getLUNByName(StorageArrayInfo storageArrayInfo, String LUNName)
        throws RestException, StorageCommonException {
        LUN lun = lunManager.getLUNByName(storageArrayInfo, LUNName);
        return lun;
    }

    /**
     * Get LUN by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUN>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<LUN> getLUNsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<LUN> luns = lunManager.getLUNsByRange(storageArrayInfo,
                begin,
                end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getLunList() == null) {
                    array.setLunList(new Vector<LUN>());
                } else {
                    array.getLunList().clear();
                }
                array.getLunList().addAll(luns);
            }
        }
        return luns;
    }

    /**
     * Expand LUN capacity
     * @param storageArrayInfo OceanStorinfo
     * @param LUNName LUN name
     * @param Capacity LUN new capacity
     * @throws RestException
     * @throws StorageCommonException
     */
    public void expandLUNCapacity(StorageArrayInfo storageArrayInfo, String LUNName, long Capacity)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, LUNName);
        lunManager.expandLUNCapacity(storageArrayInfo, real_id, Capacity);
    }

    /**
     * Delete LUN
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLUN(StorageArrayInfo storageArrayInfo, String LUNName)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, LUNName);
        lunManager.deleteLUNById(storageArrayInfo, real_id);
    }

    /**
     * Get LUN count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        String count = lunManager.getLUNTotalCount(storageArrayInfo);
        return count;
    }

    /**
     * Create LUN group
     * @param storageArrayInfo OceanStor info
     * @param name LUN group name
     * @param description LUN group description
     * @throws RestException
     */
    public void createLUNGroup(StorageArrayInfo storageArrayInfo, String name,
        String description) throws RestException {
        lunManager.createLUNGroup(storageArrayInfo,
                name,
                description,
                RestContains.APPTYPE_LUNGROUP);
    }

    /**
     * Delete LUN group
     * @param storageArrayInfo OceanStor info
     * @param LUNGroupName LUN group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLUNGroup(StorageArrayInfo storageArrayInfo,
        String LUNGroupName) throws RestException, StorageCommonException {
        String real_id = adjustLUNGroupId(storageArrayInfo, null, LUNGroupName);
        lunManager.deleteLUNGroupById(storageArrayInfo, real_id);
    }

    /**
     * Get LUN group by name
     * @param storageArrayInfo OceanStor info
     * @param LUNGroupName LUN group name
     * @return LUNGroup
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUNGroup getLUNGroupByName(StorageArrayInfo storageArrayInfo,
        String LUNGroupName) throws RestException, StorageCommonException {
        return lunManager.getLUNGroupByName(storageArrayInfo, LUNGroupName);
    }

    /**
     * Get LUN group count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNGroupTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        String count = lunManager.getLUNGroupTotalCount(storageArrayInfo);
        return count;
    }

    /**
     * Get LUN group by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUNGroup>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<LUNGroup> getLUNGroupsByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<LUNGroup> lunGroups = lunManager
                .getLUNGroupsByRange(storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getLunGroups() == null) {
                    array.setLunGroups(new Vector<LUNGroup>());
                } else {
                    array.getLunGroups().clear();
                }
                array.getLunGroups().addAll(lunGroups);
            }
        }
        return lunGroups;
    }

    /**
     * Create LUN and LUN group association
     * @param storageArrayInfo OceanStor info
     * @param LUNGroupName LUN group name
     * @param LUNName LUN name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createLUNGroupAssociate(StorageArrayInfo storageArrayInfo, String LUNGroupName, String LUNName)
        throws RestException, StorageCommonException {
        LUNGroup lunGroup = lunManager.getLUNGroupByName(storageArrayInfo,
                LUNGroupName);
        LUN lun = lunManager.getLUNByName(storageArrayInfo, LUNName);
        if (lunGroup == null) {
            throw new StorageCommonException(
                    "can not find lungroup : " + LUNGroupName);
        }
        if (lun == null) {
            throw new StorageCommonException("can not find lun : " + LUNName);
        }
        lunManager.createLUNGroupAssociation(storageArrayInfo, lunGroup.getReal_id(), lun.getReal_id());
    }

    /**
     * Delete LUN and LUN group association
     * @param storageArrayInfo OceanStor info
     * @param LUNGroupName LUN group name
     * @param LUNName LUN name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLUNGroupAssociate(StorageArrayInfo storageArrayInfo, String LUNGroupName, String LUNName)
        throws RestException, StorageCommonException {
        LUNGroup lunGroup = lunManager.getLUNGroupByName(storageArrayInfo,
                LUNGroupName);
        LUN lun = lunManager.getLUNByName(storageArrayInfo, LUNName);
        if (lunGroup == null) {
            throw new StorageCommonException(
                    "can not find lungroup : " + LUNGroupName);
        }
        if (lun == null) {
            throw new StorageCommonException("can not find lun : " + LUNName);
        }
        lunManager.deleteLUNGroupAssociation(storageArrayInfo, lunGroup.getReal_id(), lun.getReal_id());
    }

    /**
     * Create LUN and LUN group mappingview
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @param Description LUN description
     * @param Capacity LUN capacity
     * @param StoragePoolName Storage pool name
     * @param alloctype  Space allocation type
     * @param owningcontroller Owning controller
     * @param readcachepolicy Read cache policy
     * @param writecachepolicy Write cache policy.
     * @param prefetchpolicy intelligent prefetch
     * @param prefetchvalue Prefetch policy value
     * @param datatransferpolicy Data migration policy
     * @param iopriority I/O priority
     * @param lunGroup_name LUN group name
     * @param lunGroup_description LUN group description
     * @throws StorageCommonException
     * @throws RestException
     */
    public void createLunAndLunGroupMappingView(StorageArrayInfo storageArrayInfo, String LUNName,
        String Description, int Capacity, String StoragePoolName, String alloctype, String owningcontroller,
        String readcachepolicy, String writecachepolicy, String prefetchpolicy, String prefetchvalue,
        String datatransferpolicy, String iopriority, String lunGroup_name, String lunGroup_description)
        throws StorageCommonException, RestException {
        String poolId = poolManager.getPoolIdByName(StoragePoolName, storageArrayInfo);
        lunManager.createLUN(storageArrayInfo, LUNName, Description, Capacity, poolId, alloctype, owningcontroller,
            readcachepolicy, writecachepolicy, prefetchpolicy, prefetchvalue, datatransferpolicy, iopriority);
        lunManager.createLUNGroup(storageArrayInfo, lunGroup_name, lunGroup_description,
            RestContains.APPTYPE_LUNGROUP);
    }

    /**
     * Expand LUN for datastore
     * @param wwn LUN wwn
     * @param newSize LUN new size
     * @throws StorageCommonException
     * @throws RestException
     */
    public void expandLUNForDatastore(String wwn, long newSize) throws StorageCommonException, RestException {
        StorageArrayAndLUNInfo info = getStorageArrayAndLUNInfoByWWN(wwn);
        StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfoById(info.getStorageid());
        lunManager.expandLUNCapacity(storageArrayInfo, info.getId(), newSize);
    }

    /**
     * Get Storage info and LUN info by LUN wwn
     * @param wwn LUN wwn
     * @return StorageArrayAndLUNInfo
     * @throws StorageCommonException
     * @throws RestException
     */
    public StorageArrayAndLUNInfo getStorageArrayAndLUNInfoByWWN(String wwn)
        throws StorageCommonException, RestException {
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        LUN lun = null;
        StorageArrayAndLUNInfo info = null;
        for (StorageArrayInfo storageArrayInfo : storageArrayInfos) {
            try {
                lun = lunManager.getLUNByWwn(storageArrayInfo, wwn);
                if (null != lun && wwn.equals(lun.getWwn())) {
                    info = new StorageArrayAndLUNInfo();
                    info.setStorageid(storageArrayInfo.getId());
                    info.setId(lun.getReal_id());
                    info.setLunname(lun.getName());
                    break;
                }
            } catch (Exception e) {
                continue;
            }
        }
        if (null != info) {
            return info;
        } else {
            throw new StorageCommonException("can not find LUN By wwn: " + wwn);
        }
    }
}
