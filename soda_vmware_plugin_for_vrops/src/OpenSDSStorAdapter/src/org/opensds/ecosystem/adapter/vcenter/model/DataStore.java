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

package org.opensds.ecosystem.adapter.vcenter.model;

import java.util.List;

public class DataStore {
    private String dateStoreName;

    private String dateStorevol;

    private long freeSpace;

    private long maxFileSize;

    private List<FSlun> fslun;

    private List<VMware> Vms;

    private Naslun naslun;

    public List<VMware> getVms() {
        return Vms;
    }

    public void setVms(List<VMware> Vms) {
        this.Vms = Vms;
    }

    public String getDateStoreName() {
        return dateStoreName;
    }

    public void setDateStoreName(String dateStoreName) {
        this.dateStoreName = dateStoreName;
    }

    public String getDateStorevol() {
        return dateStorevol;
    }

    public void setDateStorevol(String dateStorevol) {
        this.dateStorevol = dateStorevol;
    }

    public long getFreeSpace() {
        return freeSpace;
    }

    public void setFreeSpace(long freeSpace) {
        this.freeSpace = freeSpace;
    }

    public long getMaxFileSize() {
        return maxFileSize;
    }

    public void setMaxFileSize(long maxFileSize) {
        this.maxFileSize = maxFileSize;
    }

    public List<FSlun> getFslun() {
        return fslun;
    }

    public void setFslun(List<FSlun> fslun) {
        this.fslun = fslun;
    }

    public Naslun getNaslun() {
        return naslun;
    }

    public void setNaslun(Naslun naslun) {
        this.naslun = naslun;
    }

    /**
     * ×ª»»×Ö·û´®
     *
     * @return String
     **/
    @Override
    public String toString() {
        return "dataStore [dateStoreName=" + dateStoreName + ", dateStorevol=" + dateStorevol
                + ", freeSpace=" + freeSpace + ", maxFileSize=" + maxFileSize + ", fslun=" + fslun
                + ", Vms=" + Vms + ", naslun=" + naslun + "]";
    }
}
