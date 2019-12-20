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

package org.opensds.storage.conection.rest.common;

public enum OperationNamesEnum {
	GET_SYSINFO("get-sysinfo"),
    //FileSystem
	CREATE_FILESYSTEM("create-filesystem"),          
	DELETE_FILESYSTEM("delete-filesystem"),           
	FIND_FILESYSTEM("find-filesystem"),              
	BATCH_FIND_FILESYSTEM("batch-find-filesystem"),  
	UPDATE_FILESYSTEM("update-filesystem"),
	GET_FILESYSTEM_BY_NAME("get-filesystem-by-name"), 
	GET_NFS_SHARE_BY_PATH("get-nfs-share-by-path"), 
	GET_FILESYSTEMS_BY_RANGE("get-filesystems-by-range"),
	GET_FILESYSTEMS_TOTAL_COUNT("get-filesystems-total-count"),
	//StoragePool
	CREATE_STORAGE_POOL("create-storage-pool"),
	DELETE_STORAGE_POOL("delete-storage-pool"),
	FIND_STORAGE_POOL("find-storage-pool"),
	FIND_STORAGE_POOL_BY_NAME("find-storage-pool-by-name"),
	BATCH_FIND_STORAGE_POOL("batch-find-storage-pool"),
	CREATE_LUN("create-lun"),
	MODIFY_LUN("modify-lun"),
	EXPAND_LUN("expand-lun"),
	GET_LUN_BY_NAME("get-lun-by-name"),
	GET_LUNGROUP_BY_NAME("get-lungroup-by-name"),
	DELETE_LUNGROUP_BY_ID("delete-lungroup-by-id"),
	GET_LUN_BY_ID("get-lun-by-id"),
	GET_LUN_BY_LUNGROUP("get-lun-by-lungroup"),
	DELETE_LUN_BY_ID("delete-lun-by-id"),
	GET_LUNS_BY_RANGE("get-luns-by-range"),
	GET_LUNGROUPS_BY_RANGE("get-groups-by-range"),
	GET_LUNGROUPS_BY_LUNID("get-lungroup-by-lunid"),
	GET_LUNGROUPS_BY_MAPPINGVIEW("get-lungroup-by-mappingview"),
	GET_LUN_TOTAL_COUNT("get-lun-total-count"),
	GET_LUN_GROUP_TOTAL_COUNT("get-lun-group-total-count"),
	CREATE_LUN_GROUP("create-lun-group"),
	CREATE_HOST("create-host"),
	MODIFY_HOST("modify-host"),
	CREATE_HOST_GROUP("create-host-group"),
	DELETE_HOST_BY_ID("delete-host-by-id"),
	DELETE_HOST_GROUP_BY_ID("delete-host-group-by-id"),
	GET_HOST_BY_NAME("get-host-by-name"),
	GET_HOST_GROUP_BY_NAME("get-host-group-by-name"),
	GET_HOST_GROUP_BY_MAPPINGVIEW("get-host-group-by-mappingview"),
	GET_HOSTS_BY_RANGE("get-hosts-by-range"),
	GET_HOSTS_BY_HOSTGROUP("get-hosts-by-hostgroup"),
	GET_HOSTS_TOTAL_COUNT("get-hosts-total-count"),
	GET_HOST_GROUPS_BY_RANGE("get-host-groups-by-range"),
	GET_HOST_GROUPS_TOTAL_COUNT("get-host-group-total-count"),
	CREATE_HOST_GROUP_ASSOCIATE("create-host-group-associate"),
	GET_HOST_GROUP_BY_HOST("get-host-group-by-host"),
	DELETE_HOST_GROUP_ASSOCIATE("delete-host-group-associate"),
	CREATE_LUN_GROUP_ASSOCIATE("create-lun-group-associate"),
	DELETE_LUN_GROUP_ASSOCIATE("delete-lun-group-associate"),
	CREATE_MAPPING_VIEW("create-mapping-view"),
	GET_MAPPING_VIEW_BY_NAME("get-mapping-view-by-name"),
	GET_MAPPING_VIEW_BY_ID("get-mapping-view"),
	GET_MAPPING_VIEW_BY_LUNGROUP("get-mapping-view-by-lungroup"),
	DELETE_MAPPING_VIEW("delete-mapping-view"),
	CREATE_MAPPING_VIEW_ASSOCIATE("create-mapping-view-assocaite"),
	DELETE_MAPPING_VIEW_ASSOCIATE("delete-mapping-view-assocaite"),
	UPDATE_STORAGE_POOL("update-storage-pool"),
	//NAS SHARE
	CREATE_NFS_SHARE("create-nfs-share"),
	DELETE_NFS_SHARE_BY_ID("delete-nfs-share-by-id"),
	UPDATE_NFS_SHARE_BY_ID("update-nfs-share-by-id"),
	GET_NFS_SHARE_TOTAL_COUNT("get-nfs-share-total-count"),
	CREATE_NFS_SHARE_CLIENT("create-nfs-share-client"),
	GET_NFS_SHARE_CLIENT_BY_NAME("get-nfs-share-client-by-name"),
	DELETE_NFS_SHARE_CLIENT_BY_ID("delete-nfs-share-client-by-id"),
	GET_NFS_SHARE_BY_RANGE("get-nfs-share-by-range"),
	CREATE_CIFS_SHARE("create-cifs-share"),
	UPDATE_CIFS_SHARE("update-cifs-share"),
	GET_CIFS_SHARE_TOTAL_COUNT("get-cifs-share-total-count"),
	GET_CIFS_SHARE_BY_RANGE("get-cifs-share-by-range"),
	GET_CIFS_SHARE_BY_NAME("get-cifs-share-by-name"),
	ADD_CIFS_SHARE_USER("add-cifs-share-user"),
	GET_CIFS_SHARE_USER_ID_BY_NAME("get-cifs-share-user-id"),
	REMOVE_CIFS_SHARE_USER("remove-cifs-share-user"),
	DELETE_CIFS_SHARE_BY_ID("delete-cifs-share-by-id"),
	
