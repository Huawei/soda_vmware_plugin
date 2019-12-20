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

package org.opensds.storage.conection.rest.utils;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import org.opensds.storage.conection.rest.common.OperationResult;
import org.opensds.storage.conection.rest.common.RestInterfaceConfs;
import org.opensds.storage.conection.rest.connection.MapTypeToken;
import org.opensds.storage.conection.rest.constants.DMError;
import org.opensds.storage.conection.rest.exception.RestException;

public class ComposeUtils {
    private static final Logger log = Logger.getLogger(ComposeUtils.class);

    public static String composeRelativeUri(String operationName,
                                            Map<String, String> requestData) throws RestException {
        String restRelativePath = RestInterfaceConfs
                .getRestRelativePath(operationName);
        if (restRelativePath != null) {
            if (restRelativePath.contains("?")) {
                restRelativePath = makeQueryUri(requestData, restRelativePath);
            } else {
                restRelativePath = makeActionUri(requestData, restRelativePath);
            }
        }
        log.info("restRelativePath URL::" + restRelativePath);
        return restRelativePath;
    }

    private static String makeActionUri(Map<String, String> requestData,
                                        String restRelativePath) throws RestException {
        String[] paths = restRelativePath.split(",");
        StringBuffer sf = new StringBuffer();
        for (String path : paths) {
            if (path.startsWith("${")) {
                // TODO check and make sure every variable can get from request
                // if require data not contained in requestDataMap throw
                // exception
                String varKey = (path.substring(2,
                        path.length() - 1))/* .toUpperCase() */;
                if (requestData.get(varKey) == null) {
                    throw DMError.REQUEST.MISSING_REQUIRED_PARAM
                            .newException(varKey);
                }
                path = requestData.get(varKey);
            }
            sf.append("/").append(path);
        }
        restRelativePath = sf.toString();
        return restRelativePath;
    }

    private static String makeQueryUri(Map<String, String> requestData,
                                       String restRelativePath) throws RestException {
        // filesystem?${filter=${filter},range=${range}}
        // snapshot,associate?${ASSOCIATEOBJTYPE=${ASSOCIATEOBJTYPE}}

        StringBuffer sf = new StringBuffer();
        String queryPre = restRelativePath.substring(0,
                restRelativePath.indexOf("?"));
        if (queryPre.contains(",")) {
            String[] queryPres = queryPre.split(",");
            for (String pre : queryPres) {
                if (pre.startsWith("${")) {
                    // TODO check and make sure every variable can get from request
                    // if require data not contained in requestDataMap throw
                    // exception
                    String varKey = (pre.substring(2,
                            pre.length() - 1))/* .toUpperCase() */;
                    if (requestData.get(varKey) == null) {
                        throw DMError.REQUEST.MISSING_REQUIRED_PARAM
                                .newException(varKey);
                    }
                    pre = requestData.get(varKey);
                }
                sf.append("/").append(pre);
            }
        } else {
            if (queryPre.startsWith("${")) {
                // TODO check and make sure every variable can get from request
                // if require data not contained in requestDataMap throw
                // exception
                String varKey = (queryPre.substring(2,
                        queryPre.length() - 1))/* .toUpperCase() */;
                if (requestData.get(varKey) == null) {
                    throw DMError.REQUEST.MISSING_REQUIRED_PARAM
                            .newException(varKey);
                }
                queryPre = requestData.get(varKey);
            }
            sf.append("/").append(queryPre);
        }
        sf.append("?");
        String expression = restRelativePath.substring(
                restRelativePath.indexOf("?${") + 3,
                restRelativePath.length() - 1);
        String[] exps = expression.split(",");
        for (String exp : exps) {
            String key = exp.split("=")[0];// .toUpperCase()
            if (requestData.get(key) != null) {
                sf.append(key/* .toLowerCase() */)
                        .append("=")
                        .append(requestData.get(key))
                        .append("&");
            }
        }
        if (sf.toString().endsWith("&")) {
            restRelativePath = sf.toString().substring(0, sf.length() - 1);
        }
        // if there is no additional query parameter,remove question mark
        if (sf.toString().endsWith("?")) {
            restRelativePath = sf.toString().substring(0, sf.length() - 1);
        }
        return restRelativePath;
    }

