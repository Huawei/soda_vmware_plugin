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

public class VMware {
    private String vmwareVal;

    private String vmwareVmname;

    private String annotation;

    // 最大连接数
    private Integer maxMksConnections;

    // 最大cpu使用
    private Integer maxCpuUsage;

    // 最大内存占用
    private Integer maxMemoryUsage;

    // 内存开销
    private long memoryOverhead;

    public String getVmwareVal() {
        return vmwareVal;
    }

    public void setVmwareVal(String vmwareVal) {
        this.vmwareVal = vmwareVal;
    }

    public String getVmwareVmname() {
        return vmwareVmname;
    }

    public void setVmwareVmname(String vmwareVmname) {
        this.vmwareVmname = vmwareVmname;
    }

    public String getAnnotation() {
        return annotation;
    }

    public void setAnnotation(String annotation) {
        this.annotation = annotation;
    }

    public Integer getMaxMksConnections() {
        return maxMksConnections;
    }

    public void setMaxMksConnections(Integer maxMksConnections) {
        this.maxMksConnections = maxMksConnections;
    }

    public Integer getMaxCpuUsage() {
        return maxCpuUsage;
    }

    public void setMaxCpuUsage(Integer maxCpuUsage) {
        this.maxCpuUsage = maxCpuUsage;
    }

    public Integer getMaxMemoryUsage() {
        return maxMemoryUsage;
    }

    public void setMaxMemoryUsage(Integer maxMemoryUsage) {
        this.maxMemoryUsage = maxMemoryUsage;
    }

    public long getMemoryOverhead() {
        return memoryOverhead;
    }

    public void setMemoryOverhead(long memoryOverhead) {
        this.memoryOverhead = memoryOverhead;
    }

    /**
     * 转换字符串
     *
     * @return String
     */
    @Override
    public String toString() {
        return "vmware [vmwareVal=" + vmwareVal + ", vmwareVmname=" + vmwareVmname + ", annotation="
                + annotation + ", maxMksConnections=" + maxMksConnections + ", maxCpuUsage=" + maxCpuUsage
                + ", maxMemoryUsage=" + maxMemoryUsage + ", memoryOverhead=" + memoryOverhead + "]";
    }
}
