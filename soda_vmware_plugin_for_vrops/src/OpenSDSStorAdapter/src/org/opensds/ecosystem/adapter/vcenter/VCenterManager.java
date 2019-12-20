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

package org.opensds.ecosystem.adapter.vcenter;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang3.StringUtils;

import org.opensds.ecosystem.adapter.vcenter.model.FSlun;
import org.opensds.ecosystem.adapter.vcenter.model.DataStore;
import org.opensds.ecosystem.adapter.vcenter.model.VMware;
import org.opensds.ecosystem.adapter.vcenter.model.Naslun;
import org.opensds.ecosystem.adapter.vcenter.model.VCenterModule;
import org.opensds.storage.conection.rest.domain.ConnectionData;
import com.vmware.vim25.DatastoreInfo;
import com.vmware.vim25.HostNasVolume;
import com.vmware.vim25.HostScsiDiskPartition;
import com.vmware.vim25.NasDatastoreInfo;
import com.vmware.vim25.VirtualMachineConfigInfo;
import com.vmware.vim25.VirtualMachineRuntimeInfo;
import com.vmware.vim25.VmfsDatastoreInfo;
import com.vmware.vim25.mo.Datastore;
import com.vmware.vim25.mo.Folder;
import com.vmware.vim25.mo.InventoryNavigator;
import com.vmware.vim25.mo.ManagedEntity;
import com.vmware.vim25.mo.ServiceInstance;
import com.vmware.vim25.mo.VirtualMachine;

public final class VCenterManager {
    // 日志记录器
    private static final org.apache.log4j.Logger LOGGER = org.apache.log4j.Logger.getLogger(VCenterManager.class);

    // LUN WWN校验正则表达式
    private static final Pattern WWN_PATT = Pattern
            .compile("(?:(?:[\\da-zA-Z]{10})|(?:naa\\.))?([\\da-zA-Z]{32}|[\\da-zA-Z]{16})(?:[\\da-zA-Z]{12})?");

    /**
     * 私有化构造方法
     */
    private VCenterManager() {
    }

    /**
     * 从vCenter获取LUN，FileSystem等信息
     * 获取信息详细步骤：
     * 1、使用初始化配置的用户名和密码等连接vCenter服务
     * 2、通过vCenter服务提供的API查询所有的DataStore及DataStore上的VM
     * 3、通过VM关联的Disk信息获取到磁盘的WWN，用于识别磁盘来源于存储
     * 4、返回组装好的所有的DataStore和VM信息
     *
     * @param connData vCenter服务连接信息
     * @return VCenterModule
     * @throws IOException 使用通用异常处理
     * @remark create c90005773 5 Sep 2016 1.0.0
     * @see [VCenterManager、VCenterManager#getvCenterData]
     */
    public static VCenterModule getvCenterData(ConnectionData connData) throws IOException {
        URL url = new URL(connData.getHostURL());
        String userName = connData.getUsername();
        String passWord = connData.getPassword();
        ServiceInstance si = new ServiceInstance(url, userName, passWord, true);
        Folder rootFolder = si.getRootFolder();
        ManagedEntity[] managedEntity = new InventoryNavigator(rootFolder).searchManagedEntities("Datastore");
        ArrayList<DataStore> dataStoreList = new ArrayList<DataStore>();
        for (ManagedEntity operateEntity : managedEntity) {
            // 查找到所有的DataStore对象
            DataStore DataStore = new DataStore();
            Datastore dataStore = (Datastore) operateEntity;
            DatastoreInfo dsInfo = dataStore.getInfo();
            // 获取所有的DataStore和DataStore上的虚拟机
            getDatastoreAndVirtualMachineInfo(dataStoreList, DataStore, dataStore, dsInfo);
            // 获取DataStore上关联的LUN和文件系统
            getLUNWWNAndFileSystemPath(DataStore, dsInfo);
        }

        VCenterModule vCenterModule = new VCenterModule();
        vCenterModule.setVcid(si.getAboutInfo().getInstanceUuid());
        vCenterModule.setDataStore(dataStoreList);

        // 退出vCenter连接
        logout(si);
        return vCenterModule;
    }

    /**
     * 释放vRealize客户端和vCenter服务端连接
     *
     * @param si vCenter客户端连接实例
     * @remark create c90005773 5 Sep 2016 1.0.0
     */
    private static void logout(ServiceInstance si) {
        if (si != null) {
            try {
                si.getServerConnection().logout();
            } catch (Exception e) {
                LOGGER.error("vCenter Connection logout error" + e.getMessage());
            }
        }
    }

