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

package org.opensds.ecosystem.adapter;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLEncoder;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.apache.commons.lang3.StringUtils;
import org.apache.log4j.Logger;

import org.opensds.ecosystem.adapter.model.StorData;
import org.opensds.ecosystem.adapter.model.LoadDataFactory;
import org.opensds.ecosystem.adapter.model.meta.StorModel;
import org.opensds.ecosystem.adapter.model.meta.ResourceModel;
import org.opensds.ecosystem.adapter.rest.GetRestData;
import org.opensds.ecosystem.adapter.thread.ConnectionResults;
import org.opensds.ecosystem.adapter.util.AdapterStringUtil;
import org.opensds.ecosystem.adapter.util.OpenSDSStorAdapterUtil;
import org.opensds.ecosystem.adapter.vcenter.VCenterManager;
import org.opensds.ecosystem.adapter.vcenter.model.FSlun;
import org.opensds.ecosystem.adapter.vcenter.model.DataStore;
import org.opensds.ecosystem.adapter.vcenter.model.VMware;
import org.opensds.ecosystem.adapter.vcenter.model.Naslun;
import org.opensds.ecosystem.adapter.vcenter.model.VCenterModule;
import org.opensds.storage.conection.rest.domain.ConnectionData;
import org.opensds.storage.conection.sftp.SftpAccount;
import org.opensds.storage.extracdata.PerfStatHisFileInfo;
import org.opensds.storage.extracdata.PerfStatHisFileProxy;
import org.opensds.storage.extracdata.util.ProductModeMaps;
import org.opensds.storage.extracdata.util.VerifyUtil;
import com.integrien.alive.common.adapter3.AdapterBase;
import com.integrien.alive.common.adapter3.DiscoveryParam;
import com.integrien.alive.common.adapter3.DiscoveryResult;
import com.integrien.alive.common.adapter3.DiscoveryResult.StateChangeEnum;
import com.integrien.alive.common.adapter3.IdentifierCredentialProperties;
import com.integrien.alive.common.adapter3.MetricData;
import com.integrien.alive.common.adapter3.MetricKey;
import com.integrien.alive.common.adapter3.Relationships;
import com.integrien.alive.common.adapter3.ResourceKey;
import com.integrien.alive.common.adapter3.ResourceStatus;
import com.integrien.alive.common.adapter3.TestParam;
import com.integrien.alive.common.adapter3.config.ResourceConfig;
import com.integrien.alive.common.adapter3.config.ResourceIdentifierConfig;
import com.integrien.alive.common.adapter3.describe.AdapterDescribe;
import com.integrien.alive.common.util.CommonConstants.ResourceStateEnum;
import com.integrien.alive.common.util.CommonConstants.ResourceStatusEnum;
import com.vmware.vcops.common.l10n.LocalizedMsg;
import com.vmware.vim25.mo.ServiceInstance;

public class OpenSDSStorAdapter extends AdapterBase {
    // 日志收集器
    private final Logger logger;

    // 适配器工具
    private OpenSDSStorAdapterUtil adapterUtil;

    private GetRestData getRestData;

    /**
     * 默认类构造器
     */
    public OpenSDSStorAdapter() {
        this(null, null);
    }

    /**
     * 带参数的类构造器，初始化适配器实例名称及实例ID
     *
     * @param instanceName 适配器实例名称
     * @param instanceId   适配器实例ID
     */
    public OpenSDSStorAdapter(String instanceName, Integer instanceId) {
        super(instanceName, instanceId);
        logger = loggerFactory.getLogger(OpenSDSStorAdapter.class);
        adapterUtil = new OpenSDSStorAdapterUtil(loggerFactory);
    }

    /**
     * 存储适配器描述文件，描述文件中定义了收集对象的相关信息，对应的对象描述文件describe.xml
     * vRealize框架会自动解析该配置文件，将配置文件中的对象自动解析成vRealize可识别的对象提供给上层界面使用
     *
     * @return adapterDescribe 配置对象描述信息
     */
    public AdapterDescribe onDescribe() {
        logger.info("Inside onDescribe method of OpenSDSStorAdapter class.");
        return adapterUtil.createAdapterDescribe();
    }

    /**
     * 相应前台手工收集操作，用户点击收集按钮，开始执行收集操作，
     * 系统返回收集到的后端存储的性能和容量信息
     *
     * @param discParam 前台配置的存储连接等配置信息
     * @return discoveryResult 手工收集结果
     */
    public DiscoveryResult onDiscover(DiscoveryParam discParam) {
        logger.info("Inside onDiscover method of OpenSDSStorAdapter class.");
        DiscoveryResult discoveryResult = new DiscoveryResult(
                discParam.getAdapterInstResource());

        return discoveryResult;
    }

    /**
     * 新的适配器实例创建时调用，重新配置适配器实例
     * 实例用于检查同后端存储阵列的连接关系
     *
     * @param resStatus           资源状态对象
     * @param adapterInstResource 实例配置对象
     */
    public void onConfigure(ResourceStatus resStatus,
                            ResourceConfig adapterInstResource) {
        logger.info("Inside onConfigure method of OpenSDSStorAdapter class.");
        final IdentifierCredentialProperties prop = new IdentifierCredentialProperties(
                loggerFactory, adapterInstResource);
        String userName = String
                .valueOf(prop.getCredential(Constants.USER_NAME));
        String userPassWord = String
                .valueOf(prop.getCredential(Constants.USER_PWD));
        String arrayHost = prop.getIdentifier(Constants.ARRAY_HOST);
        String certificatePath = prop.getIdentifier(Constants.CERTIFICATE_PATH);

        ConnectionData connectionData = new ConnectionData();
        connectionData.setUsername(userName);
        connectionData.setPassword(userPassWord);
        connectionData.setCertificateFilePath(certificatePath);
        connectionData.setStrictCheckHostName(Constants.NO_CHECK_DOMAIN);
        // 轮询业务IＰ，其中有一个业务ＩＰ登陆成功则认为和后端阵列状态为可连接状态

        ConnectionResults connectionResults =
                new ConnectionResults(connectionData, arrayHost, logger);
        boolean connectFalg = connectionResults.getConnectionResults();

        // 如果为可连接状态，设置适配器连接状态为正在接数据状态
        if (!connectFalg) {
            logger.error("The input IP can not be connected!");
            resStatus.setStatus(ResourceStatusEnum.RESOURCE_STATUS_DOWN);
            adapterInstResource
                    .setState(ResourceStateEnum.RESOURCE_STATE_FAILED);
        }
    }

