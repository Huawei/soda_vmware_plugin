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

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
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

public class iSCSIManager {
    private static final Logger logger = Logger.getLogger(iSCSIManager.class);

    HostManager hostManager = new HostManager();

    /**
     * Create ISCSI initiator
     * @param storageArrayInfo OceanStor info
     * @param iqn Initiator iqn number
     * @param name  Initiator name
     * @param usechap Initiator userchap
     * @param chapname Initiator chap name
     * @param chappassword Initiator chap password
     * @param chapnameinitatorvertarget Initiator chap name and initiator target
     * @param chappasswordinitatorvertarget Initiator chap password and initiator target
     * @param discoveryvermode Initiator discocer mode
     * @param normalvermode Initiator normal mode
     * @param failovermode Initiator failover mode
     * @param pathtype Initiator path type
     * @param multipathtype Initiator multopathtype
     * @throws RestException
     */
    public void createiSCSIINitiator(StorageArrayInfo storageArrayInfo,
        String iqn, String name, String usechap, String chapname, String chappassword, String chapnameinitatorvertarget,
        String chappasswordinitatorvertarget, String discoveryvermode, String normalvermode, String failovermode,
        String pathtype, String multipathtype) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_ISCSI_INITIATOR);
        operation.putOperationData(RestContains.ID, iqn);
        operation.putOperationData(RestContains.NAME, name);

        operation.putOperationData(RestContains.USECHAP, usechap);
        operation.putOperationData(RestContains.CHAPNAME, chapname);
        operation.putOperationData(RestContains.CHAPPAD, chappassword);
        operation.putOperationData(RestContains.CHAPNAMEINITATORVERTARGET,
                chapnameinitatorvertarget);
        operation.putOperationData(RestContains.CHAPPADINITATORVERTARGET,
                chappasswordinitatorvertarget);
        operation.putOperationData(RestContains.DISCOVERYVERMODE,
                discoveryvermode);
        operation.putOperationData(RestContains.NORMALVERMODE, normalvermode);

        operation.putOperationData(RestContains.MULTIPATHTYPE, multipathtype);
        operation.putOperationData(RestContains.FAILOVERMODE, failovermode);
        operation.putOperationData(RestContains.PATHTYPE, pathtype);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete ISCSI initiator
     * @param storageArrayInfo OceanStor info
     * @param iqn ISCSI initiator iqn number
     * @throws RestException
     */
    public void deleteiSCISINitiator(StorageArrayInfo storageArrayInfo, String iqn) throws RestException,
        UnsupportedEncodingException {
        OceanStorOperation operation = new OceanStorOperation();
        String id = URLEncoder.encode(iqn, "UTF-8");
        operation.setOperationName(OperationNamesEnum.DELETE_ISCSI_INITIATOR);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Create ISCSI initiator
     * @param storageArrayInfo OceanStor info
     * @param iqn Initiator iqn number
     * @param name  Initiator name
     * @param usechap Initiator userchap
     * @param chapname Initiator chap name
     * @param chappassword Initiator chap password
     * @param chapnameinitatorvertarget Initiator chap name and initiator target
     * @param chappasswordinitatorvertarget Initiator chap password and initiator target
     * @param discoveryvermode Initiator discocer mode
     * @param normalvermode Initiator normal mode
     * @param failovermode Initiator failover mode
     * @param pathtype Initiator path type
     * @param multipathtype Initiator multopathtype
     * @throws RestException
     */
    public void modifyiSCISINitiator(StorageArrayInfo storageArrayInfo, String iqn, String name, String usechap,
        String chapname, String chappassword, String chapoldpassword, String chapnameinitatorvertarget,
        String chappasswordinitatorvertarget, String chapoldpasswordinitatorvertarget, String discoveryvermode,
        String normalvermode, String failovermode, String pathtype, String multipathtype) throws RestException,
        UnsupportedEncodingException {
        OceanStorOperation operation = new OceanStorOperation();
        String id = URLEncoder.encode(iqn, "UTF-8");
        operation.setOperationName(OperationNamesEnum.MODIFY_ISCSI_INITIATOR);
        operation.putOperationData(RestContains.ID, id);
        operation.putOperationData(RestContains.NAME, name);

        operation.putOperationData(RestContains.USECHAP, usechap);

        operation.putOperationData(RestContains.CHAPNAME, chapname);
        operation.putOperationData(RestContains.CHAPPAD, chappassword);
        operation.putOperationData(RestContains.CHAPOLDPAD, chapoldpassword);

        operation.putOperationData(RestContains.CHAPNAMEINITATORVERTARGET,
                chapnameinitatorvertarget);
        operation.putOperationData(RestContains.CHAPPADINITATORVERTARGET,
                chappasswordinitatorvertarget);
        operation.putOperationData(RestContains.CHAPOLDPADINITATORVERTARGET,
                chapoldpasswordinitatorvertarget);

        operation.putOperationData(RestContains.DISCOVERYVERMODE,
                discoveryvermode);
        operation.putOperationData(RestContains.NORMALVERMODE, normalvermode);

        operation.putOperationData(RestContains.FAILOVERMODE, failovermode);
        operation.putOperationData(RestContains.PATHTYPE, pathtype);
        operation.putOperationData(RestContains.MULTIPATHTYPE, multipathtype);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get ISCSI initiator count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getiSCSINitiatorTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_ISCSI_INITIATOR_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Get ISCSI initiator by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<iSCSIInitiator>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<iSCSIInitiator> getiSCSIInitiatorByRange(
            StorageArrayInfo storageArrayInfo, int begin, int end)
            throws StorageCommonException, RestException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<iSCSIInitiator> iSCSIInitiators = performGetiSCSIInitiatorByRange(
                storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getiSCSIInitiators() == null) {
                    array.setiSCSIInitiators(new Vector<iSCSIInitiator>());
                } else {
                    array.getiSCSIInitiators().clear();
                }
                array.getiSCSIInitiators().addAll(iSCSIInitiators);
            }
        }
        return iSCSIInitiators;
    }

    /**
     * Get ISCSI initiator by host
     * @param storageArrayInfo OceanStor info
     * @param hostName Host name
     * @return List<iSCSIInitiator
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<iSCSIInitiator> getiSCSIInitiatorByHost(StorageArrayInfo storageArrayInfo, String hostName)
        throws StorageCommonException, RestException {
        Host host = hostManager.getHostByName(storageArrayInfo, hostName);
        if (null == host) {
            throw new StorageCommonException("Do not find Host by : " + hostName);
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_ISCSI_INITIATOR_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + 0 + "-" + getiSCSINitiatorTotalCount(storageArrayInfo) + "]");
        operation.putOperationData(RestContains.PARENTID, host.getReal_id());
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<iSCSIInitiator> iscsilist = new ArrayList<iSCSIInitiator>();
        for (Map<String, String> map : list) {
            iSCSIInitiator iscsi = (iSCSIInitiator) ObjectsBuilder.INSTANCE
                    .buildObject(map, iSCSIInitiator.class, storageArrayInfo);
            iscsilist.add(iscsi);
        }
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getiSCSIInitiators() == null) {
                    array.setiSCSIInitiators(new Vector<iSCSIInitiator>());
                } else {
                    array.getiSCSIInitiators().clear();
                }
                array.getiSCSIInitiators().addAll(iscsilist);
            }
        }
        return iscsilist;
    }

    /**
     * Perform get ISCSI initiator by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<iSCSIInitiator>
     * @throws StorageCommonException
     * @throws RestException
     */
    public List<iSCSIInitiator> performGetiSCSIInitiatorByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_ISCSI_INITIATOR_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<iSCSIInitiator> iscsilist = new ArrayList<iSCSIInitiator>();
        for (Map<String, String> map : list) {
            iSCSIInitiator iscsi = (iSCSIInitiator) ObjectsBuilder.INSTANCE
                    .buildObject(map, iSCSIInitiator.class, storageArrayInfo);
            iscsilist.add(iscsi);
        }
        return iscsilist;
    }

    /**
     * Add ISCSI initiator to host
     * @param storageArrayInfo OceanStor info
     * @param iqn ISCSI initiator iqn number
     * @param hostName Host name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void addiSCSIInitiatorToHost(StorageArrayInfo storageArrayInfo, String iqn, String hostName)
        throws RestException, StorageCommonException, UnsupportedEncodingException {
        String parentId = hostManager.getHostByName(storageArrayInfo, hostName).getReal_id();
        OceanStorOperation operation = new OceanStorOperation();
        String id = URLEncoder.encode(iqn, "UTF-8");
        operation.setOperationName(OperationNamesEnum.MODIFY_ISCSI_INITIATOR);
        operation.putOperationData(RestContains.ID, id);
        operation.putOperationData(RestContains.PARENTID, parentId);
        operation.putOperationData(RestContains.PARENTTYPE,
                RestContains.PARENTTYPE_HOST);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Remove ISCSI initiator from host
     * @param storageArrayInfo OceanStor info
     * @param iqn ISCSI iitiator iqn number
     * @throws RestException
     */
    public void removeiSCSIInitiatorFromHost(StorageArrayInfo storageArrayInfo, String iqn) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.REMOVE_ISCSI_INITIATOR_FROM_HOST);
        operation.putOperationData(RestContains.ID, iqn);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get ISCSI initiator by id
     * @param storageArrayInfo OceanStor info
     * @param iqn ISCSI initiator iqn number
     * @return iSCSIInitiator
     * @throws RestException
     * @throws StorageCommonException
     */
    public iSCSIInitiator getISCSIInitiatorById(StorageArrayInfo storageArrayInfo, String iqn)
        throws RestException, StorageCommonException {
        try {
            OceanStorOperation operation = new OceanStorOperation();
            operation.setOperationName(
                    OperationNamesEnum.GET_ISCSI_INITIATOR);
            operation.putOperationData(RestContains.ID, iqn);
            List<Map<String, String>> list = OperationUtil
                    .performAction(storageArrayInfo, operation);
            if (list.size() == 0) {
                throw new StorageCommonException("can not find ISCSI Initiator:" + iqn);
            }
            if (list.size() > 1) {
                throw new StorageCommonException("there is not only one ISCSI Initiator for iqn: " + iqn);
            }
            iSCSIInitiator iscsi = (iSCSIInitiator) ObjectsBuilder.INSTANCE.buildObject(list.get(0),
                    iSCSIInitiator.class, storageArrayInfo);
            return iscsi;
        } catch (RestException e) {
            if (e.getErrorDescription().startsWith("The specified object is unavailable")) {
                return null;
            } else {
                throw e;
            }
        }
    }
}
