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

package org.opensds.storage.conection.rest.constants;

public enum OperationNamesEnum {
    // FileSystem
    CREATE_FILESYSTEM("create-filesystem"), DELETE_FILESYSTEM("delete-filesystem"), FIND_FILESYSTEM_BY_ID(
            "find-filesystem-by-id"), FIND_FILESYSTEM_BY_NAME("find-filesystem-by-name"), BATCH_FIND_FILESYSTEM(
            "batch-find-filesystem"), UPDATE_FILESYSTEM("update-filesystem"),
    // StoragePool
    CREATE_STORAGE_POOL("create-storage-pool"), DELETE_STORAGE_POOL("delete-storage-pool"), FIND_STORAGE_POOL_BY_ID(
            "find-storage-pool-by-id"), FIND_STORAGE_POOL_BY_NAME("find-storage-pool-by-name"), BATCH_FIND_STORAGE_POOL(
            "batch-find-storage-pool"), UPDATE_STORAGE_POOL("update-storage-pool");
    // NAS SHARE

    // SnapShot

    // NAS Access

    private String operationName;

    OperationNamesEnum(String operationName) {
        this.operationName = operationName;
    }

    /**
     * Get value
     * @return Enum value
     */
    public String getValue() {
        return this.operationName;
    }
}
