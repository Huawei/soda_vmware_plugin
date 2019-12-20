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
import org.apache.log4j.Logger;

public class HostService {
    private static final Logger logger = Logger.getLogger(HostService.class);

    HostManager hostManager = new HostManager();

    /**
     * Get host id by host name or host id
     * @param storageArrayInfo OceanStor info
     * @param id Host id
     * @param name Host name
     * @return Host id
     * @throws StorageCommonException
     * @throws RestException
     */
    public String adjustHostId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws StorageCommonException, RestException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            Host host = hostManager.getHostByName(storageArrayInfo, name);
            real_id = host.getReal_id();
        }
        return real_id;
    }

    /**
     * Get host group id by host group name or host group id
     * @param storageArrayInfo OceanStor info
     * @param id Host group id
     * @param name Host group name
     * @return Host group id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustHostGroupId(StorageArrayInfo storageArrayInfo,
        String id, String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            HostGroup hostgroup = hostManager
                    .getHostGroupByName(storageArrayInfo, name);
            real_id = hostgroup.getReal_id();
        }
        return real_id;
    }

    /**
     * Create Host
     * @param storageArrayInfo OceanStor info
     * @param name Host name
     * @param description Host description
     * @param os Host operation system
     * @param ip Host ip address
     * @param location Host location
     * @throws RestException
     */
    public void createHost(StorageArrayInfo storageArrayInfo, String name,
        String description, String os, String ip, String location) throws RestException, StorageCommonException {
        hostManager.createHost(storageArrayInfo, name, description, os, ip, location);
    }

    /**
     * Modify Host
     * @param storageArrayInfo OceanStor info
     * @param currentName Host current name
     * @param new_name Host current name
     * @param description Host description
     * @param os Host operation system
     * @param ip Host ip address
     * @param location Host location
     * @throws RestException
     */
    public void modifyHost(StorageArrayInfo storageArrayInfo, String currentName, String new_name, String description,
        String os, String ip, String location) throws RestException, StorageCommonException {
        String real_id = adjustHostId(storageArrayInfo, null, currentName);
        hostManager.modifyHost(storageArrayInfo, real_id, new_name, description, os, ip, location);
    }

    /**
     * Delete Host
     * @param storageArrayInfo OceanStor info
     * @param name Host name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteHost(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustHostId(storageArrayInfo, null, name);
        hostManager.deleteHostById(storageArrayInfo, real_id);
    }

    /**
     * Get Host count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getHostsTotalCount(StorageArrayInfo storageArrayInfo)
            throws RestException, StorageCommonException {
        String count = hostManager.getHostsTotalCount(storageArrayInfo);
        return count;
    }

    /**
     * Get Hosts By range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<Host>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<Host> getHostsByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws RestException, StorageCommonException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                "The size of the index range must >= 0 and <= " + RestContains.RANGE_MAX +
                ". Please reduce the range and try again.");
        }
        List<Host> hosts = hostManager.getHostByRange(storageArrayInfo,
                begin,
                end);
        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository
                .getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getHostList() == null) {
                    array.setHostList(new Vector<Host>());
                } else {
                    array.getHostList().clear();
                }
                array.getHostList().addAll(hosts);
            }
        }
        return hosts;
    }

    /**
     * Create host group
     * @param storageArrayInfo OceanStor info
     * @param name Host group name
     * @param description Host group description
     * @throws RestException
     */
    public void createHostGroup(StorageArrayInfo storageArrayInfo, String name,
        String description) throws RestException, StorageCommonException {
        hostManager.createHostGroup(storageArrayInfo, name, description);
    }

    /**
     * Delete host group
     * @param storageArrayInfo OceanStor info
     * @param name Host group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteHostGroup(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustHostGroupId(storageArrayInfo, null, name);
        hostManager.deleteHostGroupById(storageArrayInfo, real_id);
    }

    /**
     * Get host group count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getHostGroupsTotalCount(StorageArrayInfo storageArrayInfo)
        throws RestException, StorageCommonException {
        String count = hostManager.getHostGroupsTotalCount(storageArrayInfo);
        return count;
    }

    /**
     * Get host group by range
     * @param storageArrayInfo OceanStor info
     * @param begin Begin index
     * @param end End index
     * @return List<HostGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<HostGroup> getHostGroupsByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        if ((end - begin > RestContains.RANGE_MAX) || (end - begin < 0)) {
            throw new StorageCommonException(
                    "The size of the index range must >= 0 and <= "
                            + RestContains.RANGE_MAX
                            + ". Please reduce the range and try again.");
        }
        List<HostGroup> hostGroups = new Vector<HostGroup>();
        hostGroups = hostManager.getHostGroupByRange(storageArrayInfo,
                begin,
                end);

        // put data in memory for finder
        List<StorageArrayInfo> storageArrayInfos = OpenSDSStorageRepository.getUniqueInstance().getStorageArrayInfos();
        for (StorageArrayInfo array : storageArrayInfos) {
            if (array.getId().equals(storageArrayInfo.getId())) {
                if (array.getHostGroupList() == null) {
                    array.setHostGroupList(new Vector<HostGroup>());
                } else {
                    array.getHostGroupList().clear();
                }
                array.getHostGroupList().addAll(hostGroups);
            }
        }
        return hostGroups;
    }

    /**
     * Create host and host group association
     * @param storageArrayInfo OceanStor info
     * @param hostGroupName Host group name
     * @param hostName Host name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createHostGroupAssociation(StorageArrayInfo storageArrayInfo,
        String hostGroupName, String hostName)throws RestException, StorageCommonException {
        String hostgroup_id = adjustHostGroupId(storageArrayInfo, null, hostGroupName);
        String host_id = adjustHostId(storageArrayInfo, null, hostName);
        hostManager.createHostGroupAssociation(storageArrayInfo, hostgroup_id, host_id);
    }

    /**
     * Delete host and host group association
     * @param storageArrayInfo OceanStor info
     * @param hostGroupName Host group name
     * @param hostName Host name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteHostGroupAssociation(StorageArrayInfo storageArrayInfo,
        String hostGroupName, String hostName) throws RestException, StorageCommonException {
        String hostgroup_id = adjustHostGroupId(storageArrayInfo, null, hostGroupName);
        String host_id = adjustHostId(storageArrayInfo, null, hostName);
        hostManager.deleteHostGroupAssociation(storageArrayInfo, hostgroup_id, host_id);
    }

    /**
     * Get host group by host
     * @param storageArrayInfo OceanStor info
     * @param hostName Host name
     * @return List<HostGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<HostGroup> getHostGroupByHost(StorageArrayInfo storageArrayInfo, String hostName)
        throws RestException, StorageCommonException {
        String host_id = adjustHostId(storageArrayInfo, null, hostName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOST_GROUP_BY_HOST);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, host_id);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_HOST);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<HostGroup> hostGroups = new ArrayList<HostGroup>();
        for (Map<String, String> map : list) {
            HostGroup hostGroup = (HostGroup) ObjectsBuilder.INSTANCE
                    .buildObject(map, HostGroup.class, storageArrayInfo);
            hostGroups.add(hostGroup);
        }
        return hostGroups;
    }
}
