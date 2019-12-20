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

package org.opensds.storage.conection.rest.operation;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.extracdata.util.LogUtil;

public class RestInterfaceConfs {
    /**
     * operationName , [restMethod,restRelativePath]
     */
    public static Map<String, List<String>> conf = new HashMap<String, List<String>>();

    private static final Logger logger;

    // when the class is loading , load the properties once
    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        logger = context.getLogger(RestInterfaceConfs.class.getName());

        Properties properties = new Properties();
        try {
            properties.load(RestInterfaceConfs.class
                    .getResourceAsStream("/org/opensds/storage/conection/rest/config/restOperations.properties"));
            for (Object key : properties.keySet()) {
                String value = (String) properties.get(key);
                String[] attrs = value.split(";");
                List<String> list = new ArrayList<String>();
                list.add(attrs[0]);
                list.add(attrs[1]);
                conf.put((String) key, list);
            }
        } catch (IOException e) {
            logger.error("The config file loading error : " + e.getMessage(), e);
        }
    }

    /**
     * check operation
     * @param operationName Operation name
     * @return boolean
     * @throws RestException
     */
    public static boolean isValidOperation(String operationName) throws RestException {
        doCheck();
        return conf.get(operationName) == null ? false : true;
    }

    private static void doCheck() throws RestException {
        if (conf.size() == 0) {
            throw OperationError.INTERNAL.OPERATION_CONFIG_FILE_LOADING_ERROR
                    .newException(RestInterfaceConfs.class.getResource("") + "config/restOperations.properties");
        }
    }

    /**
     * Get rest request method
     * @param operationName Operation name
     * @return Method name
     * @throws RestException
     */
    public static String getRestMethod(String operationName) throws RestException {
        doCheck();
        return conf.get(operationName).get(0);
    }

    /**
     * Get rest relative path
     * @param operationName Operation name
     * @return Relative path
     * @throws RestException
     */
    public static String getRestRelativePath(String operationName) throws RestException {
        doCheck();
        return conf.get(operationName).get(1);
    }
}
