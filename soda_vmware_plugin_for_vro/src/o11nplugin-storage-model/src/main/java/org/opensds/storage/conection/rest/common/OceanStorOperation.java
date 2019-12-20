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

import java.util.HashMap;
import java.util.Map;

public class OceanStorOperation {
    public String operationName;

    public Map<String, String> operationData = new HashMap<String, String>();

    public Map<String, Object> operationDataObject = new HashMap<String, Object>();

    public Map<String, String> queryData = new HashMap<String, String>();

    public Map<String, String> filterData = new HashMap<String, String>();

    private boolean swithIP = true;

    public boolean isSwithIP() {
        return swithIP;
    }

    public void setSwithIP(boolean swithIP) {
        this.swithIP = swithIP;
    }

    public Map<String, String> getOperationData() {
        return operationData;
    }

    public Map<String, Object> getOperationDataObject() {
        return operationDataObject;
    }

    public void putOperationData(String key, String value) {
        if (value != null && value.trim().length() != 0) {
            this.operationData.put(key, value.trim());
        }
    }

    public void putOperationDataObject(String key, Object value) {
        if (value != null) {
            this.operationDataObject.put(key, value);
        }
    }

    public Map<String, String> getQueryData() {
        return queryData;
    }

    public void putQueryData(String key, String value) {
        this.queryData.put(key, value);
    }

    public Map<String, String> getFilterData() {
        return filterData;
    }

    public void putFilterData(String key, String value) {
        this.filterData.put(key, value);
    }

    public String getOperationName() {
        return operationName;
    }

    public void setOperationName(String operationName) {
        this.operationName = operationName;
    }

    public void setOperationName(OperationNamesEnum operationName) {
        this.operationName = operationName.getValue();
    }

    @Override
    public String toString() {
        return "the operation is : operationName : " + operationName
                + " operationData is " + operationData;
    }

}
