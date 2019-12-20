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
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.ResponseHandler;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.google.gson.reflect.TypeToken;
import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.domain.Data;
import org.opensds.storage.conection.rest.domain.Error;
import org.opensds.storage.conection.rest.domain.RestResponse;
import org.opensds.storage.extracdata.util.LogUtil;

class RestResponseHandler implements ResponseHandler<RestResponse> {
    private static final Logger LOG;

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager
                .getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        LOG = context.getLogger(RestResponseHandler.class.getName());
    }

    /**
     * Handle http response
     * @param response Http response
     * @return Rest response
     * @throws IOException
     */
    public RestResponse handleResponse(HttpResponse response) throws IOException {
        RestResponse restResponse;
        if ((restResponse = validateResponse(response)) != null) {
            return restResponse;
        }
        HttpEntity entity = response.getEntity();
        String encoding = entity.getContentEncoding() != null ? entity.getContentEncoding().getValue() : "utf-8";
        String restResponseString;
        try {
            restResponseString = IOUtils.toString(entity.getContent(), encoding);
        } catch (IOException e) {
            LOG.error("read http body IO Exception happened :" + e.getMessage(), e);
            return makeErrorResponse(OperationError.HTTP.HTTP_RESPONSE_BODY_READ_ERROR, e);
        }

        return makeRestResponse(restResponseString);
    }

    private RestResponse makeRestResponse(String restResponseString) {
        LOG.debug("Enter makeRestResponse method : parameter is " + restResponseString);
        Gson gson = new Gson();
        JsonParser parser = new JsonParser();
        JsonObject restJSONObject = parser.parse(restResponseString).getAsJsonObject();
        JsonElement dataJSONElement = restJSONObject.get("data");
        if (dataJSONElement == null) {
            LOG.info("the return data part is missing...");
        }
        Data data = new Data();
        List<Map<String, String>> dataList = new ArrayList<Map<String, String>>();
        // Return data can be single value or array
        if (null != dataJSONElement && dataJSONElement instanceof JsonArray) {
            JsonArray dataArray = (JsonArray) dataJSONElement;
            for (JsonElement element : dataArray) {
                Map<String, String> map = gson.fromJson(element, new TypeToken<Map<String, String>>() {
                }.getType());
                dataList.add(map);
            }
        } else if (dataJSONElement != null) {
            Map<String, String> map = gson.fromJson(dataJSONElement.getAsJsonObject(),
                    new TypeToken<Map<String, String>>() {
                    }.getType());
            dataList.add(map);
        } else {
            LOG.error("The response is null!");
        }
        data.setDataList(dataList);
        LOG.debug(String.format("after parse data part is :%s", dataList));
        // get error part
        Error error = new Error(OperationError.INTERNAL.REST_RESPONSE_ERROR_CODE_MISSING.getErrorCode(),
                OperationError.INTERNAL.REST_RESPONSE_ERROR_CODE_MISSING.getErrorDesc(),
                OperationError.INTERNAL.REST_RESPONSE_ERROR_CODE_MISSING.getErrorSuggestion());
        JsonElement errorJSONObject = restJSONObject.get("error");
        if (errorJSONObject != null) {
            errorJSONObject = errorJSONObject.getAsJsonObject();
            error = gson.fromJson(errorJSONObject, Error.class);
        } else {
            LOG.info("the return error part is missing...");
        }
        LOG.debug("after parse error part is : " + error);
        return new RestResponse(data, error);
    }

    private RestResponse validateResponse(HttpResponse response) {
        if (response == null) {
            LOG.error("http response is null");
            return makeErrorResponse(OperationError.HTTP.HTTP_RESPONSE_NULL_ERROR);
        }
        if (response.getStatusLine() == null) {
            LOG.error("http response status line is null");
            return makeErrorResponse(OperationError.HTTP.HTTP_RESPONSE_NULL_STATUS_LINE);
        }

        if (response.getStatusLine().getStatusCode() != HttpStatus.SC_OK) {
            LOG.error("http response statusLine code is not 200 , the code is "
                    + response.getStatusLine().getStatusCode());
            return makeErrorResponse(OperationError.HTTP.HTTP_RESPONSE_ERROR_STATUS_CODE);
        }

        if (response.getEntity() == null || !response.getEntity().isStreaming()) {
            LOG.error("http response body is error");
            return makeErrorResponse(OperationError.HTTP.HTTP_RESPONSE_BODY_ERROR);
        }

        return null;
    }

    private RestResponse makeErrorResponse(OperationError.HTTP error) {
        return makeErrorResponse(error, null);
    }

    private RestResponse makeErrorResponse(OperationError.HTTP error, Exception e) {
        Data data = new Data();
        List<Map<String, String>> dataList = new ArrayList<Map<String, String>>();
        data.setDataList(dataList);
        return new RestResponse(data, new Error(error.getErrorCode(),
                error.getErrorDesc() + (e != null ? e.getMessage() : ""), error.getErrorSuggestion()));
    }
}
