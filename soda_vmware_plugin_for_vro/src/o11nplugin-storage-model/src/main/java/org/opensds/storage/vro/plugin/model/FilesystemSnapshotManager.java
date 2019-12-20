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

public class FilesystemSnapshotManager {
    private FilesystemManager filesystemManager = new FilesystemManager();

    /**
     * Create filesystem snapshot
     * @param storageArrayInfo OceanStor info
     * @param snapshotName Filesystem snapshot name
     * @param filesystemId Filesystem  id
     * @param filesystemName Filesystem name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createFilesystemSnapshot(StorageArrayInfo storageArrayInfo, String snapshotName, String filesystemId,
        String filesystemName) throws RestException, StorageCommonException {
        String parentId = filesystemManager.adjustId(storageArrayInfo,
                filesystemId,
                filesystemName);

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.CREATE_FILESYSTEM_SNAPSHOT);
        operation.putOperationData(RestContains.NAME, snapshotName);
        operation.putOperationData(RestContains.PARENTID, parentId);
        operation.putOperationData(RestContains.PARENTTYPE,
                RestContains.PARENTTYPE_FILESYSTEM);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete filesystem snapshot
     * @param storageArrayInfo OceanStor info
     * @param id Filesystem snapshot id
     * @throws RestException
     */
    public void deleteFilesystemSnapshot(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.DELETE_FILESYSTEM_SNAPSHOT);
        operation.putOperationData(RestContains.ID, id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get filesystem snapshot id by name
     * @param storageArrayInfo OceanStor info
     * @param snapshotName Filesystem snapshot name
     * @param filesystemId Filesystem id
     * @param filesystemName Filesystem name
     * @param vstoreId Vstore id
     * @return Filesystem snapshot id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String getIdByName(StorageArrayInfo storageArrayInfo, String snapshotName, String filesystemId,
        String filesystemName, String vstoreId) throws StorageCommonException, RestException {
        return getFilesystemSnapshotByName(storageArrayInfo,
                snapshotName,
                filesystemId,
                filesystemName,
                vstoreId).getReal_id();
    }

    /**
     * Get filesystem snapshot by name
     * @param storageArrayInfo OceanStor info
     * @param snapshotName Filesystem snapshot name
     * @param filesystemId Filesystem id
     * @param filesystemName Filesystem name
     * @param vstoreId Vstore id
     * @return FilesystemSnapshot
     * @throws StorageCommonException
     * @throws RestException
     */
    public FilesystemSnapshot getFilesystemSnapshotByName(StorageArrayInfo storageArrayInfo, String snapshotName,
        String filesystemId, String filesystemName, String vstoreId) throws StorageCommonException, RestException {
        String parentId = filesystemManager.adjustId(storageArrayInfo,
                filesystemId,
                filesystemName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_FILESYSTEM_SNAPSHOT_BY_NAME);
        operation.putOperationData(RestContains.PARENTID, parentId);
        operation.putOperationData(RestContains.SORTBY_KEY,
                RestContains.SORTBY_TIME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + snapshotName);
        operation.putOperationData(RestContains.VSORTBY_ID, vstoreId);

        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find FilesystemSnapshot for Name: "
                            + snapshotName);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one FilesystemSnapshot for Name: "
                            + snapshotName);
        }
        FilesystemSnapshot filesystemsh = (FilesystemSnapshot) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0),
                        FilesystemSnapshot.class,
                        storageArrayInfo);
        return filesystemsh;
    }

    /**
     * Rooll back filesystem snapshot
     * @param storageArrayInfo OceanStor info
     * @param id Filesystem snapshot id
     * @throws RestException
     */
    public void rollbackFilesystemSnapshot(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.ROLLBACK_FILESYSTEM_SNAPSHOT);
        operation.putOperationData(RestContains.ID, id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify filesystem snapshot
     * @param storageArrayInfo OceanStor info
     * @param id Filesystem id
     * @param name Filesystem name
     * @throws RestException
     */
    public void modifyFilesystemSnapshot(StorageArrayInfo storageArrayInfo,
        String id, String name) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.UPDATE_FILESYSTEM_SNAPSHOT);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.ID, id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get filesystem snapshots by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @param parentId Filesystem id
     * @param parentName Filesystem name
     * @return List<FilesystemSnapshot>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<FilesystemSnapshot> getFilesystemSnapshotsByRange(
        StorageArrayInfo storageArrayInfo, int begin, int end, String parentId,
        String parentName) throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<FilesystemSnapshot> list = performFilesystemSnapshotsByRange(
                storageArrayInfo, begin, end, parentId, parentName);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getFsSnapshots() == null) {
                    array.setFsSnapshots(new Vector<FilesystemSnapshot>());
                } else {
                    array.getFsSnapshots().clear();
                }
                array.getFsSnapshots().addAll(list);
            }
        }
        return list;
    }

    /**
     * Perform filesystem snapshot by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @param parentId Filesystem id
     * @param parentName Filesystem name
     * @return List<FilesystemSnapshot>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<FilesystemSnapshot> performFilesystemSnapshotsByRange(
        StorageArrayInfo storageArrayInfo, int begin, int end, String parentId,
        String parentName) throws StorageCommonException, RestException {
        String real_id = filesystemManager.adjustId(storageArrayInfo,
                parentId,
                parentName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_FILESYSTEM_SNAPSHOT_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        operation.putOperationData(RestContains.PARENTID, real_id);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<FilesystemSnapshot> filesystemSnapshots = new ArrayList<FilesystemSnapshot>();
        for (Map<String, String> map : list) {
            FilesystemSnapshot filesystemsh = (FilesystemSnapshot) ObjectsBuilder.INSTANCE
                    .buildObject(map,
                            FilesystemSnapshot.class,
                            storageArrayInfo);
            filesystemSnapshots.add(filesystemsh);
        }
        return filesystemSnapshots;
    }

    /**
     * Get Filesystem count
     * @param storageArrayInfo OceanStor info
     * @param parentId Filesystem id
     * @param parentName Filesystem name
     * @return count
     * @throws RestException
     * @throws StorageCommonException
     */
    public String geFilesystemSnapshotCount(StorageArrayInfo storageArrayInfo, String parentId, String parentName)
        throws RestException, StorageCommonException {
        String real_id = filesystemManager.adjustId(storageArrayInfo,
                parentId,
                parentName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_FILESYSTEM_SNAPSHOT_COUNT);
        operation.putOperationData(RestContains.PARENTID, real_id);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);

        return count;
    }
}
