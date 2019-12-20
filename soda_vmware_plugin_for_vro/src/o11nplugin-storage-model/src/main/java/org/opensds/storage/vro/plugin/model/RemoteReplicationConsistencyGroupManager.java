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
import org.apache.log4j.Logger;

public class RemoteReplicationConsistencyGroupManager {
    private static final Logger logger = Logger.getLogger(RemoteReplicationConsistencyGroupManager.class);

    /**
     * Create remote replication pair and remote replication consistent group associate
     * @param storageArrayInfo OceanStor info
     * @param name Remote replication consistent group name
     * @param description Remote replication description
     * @param recoveryPolicy Remote replication pair and consistent group recover policy
     * @param replicationModel Remote replication pair and consistent group replication model
     * @param speed Remote replication pair and consistent group replication speed
     * @param synchronizeType Remote replication pair and consistent group synchronize type
     * @param timingVal Remote replication pair and consistent group Timing value
     * @param timingValInsec Remote replication pair and consistent group Timing value insec
     * @param enableCompress Remote replication pair and consistent group enable compress or not
     * @return RemoteReplicationConsistencyGroup
     * @throws RestException
     * @throws StorageCommonException
     */
    public RemoteReplicationConsistencyGroup createRemoteReplicationConsistencyGroup(StorageArrayInfo storageArrayInfo,
        String name, String description, String recoveryPolicy, String replicationModel, String speed,
        String synchronizeType, String timingVal, String timingValInsec, String enableCompress)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_REMOTE_REPLICATION_CONSISTENCY_GROUP);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.TIMINGVALINSEC, timingValInsec);
        operation.putOperationData(RestContains.SYNCHRONIZETYPE, synchronizeType);
        operation.putOperationData(RestContains.RECOVERYPOLICY, recoveryPolicy);
        operation.putOperationData(RestContains.SPEED, speed);
        operation.putOperationData(RestContains.TIMINGVALINSEC, timingVal);
        operation.putOperationData(RestContains.REPLICATIONMODEL, replicationModel);
        operation.putOperationData(RestContains.ENABLECOMPRESS, enableCompress);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        RemoteReplicationConsistencyGroup group = (RemoteReplicationConsistencyGroup) ObjectsBuilder.INSTANCE
                .buildObject(list.get(0), RemoteReplicationConsistencyGroup.class, storageArrayInfo);
        return group;
    }

    /**
     * Delete Remote replication pair and consistent group
     * @param storageArrayInfo OceanStor info
     * @param isLocalDelete isLocalDelete
     * @param id Remote replication consistent group id
     * @throws RestException
     */
    public void deleteRemoteReplicationConsistencyGroup(StorageArrayInfo storageArrayInfo, String isLocalDelete,
        String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_REMOTE_REPLICATION_CONSISTENCY_GROUP);
        operation.putOperationData(RestContains.ISLOCALDELETE, isLocalDelete);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Split remote replication consistent group
     * @param storageArrayInfo OceanStor info
     * @param id Remote replication consistent group id
     * @throws RestException
     */
    public void splitRemoteReplicationConsistencyGroup(StorageArrayInfo storageArrayInfo, String id)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.SPLIT_REMOTE_REPLICATION_CONSISTENCY_GROUP);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Synchronize remote replication consistent group
     * @param storageArrayInfo OceanStor info
     * @param id Remote replication consistent group id
     * @throws RestException
     */
    public void syncRemoteReplicationConsistencyGroup(StorageArrayInfo storageArrayInfo, String id)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.SYNC_REMOTE_REPLICATION_CONSISTENCY_GROUP);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get remote replication consistent group by name
     * @param storageArrayInfo OceanStor info
     * @param groupName Remote replication consistent group id
     * @return RemoteReplicationConsistencyGroup
     * @throws StorageCommonException
     * @throws RestException
     */
    public RemoteReplicationConsistencyGroup getRemoteReplicationConsistencyGroupByName
        (StorageArrayInfo storageArrayInfo, String groupName) throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_REMOTE_REPLICATION_CONSISTENCY_GROUP_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, "NAME:" + groupName);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> performAction = OperationUtil.performAction(storageArrayInfo, operation);
        if (performAction.size() == 0) {
            throw new StorageCommonException(
                    "can not find Remote Replication Consistency Group By groupName: " + groupName);
        }
        if (performAction.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one Remote Replication Consistency Group for groupName: " + groupName);
        }
        RemoteReplicationConsistencyGroup consistencyGroup =
                (RemoteReplicationConsistencyGroup) ObjectsBuilder.INSTANCE.buildObject(performAction.get(0),
                RemoteReplicationConsistencyGroup.class, storageArrayInfo);
        return consistencyGroup;
    }

    /**
     * Add remote replication pair to consistent group
     * @param storageArrayInfo OceanStor info
     * @param id Remote replication consistent group id
     * @param rmList Remote replication pair id list
     * @throws RestException
     */
    public void addPairToRemoteReplicationConsistencyGroup(StorageArrayInfo storageArrayInfo, String id, String rmList)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.ADD_PAIR_TO_REPLICATION_CONSISTENCY_GROUP);
        operation.putOperationData(RestContains.ID, id);
        List<String> rmLists = new ArrayList<String>();
        rmLists.add(rmList);
        operation.putOperationDataObject(RestContains.RMLIST, rmLists);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get remote replication consistent group by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<RemoteReplicationConsistencyGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<RemoteReplicationConsistencyGroup> getConsistentGroupByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_REMOTE_REPLICATION_CONSISTENCY_GROUP_BY_NAME);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<RemoteReplicationConsistencyGroup> groups = new ArrayList<RemoteReplicationConsistencyGroup>();
        for (Map<String, String> map : list) {
            RemoteReplicationConsistencyGroup group =
                    (RemoteReplicationConsistencyGroup) ObjectsBuilder.INSTANCE.buildObject(map,
                    RemoteReplicationConsistencyGroup.class, storageArrayInfo);
            groups.add(group);
        }
        return groups;
    }

}
