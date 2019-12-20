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

package org.opensds.storage.extracdata.util;

import java.io.Closeable;
import java.io.File;
import java.io.IOException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

public class IOUtils {
    private static final Logger logger;

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        logger = context.getLogger(IOUtils.class.getName());
    }

    /**
     * Close quitely
     * @param closeable Closeable
     */
    public static void closeQuietly(Closeable closeable) {
        try {
            if (closeable != null) {
                closeable.close();
            }
        } catch (IOException ioe) {
            logger.error("close io error.", ioe);
        }
    }

    /**
     * Close quitely
     * @param closeable Closeable
     */
    public static void closeQuietly(Closeable... closeable) {
        if (closeable != null) {
            for (Closeable c : closeable) {
                closeQuietly(c);
            }
        }
    }

    /**
     * deleteFile
     *
     * @param file file
     * @return delete suc or failed
     */
    public static boolean deleteFile(String file) {
        try {
            if (null == file) {
                return false;
            }
            return new File(file).delete();
        } catch (Exception e) {
            logger.error("Error when delete " + file, e);
        }
        return false;
    }
}
