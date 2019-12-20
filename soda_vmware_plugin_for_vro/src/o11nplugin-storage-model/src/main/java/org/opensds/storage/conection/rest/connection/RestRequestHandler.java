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

import java.io.IOException;
import java.io.InputStream;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.StatusLine;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.client.methods.HttpRequestBase;
import org.apache.http.client.utils.HttpClientUtils;
import org.apache.http.entity.StringEntity;
import org.apache.log4j.Logger;
import org.json.JSONException;
import org.json.JSONObject;

import org.opensds.storage.conection.rest.constants.DMError;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.plugin.model.StorageCommonException;

public class RestRequestHandler {
    private static final Logger logger = Logger
            .getLogger(RestRequestHandler.class);

    private String iBaseToken;

    public RestRequestHandler() {
    }

    public JSONObject doPost(String url, JSONObject requestBody,
                             boolean checkCert) throws RestException, StorageCommonException {
        HttpPost method = new HttpPost(url);
        setRequestHeader(method);
        StringEntity bodyStringEntity = new StringEntity(requestBody.toString(),
                "utf-8");
        bodyStringEntity.setContentType("application/json");
        method.setEntity(bodyStringEntity);

        return doRequest(method, checkCert);
    }

    public JSONObject doPut(String url, JSONObject requestBody,
                            boolean checkCert) throws RestException, StorageCommonException {
        HttpPut method = new HttpPut(url);
        setRequestHeader(method);
        StringEntity bodyStringEntity = new StringEntity(requestBody.toString(),
                "utf-8");
        bodyStringEntity.setContentType("application/json");
        method.setEntity(bodyStringEntity);
        return doRequest(method, checkCert);
    }

    public JSONObject doDelete(String url, boolean checkCert)
            throws RestException, StorageCommonException {
        HttpDelete method = new HttpDelete(url);
        setRequestHeader(method);
        return doRequest(method, checkCert);
    }

    public JSONObject doGet(String url, JSONObject requestBody,
                            boolean checkCert) throws RestException, StorageCommonException {
        HttpGet method = new HttpGet(url);
        setRequestHeader(method);
        return doRequest(method, checkCert);
    }

    private void setRequestHeader(HttpRequestBase request) {
        request.setHeader("Content-Type", "application/json");
        request.setHeader("Accept-Language", "en");
        if (null != iBaseToken) {
            request.setHeader("iBaseToken", this.iBaseToken);
        }
    }

    private JSONObject doRequest(HttpRequestBase method, boolean checkCert)
            throws RestException, StorageCommonException {
        HttpResponse response = null;
        JSONObject responseJson = null;
        try {
            if (checkCert) {
                response = HttpClientFactory.getHttpClientWithTrustStore()
                        .execute(method);
            } else {
                response = HttpClientFactory.getHttpClient().execute(method);
            }
            responseJson = handleResponse(response);
        } catch (RestException e) {
            logger.error("The http request failed beacuse of an RestException: "
                    + e.getMessage());
            throw DMError.HTTP.HTTP_IO_EXCEPTION_ERROR.newException(
                    method.getURI().getHost() + ":" + method.getURI().getPort()
                            + "/" + method.getURI().getPath());
        } catch (ClientProtocolException e) {
            logger.error(
                    "The http request failed beacuse of an ClientProtocolException: "
                            + e.getMessage());
            throw new RestException(
                    "The http request failed beacuse of an ClientProtocolException: "
                            + e.getMessage());
        } catch (IOException e) {
            logger.error("The http request failed beacuse of an IOException: "
                    + e.getMessage());
            throw new RestException(DMError.IO_ERROR,
                    "The http request failed beacuse of an IOException: "
                            + e.getMessage(),
                    "IOException");
        } catch (StorageCommonException e) {
            logger.error(
                    "The http request failed beacuse of an StorageCommonException: "
                            + e.getMessage());
            throw new StorageCommonException(
                    "The http request failed beacuse of an StorageCommonException: "
                            + e.getMessage());
        } finally {
            HttpClientUtils.closeQuietly(response);
        }
        return responseJson;
    }

    private JSONObject handleResponse(HttpResponse response)
            throws RestException {
        StatusLine statusLine = response.getStatusLine();
        HttpEntity entity = response.getEntity();
        JSONObject mResponseJason = null;
        InputStream inputStream = null;

        if (null != statusLine) {

            if (statusLine.getStatusCode() >= HttpStatus.SC_MULTIPLE_CHOICES) {
                logger.error("http response status code is error:"
                        + statusLine.getStatusCode());
                throw DMError.HTTP.HTTP_RESPONSE_ERROR_STATUS_CODE
                        .newException(statusLine.getStatusCode() + "");
            }
        }

        if (null == entity || !entity.isStreaming()) {
            logger.error("http response body is error");
            throw DMError.HTTP.HTTP_RESPONSE_BODY_ERROR.newException("");
        }

        String inputString = null;
        try {
            inputStream = entity.getContent();
            inputString = IOUtils.toString(inputStream, "UTF-8");
            if (inputString.length() > 0) {
                mResponseJason = new JSONObject(inputString);
            }
        } catch (IOException e) {
            logger.error("Read http response exception occurred ", e);
            throw DMError.HTTP.HTTP_RESPONSE_BODY_READ_ERROR.newException("");
        } catch (JSONException e) {
            logger.error(
                    "Conver http response body string to JSONObject error happend,"
                            + " http body string is ",
                    e);
            throw DMError.JSON.MAKE_RESPONSE_BODY_TO_JSON_ERROR
                    .newException(inputString);
        } finally {
            closeStream(inputStream);
        }

        return mResponseJason;
    }

    private void closeStream(InputStream inputStream) {
        if (null != inputStream) {
            try {
                inputStream.close();
            } catch (IOException e) {
                logger.error(
                        "close the http input stream IOException happened :"
                                + e.getMessage());
            }
        }
    }

    public void setToken(String iBaseToken) {
        this.iBaseToken = iBaseToken;
    }
}
