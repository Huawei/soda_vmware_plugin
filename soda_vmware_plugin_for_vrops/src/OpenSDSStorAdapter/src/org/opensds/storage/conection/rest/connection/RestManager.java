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

package org.opensds.storage.conection.rest.connection;

import java.io.File;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import org.opensds.storage.conection.rest.constants.HttpMethodEnum;
import org.opensds.storage.conection.rest.constants.OperationConstants;
import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.domain.RestResponse;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.extracdata.util.LogUtil;

public class RestManager {
    private static final Logger LOGGER;

    private String hostURL;
    private RestRequestHandler restRequestHandler = new RestRequestHandler();

    public RestManager() {
    }

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        LOGGER = context.getLogger(RestManager.class.getName());
    }

    /**
     * Instantiates a new Rest manager.
     * <p>
     * when user provide ssl key file , use this constructor
     *
     * @param keyPath the key path
     * @param keyPass the key password
     * @param strictCheckHostName 是否精确校验主机名称
     */
    public RestManager(String keyPath, String keyPass, boolean strictCheckHostName) {
        restRequestHandler = new RestRequestHandler(keyPath, keyPass, strictCheckHostName);
    }

    public RestManager(String certificateFilePath, boolean strictCheckHostName) {
        restRequestHandler = new RestRequestHandler(certificateFilePath, strictCheckHostName);
    }

    /**
     * Do Http get post delete put operation make the response body string into
     * a JSONObject
     *
     * @param method Http method
     * @param restRelativeUri Rest request relative uri
     * @param requestBody Http request requestbody
     * @return RestResponses
     * @throws RestException
     */
    public RestResponse doRequest(HttpMethodEnum method, String restRelativeUri, String requestBody)
            throws RestException {
        RestResponse restResponse = null;
        String url = hostURL + restRelativeUri;
        if (LOGGER.isInfoEnabled()) {
            LOGGER.info("start rest request ,the request url is " + url + " request body is " + requestBody);
        }

        switch (method) {
            case GET:
                restResponse = restRequestHandler.doGet(url, requestBody);
                break;
            case DELETE:
                restResponse = restRequestHandler.doDelete(url, requestBody);
                break;
            case PUT:
                restResponse = restRequestHandler.doPut(url, requestBody);
                break;
            case POST:
                restResponse = restRequestHandler.doPost(url, requestBody);
                break;
            default:
                throw OperationError.HTTP.HTTP_UNSUPPORTED_HTTP_METHOD.newException(method.name());
        }
        if (LOGGER.isInfoEnabled()) {
            LOGGER.info("after rest request response is " + restResponse);
        }
        return restResponse;
    }

    /**
     * Do login http post to login the device get ibase Token
     *
     * @param loginRelativeURL Login request relative url
     * @param requestBody Http request requestbody
     * @return RestResponse
     * @throws RestException
     */
    public RestResponse doLogin(String loginRelativeURL, String requestBody) throws RestException {
        String url = this.hostURL + loginRelativeURL;
        if (LOGGER.isDebugEnabled()) {
            LOGGER.debug("login url is : " + url);
        }
        RestResponse restResponse = restRequestHandler.doPost(url, requestBody);

        if (restResponse.getError().getCode().equals("0")) {
            String iBaseToken = restResponse.getData().getDataList().get(0).get("iBaseToken");
            String deviceId = restResponse.getData().getDataList().get(0).get("deviceid");
            if (LOGGER.isDebugEnabled()) {
                LOGGER.debug("get login operation ibaseToken : " + iBaseToken);
                LOGGER.debug("get login operation deviceId : " + deviceId);
            }
            restRequestHandler.setToken(iBaseToken);
            this.hostURL = hostURL + "/" + deviceId;
        } else {
            LOGGER.error("login error happened : " + restResponse);
            throw new RestException(restResponse.getError().getCode(), restResponse.getError().getDescription(),
                    restResponse.getError().getSuggestion());
        }
        return restResponse;
    }

    /**
     * do http delete to logout the device
     *
     * @throws RestException
     */
    public void doLogout() throws RestException {
        if (LOGGER.isDebugEnabled()) {
            LOGGER.debug("the rest logout url is " + hostURL + "/sessions");
        }
        restRequestHandler.doDelete(hostURL + OperationConstants.RestRequestConstant.REST_LOGOUT_URI,
                null);
        restRequestHandler.releaseConnection();
    }

    /**
     * Sets socket timeout. when some operation need more time,use this override
     * default 30s socket idle timeout
     *
     * @param socketTimeout the socket timeout
     */
    public void setSocketTimeout(int socketTimeout) {
        this.restRequestHandler.setSocketTimeout(socketTimeout);
    }

    /**
     * Sets connect timeout. when the network environment is not very good,use
     * this override default 5s connect timeout
     *
     * @param connectTimeout the connect timeout
     */
    public void setConnectTimeout(int connectTimeout) {
        this.restRequestHandler.setConnectTimeout(connectTimeout);
    }

    public void setHostURL(String hostURL) {
        this.hostURL = hostURL;
    }
}
