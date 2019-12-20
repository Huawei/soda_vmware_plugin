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

package org.opensds.ecosystem.adapter.rest;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.apache.commons.lang3.StringUtils;
import org.apache.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONException;

import org.opensds.ecosystem.adapter.Constants;
import org.opensds.ecosystem.adapter.OpenSDSStorAdapter;
import org.opensds.ecosystem.adapter.RestApiUtil;
import org.opensds.ecosystem.adapter.model.DiskModel;
import org.opensds.ecosystem.adapter.model.FCPortModel;
import org.opensds.ecosystem.adapter.model.FileSystemModel;
import org.opensds.ecosystem.adapter.model.ISCSIPortModel;
import org.opensds.ecosystem.adapter.model.LunModel;
import org.opensds.ecosystem.adapter.model.PoolModel;
import org.opensds.ecosystem.adapter.model.RemoteReplicationModel;
import org.opensds.ecosystem.adapter.model.SmartQosModel;
import org.opensds.ecosystem.adapter.model.SnapShotModel;
import org.opensds.ecosystem.adapter.model.meta.StorModel;
import org.opensds.ecosystem.adapter.model.meta.ResourceModel;
import org.opensds.ecosystem.adapter.util.AdapterStringUtil;
import org.opensds.storage.conection.rest.domain.ConnectionData;
import com.integrien.alive.common.adapter3.DiscoveryResult;
import com.integrien.alive.common.adapter3.MetricData;
import com.integrien.alive.common.adapter3.MetricKey;
import com.integrien.alive.common.adapter3.ResourceKey;
import com.integrien.alive.common.adapter3.DiscoveryResult.StateChangeEnum;
import com.integrien.alive.common.adapter3.config.ResourceConfig;
import com.integrien.alive.common.adapter3.config.ResourceIdentifierConfig;
import com.integrien.alive.common.util.InstanceLoggerFactory;

public class GetRestData {
    // 适配器对象
    private OpenSDSStorAdapter adapter;

    private final Logger logger;

    /**
     * GetRestData构造方法
     *
     * @param adapter       适配器实例对象
     * @param loggerFactory 日志工厂
     */
    public GetRestData(OpenSDSStorAdapter adapter, InstanceLoggerFactory loggerFactory) {
        this.adapter = adapter;
        logger = loggerFactory.getLogger(GetRestData.class);
    }


