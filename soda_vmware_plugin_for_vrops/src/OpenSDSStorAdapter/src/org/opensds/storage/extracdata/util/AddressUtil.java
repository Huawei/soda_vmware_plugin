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

import java.math.BigInteger;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.Normalizer;
import java.text.Normalizer.Form;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.opensds.storage.extracdata.constant.LegoNumberConstant;

public final class AddressUtil {
    private static final int VALUE_0X00FFFFFF = 0x00FFFFFF;

    private static final int VALUE_0X0000FFFF = 0x0000FFFF;

    private static final int VALUE_0X000000FF = 0x000000FF;

    private static final String REREX_IPV6_ADDRESS = "^\\s*((([0-9A-Fa-f]{1,4}:){7}(([0-9A-Fa-f]{1,4})|:))" +
            "|(([0-9A-Fa-f]{1,4}:){6}"
            + "(:|((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})"
            + "(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})|(:[0-9A-Fa-f]{1,4})))|(([0-9A-Fa-f]{1,4}:){5}"
            + "((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|"
            + "((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}"
            + "((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|"
            + "((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}"
            + "((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|"
            + "((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}"
            + "((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|"
            + "((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:)(:[0-9A-Fa-f]{1,4}){0,4}"
            + "((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|"
            + "((:[0-9A-Fa-f]{1,4}){1,2})))|(:(:[0-9A-Fa-f]{1,4}){0,5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})"
            + "(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))"
            + "|(((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})))(%.+)?\\s*$";

    private static final BigInteger INVALID = BigInteger.ZERO;

    private AddressUtil() {
    }

