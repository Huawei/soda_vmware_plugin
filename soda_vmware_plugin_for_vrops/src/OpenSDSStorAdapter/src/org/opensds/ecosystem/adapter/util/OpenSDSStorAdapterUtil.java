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

package org.opensds.ecosystem.adapter.util;

import java.io.File;

import org.apache.log4j.Logger;

import com.integrien.alive.common.adapter3.describe.AdapterDescribe;
import com.integrien.alive.common.util.InstanceLoggerFactory;

public class OpenSDSStorAdapterUtil {
    private final Logger logger;

    /**
     * 构造方法
     *
     * @param loggerFactory 获取传递的日志工厂,便于打印日志
     */
    public OpenSDSStorAdapterUtil(InstanceLoggerFactory loggerFactory) {
        this.logger = loggerFactory.getLogger(OpenSDSStorAdapterUtil.class);
    }

    /**
     * Utility Method to create Adapter describe. Instance of AdapterDescribe is
     * created using describe.xml kept under config folder.
     *
     * @return instance of AdapterDescribe class {@link AdapterDescribe}
     */
    public AdapterDescribe createAdapterDescribe() {
        assert logger != null;

        // AdapterDescribe has all static information about the adapter like
        // what
        // all resource kinds (object types)
        // are supported by the adapter, what all metrics are expected for those
        // resource kinds

        AdapterDescribe adapterDescribe = AdapterDescribe.make(getDescribeXmlLocation() + "describe.xml");
        if (adapterDescribe == null) {
            logger.error("Unable to load adapter describe");
        } else {
            logger.debug("Successfully loaded adapter");
        }
        return adapterDescribe;
    }

    /**
     * Method to return Adapter's home directory/folder
     *
     * @return Adapter home folder path
     */
    public static String getAdapterHome() {
        // intentionally left constant CommononalConstants.ADAPTER_HOME for
        // common.jar version compatibility
        String adapterHome = System.getProperty("ADAPTER_HOME");
        String collectorHome = System.getProperty("COLLECTOR_HOME");
        if (collectorHome != null) {
            adapterHome = collectorHome + File.separator + "adapters";
        }
        return adapterHome;
    }

    /**
     * Returns the adapters root folder.
     *
     * @return String
     */
    public static String getAdapterFolder() {
        return getAdapterHome() + File.separator + "OpenSDSStorAdapter" + File.separator;
    }

    /**
     * Returns the adapters conf folder.
     *
     * @return String
     */
    public static String getConfFolder() {
        return getAdapterFolder() + "conf" + File.separator;
    }

    /**
     * Method to return describe XML location including the file name. It first
     * checks if
     *
     * @return describe XML location
     */
    public static String getDescribeXmlLocation() {
        String describeXML;
        String adapterHome = System.getProperty("ADAPTER_HOME");
        if (adapterHome == null) {
            describeXML = System.getProperty("user.dir") + File.separator;
        } else {
            describeXML = getConfFolder();
        }
        return describeXML;
    }
}
