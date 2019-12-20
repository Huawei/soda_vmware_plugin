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

package org.opensds.storage.conection.rest.common;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.apache.log4j.Logger;
import org.json.JSONException;

import org.opensds.storage.conection.rest.connection.ConnectionData;
import org.opensds.storage.conection.rest.constants.DMError;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;
import org.opensds.storage.vro.plugin.model.StorageArrayInfo;
import org.opensds.storage.vro.plugin.model.StorageCommonException;
import org.opensds.storage.vro.plugin.model.SysInfo;
import org.opensds.storage.vro.plugin.model.SysInfoManager;

public class OperationUtil {
    private static final Logger LOG = Logger.getLogger(OperationUtil.class);

    private static SysInfoManager sysInfoManager = new SysInfoManager();

    public static List<Map<String, String>> performAction(
            StorageArrayInfo storageArrayInfo, OceanStorOperation operation)
            throws RestException {
        List<Map<String, String>> result = new ArrayList<Map<String, String>>();

        try {
            result = performRestAction(storageArrayInfo, operation);
        } catch (RestException e) {
            boolean doSwitch = e.getErrorCode() != null
                    && e.getErrorCode().equals(DMError.IO_ERROR)//is connection error 
                    && operation.isSwithIP()
                    && storageArrayInfo.isNeedSwithIp()//need swith IP
                    && storageArrayInfo.getCandidateIpList() != null
                    && storageArrayInfo.getCandidateIpList().trim().length() != 0;

            if (doSwitch) {

                LOG.info("Start to switchIP::: " + storageArrayInfo);
                switchIP(storageArrayInfo, operation, result, e);
            } else {
                throw e;
            }
        }

        return result;
    }

    private static StorageArrayInfo createNewStorageArrayInfo(
            StorageArrayInfo storageArrayInfo) {
        StorageArrayInfo arrayInfo = new StorageArrayInfo();
        arrayInfo.setPort(storageArrayInfo.getPort());
        arrayInfo.setUsername(storageArrayInfo.getUsername());
        arrayInfo.setPassword(storageArrayInfo.getPassword());
        arrayInfo.setCheckCert(storageArrayInfo.getCheckCert());
        return arrayInfo;

    }

    private static void switchIP(StorageArrayInfo storageArrayInfo,
                                 OceanStorOperation operation, List<Map<String, String>> result,
                                 RestException restException) throws RestException {
        LOG.info("enter switchIP:: " + storageArrayInfo);
        String[] candidateIpArray = storageArrayInfo.getCandidateIpList()
                .split(",");
        boolean connectIsOk = false;
        int aliveIpIndex = 0;
        StorageArrayInfo newStorageArrayInfo = createNewStorageArrayInfo(
                storageArrayInfo);
        //test connection
        for (int i = 0; i < candidateIpArray.length; i++) {
            newStorageArrayInfo.setHostName(candidateIpArray[i]);
            connectIsOk = testConnection(newStorageArrayInfo,
                    operation,
                    result);
            if (connectIsOk) {
                aliveIpIndex = i;
                break;
            }
        }

        //found a IP can be switched
        if (connectIsOk) {
            String aliveIp = candidateIpArray[aliveIpIndex];
            //switch IP 
            if (!storageArrayInfo.getHostName().equals(aliveIp)) {
                String newHostName = aliveIp;
                String newId = aliveIp + "-" + storageArrayInfo.getPort();
                String oldHostName = storageArrayInfo.getHostName();
                candidateIpArray[aliveIpIndex] = storageArrayInfo.getHostName();
                StringBuffer newCandidateIPList = new StringBuffer();
                for (int i = 0; i < candidateIpArray.length; i++) {
                    newCandidateIPList.append(candidateIpArray[i]);
                    if (i < candidateIpArray.length - 1) {
                        newCandidateIPList.append(",");
                    }
                }
                try {
                    OpenSDSStorageRepository.getUniqueInstance()
                            .updateProperties(storageArrayInfo,
                                    OpenSDSStorageRepository.REMOVE);
                    storageArrayInfo.setId(newId);
                    storageArrayInfo.setHostName(newHostName);
                    storageArrayInfo
                            .setCandidateIpList(newCandidateIPList.toString());
                    SysInfo sysInfo = sysInfoManager
                            .getSysInfo(storageArrayInfo, false);
                    storageArrayInfo.setProductName(sysInfo.getProductName());
                    storageArrayInfo
                            .setProductVersion(sysInfo.getProductVersion());
                    OpenSDSStorageRepository.getUniqueInstance()
                            .updateProperties(storageArrayInfo,
                                    OpenSDSStorageRepository.ADD);
                } catch (StorageCommonException e) {
                    String errorMsg = "can not access IP " + oldHostName
                            + ", StorageCommonException happened when swith IP to  "
                            + newHostName;
                    LOG.error(errorMsg, e);
                    throw new RestException(errorMsg);
                } catch (IOException e) {
                    String errorMsg = "can not access IP " + oldHostName
                            + ", IOException happened when swith IP to  "
                            + newHostName;
                    LOG.error(errorMsg, e);
                    throw new RestException(errorMsg);
                }
            }

        }
        //no ip can be switched, throws the exception
        else {
            String errorMsg = "can not access IP "
                    + storageArrayInfo.getHostName()
                    + ", and candidate IP list "
                    + storageArrayInfo.getCandidateIpList()
                    + " can also not be accessed";
            LOG.error(errorMsg, restException);
            restException.setErrorDescription(errorMsg);
            throw restException;
        }
        LOG.info("exit switchIP:: " + storageArrayInfo);
    }

