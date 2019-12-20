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
import java.util.Vector;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;

public class LUNCopyManager {
    /**
     * Create LUN copy
     * @param storageArrayInfo OceanStor info
     * @param name LUN copy name
     * @param description LUN copy description
     * @param copySpeed LUN copy speed
     * @param sourceLUNId LUN copy source LUN id
     * @param targetLUNId LUN copy target LUN id
     * @throws RestException
     */
    public void createLUNCopy(StorageArrayInfo storageArrayInfo, String name,
        String description, String copySpeed, String sourceLUNId, String targetLUNId) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN_COPY);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.COPYSPEED, copySpeed);

        operation.putOperationData(RestContains.SOURCELUN, sourceLUNId);
        operation.putOperationData(RestContains.TARGETLUN, targetLUNId);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN copy id by id or name
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @return LUN copy id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws StorageCommonException, RestException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            LUNCopy lunCopy = getLUNCopyByName(storageArrayInfo, name);
            real_id = lunCopy.getReal_id();
        }
        return real_id;
    }

    /**
     * Delete LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLUNCopy(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, name);

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_LUN_COPY);
        operation.putOperationData(RestContains.ID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN copy by name
     * @param storageArrayInfo OceanStor info
     * @param name LUN copy name
     * @return LUNCopy
     * @throws StorageCommonException
     * @throws RestException
     */
    public LUNCopy getLUNCopyByName(StorageArrayInfo storageArrayInfo,
        String name) throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_COPY_BY_NAME);

        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find LUNCopy for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one LUNCopy for Name: " + name);
        }
        LUNCopy newlunCopy = (LUNCopy) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), LUNCopy.class, storageArrayInfo);
        return newlunCopy;
    }

    /**
     * Modify LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @param newName LUN copy new name
     * @param description LUN copy description
     * @param copySpeed LUN copy speed
     * @throws RestException
     * @throws StorageCommonException
     */
    public void modifyLUNCopy(StorageArrayInfo storageArrayInfo, String id, String name, String newName,
        String description, String copySpeed) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_LUN_COPY);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.NAME, newName);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.COPYSPEED, copySpeed);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN copy count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNCopyTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_COPY_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Get LUN copy by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUNCopy>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<LUNCopy> getLUNCopyByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<LUNCopy> list = performGetLUNCopysByRange(storageArrayInfo,
                begin,
                end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getLuncopys() == null) {
                    array.setLuncopys(new Vector<LUNCopy>());
                } else {
                    array.getLuncopys().clear();
                }
                array.getLuncopys().addAll(list);
            }
        }

        return list;
    }

    /**
     * Perform get LUN copy by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUNCopy>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<LUNCopy> performGetLUNCopysByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_COPYS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<LUNCopy> lunCopys = new ArrayList<LUNCopy>();
        for (Map<String, String> map : list) {
            LUNCopy luncopy = (LUNCopy) ObjectsBuilder.INSTANCE.buildObject(map,
                    LUNCopy.class,
                    storageArrayInfo);
            lunCopys.add(luncopy);
        }
        return lunCopys;
    }

    /**
     * Start LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void startLUNCopy(StorageArrayInfo storageArrayInfo, String id, String name)
        throws RestException, StorageCommonException {
        String rea_id = adjustId(storageArrayInfo, id, name);

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.START_LUN_COPY);
        operation.putOperationData(RestContains.ID, rea_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Stop LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void stopLUNCopy(StorageArrayInfo storageArrayInfo, String id, String name)
        throws RestException, StorageCommonException {
        String rea_id = adjustId(storageArrayInfo, id, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.STOP_LUN_COPY);
        operation.putOperationData(RestContains.ID, rea_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Suspend LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void suspendLUNCopy(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.SUSPEND_LUN_COPY);
        operation.putOperationData(RestContains.ID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Continue LUN copy
     * @param storageArrayInfo OceanStor info
     * @param id LUN copy id
     * @param name LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void continueLUNCopy(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CONTINUE_LUN_COPY);
        operation.putOperationData(RestContains.ID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Add LUN copy target
     * @param storageArrayInfo OceanStor info
     * @param targetId LUN copy target id
     * @param lunCopyId LUN copy id
     * @param lunCopyName LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void addLUNCopyTarget(StorageArrayInfo storageArrayInfo, String targetId, String lunCopyId,
        String lunCopyName) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, lunCopyId, lunCopyName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.ADD_LUN_COPY_TARGET);
        operation.putOperationData(RestContains.ID, targetId);
        operation.putOperationData(RestContains.LUNCOPYID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Remove LUN copy  target
     * @param storageArrayInfo OceanStor info
     * @param targetId LUN copy target id
     * @param lunCopyId LUN copy id
     * @param lunCopyName LUN copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void removeLUNCopyTarget(StorageArrayInfo storageArrayInfo, String targetId, String lunCopyId,
        String lunCopyName) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, lunCopyId, lunCopyName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.REMOVE_LUN_COPY_TARGET);
        operation.putOperationData(RestContains.ID, targetId);
        operation.putOperationData(RestContains.LUNCOPYID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }
}
