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

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.FutureTask;

import org.apache.log4j.Logger;

import org.opensds.ecosystem.adapter.Constants;
import org.opensds.storage.conection.rest.domain.ConnectionData;

public class ConnectionResults {
    private ConnectionData connectionData;
    private String arrayHost;
    private String[] hostIPList;
    private Logger logger;

    /**
     * 构造方法，构造必要的信息
     *
     * @param connectionData 存放连接信息的类
     * @param arrayHost      获取所有阵列IP
     * @param logger         日志的类
     */
    public ConnectionResults(ConnectionData connectionData, String arrayHost, Logger logger) {
        this.connectionData = connectionData;
        this.arrayHost = arrayHost;
        this.hostIPList = arrayHost.split(Constants.ARRAY_HOST_SPLITER);
        this.logger = logger;
    }

    /**
     * 循环所有的ip和端口，调用测试连接阵列的类
     *
     * @return 如果能连通则返回true，否则返回false
     */
    public boolean getConnectionResults() {
        // 判断用户输出的信息是否可以登录到Storage
        boolean connectFlag = false;
        List<FutureTask<RestConnectModel>> list =
                new ArrayList<FutureTask<RestConnectModel>>();
        ExecutorService exec = Executors.newFixedThreadPool(hostIPList.length * Constants.DOUBLE);
        ArrayList<Integer> portList = new ArrayList<Integer>();
        portList.add(Constants.ARRAY_REST_PORT_V3);
        portList.add(Constants.ARRAY_REST_PORT_V1);
        for (int i = 0; i < hostIPList.length; i++) {
            String hostip = hostIPList[i];
            for (Integer port : portList) {
                // 创建对象
                FutureTask<RestConnectModel> ft =
                        new FutureTask<RestConnectModel>(
                                new TestConnectByThread(connectionData, logger, hostip,
                                        port));
                // 添加到list,方便后面取得结果
                list.add(ft);
                // 一个个提交给线程池，当然也可以一次性的提交给线程池，exec.invokeAll(list);
                exec.submit(ft);
            }
        }
        // 开始统计结果，循环所有的线程
        for (FutureTask<RestConnectModel> tempFt : list) {
            try {
                RestConnectModel restConnectModel = tempFt.get();
                if (restConnectModel.isFlag()) {
                    // 如果有成功的登陆的线程返回，则终止所有线程
                    exec.shutdownNow();
                    String hostIP = restConnectModel.getHostIP();
                    connectFlag = true;
                    connectionData.setHostURL(restConnectModel.getUrl());
                    // DTS2018041303579 增加账户，登录时间日志打印
                    SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
                    if (logger.isInfoEnabled()) {
                        logger.info("onTest|Login Storage " + hostIP + "|Success!" + " userName: "
                                + connectionData.getUsername() + " ,Login time: " + df.format(new Date()));
                    }
                    break;
                }
            } catch (InterruptedException e) {
                logger.error("Test Connection InterruptedException:" +
                        e.getMessage());
            } catch (ExecutionException e) {
                logger.error("Test Connection ExecutionException:" +
                        e.getMessage());
            }
        }
        if (!connectFlag) {
            logger.error("The IP" + arrayHost + "can not be connected!");
            // 关闭所有线程
            exec.shutdown();
        }
        return connectFlag;
    }
}