    /**
     * 从rest接口获取数据到lun
     *
     * @param connectionData      连接信息
     * @param discoveryResult     收集结果集
     * @param dataModel           性能文件数据
     * @param devSN               阵列序列号
     * @param monitoringResources 上次收集到的元素集合
     * @return resList 本次收集的lun元素集合
     */
    public Map<String, ResourceKey> addLunDataByRest(ConnectionData connectionData, DiscoveryResult discoveryResult,
        LunModel dataModel, String devSN, Collection<ResourceConfig> monitoringResources) {
        Map<String, Map<String, String>> allLunData = RestApiUtil.getInstance()
                .getLunData(connectionData, Constants.REST_BATCH_PAGESIZE);
        if (allLunData.isEmpty()) {
            return null;
        }
        if (logger.isInfoEnabled()) {
            logger.info("allLunData.size():" + allLunData.size());
        }
        Map<String, ResourceKey> resList = new HashMap<String, ResourceKey>();
        ArrayList<ResourceKey> resourceKeyList = new ArrayList<ResourceKey>();

        for (Map.Entry<String, Map<String, String>> entry : allLunData.entrySet()) {
            Map<String, String> data = allLunData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest lun data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("lun id=" + entry.getKey() + " has no data !");
                continue;
            }
            String resName = data.get("NAME");
            String resId = entry.getKey();
            if (logger.isInfoEnabled()) {
                if (logger.isInfoEnabled()) {
                    logger.info("add new resourceKey:" + resName);
                }
            }
            ResourceKey resource = new ResourceKey(resName, "Lun",
                    "OpenSDSStorAdapter");
            ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig("Id",
                    devSN + "-" + "Lun" + "-" + resId, true);
            ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig("Name",
                    resName, false);

            resource.addIdentifier(ric1);
            resource.addIdentifier(ric2);
            resourceKeyList.add(resource);
            resList.put(resId, resource);
            if (logger.isInfoEnabled()) {
                logger.info("creat resource success:" + resName);
            }

            if (adapter.isNewResource(resource)) {
                discoveryResult.addResource(new ResourceConfig(resource));
                continue;
            }
            ResourceConfig resourceConfig = adapter.getMonitoringResource(resource);

            if (resourceConfig == null) {
                if (logger.isInfoEnabled()) {
                    logger.info("the resourceConfig is null" + resId);
                }
                continue;
            }

            long timestamp = System.currentTimeMillis();
            Map<String, ResourceModel> dataMap = dataModel.getDataList();
            for (Map.Entry<String, ResourceModel> entryData : dataMap
                    .entrySet()) {
                if (entryData.getKey().equals(resId)) {
                    ResourceModel resData = entryData.getValue();
                    Map<Integer, Integer> resDataMap = resData
                            .getResourceData();
                    for (Map.Entry<Integer, Integer> kv : resDataMap.entrySet()) {
                        MetricKey metricKey = new MetricKey();
                        if (logger.isInfoEnabled()) {
                            logger.info("kind=" + "Lun" + ",resName=" + resName + ",key=" + kv.getKey() + ",value="
                                    + kv.getValue());
                        }
                        metricKey.add(kv.getKey() + "");
                        double value = kv.getValue();
                        MetricData metricData = new MetricData(metricKey,
                                timestamp, value);
                        if (logger.isInfoEnabled()) {
                            logger.info("The ture value:" + value);
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("the Metric key is:" + kv.getKey()
                                    + "value:" + kv.getValue() + "resourceKind:"
                                    + "Lun" + "resid" + resId + "resource"
                                    + resourceConfig + "resConf.getResourceId():"
                                    + resourceConfig.getResourceId());
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("resourceKind:" + "RemoteReplication"
                                    + "getAdapterConfigurationName()"
                                    + resourceConfig.getAdapterConfigurationName());
                        }

                        adapter.addMetricData(resourceConfig, metricData);
                    }
                }
            }
            if (logger.isInfoEnabled()) {
                logger.info("rest data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("lun id=" + entry.getKey() + " has no data !");
                continue;
            }
            if (logger.isInfoEnabled()) {
                logger.info("lun WWN :" + data.get("WWN"));
            }

            Map<String, ResourceKey> lunWWN = dataModel.getLunWWN();

            lunWWN.put(data.get("WWN").toUpperCase(Locale.US), resource);
            Map<String, List<String>> ship = dataModel.getLun2ControllerShip();
            String controllerId = data.get("OWNINGCONTROLLER");
            if (logger.isInfoEnabled()) {
                logger.info("rest data lun controller id :" + controllerId);
            }
            if (ship.containsKey(controllerId)) {
                List<String> childLun = ship.get(controllerId);
                childLun.add(entry.getKey());
            } else {
                List<String> childLun = new ArrayList<String>();
                childLun.add(entry.getKey());
                ship.put(controllerId, childLun);
            }

            String poolId = data.get("PARENTID");
            Map<String, List<String>> lun2PoolShip = dataModel.getLun2PoolShip();
            if (logger.isInfoEnabled()) {
                logger.info("rest data lun pool id :" + poolId);
            }
            if (lun2PoolShip.containsKey(poolId)) {
                List<String> childPool = lun2PoolShip.get(poolId);
                childPool.add(entry.getKey());
            } else {
                List<String> childPool = new ArrayList<String>();
                childPool.add(entry.getKey());
                lun2PoolShip.put(poolId, childPool);
            }

            String partitionId = data.get("CACHEPARTITIONID");
            Map<String, List<String>> lun2PartitionShip = dataModel.getLun2PartitionShip();
            if (logger.isInfoEnabled()) {
                logger.info("rest data lun partition id :" + partitionId);
            }
            if (AdapterStringUtil.isNotNull(partitionId)) {
                if (lun2PartitionShip.containsKey(partitionId)) {
                    List<String> childLun = lun2PartitionShip.get(partitionId);
                    childLun.add(entry.getKey());
                } else {
                    List<String> childLun = new ArrayList<String>();
                    childLun.add(entry.getKey());
                    lun2PartitionShip.put(partitionId, childLun);
                }
            }

            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal num2 = new BigDecimal("2");
            BigDecimal total = new BigDecimal(data.get("CAPACITY"));
            BigDecimal used = new BigDecimal(data.get("ALLOCCAPACITY"));

            MetricKey metricKey = new MetricKey();
            metricKey.add("10000");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    total.divide(num1024)
                            .divide(num1024)
                            .divide(num2)
                            .doubleValue());
            adapter.addMetricData(resourceConfig, metricData);
            if (logger.isInfoEnabled()) {
                logger.info("add data:total=" + total + ",used=" + used);
            }
            if (logger.isInfoEnabled()) {
                logger.info("devSN is :" + devSN);
            }
        }

