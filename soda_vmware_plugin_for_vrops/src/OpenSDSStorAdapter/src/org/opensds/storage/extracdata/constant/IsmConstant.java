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

package org.opensds.storage.extracdata.constant;

import org.apache.log4j.Level;

public class IsmConstant {
    /**
     * Alarm attrubute separator
     */
    public static final String ALARM_ATTRIBUTE_SEPEARATOR = "\t";

    /**
     * ---
     */
    public static final String ALARM_PARAMETER_PLACEHOLDER = "#???";

    /**
     * ---
     */
    public static final String ALARM_PARAMETER_SEPARATOR = ",";

    /**
     * Blank content
     */
    public static final String BLANK_CONTENT = "--";

    /**
     * CSV separator
     */
    public static final String CSV_SEPEARATOR = ",";

    /**
     * Data format year secend
     */
    public static final String DATA_FORMAT_YEAR_SECEND = "yyyy-MM-dd hh:mm:ss";

    /**
     * DB device SN property name
     */
    public static final String DB_DEVSN_PROPERTY_NAME = "devSN";

    /**
     * DBC case blank
     */
    public static final String DBC_CASE_BLANK = " ";

    /**
     * Default root name string
     */
    public static final String DEFAULT_ROOT_NAME_STRING = "admin";

    /**
     * ---
     */
    public static final String ELEMENT_SEPARATOR = "@@@@@@";

    /**
     * ---
     */
    public static final String EMAIL_SEPARATOR = ";";

    /**
     * Error code prefix
     */
    public static final String ERROR_CODE_PREFIX = "lego.err.";

    /**
     * Excel seperator
     */
    public static final String EXCEL_SEPEARATOR = "\t";

    /**
     * File separator
     */
    public static final String FILE_SEPARATOR = System.getProperty("file.separator");

    /**
     * ---
     */
    public static final String HYPER_MANAGER_NAME = "HyperManager";

    /**
     * Invalid name string
     */
    public static final String INVALID_NAME_STRING = "--";

    /**
     * ---
     */
    public static final boolean IS_WINDOWS;

    /**
     * ISM menu prefix
     */
    public static final String ISM_MENU_PREFIX = "ism.menu.";

    /**
     * MO off line
     */
    public static final int MO_OFF_LINE = 80;

    /**
     * ---
     */
    public static final String MOBILE_PHONE_SEPARATOR = ";";

    /**
     * New line
     */
    public static final String NEW_LINE = System.getProperty("line.separator");

    /**
     * Operate log level danger
     */
    public static final int OPERATELOG_LEVEL_DANGER = 4;

    /**
     * Operate log level info
     */
    public static final int OPERATELOG_LEVEL_INFO = 1;

    /**
     * Operate log level normal
     */
    public static final int OPERATELOG_LEVEL_NORMAL = 3;

    /**
     * Operate log level warning
     */
    public static final int OPERATELOG_LEVEL_WARNING = 2;

    /**
     * Operate log result fail
     */
    public static final int OPERATELOG_RESULT_FAIL = 0;

    /**
     * Operate log result success
     */
    public static final int OPERATELOG_RESULT_SUCCESS = 1;

    /**
     * ---
     */
    public static final String PERFORMANCE_DATA_SEPARATOR = ",";

    /**
     * replease separator
     */
    public static final String REPLEASE_SEPEARATOR = " ";

    /**
     * ---
     */
    public static final String SEMICOLON = ";";

    /**
     * ---
     */
    public static final String COMMA = ",";

    /**
     * Short cross line
     */
    public static final String SHORT_CROSS_LINE = "-";

    /**
     * Switch product
     */
    public static final String SWITCH_PRODUCT = "Switch";

    /**
     * Temperature threshold
     */
    public static final int TEMPERATURE_THRESHOLD = 50;

    /**
     * Undefine value
     */
    public static final int UNDEFINE_VALUE = Integer.MIN_VALUE;

    /**
     * DASH
     */
    public static final String DASH = "_";
    /**
     * diskDomainId
     */
    public static final String DISK_DOMAIN_ID = "diskDomainId";

    /**
     * LUN id
     */
    public static final String LUN_ID = "lunResId";

    /**
     * Operate log level info
     */
    public static final String HOST_ID = "hostResId";

    /**
     * V100R001C01LNJD01
     */
    public static final String ISM_N_SPECIAL_DEVVERSION = "V100R001C01LNJD01";
    /**
     * SSO_ADMIN
     */
    public static final String USER_TYPE_SSO_ADMIN = "sso_admin";

    /**
     * SSO_READONLY
     */
    public static final String USER_TYPE_SSO_READONLY = "sso_readonly";

    /**
     * admin
     */
    public static final String USER_TYPE_ADMIN = "admin";

    /**
     * publicKey
     */
    public static final String PUBLIC_KEY = "publicKey";

    /**
     * User type user creted
     */
    public static final String USER_TYPE_USER_CREATED = "user_created";

    /**
     * Default template
     */
    public static final String DEFAULT_TEMPLATE_NAME = "Default template";

    /**
     * Customize
     */
    public static final String CUSTOMIZE = "customized";

    /**
     * top
     */
    public static final String TOP = "top";
    /**
     * bottom
     */
    public static final String BOTTOM = "bottom";

    /**
     * Empty String
     */
    public static final String EMPTY_STR = "";

    /**
     * Metric prefix
     */
    public static final String METRIC_PREFIX = "ism.report.metric.name.";

    /**
     * Title sufix
     */
    public static final String TITLE_SUFIX = ".title";

    /**
     * Offline device ip
     */
    public static final String OFFLINE_DEVICE_IP = "1.0.0.1";

    /**
     * deviceName
     */
    public static final String DEVICE_NAME = "deviceName";
    /**
     * resourceid
     */
    public static final String RESOURCE_ID = "resourceId";

    static {
        String os = System.getProperty("os.name");
        IS_WINDOWS = os != null && os.contains("Windows");
    }
}