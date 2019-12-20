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
import com.integrien.alive.common.adapter3.ResourceKey;

public class FileSystemModel extends StorModel {
    private Map<String, List<String>> fs2ControllerShip;

    private Map<String, List<String>> fs2PoolShip;

    private Map<String, List<String>> fs2PartitionShip;

    private Map<String, ResourceKey> nasMap;

    /**
     * 构造方法,初始化相关对象
     */
    public FileSystemModel() {
        this.fs2ControllerShip = new HashMap<String, List<String>>();
        this.fs2PartitionShip = new HashMap<String, List<String>>();
        this.fs2PoolShip = new HashMap<String, List<String>>();
        this.nasMap = new HashMap<String, ResourceKey>();
    }

    /**
     * 初始化
     **/
    public void init() {
        getDataTypes().add(DataType.OPS.getValue());
        getDataTypes().add(DataType.ServiceTime.getValue());
        getDataTypes().add(DataType.ReadOPS.getValue());
        getDataTypes().add(DataType.ReadBandWidth.getValue());
        getDataTypes().add(DataType.AVERAGE_READ_OPS_RESPONSE_TIME.getValue());
        getDataTypes().add(DataType.WriteOPS.getValue());
        getDataTypes().add(DataType.WriteBandWidth.getValue());
        getDataTypes().add(DataType.AVERAGE_WRITE_OPS_RESPONSE_TIME.getValue());
    }

    public Map<String, ResourceKey> getNasMap() {
        return nasMap;
    }

    public void setNasMap(Map<String, ResourceKey> nasMap) {
        this.nasMap = nasMap;
    }

    public Map<String, List<String>> getFs2ControllerShip() {
        return fs2ControllerShip;
    }

    public void setFs2ControllerShip(
            Map<String, List<String>> fs2ControllerShip) {
        this.fs2ControllerShip = fs2ControllerShip;
    }

    public Map<String, List<String>> getFs2PoolShip() {
        return fs2PoolShip;
    }

    public void setFs2PoolShip(Map<String, List<String>> fs2PoolShip) {
        this.fs2PoolShip = fs2PoolShip;
    }

    public Map<String, List<String>> getFs2PartitionShip() {
        return fs2PartitionShip;
    }

    public void setFs2PartitionShip(Map<String, List<String>> fs2PartitionShip) {
        this.fs2PartitionShip = fs2PartitionShip;
    }

}
