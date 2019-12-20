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

import org.apache.log4j.Level;

public class LogUtil {
    /**
     * Log path
     */
    public static final String LOG_PATH = "/storage/log/vcops/log/adapters/OpenSDSStorAdapter/httpRest.log";

    /**
     * Lay out
     */
    public static final String LAYOUT = "%x|%d{yyyy-MM-dd HH:mm:ss}|%p|%t|[%l]|%m%n";

    /**
     * Log level
     */
    public static final Level LOGLEVEL = Level.INFO;

    /**
     * File size
     */
    public static final String FILESIZE = "25M";

    /**
     * Max file number
     */
    public static final Integer MAXFILENUM = 20;

    /**
     * Encod info
     */
    public static final String ENCODINF = "UTF-8";

    /**
     * Log xml path
     */
    public static final String LOG_XML_PATH = "/usr/lib/vmware-vcops/user/plugins/inbound/" +
            "OpenSDSStorAdapter/conf/log4j2.xml";
}
