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

public class OperationNames {
	public static class FileSystem {
		public static final OperationNamesEnum CREATE_FILESYSTEM = OperationNamesEnum.CREATE_FILESYSTEM;
		public static final OperationNamesEnum DELETE_FILESYSTEM = OperationNamesEnum.DELETE_FILESYSTEM;
		public static final OperationNamesEnum FIND_FILESYSTEM_BY_ID = OperationNamesEnum.FIND_FILESYSTEM_BY_ID;
		public static final OperationNamesEnum FIND_FILESYSTEM_BY_NAME = OperationNamesEnum.FIND_FILESYSTEM_BY_NAME;
		public static final OperationNamesEnum BATCH_FIND_FILESYSTEM = OperationNamesEnum.BATCH_FIND_FILESYSTEM;
		public static final OperationNamesEnum UPDATE_FILESYSTEM = OperationNamesEnum.UPDATE_FILESYSTEM;
	}
	public static class StoragePool {
		public static final OperationNamesEnum CREATE_STORAGE_POOL = OperationNamesEnum.CREATE_STORAGE_POOL;
		public static final OperationNamesEnum DELETE_STORAGE_POOL = OperationNamesEnum.DELETE_STORAGE_POOL;
		public static final OperationNamesEnum FIND_STORAGE_POOL_BY_ID = OperationNamesEnum.FIND_STORAGE_POOL_BY_ID;
		public static final OperationNamesEnum BATCH_FIND_STORAGE_POOL = OperationNamesEnum.BATCH_FIND_STORAGE_POOL;
		public static final OperationNamesEnum UPDATE_STORAGE_POOL = OperationNamesEnum.UPDATE_STORAGE_POOL;
	}
}
