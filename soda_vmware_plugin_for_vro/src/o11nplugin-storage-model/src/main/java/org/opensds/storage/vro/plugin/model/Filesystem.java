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

public class Filesystem {
    private String real_id;

    private String parentsnapshotid;

    private String parentfilesystemid;

    private String enablededupcheck;

    private String type;

    private String id;

    private String name;

    private String enablededup;

    private String recycleswitch;

    private String ioclassid;

    private String capacitythreshold;

    private String parenttype;

    private String parentid;

    private String parentname;

    private String healthstatus;

    private String runningstatus;

    private String description;

    private String subtype;

    private String alloctype;

    private String root;

    private String pathnameseparatorstring;

    public String getReal_id() {
        return real_id;
    }

    public void setReal_id(String real_id) {
        this.real_id = real_id;
    }

    public String getParentsnapshotid() {
        return parentsnapshotid;
    }

    public void setParentsnapshotid(String parentsnapshotid) {
        this.parentsnapshotid = parentsnapshotid;
    }

    public String getParentfilesystemid() {
        return parentfilesystemid;
    }

    public void setParentfilesystemid(String parentfilesystemid) {
        this.parentfilesystemid = parentfilesystemid;
    }

    public String getEnablededupcheck() {
        return enablededupcheck;
    }

    public void setEnablededupcheck(String enablededupcheck) {
        this.enablededupcheck = enablededupcheck;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getEnablededup() {
        return enablededup;
    }

    public void setEnablededup(String enablededup) {
        this.enablededup = enablededup;
    }

    public String getRecycleswitch() {
        return recycleswitch;
    }

    public void setRecycleswitch(String recycleswitch) {
        this.recycleswitch = recycleswitch;
    }

    public String getIoclassid() {
        return ioclassid;
    }

    public void setIoclassid(String ioclassid) {
        this.ioclassid = ioclassid;
    }

    public String getCapacitythreshold() {
        return capacitythreshold;
    }

    public void setCapacitythreshold(String capacitythreshold) {
        this.capacitythreshold = capacitythreshold;
    }

    public String getParenttype() {
        return parenttype;
    }

    public void setParenttype(String parenttype) {
        this.parenttype = parenttype;
    }

    public String getParentid() {
        return parentid;
    }

    public void setParentid(String parentid) {
        this.parentid = parentid;
    }

    public String getParentname() {
        return parentname;
    }

    public void setParentname(String parentname) {
        this.parentname = parentname;
    }

    public String getHealthstatus() {
        return healthstatus;
    }

    public void setHealthstatus(String healthstatus) {
        this.healthstatus = healthstatus;
    }

    public String getRunningstatus() {
        return runningstatus;
    }

    public void setRunningstatus(String runningstatus) {
        this.runningstatus = runningstatus;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getSubtype() {
        return subtype;
    }

    public void setSubtype(String subtype) {
        this.subtype = subtype;
    }

    public String getAlloctype() {
        return alloctype;
    }

    public void setAlloctype(String alloctype) {
        this.alloctype = alloctype;
    }

    public String getRoot() {
        return root;
    }

    public void setRoot(String root) {
        this.root = root;
    }

    public String getPathnameseparatorstring() {
        return pathnameseparatorstring;
    }

    public void setPathnameseparatorstring(String pathnameseparatorstring) {
        this.pathnameseparatorstring = pathnameseparatorstring;
    }
}