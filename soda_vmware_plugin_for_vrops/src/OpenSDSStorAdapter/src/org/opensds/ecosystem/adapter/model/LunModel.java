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

public class LunModel extends StorModel {
    // lun和Pool的关系对象
    private Map<String, List<String>> lun2PoolShip;
    // lun和controller的关系模型
    private Map<String, List<String>> lun2ControllerShip;
    // lun和Partition的关系模型
    private Map<String, List<String>> lun2PartitionShip;

    private Map<String, ResourceKey> lunWWN;

    /**
     * 构造方法,初始化关系对象
     */
    public LunModel() {
        this.lun2ControllerShip = new HashMap<String, List<String>>();
        this.lun2PartitionShip = new HashMap<String, List<String>>();
        this.lun2PoolShip = new HashMap<String, List<String>>();
        this.lunWWN = new HashMap<String, ResourceKey>();
    }

    /**
     * 初始化
     **/
    public void init() {
        getDataTypes().add(DataType.BandWidth.getValue());
        getDataTypes().add(DataType.Throughput.getValue());
        getDataTypes().add(DataType.ReadBandWidth.getValue());
        getDataTypes().add(DataType.AverageReadIO.getValue());
        getDataTypes().add(DataType.ReadThroughput.getValue());
        getDataTypes().add(DataType.WriteBandWidth.getValue());
        getDataTypes().add(DataType.AverageWriteIO.getValue());
        getDataTypes().add(DataType.WirteThroughput.getValue());
        getDataTypes().add(DataType.AverageIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.MaxIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.MaxBandWidthWrite.getValue());
        getDataTypes().add(DataType.FetchRatio.getValue());
        getDataTypes().add(DataType.ReadCacheRatio.getValue());
        getDataTypes().add(DataType.WriteCacheHitRatio.getValue());
        getDataTypes().add(DataType.AverageReadIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.AverageWriteIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.AverageIOSize.getValue());
        getDataTypes().add(DataType.ReadPercent.getValue());
        getDataTypes().add(DataType.WritePercent.getValue());
        getDataTypes().add(DataType.HitPercent.getValue());
    }

    public Map<String, ResourceKey> getLunWWN() {
        return lunWWN;
    }

    public void setLunWWN(Map<String, ResourceKey> lunWWN) {
        this.lunWWN = lunWWN;
    }

    public Map<String, List<String>> getLun2PoolShip() {
        return lun2PoolShip;
    }

    public void setLun2PoolShip(Map<String, List<String>> lun2PoolShip) {
        this.lun2PoolShip = lun2PoolShip;
    }

    public Map<String, List<String>> getLun2ControllerShip() {
        return lun2ControllerShip;
    }

    public void setLun2ControllerShip(Map<String, List<String>> lun2ControllerShip) {
        this.lun2ControllerShip = lun2ControllerShip;
    }

    public Map<String, List<String>> getLun2PartitionShip() {
        return lun2PartitionShip;
    }

    public void setLun2PartitionShip(Map<String, List<String>> lun2PartitionShip) {
        this.lun2PartitionShip = lun2PartitionShip;
    }
}
