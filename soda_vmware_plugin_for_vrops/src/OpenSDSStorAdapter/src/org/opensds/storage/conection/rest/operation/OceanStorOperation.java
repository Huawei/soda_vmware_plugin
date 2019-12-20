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

import org.opensds.storage.conection.rest.constants.OperationNamesEnum;

import java.util.HashMap;
import java.util.Map;

public class OceanStorOperation {
    /**
     * Operation name
     */
    public String operationName;
    /**
     * Operation data
     */
    public Map<String, String> operationData = new HashMap<String, String>();
    /**
     * Query data
     */
    public Map<String, String> queryData = new HashMap<String, String>();
    /**
     * Filter data
     */
    public Map<String, String> filterData = new HashMap<String, String>();

    /**
     * Get operation data
     * @return Operation data
     */
    public Map<String, String> getOperationData() {
        return operationData;
    }

    /**
     * Put operation data
     * @param key Operation data key
     * @param value Operation data value
     */
    public void putOperationData(String key, String value) {
        this.operationData.put(key, value);
    }

    /**
     * Get Query data
     * @return Query data
     */
    public Map<String, String> getQueryData() {
        return queryData;
    }

    /**
     * Put query data
     * @param key Query data key
     * @param value Query data value
     */
    public void putQueryData(String key, String value) {
        this.queryData.put(key, value);
    }

    public Map<String, String> getFilterData() {
        return filterData;
    }

    /**
     * Put filter data
     * @param key Filter data key
     * @param value Filter data value
     */
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
        return "the operation is : operationName : " + operationName + " operationData is " + operationData;
    }
}
