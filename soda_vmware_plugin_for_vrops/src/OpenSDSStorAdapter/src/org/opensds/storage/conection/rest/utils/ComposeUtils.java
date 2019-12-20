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

package org.opensds.storage.conection.rest.utils;

import java.io.File;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import com.google.gson.JsonObject;
import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.domain.RestResponse;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.operation.OperationResult;
import org.opensds.storage.conection.rest.operation.RestInterfaceConfs;
import org.opensds.storage.extracdata.util.LogUtil;

public class ComposeUtils {
    private static final Logger LOG;

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        LOG = context.getLogger(ComposeUtils.class.getName());
    }

    /**
     * Compose relative uri
     * @param operationName Operation name
     * @param requestData Request data
     * @return Relative uri
     * @throws RestException
     */
    public static String composeRelativeUri(String operationName, Map<String, String> requestData)
            throws RestException {
        requestData = CommonUtils.upperOperationName(requestData);
        String restRelativePath = RestInterfaceConfs.getRestRelativePath(operationName);
        if (restRelativePath.contains("?")) {
            restRelativePath = makeQueryUri(requestData, restRelativePath);
        } else {
            restRelativePath = makeActionUri(requestData, restRelativePath);
        }
        return restRelativePath;
    }

    private static String makeActionUri(Map<String, String> requestData, String restRelativePath) throws RestException {
        String[] paths = restRelativePath.split(",");
        StringBuffer sf = new StringBuffer();
        for (String path : paths) {
            if (path.startsWith("${")) {
                // if require data not contained in requestDataMap throw
                // exception
                String varKey = (path.substring(2, path.length() - 1)).toUpperCase();
                if (requestData.get(varKey) == null) {
                    if (LOG.isInfoEnabled()) {
                        LOG.info("mandatory paramter " + varKey + "does not provided");
                    }
                    throw OperationError.REQUEST.MISSING_REQUIRED_PARAM.newException(varKey);
                }
                path = requestData.get(varKey);
            }
            sf.append("/").append(path);
        }
        restRelativePath = sf.toString();
        return restRelativePath;
    }

    private static String makeQueryUri(Map<String, String> requestData, String restRelativePath) {
        // filesystem?${filter=${filter},range=${range}}
        // snapshot,associate?${ASSOCIATEOBJTYPE=${ASSOCIATEOBJTYPE}}
        if (LOG.isDebugEnabled()) {
            LOG.debug("start make query uri relative path is " + restRelativePath);
        }
        StringBuffer sf = new StringBuffer();
        makePrePart(restRelativePath, sf);
        makePostPart(requestData, restRelativePath, sf);
        // if there is no additional query parameter,remove question mark
        if (sf.toString().endsWith("?")) {
            sf.deleteCharAt(sf.length() - 1);
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("after make the query uri is " + sf.toString());
        }
        return sf.toString();
    }

    private static void makePostPart(Map<String, String> requestData, String restRelativePath, StringBuffer sf) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("start make query uri post part");
        }
        // filesystem?${filter=${filter},range=${range}}
        String expression = restRelativePath.substring(restRelativePath.indexOf("?${") + 3,
                restRelativePath.length() - 1);
        String[] exps = expression.split(",");
        for (String exp : exps) {
            String[] equation = exp.split("=");
            String key = equation[0];
            String value = equation[1];
            if ("filter".equals(key)) {
                dealFilterCondition(requestData, sf, value);
                continue;
            }

            if (value.startsWith("${")) {
                value = getExpressionPayLoad(value);
                String[] split = value.split("}");
                value = split[0];
                // if request part not contain , just drop it

                value = value.toUpperCase();
                if (requestData.get(value) != null) {
                    sf.append(key).append("=").append(requestData.get(value)).append("&");
                }
            } else {
                sf.append(key).append("=").append(value).append("&");
            }
        }
        // if there is no more query condition,remove the last add mark
        if (sf.toString().endsWith("&")) {
            sf.deleteCharAt(sf.length() - 1);
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("after post part deal , the query uri is : " + sf.toString());
        }
    }

    private static String getExpressionPayLoad(String value) {
        // get string inside ${} part
        return value.substring(2, value.length() - 1);
    }

    private static void makePrePart(String restRelativePath, StringBuffer sf) {
        if (LOG.isDebugEnabled()) {
            LOG.debug("start make query uri start part " + restRelativePath);
        }
        String queryPre = restRelativePath.substring(0, restRelativePath.indexOf("?") + 1);
        if (queryPre.contains(",")) {
            String[] queryPres = queryPre.split(",");
            for (String pre : queryPres) {
                sf.append("/").append(pre);
            }
        } else {
            sf.append("/").append(queryPre);
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("after make query uri start part , the query uri is " + sf.toString());
        }
    }

    private static void dealFilterCondition(Map<String, String> requestData, StringBuffer sf, String value) {
        // remove ${}
        // Double " :: "
        // find-filesystem-by-name=GET;filesystem?${filter=${NAME::${name}}
        sf.append("filter=");
        String[] filterItems = value.substring(2, value.length() - 1).split(",");
        if (value.contains("::")) {
            for (String f : filterItems) {
                String[] condition = f.split("::");
                sf.append(condition[0]).append("::").append(requestData.get(condition[0])).append(",");
            }
            // single " : "
            // find-CIFS-share-by-name=GET;CIFSHARE?${filter=${NAME:${name},range=[0-100]}
        } else if (value.matches("[\\s\\S]+:[\\s\\S]+")) {
            for (String f : filterItems) {
                String[] equation = f.split(":");
                if (equation[1].startsWith("${")) {
                    String keyInRequest = equation[1].substring(2, equation[1].length() - 1);
                    sf.append(equation[0]).append(":").append(requestData.get(keyInRequest.toUpperCase())).append(",");
                } else {
                    sf.append(equation[0]).append(":").append(equation[1]).append(",");
                }
            }
        } else {
            if (value.startsWith("${")) {
                String reuqestKey = value.substring(2, value.length() - 1).toUpperCase();
                sf.append(requestData.get(reuqestKey)).append(",");
            }
        }

        if (sf.toString().endsWith(",")) {
            sf.deleteCharAt(sf.length() - 1).append("&");
        }
    }

    /**
     * Compose json from map
     * @param operationData Operation data
     * @return request json
     * @throws RestException
     */
    public static String composeJsonFromMap(Map<String, String> operationData) throws RestException {
        JsonObject jsonObject = new JsonObject();
        for (Entry<String, String> entry : operationData.entrySet()) {
            String key = entry.getKey().toString().toUpperCase().replace("-", "")
                    .replace("_", "");
            String value = entry.getValue();
            jsonObject.addProperty(key, value);
        }
        return jsonObject.toString();
    }

    /**
     * Compose result from rest
     * @param restResult Rest result
     * @return OperationResult
     */
    public static OperationResult composeResultFromRest(RestResponse restResult) {
        OperationResult operationResult = new OperationResult();
        operationResult.setErrorCode(restResult.getError().getCode());
        operationResult.setErrorDescription(restResult.getError().getDescription());
        operationResult.setErrorSuggestion(restResult.getError().getSuggestion());
        operationResult.setResultData(restResult.getData().getDataList());
        return operationResult;
    }
}