    public static JSONObject composeJsonFromMap(
            Map<String, String> operationData, Map<String, Object> operationDataObject) throws RestException {
        JSONObject requestBody = new JSONObject();
        // TODO convert the map string value to int or long value here
        for (Entry<String, String> entry : operationData.entrySet()) {
            String key = entry.getKey().toString().toUpperCase(Locale.US);
            Object value = entry.getValue();
            try {
                requestBody.put(key, value);
            } catch (JSONException e) {
                throw DMError.JSON.MAKE_REQUEST_BODY_JSON_ERROR
                        .newException(key + ":" + value);
            }
        }

        for (Entry<String, Object> entry : operationDataObject.entrySet()) {
            String key = entry.getKey().toString().toUpperCase(Locale.US);
            Object value = entry.getValue();
            try {
                requestBody.put(key, value);
            } catch (JSONException e) {
                throw DMError.JSON.MAKE_REQUEST_BODY_JSON_ERROR
                        .newException(key + ":" + value);
            }
        }
        return requestBody;
    }

    public static OperationResult composeResultFromJson(JSONObject responseJSON,
                                                        OperationResult operationResult) throws RestException {
        OperationResult composeResult = composeResultFromJson(responseJSON);
        operationResult.setErrorCode(composeResult.getErrorCode());
        operationResult
                .setErrorDescription(composeResult.getErrorDescription());
        operationResult.setErrorSuggestion(composeResult.getErrorSuggestion());
        operationResult.setResultData(composeResult.getResultData());
        operationResult.setJsonResult(responseJSON);
        return operationResult;
    }

    public static Map<String, String> composeMapFromJson(JSONObject jsonObject) {
        TypeToken<Map<String, String>> typeToken = new MapTypeToken();
        Type type = typeToken.getType();
        Gson gson = new Gson();
        Map<String, String> dataMap = gson.fromJson(jsonObject.toString(),
                type);
        return dataMap;
    }

    public static OperationResult composeResultFromJson(JSONObject responseJSON)
            throws RestException {
        List<Map<String, String>> resultDataList = new ArrayList<Map<String, String>>();
        OperationResult operationResult = new OperationResult();
        try {
            JSONObject errJsonObject = responseJSON.getJSONObject("error");
            String errorCode = errJsonObject.get("code").toString();
            String errorDesc = errJsonObject.getString("description");
            String errorSuggest = "";
            if (errJsonObject.has("suggestion")) {
                errorSuggest = errJsonObject.getString("suggestion");
            }
            if (errorCode.equals("0") || errorCode.isEmpty()) {
                if (responseJSON.has("data")
                        && null != responseJSON.get("data")) {
                    Object data = responseJSON.get("data");
                    if (data instanceof JSONArray) {
                        JSONArray array = (JSONArray) data;
                        for (int i = 0; i < array.length(); i++) {
                            resultDataList.add(
                                    composeMapFromJson(array.getJSONObject(i)));
                        }
                    } else {
                        resultDataList
                                .add(composeMapFromJson((JSONObject) data));
                    }
                }
            } else {
                throw new RestException(errorCode, errorDesc, errorSuggest);
            }

            operationResult.setResultData(resultDataList);
            operationResult.setErrorCode(errorCode + "");
            operationResult.setErrorDescription(errorDesc);
        } catch (JSONException e) {
            log.error("composeResultFromJson::" + e.getMessage());
            throw DMError.JSON.RESPONSE_BODY_JSON_PARSE_ERROR.newException(e.getMessage());
        }
        return operationResult;
    }

}
