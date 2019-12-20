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
import org.opensds.storage.vro.OpenSDSStorageRepository;
import org.apache.log4j.Logger;

public class RemoteReplicationPairManager {
    private static final Logger logger = Logger.getLogger(RemoteReplicationPairManager.class);

    private LUNManager lunManager = new LUNManager();

    /**
     * Create remote replication pair
     * @param storageArrayInfo OceanStor info
     * @param localResName Local LUN or filesystem name
     * @param localResType LUN or filesystem
     * @param remoteDeviceName Remote device name
     * @param remoteResName Remote LUN or filesystem name
     * @param synchronizeType Remote replication pair synchronize type
     * @param recoveryPolicy Remote replication pair recover policy
     * @param speed Remote replication pair speed
     * @param timingVal Remote replication pair timing value
     * @param replicationModel Remote replication pair replication model
     * @param isDataSync Remote replication pair is date synchronize
     * @param enableCompress Remote replication pair enable compress or not
     * @param remTimeoutPeriod Remote replication pair remtime out preiod
     * @param synchronizeSchedule Remote replication pair synchronize schedule
     * @return RemoteReplicationPait
     * @throws RestException
     * @throws StorageCommonException
     */
    public RemoteReplicationPair createRemoteReplicationPair(StorageArrayInfo storageArrayInfo, String localResName,
        String localResType, String remoteDeviceName, String remoteResName, String synchronizeType,
        String recoveryPolicy, String speed, String timingVal, String replicationModel, String isDataSync,
        String enableCompress, String remTimeoutPeriod, String synchronizeSchedule)
        throws RestException, StorageCommonException {
        RemoteDevice device = getRemoteDeviceByName(storageArrayInfo, remoteDeviceName);
        if (!RestContains.NORMAL.equals(device.getHealthstatus()) || !RestContains.LINK_UP.equals(
                device.getRunningstatus())) {
            throw new StorageCommonException("The Remote Device Status Error");
        }
        LUN localLun = lunManager.getLUNByName(storageArrayInfo, localResName);
        StorageArrayInfo remoteArray = getStorageArrayInfoBySN(device.getSn());
        LUN remoteLun = lunManager.getLUNByName(remoteArray, remoteResName);
        if (null == remoteLun) {
            throw new StorageCommonException("Can not find Remote LUN!");
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CEREATE_REMOTE_REPLICATION_PAIR);
        operation.putOperationData(RestContains.LOCALRESID, localLun.getReal_id());
        operation.putOperationData(RestContains.LOCALRESTYPE, localResType);
        operation.putOperationData(RestContains.REMOTEDEVICEID, device.getReal_id());
        operation.putOperationData(RestContains.REMOTERESID, remoteLun.getReal_id());
        operation.putOperationData(RestContains.SYNCHRONIZETYPE, synchronizeType);
        operation.putOperationData(RestContains.RECOVERYPOLICY, recoveryPolicy);
        operation.putOperationData(RestContains.SPEED, speed);
        operation.putOperationData(RestContains.TIMINGVAL, timingVal);
        operation.putOperationData(RestContains.REPLICATIONMODEL, replicationModel);
        operation.putOperationData(RestContains.ISDATASYNC, isDataSync);
        operation.putOperationData(RestContains.ENABLECOMPRESS, enableCompress);
        operation.putOperationData(RestContains.REMTIMEOUTPERIOD, remTimeoutPeriod);
        operation.putOperationData(RestContains.SYNCHRONIZESCHEDULE, synchronizeSchedule);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);

