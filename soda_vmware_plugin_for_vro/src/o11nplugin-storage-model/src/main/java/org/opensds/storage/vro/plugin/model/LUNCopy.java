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

public class LUNCopy {
    private String real_id;

    private String type;

    private String id;

    private String name;

    private String healthstatus;

    private String runningstatus;

    private String description;

    private String subtype;

    private String copyspeed;

    private String luncopytype;

    private String copyprogress;

    private String copystarttime;

    private String copystoptime;

    private String sourcelun;

    private String sourcelunname;

    private String sourcelunwwn;

    private String sourcelunwwpn;

    private String sourceluncapacity;

    private String sourceluncapacitybyte;

    private String sourcedeviceid;

    private String sourcedevicename;

    private String baselun;

    public String getReal_id() {
        return real_id;
    }

    public void setReal_id(String real_id) {
        this.real_id = real_id;
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

    public String getCopyspeed() {
        return copyspeed;
    }

    public void setCopyspeed(String copyspeed) {
        this.copyspeed = copyspeed;
    }

    public String getLuncopytype() {
        return luncopytype;
    }

    public void setLuncopytype(String luncopytype) {
        this.luncopytype = luncopytype;
    }

    public String getCopyprogress() {
        return copyprogress;
    }

    public void setCopyprogress(String copyprogress) {
        this.copyprogress = copyprogress;
    }

    public String getCopystarttime() {
        return copystarttime;
    }

    public void setCopystarttime(String copystarttime) {
        this.copystarttime = copystarttime;
    }

    public String getCopystoptime() {
        return copystoptime;
    }

    public void setCopystoptime(String copystoptime) {
        this.copystoptime = copystoptime;
    }

    public String getSourcelun() {
        return sourcelun;
    }

    public void setSourcelun(String sourcelun) {
        this.sourcelun = sourcelun;
    }

    public String getSourcelunname() {
        return sourcelunname;
    }

    public void setSourcelunname(String sourcelunname) {
        this.sourcelunname = sourcelunname;
    }

    public String getSourcelunwwn() {
        return sourcelunwwn;
    }

    public void setSourcelunwwn(String sourcelunwwn) {
        this.sourcelunwwn = sourcelunwwn;
    }

    public String getSourcelunwwpn() {
        return sourcelunwwpn;
    }

    public void setSourcelunwwpn(String sourcelunwwpn) {
        this.sourcelunwwpn = sourcelunwwpn;
    }

    public String getSourceluncapacity() {
        return sourceluncapacity;
    }

    public void setSourceluncapacity(String sourceluncapacity) {
        this.sourceluncapacity = sourceluncapacity;
    }

    public String getSourceluncapacitybyte() {
        return sourceluncapacitybyte;
    }

    public void setSourceluncapacitybyte(String sourceluncapacitybyte) {
        this.sourceluncapacitybyte = sourceluncapacitybyte;
    }

    public String getSourcedeviceid() {
        return sourcedeviceid;
    }

    public void setSourcedeviceid(String sourcedeviceid) {
        this.sourcedeviceid = sourcedeviceid;
    }

    public String getSourcedevicename() {
        return sourcedevicename;
    }

    public void setSourcedevicename(String sourcedevicename) {
        this.sourcedevicename = sourcedevicename;
    }

    public String getBaselun() {
        return baselun;
    }

    public void setBaselun(String baselun) {
        this.baselun = baselun;
    }
}