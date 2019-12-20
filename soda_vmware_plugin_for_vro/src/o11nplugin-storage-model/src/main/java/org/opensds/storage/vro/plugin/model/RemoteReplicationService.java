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

import org.opensds.storage.conection.rest.exception.RestException;

import java.util.List;

import org.apache.log4j.Logger;

public class RemoteReplicationService {
    private static final Logger log = Logger.getLogger(RemoteReplicationService.class);

    private RemoteReplicationPairManager remoteReplicationPairManager = new RemoteReplicationPairManager();

    private RemoteReplicationConsistencyGroupManager remoteReplicationConsistencyGroupManager =
            new RemoteReplicationConsistencyGroupManager();

    /**
     * Create remote replication pair and consistent group associate
     * @param storagearrayInfo OceanStor info
     * @param localResName Local LUN or filesystem name
     * @param localRestype LUN or filesystem
     * @param remoteDeviceName Remote device name
     * @param remoteResName Remote LUN or filesystem name
     * @param synchronizeType Remote replication pair and consistent group synchronize type
     * @param recoveryPolicy Remote replication pair and consistent group recover policy
     * @param speed Remote replication pair and consistent group speed
     * @param timingVal Remote replication pair and consistent group timing value
     * @param replicationModel Remote replication pair and consistent group replication model
     * @param isDataSync Remote replication pair and consistent group data synchronize or not
     * @param enableCompress Remote replication pair and consistent group enable compress or not
     * @param remtimeoutPeriod Remote replication pair and consistent group remtimeout period
     * @param synchronizeSchedule Remote replication pair and consistent group synchronize schedule
     * @param name Remote replication consistent group name
     * @param description Remote replication consistent group description
     * @param toSyncsrWhendelete Remote replication pair and consistent group toSyncsrWhendelete
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createRemoteReplicationPairAndConsistencyGroup(StorageArrayInfo storagearrayInfo, String localResName,
        String localRestype, String remoteDeviceName, String remoteResName, String synchronizeType,
        String recoveryPolicy, String speed, String timingVal, String replicationModel, String isDataSync,
        String enableCompress, String remtimeoutPeriod, String synchronizeSchedule, String name, String description,
        String isLocalDelete, String toSyncsrWhendelete) throws RestException, StorageCommonException {
        if (null == toSyncsrWhendelete) {
            toSyncsrWhendelete = "true";
        }
        RemoteReplicationPair remoteReplicationPair = remoteReplicationPairManager.createRemoteReplicationPair(
                storagearrayInfo, localResName, localRestype, remoteDeviceName, remoteResName, synchronizeType,
                recoveryPolicy, speed, timingVal, replicationModel, isDataSync, enableCompress, remtimeoutPeriod,
                synchronizeSchedule);
        RemoteReplicationConsistencyGroup remoteReplicationConsistencyGroup = null;
        if (null != remoteReplicationPair && null != remoteReplicationPair.getReal_id()) {
            try {
                String timingValue = null;
                String timingValInsec = null;
                if (null != timingVal && timingVal.length() != 0) {
                    int timing = Integer.valueOf(timingVal);
                    timingValue = (timing >= 60) ? String.valueOf(timing / 60) : null;
                    timingValInsec = (timing < 60) ? timingVal : null;
                }
                remoteReplicationConsistencyGroup =
                        remoteReplicationConsistencyGroupManager.createRemoteReplicationConsistencyGroup(
                        storagearrayInfo, name, description, recoveryPolicy, replicationModel, speed,
                        synchronizeType, timingValue, timingValInsec, enableCompress);
            } catch (Exception e) {
                remoteReplicationPairManager.deleteRemoteReplicationPair(storagearrayInfo, isLocalDelete,
                        toSyncsrWhendelete, remoteReplicationPair.getReal_id());
                throw new StorageCommonException("Create replication consistent group failed");
            }
            if (null != remoteReplicationConsistencyGroup && null != remoteReplicationConsistencyGroup.getReal_id()) {
                try {
                    if (log.isInfoEnabled()) {
                        log.info(remoteReplicationConsistencyGroup + "," + remoteReplicationPair);
                    }
                    remoteReplicationConsistencyGroupManager.addPairToRemoteReplicationConsistencyGroup(
                            storagearrayInfo, remoteReplicationConsistencyGroup.getReal_id(),
                            remoteReplicationPair.getReal_id());
                } catch (RestException e) {
                    if (!RestContains.SPLITED.equals(remoteReplicationPair.getRunningstatus())) {
                        remoteReplicationPairManager.splitRemoteReplicationPair(storagearrayInfo,
                                remoteReplicationPair.getReal_id());
                    }
                    remoteReplicationPairManager.deleteRemoteReplicationPair(storagearrayInfo, isLocalDelete,
                            toSyncsrWhendelete, remoteReplicationPair.getReal_id());
                    remoteReplicationConsistencyGroupManager.deleteRemoteReplicationConsistencyGroup(
                            storagearrayInfo, isLocalDelete, remoteReplicationConsistencyGroup.getReal_id());
                    throw e;
                }
            } else {
                if (!RestContains.SPLITED.equals(remoteReplicationPair.getRunningstatus())) {
                    remoteReplicationPairManager.splitRemoteReplicationPair(storagearrayInfo,
                            remoteReplicationPair.getReal_id());
                }
                remoteReplicationPairManager.deleteRemoteReplicationPair(storagearrayInfo, isLocalDelete,
                        toSyncsrWhendelete, remoteReplicationPair.getReal_id());
            }
        } else {
            throw new StorageCommonException("Create Remote Replication pair failed");
        }
        try {
            remoteReplicationConsistencyGroupManager.syncRemoteReplicationConsistencyGroup(
                    storagearrayInfo, remoteReplicationConsistencyGroup.getReal_id());
        } catch (Exception e) {
            deleteRemoteReplicationPairAndConsistencyGroup(storagearrayInfo,
                    remoteReplicationConsistencyGroup.getName(), isLocalDelete, toSyncsrWhendelete);
            throw new StorageCommonException("Synchronize replication consistent group failed");
        }
    }

    /**
     * Delete remote replication
     * @param storagearrayInfo OceanStor info
     * @param consistencyGroupName Remote replication consistent group name
     * @param isLocalDelete isLocalDelete
     * @param toSyncsrWhendelete toSyncsrWhendelete
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteRemoteReplicationPairAndConsistencyGroup(StorageArrayInfo storagearrayInfo,
        String consistencyGroupName, String isLocalDelete, String toSyncsrWhendelete) throws RestException,
        StorageCommonException {
        if (null == toSyncsrWhendelete) {
            toSyncsrWhendelete = "true";
        }
        RemoteReplicationConsistencyGroup consistencyGroup =
                remoteReplicationConsistencyGroupManager.getRemoteReplicationConsistencyGroupByName(
                storagearrayInfo, consistencyGroupName);
        if (!checkifRemoteReplicationCanDeleted(consistencyGroup.getRunningstatus())) {
            remoteReplicationConsistencyGroupManager.splitRemoteReplicationConsistencyGroup(
                storagearrayInfo, consistencyGroup.getReal_id());
        }
        List<RemoteReplicationPair> remoteReplicationPairs =
                remoteReplicationPairManager.getRemoteReplicationPairByConsistencyGroupId(
                storagearrayInfo, consistencyGroup.getReal_id());
        while (true) {
            consistencyGroup = remoteReplicationConsistencyGroupManager.getRemoteReplicationConsistencyGroupByName(
                    storagearrayInfo, consistencyGroupName);
            if (checkifRemoteReplicationCanDeleted(consistencyGroup.getRunningstatus())) {
                remoteReplicationConsistencyGroupManager.deleteRemoteReplicationConsistencyGroup(storagearrayInfo,
                        isLocalDelete, consistencyGroup.getReal_id());
                break;
            }
        }
        for (RemoteReplicationPair remoteReplicationPair : remoteReplicationPairs) {
            if (!checkifRemoteReplicationCanDeleted(remoteReplicationPair.getRunningstatus())) {
                remoteReplicationPairManager.splitRemoteReplicationPair(storagearrayInfo,
                        remoteReplicationPair.getReal_id());
            }
            remoteReplicationPairManager.deleteRemoteReplicationPair(storagearrayInfo, isLocalDelete,
                    toSyncsrWhendelete, remoteReplicationPair.getReal_id());
        }
    }

    /**
     * Split remote replication consistent group
     * @param storagearrayInfo OceanStor info
     * @param consistencyGroupName Remote replication consistent group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void splitRemoteReplicationConsistencyGroup(StorageArrayInfo storagearrayInfo, String consistencyGroupName)
        throws RestException, StorageCommonException {
        RemoteReplicationConsistencyGroup consistencyGroup =
                remoteReplicationConsistencyGroupManager.getRemoteReplicationConsistencyGroupByName(
                storagearrayInfo, consistencyGroupName);
        remoteReplicationConsistencyGroupManager.splitRemoteReplicationConsistencyGroup(storagearrayInfo,
                consistencyGroup.getReal_id());
    }

    /**
     * Synchronize remote replication consistent group
     * @param storagearrayInfo OceanStor info
     * @param consistencyGroupName Remote replication consistent group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void syncRemoteReplicationConsistencyGroup(StorageArrayInfo storagearrayInfo, String consistencyGroupName)
            throws RestException, StorageCommonException {
        RemoteReplicationConsistencyGroup consistencyGroup =
                remoteReplicationConsistencyGroupManager.getRemoteReplicationConsistencyGroupByName(
                storagearrayInfo, consistencyGroupName);
        remoteReplicationConsistencyGroupManager.syncRemoteReplicationConsistencyGroup(
                storagearrayInfo, consistencyGroup.getReal_id());
    }

    private boolean checkifRemoteReplicationCanDeleted(String runningStatus) {
        // Only runningstatus is 26:splited , 34:interrupted or 35:invalid, RemoteConsistencyGroup can be deleted.
        if (runningStatus.equalsIgnoreCase("26") || runningStatus.equalsIgnoreCase("34")
                || runningStatus.equalsIgnoreCase("35")) {
            return true;
        } else {
            return false;
        }
    }
}
