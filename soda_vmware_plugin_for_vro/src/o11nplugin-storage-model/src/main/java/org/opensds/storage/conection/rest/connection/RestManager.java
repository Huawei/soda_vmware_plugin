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

package org.opensds.storage.conection.rest.connection;

import org.apache.log4j.Logger;
import org.json.JSONObject;

import org.opensds.storage.conection.rest.common.OperationResult;
import org.opensds.storage.conection.rest.constants.HttpMethodEnum;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.utils.ComposeUtils;
import org.opensds.storage.vro.plugin.model.StorageCommonException;

public class RestManager {
    private static final Logger logger = Logger.getLogger(RestManager.class);

    private String hostURL;

    private RestRequestHandler restRequestHandler = new RestRequestHandler();

    public void setHostURL(String hostURL) {
        this.hostURL = hostURL;

    }

    public RestManager() {
    }

    /**
     * Do Http get post delete put operation make the response body string into
     * a JSONObject
     *
     * @param method
     * @param restRelativeUri
     * @param requestBody
     * @return
     * @throws RestException
     * @throws StorageCommonException
     */
    public JSONObject doRequest(HttpMethodEnum method, String restRelativeUri,
                                JSONObject requestBody, Boolean checkCert)
            throws RestException, StorageCommonException {
        logger.info("requestBody=" + requestBody.toString());
        JSONObject jsonResult = null;
        String url = hostURL + restRelativeUri;
        switch (method) {
            case GET:
                jsonResult = restRequestHandler.doGet(url,
                        requestBody,
                        checkCert);
                break;
            case DELETE:
                jsonResult = restRequestHandler.doDelete(url, checkCert);
                break;
            case PUT:
                jsonResult = restRequestHandler.doPut(url,
                        requestBody,
                        checkCert);
                break;
            case POST:
                jsonResult = restRequestHandler.doPost(url,
                        requestBody,
                        checkCert);
                break;
            default:
                break;
        }
        return jsonResult;
    }

    /**
     * Do login http post to login the device get ibaseToken
     *
     * @param post
     * @param loginRelativeURL
     * @param requestBody
     * @return
     * @throws RestException
     * @throws StorageCommonException
     */
    public JSONObject doLoginRequest(HttpMethodEnum post,
                                     String loginRelativeURL, JSONObject requestBody, Boolean checkCert)
            throws RestException, StorageCommonException {
        String url = this.hostURL + loginRelativeURL;
        JSONObject returnJson = restRequestHandler.doPost(url,
                requestBody,
                checkCert);

        OperationResult operationResult = ComposeUtils
                .composeResultFromJson(returnJson);
        if (operationResult.getErrorCode().equals("0")) {
            String iBaseToken = operationResult.getResultData()
                    .get(0)
                    .get("iBaseToken");
            String deviceId = operationResult.getResultData()
                    .get(0)
                    .get("deviceid");
            restRequestHandler.setToken(iBaseToken);
            this.hostURL = hostURL + "/" + deviceId;
        }
        return returnJson;
    }
}
