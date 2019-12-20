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

package org.opensds.storage.vro.plugin.core;

import java.util.List;
import java.util.Vector;

import org.opensds.storage.vro.plugin.model.Configration;
import org.opensds.storage.vro.plugin.model.LUNService;
import org.opensds.storage.vro.plugin.model.FilesystemManager;
import org.opensds.storage.vro.plugin.model.CIFSShareManager;
import org.opensds.storage.vro.plugin.model.FCInitiatorManager;
import org.opensds.storage.vro.plugin.model.FilesystemSnapshotManager;
import org.opensds.storage.vro.plugin.model.FTPShareManager;
import org.opensds.storage.vro.plugin.model.HostService;
import org.opensds.storage.vro.plugin.model.PortGroupManager;
import org.opensds.storage.vro.plugin.model.LUNCopyManager;
import org.opensds.storage.vro.plugin.model.iSCSIManager;
import org.opensds.storage.vro.plugin.model.LUNSnapshotManager;
import org.opensds.storage.vro.plugin.model.MappingViewService;
import org.opensds.storage.vro.plugin.model.NFSShareManager;
import org.opensds.storage.vro.plugin.model.RemoteReplicationPairManager;
import org.opensds.storage.vro.plugin.model.RemoteReplicationConsistencyGroupManager;
import org.opensds.storage.vro.plugin.model.RemoteReplicationService;
import org.opensds.storage.vro.plugin.model.LunAndHostService;
import org.opensds.storage.vro.plugin.model.StorageArrayInfo;
import org.apache.log4j.Logger;

import org.opensds.storage.vro.OpenSDSStorageRepository;

import ch.dunes.vso.sdk.api.HasChildrenResult;
import ch.dunes.vso.sdk.api.IPluginFactory;
import ch.dunes.vso.sdk.api.IPluginNotificationHandler;
import ch.dunes.vso.sdk.api.PluginExecutionException;
import ch.dunes.vso.sdk.api.QueryResult;

public class OpenSDSStorageFactory implements IPluginFactory {
    private static final Logger log = Logger
            .getLogger(OpenSDSStorageFactory.class);
    
    private Configration conf = new Configration("conf", "conf");
    
    private LUNService lunService = new LUNService();
    
    private FilesystemManager filesystemManager = new FilesystemManager();
    
    private CIFSShareManager cIFSShareManager = new CIFSShareManager();
    
    private FCInitiatorManager fcInitiatorManager = new FCInitiatorManager();
    
    private FilesystemSnapshotManager filesystemSnapshotManager = new FilesystemSnapshotManager();
    
    private FTPShareManager fTPShareManager = new FTPShareManager();
    
    private HostService hostService = new HostService();
    
    private iSCSIManager iSCSIManager = new iSCSIManager();

    private PortGroupManager portGroupManager = new PortGroupManager();
    
    private LUNCopyManager lUNCopyManager = new LUNCopyManager();
    
    private LUNSnapshotManager lUNSnapshotManager = new LUNSnapshotManager();
    
    private MappingViewService mappingViewService = new MappingViewService();
    
    private NFSShareManager nFSShareManager = new NFSShareManager();
    
    private RemoteReplicationPairManager remoteReplicationPairManager = new RemoteReplicationPairManager();
    
    private RemoteReplicationConsistencyGroupManager remoteReplicationConsistencyGroupManager =
            new RemoteReplicationConsistencyGroupManager();
    
    private RemoteReplicationService remoteReplicationService = new RemoteReplicationService();
    
    private LunAndHostService lunAndHostService = new LunAndHostService();
    
    public OpenSDSStorageFactory(IPluginNotificationHandler notificationHandler) {
        super();
        new OpenSDSStorageEventListener(
                OpenSDSStorageRepository.getUniqueInstance(),
                notificationHandler);
    }
    
    /**
     * Process a command directly transfered from the GUI.
     * 
     * @param cmd is totally plugin specific
     */
    public void executePluginCommand(String cmd) throws PluginExecutionException {
        // No command supported right now.
    }
    