    /**
     * 相应前台测试功能按钮，用于测试和后端存储及vCenter的服务联通性
     * 功能详细说明：
     * 1、根据配置的阵列的用户名和密码，测试同阵列的服务连通性
     * 2、根据配置的vCenter的用户名和密码等，测试同vCenter的连通性
     *
     * @param testParam 测试连接对象，内包括存储阵列和vCenter配置
     * @return boolean
     */
    public boolean onTest(TestParam testParam) {
        if (logger.isInfoEnabled()) {
            logger.info("OpenSDSStorAdpater|onTest|Inside onTest method of OpenSDSStorAdapter class.");
        }
        ResourceConfig adapterInstResource = testParam.getAdapterConfig()
                .getAdapterInstResource();

        final IdentifierCredentialProperties prop = new IdentifierCredentialProperties(
                loggerFactory, adapterInstResource);
        String userName = String
                .valueOf(prop.getCredential(Constants.USER_NAME));
        String userPassWord = String
                .valueOf(prop.getCredential(Constants.USER_PWD));
        String arrayHost = prop.getIdentifier(Constants.ARRAY_HOST);
        String certificatePath = prop.getIdentifier(Constants.CERTIFICATE_PATH);

        ConnectionData connectionData = new ConnectionData();
        connectionData.setUsername(userName);
        connectionData.setPassword(userPassWord);
        connectionData.setCertificateFilePath(certificatePath);
        connectionData.setStrictCheckHostName(Constants.NO_CHECK_DOMAIN);

        // 轮询业务IＰ，其中有一个业务ＩＰ登陆成功则认为和后端阵列状态为可连接状态
        String invalidIP = "";
        String[] hostIPList = arrayHost.split(Constants.ARRAY_HOST_SPLITER);
        for (int i = 0; i < hostIPList.length; i++) {
            if (logger.isInfoEnabled()) {
                logger.info("onTest|Debug iP is:" + hostIPList[i] + ".");
            }
            // 如果不符合IP V4和V6规则，则弹出提示
            if (RestApiUtil.validataIPv4(hostIPList[i])
                    || RestApiUtil.validataIPv6(hostIPList[i])) {
                continue;
            } else {
                if (!StringUtils.isBlank(invalidIP)) {
                    invalidIP += ";" + hostIPList[i];
                } else {
                    invalidIP += hostIPList[i];
                }
            }
        }
        final LocalizedMsg.Namespace adapterNamespace = new LocalizedMsg.Namespace("OpenSDSStorAdapter");
        if (!StringUtils.isBlank(invalidIP)) {
            String megToUser = "The input IP:" + invalidIP + " is invalid!";
            testParam.setLocalizedMsg(LocalizedMsg._tr(adapterNamespace, megToUser));
            return false;
        }
        // 判断用户输出的信息是否可以登录到Storage
        ConnectionResults connectionResults =
                new ConnectionResults(connectionData, arrayHost, logger);
        boolean connectFlag = connectionResults.getConnectionResults();

        if (!connectFlag) {
            testParam.setLocalizedMsg(LocalizedMsg._tr(adapterNamespace,
                    "The input IP can not be connected!"));
            return connectFlag;
        }

        // 判断IP地址是否属于同一阵列
        if (connectFlag && (arrayHost.contains(Constants.ARRAY_HOST_SPLITER))) {
            Map<String, List<String>> controllerIPList = RestApiUtil
                    .getInstance().getHostControllerIP(connectionData);
            if (!isNullMap(controllerIPList)) {
                List<String> ipV4list = controllerIPList.get("IPV4");
                List<String> ipV6list = controllerIPList.get("IPV6");
                for (int i = 0; i < hostIPList.length; i++) {
                    if (RestApiUtil.validataIPv4(hostIPList[i])) {
                        if (!ipV4list.isEmpty()
                                && !ipV4list.contains(hostIPList[i])) {
                            testParam.setLocalizedMsg(LocalizedMsg._tr(
                                    adapterNamespace,
                                    "The input IP does not belong to the same device!"));
                            return false;
                        }
                    }
                    if (RestApiUtil.validataIPv6(hostIPList[i])) {
                        String ipV6Add = hostIPList[i].replace("[", "")
                                .replace("]", "");
                        if (!ipV6list.isEmpty() && !ipV6list.contains(ipV6Add)) {
                            testParam.setLocalizedMsg(LocalizedMsg._tr(
                                    adapterNamespace,
                                    "The input IP does not belong to the same device!"));
                            return false;
                        }
                    }
                }
            } else {
                testParam.setLocalizedMsg(LocalizedMsg._tr(adapterNamespace,
                        "The input IP can not be connected!"));
                return false;
            }
        }
        if (connectFlag) {
            ConnectionData vCenterConnection = getVCenterConnectInfo(prop);
            ServiceInstance si = null;
            try {
                URL url = new URL(vCenterConnection.getHostURL());
                String vcUserName = vCenterConnection.getUsername();
                String vcPassWord = vCenterConnection.getPassword();
                si = new ServiceInstance(url, vcUserName, vcPassWord, true, 3000, 3000);
                connectFlag = true;
            } catch (MalformedURLException e) {
                logger.error("Get Host URL Error:" + e.getMessage());
                testParam.setLocalizedMsg(LocalizedMsg._tr(adapterNamespace,
                        "OpenSDS Storage connection succeed, but vCenter connection failed"));
                connectFlag = false;
            } catch (RemoteException e) {
                logger.error("VCenter connection error :"
                        + vCenterConnection.getHostURL() + ".");
                testParam.setLocalizedMsg(LocalizedMsg._tr(adapterNamespace,
                        "OpenSDS Storage connection succeed, but vCenter connection failed"));
                connectFlag = false;
            } finally {
                if (si != null) {
                    si.getServerConnection().logout();
                    if (logger.isInfoEnabled()) {
                        logger.info("VCenter connection logout :" + vCenterConnection.getHostURL() + ".");
                    }
                }
            }
        }
        return connectFlag;
    }

    /**
     * 相应定时收集特性，收集后台阵列的性能和容量数据，默认为5分钟，该时间可调整
     * 功能实现步骤：
     * 1、通过配置的管理IP，通过SFTP获取阵列的性能配置文件，分析出所有的性能指标
     * 2、通过Rest接口，获取指定对象LUN，StoragePool等容量指标
     * 3、组装对象关系模型，形成以树为结构的树形对象模型，提供给前台对象展示使用
     *
     * @param adapterInstResource 适配器资源对象
     * @param monitoringResources 适配器监控对象
     */
    public void onCollect(ResourceConfig adapterInstResource, Collection<ResourceConfig> monitoringResources) {
        Date begin;
        begin = new Date();
        logger.info("Collect Begin.");
        logger.info("OpenSDSStorAdpater|onCollect|Inside onCollect method of OpenSDSStorAdapter class.");

        final IdentifierCredentialProperties prop = new IdentifierCredentialProperties(loggerFactory,
                adapterInstResource);
        DiscoveryResult discoveryResult = new DiscoveryResult(adapterInstResource);
        collectResult.setDiscoveryResult(discoveryResult);

        ConnectionData connectionData = new ConnectionData();

        String arrayUserPass = prop.getCredential(Constants.USER_PWD);
        String arrayHost = prop.getIdentifier(Constants.ARRAY_HOST);
        String arrayUserName = prop.getCredential(Constants.USER_NAME);
        String certificatePath = prop.getIdentifier(Constants.CERTIFICATE_PATH);

        connectionData.setUsername(arrayUserName);
        connectionData.setPassword(arrayUserPass);
        connectionData.setCertificateFilePath(certificatePath);
        connectionData.setStrictCheckHostName(Constants.NO_CHECK_DOMAIN);

        // 判断用户输出的信息是否可以登录到Storage
        ConnectionResults connectionResults = new ConnectionResults(connectionData, arrayHost, logger);
        boolean connectFlag = connectionResults.getConnectionResults();
        if (!connectFlag) {
            logger.error("Could not connect to Storage:" + arrayHost);
            setResourceDown(adapterInstResource);
            return;
        }
        // Try to get arrayDevsn
        String arrayDevsn = null;
        String storageName = null;
        storageName = RestApiUtil.getInstance().getStorageName(connectionData);

        arrayDevsn = RestApiUtil.getInstance().getDevsnId(connectionData);
        if (storageName != null && !storageName.equals("null")) {
            if (logger.isInfoEnabled()) {
                logger.info("Get StorageName Success:" + storageName + ".");
            }
        }
        if (arrayDevsn != null && !arrayDevsn.equals("null")) {
            if (logger.isInfoEnabled()) {
                logger.info("Get ArrayDevsn Success:" + arrayDevsn + ".");
            }
        }

        // 如果获取不到storageName或者arrayDevsn,终止这次收集,等待下一次收集开始
        if (StringUtils.isBlank(storageName) || storageName.equals("null") || StringUtils.isBlank(arrayDevsn)
                || arrayDevsn.equals("null")) {
            logger.error("Get StorageName or ArrayDevsn faild");
            setResourceDown(adapterInstResource);
            return;
        }
        String resourceId = String.valueOf(adapterInstResource.getResourceId());
        String fileTempPath = Constants.TEMP_FILE_PATH + Constants.FILE_SEPARATOR + storageName + "_" + resourceId;

        try {
            if (logger.isInfoEnabled()) {
                logger.info("Start check temp file path:" + URLEncoder.encode(fileTempPath, "UTF8"));
            }
        } catch (UnsupportedEncodingException e) {
            logger.error(e.getMessage());
        }
        checkDirectory(fileTempPath);
        StorData data = new StorData();
        int isSeriesStor = is18000seriesStor(connectionData, adapterInstResource);
        String utcTime = RestApiUtil.getInstance().getSystemUtcTime(connectionData);
        String perfPath = getPerfFilePath(connectionData);
        int arrayPort = Constants.ARRAY_PORT;

        // 判断是否为18000系列的设备
        if (isSeriesStor == Constants.IS_18000_STOR) {
            extracDataFromArray(arrayHost, connectionData, arrayPort, fileTempPath, data, utcTime);
        } else if (isSeriesStor == Constants.IS_6800_STOR) {
            String path;
            if (isWindows()) {
                path = Constants.WINDOWS_C_DISC;
            } else {
                path = Constants.FILE_SEPARATOR + Constants.STOR_6800_FILEPATH;
            }

            compress6800DataFile(path, data, arrayDevsn);
            FileUtils fileUtils = new FileUtils(logger);
            fileUtils.deleteFile(new File(fileTempPath));
        } else {
            PerfStatHisFileInfo perfStatHisFileInfo = null;
            String[] host = arrayHost.split(Constants.ARRAY_HOST_SPLITER);
            for (int i = 0; i < host.length; i++) {
                String hostIp = host[i];
                if (AdapterStringUtil.isNull(hostIp)) {
                    continue;
                }
                if (logger.isInfoEnabled()) {
                    logger.info("Start extrac data from ip :" + hostIp + ".");
                }
                perfStatHisFileInfo = extracDataFromArray(hostIp, arrayUserName, arrayUserPass, arrayPort, fileTempPath,
                        utcTime, perfPath);
                if (perfStatHisFileInfo == null) {
                    continue;
                }
                // add data to StorData
                if (logger.isInfoEnabled()) {
                    logger.info("Start add data to StorData!");
                }
                LoadDataFactory.getInstance().add2MergeStorData(data, perfStatHisFileInfo);
            }
            if (perfStatHisFileInfo == null) {
                // 如果获取不到性能文件，对象关闭
                logger.error("Unable to get performance file");
                setResourceDown(getAdapterInstResource());
            }
            if (logger.isInfoEnabled()) {
                logger.info("start to deleteFile:" + fileTempPath);
            }
            FileUtils fileUtils = new FileUtils(logger);
            fileUtils.deleteFile(new File(fileTempPath));
        }

        addData(monitoringResources, discoveryResult, connectionData, storageName, data);
        Date end = new Date();
        long diffInSecs = (end.getTime() - begin.getTime()) / Constants.MILLISECODS_TO_SECONDS;
        if (logger.isInfoEnabled()) {
            logger.info("Collect END - {} seconds:" + diffInSecs + ".");
        }
    }

