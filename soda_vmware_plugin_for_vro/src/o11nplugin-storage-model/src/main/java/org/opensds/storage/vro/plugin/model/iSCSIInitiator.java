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

public class iSCSIInitiator {
    private String id;
    
    private String real_id;
    
    private String name;
    
    private String discoveryvermode;
    
    private String normalvermode;
    
    private String failovermode;
    
    private String pathtype;

    private String parentid;

    private String parentname;

    private String runningstatus;

    public String getRunningstatus() {
        return runningstatus;
    }

    public void setRunningstatus(String runningstatus) {
        this.runningstatus = runningstatus;
    }

    public String getParentname() {
        return parentname;
    }

    public void setParentname(String parentname) {
        this.parentname = parentname;
    }

    public String getParentid() {
        return parentid;
    }

    public void setParentid(String parentid) {
        this.parentid = parentid;
    }

    public String getId() {
        return id;
    }
    
    public void setId(String id) {
        this.id = id;
    }
    
    public String getReal_id() {
        return real_id;
    }
    
    public void setReal_id(String real_id) {
        this.real_id = real_id;
    }
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getDiscoveryvermode() {
        return discoveryvermode;
    }
    
    public void setDiscoveryvermode(String discoveryvermode) {
        this.discoveryvermode = discoveryvermode;
    }
    
    public String getNormalvermode() {
        return normalvermode;
    }
    
    public void setNormalvermode(String normalvermode) {
        this.normalvermode = normalvermode;
    }
    
    public String getFailovermode() {
        return failovermode;
    }
    
    public void setFailovermode(String failovermode) {
        this.failovermode = failovermode;
    }
    
    public String getPathtype() {
        return pathtype;
    }
    
    public void setPathtype(String pathtype) {
        this.pathtype = pathtype;
    }
}