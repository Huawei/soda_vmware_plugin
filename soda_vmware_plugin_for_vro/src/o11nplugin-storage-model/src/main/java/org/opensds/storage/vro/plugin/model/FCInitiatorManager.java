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

public class FCInitiatorManager {
    HostManager hostManager = new HostManager();

    /**
     * Create FC initiator
     * @param storageArrayInfo OceanStor info
     * @param fcWWN FC wwn
     * @param fcName FC name
     * @param failoverMode Initiator switchover mode
     * @param pathType Initiator path type
     * @param multipathtype Multipathing type
     * @throws RestException
     */
    public void createFCINitiator(StorageArrayInfo storageArrayInfo,
        String fcWWN, String fcName, String failoverMode, String pathType, String multipathtype) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_FC_INITIATOR);
        operation.putOperationData(RestContains.ID, fcWWN);
        operation.putOperationData(RestContains.NAME, fcName);
        operation.putOperationData(RestContains.FAILOVERMODE, failoverMode);
        operation.putOperationData(RestContains.PATHTYPE, pathType);
        operation.putOperationData(RestContains.MULTIPATHTYPE, multipathtype);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete FC initiator
     * @param storageArrayInfo OceanStor info
     * @param fcWWN FC initiator wwn
     * @throws RestException
     */
    public void deleteFCINitiator(StorageArrayInfo storageArrayInfo, String fcWWN) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_FC_INITIATOR);
        operation.putOperationData(RestContains.ID, fcWWN);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get FC initiator count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getFCINitiatorTotalCount(StorageArrayInfo storageArrayInfo)
            throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_FC_INITIATOR_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Get FC initiator by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<FCInitiator>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<FCInitiator> getFCInitiatorsByRange(
            StorageArrayInfo storageArrayInfo, int begin, int end)
            throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<FCInitiator> fCInitiators = performGetFCInitiatorsByRange(
                storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getfCInitiators() == null) {
                    array.setfCInitiators(new Vector<FCInitiator>());
                } else {
                    array.getfCInitiators().clear();
                }
                array.getfCInitiators().addAll(fCInitiators);
            }
        }

        return fCInitiators;
    }

    /**
     * Perform get FC initiator by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<FCInitiator>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<FCInitiator> performGetFCInitiatorsByRange(
            StorageArrayInfo storageArrayInfo, int begin, int end)
            throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation
                .setOperationName(OperationNamesEnum.GET_FC_INITIATOR_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<FCInitiator> fclist = new ArrayList<FCInitiator>();
        for (Map<String, String> map : list) {
            FCInitiator fc = (FCInitiator) ObjectsBuilder.INSTANCE
                    .buildObject(map, FCInitiator.class, storageArrayInfo);
            fclist.add(fc);
        }
        return fclist;
    }

    /**
     * add FC initiator to host
     * @param storageArrayInfo OceanStor info
     * @param fcWWN FC initiator wwn number
     * @param hostName Host name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void addFCInitiatortoHost(StorageArrayInfo storageArrayInfo, String fcWWN, String hostName)
            throws RestException, StorageCommonException {
        String parentId = hostManager.getHostByName(storageArrayInfo, hostName)
                .getReal_id();
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_FC_INITIATOR);
        operation.putOperationData(RestContains.ID, fcWWN);
        operation.putOperationData(RestContains.PARENTID, parentId);
        operation.putOperationData(RestContains.PARENTTYPE,
                RestContains.PARENTTYPE_HOST);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Remove FC initiator from host
     * @param storageArrayInfo OceanStor info
     * @param fcWWN FC wwn number
     * @throws RestException
     */
    public void removeFCInitiatorFromHost(StorageArrayInfo storageArrayInfo, String fcWWN) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.REMOVE_FC_INITIATOR_FROM_HOST);
        operation.putOperationData(RestContains.ID, fcWWN);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify FC initiator
     * @param storageArrayInfo OceanStor info
     * @param fcWWN FC wwn number
     * @param fcName FC initiator name
     * @param failoverMode Initiator switchover mode
     * @param pathType Initiator path type
     * @param multipathtype Multipathing type
     * @throws RestException
     */
    public void modifyFCInitiator(StorageArrayInfo storageArrayInfo, String fcWWN, String fcName, String multipathtype,
        String failoverMode, String pathType) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_FC_INITIATOR);
        operation.putOperationData(RestContains.ID, fcWWN);
        operation.putOperationData(RestContains.NAME, fcName);
        operation.putOperationData(RestContains.MULTIPATHTYPE, multipathtype);
        operation.putOperationData(RestContains.FAILOVERMODE, failoverMode);
        operation.putOperationData(RestContains.PATHTYPE, pathType);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get FC initiator
     * @param storageArrayInfo OceanStor info
     * @param wwn FC initiator wwn number
     * @return FCInitiator
     * @throws RestException
     * @throws StorageCommonException
     */
    public FCInitiator getFCInitiatorById(StorageArrayInfo storageArrayInfo,
        String wwn) throws RestException, StorageCommonException {
        try {
            OceanStorOperation operation = new OceanStorOperation();
            operation.setOperationName(
                    OperationNamesEnum.GET_FC_INITIATOR_BY_ID);
            operation.putOperationData(RestContains.ID, wwn);
            List<Map<String, String>> list = OperationUtil
                    .performAction(storageArrayInfo, operation);
            if (list.size() == 0) {
                throw new StorageCommonException("can not find FC Initiator:" + wwn);
            }
            if (list.size() > 1) {
                throw new StorageCommonException("there is not only one FC Initiator for wwn: " + wwn);
            }
            FCInitiator fcInitiator = (FCInitiator) ObjectsBuilder.INSTANCE.buildObject(list.get(0),
                    FCInitiator.class, storageArrayInfo);
            return fcInitiator;
        } catch (RestException e) {
            if (e.getErrorDescription().startsWith("The specified object is unavailable")) {
                return null;
            } else {
                throw e;
            }
        }
    }
}
