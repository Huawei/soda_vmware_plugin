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

import java.util.List;
import java.util.Map;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.common.ProductMode;
import org.opensds.storage.conection.rest.exception.RestException;
import org.apache.log4j.Logger;

public class SysInfoManager {
    private static final Logger logger = Logger.getLogger(SysInfoManager.class);

    /**
     * Get System info
     * @param storageArrayInfo OnceanStor info
     * @return SysInfo
     * @throws RestException
     * @throws StorageCommonException
     */
    public SysInfo getSysInfo(StorageArrayInfo storageArrayInfo) throws RestException, StorageCommonException {
        return getSysInfo(storageArrayInfo, true);
    }

    /**
     * Get system info
     * @param storageArrayInfo OceanStor info
     * @param needSwithIP need switch ip
     * @return SysInfo
     * @throws RestException
     * @throws StorageCommonException
     */
    public SysInfo getSysInfo(StorageArrayInfo storageArrayInfo, boolean needSwithIP)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setSwithIP(needSwithIP);
        operation.setOperationName(OperationNamesEnum.GET_SYSINFO);
        
        List<Map<String, String>> operationResult = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (operationResult.size() != 1) {
            throw new StorageCommonException(
                    "can not get system info, sysinfo count = "
                            + operationResult.size());
        }
        String productModeEnum = operationResult.get(0)
                .get(RestContains.PRODUCTMODE);
        String pointRelease = operationResult.get(0)
                .get(RestContains.POINTRELEASE);
        String productVersion = null;
        if (null != pointRelease && !"".equals(pointRelease)) {
            productVersion = pointRelease;
        } else {
            productVersion = operationResult.get(0)
                    .get(RestContains.PRODUCTVERSION);
        }
        String id = operationResult.get(0).get(RestContains.ID);
        
        ProductMode productModeE = ProductMode
                .valueOf(Integer.valueOf(productModeEnum));
        String productModeString = null;
        if (productModeE == null) {
            productModeString = productModeEnum;
        } else if ("V500R007C60 Kunpeng".equals(productVersion)) {
            productModeString = operationResult.get(0)
                    .get(RestContains.PRODUCTMODESTRING);
        } else {
            productModeString = productModeE.name();
            if (productModeString.startsWith("U")) {
                productModeString = productModeString.substring(2);
            }
            if (productModeString.endsWith("Enhanced")) {
                productModeString = productModeString.substring(0, productModeString.length() - 9);
            }
        }
        return new SysInfo(productModeString, productVersion, id);
    }
}
