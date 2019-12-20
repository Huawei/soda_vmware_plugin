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

public class StorData {
    private int version;

    private String deviceSn;

    private String deviceName;

    private HostModel hostData;

    private ControllerModel controllerData;

    private DiskDomainModel diskDomainData;

    private PoolModel poolData;

    private DiskModel diskData;

    private LunModel lunData;

    private FileSystemModel fileSystemData;

    private FCPortModel fcPortData;

    private ISCSIPortModel iscsiPortData;

    private RemoteReplicationModel remoteReplicationModel;

    private SnapShotModel snapShotModel;

    private SmartPartitionModel smartPartitionModel;

    private SmartQosModel smartQosModel;

    /**
     * 存储对象模型
     */
    public StorData() {
        this.hostData = new HostModel();
        this.controllerData = new ControllerModel();
        this.diskDomainData = new DiskDomainModel();
        this.poolData = new PoolModel();
        this.diskData = new DiskModel();
        this.lunData = new LunModel();
        this.fileSystemData = new FileSystemModel();
        this.fcPortData = new FCPortModel();
        this.iscsiPortData = new ISCSIPortModel();
        this.remoteReplicationModel = new RemoteReplicationModel();
        this.snapShotModel = new SnapShotModel();
        this.smartPartitionModel = new SmartPartitionModel();
        this.smartQosModel = new SmartQosModel();
    }

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }

    public String getDeviceSn() {
        return deviceSn;
    }

    public void setDeviceSn(String deviceSn) {
        this.deviceSn = deviceSn;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public HostModel getHostData() {
        return hostData;
    }

    public void setHostData(HostModel hostData) {
        this.hostData = hostData;
    }

    public ControllerModel getControllerData() {
        return controllerData;
    }

    public void setControllerData(ControllerModel controllerData) {
        this.controllerData = controllerData;
    }

    public DiskDomainModel getDiskDomainData() {
        return diskDomainData;
    }

    public void setDiskDomainData(DiskDomainModel diskDomainData) {
        this.diskDomainData = diskDomainData;
    }

    public PoolModel getPoolData() {
        return poolData;
    }

    public void setPoolData(PoolModel poolData) {
        this.poolData = poolData;
    }

    public DiskModel getDiskData() {
        return diskData;
    }

    public void setDiskData(DiskModel diskData) {
        this.diskData = diskData;
    }

    public LunModel getLunData() {
        return lunData;
    }

    public void setLunData(LunModel lunData) {
        this.lunData = lunData;
    }

    public FileSystemModel getFileSystemData() {
        return fileSystemData;
    }

    public void setFileSystemData(FileSystemModel fileSystemData) {
        this.fileSystemData = fileSystemData;
    }

    public FCPortModel getFcPortData() {
        return fcPortData;
    }

    public void setFcPortData(FCPortModel fcPortData) {
        this.fcPortData = fcPortData;
    }

    public ISCSIPortModel getIscsiPortData() {
        return iscsiPortData;
    }

    public void setIscsiPortData(ISCSIPortModel iscsiPortData) {
        this.iscsiPortData = iscsiPortData;
    }

    public RemoteReplicationModel getRemoteReplicationModel() {
        return remoteReplicationModel;
    }

    public void setRemoteReplicationModel(
            RemoteReplicationModel remoteReplicationModel) {
        this.remoteReplicationModel = remoteReplicationModel;
    }

    public SnapShotModel getSnapShotModel() {
        return snapShotModel;
    }

    public void setSnapShotModel(SnapShotModel snapShotModel) {
        this.snapShotModel = snapShotModel;
    }

    public SmartPartitionModel getSmartPartitionModel() {
        return smartPartitionModel;
    }

    public void setSmartPartitionModel(
            SmartPartitionModel smartPartitionModel) {
        this.smartPartitionModel = smartPartitionModel;
    }

    public SmartQosModel getSmartQosModel() {
        return smartQosModel;
    }

    public void setSmartQosModel(SmartQosModel smartQosModel) {
        this.smartQosModel = smartQosModel;
    }

}
