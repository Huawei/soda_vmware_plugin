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
import org.apache.log4j.Logger;

public class LUNSnapshotManager {
    private static final Logger logger = Logger.getLogger(LUNSnapshotManager.class);

    private LUNManager lunManager = new LUNManager();

    /**
     * Create LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @param description LUN snapshot description
     * @param LUNName LUN name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createLUNSnapshot(StorageArrayInfo storageArrayInfo,
        String name, String description, String LUNName) throws RestException, StorageCommonException {
        LUN lun = lunManager.getLUNByName(storageArrayInfo, LUNName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.PARENTID, lun.getReal_id());
        operation.putOperationData(RestContains.DESCRIPTION, description);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param currentName LUN snapshot current name
     * @param newName LUN snapshot new name
     * @param newDescription LUN snapshot new description
     * @throws RestException
     * @throws StorageCommonException
     */
    public void modifyLUNSnapshot(StorageArrayInfo storageArrayInfo, String currentName, String newName,
        String newDescription) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, currentName);

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.NAME, newName);
        operation.putOperationData(RestContains.DESCRIPTION, newDescription);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN snapshot id by name
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @return LUN snapshot id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String getIdByName(StorageArrayInfo storageArrayInfo, String name)
        throws RestException, StorageCommonException {
        return getLUNSnapshotByName(storageArrayInfo, name).getReal_id();
    }

    /**
     * Get LUN snapshot info by LUN snapshot name
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @return LUNSnapshot
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUNSnapshot getLUNSnapshotByName(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        LUNSnapshot lunSnapshot = performGetLUNSnapshotByName(storageArrayInfo, name);
        return lunSnapshot;
    }

    /**
     * Perform get LUN snapshot by name
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @return LUNSnapshot
     * @throws RestException
     * @throws StorageCommonException
     */
    public LUNSnapshot performGetLUNSnapshotByName(StorageArrayInfo storageArrayInfo, String name)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_SNAPSHOT_BY_NAME);

        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find LUN Snapshot: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one LUN Snapshot: " + name);
        }
        LUNSnapshot lunSnapshot = (LUNSnapshot) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), LUNSnapshot.class, storageArrayInfo);
        return lunSnapshot;
    }

    /**
     * Delete LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteLUNSnapshot(StorageArrayInfo storageArrayInfo, String name)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Active LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param snapshotIdList LUN snapshot id list
     * @throws RestException
     */
    public void activeLUNSnapshot(StorageArrayInfo storageArrayInfo, String snapshotIdList) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.ACTIVE_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.SNAPSHOTLIST, "[" + snapshotIdList + "]");
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Deactive LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deactiveLUNSnapshot(StorageArrayInfo storageArrayInfo, String name)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DEACTIVE_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Rollback LUN snapshot
     * @param storageArrayInfo OceanStor info
     * @param name LUN snapshot name
     * @param rollbackSpeed Rollback speed
     * @throws RestException
     * @throws StorageCommonException
     */
    public void rollbackLUNSnapshot(StorageArrayInfo storageArrayInfo,
        String name, String rollbackSpeed) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.ROLLBACK_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.ROLLBACKSPEED, rollbackSpeed);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Cancel rollback LUN snapshot
     * @param storageArrayInfo FusionStorage info
     * @param name LUN snapshot name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void cancelRollbackLUNSnapshot(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CANCEL_ROLLBACK_LUN_SNAPSHOT);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Create LUN snapshot copy
     * @param storageArrayInfo OceanStor info
     * @param snapshotId LUN snapshot id
     * @param snapshotName LUN snapshot name
     * @param snapshotCopyName LUN snapshot copy name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createLUNSnapshotCopy(StorageArrayInfo storageArrayInfo, String snapshotId, String snapshotName,
        String snapshotCopyName) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, snapshotId, snapshotName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_LUN_SNAPSHOT_COPY);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.NAME, snapshotCopyName);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get LUN snapshot count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getLUNSnapshotTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_LUN_SNAPSHOT_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Get LUN snapshot by range
     * @param storageArrayInfo CeanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<LUNSnapshot>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<LUNSnapshot> getLUNSnapshotByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<LUNSnapshot> lunsnapshots = performLUNSnapshotByRange(
                storageArrayInfo, String.valueOf(begin), String.valueOf(end));

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getLunsnapshots() == null) {
                    array.setLunsnapshots(new Vector<LUNSnapshot>());
                } else {
                    array.getLunsnapshots().clear();
                }
                array.getLunsnapshots().addAll(lunsnapshots);
            }
        }
        return lunsnapshots;
    }

    /**
     * Perfom get LUN snapshot by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end  End index
     * @return List<LUNSnapshot>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<LUNSnapshot> performLUNSnapshotByRange(StorageArrayInfo storageArrayInfo, String begin, String end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation
                .setOperationName(OperationNamesEnum.GET_LUN_SNAPSHOT_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<LUNSnapshot> lunSnapshots = new ArrayList<LUNSnapshot>();
        for (Map<String, String> map : list) {
            LUNSnapshot lunSnapshot = (LUNSnapshot) ObjectsBuilder.INSTANCE
                    .buildObject(map, LUNSnapshot.class, storageArrayInfo);
            lunSnapshots.add(lunSnapshot);
        }
        return lunSnapshots;
    }

    private String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            LUNSnapshot lunSnapshot = performGetLUNSnapshotByName(
                    storageArrayInfo, name);
            real_id = lunSnapshot.getReal_id();
        }
        return real_id;
    }
}
