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

package org.opensds.storage.vro.plugin.model;

public class MappingInfo {
    private String lunname;

    private String lungroupname;

    private String hostname;

    private String hostgroupname;

    private String mappingviewname;

    private String id;

    private String lunwwn;

    public String getLunwwn() {
        return lunwwn;
    }

    public void setLunwwn(String lunwwn) {
        this.lunwwn = lunwwn;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getLunname() {
        return lunname;
    }

    public void setLunname(String lunname) {
        this.lunname = lunname;
    }

    public String getLungroupname() {
        return lungroupname;
    }

    public void setLungroupname(String lungroupname) {
        this.lungroupname = lungroupname;
    }

    public String getHostname() {
        return hostname;
    }

    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    public String getHostgroupname() {
        return hostgroupname;
    }

    public void setHostgroupname(String hostgroupname) {
        this.hostgroupname = hostgroupname;
    }

    public String getMappingviewname() {
        return mappingviewname;
    }

    public void setMappingviewname(String mappingviewname) {
        this.mappingviewname = mappingviewname;
    }
}