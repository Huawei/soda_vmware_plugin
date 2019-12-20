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

public class HostManager {
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
        String description, String os, String ip, String location) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_HOST);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.OS, os);
        operation.putOperationData(RestContains.IP, ip);
        operation.putOperationData(RestContains.LOCATION, location);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Modify Host
     * @param storageArrayInfo OceanStor info
     * @param id Host id
     * @param name Host current name
     * @param description Host description
     * @param os Host operation system
     * @param ip Host ip address
     * @param location Host location
     * @throws RestException
     */
    public void modifyHost(StorageArrayInfo storageArrayInfo, String id,
        String name, String description, String os, String ip, String location) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.MODIFY_HOST);
        operation.putOperationData(RestContains.ID, id);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        operation.putOperationData(RestContains.OS, os);
        operation.putOperationData(RestContains.IP, ip);
        operation.putOperationData(RestContains.LOCATION, location);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get Host
     * @param storageArrayInfo OceanStor info
     * @param name Host name
     * @return Host
     * @throws StorageCommonException
     * @throws RestException
     */
    public Host getHostByName(StorageArrayInfo storageArrayInfo, String name)
        throws StorageCommonException, RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOST_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find Host for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one Host for Name: " + name);
        }
        Host host = (Host) ObjectsBuilder.INSTANCE.buildObject(list.get(0), Host.class, storageArrayInfo);
        return host;
    }

    /**
     * get host by host group
     * @param storageArrayInfo OceanStor info
     * @param hostGroupId host group id
     * @return List<Host>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<Host> getHostByHostGroup(StorageArrayInfo storageArrayInfo, String hostGroupId)
        throws RestException, StorageCommonException {
        List<Host> result = new ArrayList<Host>();
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOSTS_BY_HOSTGROUP);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        for (Map<String, String> map : list) {
            Host host = (Host) ObjectsBuilder.INSTANCE.buildObject(map, Host.class, storageArrayInfo);
            result.add(host);
        }
        return result;
    }

    /**
     * Delete Host
     * @param storageArrayInfo OceanStor info
     * @param id Host id
     * @throws RestException
     */
    public void deleteHostById(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_HOST_BY_ID);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get Host count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getHostsTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOSTS_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        return list.get(0).get(RestContains.COUNT);
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
    public List<Host> getHostByRange(StorageArrayInfo storageArrayInfo,
        int begin, int end) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOSTS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<Host> hosts = new ArrayList<Host>();
        for (Map<String, String> map : list) {
            Host host = (Host) ObjectsBuilder.INSTANCE.buildObject(map,
                    Host.class,
                    storageArrayInfo);
            hosts.add(host);
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
        String description) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_HOST_GROUP);
        operation.putOperationData(RestContains.NAME, name);
        operation.putOperationData(RestContains.DESCRIPTION, description);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get host group by name
     * @param storageArrayInfo OceanStor info
     * @param name Host group name
     * @return HostGroup
     * @throws RestException
     * @throws StorageCommonException
     */
    public HostGroup getHostGroupByName(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOST_GROUP_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY,
                RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException(
                    "can not find Host Group for Name: " + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException(
                    "there is not only one HostGroup for Name: " + name);
        }
        HostGroup hostgroup = (HostGroup) ObjectsBuilder.INSTANCE.buildObject(list.get(0), HostGroup.class,
            storageArrayInfo);
        return hostgroup;
    }

    /**
     * get host group by MappingView
     * @param storageArrayInfo OceanStor info
     * @param mappingViewId MappingView id
     * @return List<HostGroup>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<HostGroup> getHostGroupByMappingView(StorageArrayInfo storageArrayInfo, String mappingViewId)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOST_GROUP_BY_MAPPINGVIEW);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_MAPPING_VIEW);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, mappingViewId);
        operation.putOperationData(RestContains.RANGE, RestContains.RANGE_DEFAULT);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        List<HostGroup> result = new ArrayList<HostGroup>();
        for (Map<String, String> map : list) {
            HostGroup host = (HostGroup) ObjectsBuilder.INSTANCE.buildObject(map, HostGroup.class, storageArrayInfo);
            result.add(host);
        }
        return result;
    }

    /**
     * Delete Host group
     * @param storageArrayInfo OceanStor info
     * @param id Host group id
     * @throws RestException
     */
    public void deleteHostGroupById(StorageArrayInfo storageArrayInfo, String id) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_HOST_GROUP_BY_ID);
        operation.putOperationData(RestContains.ID, id);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Get host group count
     * @param storageArrayInfo OceanStor info
     * @return count
     * @throws RestException
     */
    public String getHostGroupsTotalCount(StorageArrayInfo storageArrayInfo) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.GET_HOST_GROUPS_TOTAL_COUNT);
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        return list.get(0).get(RestContains.COUNT);
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
    public List<HostGroup> getHostGroupByRange(StorageArrayInfo storageArrayInfo, int begin, int end)
        throws RestException, StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_HOST_GROUPS_BY_RANGE);
        operation.putOperationData(RestContains.RANGE,
                "[" + begin + "-" + end + "]");
        List<Map<String, String>> list = OperationUtil
                .performAction(storageArrayInfo, operation);
        List<HostGroup> hostgroups = new ArrayList<HostGroup>();
        for (Map<String, String> map : list) {
            HostGroup hostgroup = (HostGroup) ObjectsBuilder.INSTANCE
                    .buildObject(map, HostGroup.class, storageArrayInfo);
            hostgroups.add(hostgroup);
        }
        return hostgroups;
    }

    /**
     * Create host group and host association
     * @param storageArrayInfo OceanStor info
     * @param hostGroupId Host group id
     * @param hostId Host id
     * @throws RestException
     */
    public void createHostGroupAssociation(StorageArrayInfo storageArrayInfo,
        String hostGroupId, String hostId) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.CREATE_HOST_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, hostGroupId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, hostId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE,
                RestContains.ASSOCIATEOBJTYPE_HOST);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete host group and host association
     * @param storageArrayInfo OceanStor info
     * @param hostGroupId Host group id
     * @param hostId Host id
     * @throws RestException
     */
    public void deleteHostGroupAssociation(StorageArrayInfo storageArrayInfo,
        String hostGroupId, String hostId) throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(
                OperationNamesEnum.DELETE_HOST_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, hostGroupId);
        operation.putOperationData(RestContains.ASSOCIATEOBJID, hostId);
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE,
                RestContains.ASSOCIATEOBJTYPE_HOST);
        OperationUtil.performAction(storageArrayInfo, operation);
    }
}
