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

import java.util.HashMap;
import java.util.Map;

public class ResourceModel {
    private String resourceId;

    private String resourceName;

    private Map<Integer, Integer> resourceData = new HashMap<Integer, Integer>();

    public String getResourceId() {
        return resourceId;
    }

    public void setResourceId(String resourceId) {
        this.resourceId = resourceId;
    }

    public String getResourceName() {
        return resourceName;
    }

    public void setResourceName(String resourceName) {
        this.resourceName = resourceName;
    }

    public Map<Integer, Integer> getResourceData() {
        return resourceData;
    }

    public void setResourceData(Map<Integer, Integer> resourceData) {
        this.resourceData = resourceData;
    }

    /**
     * ×ª»»³É×Ö·û´®
     *
     * @return String
     **/
    @Override
    public String toString() {
        StringBuffer stringBuffer = new StringBuffer();
        stringBuffer.append("{id:").append(getResourceId()).append(',');
        stringBuffer.append("name:").append(getResourceName()).append(",data:{");
        for (Map.Entry<Integer, Integer> entry : resourceData.entrySet()) {
            stringBuffer.append("[key:").append(entry.getKey()).append(',');
            stringBuffer.append("value:").append(entry.getValue()).append(']');
        }
        stringBuffer.append('}');
        return stringBuffer.toString();
    }

}
