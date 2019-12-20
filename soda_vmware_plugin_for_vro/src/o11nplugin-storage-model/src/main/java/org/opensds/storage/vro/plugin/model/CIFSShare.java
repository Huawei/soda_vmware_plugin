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

public class CIFSShare {
    private String id;

    private String real_id;

    private String name;

    private String description;

    private String sharepath;

    private String enableoplock;

    private String enablenotify;

    private String enableca;

    private String offlinefilemode;

    private String enableipcontrol;

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

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getSharepath() {
        return sharepath;
    }

    public void setSharepath(String sharepath) {
        this.sharepath = sharepath;
    }

    public String getEnableoplock() {
        return enableoplock;
    }

    public void setEnableoplock(String enableoplock) {
        this.enableoplock = enableoplock;
    }

    public String getEnablenotify() {
        return enablenotify;
    }

    public void setEnablenotify(String enablenotify) {
        this.enablenotify = enablenotify;
    }

    public String getEnableca() {
        return enableca;
    }

    public void setEnableca(String enableca) {
        this.enableca = enableca;
    }

    public String getOfflinefilemode() {
        return offlinefilemode;
    }

    public void setOfflinefilemode(String offlinefilemode) {
        this.offlinefilemode = offlinefilemode;
    }

    public String getEnableipcontrol() {
        return enableipcontrol;
    }

    public void setEnableipcontrol(String enableipcontrol) {
        this.enableipcontrol = enableipcontrol;
    }
}