    /**
     * 添加数据
     *
     * @param monitoringResources Monitoring resources
     * @param discoveryResult Discovery result
     * @param connectionData Connection data
     * @param storageName Storage name
     * @param data Data
     */
    private void addData(Collection<ResourceConfig> monitoringResources, DiscoveryResult discoveryResult,
        ConnectionData connectionData, String storageName, StorData data) {
        getRestData = new GetRestData(this, loggerFactory);
        String arrayDevsn = data.getDeviceSn();
        ResourceKey storageResouce = addStorageData(discoveryResult,
                arrayDevsn, connectionData, monitoringResources, storageName);
        Map<String, ResourceKey> controllerResource = addControllerData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> diskDomainResource = addDiskDomainData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> diskResource = addDiskData(discoveryResult,
                data, arrayDevsn, connectionData, monitoringResources);
        Map<String, ResourceKey> fileSystemRes = addFileSystemData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> hostResource = addHostData(discoveryResult,
                data, arrayDevsn, connectionData, monitoringResources);
        Map<String, ResourceKey> lunResource = addLunData(discoveryResult,
                data, arrayDevsn, connectionData, monitoringResources);
        Map<String, ResourceKey> poolResource = addPoolData(discoveryResult,
                data, arrayDevsn, connectionData, monitoringResources);
        Map<String, ResourceKey> fcPortResource = addFCPortData(discoveryResult,
                data, arrayDevsn, connectionData, monitoringResources);
        Map<String, ResourceKey> iscsiPortResource = addISCSIPortData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> remoteResource = addRemoteReplicationData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> snapShotResource = addSnapShotData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);
        Map<String, ResourceKey> smartPartitionResource = addSmartPartitionData(
                discoveryResult, data, arrayDevsn, monitoringResources);
        Map<String, ResourceKey> smartQosResource = addSmartQosData(
                discoveryResult, data, arrayDevsn, connectionData,
                monitoringResources);

        Relationships rel = new Relationships();

        logger.info("Start add storageResouce!");
        List<Map<String, ResourceKey>> storageChildList =
                new ArrayList<Map<String, ResourceKey>>();
        addStorageChildMap(controllerResource, storageChildList);
        addStorageChildMap(diskDomainResource, storageChildList);
        addStorageChildMap(hostResource, storageChildList);
        addStorageChildMap(smartPartitionResource, storageChildList);
        addStorageChildMap(smartQosResource, storageChildList);
        addStorageRelationships(storageResouce, storageChildList, rel);
        logger.info("Start add lun2ControllerShip!");
        addAdapterRelationships(data.getLunData().getLun2ControllerShip(),
                controllerResource, lunResource, rel);
        logger.info("Start add pool2DiskDomainShip!");
        addAdapterRelationships(data.getPoolData().getPool2DiskDomainShip(),
                diskDomainResource, poolResource, rel);
        logger.info("Start add lun2PoolShip!");
        addAdapterRelationships(data.getLunData().getLun2PoolShip(),
                poolResource, lunResource, rel);
        logger.info("Start add fcport2ControllerShip!");
        addAdapterRelationships(data.getFcPortData().getFcport2ControllerShip(),
                controllerResource, fcPortResource, rel);
        logger.info("Start add iscsiport2ControllerShip!");
        addAdapterRelationships(
                data.getIscsiPortData().getIscsiport2ControllerShip(),
                controllerResource, iscsiPortResource, rel);
        logger.info("Start add disk2DiskDomainShip!");
        addAdapterRelationships(data.getDiskData().getDisk2DiskDomainShip(),
                diskDomainResource, diskResource, rel);
        logger.info("Start add fs2PoolShip!");
        addAdapterRelationships(data.getFileSystemData().getFs2PoolShip(),
                poolResource, fileSystemRes, rel);
        logger.info("Start add snapShot2LunShip!");
        addAdapterRelationships(data.getSnapShotModel().getSnapShot2LunShip(),
                lunResource, snapShotResource, rel);
        logger.info("Start add replication2LunShip!");
        addAdapterRelationships(
                data.getRemoteReplicationModel().getReplication2LunShip(),
                lunResource, remoteResource, rel);
        logger.info("Start add lun2QosShip!");
        addAdapterRelationships(data.getSmartQosModel().getQos2LunShip(),
                smartQosResource, lunResource, rel);
        logger.info("Start add lun2PartitionShip!");
        addAdapterRelationships(data.getLunData().getLun2PartitionShip(),
                smartPartitionResource, lunResource, rel);
        logger.info("Start add fs2ControllerShip!");
        addAdapterRelationships(data.getFileSystemData().getFs2ControllerShip(),
                controllerResource, fileSystemRes, rel);
        logger.info("Start add replication2FileSystemShip!");
        addAdapterRelationships(
                data.getRemoteReplicationModel()
                        .getReplication2FileSystemShip(), fileSystemRes,
                remoteResource, rel);
        logger.info("Start add fs2QosShip!");
        addAdapterRelationships(data.getSmartQosModel().getQos2FileSystemShip(),
                smartQosResource, fileSystemRes, rel);
        logger.info("Start add vcenter datastorage relationships!");
        getvCenterdata(rel, data, discoveryResult, arrayDevsn, monitoringResources);

