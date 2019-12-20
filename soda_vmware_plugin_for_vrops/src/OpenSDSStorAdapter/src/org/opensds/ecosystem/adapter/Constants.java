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

public class Constants {
    /**
     * vCenter服务URL地址
     */
    public static final String VCENTER_REST_URL = "vCenterRestURL";

    /**
     * vCenter服务UserName
     */
    public static final String VCENTER_USER_NAME = "vCenterUserName";

    /**
     * vCenter服务Password
     */
    public static final String VCENTER_USER_PWD = "vCenterUserPwd";

    /**
     * 连接阵列请求服务地址
     */
    public static final String REST_URL = "RestURL";

    /**
     * 连接阵列请求用户名
     */
    public static final String USER_NAME = "UserName";

    /**
     * 连接阵列请求密码
     */
    public static final String USER_PWD = "UserPwd";

    /**
     * 证书路径
     */
    public static final String CERTIFICATE_PATH = "CertificatePath";

    /**
     * 阵列服务器IP地址
     */
    public static final String ARRAY_HOST = "ArrayHost";

    /**
     * 阵列服务器设备号
     */
    public static final String ARRAY_DEVICE_SN = "DeviceSn";

    /**
     * 用户临时文件夹
     */
    public static final String TEMP_FILE_PATH = System.getProperty("user.dir");

    /**
     * 文件分离器
     */
    public static final String FILE_SEPARATOR = File.separator;

    /**
     * 存放数据文件的临时文件夹
     */
    public static final String TEMP_PERF_FILE_FOLDER = TEMP_FILE_PATH
            + File.separator + "TempPerf";

    /**
     * 服务器路径分隔符
     */
    public static final String SERVER_SPLITER = ":";

    /**
     * 阵列服务器数据文件目录
     */
    public static final String DATA_PERF_FILE_FOLDER = "/OSM/coffer_data/perf/perf_files/";

    /**
     * Dorado C20阵列服务器数据目录
     */
    public static final String DATA_PERF_FILE_FOLDER_C20 = "/OSM/coffer_data/omm/perf/perf_files/";

    /**
     * 18000系列阵列服务器数据文件目录
     */
    public static final String IS_18000_DATA_PERF_FILE_FOLDER = "/svp_data/PerfFile/";
    /**
     * 18500V1系列阵列服务器数据文件目录
     */
    public static final String IS_18500V1_DATA_PERF_FILE_FOLDER = "/svp_data/PerfFiles/";

    /**
     * Rest服务地址
     */
    public static final String ARRAY_REST_PATH = "/deviceManager/rest";

    /**
     * Rest服务协议头
     */
    public static final String ARRAY_PROTOCOL = "https://";
    /**
     * 拼装链接Vcenter的url
     */
    public static final String VCENTER_URL_END = "/sdk";
    /**
     * 多个管理IP分割符
     */
    public static final String ARRAY_HOST_SPLITER = ";";
    /**
     * 管理端口
     */
    public static final String MANAGEMENT_PORT = "2";
    /**
     * 管理/业务混合口
     */
    public static final String MANAGEMENT_OR_SERVICE_PORT = "5";
    /**
     * SFTP服务端口，默认为22
     */
    public static final int ARRAY_PORT = 22;

    /**
     * Rest服务请求端口
     */
    public static final Integer ARRAY_REST_PORT_V3 = 8088;

    /**
     * 部分老型号阵列Rest服务请求端口
     */
    public static final Integer ARRAY_REST_PORT_V1 = 443;

    /**
     * Rest批量查询分页条数
     */
    public static final int REST_BATCH_PAGESIZE = 100;

    /**
     * 查询第一个数字
     */
    public static final int FIRST_VALUE = 0;

    /**
     * 一分钟
     */
    public static final int ONE_MINUTE = 60;

    /**
     * 一分钟毫秒
     */
    public static final long ONE_MINUTE_MILLISECOND = 60000;

    /**
     * 半小时毫秒
     */
    public static final long HALF_HOUR_MILLISECOND = 1800000;

