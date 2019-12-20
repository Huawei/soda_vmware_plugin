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

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;

public class FTPShareManager {
    /**
     * Create FTP share
     * @param storageArrayInfo OceanStor info
     * @param ftpShare FTPshare
     * @throws RestException
     */
    public void createFTPShare(StorageArrayInfo storageArrayInfo, FTPShare ftpShare) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_FTP_SHARE);
        operation.putOperationData(RestContains.ACCESSNAME,
                ftpShare.getAccessname());
        operation.putOperationData(RestContains.SHAREPATH,
                ftpShare.getSharepath());
        operation.putOperationData(RestContains.PERMISSION,
                ftpShare.getPermission());
        operation.putOperationData(RestContains.UPBANDWIDTH,
                ftpShare.getUpbandwidth());
        operation.putOperationData(RestContains.DOWNBANDWIDTH,
                ftpShare.getDownbandwidth());

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete FTP share
     * @param storageArrayInfo OceanStor info
     * @param id FTP share id
     * @throws RestException
     */
    public void deleteFTPShareById(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_FTP_SHARE_BY_ID);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Update FTPshare
     * @param storageArrayInfo OceanStor info
     * @param ftpShare FTPshare
     * @throws RestException
     */
    public void updateFTPShare(StorageArrayInfo storageArrayInfo, FTPShare ftpShare) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_FTP_SHARE);
        operation.putOperationData(RestContains.ACCESSNAME,
                ftpShare.getAccessname());
        operation.putOperationData(RestContains.SHAREPATH,
                ftpShare.getSharepath());
        operation.putOperationData(RestContains.PERMISSION,
                ftpShare.getPermission());
        operation.putOperationData(RestContains.UPBANDWIDTH,
                ftpShare.getUpbandwidth());
        operation.putOperationData(RestContains.DOWNBANDWIDTH,
                ftpShare.getDownbandwidth());

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get FTP share count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getFTPShareTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation
                .setOperationName(OperationNamesEnum.GET_FTP_SHARE_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        return list.get(0).get(RestContains.COUNT);
    }

    /**
     * Get FTP share by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<FTPShare>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<FTPShare> getFTPSharesByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_FTP_SHARE_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<FTPShare> ftplist = new ArrayList<FTPShare>();
        for (Map<String, String> map : list) {
            FTPShare ftpshare = (FTPShare) ObjectsBuilder.INSTANCE
                    .buildObject(map, FTPShare.class, storageArrayInfo);
            ftplist.add(ftpshare);
        }
        return ftplist;
    }

}