	CREATE_FTP_SHARE("create-ftp-share"),
	UPDATE_FTP_SHARE("update-ftp-share"),
	GET_FTP_SHARE_TOTAL_COUNT("get-ftp-share-total-count"),
	GET_FTP_SHARE_BY_RANGE("get-ftp-share-by-range"),
	DELETE_FTP_SHARE_BY_ID("delete-ftp-share"),
	
	//SnapShot
	UPDATE_LUN_SNAPSHOT("update-lun-snapshot"),
	DELETE_LUN_SNAPSHOT("delete-lun-snapshot"),
	ACTIVE_LUN_SNAPSHOT("active-lun-snapshot"),
	DEACTIVE_LUN_SNAPSHOT("deactive-lun-snapshot"),
	
	ROLLBACK_LUN_SNAPSHOT("rollback-lun-snapshot"),
	CANCEL_ROLLBACK_LUN_SNAPSHOT("cancel-rollback-lun-snapshot"),
	GET_LUN_SNAPSHOT_BY_NAME("get-lun-snapshot-by-name"),
	CREATE_LUN_SNAPSHOT_COPY("create-lun-snapshot-copy"),
	GET_LUN_SNAPSHOT_TOTAL_COUNT("get-lun-snapshot-total-count"),
	GET_LUN_SNAPSHOT_BY_RANGE("get-lun-snapshot-by-range"),
	CREATE_LUN_SNAPSHOT("create-lun-snapshot"),
	
	CREATE_FILESYSTEM_SNAPSHOT("create-filesystem-snapshot"),
	DELETE_FILESYSTEM_SNAPSHOT("delete-filesystem-snapshot"),
	GET_FILESYSTEM_SNAPSHOT_BY_NAME("get-filesystem-snapshot-by-name"),
	UPDATE_FILESYSTEM_SNAPSHOT("update-filesystem-snapshot"),
	GET_FILESYSTEM_SNAPSHOT_COUNT("get-filesystem-snapshot-count"),
	GET_FILESYSTEM_SNAPSHOT_BY_RANGE("get-filesystem-snapshot-by-range"),
	ROLLBACK_FILESYSTEM_SNAPSHOT("rollback-filesystem-snapshot"),
	
