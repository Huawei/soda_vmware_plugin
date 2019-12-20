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

public class SmartQosModel extends StorModel {
    // qos和lun的关系对象
    private Map<String, List<String>> qos2LunShip;
    // qos和fileSystem关系对象
    private Map<String, List<String>> qos2FileSystemShip;

    /**
     * 构造方法
     */
    public SmartQosModel() {
        this.qos2FileSystemShip = new HashMap<String, List<String>>();
        this.qos2LunShip = new HashMap<String, List<String>>();
    }

    /**
     * 初始化
     **/
    public void init() {
        getDataTypes().add(DataType.MaxBandWidth.getValue());
        getDataTypes().add(DataType.Usage.getValue());
        getDataTypes().add(DataType.QueueSize.getValue());
        getDataTypes().add(DataType.BandWidth.getValue());
        getDataTypes().add(DataType.Throughput.getValue());
        getDataTypes().add(DataType.ReadBandWidth.getValue());
        getDataTypes().add(DataType.AverageReadIO.getValue());
        getDataTypes().add(DataType.ReadThroughput.getValue());
        getDataTypes().add(DataType.WriteBandWidth.getValue());
        getDataTypes().add(DataType.AverageWriteIO.getValue());
        getDataTypes().add(DataType.WirteThroughput.getValue());
        getDataTypes().add(DataType.ServiceTime.getValue());
        getDataTypes().add(DataType.AverageIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.MaxIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.AverageReadIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.AverageWriteIOResponseTimeUs.getValue());
        getDataTypes().add(DataType.AverageIOSize.getValue());
        getDataTypes().add(DataType.CompleteSCSICommandsPerSecond.getValue());
        getDataTypes().add(DataType.VerifyCommandsPerSecond.getValue());
        getDataTypes().add(DataType.ReadPercent.getValue());
        getDataTypes().add(DataType.WritePercent.getValue());
        getDataTypes().add(DataType.MaxIOPSsPerSec.getValue());
    }

    public Map<String, List<String>> getQos2LunShip() {
        return qos2LunShip;
    }

    public void setQos2LunShip(Map<String, List<String>> qos2LunShip) {
        this.qos2LunShip = qos2LunShip;
    }

    public Map<String, List<String>> getQos2FileSystemShip() {
        return qos2FileSystemShip;
    }

    public void setQos2FileSystemShip(Map<String, List<String>> qos2FileSystemShip) {
        this.qos2FileSystemShip = qos2FileSystemShip;
    }
}