    /**
     * 文件名最小长度
     */
    public static final int FILE_NAME_MINIMUM_LENGTH = 4;

    /**
     * sectors 转换单位
     */
    public static final int SECTORS_UNIT = 4;

    /**
     * ip最大长度
     */
    public static final int IP_MAXIMUM_LENGTH = 39;

    /**
     * Two
     */
    public static final int DOUBLE = 2;

    /**
     * 百分比转换单位
     * Percent conversion unit
     */
    public static final double PERCENT_CONVERSION_UNIT = 100;

    /**
     * 是否进行域名检查
     */
    public static final boolean NO_CHECK_DOMAIN = false;

    /**
     * 18500v1
     */
    public static final String IS18500V11 = "46";
    /**
     * 远程复制的类型为lun
     */
    public static final String LUN_TYPE = "11";
    /**
     * 远程复制的类型为 文件系统
     */
    public static final String FILE_SYSTEM_TYPE = "40";
    /**
     * 18500v1
     */
    public static final String IS18500V12 = "57";

    /**
     * 18800v11
     */
    public static final String IS18800V11 = "47";

    /**
     * 18800v11
     */
    public static final String IS18800V12 = "56";

    /**
     * 6800v3
     */
    public static final String IS6800V3 = "61";
    /**
     * 18800v11
     */
    public static final String IS18800V13 = "58";

    /**
     * 18500v3
     */
    public static final String IS18500V3 = "72";

    /**
     * 18800v3
     */
    public static final String IS18800V3 = "73";

    /**
     * D5000V6
     */
    public static final String D5000V6 = "811";

    /**
     * D5000V6_N
     */
    public static final String D5000V6_N = "812";

    /**
     * D6000V6
     */
    public static final String D6000V6 = "813";

    /**
     * D6000V6_N
     */
    public static final String D6000V6_N = "814";

    /**
     * D8000V6
     */
    public static final String D8000V6 = "815";

    /**
     * D8000V6_N
     */
    public static final String D8000V6_N = "816";

    /**
     * D18000V6
     */
    public static final String D18000V6 = "817";

    /**
     * D18000V6_N
     */
    public static final String D18000V6_N = "818";

    /**
     * D3000V6
     */
    public static final String D3000V6 = "819";

    /**
     * D5000V6_I
     */
    public static final String D5000V6_I = "821";

    /**
     * D6000V6_I
     */
    public static final String D6000V6_I = "822";

    /**
     * D8000V6_I
     */
    public static final String D8000V6_I = "823";

    /**
     * D18000V6_I
     */
    public static final String D18000V6_I = "824";

    /**
     * Dorado5300_V6
     */
    public static final String DORADO5300_V6 = "825";

    /**
     * Dorado5500_V6
     */
    public static final String DORADO5500_V6 = "826";

    /**
     * Dorado5600_V6
     */
    public static final String DORADO5600_V6 = "827";

    /**
     * Dorado5800_V6
     */
    public static final String DORADO5800_V6 = "828";

    /**
     * Dorado6800_V6
     */
    public static final String DORADO6800_V6 = "829";

    /**
     * Dorado18500_V6
     */
    public static final String DORADO18500_V6 = "830";

    /**
     * Dorado18800_V6
     */
    public static final String DORADO18800_V6 = "831";

    /**
     * Milliseconds to seconds
     */
    public static final long MILLISECODS_TO_SECONDS = 1000L;
    /**
     * 18000系列设备
     */
    public static final int IS_18000_STOR = 18000;
    /**
     * 6800系列设备
     */
    public static final int IS_6800_STOR = 6800;

    /**
     * 非6800和18000系列设备
     */
    public static final int IS_OPENSDS_STOR = 0;

    /**
     * 6800的性能文件路径
     */
    public static final String STOR_6800_FILEPATH = "storage";
    /**
     * 6800的性能文件路径
     */
    public static final String STOR_6800_PERF_FOLDER = "OpenSDSStorAdapter";

    /**
     * windows的C盘
     */
    public static final String WINDOWS_C_DISC = "c:";

    public static final String PRODUCTMODE_V5R7C60 = "120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135";
}
