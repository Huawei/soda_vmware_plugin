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

public class LUNSnapshot {
    private String real_id;

    private String id;

    private String name;

    private String parenttype;

    private String parentid;

    private String parentname;

    private String healthstatus;

    private String runningstatus;

    private String description;

    private String subtype;

    private String snapshotuser;

    private String timestamp;

    private String rollbackstarttime;

    private String rollbackendtime;

    private String rollbackspeed;

    private String rollbackrate;

    private String exposedtoinitiator;

    private String wwn;

    private String usercapacity;

    private String consumedcapacity;

    private String workingcontroller;

    private String ioclassid;

    private String iopriority;

    private String replicationcapacity;

    public String getReal_id() {
        return real_id;
    }

    public void setReal_id(String real_id) {
        this.real_id = real_id;
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

    public String getSnapshotuser() {
        return snapshotuser;
    }

    public void setSnapshotuser(String snapshotuser) {
        this.snapshotuser = snapshotuser;
    }

    public String getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(String timestamp) {
        this.timestamp = timestamp;
    }

    public String getRollbackstarttime() {
        return rollbackstarttime;
    }

    public void setRollbackstarttime(String rollbackstarttime) {
        this.rollbackstarttime = rollbackstarttime;
    }

    public String getRollbackendtime() {
        return rollbackendtime;
    }

    public void setRollbackendtime(String rollbackendtime) {
        this.rollbackendtime = rollbackendtime;
    }

    public String getRollbackspeed() {
        return rollbackspeed;
    }

    public void setRollbackspeed(String rollbackspeed) {
        this.rollbackspeed = rollbackspeed;
    }

    public String getRollbackrate() {
        return rollbackrate;
    }

    public void setRollbackrate(String rollbackrate) {
        this.rollbackrate = rollbackrate;
    }

    public String getExposedtoinitiator() {
        return exposedtoinitiator;
    }

    public void setExposedtoinitiator(String exposedtoinitiator) {
        this.exposedtoinitiator = exposedtoinitiator;
    }

    public String getWwn() {
        return wwn;
    }

    public void setWwn(String wwn) {
        this.wwn = wwn;
    }

    public String getUsercapacity() {
        return usercapacity;
    }

    public void setUsercapacity(String usercapacity) {
        this.usercapacity = usercapacity;
    }

    public String getConsumedcapacity() {
        return consumedcapacity;
    }

    public void setConsumedcapacity(String consumedcapacity) {
        this.consumedcapacity = consumedcapacity;
    }

    public String getWorkingcontroller() {
        return workingcontroller;
    }

    public void setWorkingcontroller(String workingcontroller) {
        this.workingcontroller = workingcontroller;
    }

    public String getIoclassid() {
        return ioclassid;
    }

    public void setIoclassid(String ioclassid) {
        this.ioclassid = ioclassid;
    }

    public String getIopriority() {
        return iopriority;
    }

    public void setIopriority(String iopriority) {
        this.iopriority = iopriority;
    }

    public String getReplicationcapacity() {
        return replicationcapacity;
    }

    public void setReplicationcapacity(String replicationcapacity) {
        this.replicationcapacity = replicationcapacity;
    }
}