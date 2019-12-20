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
import java.util.Map;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;

public class StoragePoolManager {
    /**
     * Get Storage pool by name
     * @param name Storage pool name
     * @param storageArrayInfo OceanStor info
     * @return Storage pool id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String getPoolIdByName(String name, StorageArrayInfo storageArrayInfo)
        throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.FIND_STORAGE_POOL_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + name);

        List<Map<String, String>> operationResult = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (operationResult.size() != 1) {
            throw new StorageCommonException(
                    "can not find storage pool or there is not only one pool by name : "
                            + name + " count: " + operationResult.size());
        }
        Map<String, String> map = operationResult.get(0);
        return (String) map.get(RestContains.ID);
    }

    /**
     * Get Storage pool id by id or name
     * @param storageArrayInfo OceanStorage info
     * @param id Storage pool id
     * @param name Storage pool name
     * @return Storage pool id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws StorageCommonException, RestException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            real_id = getPoolIdByName(name, storageArrayInfo);
        }
        return real_id;
    }
}