        // 修改已经删除的lun状态
        adapter.changeResState(monitoringResources, resourceKeyList, "Lun");
        return resList;
    }

    /**
     * 从rest接口获取host数据添加给host
     *
     * @param connectionData      连接信息对象
     * @param discoveryResult     收集集合
     * @param dataModel           性能文件数据
     * @param devSN               阵列序列号
     * @param monitoringResources 上次收集到的元素
     * @return resList 收集到的host元素列表
     */
    public Map<String, ResourceKey> addHostDataByRest(
            ConnectionData connectionData, DiscoveryResult discoveryResult,
            StorModel dataModel, String devSN,
            Collection<ResourceConfig> monitoringResources) {
        Map<String, Map<String, String>> allHostData = RestApiUtil.getInstance()
                .getHostData(connectionData, Constants.REST_BATCH_PAGESIZE);

        if (allHostData.isEmpty()) {
            logger.error("allHostData is empty");
            return null;
        }

        // reskeyList
        Map<String, ResourceKey> resList = new HashMap<String, ResourceKey>();

        ArrayList<ResourceKey> resourceKeyList = new ArrayList<ResourceKey>();

        for (Map.Entry<String, Map<String, String>> entry : allHostData.entrySet()) {
            Map<String, String> data = allHostData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest host data:" + data);
            }
            if (data == null || data.isEmpty()) {
                logger.error("Host id=" + entry.getKey() + " has no data !");
                continue;
            }
            String resName = data.get("NAME");
            String resId = entry.getKey();

            if (logger.isInfoEnabled()) {
                logger.info("add new resourceKey:" + resName);
            }
            ResourceKey resource = new ResourceKey(resName, "Host",
                    "OpenSDSStorAdapter");
            ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig("Id",
                    devSN + "-" + "Host" + "-" + resId, true);
            ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig("Name",
                    resName, false);

            resource.addIdentifier(ric1);
            resource.addIdentifier(ric2);
            resList.put(resId, resource);
            resourceKeyList.add(resource);
            if (logger.isInfoEnabled()) {
                logger.info("creat resource success:" + resName);
            }
            if (adapter.isNewResource(resource)) {
                discoveryResult.addResource(new ResourceConfig(resource));
                continue;
            }

            ResourceConfig resourceConfig = adapter.getMonitoringResource(resource);

            if (resourceConfig == null) {
                if (logger.isInfoEnabled()) {
                    logger.info("the resourceConfig is null" + resId);
                }
                continue;
            }

            long timestamp = System.currentTimeMillis();
            Map<String, ResourceModel> dataMap = dataModel.getDataList();
            for (Map.Entry<String, ResourceModel> entryData : dataMap.entrySet()) {
                if (entryData.getKey().equals(resId)) {
                    ResourceModel resData = entryData.getValue();
                    Map<Integer, Integer> resDataMap = resData.getResourceData();
                    for (Map.Entry<Integer, Integer> kv : resDataMap.entrySet()) {
                        MetricKey metricKey = new MetricKey();
                        if (logger.isInfoEnabled()) {
                            logger.info("kind=" + "Host" + ",resName=" + resName + ",key=" + kv.getKey()
                                    + ",value=" + kv.getValue());
                        }
                        metricKey.add(kv.getKey() + "");
                        double value = kv.getValue();
                        MetricData metricData = new MetricData(metricKey, timestamp, value);

                        if (logger.isInfoEnabled()) {
                            logger.info("The ture value:" + value);
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("resourceKind:" + "RemoteReplication"
                                    + "getAdapterConfigurationName()"
                                    + resourceConfig.getAdapterConfigurationName());
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("the Metric key is:" + kv.getKey() + "value:" + kv.getValue()
                                    + "resourceKind:" + "Host" + "resid" + resId + "resource" + resourceConfig
                                    + "resConf.getResourceId():" + resourceConfig.getResourceId());
                        }

                        adapter.addMetricData(resourceConfig, metricData);
                    }
                }
            }
        }

        // 修改已经删除的lun状态
        adapter.changeResState(monitoringResources, resourceKeyList, "Host");
        return resList;
    }

    /**
     * 从rest接口获取Pool的部分数据
     *
     * @param connectionData 收集集合
     * @param res            本次收集到的数据
     * @param poolData       性能文件pool数据
     */
    public void addPoolDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res, PoolModel poolData) {
        Map<String, Map<String, String>> allPoolData = RestApiUtil.getInstance().getPoolData(connectionData,
                Constants.REST_BATCH_PAGESIZE);

        long timestamp = System.currentTimeMillis();
        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(
                    entry.getValue());
            if (resourceConfig == null) {
                continue;
            }

            if (!allPoolData.containsKey(entry.getKey())) {
                logger.error("pool id=" + entry.getKey() + " has no data !");
                continue;
            }

            Map<String, String> data = allPoolData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("pool id=" + entry.getKey() + " has no data !");
                continue;
            }
            String diskdomainId = data.get("PARENTID");
            if (logger.isInfoEnabled()) {
                logger.info("rest data pool to diskdomain id :" + diskdomainId);
            }

            Map<String, List<String>> pool2DiskDomainShip =
                    poolData.getPool2DiskDomainShip();

            if (pool2DiskDomainShip.containsKey(diskdomainId)) {
                List<String> childPool = pool2DiskDomainShip.get(diskdomainId);
                childPool.add(entry.getKey());
            } else {
                List<String> childPool = new ArrayList<String>();
                childPool.add(entry.getKey());
                pool2DiskDomainShip.put(diskdomainId, childPool);
            }

            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal num2 = new BigDecimal("2");
            BigDecimal total = new BigDecimal(data.get("USERTOTALCAPACITY"));
            BigDecimal used = new BigDecimal(data.get("USERCONSUMEDCAPACITY"));
            // 防止Dorado nas获取该字段
            BigDecimal replication;
            BigDecimal displayUsed = used;
            if (data.containsKey("REPLICATIONCAPACITY")) {
                replication = new BigDecimal(data.get("REPLICATIONCAPACITY"));
                displayUsed = used.add(replication);
            }
            MetricKey metricKey = new MetricKey();
            metricKey.add("10000");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    total.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10001");
            metricData = new MetricData(metricKey, timestamp,
                    displayUsed.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10002");
            BigDecimal free = new BigDecimal(data.get("USERFREECAPACITY"));
            metricData = new MetricData(metricKey, timestamp,
                    free.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10003");
            BigDecimal num100 = new BigDecimal("100");
            BigDecimal capacityFree = free.divide(total, Constants.SECTORS_UNIT, BigDecimal.ROUND_HALF_UP);
            BigDecimal capacityFreeRate = capacityFree
                    .multiply(new BigDecimal(Double.toString(Constants.PERCENT_CONVERSION_UNIT)));
            BigDecimal usedRate = num100.subtract(capacityFreeRate);
            metricData = new MetricData(metricKey, timestamp,
                    usedRate.doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10004");
            metricData = new MetricData(metricKey, timestamp, capacityFreeRate.doubleValue());
            adapter.addMetricData(resourceConfig, metricData);
            if (logger.isInfoEnabled()) {
                logger.info("add data:total=" + total + ",used=" + used + ",free=" + free);
            }
        }
    }


    /**
     * 从rest接口获取远程复制数据
     *
     * @param connectionData      连接信息
     * @param discoveryResult     收集结果集
     * @param dataModel           性能文件数据
     * @param devSN               阵列序列号
     * @param monitoringResources 收集到的元素集合
     * @return resList 本次收集的元素集合
     */
    public Map<String, ResourceKey> addRemoteReplicationDataByRest(ConnectionData connectionData,
        DiscoveryResult discoveryResult, RemoteReplicationModel dataModel, String devSN,
        Collection<ResourceConfig> monitoringResources) {
        Map<String, Map<String, String>> allRemoteData = RestApiUtil.getInstance().getReplicationData(connectionData,
            Constants.REST_BATCH_PAGESIZE);
        if (allRemoteData.isEmpty()) {
            return null;
        }

        // reskeyList
        Map<String, ResourceKey> resList = new HashMap<String, ResourceKey>();
        ArrayList<ResourceKey> resourceKeyList = new ArrayList<ResourceKey>();
        long timestamp = System.currentTimeMillis();

        for (Map.Entry<String, Map<String, String>> entry : allRemoteData.entrySet()) {
            Map<String, String> data = allRemoteData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest Remote data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("remote replication id=" + entry.getKey()
                        + " has no data !");
                continue;
            }
            String resName = data.get("ID");
            String resId = entry.getKey();

            if (logger.isInfoEnabled()) {
                logger.info("add new resourceKey:" + resName);
            }
            ResourceKey resource = new ResourceKey(resName,
                    "RemoteReplication", "OpenSDSStorAdapter");
            ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig("Id",
                    devSN + "-" + "RemoteReplication" + "-" + resId, true);
            ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig("Name",
                    resName, false);

            resource.addIdentifier(ric1);
            resource.addIdentifier(ric2);
            resList.put(resId, resource);
            resourceKeyList.add(resource);
            if (logger.isInfoEnabled()) {
                logger.info("creat resource success:" + resName);
            }

            if (adapter.isNewResource(resource)) {
                discoveryResult.addResource(new ResourceConfig(resource));
                continue;
            }

            ResourceConfig resourceConfig = adapter.getMonitoringResource(resource);

            if (resourceConfig == null) {
                if (logger.isInfoEnabled()) {
                    logger.info("the resourceConfig is null" + resId);
                }
                continue;
            }

            Map<String, ResourceModel> dataMap = dataModel.getDataList();
            for (Map.Entry<String, ResourceModel> entryData : dataMap
                    .entrySet()) {
                if (entryData.getKey().equals(resId)) {
                    ResourceModel resData = entryData.getValue();
                    Map<Integer, Integer> resDataMap = resData
                            .getResourceData();
                    for (Map.Entry<Integer, Integer> kv : resDataMap.entrySet()) {
                        MetricKey metricKey = new MetricKey();
                        if (logger.isInfoEnabled()) {
                            logger.info("kind=" + "RemoteReplication"
                                    + ",resName=" + resName + ",key=" + kv.getKey() + ",value=" + kv.getValue());
                        }
                        metricKey.add(kv.getKey() + "");
                        double value = kv.getValue();
                        MetricData metricData = new MetricData(metricKey,
                                timestamp, value);
                        if (logger.isInfoEnabled()) {
                            logger.info("The ture value:" + value);
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("the Metric key is:" + kv.getKey()
                                    + "value:" + kv.getValue() + "resourceKind:"
                                    + "RemoteReplication" + "resid" + resId + "resource" + resourceConfig
                                    + "resConf.getResourceId():" + resourceConfig.getResourceId());
                        }
                        if (logger.isInfoEnabled()) {
                            logger.info("resourceKind:" + "RemoteReplication"
                                    + "getAdapterConfigurationName()"
                                    + resourceConfig.getAdapterConfigurationName());
                        }

                        adapter.addMetricData(resourceConfig, metricData);
                    }
                }
            }

            MetricKey metricKeyResName = new MetricKey();
            metricKeyResName.add("LOCALRESNAME");
            String localResName = data.get("LOCALRESNAME");
            if (localResName == null) {
                localResName = "null";
            }
            MetricData metricDatalocalRes = new MetricData(metricKeyResName,
                    timestamp, localResName + "");
            adapter.addMetricData(resourceConfig, metricDatalocalRes);

            String pid = data.get("LOCALRESID");
            String ptype = data.get("LOCALRESTYPE");
            if (logger.isInfoEnabled()) {
                logger.info("add RemoteReplication Ship");
            }
            Map<String, List<String>> replication2FileSystemShip =
                    dataModel.getReplication2FileSystemShip();
            Map<String, List<String>> replication2LunShip =
                    dataModel.getReplication2LunShip();

            if (Constants.LUN_TYPE.equals(ptype)) {
                if (logger.isInfoEnabled()) {
                    logger.info("rest data file lun id :" + pid);
                }
                if (replication2LunShip.containsKey(pid)) {
                    List<String> childRep = replication2LunShip.get(pid);
                    childRep.add(entry.getKey());
                } else {
                    List<String> childRep = new ArrayList<String>();
                    childRep.add(entry.getKey());
                    replication2LunShip.put(pid, childRep);
                }
            } else if (Constants.FILE_SYSTEM_TYPE.equals(ptype)) {
                if (logger.isInfoEnabled()) {
                    logger.info("rest data file filesystem id :" + pid);
                }
                if (replication2FileSystemShip.containsKey(pid)) {
                    List<String> childRep = replication2FileSystemShip.get(pid);
                    childRep.add(entry.getKey());
                } else {
                    List<String> childRep = new ArrayList<String>();
                    childRep.add(entry.getKey());
                    replication2FileSystemShip.put(pid, childRep);
                }
            } else {
                if (logger.isInfoEnabled()) {
                    logger.info("rest data file lun id :" + pid);
                }
                if (replication2LunShip.containsKey(pid)) {
                    List<String> childRep = replication2LunShip.get(pid);
                    childRep.add(entry.getKey());
                } else {
                    List<String> childRep = new ArrayList<String>();
                    childRep.add(entry.getKey());
                    replication2LunShip.put(pid, childRep);
                }
            }
        }

        // 修改已经删除的RemoteReplication状态
        adapter.changeResState(monitoringResources, resourceKeyList, "RemoteReplication");
        return resList;
    }


    /**
     * 从rest接口获取qos的部分数据
     *
     * @param connectionData 链接信息集合
     * @param res            这次收集到的元素集合
     * @param smartQosData   性能数据
     */
    public void addQosDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res,
        SmartQosModel smartQosData) {
        Map<String, List<String>> fileSystem2QosShip = smartQosData.getQos2FileSystemShip();
        Map<String, List<String>> lun2QosShip = smartQosData.getQos2LunShip();
        Map<String, Map<String, String>> allQosData = RestApiUtil.getInstance()
                .getQosData(connectionData, Constants.REST_BATCH_PAGESIZE);
        if (allQosData.isEmpty()) {
            return;
        }

        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(
                    entry.getValue());
            if (resourceConfig == null) {
                continue;
            }
            if (!allQosData.containsKey(entry.getKey())) {
                logger.error("smart qos id=" + entry.getKey() + " has no data !");
                continue;
            }
            Map<String, String> data = allQosData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest qos data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("smart qos id=" + entry.getKey() + " has no data !");
                continue;
            }
            String lunList = data.get("LUNLIST");
            String fsList = data.get("FSLIST");
            if (StringUtils.isNotBlank(lunList)) {
                if (logger.isInfoEnabled()) {
                    logger.info("rest qos data:" + data);
                }
                if (logger.isInfoEnabled()) {
                    logger.info("rest data file lun list :" + lunList);
                }
                JSONArray lunarr = AdapterStringUtil
                        .parseString2JSONArray(lunList);
                List<String> childLun = new ArrayList<String>();
                for (int i = 0; i < lunarr.length(); i++) {
                    try {
                        String lunArr = lunarr.getString(i);
                        if (StringUtils.isBlank(lunArr)) {
                            continue;
                        } else {
                            childLun.add(lunArr);
                        }
                    } catch (JSONException e) {
                        logger.error(e.getMessage());
                    }
                }
                if (childLun.isEmpty()) {
                    logger.error("the childLun is null");
                } else {
                    lun2QosShip.put(data.get("ID"), childLun);
                }
            }
            if (StringUtils.isNotBlank(fsList)) {
                if (logger.isInfoEnabled()) {
                    logger.info("rest data file FS list :" + fsList);
                }
                JSONArray fsarr = AdapterStringUtil
                        .parseString2JSONArray(fsList);
                List<String> childFs = new ArrayList<String>();
                for (int i = 0; i < fsarr.length(); i++) {
                    try {
                        String fsArr = fsarr.getString(i);
                        if (StringUtils.isBlank(fsArr)) {
                            continue;
                        } else {
                            childFs.add(fsArr);
                        }
                    } catch (JSONException e) {
                        logger.error(e.getMessage());
                    }
                }
                if (childFs.isEmpty()) {
                    logger.error("the FSLIST is null");
                } else {
                    fileSystem2QosShip.put(data.get("ID"), childFs);
                }
            }
        }
    }

    /**
     * 从rest接口获取disk部分数据
     *
     * @param connectionData 链接信息集合
     * @param res            这次收集到的元素集合
     * @param diskModel      性能数据
     */
    public void addDiskDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res, DiskModel diskModel) {
        Map<String, Map<String, String>> allDiskData = RestApiUtil.getInstance().getDiskData(connectionData);
        if (allDiskData.isEmpty()) {
            return;
        }

        long timestamp = System.currentTimeMillis();
        Map<String, List<String>> disk2DiskDomainShip = diskModel.getDisk2DiskDomainShip();
        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(entry.getValue());
            if (resourceConfig == null) {
                continue;
            }

            if (!allDiskData.containsKey(entry.getKey())) {
                logger.error("disk id=" + entry.getKey() + " has no data !");
                continue;
            }

            Map<String, String> data = allDiskData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest disk data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("disk id=" + entry.getKey() + " has no data !");
                continue;
            }
            String diskdomainId = data.get("POOLID");
            if (logger.isInfoEnabled()) {
                logger.info("rest data disk to diskdomain id :" + diskdomainId);
            }
            if (disk2DiskDomainShip.containsKey(diskdomainId)) {
                List<String> childDisk = disk2DiskDomainShip.get(diskdomainId);
                childDisk.add(entry.getKey());
            } else {
                List<String> childDisk = new ArrayList<String>();
                childDisk.add(entry.getKey());
                disk2DiskDomainShip.put(diskdomainId, childDisk);
            }

            String usedRate = data.get("CAPACITYUSAGE");
            BigDecimal sectors = new BigDecimal(data.get("SECTORS"));
            BigDecimal sectorsize = new BigDecimal(data.get("SECTORSIZE"));
            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal total = sectors.multiply(sectorsize);

            MetricKey metricKey = new MetricKey();
            metricKey.add("10000");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    total.divide(num1024).divide(num1024).divide(num1024).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("Capacity Usage Rate");
            float usedRatefloat = Float.parseFloat(usedRate);
            metricData = new MetricData(metricKey, timestamp, usedRatefloat);
            adapter.addMetricData(resourceConfig, metricData);
            if (logger.isInfoEnabled()) {
                logger.info("add usedRate=" + usedRate);
            }
        }
    }

    /**
     * 从rest接口获取FileSystemData的部分数据
     *
     * @param connectionData 链接信息集合
     * @param res            这次收集到的元素集合
     * @param fileSystemData 性能数据
     */
    public void addFileSystemDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res,
        FileSystemModel fileSystemData) {
        Map<String, Map<String, String>> allFsData = RestApiUtil.getInstance().getFileSystemData(connectionData,
                Constants.REST_BATCH_PAGESIZE);

        long timestamp = System.currentTimeMillis();
        Map<String, List<String>> fs2ControllerShip = fileSystemData.getFs2ControllerShip();
        Map<String, List<String>> fs2PoolShip = fileSystemData.getFs2PoolShip();
        Map<String, List<String>> fs2PartitionShip = fileSystemData.getFs2PartitionShip();
        Map<String, ResourceKey> nasMap = fileSystemData.getNasMap();
        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(entry.getValue());
            if (resourceConfig == null) {
                continue;
            }

            if (!allFsData.containsKey(entry.getKey())) {
                logger.error("fileSystem id=" + entry.getKey() + " has no data !");
                continue;
            }

            Map<String, String> data = allFsData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest fileSystem data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("fileSystem id=" + entry.getKey() + " has no data !");
                continue;
            }
            String controllerId = data.get("OWNINGCONTROLLER");
            String nasName = data.get("ROOT");
            nasMap.put(nasName, entry.getValue());
            if (logger.isInfoEnabled()) {
                logger.info("rest data file system controller id :" + controllerId);
            }
            if (fs2ControllerShip.containsKey(controllerId)) {
                List<String> childFs = fs2ControllerShip.get(controllerId);
                childFs.add(entry.getKey());
            } else {
                List<String> childFs = new ArrayList<String>();
                childFs.add(entry.getKey());
                fs2ControllerShip.put(controllerId, childFs);
            }

            String poolId = data.get("PARENTID");
            if (logger.isInfoEnabled()) {
                logger.info("rest data file system pool id :" + poolId);
            }
            if (fs2PoolShip.containsKey(poolId)) {
                List<String> childPool = fs2PoolShip.get(poolId);
                childPool.add(entry.getKey());
            } else {
                List<String> childPool = new ArrayList<String>();
                childPool.add(entry.getKey());
                fs2PoolShip.put(poolId, childPool);
            }

            String partitionId = data.get("SMARTCACHEPARTITIONID");
            if (logger.isInfoEnabled()) {
                logger.info("rest data file system partition id :" + partitionId);
            }
            if (AdapterStringUtil.isNotNull(partitionId)) {
                if (fs2PartitionShip.containsKey(partitionId)) {
                    List<String> childFS = fs2PartitionShip.get(partitionId);
                    childFS.add(entry.getKey());
                } else {
                    List<String> childFS = new ArrayList<String>();
                    childFS.add(entry.getKey());
                    fs2PartitionShip.put(partitionId, childFS);
                }
            }

            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal num2 = new BigDecimal("2");
            BigDecimal total = new BigDecimal(data.get("CAPACITY"));

            MetricKey metricKey = new MetricKey();
            metricKey.add("10000");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    total.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10001");
            BigDecimal used = new BigDecimal(data.get("ALLOCCAPACITY"));
            metricData = new MetricData(metricKey, timestamp,
                    used.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10003");
            metricData = new MetricData(metricKey, timestamp,
                    used.divide(total, Constants.SECTORS_UNIT, RoundingMode.HALF_UP)
                            .multiply(new BigDecimal(Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                            .doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10004");
            BigDecimal free = total.subtract(used);
            metricData = new MetricData(metricKey, timestamp,
                    free.divide(total, Constants.SECTORS_UNIT, BigDecimal.ROUND_HALF_UP)
                            .multiply(new BigDecimal(Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                            .doubleValue());
            adapter.addMetricData(resourceConfig, metricData);
            if (logger.isInfoEnabled()) {
                logger.info("add data:total=" + total + ",used=" + used + ",free=" + free);
            }
        }
    }

    /**
     * 从rest接口获取Diskdomain的部分数据
     *
     * @param connectionData 连接信息集合
     * @param res            这次收集到的元素集合
     */
    public void addDiskdomainDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res) {
        Map<String, Map<String, String>> allDiskdomainData = RestApiUtil.getInstance().getDiskdomainData(connectionData,
                Constants.REST_BATCH_PAGESIZE);
        long timestamp = System.currentTimeMillis();

        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(entry.getValue());
            if (resourceConfig == null) {
                continue;
            }
            if (!allDiskdomainData.containsKey(entry.getKey())) {
                logger.error("diskDomain id=" + entry.getKey() + " has no data !");
                continue;
            }
            Map<String, String> data = allDiskdomainData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest diskDomain data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("diskDomain id=" + entry.getKey() + " has no data !");
                continue;
            }

            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal num2 = new BigDecimal("2");
            BigDecimal total = new BigDecimal(data.get("TOTALCAPACITY"));
            BigDecimal free = new BigDecimal(data.get("FREECAPACITY"));
            BigDecimal used = total.subtract(free);

            MetricKey metricKey = new MetricKey();
            metricKey.add("10000");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    total.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10001");
            metricData = new MetricData(metricKey, timestamp,
                    used.divide(num1024).divide(num1024).divide(num2).doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10003");
            metricData = new MetricData(metricKey, timestamp,
                    used.divide(total, Constants.SECTORS_UNIT, RoundingMode.HALF_UP)
                            .multiply(new BigDecimal(Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                            .doubleValue());
            adapter.addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("10004");
            metricData = new MetricData(metricKey, timestamp,
                    free.divide(total, Constants.SECTORS_UNIT, BigDecimal.ROUND_HALF_UP)
                            .multiply(new BigDecimal(Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                            .doubleValue());
            adapter.addMetricData(resourceConfig, metricData);
            if (logger.isInfoEnabled()) {
                logger.info("add data:total=" + total + ",used=" + used + ",free=" + free);
            }
        }
    }

    /**
     * 从rest接口获取FC的部分数据
     *
     * @param connectionData 连接信息集合
     * @param res            这次收集到的元素集合
     * @param fcPortData     性能数据
     */
    public void addFCPortDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res,
        FCPortModel fcPortData) {
        Map<String, Map<String, String>> allFCPortData = RestApiUtil.getInstance().getFCPortData(connectionData);

        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(entry.getValue());
            if (resourceConfig == null) {
                continue;
            }
            if (!allFCPortData.containsKey(entry.getKey())) {
                logger.error("fc port id=" + entry.getKey() + " has no data !");
                continue;
            }
            Map<String, String> data = allFCPortData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest FC data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("fc port id=" + entry.getKey() + " has no data !");
                continue;
            }

            String controllerId = data.get("PARENTID");
            if (StringUtils.isNotBlank(controllerId)) {
                String[] split = controllerId.split("\\.");
                controllerId = split[0];
            }
            String ptype = data.get("PARENTTYPE");
            if ((!"207".equals(ptype)) && (!"209".equals(ptype))) {
                continue;
            }
            if (logger.isInfoEnabled()) {
                logger.info("rest data file controller id :" + controllerId);
            }

            Map<String, List<String>> fcport2ControllerShip =
                    fcPortData.getFcport2ControllerShip();
            if (fcport2ControllerShip.containsKey(controllerId)) {
                List<String> childFcPort = fcport2ControllerShip.get(controllerId);
                childFcPort.add(entry.getKey());
            } else {
                List<String> childFcPort = new ArrayList<String>();
                childFcPort.add(entry.getKey());
                fcport2ControllerShip.put(controllerId, childFcPort);
            }
        }
    }

    /**
     * 从rest接口获取iscsiPort的部分数据
     *
     * @param connectionData 连接信息集合
     * @param res            这次收集到的元素集合
     * @param iscsiPort      性能数据
     */
    public void addiSCSIPortDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res,
        ISCSIPortModel iscsiPort) {
        Map<String, Map<String, String>> alliSCSIPortData = RestApiUtil.getInstance().getiSCSIPortData(connectionData);
        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(entry.getValue());
            if (resourceConfig == null) {
                continue;
            }
            if (!alliSCSIPortData.containsKey(entry.getKey())) {
                logger.error("IscsiPort id=" + entry.getKey() + " has no data !");
                continue;
            }
            Map<String, String> data = alliSCSIPortData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest iscsiPort data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("iscsi port id=" + entry.getKey() + " has no data !");
                continue;
            }

            String controllerId = data.get("PARENTID");
            if (StringUtils.isNotBlank(controllerId)) {
                String[] split = controllerId.split("\\.");
                controllerId = split[0];
            }
            String ptype = data.get("PARENTTYPE");
            if ((!"207".equals(ptype)) && (!"209".equals(ptype))) {
                continue;
            }
            if (logger.isInfoEnabled()) {
                logger.info("rest data file controller id :" + controllerId);
            }
            Map<String, List<String>> iscsiport2ControllerShip =
                    iscsiPort.getIscsiport2ControllerShip();

            if (iscsiport2ControllerShip.containsKey(controllerId)) {
                List<String> childiscsiPort = iscsiport2ControllerShip
                        .get(controllerId);
                childiscsiPort.add(entry.getKey());
            } else {
                List<String> childiscsiPort = new ArrayList<String>();
                childiscsiPort.add(entry.getKey());
                iscsiport2ControllerShip.put(controllerId, childiscsiPort);
            }
        }
    }

    /**
     * 从rest接口获取SnapShot的部分数据
     *
     * @param connectionData 连接信息集合
     * @param res            这次收集到的元素集合
     * @param snapShotData   性能数据
     */
    public void addSnapShotDataByRest(ConnectionData connectionData, Map<String, ResourceKey> res,
        SnapShotModel snapShotData) {
        Map<String, Map<String, String>> allSnapShotData = RestApiUtil
                .getInstance()
                .getSnapShotData(connectionData, Constants.REST_BATCH_PAGESIZE);

        for (Map.Entry<String, ResourceKey> entry : res.entrySet()) {
            ResourceConfig resourceConfig = adapter.getMonitoringResource(
                    entry.getValue());
            if (resourceConfig == null) {
                continue;
            }
            if (!allSnapShotData.containsKey(entry.getKey())) {
                logger.error("snapshot id=" + entry.getKey() + " has no data !");
                continue;
            }
            Map<String, String> data = allSnapShotData.get(entry.getKey());
            if (logger.isInfoEnabled()) {
                logger.info("rest snapshot data:" + data);
            }
            if (data.isEmpty()) {
                logger.error("snapshot id=" + entry.getKey() + " has no data !");
                continue;
            }

            String lunId = data.get("PARENTID");
            String ptype = data.get("PARENTTYPE");
            if (!"11".equals(ptype)) {
                continue;
            }
            if (logger.isInfoEnabled()) {
                logger.info("rest data file lun id :" + lunId);
            }
            Map<String, List<String>> snapShot2LunShip =
                    snapShotData.getSnapShot2LunShip();
            if (snapShot2LunShip.containsKey(lunId)) {
                List<String> childSnapShot = snapShot2LunShip.get(lunId);
                childSnapShot.add(entry.getKey());
            } else {
                List<String> childSnapShot = new ArrayList<String>();
                childSnapShot.add(entry.getKey());
                snapShot2LunShip.put(lunId, childSnapShot);
            }
        }
    }
}
