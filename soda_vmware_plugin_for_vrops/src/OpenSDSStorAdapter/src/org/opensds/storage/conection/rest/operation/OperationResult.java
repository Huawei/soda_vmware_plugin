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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class OperationResult {
    private String errorCode = "0";
    private String errorDescription;
    private String errorSuggestion;
    private List<Map<String, String>> resultData = new ArrayList<Map<String, String>>();

    public String getErrorCode() {
        return errorCode;
    }

    public void setErrorCode(String errorCode) {
        this.errorCode = errorCode;
    }

    public String getErrorDescription() {
        return errorDescription;
    }

    public void setErrorDescription(String errorDescription) {
        this.errorDescription = errorDescription;
    }

    public String getErrorSuggestion() {
        return errorSuggestion;
    }

    public void setErrorSuggestion(String errorSuggest) {
        this.errorSuggestion = errorSuggest;
    }

    public List<Map<String, String>> getResultData() {
        return resultData;
    }

    public void setResultData(List<Map<String, String>> resultData) {
        this.resultData = resultData;
    }

    @Override
    public String toString() {
        return "Rest OperationResult is :" + "[ErrorCode is : " + errorCode + ",ErrorDescription is "
                + errorDescription + ",ErrorSuggestion is " + errorSuggestion + ",Result data size is "
                + resultData.size() + ",Result data is " + resultData + "]";
    }

    public void addEmptyResult() {
        List<Map<String, String>> list = new ArrayList<Map<String, String>>();
        list.add(new HashMap<String, String>());
        setResultData(list);
    }
}
