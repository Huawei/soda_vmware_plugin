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

package org.opensds.storage.conection.sftp;

import com.jcraft.jsch.SftpProgressMonitor;

public class SftpTransferProgress implements SftpProgressMonitor {
    private long fileSize;

    private long finishSize;

    /**
     * init
     * @param tansferState Transfer
     * @param sourcePath Source path
     * @param destinationPath Destination path
     * @param size Size
     */
    public synchronized void init(int tansferState, String sourcePath, String destinationPath, long size) {
        this.fileSize = size;
    }

    /**
     * Count
     * @param mount mount
     * @return boolean
     */
    public synchronized boolean count(long mount) {
        this.finishSize += mount;
        return true;
    }

    /**
     * end
     */
    public synchronized void end() {
        this.finishSize = this.fileSize;
    }

    public synchronized long getFileSize() {
        return this.fileSize;
    }

    public synchronized long getFinishSize() {
        return this.finishSize;
    }

}
