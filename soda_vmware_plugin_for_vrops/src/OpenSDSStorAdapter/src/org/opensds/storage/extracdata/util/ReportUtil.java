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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;
import org.json.JSONException;
import org.json.JSONObject;

import org.opensds.storage.extracdata.constant.LegoNumberConstant;

public class ReportUtil {
    private static final Logger logger;

    /**
     * Error
     */
    public static final String ERROR = "error";

    /**
     * Error code
     */
    public static final String ERROR_CODE = "errorCode";

    /**
     * Description
     */
    public static final String DESCRIPTION = "description";

    /**
     * Template id
     */
    public static final String TEMPLATE_ID = "templateId";

    /**
     * Error prefix
     */
    public static final String ERROR_PREFIX = "lego.err.";

    /**
     * Device ip
     */
    public static final String DEV_IP = "accessIp";

    /**
     * Status
     */
    public static final String STATUS = "status";

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        logger = context.getLogger(ReportUtil.class.getName());
    }

    /**
     * Get zero clock time of date
     * @param time time
     * @return long
     */
    public static Long getZeroClockTimeOfDay(Long time) {
        Calendar c = Calendar.getInstance();
        c.setTime(new Date(time));
        c.set(Calendar.HOUR_OF_DAY, 0);
        c.set(Calendar.MINUTE, 0);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        Long dayTime = c.getTime().getTime();
        return dayTime;
    }

    /**
     * Get period of time
     * @param time time
     * @return long[]
     */
    public static long[] getPeriodOfTime(int time) {
        long[] returnTime = new long[LegoNumberConstant.TWO];
        Date todayTime = new Date();
        Calendar c = Calendar.getInstance();
        c.setTime(todayTime);
        c.add(Calendar.DAY_OF_YEAR, -time);
        returnTime[0] = c.getTime().getTime();
        returnTime[1] = todayTime.getTime();
        return returnTime;
    }

    /**
     * Minute to millis
     * @param minute Minute
     * @return long
     */
    public static long minuteToMillis(int minute) {
        return (long) minute * LegoNumberConstant.SIX * LegoNumberConstant.TEN * LegoNumberConstant.THOUSAND;
    }

    /**
     * Hour to millis
     * @param hour hour
     * @return long
     */
    public static long hourToMillis(int hour) {
        return (long) hour * LegoNumberConstant.SIX * LegoNumberConstant.TEN * LegoNumberConstant.SIX
                * LegoNumberConstant.TEN * LegoNumberConstant.THOUSAND;
    }

    /**
     * Change to English title
     * @param inStr in String
     * @param local local
     * @return English String
     */
    public static String changeToEnglishTitle(String inStr, Locale local) {
        List<String> words = new ArrayList<String>();
        words.add("of");
        words.add("and");
        words.add("in");
        words.add("on");
        words.add("at");
        words.add("to");
        words.add("a");
        words.add("an");
        words.add("the");
        String resultStr = "";
        String[] sb = inStr.split(" ");
        for (int i = 0; i < sb.length; i++) {
            if (i == 0 || !words.contains(sb[i])) {
                String tmpStr = sb[i].substring(0, 1);
                sb[i] = tmpStr.toUpperCase(local).concat(sb[i].substring(1));
            }
            resultStr = resultStr.concat(sb[i]);
            resultStr = resultStr.concat(" ");
        }
        resultStr = resultStr.substring(0, resultStr.length() - 1);
        return resultStr;
    }

    /**
     * Check date directory
     * @param srcpath Src path
     * @return boolean
     */
    public static boolean validateDir(String srcpath) {
        boolean result1 = null != srcpath && !"".equals(srcpath);
        boolean result2 = null != srcpath && srcpath.indexOf("\"") < 0 && srcpath.indexOf("|") < 0
                && srcpath.indexOf("?") < 0;
        boolean result3 = null != srcpath && srcpath.indexOf("*") < 0 && srcpath.indexOf("<") < 0
                && srcpath.indexOf(">") < 0;
        return !(result1 && result2 && result3);
    }

    /**
     * check operation system
     * @return boolean
     */
    public static boolean isLinux() {
        String n = System.getProperty("os.name");
        if (n == null) {
            return false;
        } else {
            return n.toLowerCase(Locale.US).indexOf("linux") >= 0;
        }
    }

    /**
     * check windows operation system
     * @return boolean
     */
    public static boolean isWindows() {
        String n = System.getProperty("os.name");
        if (n == null) {
            return false;
        } else {
            return n.toLowerCase(Locale.US).indexOf("windows") >= 0;
        }
    }

    /**
     * Get json String value
     * @param obj JsonObject
     * @param defaultVal Default value
     * @param paramName Param name
     * @return value
     */
    public static String getJSONStrVal(JSONObject obj, String defaultVal, String paramName) {
        try {
            return obj.getString(paramName);
        } catch (JSONException e) {
            return defaultVal;
        }
    }

    /**
     * Get Json String value
     * @param obj JsonObject
     * @param paramName Param name
     * @return String
     */
    public static String getJSONStrVal(JSONObject obj, String paramName) {
        return getJSONStrVal(obj, null, paramName);
    }

    /**
     * Get Json int value
     * @param obj JsonObject
     * @param defaultVal Defau
     * @param paramName Param name
     * @return int
     */
    public static int getJSONIntVal(JSONObject obj, int defaultVal, String paramName) {
        try {
            return obj.getInt(paramName);
        } catch (JSONException e) {
            return defaultVal;
        }
    }

    /**
     * Get Json int value
     * @param obj JsonObject
     * @param paramName Param name
     * @return int
     */
    public static int getJSONIntVal(JSONObject obj, String paramName) {
        return getJSONIntVal(obj, LegoNumberConstant.ZERO, paramName);
    }

    /**
     * Get Json Long value
     * @param obj JsonObject
     * @param defaultVal Defau
     * @param paramName Param name
     * @return long
     */
    public static long getJSONLongVal(JSONObject obj, long defaultVal, String paramName) {
        try {
            return obj.getLong(paramName);
        } catch (JSONException e) {
            return defaultVal;
        }
    }

    /**
     * Get Json Long value
     * @param obj JsonObject
     * @param paramName Param name
     * @return long
     */
    public static long getJSONLongVal(JSONObject obj, String paramName) {
        return getJSONLongVal(obj, LegoNumberConstant.ZERO, paramName);
    }

    /**
     * Check resource type
     * @param resourceType Resource type
     * @return boolean
     */
    public static boolean checkResourceType(int resourceType) {
        if (resourceType < LegoNumberConstant.ONE || resourceType > LegoNumberConstant.THIRTY_SIX) {
            return Boolean.FALSE;
        }
        return Boolean.TRUE;
    }

    /**
     * Get Json Boolean value
     * @param obj JsonObject
     * @param paramName Param name
     * @return boolean
     */
    public static boolean getJSONBoolVal(JSONObject obj, String paramName) {
        try {
            return obj.getBoolean(paramName);
        } catch (JSONException e) {
            return Boolean.FALSE;
        }
    }

    /**
     * Put json value
     * @param json JsonObject
     * @param paramName Param name
     */
    public static <T> void putJsonVal(JSONObject json, String paramName, T val) {
        if (json == null || paramName == null) {
            throw new IllegalArgumentException("json || paramName is null");
        }
        try {
            json.put(paramName, val);
        } catch (JSONException e) {
            throw new IllegalArgumentException("json error!", e);
        }
    }

    /**
     * Get json value
     * @param json JsonObject
     * @param paramName Param name
     */
    public static <T> T getJsonVal(JSONObject json, String paramName, T val) {
        if (json == null || paramName == null) {
            throw new IllegalArgumentException("json || paramName is null");
        }
        try {
            T value = (T) json.get(paramName);
            return value;
        } catch (JSONException e) {
            return val;
        }
    }

    /**
     * Check directory
     * @param loc local
     */
    public static void checkDirectory(String loc) {
        if (VerifyUtil.isEmpty(loc)) {
            return;
        }
        File file = new File(loc);
        if (!file.exists()) {
            if (!file.mkdir()) {
                logger.warn("mkdir error");
            }
        }
    }

    /**
     * Parse int
     * @param numStr number String
     * @param defaultVal Default value
     * @return int
     */
    public static int parseInt(String numStr, int defaultVal) {
        int result = defaultVal;
        try {
            result = Integer.valueOf(numStr);
        } catch (NumberFormatException e) {
            return result;
        }
        return result;
    }

    /**
     * Parse int
     * @param numStr number String
     * @return int
     */
    public static int parseInt(String numStr) {
        int result = LegoNumberConstant.ZERO;
        try {
            result = Integer.valueOf(numStr.trim());
        } catch (NumberFormatException e) {
            return result;
        }
        return result;
    }

    /**
     * Parse Long
     * @param numStr number String
     * @param defaultVal Default value
     * @return long
     */
    public static long parseLong(String numStr, long defaultVal) {
        long result = defaultVal;
        try {
            result = Long.valueOf(numStr);
        } catch (NumberFormatException e) {
            return result;
        }
        return result;
    }

    /**
     * Parse Long
     * @param numStr number String
     * @return long
     */
    public static long parseLong(String numStr) {
        long result = LegoNumberConstant.ZERO;
        try {
            result = Long.valueOf(numStr);
        } catch (NumberFormatException e) {
            return result;
        }
        return result;
    }

    /**
     * Copy directory
     * @param sourceDir Source directory
     * @param targetDir Target Directory
     */
    public static void copyDirectiory(String sourceDir, String targetDir) {
        checkDirectory(targetDir);
        File[] file = (new File(sourceDir)).listFiles();
        if (file == null) {
            return;
        }
        for (int i = 0; i < file.length; i++) {
            if (file[i].isFile()) {
                File sourceFile = file[i];
                File targetFile = new File(new File(targetDir).getAbsolutePath() + File.separator + file[i].getName());
                FileUtil.copyFile(sourceFile, targetFile);
            }
            if (file[i].isDirectory()) {
                String dir1 = sourceDir + File.separator + file[i].getName();
                String dir2 = targetDir + File.separator + file[i].getName();
                copyDirectiory(dir1, dir2);
            }
        }
    }

    /**
     * Read file by lines
     * @param fileName File name
     * @return content
     */
    public static String readFileByLines(String fileName) {
        if (fileName == null) {
            return null;
        }
        File file = new File(fileName);
        BufferedReader reader = null;
        StringBuilder result = new StringBuilder();
        InputStreamReader isr = null;
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(file);
            isr = new InputStreamReader(fis, "utf-8");
            reader = new BufferedReader(isr);
            String line = null;
            while ((line = reader.readLine()) != null) {
                result.append(line).append("\r\n");
            }
        } catch (IOException e) {
            logger.error("read file error", e);
        } finally {
            IOUtils.closeQuietly(fis);
            IOUtils.closeQuietly(isr);
            IOUtils.closeQuietly(reader);
        }
        return result.toString();
    }

    /**
     * Delete file
     * @param file file
     */
    public static void deleteFile(File file) {
        if (file.exists()) {
            if (file.isFile()) {
                Boolean result = file.delete();
                if (!result) {
                    logger.error("delete file error");
                }
            } else if (file.isDirectory()) {
                File files[] = file.listFiles();
                for (int i = 0; i < files.length; i++) {
                    deleteFile(files[i]);
                }

                Boolean result = file.delete();
                if (!result) {
                    logger.error("delete file error");
                }
            } else {
                logger.error("file type incorrect");
        }
        } else {
            logger.error("file is not exist");
        }
    }

}
