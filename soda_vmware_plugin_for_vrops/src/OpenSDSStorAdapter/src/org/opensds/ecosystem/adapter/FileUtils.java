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

package org.opensds.ecosystem.adapter;

import java.io.File;

import org.apache.log4j.Logger;

public class FileUtils {
    private final Logger logger;

    /**
     * 构造方法
     *
     * @param log 日志
     */
    public FileUtils(Logger log) {
        logger = log;
    }

    /**
     * 获取文件扩展名
     *
     * @param filename 传入文件名
     * @return 返回后缀名
     */
    public String getExtensionName(String filename) {
        if ((filename != null) && (filename.length() > 0)) {
            int dot = filename.lastIndexOf('.');
            if ((dot > -1) && (dot < (filename.length() - 1))) {
                return filename.substring(dot + 1);
            }
        }
        return filename;
    }

    /**
     * 删除文件
     *
     * @param file 需要删除的文件
     */
    public void deleteFile(File file) {
        if (!file.exists()) {
            return;
        }
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (File everyFile : files) {
                deleteFile(everyFile);
            }
        }
        boolean delete = file.delete();
        if (delete) {
            if (logger.isDebugEnabled()) {
                logger.debug(file.getName() + " delete success");
            }
        } else {
            if (logger.isDebugEnabled()) {
                logger.error(file.getName() + " delete failed");
            }
        }
    }
}
