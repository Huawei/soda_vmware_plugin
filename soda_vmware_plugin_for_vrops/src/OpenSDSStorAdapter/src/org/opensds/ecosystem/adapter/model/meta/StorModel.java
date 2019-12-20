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

package org.opensds.ecosystem.adapter.model.meta;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class StorModel {
    private List<String> resourceIdList = new ArrayList<String>();
    
    private Map<String, ResourceModel> dataList = new HashMap<String, ResourceModel>();
    
    private Set<Integer> dataTypes = new HashSet<Integer>();
    
    /**
     * 初始化
     */
    public StorModel() {
        this.init();
    }
    
    public List<String> getResourceIdList() {
        return resourceIdList;
    }
    
    public void setResourceIdList(List<String> resourceIdList) {
        this.resourceIdList = resourceIdList;
    }
    
    public Set<Integer> getDataTypes() {
        return dataTypes;
    }
    
    public Map<String, ResourceModel> getDataList() {
        return dataList;
    }
    
    public void setDataList(Map<String, ResourceModel> dataList) {
        this.dataList = dataList;
    }
    
    /**
    * 获取数据
    * @param resourceId ResourceId
    * @return resourceId
    */
    public ResourceModel getDataByResource(String resourceId) {
        return getDataList().get(resourceId);
    }
    
    /**
    * 获取数据类型ID
    * @param sourceId SourceId
    * @param dataTypeId DataTypeId
    * @return dataTypeId
    */
    public Integer getDataByResource2DataType(String sourceId, Integer dataTypeId) {
        ResourceModel resourceModel = getDataList().get(sourceId);
        if (resourceModel == null) {
            return null;
        }
        return resourceModel.getResourceData().get(dataTypeId);
    }
    
    /**
    * 初始化
    */
    public void init() {
    }
    
    /**
     * 转换字符串
     * @return String
     **/
    @Override
    public String toString() {
        StringBuffer stringBuffer = new StringBuffer();
        for (Map.Entry<String, ResourceModel> entry : dataList.entrySet()) {
            stringBuffer.append('\r').append(entry.toString());
        }
        return stringBuffer.toString();
    }
}
