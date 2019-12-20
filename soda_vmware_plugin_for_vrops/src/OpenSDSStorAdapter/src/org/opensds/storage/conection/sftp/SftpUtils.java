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

import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class SftpUtils {
    private static Map<String, Set<String>> unReachableIps = new ConcurrentHashMap<String, Set<String>>();

    private static Map<String, Long> lastCheckTime = new ConcurrentHashMap<String, Long>();

    /**
     * Sftp adapter
     * @param ip Ip address
     * @param uname User name
     * @param upass password
     * @param port port
     * @return SftpAccount
     */
    public static SftpAccount sftpAdapter(String ip, String uname, String upass, int port) {
        SftpAccount account = null;
        account = new SftpAccount(ip, uname, upass, port);
        return account;
    }

    public static Map<String, Set<String>> getUnReachableIps() {
        return unReachableIps;
    }

    public static void setUnReachableIps(Map<String, Set<String>> unReachableIps) {
        SftpUtils.unReachableIps = unReachableIps;
    }

    /**
     * Get last check time
     * @return Map<String, Long>
     */
    public static Map<String, Long> getLastCheckTime() {
        return lastCheckTime;
    }

    public static void setLastCheckTime(Map<String, Long> lastCheckTime) {
        SftpUtils.lastCheckTime = lastCheckTime;
    }

    public static boolean isIpUnreachAble(String ip, String deviceSn) {
        if (ip == null || "".equals(ip.trim())) {
            return Boolean.TRUE;
        }
        Set<String> unReacheabeIps = getUnReachableIps().get(deviceSn);
        return unReacheabeIps != null && unReacheabeIps.contains(ip);
    }

}