    /**
     * Find an item by its ID (for a given type)
     * 
     * @param type is the type (as defined in the Finders part of the vso.xml file
     * @param id the unique id (for the given type) of the element to get
     * @return The element if found (null if asked for the OpenSDS)
     * @throws IndexOutOfBoundsException if the Type of the element is unknown.
     */
    public Object find(String type, String id) {
        if (log.isDebugEnabled()) {
            log.debug("find: " + type + ", " + id );
        }
        if (type.equals("LUN")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getLUNById(id);
        }
        if (type.equals("Host")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getHostById(id);
        }
        if (type.equals("HostGroup")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getHostGroupById(id); 
        }
        if (type.equals("LUNSnapshot")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getLUNSnapshotById(id);
        }
        if (type.equals("FilesystemSnapshot")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getFilesystemSnapshotById(id);
        }
        if (type.equals("LUNCopy")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getLUNCopyById(id);
        }
        if (type.equals("Filesystem")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getFilesystemById(id);
        }
        if (type.equals("NFSShare")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getNFSShareById(id);
        }
        if (type.equals("CIFSShare")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getCIFSShareById(id);
        }
        if (type.equals("FCInitiator")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getFCInitiatorById(id);
        }
        if (type.equals("iSCSIInitiator")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getiSCSIInitiatorById(id);
        }
        if (type.equals("LUNGroup")) {
            StorageArrayInfo storageArrayInfo = OpenSDSStorageRepository
                    .getUniqueInstance().getStorageArrayInfoById(id);
            return storageArrayInfo.getLUNGroupById(id);
        }
        if (type.equals("LUNService")) {
            return lunService;
        }
        if (type.equals("FilesystemManager")) {
            return filesystemManager;
        }
        if (type.equals("CIFSShareManager")) {
            return cIFSShareManager;
        }
        if (type.equals("FCInitiatorManager")) {
            return fcInitiatorManager;
        }
        if (type.equals("FilesystemSnapshotManager")) {
            return filesystemSnapshotManager;
        }
        if (type.equals("FTPShareManager")) {
            return fTPShareManager;
        }
        if (type.equals("HostService")) {
            return hostService;
        }
        if (type.equals("iSCSIManager")) {
            return iSCSIManager;
        }
        if (type.equals("PortGroupManager")) {
            return portGroupManager;
        }
        if (type.equals("LUNCopyManager")) {
            return lUNCopyManager;
        }
        if (type.equals("LUNSnapshotManager")) {
            return lUNSnapshotManager;
        }
        if (type.equals("MappingViewService")) {
            return mappingViewService;
        }
        if (type.equals("NFSShareManager")) {
            return nFSShareManager;
        }
        if (type.equals("StorageArrayInfo")) {
            return OpenSDSStorageRepository.getUniqueInstance()
                    .getStorageArrayInfoById(id);
        }
        if (type.equals("Conf")) {
            return conf; // No object for OpenSDS defined now
        }
        if (type.equals("RemoteReplicationPairManager")) {
            return remoteReplicationPairManager;
        }
        if (type.equals("RemoteReplicationConsistencyGroupManager")) {
            return remoteReplicationConsistencyGroupManager;
        }
        if (type.equals("RemoteReplicationService")) {
            return remoteReplicationService;
        }
        if (type.equals("LunAndHostService")) {
            return lunAndHostService;
        }
        log.error("Type " + type + " + unknown for OpenSDS Storage Plugin");
        return new Object();
    }
    
    /**
     * Get all item of a given type (Finder type).
     * 
     * @param type is the type of the elements to return
     * @param query is the plugin specific query. Could be ignored...
     * @return A QueryResult contains the resulting planet.
     * @throws IndexOutOfBoundsException if the type is not a valid type
     */
    public QueryResult findAll(String type, String query) {
        if (log.isDebugEnabled()) {
            log.debug("findAll: " + type + ", " + query);
        }
        List list = new Vector(); // The list can contains any element from the plugin
        if (type.equals("StorageArrayInfo")) {
            list = new Vector();
            list.addAll(OpenSDSStorageRepository.getUniqueInstance()
                    .getStorageArrayInfos());
        } else if (type.equals("OpenSDS")) {
            list = new Vector();
        }  else {
            throw new IndexOutOfBoundsException(
                    "Type " + type + " unknown for plugin OpenSDS Storage");
        }
        return new QueryResult(list);
    }
    
    /**
     * Find a list of elements related to another one (the parent).
     * 
     * @param parentType is the Finder type (vso.xml) of the "parent" element
     * @param parentId is the ID of the "parent" element
     * @param relationName is the name of the relation (vso.xml) linking the parent with
     * the elements you want to return. Relation names are define in
     * the vso.xml for each finders.
     * @return The related element through the relation name.
     * @throws IndexOutOfBoundsException if the relationName is not valid for a given Finder type.
     * Null if the "parent" provided is not intended to have
     * children. (prevent crash, as we provide unknown for
     * hasChildrenInRelation
     */
    public List findRelation(String parentType, String parentId,
        String relationName) {
        if (log.isDebugEnabled()) {
            log.debug("findRelation: " + parentType + ", " + parentId + ", "
                    + relationName);
        }
        if (parentId == null) {
            if (relationName.equals("StorageArrayInfos")) {
                List<StorageArrayInfo> list = new Vector<StorageArrayInfo>();
                list.addAll(OpenSDSStorageRepository.getUniqueInstance()
                        .getStorageArrayInfos());
                return list;
            } else {
                return new Vector();
            }
        }
        return new Vector();
    }
    
    /**
     * This method allow the UI (or Web Service clients) to know quickly if the
     * "parent" has children for the given relation. In this sample, we don't
     * check and juste return unknown.
     * 
     * @param parentType is the Finder type (vso.xml) of the "parent" element
     * @param parentId is the ID of the "parent" element
     * @param relationName is the name of the relation (vso.xml) for which you want to know the children lists.
     * @return The related element through the relation name.
     */
    public HasChildrenResult hasChildrenInRelation(String parentType,
        String parentId, String relationName) {
        return HasChildrenResult.Unknown; // don't bother with this for now.
    }
    
    /**
     * Invalidate a defined Finder. Not used is this sample
     */
    public void invalidate(String type, String id) {
        // We never need to invalidate, as we don't have cache
    }
    
    /**
     * Invalidate all finders Not used in this sample
     */
    public void invalidateAll() {
        // We never need to invalidate, as we don't have cache
    }
    
}
