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

public class NFSShareManager {
    private FilesystemManager filesystemManager = new FilesystemManager();

    /**
     * Get NFSS shre by NFSS share id or name
     * @param storageArrayInfo OceanStor info
     * @param id NFSS share id
     * @param path NFSS share name
     * @return NFSS share id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String path) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            NFSShare nfsShare = getNFSShareBySharePath(storageArrayInfo, path);
            real_id = nfsShare.getReal_id();
        }
        return real_id;
    }

    /**
     * Create NFSS share
     * @param storageArrayInfo OceanStor info
     * @param filesystemName Filesystem name
     * @param sharepath NFSS share path
     * @param description MFSS share description
     * @param encoding NFSS share encoding
     * @throws StorageCommonException
     * @throws RestException
     */
    public void createNFSShare(StorageArrayInfo storageArrayInfo, String filesystemName, String sharepath,
        String description, String encoding) throws StorageCommonException, RestException {
        String path = null;
        if (sharepath == null || sharepath.trim().length() == 0) {
            Filesystem fs = filesystemManager
                    .getFilesystemByName(storageArrayInfo, filesystemName);
            path = "/" + fs.getName() + "/";
        } else {
            path = sharepath;
        }

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_NFS_SHARE);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.SHAREPATH, path);
        operation.putOperationData(RestContains.CHARACTERENCODING, encoding);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get NFSS share by share path
     * @param storageArrayInfo OceanStor info
     * @param sharePath NFSS share path
     * @return NFSSshare
     * @throws RestException
     * @throws StorageCommonException
     */
    public NFSShare getNFSShareBySharePath(StorageArrayInfo storageArrayInfo, String sharePath)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_NFS_SHARE_BY_PATH);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_SHAREPATH_PARAMETER + sharePath);
        operation.putOperationData(RestContains.RANGE,
                RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find Filesystem for path: " + sharePath);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one Filesystem for path: " + sharePath);
        }
        NFSShare nfsShare = (NFSShare) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), NFSShare.class, storageArrayInfo);
        return nfsShare;
    }

    /**
     * Delete NFSS share
     * @param storageArrayInfo OceanStor info
     * @param id NFSS share id
     * @param sharePath NFSS share path
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteNFSShare(StorageArrayInfo storageArrayInfo, String id, String sharePath)
        throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, sharePath);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_NFS_SHARE_BY_ID);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify NFSS share
     * @param storageArrayInfo OceanStor info
     * @param id NFSS share id
     * @param sharePath NFSS share path
     * @param description NFSS share description
     * @param encoding NFSS share encoding
     * @throws RestException
     * @throws StorageCommonException
     */
    public void modifyNFSShare(StorageArrayInfo storageArrayInfo, String id, String sharePath, String description,
        String encoding) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, id, sharePath);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_NFS_SHARE_BY_ID);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.CHARACTERENCODING, encoding);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get NFSS count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getNFSShareTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_NFS_SHARE_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);

        return count;
    }

    /**
     * Get NFSS share by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<NFSShare>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<NFSShare> getNFSSharesByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<NFSShare> nfsShares = performGetNFSSharesByRange(storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getNfsShares() == null) {
                    array.setNfsShares(new Vector<NFSShare>());
                } else {
                    array.getNfsShares().clear();
                }
                array.getNfsShares().addAll(nfsShares);
            }
        }
        return nfsShares;
    }

    /**
     * Perform get NFSS share by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<NFSShare>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<NFSShare> performGetNFSSharesByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_NFS_SHARE_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<NFSShare> nfslist = new ArrayList<NFSShare>();
        for (Map<String, String> map : list) {
            NFSShare nfs = (NFSShare) ObjectsBuilder.INSTANCE.buildObject(map,
                    NFSShare.class,
                    storageArrayInfo);
            nfslist.add(nfs);
        }
        return nfslist;
    }

    /**
     * Create NFSS share client
     * @param storageArrayInfo OceanStor info
     * @param name NFSS share name
     * @param sharePath NFSS share path
     * @param accessval NFSS share accessval
     * @param sync NFSS share sync
     * @param allsquash NFSS share all squash
     * @param rootSquash NFSS share root squash
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createNFSShareClient(StorageArrayInfo storageArrayInfo, String name, String sharePath,
        String accessval, String sync, String allsquash, String rootSquash)
        throws RestException, StorageCommonException {
        String parentId = getNFSShareBySharePath(storageArrayInfo, sharePath)
                .getReal_id();
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_NFS_SHARE_CLIENT);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.PARENTID, parentId);
        operation.putOperationData(RestContains.ACCESSVAL, accessval);
        operation.putOperationData(RestContains.SYNC, sync);
        operation.putOperationData(RestContains.ALLSQUASH, allsquash);
        operation.putOperationData(RestContains.ROOTSQUASH, rootSquash);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get NFSS share client id by NFSS share client name
     * @param storageArrayInfo OceanStor info
     * @param name NFSS share client name
     * @return NFSS share client id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String getNFSShareClientIdByName(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_NFS_SHARE_CLIENT_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        operation.putOperationData(RestContains.RANGE,
                RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find  NFSShareClient for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one NFSShareClient for Name: " + name
                            + ", please use id instead of name");
        }
        return list.get(0).get(RestContains.ID);
    }

    /**
     * Delete NFSS share client
     * @param storageArrayInfo OceanStor info
     * @param id OceanStor id
     * @param name OceanStor name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteNFSShareClient(StorageArrayInfo storageArrayInfo,
        String id, String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            real_id = getNFSShareClientIdByName(storageArrayInfo, name);
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.DELETE_NFS_SHARE_CLIENT_BY_ID);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }
}
