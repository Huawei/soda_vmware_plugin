// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.storage.vro.plugin.model;

import java.io.IOException;
import java.util.List;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import org.opensds.storage.conection.rest.connection.HttpClientFactory;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;

import ch.dunes.util.EncryptHelper;

public class Configration {
    private static final Logger LOG = Logger.getLogger(Configration.class);

    private static final String PACKAGE_NAME = "org.opensds.storage";

    private SysInfoManager sysInfoManager = new SysInfoManager();

    private String id;

    private String name;

    public Configration(String id, String name) {
        super();
        this.id = id;
        this.name = name;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    /**
     * regist OceanStor
     * @param arrayName OceanStor name
     * @param hostname OceanStor ip address
     * @param port OceanStor port
     * @param candidateIpList candidate ip list
     * @param needSwithIp need switch ip or not
     * @param username OceanStor username
     * @param password OceanStor password
     * @param checkCert check cert or not
     * @param productModel OceanStor product model
     * @throws IOException
     * @throws StorageCommonException
     * @throws RestException
     */
    public void register(String arrayName, String hostname, String port,
        String candidateIpList, boolean needSwithIp, String username, String password,
        boolean checkCert, String productModel) throws IOException, StorageCommonException, RestException {
        String uniqId = hostname + "-" + port;
        StorageArrayInfo arrayInfo = new StorageArrayInfo(uniqId, hostname,
                port);
        arrayInfo.setArrayName(arrayName);
        arrayInfo.setHostName(hostname);
        arrayInfo.setPort(port);
        arrayInfo.setNeedSwithIp(needSwithIp);
        arrayInfo.setCandidateIpList(candidateIpList);
        arrayInfo.setUsername(username);
        arrayInfo.setPassword(EncryptHelper.encrypt(password));
        arrayInfo.setCheckCert(checkCert);
        arrayInfo.setProductModel(productModel);
        if (LOG.isInfoEnabled()) {
            LOG.info("register:::" + arrayInfo);
        }
        SysInfo sysinfo = sysInfoManager.getSysInfo(arrayInfo);
        arrayInfo.setProductName(sysinfo.getProductName());
        arrayInfo.setProductVersion(sysinfo.getProductVersion());
        arrayInfo.setProductSN(sysinfo.getId());

        OpenSDSStorageRepository.getUniqueInstance().updateProperties(arrayInfo,
                OpenSDSStorageRepository.ADD);
    }

    /**
     * unregist OceanStor
     * @param arrayInfo OceanStor info
     * @throws IOException
     */
    public void unregister(StorageArrayInfo arrayInfo) throws IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info("unregister:::" + arrayInfo);
        }
        OpenSDSStorageRepository.getUniqueInstance().updateProperties(arrayInfo,
                OpenSDSStorageRepository.REMOVE);
    }

    /**
     * Reload trust store
     * @throws StorageCommonException
     */
    public void reloadTrustStore() throws StorageCommonException {
        HttpClientFactory.reloadHttpClient();
    }

    /**
     * clear OceanStor cache
     */
    public void clearCache() {
        OpenSDSStorageRepository.getUniqueInstance().clearCache();
    }

    /**
     * Open DEBUG model
     * @throws StorageCommonException
     */
    public void debugOn() throws StorageCommonException {
        LOG.info("debug On");
        Logger log = org.apache.log4j.Logger.getLogger(PACKAGE_NAME);
        if (log != null) {
            log.setLevel(Level.DEBUG);
        } else {
            throw new StorageCommonException(
                    "can not getLogger::" + PACKAGE_NAME);
        }
    }

    /**
     * Close DEBUG model
     * @throws StorageCommonException
     */
    public void debugOff() throws StorageCommonException {
        LOG.info("debug Off");
        Logger log = org.apache.log4j.Logger.getLogger(PACKAGE_NAME);
        if (log != null) {
            log.setLevel(Level.INFO);
        } else {
            throw new StorageCommonException(
                    "can not getLogger::" + PACKAGE_NAME);
        }
    }

    /**
     * Get all OceanStors info
     * @return List<StorageArrayInfo>
     */
    public List<StorageArrayInfo> getStorageArrayInfos() {
        return OpenSDSStorageRepository.getUniqueInstance()
                .getStorageArrayInfos();
    }
}