        discoveryResult.addRelationships(rel);
    }

    /**
     * 添加storage子对象
     *
     * @param storageChildresource storage子对象资源
     * @param storageChildList     storage子对象集合
     */
    private void addStorageChildMap(Map<String, ResourceKey> storageChildresource,
                                    List<Map<String, ResourceKey>> storageChildList) {
        if (!isNullMap(storageChildresource)) {
            storageChildList.add(storageChildresource);
        }
    }

    /**
     * 1.Doradoc20，Doradoc21性能文件存放路径有变化：/OSM/coffer_data/omm/perf/perf_files/
     * 2.通过rest接口获取性能文件存放路径，如果获取失败，优先在/OSM/coffer_data/perf/perf_files/路径下获取
     * @param connectionData 连接数据
     * @return 文件路径
     */
    private String getPerfFilePath(ConnectionData connectionData) {
        String perfPath = "";
        try {
            Map<String, Map<String, String>> allFilePathData = RestApiUtil.getInstance().getPerfFilePath(
                    connectionData);
            for (Map<String, String> data : allFilePathData.values()) {
                perfPath = data.get("CMO_STATISTIC_FILE");
                if (logger.isInfoEnabled()) {
                    logger.info("the rest perfPath<---->CMO_STATISTIC_FILE:" + perfPath);
                }
            }
            if (!"".equals(perfPath)) {
                perfPath = perfPath.substring(perfPath.indexOf("/"), perfPath.lastIndexOf("/") + 1);
            }
            if ("".equals(perfPath) || perfPath == null) {
                perfPath = Constants.DATA_PERF_FILE_FOLDER;
            }
            if (logger.isInfoEnabled()) {
                logger.info("get perfPath from rest:" + perfPath);
            }
        } catch (Exception e) {
            logger.error("get perfPath fail from rest:" + e.getMessage());
            perfPath = Constants.DATA_PERF_FILE_FOLDER;
        }
        return perfPath;
    }

    /**
     * 解析后端存储提供的性能文件
     * 功能实现步骤：
     * 1、通过ＦＴＰ登陆到存储阵列
     * 2、查找最新的一个性能文件
     * 3、获取该性能文件，然后按照分析规格分析出指定的性能数据,返回分析完成的性能数据
     *
     * @param host     登陆主机IP
     * @param uname    登陆用户名
     * @param upass    密码
     * @param port     登陆端口
     * @param tempPath 临时文件目录
     * @return 解析后的数据
     */
    public PerfStatHisFileInfo extracDataFromArray(String host, String uname, String upass, int port, String tempPath,
        String utcTime, String perfPath) {
        SftpAccount account = new SftpAccount(host, uname, upass, port);
        SftpUtil ftpUtil = new SftpUtil(logger);

        String newDataFile = ftpUtil.getDataFileNew(account, perfPath, utcTime);
        if ("".equals(newDataFile) || newDataFile == null) {
            logger.error("the data file not exists from the path<----->" + perfPath);
            if (logger.isInfoEnabled()) {
                logger.info("start get data file: device is Doradoc20 or Doradoc21 from the path<---->"
                        + Constants.DATA_PERF_FILE_FOLDER_C20);
            }
            newDataFile = ftpUtil.getDataFileNew(account, Constants.DATA_PERF_FILE_FOLDER_C20, utcTime);
        }
        String filePath = host + Constants.SERVER_SPLITER + perfPath;
        String downLoadFile = perfPath + newDataFile;

        // 下载最新性能文件
        if (logger.isInfoEnabled()) {
            logger.info("Start download file:" + filePath + "|newDataFile:" + newDataFile + ".");
        }
        ftpUtil.downloadFile(account, downLoadFile, tempPath);

        try {
            String datafile = tempPath + File.separator + newDataFile;
            if (logger.isInfoEnabled()) {
                logger.info("Start extrac data file :" + URLEncoder.encode(datafile, "UTF8"));
            }
            PerfStatHisFileProxy perfStatHisFileProxy = new PerfStatHisFileProxy();
            return perfStatHisFileProxy.queryPerfStatHisFileInfoByCompress(datafile, logger);
        } catch (Exception e) {
            logger.error("Extrac data file fail!" + e.getMessage());
            logger.error("Unable to get performance file:" + host);
        }
        return null;
    }

    /**
     * 下载解析18000系列阵列性能文件
     *
     * @param host           ip地址
     * @param connectionData 连接信息
     * @param port           端口
     * @param tempPath       临时文件目录
     * @param data           性能文件对象
     */
    public void extracDataFromArray(String host, ConnectionData connectionData, int port, String tempPath,
        StorData data, String utcTime) {
        SftpAccount account = new SftpAccount(host, connectionData.getUsername(), connectionData.getPassword(), port);
        SftpUtil ftpUtil = new SftpUtil(logger);

        List<String> dataFileNews = ftpUtil.getDataFileNews(account,
                Constants.IS_18000_DATA_PERF_FILE_FOLDER, utcTime);
        if (logger.isInfoEnabled()) {
            logger.info("Get dataFileNews success:" + dataFileNews + ".");
        }
        PerfStatHisFileInfo perfStatHisFileInfo = null;
        for (String newDataFile : dataFileNews) {
            String downLoadFile = Constants.IS_18000_DATA_PERF_FILE_FOLDER
                    + newDataFile;
            // 下载最新性能文件
            if (logger.isInfoEnabled()) {
                logger.info("Start download file:" + downLoadFile + ".");
            }
            ftpUtil.downloadFile(account, downLoadFile, tempPath);
            try {
                String datafile = tempPath + File.separator + newDataFile;
                if (logger.isInfoEnabled()) {
                    logger.info("Start extrac data file :" + URLEncoder.encode(datafile, "UTF8"));
                }
                PerfStatHisFileProxy perfStatHisFileProxy = new PerfStatHisFileProxy();
                perfStatHisFileInfo = perfStatHisFileProxy
                        .queryPerfStatHisFileInfoByCompress(datafile, logger);

            } catch (UnsupportedEncodingException e) {
                logger.error(e.getMessage());
            } catch (Exception e) {
                logger.error("Extrac data file fail!" + e.getMessage());
                // 收集状态修改为不可用

                continue;
            }
            if (perfStatHisFileInfo == null) {
                continue;
            }
            LoadDataFactory.getInstance().add2MergeStorData(data,
                    perfStatHisFileInfo);
        }
        if (logger.isInfoEnabled()) {
            logger.info("start to deleteFile:" + tempPath);
        }
        FileUtils fileUtils = new FileUtils(logger);
        fileUtils.deleteFile(new File(tempPath));
        if (perfStatHisFileInfo == null) {
            logger.error("Unable to get performance file.");
            setResourceDown(getAdapterInstResource());
        }
    }

    /**
     * 获取vCenter数据,并给阵列上的虚拟机添加对象和关系
     *
     * @param rel                 关系模型
     * @param data                性能文件数据集合
     * @param discoveryResult     结果集
     * @param devSN               设备号
     * @param monitoringResources 元素集合
     */
    public void getvCenterdata(Relationships rel, StorData data, DiscoveryResult discoveryResult,
        String devSN, Collection<ResourceConfig> monitoringResources) {
        final IdentifierCredentialProperties prop = new IdentifierCredentialProperties(
                loggerFactory, getAdapterInstResource());
        ConnectionData connectionData = getVCenterConnectInfo(prop);
        ArrayList<ResourceKey> resourceKeyList = new ArrayList<ResourceKey>();

        VCenterModule vCenterData = null;
        try {
            vCenterData = VCenterManager.getvCenterData(connectionData);
        } catch (IOException e) {
            logger.error("Get getvCenterData error:" + e.getMessage());
        }
        if (vCenterData == null) {
            return;
        }
        List<DataStore> dataStores = vCenterData.getDataStore();
        for (DataStore dataStore : dataStores) {
            if (logger.isInfoEnabled()) {
                logger.info("dataStore|:" + dataStore + ".");
            }
            List<FSlun> fslunlist = dataStore.getFslun();
            Naslun naslun = dataStore.getNaslun();
            ArrayList<ResourceKey> lunList = new ArrayList<ResourceKey>();
            if (fslunlist != null || naslun != null) {
                String dateStoreName = dataStore.getDateStoreName();
                String dateStorevol = dataStore.getDateStorevol();
                ResourceKey datastoreResource = new ResourceKey(dateStoreName,
                        "Datastore", "VMWARE");
                ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig(
                        "VMEntityName", dateStoreName, false);
                ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig(
                        "VMEntityObjectID", dateStorevol, true);
                ResourceIdentifierConfig ric3 = new ResourceIdentifierConfig(
                        "VMEntityVCID", vCenterData.getVcid(), true);
                datastoreResource.addIdentifier(ric1);
                datastoreResource.addIdentifier(ric2);
                datastoreResource.addIdentifier(ric3);

                if (fslunlist != null && !fslunlist.isEmpty()) {
                    boolean flag = false;
                    // 判断这个阵列上是否有vCenter上的提取出的lun
                    Map<String, ResourceKey> lunWWN = data.getLunData().getLunWWN();
                    for (FSlun fsLUN : fslunlist) {
                        String wwn = fsLUN.getWwn();
                        if (lunWWN.containsKey(wwn)) {
                            lunList.add(lunWWN.get(wwn));
                            if (logger.isInfoEnabled()) {
                                logger.info("LunList add lunWWN is:" + lunWWN.get(wwn) + ".");
                            }
                            flag = true;
                        }
                    }
                    if (flag) {
                        List<VMware> VMS = dataStore.getVms();

                        for (VMware vmware : VMS) {
                            ResourceKey resource = new ResourceKey(
                                    vmware.getVmwareVmname(),
                                    "VirtualMachine", "OpenSDSStorAdapter");
                            ResourceIdentifierConfig ric11 = new ResourceIdentifierConfig(
                                    "Id", devSN + "-VirtualMachine-"
                                    + vmware.getVmwareVal(),
                                    true);
                            ResourceIdentifierConfig ric22 = new ResourceIdentifierConfig(
                                    "Name", vmware.getVmwareVmname(), false);
                            resource.addIdentifier(ric11);
                            resource.addIdentifier(ric22);

                            if (isNewResource(resource)) {
                                discoveryResult.addResource(
                                        new ResourceConfig(resource));
                                continue;
                            }
                            ResourceConfig resourceConfig = getMonitoringResource(
                                    resource);
                            if (resourceConfig == null) {
                                continue;
                            }

                            MetricKey mk = new MetricKey();
                            mk.add("maxMksConnections");
                            Integer value = vmware.getMaxMksConnections();
                            if (value == null) {
                                value = 0;
                            }
                            MetricData maxMksData = new MetricData(mk, System.currentTimeMillis(), value);

                            MetricKey mkMaxCpu = new MetricKey();
                            mkMaxCpu.add("MaxCpuUsage");
                            Integer mkMaxCpuValue = vmware.getMaxCpuUsage();
                            if (mkMaxCpuValue == null) {
                                mkMaxCpuValue = 0;
                            }
                            MetricKey mkMaxMemory = new MetricKey();
                            mkMaxMemory.add("MaxMemoryUsage");

                            Integer maxMemoryValue = vmware
                                    .getMaxMemoryUsage();
                            if (maxMemoryValue == null) {
                                maxMemoryValue = 0;
                            }
                            MetricData maxMemoryValueData = new MetricData(
                                    mkMaxMemory, System.currentTimeMillis(),
                                    maxMemoryValue);

                            MetricKey mkMemoryOverhead = new MetricKey();
                            mkMemoryOverhead.add("MemoryOverhead");
                            Long memoryOverheadValue = vmware
                                    .getMemoryOverhead();

                            MetricData memoryOverheadData = new MetricData(
                                    mkMemoryOverhead,
                                    System.currentTimeMillis(),
                                    memoryOverheadValue);
                            MetricData maxCpuUsageData = new MetricData(mkMaxCpu, System.currentTimeMillis(),
                                    mkMaxCpuValue);

                            addMetricData(resourceConfig, maxMksData);
                            addMetricData(resourceConfig, maxCpuUsageData);
                            addMetricData(resourceConfig, maxMemoryValueData);
                            addMetricData(resourceConfig, memoryOverheadData);
                            if (!lunList.isEmpty()) {
                                lunList.add(resource);
                            }
                            resourceKeyList.add(resource);
                        }
                    }
                } else {
                    if (logger.isInfoEnabled()) {
                        logger.info("The dataStore:" + dateStoreName
                                + " has no fslun or naslun at the STORE.");
                    }
                }
                if (logger.isInfoEnabled()) {
                    logger.info("Add datastore rel" + datastoreResource + "|" + lunList + ".");
                }
                if (!lunList.isEmpty()) {
                    rel.addRelationships(datastoreResource, lunList);
                }
            }
            changeResState(monitoringResources,
                    resourceKeyList,
                    "VirtualMachine");
        }
    }

    /**
     * 获取vCenter前台配置信息，公用函数封装
     *
     * @param prop 前台属性配置信息，包含vCenter基本配置信息
     * @return connectionData 返回组装好的的vCenter连接信息
     */
    private ConnectionData getVCenterConnectInfo(final IdentifierCredentialProperties prop) {
        String vCenterRestUrl = String
                .valueOf(prop.getIdentifier(Constants.VCENTER_REST_URL));
        String vCenterUserName = String
                .valueOf(prop.getCredential(Constants.VCENTER_USER_NAME));
        String vCenterPW = String
                .valueOf(prop.getCredential(Constants.VCENTER_USER_PWD));
        vCenterRestUrl = Constants.ARRAY_PROTOCOL + vCenterRestUrl
                + Constants.VCENTER_URL_END;
        ConnectionData connectionData = new ConnectionData();
        connectionData.setHostURL(vCenterRestUrl);
        connectionData.setUsername(vCenterUserName);
        connectionData.setPassword(vCenterPW);

        return connectionData;
    }

    /**
     * 添加存储对象关系
     *
     * @param resource         存储元素
     * @param storageChildList 存储子对象集合
     * @param rel              关系模型
     */
    public void addStorageRelationships(ResourceKey resource, List<Map<String, ResourceKey>> storageChildList,
        Relationships rel) {
        if (storageChildList.isEmpty()) {
            return;
        }
        List<ResourceKey> child = new ArrayList<ResourceKey>();
        for (Map<String, ResourceKey> map : storageChildList) {
            for (ResourceKey resourceKey : map.values()) {
                child.add(resourceKey);
            }
        }
        rel.setRelationships(resource, child);
        if (logger.isInfoEnabled()) {
            logger.info("Add StorageRelation relationships : parent =" + resource + ",children=" + child + ".");
        }
    }

    /**
     * 添加适配器对象关系
     * 通用添加适配器对象关系函数
     *
     * @param relationMap          对象关系表
     * @param parentResourceKeyMap 父对象关系
     * @param childResourceKeyMap  子对象关系
     * @param rel                  关系模型
     */
    public void addAdapterRelationships(Map<String, List<String>> relationMap,
        Map<String, ResourceKey> parentResourceKeyMap,
        Map<String, ResourceKey> childResourceKeyMap, Relationships rel) {
        if (isNullMap(relationMap) || isNullMap(childResourceKeyMap)
                || isNullMap(parentResourceKeyMap)) {
            logger.error("Add Adapter Relationships error:null"
                    + "relationMap:|" + relationMap + "parentResourceKeyMap :|"
                    + parentResourceKeyMap + "childResourceKeyMap:|"
                    + childResourceKeyMap);
            return;
        }
        for (Map.Entry<String, List<String>> entry : relationMap.entrySet()) {
            if (parentResourceKeyMap.get(entry.getKey()) == null) {
                continue;
            }
            List<ResourceKey> childResourceKeyList = new ArrayList<ResourceKey>();
            List<String> child = entry.getValue();
            for (String childId : child) {
                ResourceKey childRes = childResourceKeyMap.get(childId);
                if (childRes == null) {
                    continue;
                }
                childResourceKeyList.add(childRes);
            }
            if (!childResourceKeyList.isEmpty()) {
                rel.addRelationships(parentResourceKeyMap.get(entry.getKey()),
                        childResourceKeyList);
                if (logger.isInfoEnabled()) {
                    logger.info("Add relationships : parent =" + parentResourceKeyMap.get(entry.getKey())
                            + ",children=" + childResourceKeyList + ".");
                }
            }
        }
    }

    /**
     * 检查临时目录是否存在
     *
     * @param fileTempPath 临时文件路径
     * @exception/throws 违例类型  违例说明
     */
    public void checkDirectory(String fileTempPath) {
        if (VerifyUtil.isEmpty(fileTempPath)) {
            return;
        }
        File file = new File(fileTempPath);
        if (!file.exists()) {
            if (!file.mkdirs()) {
                logger.error("Make directory error.");
            }
        }
    }

    /**
     * 添加存储数据
     *
     * @param discoveryResult     结果集
     * @param devSN               阵列序列号
     * @param connectionData      连接集合
     * @param monitoringResources 元素集合
     * @param storageName         存储名称
     * @return resource 存储对象
     */
    public ResourceKey addStorageData(DiscoveryResult discoveryResult, String devSN, ConnectionData connectionData,
        Collection<ResourceConfig> monitoringResources, String storageName) {
        if (StringUtils.isBlank(devSN) || StringUtils.isBlank(storageName)) {
            logger.error("OpenSDS Storage SN is empty, Please check whether the network is normal?");
            return null;
        }
        ResourceKey resource = new ResourceKey(storageName + "-" + devSN,
                "Storage", "OpenSDSStorAdapter");
        ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig("Id",
                devSN + "Storage" + "", true);
        ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig("Name",
                devSN + "-" + storageName, false);
        resource.addIdentifier(ric1);
        resource.addIdentifier(ric2);
        if (isNewResource(resource)) {
            discoveryResult.addResource(new ResourceConfig(resource));
            return resource;
        }
        if (logger.isInfoEnabled()) {
            logger.info("Add storage:" + resource + ".");
        }
        long timestamp = System.currentTimeMillis();

        Map<String, Map<String, String>> allSystemData = RestApiUtil
                .getInstance().getSystemData(connectionData);
        if (logger.isInfoEnabled()) {
            logger.info("Systemdata:" + allSystemData + ".");
        }
        ResourceConfig resourceConfig = getMonitoringResource(resource);
        if (logger.isInfoEnabled()) {
            logger.info("Resource config:" + resourceConfig + ".");
        }
        if (resourceConfig == null) {
            return resource;
        }

        for (Map<String, String> data : allSystemData.values()) {
            MetricKey metricKey = new MetricKey();
            metricKey.add("SystemId");
            MetricData metricData = new MetricData(metricKey, timestamp,
                    data.get("ID"));
            addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("SystemName");
            metricData = new MetricData(metricKey, timestamp, data.get("NAME"));
            addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("ProductMode");
            String mode = data.get("PRODUCTMODE");
            if (mode != null && !"".equals(mode)) {
                if (Constants.PRODUCTMODE_V5R7C60.indexOf(mode) != -1) {
                    mode = data.get("productModeString");
                    if (logger.isInfoEnabled()) {
                        logger.info("in productModeString ...mode:" + mode);
                    }
                } else {
                    mode = ProductModeMaps.getValue(mode);
                }
            }
            if (logger.isInfoEnabled()) {
                logger.info("mode:" + mode);
            }
            metricData = new MetricData(metricKey, timestamp, mode);
            addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("ProductVersion");
            String productVersion = null;
            String pointRelease = data.get("pointRelease");
            if (null != pointRelease && pointRelease.length() != 0) {
                productVersion = pointRelease;
            } else {
                productVersion = data.get("PRODUCTVERSION");
            }
            metricData = new MetricData(metricKey, timestamp,
                    productVersion);
            addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("WWN");
            metricData = new MetricData(metricKey, timestamp, data.get("wwn"));
            addMetricData(resourceConfig, metricData);

            metricKey = new MetricKey();
            metricKey.add("TotalCapacity");
            BigDecimal num1024 = new BigDecimal("1024");
            BigDecimal num2 = new BigDecimal("2");
            String totalCapacity = data.get("TOTALCAPACITY");
            if ((totalCapacity == null || "".equals(totalCapacity)) &&
                Constants.IS18800V11.equals(data.get("PRODUCTMODE"))) {
                totalCapacity = data.get("MEMBERDISKSCAPACITY");
            }
            BigDecimal total = null;
            if (totalCapacity != null && !"".equals(totalCapacity)) {
                total = new BigDecimal(totalCapacity);
                metricData = new MetricData(metricKey, timestamp,
                        total.divide(num1024)
                                .divide(num2)
                                .divide(num1024)
                                .doubleValue());
                addMetricData(resourceConfig, metricData);
            }

            metricKey = new MetricKey();
            metricKey.add("UsedCapacity");
            String usedCapacity = data.get("USEDCAPACITY");
            BigDecimal used = null;
            if (usedCapacity != null && !"".equals(usedCapacity)) {
                used = new BigDecimal(usedCapacity);
                metricData = new MetricData(metricKey, timestamp,
                        used.divide(num1024)
                                .divide(num1024)
                                .divide(num2)
                                .doubleValue());
                addMetricData(resourceConfig, metricData);
            }

            metricKey = new MetricKey();
            metricKey.add("UserFreeCapacity");
            String userFreeCapacity = data.get("userFreeCapacity");
            BigDecimal free = null;
            if (userFreeCapacity != null && !"".equals(userFreeCapacity)) {
                free = new BigDecimal(userFreeCapacity);
                metricData = new MetricData(metricKey, timestamp,
                        free.divide(num1024)
                                .divide(num1024)
                                .divide(num2)
                                .doubleValue());
                addMetricData(resourceConfig, metricData);
            }

            metricKey = new MetricKey();
            metricKey.add("CapacityUsageRate");
            if (total != null && used != null) {
                metricData = new MetricData(metricKey, timestamp, used
                        .divide(total, Constants.SECTORS_UNIT, RoundingMode.HALF_UP)
                        .multiply(new BigDecimal(
                                Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                        .doubleValue());
                addMetricData(resourceConfig, metricData);
            }

            metricKey = new MetricKey();
            metricKey.add("CapacityFreeRate");
            if (total != null && free != null) {
                metricData = new MetricData(metricKey, timestamp, free
                        .divide(total, Constants.SECTORS_UNIT, RoundingMode.HALF_UP)
                        .multiply(new BigDecimal(
                                Double.toString(Constants.PERCENT_CONVERSION_UNIT)))
                        .doubleValue());
                addMetricData(resourceConfig, metricData);
            }
        }

        // 修改历史数据状态
        for (ResourceConfig config : monitoringResources) {
            String resourceKind = config.getResourceKind();
            if (resourceKind.equalsIgnoreCase("Storage")) {
                if (!config.getResourceName()
                        .equalsIgnoreCase(resourceConfig.getResourceName())) {
                    changeResourceState(config, StateChangeEnum.NOTEXIST);
                }
            }
        }

        return resource;
    }

    /**
     * 添加收集的所有对象数据信息
     *
     * @param discoveryResult 结果集
     * @param dataModel       数据模型
     * @param resourceKind    数据源类型
     * @param devSN           序列号
     * @return resList 返回
     */
    public Map<String, ResourceKey> addCollectData(
            DiscoveryResult discoveryResult, StorModel dataModel,
            String resourceKind, String devSN) {
        if (StringUtils.isBlank(devSN)) {
            logger.error("OpenSDS Storage SN is empty, Please check whether the network is normal?");
            return null;
        }
        Map<String, ResourceModel> dataMap = dataModel.getDataList();
        Map<String, ResourceKey> resList = new HashMap<String, ResourceKey>();

        for (Map.Entry<String, ResourceModel> entry : dataMap.entrySet()) {
            ResourceModel resData = entry.getValue();
            String resName = resData.getResourceName();
            String resId = resData.getResourceId();
            if (StringUtils.isBlank(resId)) {
                logger.error("resId id null");
                continue;
            }
            if (StringUtils.isBlank(resName)) {
                resName = resId;
                resData.setResourceName(resId);
                if (logger.isInfoEnabled()) {
                    logger.info("resName is null The resourceKind is:" + resourceKind);
                }
            }

            ResourceKey resource = new ResourceKey(resName, resourceKind,
                    "OpenSDSStorAdapter");
            ResourceIdentifierConfig ric1 = new ResourceIdentifierConfig("Id",
                    devSN + "-" + resourceKind + "-" + resId, true);
            ResourceIdentifierConfig ric2 = new ResourceIdentifierConfig("Name",
                    resName, false);

            resource.addIdentifier(ric1);
            resource.addIdentifier(ric2);
            resList.put(resId, resource);

            Map<Integer, Integer> resDataMap = resData.getResourceData();

            if (isNewResource(resource)) {
                discoveryResult.addResource(new ResourceConfig(resource));
                continue;
            }
            ResourceConfig resourceConfig = getMonitoringResource(resource);
            if (resourceConfig == null) {
                continue;
            }
            if (resourceConfig.getResourceId() != null) {
                if (logger.isInfoEnabled()) {
                    logger.info("ResourceConfig.getResourceId():" + resourceConfig.getResourceId() + ".");
                }
            } else {
                logger.error("ResourceConfig.getResourceId is null.");
            }

            long timestamp = System.currentTimeMillis();
            for (Map.Entry<Integer, Integer> kv : resDataMap.entrySet()) {
                MetricKey metricKey = new MetricKey();
                if (logger.isInfoEnabled()) {
                    logger.info("Kind=" + resourceKind + ",resName=" + resName
                            + ",key=" + kv.getKey() + ",value=" + kv.getValue() + ".");
                }
                metricKey.add(kv.getKey() + "");

                double value = kv.getValue();

                MetricData metricData = new MetricData(metricKey, timestamp,
                        value);
                if (logger.isInfoEnabled()) {
                    logger.info("The ture value:" + value);
                }
                if (logger.isInfoEnabled()) {
                    logger.info("The Metric key is:" + kv.getKey() + "value:"
                            + kv.getValue() + "resourceKind:" + resourceKind
                            + "resid" + resId + "resource" + resourceConfig
                            + "resConf.getResourceId():"
                            + resourceConfig.getResourceId());
                }
                if (logger.isInfoEnabled()) {
                    logger.info("ResourceKind:" + resourceKind
                            + "getAdapterConfigurationName()"
                            + resourceConfig.getAdapterConfigurationName() + ".");
                }

                addMetricData(resourceConfig, metricData);
            }
        }
        return resList;
    }

    /**
     * 添加Controller数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return controllerRes对象集合
     */
    public Map<String, ResourceKey> addControllerData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of controller!");
        Map<String, ResourceKey> controllerRes = addCollectData(discoveryResult,
                data.getControllerData(),
                "Controller",
                devSN);
        if (controllerRes == null) {
            return null;
        }
        changeResState(monitoringResources, controllerRes, "Controller");
        return controllerRes;
    }

    /**
     * 添加DiskDomain数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return diskdomainRes对象集合
     */
    public Map<String, ResourceKey> addDiskDomainData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of diskdomain!");
        Map<String, ResourceKey> diskdomainRes = addCollectData(discoveryResult,
                data.getDiskDomainData(),
                "DiskDomain",
                devSN);
        if (diskdomainRes == null) {
            return null;
        }
        getRestData.addDiskdomainDataByRest(connectionData, diskdomainRes);
        changeResState(monitoringResources, diskdomainRes, "DiskDomain");
        return diskdomainRes;
    }

    /**
     * 添加FCPort数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return fcPortRes对象集合
     */
    public Map<String, ResourceKey> addFCPortData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of fcport!");
        Map<String, ResourceKey> fcPortRes = addCollectData(discoveryResult,
                data.getFcPortData(),
                "FCPort",
                devSN);
        if (fcPortRes == null) {
            return null;
        }
        getRestData.addFCPortDataByRest(connectionData,
                fcPortRes,
                data.getFcPortData());
        changeResState(monitoringResources, fcPortRes, "FCPort");
        return fcPortRes;
    }

    /**
     * 添加ISCSIPort数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return iscsiPortRes对象集合
     */
    public Map<String, ResourceKey> addISCSIPortData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of iscsiport!");
        Map<String, ResourceKey> iscsiPortRes = addCollectData(discoveryResult,
                data.getIscsiPortData(),
                "ISCSIPort",
                devSN);
        if (iscsiPortRes == null) {
            return null;
        }
        getRestData.addiSCSIPortDataByRest(connectionData,
                iscsiPortRes,
                data.getIscsiPortData());
        changeResState(monitoringResources, iscsiPortRes, "ISCSIPort");
        return iscsiPortRes;
    }

    /**
     * 添加SnapShot数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return snapShotRes对象集合
     */
    public Map<String, ResourceKey> addSnapShotData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of snapshot!");
        Map<String, ResourceKey> snapShotRes = addCollectData(discoveryResult,
                data.getSnapShotModel(),
                "SnapShot",
                devSN);
        if (snapShotRes == null) {
            return null;
        }
        getRestData.addSnapShotDataByRest(connectionData,
                snapShotRes,
                data.getSnapShotModel());
        changeResState(monitoringResources, snapShotRes, "SnapShot");
        return snapShotRes;
    }

    /**
     * 添加SmartPartition数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param monitoringResources 元素集合
     * @return 对象集合
     */
    public Map<String, ResourceKey> addSmartPartitionData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of smartpartition!");
        Map<String, ResourceKey> smartPartRes = addCollectData(discoveryResult,
                data.getSmartPartitionModel(),
                "SmartPartition",
                devSN);
        if (smartPartRes == null) {
            return null;
        }
        changeResState(monitoringResources, smartPartRes, "SmartPartition");
        return smartPartRes;
    }

    /**
     * 添加SmartQos数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return qosRes对象集合
     */
    public Map<String, ResourceKey> addSmartQosData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of smartqos!");
        Map<String, ResourceKey> qosRes = addCollectData(discoveryResult,
                data.getSmartQosModel(),
                "SmartQos",
                devSN);
        if (qosRes == null) {
            return null;
        }
        getRestData.addQosDataByRest(connectionData,
                qosRes,
                data.getSmartQosModel());
        changeResState(monitoringResources, qosRes, "SmartQos");
        return qosRes;
    }

    /**
     * 添加远程复制数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return remoteRes对象集合
     */
    public Map<String, ResourceKey> addRemoteReplicationData(
            DiscoveryResult discoveryResult, StorData data, String devSN,
            ConnectionData connectionData,
            Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of remotereplication!");
        Map<String, ResourceKey> remoteRes = getRestData
                .addRemoteReplicationDataByRest(connectionData,
                        discoveryResult,
                        data.getRemoteReplicationModel(),
                        devSN,
                        monitoringResources);
        return remoteRes;
    }

    /**
     * 添加Disk数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return diskRes对象集合
     */
    public Map<String, ResourceKey> addDiskData(DiscoveryResult discoveryResult,
                                                StorData data, String devSN, ConnectionData connectionData,
                                                Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of disk!");
        Map<String, ResourceKey> diskRes = addCollectData(discoveryResult,
                data.getDiskData(),
                "Disk",
                devSN);
        if (diskRes == null) {
            return null;
        }
        getRestData.addDiskDataByRest(connectionData,
                diskRes,
                data.getDiskData());
        changeResState(monitoringResources, diskRes, "Disk");
        return diskRes;
    }

    /**
     * 添加fileSystem数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return fileSystemRes对象集合
     */
    public Map<String, ResourceKey> addFileSystemData(DiscoveryResult discoveryResult, StorData data, String devSN,
        ConnectionData connectionData, Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of filesystem!");
        Map<String, ResourceKey> fileSystemRes = addCollectData(discoveryResult,
                data.getFileSystemData(),
                "FileSystem",
                devSN);
        if (fileSystemRes == null) {
            return null;
        }
        getRestData.addFileSystemDataByRest(connectionData,
                fileSystemRes,
                data.getFileSystemData());
        changeResState(monitoringResources, fileSystemRes, "FileSystem");
        return fileSystemRes;
    }

    /**
     * 添加host数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return hostRes对象集合
     */
    public Map<String, ResourceKey> addHostData(DiscoveryResult discoveryResult,
        StorData data, String devSN, ConnectionData connectionData, Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of host!");
        Map<String, ResourceKey> hostRes = getRestData.addHostDataByRest(
                connectionData,
                discoveryResult,
                data.getHostData(),
                devSN,
                monitoringResources);
        return hostRes;
    }

    /**
     * 添加lun数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return lunRes对象集合
     */
    public Map<String, ResourceKey> addLunData(DiscoveryResult discoveryResult,
        StorData data, String devSN, ConnectionData connectionData, Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of lun!");
        Map<String, ResourceKey> lunRes = getRestData.addLunDataByRest(
                connectionData,
                discoveryResult,
                data.getLunData(),
                devSN,
                monitoringResources);
        return lunRes;
    }

    /**
     * 添加Pool数据
     *
     * @param discoveryResult     容器
     * @param data                性能文件数据容器
     * @param devSN               阵列序列号
     * @param connectionData      连接信息
     * @param monitoringResources 元素集合
     * @return poolRes对象集合
     */
    public Map<String, ResourceKey> addPoolData(DiscoveryResult discoveryResult,
        StorData data, String devSN, ConnectionData connectionData, Collection<ResourceConfig> monitoringResources) {
        logger.info("Add adapter data of pool!");
        Map<String, ResourceKey> poolRes = addCollectData(discoveryResult,
                data.getPoolData(),
                "Pool",
                devSN);
        if (poolRes == null) {
            return null;
        }
        getRestData.addPoolDataByRest(connectionData,
                poolRes,
                data.getPoolData());
        changeResState(monitoringResources, poolRes, "Pool");
        return poolRes;
    }

    /**
     * 修改已删除对象的状态
     *
     * @param monitoringResources 适配器监控对象
     * @param resourceKeyList     本次收集的ResourceKey集合
     * @param resourceKind        对象类型
     */
    public void changeResState(Collection<ResourceConfig> monitoringResources,
        List<ResourceKey> resourceKeyList, String resourceKind) {
        if (resourceKeyList.isEmpty() || monitoringResources.isEmpty()) {
            return;
        }
        if (logger.isInfoEnabled()) {
            logger.info("Start to change Resource State:" + monitoringResources + ".");
        }
        List<ResourceKey> monResourceKeyList = new ArrayList<ResourceKey>();
        // 取出这类型的对象
        for (ResourceConfig monResourceConfig : monitoringResources) {
            if (monResourceConfig.getResourceKind().equalsIgnoreCase(resourceKind)) {
                ResourceKey resourceKey = monResourceConfig.getResourceKey();
                monResourceKeyList.add(resourceKey);
                for (ResourceKey resourceKey1 : resourceKeyList) {
                    if (resourceKey.equals(resourceKey1)) {
                        if (!resourceKey.getResourceName().equals(resourceKey1.getResourceName())) {
                            if (logger.isInfoEnabled()) {
                                logger.info("<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>"
                                        + resourceKey1.getResourceName());
                            }
                        }
                    }
                }
            }
        }
        // 从历史对象中删除这次收集到的对象
        if (monResourceKeyList.isEmpty()) {
            return;
        }
        monResourceKeyList.removeAll(resourceKeyList);
        if (logger.isInfoEnabled()) {
            logger.info("After monResourceKeyList is :" + monResourceKeyList + ".");
        }
        if (monResourceKeyList.isEmpty()) {
            return;
        }
        if (logger.isInfoEnabled()) {
            logger.info("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
                    + monResourceKeyList.size());
        }
        for (ResourceKey resourceKey : monResourceKeyList) {
            if (logger.isInfoEnabled()) {
                logger.info("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<::>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
                        + resourceKey.getResourceName());
            }
        }

        // 将已删除的对象状态变更
        for (ResourceKey res : monResourceKeyList) {
            if (logger.isInfoEnabled()) {
                logger.info("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<:::>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
                        + res.getResourceName());
            }
            ResourceConfig resourceConfig = getMonitoringResource(res);
            changeResourceState(resourceConfig, StateChangeEnum.NOTEXIST);
            if (logger.isInfoEnabled()) {
                logger.info("ChangeLunState: " + "resourceKind :" + resourceKind + resourceConfig + ".");
            }
        }
    }

    /**
     * 修改已删除对象的状态
     *
     * @param monitoringResources 适配器监控对象
     * @param addCollectData      本次收集数据对象的map集合
     * @param resourceKind        对象类型
     */
    private void changeResState(Collection<ResourceConfig> monitoringResources,
        Map<String, ResourceKey> addCollectData, String resourceKind) {
        if (addCollectData.isEmpty() || monitoringResources.isEmpty()) {
            return;
        }
        if (logger.isInfoEnabled()) {
            logger.info("Start to change Resource State:" + monitoringResources + ".");
        }
        ArrayList<ResourceKey> monResourceKeyList = new ArrayList<ResourceKey>();
        ArrayList<ResourceKey> resourceKeyList = new ArrayList<ResourceKey>();

        // 从所有历史对象中取出这类型的对象
        for (Map.Entry<String, ResourceKey> entry : addCollectData.entrySet()) {
            ResourceKey resourceKey = entry.getValue();
            resourceKeyList.add(resourceKey);
        }

        // 从本次收集中取出resourceKey
        for (ResourceConfig monResourceConfig : monitoringResources) {
            if (monResourceConfig.getResourceKind()
                    .equalsIgnoreCase(resourceKind)) {
                ResourceKey resourceKey = monResourceConfig.getResourceKey();
                monResourceKeyList.add(resourceKey);
            }
        }

        // 从历史对象中删除这次收集到的对象
        monResourceKeyList.removeAll(resourceKeyList);
        if (logger.isInfoEnabled()) {
            logger.info("after monResourceKeyList is :" + monResourceKeyList);
        }
        if (monResourceKeyList.isEmpty()) {
            return;
        }
        // 将已删除的对象状态变更
        for (ResourceKey res : monResourceKeyList) {
            ResourceConfig resourceConfig = getMonitoringResource(res);
            changeResourceState(resourceConfig, StateChangeEnum.NOTEXIST);
            if (logger.isInfoEnabled()) {
                logger.info("ChangeLunState: " + "resourceKind :" + resourceKind + resourceConfig + ".");
            }
        }
    }


    /**
     * 判断是否为18000系列的设备
     *
     * @param connectionData      连接信息集合
     * @param adapterInstResource 适配器实例
     * @return 返回阵列大类型
     */
    public int is18000seriesStor(ConnectionData connectionData, ResourceConfig adapterInstResource) {
        ArrayList<String> seriesStorList = new ArrayList<String>();
        seriesStorList.add(Constants.IS18500V11);
        seriesStorList.add(Constants.IS18500V3);
        seriesStorList.add(Constants.IS18800V12);
        seriesStorList.add(Constants.IS18800V3);

        Map<String, Map<String, String>> allSystemData = RestApiUtil
                .getInstance().getSystemData(connectionData);
        for (Map<String, String> data : allSystemData.values()) {
            String productMode = data.get("PRODUCTMODE");
            if (logger.isInfoEnabled()) {
                logger.info("PRODUCTMODE:" + productMode);
            }
            if (StringUtils.isBlank(productMode)) {
                logger.error("Unable to get system model");
                setResourceDown(adapterInstResource);
            }
            if (seriesStorList.contains(productMode)) {
                if (logger.isInfoEnabled()) {
                    logger.info("The storage type is 18800.");
                }
                return Constants.IS_18000_STOR;
            }
            if (productMode.equals(Constants.IS6800V3) ||
                    productMode.equals(Constants.IS18500V12) ||
                    productMode.equals(Constants.IS18800V11) ||
                    productMode.equals(Constants.IS18800V13) ||
                    productMode.equals(Constants.D5000V6) ||
                    productMode.equals(Constants.D5000V6_N) ||
                    productMode.equals(Constants.D6000V6) ||
                    productMode.equals(Constants.D6000V6_N) ||
                    productMode.equals(Constants.D8000V6) ||
                    productMode.equals(Constants.D8000V6_N) ||
                    productMode.equals(Constants.D18000V6) ||
                    productMode.equals(Constants.D18000V6_N) ||
                    productMode.equals(Constants.D3000V6) ||
                    productMode.equals(Constants.D5000V6_I) ||
                    productMode.equals(Constants.D6000V6_I) ||
                    productMode.equals(Constants.D8000V6_I) ||
                    productMode.equals(Constants.D18000V6_I) ||
                    productMode.equals(Constants.DORADO5300_V6) ||
                    productMode.equals(Constants.DORADO5500_V6) ||
                    productMode.equals(Constants.DORADO5600_V6) ||
                    productMode.equals(Constants.DORADO5800_V6) ||
                    productMode.equals(Constants.DORADO6800_V6) ||
                    productMode.equals(Constants.DORADO18500_V6) ||
                    productMode.equals(Constants.DORADO18800_V6)) {
                if (logger.isInfoEnabled()) {
                    logger.info("The storage type is 68000v3 or 18500v1.");
                }
                return Constants.IS_6800_STOR;
            }
        }
        if (logger.isInfoEnabled()) {
            logger.info("The storage type is opensds storage.");
        }
        return Constants.IS_OPENSDS_STOR;
    }

    /**
     * 解析存储提供的68000类型阵列性能文件
     *
     * @param path 本地文件路径
     * @param data 数据存储对象
     * @param sn   阵列序列号
     */
    public void compress6800DataFile(String path, StorData data, String sn) {
        if (StringUtils.isBlank(path)) {
            if (logger.isInfoEnabled()) {
                logger.info("The path is null.");
            }
            setResourceDown(getAdapterInstResource());
        }
        String filePath = path + File.separator + Constants.STOR_6800_PERF_FOLDER;
        File dirFile = new File(filePath);
        if (!dirFile.isDirectory()) {
            logger.error("Unable to retrieve performance file directory.");
            setResourceDown(getAdapterInstResource());
        }
        File[] tempList = dirFile.listFiles();
        long latestFileTime = Constants.FIRST_VALUE;
        if (tempList == null) {
            return;
        }
        for (File file : tempList) {
            String fileName = file.getName();
            if (fileName.contains(sn) && fileName.endsWith(".tgz")) {
                long lastModifiedTime = file.lastModified();

                if (latestFileTime < lastModifiedTime) {
                    latestFileTime = lastModifiedTime;
                }
            }
        }
        if (logger.isInfoEnabled()) {
            logger.info("The First file date is:" + latestFileTime + ".");
        }
        PerfStatHisFileInfo perfStatHisFileInfo = null;
        for (File file : tempList) {
            String fileName = file.getName();
            if (fileName.contains(sn) && fileName.endsWith(".tgz")) {
                if (file.lastModified() < latestFileTime + Constants.ONE_MINUTE_MILLISECOND
                        && file.lastModified() > latestFileTime - Constants.ONE_MINUTE_MILLISECOND) {
                    String datafile = filePath + File.separator + fileName;
                    if (logger.isInfoEnabled()) {
                        logger.info("6800datafile:" + datafile);
                    }

                    try {
                        PerfStatHisFileProxy perfStatHisFileProxy = new PerfStatHisFileProxy();
                        perfStatHisFileInfo = perfStatHisFileProxy.queryPerfStatHisFileInfoByCompress(
                                datafile, logger);
                    } catch (Exception e) {
                        logger.error("datafile Compress error" + datafile);
                        continue;
                    }
                    if (perfStatHisFileInfo == null) {
                        continue;
                    }
                    LoadDataFactory.getInstance().add2MergeStorData(data, perfStatHisFileInfo);
                }
            }
        }
        FileUtils fileUtils = new FileUtils(logger);
        for (File file : tempList) {
            if (file.lastModified() <= latestFileTime - Constants.HALF_HOUR_MILLISECOND
                    && file.getName().length() >= Constants.FILE_NAME_MINIMUM_LENGTH) {
                fileUtils.deleteFile(file);
            }
        }
        for (File file : tempList) {
            String filename = file.getName();
            String extensionName = fileUtils.getExtensionName(filename);
            if (filename.contains(sn) && extensionName.contains("dat")) {
                fileUtils.deleteFile(file);
            }
        }

        if (perfStatHisFileInfo == null) {
            logger.error("Unable to get performance file");
            setResourceDown(getAdapterInstResource());
        }
    }

    /**
     * 判断对象是否为空
     *
     * @param map Map
     * @return boolean
     */
    public Boolean isNullMap(Map map) {
        return map == null || map.isEmpty() ? true : false;
    }

    /**
     * 判断当前运行环境是否为windows
     *
     * @return 是windows返回true, 否则false
     */
    public boolean isWindows() {
        String property = System.getProperties().getProperty("os.name");
        if (property == null) {
            logger.error("Can't get OS type.");
            setResourceDown(getAdapterInstResource());
        } else if (property.toLowerCase(Locale.getDefault()).startsWith("win")) {
            return true;
        } else {
            logger.error("Can't get OS type with" + Locale.getDefault());
        }
        return false;
    }
}