    /**
     * 获取DataStor上关联的磁盘的WWN，如果为文件系统，则获取文件系统的共享路径
     * 获取步骤说明：
     * 1、获取所有的DataStore，查询到所使用的磁盘分区
     * 2、查找磁盘分区名称，获取其中的WWN
     * 3、如果DataStore类型为NAS类型，则获取NAS共享的服务主机host和共享点路径path
     *
     * @param DataStore vRealize存储存储对象数据结构
     * @param dsInfo      vCenter上所有的DataStore对象
     */
    private static void getLUNWWNAndFileSystemPath(DataStore DataStore, DatastoreInfo dsInfo) {
        ArrayList<FSlun> lunList = new ArrayList<FSlun>();
        if (dsInfo instanceof VmfsDatastoreInfo) {
            VmfsDatastoreInfo vdsInfo = (VmfsDatastoreInfo) dsInfo;
            HostScsiDiskPartition[] extentArr = vdsInfo.getVmfs().getExtent();

            for (HostScsiDiskPartition diskPartition : extentArr) {
                String wwn = getWwnFromIdentifier(diskPartition.getDiskName());
                if (StringUtils.isNotBlank(wwn)) {
                    FSlun blockLun = new FSlun();
                    blockLun.setWwn(normalizeWwn(wwn));
                    lunList.add(blockLun);
                }
                DataStore.setFslun(lunList);
            }
        } else if (dsInfo instanceof NasDatastoreInfo) {
            HostNasVolume nasVolume = ((NasDatastoreInfo) dsInfo).getNas();
            if (null != nasVolume) {
                Naslun naslun = new Naslun();
                naslun.setNfsHost(nasVolume.getRemoteHost());
                naslun.setNfsPath(nasVolume.getRemotePath());
                DataStore.setNaslun(naslun);
            }
        } else {
            return;
        }
    }

    /**
     * 获取所有的DataStore和DataStore上的虚拟机
     * 详细步骤：
     * 1、通过DataStore对象获取到所有的VM
     * 2、同步获取VM虚拟机磁盘的容量等信息
     *
     * @param [dataStoreList] [自定义数据结构用于存储所有的DataStore对象]
     * @param [DataStore]   [自定义数据结构用户记录VM等相关信息]
     * @param [dataStore]     [vCenter DataStore对象]
     * @param [dsInfo]        [DataStore详情信息对象]
     * @remark create c90005773 5 Sep 2016 1.0.0
     */
    private static void getDatastoreAndVirtualMachineInfo(List<DataStore> dataStoreList, DataStore DataStore,
                                                          Datastore dataStore, DatastoreInfo dsInfo) {
        // Get VM info from vCenter
        VirtualMachine[] vms = dataStore.getVms();
        ArrayList<VMware> vmsList = new ArrayList<VMware>();
        for (VirtualMachine virtualMachine : vms) {
            VMware Vmware = new VMware();
            if (null != virtualMachine) {
                VirtualMachineConfigInfo config = virtualMachine.getConfig();
                Vmware.setVmwareVal(virtualMachine.getMOR().getVal());
                Vmware.setVmwareVmname(virtualMachine.getName());
                if (config != null) {
                    Vmware.setAnnotation(config.getAnnotation());
                    Vmware.setMaxMksConnections(config.getMaxMksConnections());
                }

                VirtualMachineRuntimeInfo runtime = virtualMachine.getRuntime();

                Vmware.setMaxCpuUsage(runtime.getMaxCpuUsage() == null
                        ? Integer.valueOf(0) : runtime.getMaxCpuUsage());
                Vmware.setMaxMemoryUsage(runtime.getMaxMemoryUsage() == null
                        ? Integer.valueOf(0) : runtime.getMaxMemoryUsage());
                Vmware.setMemoryOverhead(runtime.getMemoryOverhead() == null ? 0 : runtime.getMemoryOverhead());

                vmsList.add(Vmware);
            }
        }

        DataStore.setVms(vmsList);
        DataStore.setDateStoreName(dataStore.getName());
        DataStore.setDateStorevol(dataStore.getMOR().getVal());
        DataStore.setFreeSpace(dsInfo.getFreeSpace());
        DataStore.setMaxFileSize(dsInfo.getMaxFileSize());

        dataStoreList.add(DataStore);
    }

    /**
     * 获取虚拟机磁盘上的WWN信息
     *
     * @param diskName 虚拟机磁盘名称
     * @return WWN
     * @remark create c90005773 5 Sep 2016 1.0.0
     */
    public static String getWwnFromIdentifier(String diskName) {
        if (StringUtils.isNotBlank(diskName)) {
            Matcher matcher = WWN_PATT.matcher(diskName);
            if (matcher.matches()) {
                return matcher.group(1);
            }
        }
        return null;
    }

    /**
     * 格式化LUN WWN，处理掉特殊字符等
     *
     * @param [wwn] [从VM上获取的WWN]
     * @return [String] [disk WWN]
     */
    private static String normalizeWwn(String wwn) {
        return StringUtils.isEmpty(wwn) ? "" : wwn.replaceAll("\\:", "").toUpperCase(Locale.US);
    }

}
