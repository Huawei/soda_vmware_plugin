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

package org.opensds.ecosystem.adapter.model;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.opensds.ecosystem.adapter.model.meta.DataType;
import org.opensds.ecosystem.adapter.model.meta.StorModel;

public class RemoteReplicationModel extends StorModel {
    // 远程复制和lun的关系
    private Map<String, List<String>> replication2LunShip;
    // 远程复制和文件系统的关系
    private Map<String, List<String>> replication2FileSystemShip;

    /**
     * 构造方法,初始化关系对象
     */
    public RemoteReplicationModel() {
        this.replication2FileSystemShip = new HashMap<String, List<String>>();
        this.replication2LunShip = new HashMap<String, List<String>>();
    }

    /**
     * 初始化
     **/
    public void init() {
        getDataTypes().add(DataType.LOGICAL_BANDWITH.getValue());
        getDataTypes().add(DataType.SYNCHRONIZATION_DURATION.getValue());
        getDataTypes().add(DataType.BandWidth.getValue());
        getDataTypes().add(DataType.AsyncTime.getValue());
    }

    public Map<String, List<String>> getReplication2LunShip() {
        return replication2LunShip;
    }

    public void setReplication2LunShip(
            Map<String, List<String>> replication2LunShip) {
        this.replication2LunShip = replication2LunShip;
    }

    public Map<String, List<String>> getReplication2FileSystemShip() {
        return replication2FileSystemShip;
    }

    public void setReplication2FileSystemShip(
            Map<String, List<String>> replication2FileSystemShip) {
        this.replication2FileSystemShip = replication2FileSystemShip;
    }

}
