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

import org.opensds.storage.conection.rest.exception.RestException;

import java.util.List;

public class MappingViewService {
    private LUNManager lunManager = new LUNManager();

    private HostManager hostManager = new HostManager();

    private MappingViewManager mappingViewManager = new MappingViewManager();

    /**
     * Get mappingview id by mappingview name or id
     * @param storageArrayInfo OceanStor info
     * @param id Mappingview id
     * @param name Mappingview name
     * @return Mappingview id
     * @throws RestException
     * @throws StorageCommonException
     */
    public String adjustId(StorageArrayInfo storageArrayInfo, String id,
        String name) throws RestException, StorageCommonException {
        String real_id = id;
        if (id == null || id.trim().length() == 0) {
            MappingView mv = mappingViewManager
                    .getMappingViewByName(storageArrayInfo, name);
            real_id = mv.getReal_id();
        }
        return real_id;
    }

    /**
     * Create mappingview
     * @param storageArrayInfo OceanStor info
     * @param name Mappingview name
     * @param description Mappingview description
     * @throws RestException
     */
    public void createMappingView(StorageArrayInfo storageArrayInfo,
        String name, String description) throws RestException {
        mappingViewManager.createMappingView(storageArrayInfo, name, description);
    }

    /**
     * Detele mappingview
     * @param storageArrayInfo OceanStor info
     * @param name Mappingview name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteMappingView(StorageArrayInfo storageArrayInfo,
        String name) throws RestException, StorageCommonException {
        String real_id = adjustId(storageArrayInfo, null, name);
        mappingViewManager.deleteMappingViewById(storageArrayInfo, real_id);
    }

    /**
     * Create mappingview associate
     * @param storageArrayInfo OceanStor info
     * @param mappingViewName Mappingview name
     * @param lunGroupName LUN group name
     * @param hostGroupName Host group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void createMappingViewAssociate(StorageArrayInfo storageArrayInfo, String mappingViewName,
        String lunGroupName, String hostGroupName) throws RestException, StorageCommonException {
        LUNGroup lunGroup = lunManager.getLUNGroupByName(storageArrayInfo, lunGroupName);
        HostGroup hostGroup = hostManager.getHostGroupByName(storageArrayInfo, hostGroupName);
        MappingView mappingView = mappingViewManager
                .getMappingViewByName(storageArrayInfo, mappingViewName);

        createAssociate(storageArrayInfo, mappingView, lunGroup, hostGroup);
    }

    private void createAssociate(StorageArrayInfo storageArrayInfo, MappingView mappingView,
        LUNGroup lunGroup, HostGroup hostGroup) throws RestException {
        mappingViewManager.createMappingViewAssociate(storageArrayInfo, mappingView.getReal_id(),
            hostGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
        mappingViewManager.createMappingViewAssociate(storageArrayInfo, mappingView.getReal_id(),
            lunGroup.getReal_id(), RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
    }

    /**
     * Delete mappingview associate
     * @param storageArrayInfo OceanStor info
     * @param mappingViewName Mappingview name
     * @param lunGroupName LUN group name
     * @param hostGroupName Host group name
     * @throws RestException
     * @throws StorageCommonException
     */
    public void deleteMappingViewAssociate(StorageArrayInfo storageArrayInfo, String mappingViewName,
        String lunGroupName, String hostGroupName) throws RestException, StorageCommonException {
        LUNGroup lunGroup = lunManager.getLUNGroupByName(storageArrayInfo, lunGroupName);
        HostGroup hostGroup = hostManager.getHostGroupByName(storageArrayInfo, hostGroupName);
        MappingView mappingView = mappingViewManager
                .getMappingViewByName(storageArrayInfo, mappingViewName);

        deleteAssociate(storageArrayInfo, mappingView, lunGroup, hostGroup);
    }

    private void deleteAssociate(StorageArrayInfo storageArrayInfo,
        MappingView mappingView, LUNGroup lunGroup, HostGroup hostGroup) throws RestException {
        mappingViewManager.deleteMappingViewAssociate(storageArrayInfo,
                mappingView.getReal_id(),
                lunGroup.getReal_id(),
                RestContains.ASSOCIATEOBJTYPE_LUN_GROUP);
        mappingViewManager.deleteMappingViewAssociate(storageArrayInfo,
                mappingView.getReal_id(),
                hostGroup.getReal_id(),
                RestContains.ASSOCIATEOBJTYPE_HOST_GROUP);
    }

    /**
     * Get mappingview by host group
     * @param storageArrayInfo OceanStor info
     * @param groupId Host group id
     * @return List<MappingView>
     * @throws RestException
     * @throws StorageCommonException
     */
    public List<MappingView> getMappingViewByHostGroup(StorageArrayInfo storageArrayInfo, String groupId)
            throws RestException, StorageCommonException {
        return mappingViewManager.getMappingViewByHostGroup(storageArrayInfo, groupId);
    }

    /**
     * Get mappingview by id
     * @param storageArrayInfo OceanStor info
     * @param id Mappingview id
     * @return MappingViewInfo
     * @throws RestException
     * @throws StorageCommonException
     */
    public MappingViewInfo getMappingViewById(StorageArrayInfo storageArrayInfo, String id)
            throws RestException, StorageCommonException {
        return mappingViewManager.getMappingViewById(storageArrayInfo, id);
    }
}