    private static boolean testConnection(StorageArrayInfo storageArrayInfo,
                                          OceanStorOperation operation, List<Map<String, String>> result)
            throws RestException {
        LOG.info("Begin to testConnection::: " + storageArrayInfo);
        boolean connectIsOk = false;
        try {
            List<Map<String, String>> returnResult = performRestAction(
                    storageArrayInfo, operation);
            result.clear();
            result.addAll(returnResult);
            connectIsOk = true;
        } catch (RestException e) {
            //connection error
            if (e.getErrorCode() != null
                    && e.getErrorCode().equals(DMError.IO_ERROR)) {
                LOG.warn("testConnection:::" + e.getMessage(), e);
            }
            //no connection error
            else {
                LOG.error("testConnection:::" + e.getMessage(), e);
                throw e;
            }
        }
        LOG.info("testConnection is " + connectIsOk + " for "
                + storageArrayInfo);
        return connectIsOk;
    }

    private static List<Map<String, String>> performRestAction(
            StorageArrayInfo storageArrayInfo, OceanStorOperation operation)
            throws RestException {
        OperationResult operationResult = null;
        DeviceManager deviceManager = null;
        List<Map<String, String>> list = new ArrayList<Map<String, String>>();
        if (LOG.isInfoEnabled()) {
            LOG.info("StorageArrayInfo::: " + storageArrayInfo.getId());
            LOG.info("Operation Name::: " + operation.getOperationName());
            LOG.info("checkCert::: " + storageArrayInfo.getCheckCert());
        }
        Map<String, String> operData = operation.getOperationData();
        Map<String, String> operDataSecure = new HashMap<String, String>();
        for (Map.Entry<String, String> entry : operData.entrySet()) {
            String key = new String(entry.getKey()).toLowerCase(Locale.US);
            if (key.contains("password") || key.contains("pass")) {
                operDataSecure.put(key, "******");
            } else {
                operDataSecure.put(key, entry.getValue());
            }
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("OperationData::::" + operDataSecure);
        }
        try {
            ConnectionData connectionData = new ConnectionData();
            connectionData.setHostURL(storageArrayInfo.getURL());
            connectionData.setUsername(storageArrayInfo.getUsername());
            connectionData.setCheckCert(storageArrayInfo.getCheckCert());
            String pass = ch.dunes.util.EncryptHelper
                    .decrypt(storageArrayInfo.getPassword());

            connectionData.setPassword(pass);
            deviceManager = new DeviceManager(connectionData);
            deviceManager.login();

            operationResult = deviceManager.performAction(operation);

            if (operationResult != null) {
                list = operationResult.getResultData();
            }
            if (LOG.isInfoEnabled()) {
                LOG.info("operationResult return:::" + list);
            }
            return list;
        } catch (JSONException e) {
            LOG.error("performAction:::" + e.getMessage(), e);
            throw new RestException(e.getMessage());
        } catch (RestException e) {
            LOG.error("performAction:::" + e.getMessage(), e);
            throw e;
        } catch (StorageCommonException e) {
            LOG.error("deviceManager.performAction error", e);
            throw new RestException(e);
        } finally {
            if (deviceManager != null) {
                try {
                    deviceManager.logout();
                } catch (RestException e) {
                    LOG.error("deviceManager.logout error", e);
                } catch (StorageCommonException e) {
                    LOG.error("deviceManager.logout error", e);
                }
            }
        }
    }
}
