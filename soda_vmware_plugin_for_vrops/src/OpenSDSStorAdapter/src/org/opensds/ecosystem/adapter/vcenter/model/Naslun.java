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

public class Naslun {
    private String nfsHost;

    private String nfsPath;

    public String getNfsHost() {
        return nfsHost;
    }

    public void setNfsHost(String nfsHost) {
        this.nfsHost = nfsHost;
    }

    public String getNfsPath() {
        return nfsPath;
    }

    public void setNfsPath(String nfsPath) {
        this.nfsPath = nfsPath;
    }

    /**
     * ×ª»»³É×Ö·û´®
     *
     * @return String
     **/
    @Override
    public String toString() {
        return "Naslun [nfsHost=" + nfsHost + ", nfsPath=" + nfsPath + "]";
    }
}
