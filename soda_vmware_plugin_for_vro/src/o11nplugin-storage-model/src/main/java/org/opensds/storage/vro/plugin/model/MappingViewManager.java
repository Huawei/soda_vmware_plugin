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

public class MappingViewManager {
    /**
     * Create Mappingview
     * @param storageArrayInfo OceanStor info
     * @param name Mappingview name
     * @param description Mappingview description
     * @return List<Map<String, String>>
     * @throws RestException
     */
    public List<Map<String, String>> createMappingView(StorageArrayInfo storageArrayInfo, String name,
        String description) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_MAPPING_VIEW);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);

        return OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get mappingview by name
     * @param storageArrayInfo OceanStor info
     * @param name Mappingview name
     * @return MappingView
     * @throws RestException
     * @throws StorageCommonException
     */
    public MappingView getMappingViewByName(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_MAPPING_VIEW_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find MappingView for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one MappingView for Name: " + name);
        }
        MappingView mappingView = (MappingView) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), MappingView.class, storageArrayInfo);
        return mappingView;
    }

    /**
     * Get Mappingview by id
     * @param storageArrayInfo OceanStor info
     * @param id Mappingview id
     * @return MappingViewInfo
     * @throws RestException
     * @throws StorageCommonException
     */
    public MappingViewInfo getMappingViewById(StorageArrayInfo storageArrayInfo,
        String id) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_MAPPING_VIEW_BY_ID);
        operation.putOperationData(RestContains.ID, id);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find MappingView for Name: " + id);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one MappingView for Name: " + id);
        }
        MappingViewInfo mappingView = (MappingViewInfo) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), MappingViewInfo.class, storageArrayInfo);
        return mappingView;
    }

    /**
     * Get Mappingview by LUN group
     * @param storageArrayInfo OceanStor info
     * @param groupId LUN group id
     * @return List<MappingView>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<MappingView> getMappingViewByLUNGroup(StorageArrayInfo storageArrayInfo, String groupId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_MAPPING_VIEW_BY_LUNGROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, groupId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<MappingView> result = new ArrayList<MappingView>();
        for (Map<String, String> map : list) {
            MappingView mappingView = (MappingView) ObjectsBuilder.INSTANCE
                    .buildObject(list.get(0), MappingView.class, storageArrayInfo);
            result.add(mappingView);
        }
        return result;
    }

    /**
     * Get mappingview by host group
     * @param storageArrayInfo OceanStor info
     * @param groupId Host group id
     * @return List<MappingView>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<MappingView> getMappingViewByHostGroup(StorageArrayInfo storageArrayInfo, String groupId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_MAPPING_VIEW_BY_LUNGROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, groupId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<MappingView> result = new ArrayList<MappingView>();
        for (Map<String, String> map : list) {
            MappingView mappingView = (MappingView) ObjectsBuilder.INSTANCE
                    .buildObject(list.get(0), MappingView.class, storageArrayInfo);
            result.add(mappingView);
        }
        return result;
    }

    /**
     * Delete mappingview
     * @param storageArrayInfo OceanStor info
     * @param id Mappingview id
     * @throws RestException
     */
    public void deleteMappingViewById(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_MAPPING_VIEW);
        operation.putOperationData(RestContains.ID, id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Create Mapping view associate
     * @param storageArrayInfo OceanStor info
     * @param mappingViewId Mappingview id
     * @param associateObjectId Associate id : Host group id or LUN group id
     * @param associateObjectType Associate object type : LUN group or host group
     * @throws RestException
     */
    public void createMappingViewAssociate(StorageArrayInfo storageArrayInfo, String mappingViewId,
        String associateObjectId, String associateObjectType) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.CREATE_MAPPING_VIEW_ASSOCIATE);
        operation.putOperationData(RestContains.ID, mappingViewId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID,
                associateObjectId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE,
                associateObjectType);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete Mapping view associate
     * @param storageArrayInfo OceanStor info
     * @param mappingViewId Mappingview id
     * @param associateObjectId Associate id : Host group id or LUN group id
     * @param associateObjectType Associate object type : LUN group or host group
     * @throws RestException
     */
    public void deleteMappingViewAssociate(StorageArrayInfo storageArrayInfo, String mappingViewId,
        String associateObjectId, String associateObjectType) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.DELETE_MAPPING_VIEW_ASSOCIATE);
        operation.putOperationData(RestContains.ID, mappingViewId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID,
                associateObjectId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE,
                associateObjectType);
        OperationUtil.performAction(storageArrayInfo, operation);
    }
}
