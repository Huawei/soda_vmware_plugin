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

package org.opensds.ecosystem.adapter.thread;

import java.util.Locale;
import java.util.concurrent.Callable;

import org.apache.log4j.Logger;

import org.opensds.ecosystem.adapter.Constants;
import org.opensds.storage.conection.rest.domain.ConnectionData;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.operation.DeviceManager;

public class TestConnectByThread implements Callable<RestConnectModel> {
    private ConnectionData connectionData = new ConnectionData(); //连接信息的对象
    private Logger logger; //日志对象
    private String hostip; //阵列IP
    private Integer port; //阵列端口

    /**
     * 构造方法初始化对象
     *
     * @param connectionData 连接信息的对象
     * @param logger 日志对象
     * @param hostip 阵列IP
     * @param port 阵列端口
     */
    public TestConnectByThread(ConnectionData connectionData, Logger logger, String hostip, Integer port) {
        this.connectionData.setCertificateFilePath(connectionData.getCertificateFilePath());
        this.connectionData.setUsername(connectionData.getUsername());
        this.connectionData.setPassword(connectionData.getPassword());
        this.logger = logger;
        this.hostip = hostip;
        this.port = port;
    }

    /**
     * 重写call方法，用于多线程调用
     */
    @Override
    public RestConnectModel call() {
        DeviceManager deviceManager = null;
        String restURL = null;
        restURL = Constants.ARRAY_PROTOCOL + hostip
                + Constants.SERVER_SPLITER + port
                + Constants.ARRAY_REST_PATH;

        logger.info("Start test restURL:" + restURL);
        connectionData.setHostURL(restURL);
        deviceManager = new DeviceManager(connectionData);
        try {
            deviceManager.login();
            if (logger.isInfoEnabled()) {
                logger.info("Login storage:" + hostip + "|Success!");
            }

            String msg = String.format(Locale.ROOT,
                    "Login Storage Success,IP: '%s',Port:%s ", new Object[]{hostip, port});
            if (logger.isInfoEnabled()) {
                logger.info(msg);
            }
            return new RestConnectModel(true, restURL, hostip + ":" + port);
        } catch (RestException e) {
            String msg = String.format(Locale.ROOT,
                    "Failed to connect Storage IP: '%s' port:%s", new Object[]{hostip, port});
            logger.error(msg);
            logger.error("e:" + e);
            return new RestConnectModel(false, restURL, hostip);
        } finally {
            deviceManager.logout();
        }
    }
}
