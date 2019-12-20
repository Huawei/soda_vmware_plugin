// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.storage.conection.rest.operation;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import org.opensds.storage.conection.rest.domain.ConnectionData;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.utils.CommonUtils;
import org.opensds.storage.extracdata.util.LogUtil;

public class OperationExecutor {
    private static final Logger LOG;

    private DeviceManager deviceManager;
    private ConnectionData connectionData = new ConnectionData();

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        LOG = context.getLogger(OperationExecutor.class.getName());
    }

    public OperationExecutor() {
    }

    /**
     * Set devicemanager connection
     * @param dmUsername Devicemanager username
     * @param dmPassword Devicemanager password
     * @param dmUrl Devicemanager url
     * @param dmUserScope Devicemanager scope
     */
    public void setDMConnection(String dmUsername, String dmPassword, String dmUrl, String dmUserScope) {
        this.connectionData.setUsername(dmUsername);
        this.connectionData.setPassword(dmPassword);
        this.connectionData.setHostURL(dmUrl);
        this.connectionData.setScope(dmUserScope);
    }

    /**
     * execute
     * @param operation Http operation
     * @return Operation result
     */
    public OperationResult execute(OceanStorOperation operation) {
        if (LOG.isInfoEnabled()) {
            LOG.info("Operation executor start process.. operation data is " + operation);
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("operation conntionData is " + connectionData);
        }
        deviceManager = new DeviceManager(this.connectionData);
        OperationResult operationResult = new OperationResult();
        try {
            deviceManager.login();
            // workaround for get-CIFS-by-name
            if (operation.getOperationName().contains("fuzzy-single")) {
                operationResult = fuzzyFindReturnOne(operation, deviceManager);
            } else {
                operationResult = deviceManager.performAction(operation);
            }
        } catch (RestException e) {
            if (LOG.isInfoEnabled()) {
                LOG.error("execution exception happened : " + e.getMessage(), e);
            }
            operationResult.setErrorCode(e.getErrorCode());
            operationResult.setErrorDescription(e.getErrorDescription());
            operationResult.setErrorSuggestion(e.getErrorSuggestion());
            operationResult.addEmptyResult();
        } finally {
            deviceManager.logout();
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("operation complete, operation resulst is " + operationResult);
        }
        return operationResult;
    }

    // ==========================================================
    // for rest fuzzy query but only precise one need(for CIFS !!!)
    // todo one day you should remove this
    // ================================================================
    private OperationResult fuzzyFindReturnOne(OceanStorOperation operation, DeviceManager deviceManager)
            throws RestException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("start deal fuzzy find and return one. opertaion [" + operation + "]");
        }
        OceanStorOperation countOperation = new OceanStorOperation();
        String operationName = operation.getOperationName();
        String target = CommonUtils.upperOperationName(operation.getOperationData()).get("NAME");
        countOperation.setOperationName(operationName.substring(0, operationName.indexOf("by")) + "count");
        countOperation.putOperationData("filter", "NAME:" + target);
        OperationResult result = deviceManager.performAction(countOperation);
        if (!"0".equals(result.getErrorCode()) || result.getResultData() == null
                || result.getResultData().get(0) == null || result.getResultData().get(0).get("COUNT") == null) {
            throw new RestException(result.getErrorCode(), result.getErrorDescription(), result.getErrorSuggestion());
        }
        int count = Integer.parseInt(result.getResultData().get(0).get("COUNT"));
        OperationResult opResult = new OperationResult();
        if (LOG.isDebugEnabled()) {
            LOG.debug("find all fuzzy result count is " + count);
        }
        // page find and search
        if (count > 100) {
            int pages = count / 100;
            if (count % 100 != 0) {
                pages += 1;
            }
            for (int i = 0; i < pages; i++) {
                int start = i * 100;
                int end = start + 99;
                if (end > count) {
                    end = count;
                }
                if (LOG.isDebugEnabled()) {
                    LOG.debug("start get the range " + start + "-" + end);
                }
                operation.putOperationData("range", "[" + start + "-" + end + "]");
                opResult = deviceManager.performAction(operation);
                if (LOG.isDebugEnabled()) {
                    LOG.debug("start find target in result " + target);
                }
                if (findTarget(opResult, target)) {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("find result " + opResult + "in the range " + "[" + start + "---" + end + "]");
                    }
                    break;
                }
            }
        } else {
            operation.putOperationData("range", "[0-100]");
            opResult = deviceManager.performAction(operation);
            if (LOG.isDebugEnabled()) {
                LOG.debug("get from rest result is " + opResult);
                LOG.debug("start find target in result " + target);
            }
            findTarget(opResult, target);
        }
        return opResult;
    }

    private boolean findTarget(OperationResult operationResult, String target) {
        List<Map<String, String>> resultData = operationResult.getResultData();
        boolean isfound = false;
        for (Map<String, String> result : resultData) {
            if (result.get("NAME").equals(target)) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("find target " + target);
                }
                List<Map<String, String>> l = new ArrayList<Map<String, String>>();
                l.add(result);
                operationResult.setResultData(l);
                isfound = true;
                break;
            }
        }
        if (!isfound) {
            operationResult.addEmptyResult();
        }
        return isfound;
    }
}
