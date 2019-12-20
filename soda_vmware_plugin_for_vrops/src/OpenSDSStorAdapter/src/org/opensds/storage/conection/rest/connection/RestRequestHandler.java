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
import java.io.IOException;

import org.apache.http.client.config.RequestConfig;
import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpEntityEnclosingRequestBase;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.client.methods.HttpRequestBase;
import org.apache.http.entity.StringEntity;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.domain.RestResponse;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.extracdata.util.LogUtil;

public class RestRequestHandler {
    private static final Logger LOGGER;
    private static final int SOCKET_TIMEOUT = 30 * 1000; // default data read
    // timeout
    private static final int CONNECT_TIMEOUT = 15 * 1000; // default connection
    // timeout
    private static final int CONNECT_REQUEST_TIMEOUT = 10 * 1000; // default
    private String certificateFilePath;
    private String keyPath;
    private String keyPass;
    private boolean strictCheckHostName;
    private String iBaseToken;
    // connection
    // timeout
    private HttpClientManager clientManager = new HttpClientManager();
    private int socketTimeout = SOCKET_TIMEOUT;
    private int connectTimeout = CONNECT_TIMEOUT;
    private int connectRequestTimeout = CONNECT_REQUEST_TIMEOUT;

    public RestRequestHandler() {
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
        LOGGER = context.getLogger(RestRequestHandler.class.getName());
    }

    public RestRequestHandler(String keyPath, String keyPass, boolean strictCheckHostName) {
        this.keyPath = keyPath;
        this.keyPass = keyPass;
        this.strictCheckHostName = strictCheckHostName;
    }

    public RestRequestHandler(String certificateFilePath, boolean strictCheckHostName) {
        this.certificateFilePath = certificateFilePath;
        this.strictCheckHostName = strictCheckHostName;
    }

    RestResponse doPost(String url, String requestBody) throws RestException {
        HttpPost method = new HttpPost(url);
        setRequestHeader(method);
        setRequestBody(requestBody, method);
        return doRequest(method);
    }

    RestResponse doPut(String url, String requestBody) throws RestException {
        HttpPut method = new HttpPut(url);
        setRequestHeader(method);
        setRequestBody(requestBody, method);
        return doRequest(method);
    }

    private void setRequestBody(String requestBody, HttpEntityEnclosingRequestBase method) throws RestException {
        StringEntity bodyStringEntity = new StringEntity(requestBody, "utf-8");
        bodyStringEntity.setContentType("application/json");
        method.setEntity(bodyStringEntity);
    }

    RestResponse doDelete(String url, String requestBody) throws RestException {
        HttpDelete method = new HttpDelete(url);
        setRequestHeader(method);
        return doRequest(method);
    }

    RestResponse doGet(String url, String requestBody) throws RestException {
        HttpGet method = new HttpGet(url);
        setRequestHeader(method);
        return doRequest(method);
    }

    private void setRequestHeader(HttpRequestBase request) {
        request.setHeader("Content-Type", "application/json");
        request.setHeader("Accept-Language", "en");
        if (null != iBaseToken) {
            request.setHeader("iBaseToken", iBaseToken);
        }
    }

    private void setRequestTimeouts(HttpRequestBase method) {
        RequestConfig requestConfig = RequestConfig.custom().setSocketTimeout(socketTimeout)
                .setConnectTimeout(connectTimeout).setConnectionRequestTimeout(connectRequestTimeout).build();
        method.setConfig(requestConfig);
    }

    private RestResponse doRequest(HttpRequestBase method) throws RestException {
        RestResponse restResponse;
        try {
            setRequestTimeouts(method);
            if (keyPath != null && keyPass != null) {
                restResponse = clientManager.getHttpClient(keyPath, keyPass, strictCheckHostName).execute(method,
                        new RestResponseHandler());
            } else if (certificateFilePath != null && certificateFilePath.length() > 0) {
                restResponse = clientManager.getHttpClient(certificateFilePath, strictCheckHostName).execute(method,
                        new RestResponseHandler());
            } else {
                restResponse = clientManager.getHttpClient().execute(method, new RestResponseHandler());
            }
        } catch (IOException e) {
            LOGGER.error("The http request failed because of an IOException: " + e.getMessage(), e);
            throw OperationError.HTTP.HTTP_IO_EXCEPTION_ERROR.newException(method.getMethod(),
                    method.getURI().getHost() + ":" + method.getURI().getPort() + method.getURI().getPath(),
                    e + ":" + e.getMessage());
        } catch (RestException e) {
            throw e;
        } catch (Exception e) {
            LOGGER.error("The http request failed because of an Exception: " + e.getMessage(), e);
            throw OperationError.HTTP.HTTP_EXCEPTION_ERROR.newException(method.getMethod(),
                    method.getURI().getHost() + ":" + method.getURI().getPort() + method.getURI().getPath(),
                    e + ":" + e.getMessage());
        }
        return restResponse;
    }

    void setToken(String iBaseToken) {
        this.iBaseToken = iBaseToken;
    }

    public void setSocketTimeout(int socketTimeout) {
        this.socketTimeout = socketTimeout;
    }

    public void setConnectTimeout(int connectTimeout) {
        this.connectTimeout = connectTimeout;
    }

    void releaseConnection() {
        if (clientManager != null) {
            clientManager.releaseHttpClient();
        }
    }

}
