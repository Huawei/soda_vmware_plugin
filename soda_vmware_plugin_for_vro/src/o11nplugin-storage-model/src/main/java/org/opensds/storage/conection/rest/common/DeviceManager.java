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

import java.util.Map;

import org.apache.log4j.Logger;
import org.json.JSONException;
import org.json.JSONObject;
import org.opensds.storage.conection.rest.connection.ConnectionData;
import org.opensds.storage.conection.rest.connection.RestManager;
import org.opensds.storage.conection.rest.constants.DMError;
import org.opensds.storage.conection.rest.constants.HttpMethodEnum;
import org.opensds.storage.conection.rest.constants.OperationConstants.RestRequestConstant;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.utils.ComposeUtils;
import org.opensds.storage.vro.plugin.model.StorageCommonException;


public class DeviceManager {
    private static final Logger log = Logger.getLogger(DeviceManager.class);

    private RestManager restManager = new RestManager();

    private ConnectionData connectionData = new ConnectionData();

    public DeviceManager(ConnectionData connectionData) {
        this.connectionData = connectionData;
    }

    public DeviceManager(String hostURL, String username, String password,
                         String scope) {
        connectionData.setHostURL(hostURL);
        connectionData.setUsername(username);
        connectionData.setPassword(password);
        connectionData.setScope(scope);
    }

    public DeviceManager(String hostURL, String username, String password) {
        connectionData.setHostURL(hostURL);
        connectionData.setUsername(username);
        connectionData.setPassword(password);
    }

    /**
     * Login into the device , get the login ibase and cookie and put them
     * into the follow request header
     *
     * @throws RestException
     * @throws JSONException
     * @throws StorageCommonException
     */
    public void login()
            throws RestException, JSONException, StorageCommonException {
        if (log.isDebugEnabled()) {
            log.debug("start login:::");
        }
        restManager.setHostURL(connectionData.getHostURL());
        JSONObject requestBody = new JSONObject();
        requestBody.put("username", connectionData.getUsername());
        requestBody.put("password", connectionData.getPassword());
        requestBody.put("scope", connectionData.getScope());
        JSONObject response = restManager.doLoginRequest(HttpMethodEnum.POST,
                RestRequestConstant.REST_LOGIN_URI,
                requestBody,
                connectionData.isCheckCert());
        System.out.println("response is:" + response);
    }

    /**
     * Logout the device
     *
     * @throws RestException
     * @throws StorageCommonException
     */
    public void logout() throws RestException, StorageCommonException {

        if (restManager != null) {
            restManager.doRequest(HttpMethodEnum.DELETE,
                    RestRequestConstant.REST_LOGOUT_URI,
                    new JSONObject(),
                    connectionData.isCheckCert());
        }
    }

    /**
     * do the rest quest and do login first
     *
     * @param operation
     * @return
     * @throws RestException
     * @throws StorageCommonException
     */
    public OperationResult performAction(OceanStorOperation operation)
            throws RestException, StorageCommonException {
        checkOperation(operation);
        return doRestRequest(operation.getOperationName(),
                operation.getOperationData(), operation.getOperationDataObject());
    }

    /**
     * check if the request data is valid
     *
     * @param operationData
     * @throws RestException
     */
    private void checkOperation(OceanStorOperation operationData)
            throws RestException {
        if (!RestInterfaceConfs
                .isValidOperation(operationData.getOperationName())) {
            throw DMError.REQUEST.REQUEST_OPERATION_NAME_INVALID
                    .newException(operationData.getOperationName());
        }
        // TODO check mandatory field here

    }

    /**
     * do real Http request
     *
     * @param operationName
     * @param operationData
     * @return
     * @throws RestException
     * @throws StorageCommonException
     */
    private OperationResult doRestRequest(String operationName,
                                          Map<String, String> operationData, Map<String, Object> operationDataObject)
            throws RestException, StorageCommonException {
        OperationResult operationResult = new OperationResult();
        try {
            String restMethod = RestInterfaceConfs.getRestMethod(operationName);
            String restRelativeUri = ComposeUtils
                    .composeRelativeUri(operationName, operationData);
            JSONObject requestBody = ComposeUtils
                    .composeJsonFromMap(operationData, operationDataObject);
            if (log.isInfoEnabled()) {
                log.info("the request param is::: " + restMethod + ";"
                        + restRelativeUri);
            }
            JSONObject restResult = restManager.doRequest(
                    HttpMethodEnum.valueOf(restMethod),
                    restRelativeUri,
                    requestBody,
                    connectionData.isCheckCert());
            ComposeUtils.composeResultFromJson(restResult, operationResult);
        } catch (RestException e) {
            log.error("execute exeception happened,Exception is:"
                    + e.getMessage(), e);
            operationResult.setErrorCode(e.getErrorCode());
            operationResult.setErrorDescription(e.getErrorDescription());
            operationResult.setErrorSuggestion(e.getErrorSuggestion());
            throw new RestException(e.getErrorCode(), e.getErrorDescription(),
                    e.getErrorSuggestion());
        }
        return operationResult;
    }

}
