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

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;

public class LUNManager {
    /**
     * Get LUN by LUN id or LUN name
     * @param storageArrayInfo OceanStor info
     * @param id LUN id
     * @param name LUN name
     * @return LUN id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id, String name)
        throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            LUN lun = getLUNByName(storageArrayInfo, name);
            real_id = lun.getReal_id();
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
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_BY_ID);
        operation.putOperationData(RestContains.ID, LUNId);

        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find LUN By Id: " + LUNId);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one LUN for Id: " + LUNId);
        }
        LUN lun = (LUN) ObjectsBuilder.INSTANCE.buildObject(list.get(0), LUN.class, storageArrayInfo);
        return lun;
    }

    /**
     * Create LUN
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @param Description LUN description
     * @param Capacity LUN capacity
     * @param StoragePoolId Storage pool id
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
        String StoragePoolId, String alloctype, String owningcontroller, String readcachepolicy,
        String writecachepolicy, String prefetchpolicy, String prefetchvalue, String datatransferpolicy,
        String iopriority) throws RestException {
        // 固定预取单位为扇区，需要转换为KB
        String value = null;
        if (null != prefetchpolicy && prefetchpolicy.equals("1")
            && null != prefetchvalue && !prefetchvalue.equals("")) {
            value = String.valueOf(Integer.valueOf(prefetchvalue) * 2);
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN);
        operation.putOperationData(RestContains.NAME, LUNName);
        operation.putOperationData(RestContains.DESCRIPTION, Description);
        operation.putOperationData(RestContains.CAPACITY, String.valueOf(Capacity * RestContains.GB_TO_SECTOR));
        operation.putOperationData(RestContains.PARENTID, StoragePoolId);

        operation.putOperationData(RestContains.ALLOCTYPE, alloctype);
        operation.putOperationData(RestContains.OWNINGCONTROLLER, owningcontroller);
        operation.putOperationData(RestContains.READCACHEPOLICY, readcachepolicy);
        operation.putOperationData(RestContains.WRITECACHEPOLICY, writecachepolicy);
        operation.putOperationData(RestContains.PREFETCHPOLICY, prefetchpolicy);
        operation.putOperationData(RestContains.PREFETCHVALUE, value);
        operation.putOperationData(RestContains.DATATRANSFERPOLICY, datatransferpolicy);
        operation.putOperationData(RestContains.IOPRIORITY, iopriority);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * MOdify LUN
     * @param storageArrayInfo OceanStor info
     * @param id LUN id
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
    public void modifyLUN(StorageArrayInfo storageArrayInfo, String id, String newName, String Description,
        String owningcontroller, String readcachepolicy, String writecachepolicy, String prefetchpolicy,
        String prefetchvalue, String datatransferpolicy, String iopriority) throws RestException {
        // 固定预取单位为扇区，需要转换为KB
        String value = null;
        if (null != prefetchpolicy && prefetchpolicy.equals("1")
            && null != prefetchvalue && !prefetchvalue.equals("")) {
            value = String.valueOf(Integer.valueOf(prefetchvalue) * 2);
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.MODIFY_LUN);
        operation.putOperationData(RestContains.ID, id);
        operation.putOperationData(RestContains.NAME, newName);
        operation.putOperationData(RestContains.DESCRIPTION, Description);

        operation.putOperationData(RestContains.OWNINGCONTROLLER, owningcontroller);
        operation.putOperationData(RestContains.READCACHEPOLICY, readcachepolicy);
        operation.putOperationData(RestContains.WRITECACHEPOLICY, writecachepolicy);
        operation.putOperationData(RestContains.PREFETCHPOLICY, prefetchpolicy);
        operation.putOperationData(RestContains.PREFETCHVALUE, value);
        operation.putOperationData(RestContains.DATATRANSFERPOLICY, datatransferpolicy);
        operation.putOperationData(RestContains.IOPRIORITY, iopriority);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUN>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUN> getLUNsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUNS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE, "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<LUN> luns = new ArrayList<LUN>();
        for (Map<String, String> map : list) {
            LUN lun = (LUN) ObjectsBuilder.INSTANCE.buildObject(map, LUN.class, storageArrayInfo);
            luns.add(lun);
        }
        return luns;
    }

    /**
     * Get LUN by name
     * @param storageArrayInfo OceanStor info
     * @param LUNName LUN name
     * @return LUN
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUN getLUNByName(StorageArrayInfo storageArrayInfo, String LUNName)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + LUNName);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find LUN:" + LUNName);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one LUN for Name: " + LUNName);
        }
        LUN lun = (LUN) ObjectsBuilder.INSTANCE.buildObject(list.get(0), LUN.class, storageArrayInfo);
        return lun;
    }

    /**
     * Get LUN by wwn
     * @param storageArrayInfo OceanStor info
     * @param wwn LUN wwn
     * @return LUN
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUN getLUNByWwn(StorageArrayInfo storageArrayInfo, String wwn)
        throws RestException, StorageCommonException {
        Integer lunId = Integer.parseInt(wwn.substring(wwn.length() - 8), 16);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_BY_ID);
        operation.putOperationData(RestContains.ID, lunId.toString());

        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find LUN By wwn: " + wwn);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one LUN for wwn: " + wwn);
        }
        LUN lun = (LUN) ObjectsBuilder.INSTANCE.buildObject(list.get(0), LUN.class, storageArrayInfo);
        if (!lun.getWwn().equalsIgnoreCase(wwn)) {
            throw new StorageCommonException("can not find LUN By wwn: " + wwn);
        }
        return lun;
    }

    /**
     * Get LUN by LUN group
     * @param storageArrayInfo OceanStor info
     * @param groupId LUN group id
     * @return List<LUN>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUN> getLUNsByLungroup(StorageArrayInfo storageArrayInfo, String groupId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_BY_LUNGROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, groupId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<LUN> luns = new ArrayList<LUN>();
        for (Map<String, String> map : list) {
            LUN lun = (LUN) ObjectsBuilder.INSTANCE.buildObject(map, LUN.class, storageArrayInfo);
            luns.add(lun);
        }
        return luns;
    }

    /**
     * Get LUN group by mapping view
     * @param storageArrayInfo OceanStor info
     * @param mappingViewId Mapping view id
     * @return List<LUNGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUNGroup> getLUNGroupsByMappingview(StorageArrayInfo storageArrayInfo, String mappingViewId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUNGROUPS_BY_MAPPINGVIEW);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_MAPPING_VIEW);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, mappingViewId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<LUNGroup> result = new ArrayList<LUNGroup>();
        for (Map<String, String> map : list) {
            LUNGroup lunGroup = (LUNGroup) ObjectsBuilder.INSTANCE.buildObject(map, LUNGroup.class, storageArrayInfo);
            result.add(lunGroup);
        }
        return result;
    }

    /**
     * Expand LUN capacity
     * @param storageArrayInfo OceanStor info
     * @param LUNId LUN id
     * @param Capacity LUN new capacity
     * @throws RestException
     */
    public void expandLUNCapacity(StorageArrayInfo storageArrayInfo, String LUNId, long Capacity) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.EXPAND_LUN);
        operation.putOperationData(RestContains.ID, LUNId);
        if (Capacity != 0) {
            operation.putOperationData(RestContains.CAPACITY, String.valueOf(Capacity * RestContains.GB_TO_SECTOR));
        }
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete LUN by id
     * @param storageArrayInfo OceanStor info
     * @param id LUN id
     * @throws RestException
     */
    public void deleteLUNById(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_LUN_BY_ID);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        return list.get(0).get(RestContains.COUNT);
    }

    /**
     * Create LUN group
     * @param storageArrayInfo OceanStor info
     * @param name LUN group name
     * @param description LUN group description
     * @param appType LUN group application type
     * @throws RestException
     */
    public void createLUNGroup(StorageArrayInfo storageArrayInfo, String name, String description, String appType)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN_GROUP);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.APPTYPE, appType);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete LUN group
     * @param storageArrayInfo OceanStor info
     * @param lunGroupId LUN group id
     * @throws RestException
     */
    public void deleteLUNGroupById(StorageArrayInfo storageArrayInfo, String lunGroupId) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_LUNGROUP_BY_ID);
        operation.putOperationData(RestContains.ID, lunGroupId);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN group by name
     * @param storageArrayInfo OceanStor info
     * @param LUNGroupName LUN group name
     * @return LUNGroup
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUNGroup getLUNGroupByName(StorageArrayInfo storageArrayInfo, String LUNGroupName)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUNGROUP_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + LUNGroupName);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find LUN  Group for Name: " + LUNGroupName);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one LUN Group for Name: " + LUNGroupName);
        }
        LUNGroup lungroup = (LUNGroup) ObjectsBuilder.INSTANCE.buildObject(list.get(0), LUNGroup.class,
                storageArrayInfo);
        return lungroup;
    }

    /**
     * Get LUN group by lun id
     * @param storageArrayInfo OceanStor info
     * @param lunId LUN id
     * @return List<LUNGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUNGroup> getLUNGroupByLunId(StorageArrayInfo storageArrayInfo, String lunId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUNGROUPS_BY_LUNID);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, lunId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find LUN  Group for Lunid: " + lunId);
        }
        List<LUNGroup> result = new ArrayList<LUNGroup>();
        for (Map<String, String> map : list) {
            LUNGroup lunGroup = (LUNGroup) ObjectsBuilder.INSTANCE.buildObject(map, LUNGroup.class, storageArrayInfo);
            result.add(lunGroup);
        }
        return result;
    }

    /**
     * Get LUN group by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUNGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUNGroup> getLUNGroupsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUNGROUPS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE, "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<LUNGroup> lungroups = new ArrayList<LUNGroup>();
        for (Map<String, String> map : list) {
            LUNGroup lungroup = (LUNGroup) ObjectsBuilder.INSTANCE.buildObject(map, LUNGroup.class, storageArrayInfo);
            lungroups.add(lungroup);
        }
        return lungroups;
    }

    /**
     * Get LUN group total count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNGroupTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_GROUP_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        return list.get(0).get(RestContains.COUNT);
    }

    /**
     * Create LUN and LUN group association
     * @param storageArrayInfo OceanStor info
     * @param lunGroupId LUN group id
     * @param lunId LUN id
     * @throws RestException
     */
    public void createLUNGroupAssociation(StorageArrayInfo storageArrayInfo, String lunGroupId, String lunId)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, lunGroupId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, lunId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete LUN group association
     * @param storageArrayInfo OceanStor info
     * @param lunGroupId LUN group id
     * @param lunId LUN id
     * @throws RestException
     */
    public void deleteLUNGroupAssociation(StorageArrayInfo storageArrayInfo, String lunGroupId, String lunId)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_LUN_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, lunGroupId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, lunId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN);
        operation.putOperationData(RestContains.TIME, String.valueOf(System.currentTimeMillis()));
        OperationUtil.performAction(storageArrayInfo, operation);
    }
}