    /**
     * @param ip ip
     * @return boolean
     * @throws Exception
     */
    public static boolean checkLocalIP(String ip) throws Exception {
        if ((null == ip) || (ip.length() <= 0)) {
            return false;
        }

        if ("127.0.0.1".equals(ip)) {
            return true;
        }

        if ("localhost".equalsIgnoreCase(ip)) {
            return true;
        }

        InetAddress addrd;

        try {
            addrd = InetAddress.getLocalHost();
        } catch (UnknownHostException ee) {
            throw new Exception(ee);
        }

        String hostName = addrd.getHostName();
        InetAddress[] ipsAddr = null;

        try {
            ipsAddr = InetAddress.getAllByName(hostName);
        } catch (UnknownHostException ex) {
            throw new Exception(ex);
        }

        String[] localServers = new String[ipsAddr.length];

        for (int i = 0; i < ipsAddr.length; i++) {
            if (ipsAddr[i] != null) {
                localServers[i] = ipsAddr[i].getHostAddress();

                if (ip.equals(localServers[i])) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Get local ip
     * @return Local ip
     * @throws Exception
     */
    public static String getLocalIP() throws Exception {
        InetAddress addr;

        try {
            addr = InetAddress.getLocalHost();
        } catch (UnknownHostException e) {
            throw new Exception(e);
        }

        String hostName = addr.getHostName();
        InetAddress[] ipsAddr = null;

        try {
            ipsAddr = InetAddress.getAllByName(hostName);
        } catch (UnknownHostException e) {
            throw new Exception(e);
        }

        if ((null != ipsAddr) && (ipsAddr.length > 0)) {
            return ipsAddr[0].getHostAddress();
        }

        return "";
    }

    /**
     * Ip to long
     * @param ipaddress Ip address
     * @return address to long
     */
    public static long ipToLong(String ipaddress) {
        long[] ip = new long[LegoNumberConstant.FOUR];

        int position1 = ipaddress.indexOf(".");
        int position2 = ipaddress.indexOf(".", position1 + 1);
        int position3 = ipaddress.indexOf(".", position2 + 1);
        ip[0] = Long.parseLong(ipaddress.substring(0, position1));
        ip[1] = Long.parseLong(ipaddress.substring(position1 + 1, position2));
        ip[LegoNumberConstant.TWO] = Long.parseLong(ipaddress.substring(position2 + 1, position3));
        ip[LegoNumberConstant.THREE] = Long.parseLong(ipaddress.substring(position3 + 1));

        return (ip[0] << LegoNumberConstant.TWENTY_FOUR) + (ip[1] << LegoNumberConstant.SIXTEEN)
                + (ip[LegoNumberConstant.TWO] << LegoNumberConstant.EIGHT) + ip[LegoNumberConstant.THREE];
    }

    /**
     * long to ip
     * @param ipaddress Ip address
     * @return long to ip address
     */
    public static String longToIP(long ipaddress) {
        StringBuffer sb = new StringBuffer("");
        sb.append(String.valueOf(ipaddress >>> LegoNumberConstant.TWENTY_FOUR));
        sb.append('.');
        sb.append(String.valueOf((ipaddress & VALUE_0X00FFFFFF) >>> LegoNumberConstant.SIXTEEN));
        sb.append('.');
        sb.append(String.valueOf((ipaddress & VALUE_0X0000FFFF) >>> LegoNumberConstant.EIGHT));
        sb.append('.');
        sb.append(String.valueOf(ipaddress & VALUE_0X000000FF));

        return sb.toString();
    }

    /**
     * check ip address
     * @param ip Ip address
     * @return boolean
     */
    public static boolean ipValid(String ip) {
        if (null == ip) {
            return false;
        }

        String regex = "^([1-9]|[1-9]\\d|1\\d{2}|2[0-1]\\d|22[0-3]).(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])."
                + "(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5]).(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])$";
        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(ip);
        boolean isIp = m.matches();

        if (isIp) {
            String[] iPArray = ip.split("\\.");

            if (iPArray.length != LegoNumberConstant.FOUR) {
                return false;
            }

            int val = Integer.parseInt(iPArray[0]);

            if (val == LegoNumberConstant.ONE_HUNDRED_TWENTY_SEVEN) {
                return false;
            }

            return true;
        }

        return false;
    }

    /**
     * Mask to ip
     * @param mask mask
     * @return ip
     */
    public static String maskToIp(int mask) {
        int x;
        int y;
        String part = "";

        x = mask / LegoNumberConstant.EIGHT;
        y = mask % LegoNumberConstant.EIGHT;

        for (int i = 0; i < x; i++) {
            if (x == LegoNumberConstant.FOUR) {
                part = "255.255.255.255";

                break;
            }

            part = part + "255.";
        }

        int sum = 0;
        int constant = LegoNumberConstant.TWO_HUNDRED_FIFTY_SIX;

        for (int i = 0; i < y; i++) {
            constant = constant >> 1;
            sum = sum + constant;
        }

        /* String temp = ""; */
        StringBuffer buf = new StringBuffer();

        if (x != LegoNumberConstant.FOUR) {
            for (int i = 0; i < (LegoNumberConstant.THREE - x); i++) {
                buf = buf.append(".0");
            }
            part = part + sum + buf.toString();
        }
        return part;
    }

    /**
     * Ipv6 to big Integer
     * @param addr Ip address
     * @return BigInterger
     */
    public static BigInteger ipv6ToBigInteger(String addr) {
        if (addr == null) {
            return INVALID;
        }
        int startIndex = addr.indexOf("::");

        if (startIndex != -1) {
            String firstStr = addr.substring(0, startIndex);
            String secondStr = addr.substring(startIndex + LegoNumberConstant.TWO, addr.length());

            BigInteger first = ipv6ToBigInteger(firstStr);
            int x = countChar(firstStr, ':');
            BigInteger two = ipv6ToBigInteger(secondStr);
            first = first.shiftLeft(LegoNumberConstant.SIXTEEN * (LegoNumberConstant.SEVEN - x)).add(two);

            return first;
        }

        String[] strArr = addr.split(":");

        BigInteger retValue = BigInteger.valueOf(0);
        for (int i = 0; i < strArr.length; i++) {
            if (strArr[i].isEmpty()) {
                strArr[i] = "0";
            }
            BigInteger bi = new BigInteger(strArr[i], LegoNumberConstant.SIXTEEN);
            retValue = retValue.shiftLeft(LegoNumberConstant.SIXTEEN).add(bi);
        }
        return retValue;
    }

    /**
     * BigInteger to ipv6
     * @param ipNumber Ip number
     * @return Ip address
     */
    public static String bigIntegerToipv6(BigInteger ipNumber) {
        String ipString = "";
        BigInteger a = new BigInteger("FFFF", LegoNumberConstant.SIXTEEN);
        for (int i = 0; i < LegoNumberConstant.EIGHT; i++) {
            ipString = ipNumber.and(a).toString(LegoNumberConstant.SIXTEEN) + ":" + ipString;

            ipNumber = ipNumber.shiftRight(LegoNumberConstant.SIXTEEN);
        }
        return ipString.substring(0, ipString.length() - 1).replaceFirst("(^|:)(0+(:|$)){2,8}", "::");
    }

    /**
     * Check ipv6
     * @param ipv6 Ipv6 address
     * @return boolean
     */
    public static boolean isValidIPV6(String ipv6) {
        if (null == ipv6) {
            return false;
        }
        ipv6 = Normalizer.normalize(ipv6, Form.NFKC);
        Pattern pattern = Pattern.compile(REREX_IPV6_ADDRESS);
        Matcher mat = pattern.matcher(ipv6);
        if (mat.matches()) {
            try {
                InetAddress address = InetAddress.getByName(ipv6);
                if (address == null) {
                    return false;
                }
                if (address instanceof Inet4Address && ipv6.equals(address.getHostAddress())) {
                    return false;
                }
                return true;
            } catch (UnknownHostException e) {
                return false;
            } catch (Exception e1) {
                return false;
            }
        } else {
            return false;
        }
    }

    /**
     * Check ipv4
     * @param ip Ipv4 address
     * @return
     */
    public static boolean isValidIPV4(String ip) {
        return ipValid(ip);
    }

    private static int countChar(String str, char reg) {
        char[] ch = str.toCharArray();
        int count = 0;
        for (int i = 0; i < ch.length; ++i) {
            if (ch[i] == reg) {
                ++count;
            }
        }
        return count;
    }
}
