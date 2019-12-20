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

public class FilesystemManager {
    private StoragePoolManager storagePoolManager = new StoragePoolManager();

    /**
     * get FileSystem id by name
     * @param storageArrayInfo OceanStor info
     * @param id FileSystem id
     * @param name FileSystem name
     * @return Filesystem id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws StorageCommonException, RestException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            Filesystem filesystem = getFilesystemByName(storageArrayInfo, name);
            real_id = filesystem.getReal_id();
        }
        return real_id;
    }

    /**
     * Create filesystem
     * @param storageArrayInfo OceanStor info
     * @param name Filesystem info
     * @param description Filesystem description
     * @param poolId Storage pool id
     * @param poolName Storage pool name
     * @param capacity Filesystem capacity
     * @param alloctype Filesystem alloctype
     * @param sectorsize Filesystem sector size
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createFilesystem(StorageArrayInfo storageArrayInfo, String name,
        String description, String poolId, String poolName, long capacity,
        String alloctype, String sectorsize) throws RestException, StorageCommonException {
        String real_parentId = storagePoolManager.adjustId(storageArrayInfo, poolId, poolName);

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_FILESYSTEM);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.CAPACITY,
                String.valueOf(capacity * RestContains.GB_TO_SECTOR));
        operation.putOperationData(RestContains.PARENTID, real_parentId);
        operation.putOperationData(RestContains.ALLOCTYPE, alloctype);
        operation.putOperationData(RestContains.SECTORSIZE, sectorsize);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete filesystem
     * @param storageArrayInfo OceanStor info
     * @param name Filesystem name
     * @param id Filesystem id
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteFilesystem(StorageArrayInfo storageArrayInfo, String name,
        String id) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, name);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_FILESYSTEM);
        operation.putOperationData(RestContains.ID, real_id);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify filesystem
     * @param storageArrayInfo OceanStor info
     * @param id Filesystem id
     * @param currentName Filesystem current name
     * @param newName Filesystem new name
     * @param newDescription Filesystem description
     * @param newCapacity Filesystem capacity
     * @throws RestException
     * @throws StorageCommonException
     */
    public void modifyFilesystem(StorageArrayInfo storageArrayInfo, String id,
        String currentName, String newName, String newDescription,
        long newCapacity) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, currentName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_FILESYSTEM);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.NAME, newName);
        operation.putOperationData(RestContains.DESCRIPTION, newDescription);
        if (newCapacity != 0) {
            operation.putOperationData(RestContains.CAPACITY,
                    String.valueOf(newCapacity * RestContains.GB_TO_SECTOR));
        }

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get filesystem count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getFilesystemTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_FILESYSTEMS_TOTAL_COUNT);

        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Perform get filesystem by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<Filesystem>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<Filesystem> performGetFilesystemsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_FILESYSTEMS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<Filesystem> filesystems = new ArrayList<Filesystem>();
        for (Map<String, String> map : list) {
            Filesystem filesystem = (Filesystem) ObjectsBuilder.INSTANCE
                    .buildObject(map, Filesystem.class, storageArrayInfo);
            filesystems.add(filesystem);
        }
        return filesystems;
    }

    /**
     * Get filesystem by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<Filesystem>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<Filesystem> getFilesystemsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<Filesystem> filesystems = performGetFilesystemsByRange(
                storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getFilesystems() == null) {
                    array.setFilesystems(new Vector<Filesystem>());
                } else {
                    array.getFilesystems().clear();
                }
                array.getFilesystems().addAll(filesystems);
            }
        }
        return filesystems;
    }

    /**
     * Get filesystem by name
     * @param storageArrayInfo OceanStor info
     * @param name Filesystem name
     * @return Filesystem
     * @throws StorageCommonException
     * @throws RestException
     */
    public Filesystem getFilesystemByName(StorageArrayInfo storageArrayInfo,
        String name) throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_FILESYSTEM_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find Filesystem for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one Filesystem for Name: " + name);
        }
        Filesystem filesystem = (Filesystem) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), Filesystem.class, storageArrayInfo);
        return filesystem;
    }
}
