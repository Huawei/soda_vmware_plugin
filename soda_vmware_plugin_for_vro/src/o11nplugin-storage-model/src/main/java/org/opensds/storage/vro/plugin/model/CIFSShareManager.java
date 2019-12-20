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
import java.util.Vector;

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.vro.OpenSDSStorageRepository;

public class CIFSShareManager {
    private static final int BEGIN = 0;

    private static final int STEP = 100;

    private static final int END = 99;

    private FilesystemManager filesystemManager = new FilesystemManager();

    /**
     * Create CIFS share
     * @param storageArrayInfo OceanStor info
     * @param fs_name Filesystem name
     * @param sharePath Share path
     * @param name CIFS share name
     * @param description CIFS share description
     * @param enable_oplock Indicates whether Oplock is enabled
     * @param enable_notify Indicates whether the notify function is enabled
     * @param enable_ca Indicates whether failover is enabled
     * @param offline_cache_mode Offline Cache Mode
     * @param enable_ip_control Whether to enable the IP address access control feature
     * @throws StorageCommonException
     * @throws RestException
     */
    public void createCIFSShare(StorageArrayInfo storageArrayInfo,
        String fs_name, String sharePath, String name, String description,
        String enable_oplock, String enable_notify, String enable_ca,
        String offline_cache_mode, String enable_ip_control)
            throws StorageCommonException, RestException {
        boolean nameNotValid = name.trim().length() > 80 || name.contains("<")
                || name.contains(">") || name.contains("(")
                || name.contains(")") || name.contains("&")
                || name.trim().contains(" ") || name.contains("^")
                || name.contains("/") || name.contains(":")
                || name.contains("|") || name.contains("+")
                || name.contains(";") || name.contains(",")
                || name.contains("*") || name.contains("~")
                || name.contains("=") || name.equals("autohome");
        if (nameNotValid) {
            throw new StorageCommonException("CIFS Share name " + name
                    + " is not valid, "
                    + "it shoule be: length 1-80, can not contain "
                    + "()/[]:|<>+;,*&~= space and can not use ipc$, autohome, print$");
        }

        String path = null;
        if (sharePath == null || sharePath.trim().length() == 0) {
            Filesystem fs = filesystemManager
                    .getFilesystemByName(storageArrayInfo, fs_name);
            path = "/" + fs.getName() + "/";
        } else {
            path = sharePath;
        }

        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_CIFS_SHARE);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.SHAREPATH, path);

        operation.putOperationData(RestContains.ENABLEOPLOCK, enable_oplock);
        operation.putOperationData(RestContains.ENABLENOTIFY, enable_notify);
        operation.putOperationData(RestContains.ENABLECA, enable_ca);
        operation.putOperationData(RestContains.OFFLINEFILEMODE,
                offline_cache_mode);
        operation.putOperationData(RestContains.ENABLEIPCONTROL,
                enable_ip_control);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get CIFS share by name
     * @param storageArrayInfo OceanStor
     * @param name CIFS share name
     * @return CIFS share id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String getCIFShareIdByName(StorageArrayInfo storageArrayInfo,
        String name) throws StorageCommonException, RestException {
        int begin = BEGIN;
        int end = END;
        int totalCount = Integer.valueOf(getCIFSShareTotalCount(storageArrayInfo));
        if (totalCount == 0) {
            throw new StorageCommonException(
                    "can not find object for Name: " + name);
        }
        List<Map<String, String>> list = null;
        String id = null;
        while (true) {
            OceanStorOperation operation = new OceanStorOperation();
            operation.setOperationName(
                    OperationNamesEnum.GET_CIFS_SHARE_BY_NAME);
            operation.putOperationData(RestContains.FILTER_KEY,
                    RestContains.FILTER_NAME_PARAMETER_SPECIAL + name);
            operation.putOperationData(RestContains.RANGE,
                    "[" + begin + "-" + end + "]");
            list = OperationUtil.performAction(storageArrayInfo, operation);
            if (list.size() == 0) {
                break;
            }
            if (list.size() >= 1) {
                for (Map<String, String> map : list) {
                    CIFSShare cifsShare = (CIFSShare) ObjectsBuilder.INSTANCE
                            .buildObject(map,
                                    CIFSShare.class,
                                    storageArrayInfo);
                    if (cifsShare.getName().equals(name)) {
                        id = cifsShare.getReal_id();
                        break;
                    }
                }
                if (id != null) {
                    break;
                }
            }
            if (end > totalCount) {
                break;
            }
            int step = STEP;
            begin = begin + step;
            end = end + step;
        }
        if (id == null) {
            throw new StorageCommonException(
                    "can not find CIFShare for Name: " + name);
        }
        return id;
    }

    /**
     * Delete CIFS share
     * @param storageArrayInfo OceanStor info
     * @param id CIFS share id
     * @param name CIFS share name
     * @throws StorageCommonException
     * @throws RestException
     */
    public void deleteCIFSShareById(StorageArrayInfo storageArrayInfo,
                                    String id, String name) throws StorageCommonException, RestException {
        String real_id = null;
        if (id == null || id.trim().length() == 0) {
            real_id = getCIFShareIdByName(storageArrayInfo, name);
        } else {
            real_id = id;
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_CIFS_SHARE_BY_ID);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * modify CIFS share
     * @param storageArrayInfo OceanStor info
     * @param id CIFS share id
     * @param currentName CIFS share current name
     * @param description CIFS share description
     * @param enable_oplock Indicates whether Oplock is enabled
     * @param enable_notify Indicates whether the notify function is enabled
     * @param enable_ca Indicates whether failover is enabled
     * @param offline_cache_mode Offline Cache Mode
     * @param enable_ip_control Whether to enable the IP address access control feature
     * @throws StorageCommonException
     * @throws RestException
     */
    public void modifyCIFSShare(StorageArrayInfo storageArrayInfo, String id,
        String currentName, String description, String enable_oplock,
        String enable_notify, String enable_ca, String offline_cache_mode,
        String enable_ip_control) throws StorageCommonException, RestException {
        String real_id = null;
        if (id == null || id.trim().length() == 0) {
            real_id = getCIFShareIdByName(storageArrayInfo, currentName);
        } else {
            real_id = id;
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.UPDATE_CIFS_SHARE);
        operation.putOperationData(RestContains.ID, real_id);
        operation.putOperationData(RestContains.DESCRIPTION, description);

        operation.putOperationData(RestContains.ENABLEOPLOCK, enable_oplock);
        operation.putOperationData(RestContains.ENABLENOTIFY, enable_notify);
        operation.putOperationData(RestContains.ENABLECA, enable_ca);
        operation.putOperationData(RestContains.OFFLINEFILEMODE,
                offline_cache_mode);
        operation.putOperationData(RestContains.ENABLEIPCONTROL,
                enable_ip_control);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get CIFS share count
     * @param storageArrayInfo OcenStor info
     * @return count
     * @throws RestException
     */
    public String getCIFSShareTotalCount(StorageArrayInfo storageArrayInfo)
            throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_CIFS_SHARE_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        String count = list.get(0).get(RestContains.COUNT);
        return count;
    }

    /**
     * Get CIFS share by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<CIFSShare>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<CIFSShare> performGetCIFSShareByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_CIFS_SHARE_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<CIFSShare> cifslist = new ArrayList<CIFSShare>();
        for (Map<String, String> map : list) {
            CIFSShare cifsshare = (CIFSShare) ObjectsBuilder.INSTANCE
                    .buildObject(map, CIFSShare.class, storageArrayInfo);
            cifslist.add(cifsshare);
        }
        return cifslist;
    }

    /**
     * Get CIFS share by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<CIFSShare>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<CIFSShare> getCIFSSharesByRange(
            StorageArrayInfo storageArrayInfo, int begin, int end)
            throws RestException, StorageCommonException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin <= 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<CIFSShare> ncifsShares = performGetCIFSShareByRange(
                storageArrayInfo, begin, end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getCifsShares() == null) {
                    array.setCifsShares(new Vector<CIFSShare>());
                } else {
                    array.getCifsShares().clear();
                }
                array.getCifsShares().addAll(ncifsShares);
            }
        }

        return ncifsShares;
    }

    /**
     * add user to CIFS share
     * @param storageArrayInfo OceanStor info
     * @param name User name
     * @param cifsShareId CIFS share id
     * @param cifsshareName CIFS share name
     * @param permission permission
     * @param domaintype Domain type
     * @throws StorageCommonException
     * @throws RestException
     */
    public void addUserForCIFSShare(StorageArrayInfo storageArrayInfo,
        String name, String cifsShareId, String cifsshareName,
        String permission, String domaintype) throws StorageCommonException, RestException {
        String real_id = null;
        if (cifsShareId == null || cifsShareId.trim().length() == 0) {
            real_id = getCIFShareIdByName(storageArrayInfo, cifsshareName);
        } else {
            real_id = cifsShareId;
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.ADD_CIFS_SHARE_USER);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.PARENTID, real_id);

        operation.putOperationData(RestContains.PERMISSION, permission);
        operation.putOperationData(RestContains.DOMAINTYPE, domaintype);

        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Remove user from CIFS share
     * @param storageArrayInfo OceanStor info
     * @param id CIFS share id
     * @param name CIFS share name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void removeUser(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = null;
        if (id == null || id.trim().length() == 0) {
            real_id = getCIFSShareClientIdByName(storageArrayInfo, name);
        } else {
            real_id = id;
        }
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.REMOVE_CIFS_SHARE_USER);
        operation.putOperationData(RestContains.ID, real_id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get CIFS share client id by name
     * @param storageArrayInfo OceanStor info
     * @param name CIFS share client name
     * @return CIFS share client id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String getCIFSShareClientIdByName(StorageArrayInfo storageArrayInfo,
        String name) throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_CIFS_SHARE_USER_ID_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        operation.putOperationData(RestContains.RANGE,
                RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find  object for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one object for Name: " + name
                            + ", please use id instead of name to remove CIFS User!");
        }
        return list.get(0).get(RestContains.ID);
    }
}