        RemoteReplicationPair remoteReplicationPair = (RemoteReplicationPair) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), RemoteReplicationPair.class, storageArrayInfo);
        return remoteReplicationPair;
    }

    /**
     * Get remote replication pair by consistent group id
     * @param storageArrayInfo OceanStor info
     * @param groupId Remote replication consistent group id
     * @return List<RemoteReplicationPair>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<RemoteReplicationPair> getRemoteReplicationPairByConsistencyGroupId(StorageArrayInfo storageArrayInfo,
        String groupId) throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        List<RemoteReplicationPair> pairs = new ArrayList<RemoteReplicationPair>();
        operation.setOperationName(OperationNamesEnum.GET_REMOTE_REPLICATION_PAIR_BY_GROUPID);
        operation.putOperationData(RestContains.FILTER_KEY, "CGID::" + groupId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> performAction = OperationUtil.performAction(storageArrayInfo, operation);
        for (Map<String, String> map : performAction) {
            RemoteReplicationPair pair = (RemoteReplicationPair) ObjectsBuilder.INSTANCE.buildObject(
                    map, RemoteReplicationPair.class, storageArrayInfo);
            pairs.add(pair);
        }
        return pairs;
    }

    /**
     * Split Remote replication pair
     * @param storageArrayInfo OceanStor info
     * @param id Remote replication pair id
     * @throws RestException
     */
    public void splitRemoteReplicationPair(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.SPLIT_REMOTE_REPLICATION_PAIR);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get remote replication pair by lun id
     * @param storageArrayInfo OceanStor info
     * @param id LUN id
     * @return RemoteReplicationPair
     * @throws StorageCommonException
     * @throws RestException
     */
    public RemoteReplicationPair getRemoteReplicationPairByLUNId(StorageArrayInfo storageArrayInfo, String id)
        throws StorageCommonException, RestException {
        List<RemoteReplicationPair> pairs = new ArrayList<RemoteReplicationPair>();
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_REMOTER_EPLICATION_PAIR_BY_LUNID);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_LUN);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, id);
        List<Map<String, String>> performAction = OperationUtil.performAction(storageArrayInfo, operation);
        if (performAction.size() == 0) {
            throw new StorageCommonException("can not find Pair:" + id);
        }
        if (performAction.size() > 1) {
            throw new StorageCommonException("there is not only one Pair for LUN: " + id);
        }
        RemoteReplicationPair pair = (RemoteReplicationPair) ObjectsBuilder.INSTANCE.buildObject(performAction.get(0),
                RemoteReplicationPair.class, storageArrayInfo);
        return pair;
    }

    /**
     * Delete remote replication pair
     * @param storageArrayInfo OceanStor info
     * @param isLocalDelete isLocalDelete
     * @param toSyncsrWhenDelete toSyncsrWhenDelete
     * @param id Remote replication pair id
     * @throws RestException
     */
    public void deleteRemoteReplicationPair(StorageArrayInfo storageArrayInfo, String isLocalDelete,
        String toSyncsrWhenDelete, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_REMOTER_EPLICATION_PAIR);
        operation.putOperationData(RestContains.ISLOCALDELETE, isLocalDelete);
        operation.putOperationData(RestContains.TOSYNCSRWHENDELETE, toSyncsrWhenDelete);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get remote device by device name
     * @param storageArrayInfo OceanStor info
     * @param RemoteDeviceName Remote device name
     * @return RemoteDevice
     * @throws RestException
     * @throws StorageCommonException
     */
    public RemoteDevice getRemoteDeviceByName(StorageArrayInfo storageArrayInfo, String RemoteDeviceName)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_RemoteDevice_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + RemoteDeviceName);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find Remote Device:" + RemoteDeviceName);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one Remote Device for Name: " + RemoteDeviceName);
        }
        RemoteDevice remoteDevice = (RemoteDevice) ObjectsBuilder.INSTANCE.buildObject(list.get(0), RemoteDevice.class,
                storageArrayInfo);
        return remoteDevice;
    }

    private StorageArrayInfo getStorageArrayInfoBySN(String SN) throws StorageCommonException {
        List<StorageArrayInfo> arrays = OpenSDSStorageRepository.getUniqueInstance().getStorageArrayInfos();
        StorageArrayInfo arrayInfo = null;
        if (null != arrays) {
            for (StorageArrayInfo array : arrays) {
                if (logger.isInfoEnabled()) {
                    logger.info("The array SN is:" + array.getProductSN());
                }
                if (SN.equals(array.getProductSN())) {
                    arrayInfo = array;
                    break;
                }
            }
        }
        if (null == arrayInfo) {
            throw new StorageCommonException("Do not find Array By SN: " + SN);
        }
        return arrayInfo;
    }

}