	CREATE_LUN_COPY("create-lun-copy"),
	DELETE_LUN_COPY("delete-lun-copy"),
	UPDATE_LUN_COPY("update-lun-copy"),
	GET_LUN_COPY_BY_NAME("get-lun-copy-by-name"),
	GET_LUN_COPY_TOTAL_COUNT("get-lun-copy-total-count"),
	GET_LUN_COPYS_BY_RANGE("get-lun-copys-by-range"),
	START_LUN_COPY("start-lun-copy"),
	STOP_LUN_COPY("stop-lun-copy"),
	SUSPEND_LUN_COPY("suspend-lun-copy"),
	CONTINUE_LUN_COPY("continue-lun-copy"),
	ADD_LUN_COPY_TARGET("add-lun-copy-target"),
	REMOVE_LUN_COPY_TARGET("remove-lun-copy-target"),
	
	CREATE_FC_INITIATOR("create-fc-initiator"),
	DELETE_FC_INITIATOR("delete-fc-initiator"),
	GET_FC_INITIATOR_TOTAL_COUNT("get-fc-initiator-total-count"),
	GET_FC_INITIATOR_BY_ID("get-fc-initiator-by-id"),
	GET_FC_INITIATOR_BY_RANGE("get-fc-initiator-by-range"),
	UPDATE_FC_INITIATOR("update-fc-initiator"),
	REMOVE_FC_INITIATOR_FROM_HOST("remove-fc-initiator-from-host"),
	
	CREATE_ISCSI_INITIATOR("create-iSCSI-initiator"),
	DELETE_ISCSI_INITIATOR("delete-iSCSI-initiator"),
	MODIFY_ISCSI_INITIATOR("modify-iSCSI-initiator"),
	GET_ISCSI_INITIATOR("get-iSCSI-initiator"),
	GET_ISCSI_INITIATOR_TOTAL_COUNT("get-iSCSI-initiator-total-count"),
	GET_ISCSI_INITIATOR_BY_RANGE("get-iSCSI-initiator-by-range"),
	REMOVE_ISCSI_INITIATOR_FROM_HOST("remove-iSCSI-initiator-from-host"),
	
	CEREATE_REMOTE_REPLICATION_PAIR("create-remote-replication-pair"),
	SPLIT_REMOTE_REPLICATION_PAIR("split-remote-replication-pair"),
	GET_REMOTE_REPLICATION_PAIR_BY_GROUPID("get-remote-replication-pair-by-groupid"),
	DELETE_REMOTER_EPLICATION_PAIR("delete-remote-replication-pair"),
	GET_REMOTER_EPLICATION_PAIR_BY_LUNID("get-remote-replication-pair-by-lunid"),
	
	CREATE_REMOTE_REPLICATION_CONSISTENCY_GROUP("create-remote-replication-consistency-group"),
	DELETE_REMOTE_REPLICATION_CONSISTENCY_GROUP("delete-remote-replication-consistency-group"),
	SPLIT_REMOTE_REPLICATION_CONSISTENCY_GROUP("split-remote-replication-consistency-group"),
	SYNC_REMOTE_REPLICATION_CONSISTENCY_GROUP("sync-remote-replication-consistency-group"),
	GET_REMOTE_REPLICATION_CONSISTENCY_GROUP_BY_NAME("get-remote-replication-consistency-group-by-name"),
	ADD_PAIR_TO_REPLICATION_CONSISTENCY_GROUP("add-pair-to-replication-consistency-group"),
	GET_RemoteDevice_BY_NAME("get-Remote-Device-by-name"),


	//portgroup
	CREATE_PORT_GROUP("create-port-group"),
	DELETE_PORT_GROUP("delete-port-group"),
	CREATE_PORT_GROUP_ASSOCIATE("create-port-group-associate"),
	DELETE_PORT_GROUP_ASSOCIATE("delete-port-group-associate"),
	GET_PORT_GROUP_BY_NAME("get-port-group-by-name"),
	GET_PORT_BY_NAME("get-port-by-name");
	//NAS Access

	
	
	
	
	
	private String operationName;
	
	OperationNamesEnum(String operationName){
		this.operationName = operationName;
	}
	
	public String getValue(){
		return this.operationName;
	}
}
