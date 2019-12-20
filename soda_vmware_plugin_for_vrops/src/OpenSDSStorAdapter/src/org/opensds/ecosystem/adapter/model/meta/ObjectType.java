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

package org.opensds.ecosystem.adapter.model.meta;

public enum ObjectType {
    /**
     * 控制器
     */
    Controller(207),
    
    /**
     * 硬盘域
     */
    DiskDomain(266),
    
    /**
     * 资源池
     */
    Pool(216),
    
    /**
     * 磁盘
     */
    Disk(10),
    
    /**
     * LUN信息
     */
    Lun(11),
    
    /**
     * 文件系统
     */
    FileSystem(40),
    
    /**
     * FC端口
     */
    FCPort(212),
    
    /**
     * iSCSI端口
     */
    ISCSIPort(213),
    
    /**
     * 远程复制
     */
    RemoteReplication(263),
    
    /**
     * 分区信息
     */
    SmartPartition(268),
    
    /**
     * Qos信息
     */
    SmartQos(230),
    
    /**
     * 拷贝信息
     */
    LunCopy(219),
    
    /**
     * 快照信息
     */
    SnapShot(27),

    /**
     * SAS端口
     */
    SASPort(214),

    /**
     * 主机信息
     */
    Host(21);
    
    private int id;
    
    private ObjectType(int id) {
        this.id = id;
    }
    
    public int getId() {
        return this.id;
    }
    
    /**
     * 转换字符串
     * @return String
     **/
    @Override
    public String toString() {
        return String.valueOf(this.id);
    }
}
