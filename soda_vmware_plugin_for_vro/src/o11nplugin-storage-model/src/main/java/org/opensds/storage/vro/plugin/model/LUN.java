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

public class LUN {
    private String id;

    private String real_id;

    private String name;

    private String capacity;

    private String parentid;

    private String alloctype;

    private String description;

    private String owningcontroller;

    private String readcachepolicy;

    private String writecachepolicy;

    private String prefetchpolicy;

    private String prefetchvalue;

    private String datatransferpolicy;

    private String iopriority;

    private String wwn;

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

    public String getCapacity() {
        return capacity;
    }

    public void setCapacity(String capacity) {
        this.capacity = capacity;
    }

    public String getParentid() {
        return parentid;
    }

    public void setParentid(String parentid) {
        this.parentid = parentid;
    }

    public String getAlloctype() {
        return alloctype;
    }

    public void setAlloctype(String alloctype) {
        this.alloctype = alloctype;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getOwningcontroller() {
        return owningcontroller;
    }

    public void setOwningcontroller(String owningcontroller) {
        this.owningcontroller = owningcontroller;
    }

    public String getReadcachepolicy() {
        return readcachepolicy;
    }

    public void setReadcachepolicy(String readcachepolicy) {
        this.readcachepolicy = readcachepolicy;
    }

    public String getWritecachepolicy() {
        return writecachepolicy;
    }

    public void setWritecachepolicy(String writecachepolicy) {
        this.writecachepolicy = writecachepolicy;
    }

    public String getPrefetchpolicy() {
        return prefetchpolicy;
    }

    public void setPrefetchpolicy(String prefetchpolicy) {
        this.prefetchpolicy = prefetchpolicy;
    }

    public String getPrefetchvalue() {
        return prefetchvalue;
    }

    public void setPrefetchvalue(String prefetchvalue) {
        this.prefetchvalue = prefetchvalue;
    }

    public String getDatatransferpolicy() {
        return datatransferpolicy;
    }

    public void setDatatransferpolicy(String datatransferpolicy) {
        this.datatransferpolicy = datatransferpolicy;
    }

    public String getIopriority() {
        return iopriority;
    }

    public void setIopriority(String iopriority) {
        this.iopriority = iopriority;
    }

    public String getWwn() {
        return wwn;
    }

    public void setWwn(String wwn) {
        this.wwn = wwn;
    }
}