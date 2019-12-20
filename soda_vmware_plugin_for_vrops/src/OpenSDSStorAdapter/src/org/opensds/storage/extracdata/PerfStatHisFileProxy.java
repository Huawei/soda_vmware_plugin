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

package org.opensds.storage.extracdata;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import org.opensds.storage.extracdata.util.GFCommon;
import org.opensds.storage.extracdata.util.TarUtils;

public class PerfStatHisFileProxy {
    private static final int INFO_HEAD_LENGTH = 337;

    private static final int CHECK_CODE_LENGTH = 32;

    private static final int VERSION_LENGTH = 4;

    private static final int DEVICE_SN_LENGTH = 256;

    private static final int DEVICE_NAME_LENGTH = 41;

    private static final int DATA_LENGTH_LENGTH = 4;

    private static final String DATA_TLV_MAP_STRING = "\"Map\":{";

    private static final String DATA_TLV_OBJ_SPLIT = "]},";

    private static final String DATA_ATTR_OBJ_SPLIT = "\"attr\":{";

    private static final long ONE_DAY_TIME_MILLIS = 86400000;

    private Logger logger;

    /**
     * Query perfstat History file info by compress
     * @param fileName File name
     * @param logger logger
     * @return PerfStatHisFileInfo
     * @throws Exception
     */
    public PerfStatHisFileInfo queryPerfStatHisFileInfoByCompress(String fileName, Logger logger) throws Exception {
        this.logger = logger;
        String filename = decompress(fileName);
        return queryPerfStatHisFileInfo(filename);
    }

    /**
     * Query perfstat History file info
     * @param fileName File name
     * @return PerfStatHisFileInfo
     * @throws Exception
     */
    public PerfStatHisFileInfo queryPerfStatHisFileInfo(String fileName) throws Exception {
        PerfStatHisFileInfo perfStatHisFileInfo = new PerfStatHisFileInfo();
        File file = new File(fileName);
        perfStatHisFileInfo.setFileName(file.getAbsolutePath());
        perfStatHisFileInfo.setSize(file.length());
        RandomAccessFile raf = null;
        try {
            raf = open(fileName);
            byte[] headerBuf = new byte[INFO_HEAD_LENGTH];
            raf.readFully(headerBuf);
            int dataLength = parseFileBaseInfo(perfStatHisFileInfo, headerBuf);
            boolean continueRead = Boolean.TRUE;
            while (continueRead && raf.getFilePointer() < raf.length()) {
                continueRead = queryPerfStatHisFileInfo(perfStatHisFileInfo, raf, dataLength);
            }
            return perfStatHisFileInfo;
        } catch (IOException e) {
            logger.error("read data error", e);
            throw new Exception("HIS_FILE_PARSE_ERROR", e);
        } catch (Exception e) {
            logger.error("performance history file error :" + e.getMessage(), e);
            throw new Exception("HIS_FILE_PARSE_ERROR", e);
        } finally {
            close(raf);
        }
    }

    private RandomAccessFile open(String fileName) throws FileNotFoundException {
        RandomAccessFile randomAccessFile = new RandomAccessFile(fileName, "rw");
        return randomAccessFile;
    }

    private void close(RandomAccessFile randomAccessFile) {
        if (randomAccessFile != null) {
            try {
                randomAccessFile.close();
            } catch (IOException e) {
                logger.error("close performance history file error", e);
            }
        }
    }

    private String perfStatHisFileTlvHeaderParser(RandomAccessFile raf) throws IOException, JSONException {
        int skipLen = raf.skipBytes(4);

        if (skipLen != 4) {
            if (logger.isInfoEnabled()) {
                logger.info("parse file fail,skip=4,skipLen=" + skipLen);
            }
            return null;
        }

        byte[] tlvLenBuf = new byte[4];
        raf.readFully(tlvLenBuf);

        int tlvLength = GFCommon.bytes2int(tlvLenBuf, false);
        if (tlvLength <= 8) {
            if (logger.isDebugEnabled()) {
                logger.debug("parse file fail, zero byte left.fileLen:" + raf.length() + " current tlv length:"
                        + tlvLength + " current file pointer:" + raf.getFilePointer());
            }
            return null;
        }

        if (tlvLength > 1024 * 1024 * 32) {
            logger.error("The file is exceed file lenth:" + tlvLength);
            return null;
        }

        String json = null;
        try {
            byte[] jsonByteBuf = new byte[tlvLength - 8];
            raf.readFully(jsonByteBuf);
            json = new String(jsonByteBuf, "UTF-8").trim();
        } catch (Exception e) {
            logger.error("Failed to process file.", e);
        }

        return json;
    }

    private void perfStatHisFileTvlBodyParser(PerfStatHisFileInfo perfStatHisFileInfo, RandomAccessFile raf,
        JSONObject jsonObject, List<String> objTypes, int dataLength) throws IOException, JSONException {
        JSONObject objectMap = jsonObject.getJSONObject("Map");

        Map<Integer, List<String>> objTypeToNamesTmp = new HashMap<Integer, List<String>>();
        Map<Integer, List<String>> objTypeToIdsTmp = new HashMap<Integer, List<String>>();
        Map<Integer, List<Integer>> objTypeToDataTypesTmp = new HashMap<Integer, List<Integer>>();
        List<Integer> objectTypesTmp = new LinkedList<Integer>();

        int periodDataLength = 0;
        for (String objectTypeString : objTypes) {
            Integer objectType = 0;
            try {
                objectType = Integer.parseInt(objectTypeString);
            } catch (Exception e) {
                logger.error("conversion failed.", e);
            }

            JSONObject objectTypeMap = objectMap.getJSONObject(objectTypeString);
            List<String> resIds = getResIds(objectTypeMap);
            List<String> names = getNames(objectTypeMap);
            List<Integer> dataTypes = getDataTypes(objectTypeMap);
            objTypeToIdsTmp.put(objectType, resIds);
            objTypeToNamesTmp.put(objectType, names);
            objTypeToDataTypesTmp.put(objectType, dataTypes);
            objectTypesTmp.add(objectType);
            periodDataLength += resIds.size() * dataTypes.size() * dataLength;
        }

        mergeObjectTypeToMapping(perfStatHisFileInfo.getObjectTypeToIdsMapping(), objTypeToIdsTmp);
        mergeObjectTypeToMapping(perfStatHisFileInfo.getObjectTypeToNamesMapping(), objTypeToNamesTmp);
        mergeObjectTypeToDataTypesMapping(perfStatHisFileInfo.getObjectTypeToDataTypeMapping(), objTypeToDataTypesTmp);
        mergeObjectTypes(perfStatHisFileInfo.getObjectTypes(), objectTypesTmp);
        byte[] periodDataBuf = new byte[periodDataLength];

        String dataStart = jsonObject.getString("StartTime").trim();
        long dataStartTime = Long.parseLong(dataStart) * 1000;
        String dataEnd = jsonObject.getString("EndTime");
        long dataEndTime = Long.parseLong(dataEnd.trim()) * 1000;
        String peri = jsonObject.getString("Archive").trim();
        int period = Integer.valueOf(peri) * 1000;
        long currPeriodDataTime = dataStartTime;
        int offset = 0;
        while (currPeriodDataTime < dataEndTime) {
            raf.readFully(periodDataBuf);
            offset = 0;

            for (Integer objType : objectTypesTmp) {
                Map<Integer, List<PerfStatHisObject>> dataMap = perfStatHisFileInfo.getObjectTypeToDatasMapping();
                List<PerfStatHisObject> dataList = dataMap.get(objType);
                if (dataList == null) {
                    dataList = new ArrayList<PerfStatHisObject>();
                    dataMap.put(objType, dataList);
                }
                List<String> resIds = objTypeToIdsTmp.get(objType);
                List<Integer> dataTypeEnum = objTypeToDataTypesTmp.get(objType);
                for (String resId : resIds) {
                    PerfStatHisObject perfStatHisObject = new PerfStatHisObject();
                    perfStatHisObject.setPeriod(period);
                    perfStatHisObject.setObjectType(objType);
                    perfStatHisObject.setDataTypes(dataTypeEnum);
                    perfStatHisObject.setResId(resId);
                    perfStatHisObject.setStartTime(currPeriodDataTime);
                    getDataSource(offset, periodDataBuf, dataLength, perfStatHisObject);
                    dataList.add(perfStatHisObject);

                    offset += dataLength * dataTypeEnum.size();
                }
            }
            currPeriodDataTime = currPeriodDataTime + period;
        }
    }

    private List<Integer> getDataTypes(JSONObject objectTypeMap) throws JSONException {
        JSONArray dataTypesJSONArray = objectTypeMap.getJSONArray("DataTypes");
        List<Integer> dataTypes = new LinkedList<Integer>();
        for (int j = 0; j < dataTypesJSONArray.length(); j++) {
            dataTypes.add(dataTypesJSONArray.getInt(j));
        }
        return dataTypes;
    }

    private boolean queryPerfStatHisFileInfo(PerfStatHisFileInfo perfStatHisFileInfo, RandomAccessFile raf,
        int dataLength) throws Exception {
        try {
            if (logger.isInfoEnabled()) {
                logger.info("config beg:" + raf.getFilePointer());
            }
            String jsonString = perfStatHisFileTlvHeaderParser(raf);
            if (logger.isInfoEnabled()) {
                logger.info(jsonString);
                logger.info("config end:" + raf.getFilePointer());
            }
            if (null == jsonString) {
                if (logger.isDebugEnabled()) {
                    logger.debug("read file fail. JSON STRING IS NULL,CompressedFileName:"
                            + perfStatHisFileInfo.getCompressedFileName());
                }
                return false;
            }
            int mapStart = jsonString.indexOf(DATA_TLV_MAP_STRING) + DATA_TLV_MAP_STRING.length();
            int mapEnd = jsonString.indexOf(DATA_ATTR_OBJ_SPLIT);
            String[] stringObjTypes = null;
            if (-1 != mapEnd) {
                stringObjTypes = jsonString.substring(mapStart, mapEnd).split(DATA_TLV_OBJ_SPLIT);
            } else {
                stringObjTypes = jsonString.substring(mapStart).split(DATA_TLV_OBJ_SPLIT);
            }

            List<String> objTypes = new ArrayList<String>();
            addDatatypes(stringObjTypes, objTypes);

            JSONObject jsonObject = new JSONObject(jsonString);
            String dataStart = jsonObject.getString("StartTime");
            long dataStartTime = Long.parseLong(dataStart.trim()) * 1000;
            String dataEnd = jsonObject.getString("EndTime");
            long dataEndTime = Long.parseLong(dataEnd.trim()) * 1000;
            String controller = jsonObject.getString("CtrlID").trim();
            int controllerId = Integer.valueOf(controller);
            String peri = jsonObject.getString("Archive").trim();
            int period = Integer.valueOf(peri) * 1000;

            long[] tlvInfo = {dataStartTime, dataEndTime, controllerId};
            perfStatHisFileInfo.getTlvInfos().add(tlvInfo);
            perfStatHisFileInfo.setPeriod(period);

            long minStartTime = perfStatHisFileInfo.getStartTime();
            long maxEndTime = perfStatHisFileInfo.getEndTime();
            minStartTime = getMinStartTime(minStartTime, dataStartTime);
            maxEndTime = getMaxEndTime(maxEndTime, dataEndTime);
            if (maxEndTime - minStartTime > ONE_DAY_TIME_MILLIS) {
                logger.warn("File time too looooooooooooooong, file name: " + perfStatHisFileInfo.getFileName()
                        + ", start time: " + minStartTime + ", end time: " + maxEndTime);
                return Boolean.FALSE;
            }
            perfStatHisFileTvlBodyParser(perfStatHisFileInfo, raf, jsonObject, objTypes, dataLength);
            perfStatHisFileInfo.setStartTime(minStartTime);

            perfStatHisFileInfo.setEndTime(maxEndTime - period);
        } catch (IOException e) {
            logger.error("read file fail.", e);
            throw new Exception("HIS_FILE_PARSE_ERROR", e);
        } catch (JSONException e) {
            logger.error("performance history file json parse error.", e);
            throw new Exception("HIS_FILE_PARSE_ERROR", e);
        } catch (Exception e) {
            logger.error("performance history file json parse error.", e);
            throw new Exception("HIS_FILE_PARSE_ERROR", e);
        }
        return true;
    }

    private void addDatatypes(String[] stringObjTypes, List<String> objTypes) {
        for (int i = 0; i < stringObjTypes.length; i++) {
            int index = stringObjTypes[i].indexOf(":{");
            if (-1 == index) {
                if (logger.isDebugEnabled()) {
                    logger.debug("no data. objType:" + stringObjTypes[i]);
                }
                continue;
            }
            String objType = stringObjTypes[i].substring(0, index);
            objType = objType.replaceAll("\"", "").trim();
            objTypes.add(objType);
        }
    }

    private static long getMaxEndTime(long maxEndTime, long dataEndTime) {
        return Math.max(maxEndTime, dataEndTime);
    }

    private static long getMinStartTime(long minStartTime, long dataStartTime) {
        if (minStartTime == 0) {
            return dataStartTime;
        }
        return Math.min(minStartTime, dataStartTime);
    }

    private int parseFileBaseInfo(PerfStatHisFileInfo perfStatHisFileInfo, byte[] headerBuf)
            throws UnsupportedEncodingException {
        byte[] checkCodeBuf = new byte[CHECK_CODE_LENGTH];
        System.arraycopy(headerBuf, 0, checkCodeBuf, 0, checkCodeBuf.length);
        perfStatHisFileInfo.setCheckCode(new String(checkCodeBuf));

        byte[] versionBuf = new byte[VERSION_LENGTH];
        System.arraycopy(headerBuf, 32, versionBuf, 0, versionBuf.length);
        perfStatHisFileInfo.setVersion(GFCommon.bytes2int(versionBuf, false));

        byte[] deviceSnBuf = new byte[DEVICE_SN_LENGTH];
        System.arraycopy(headerBuf, 36, deviceSnBuf, 0, deviceSnBuf.length);
        perfStatHisFileInfo.setDeviceSn(new String(deviceSnBuf).trim());

        byte[] deviceNameBuf = new byte[DEVICE_NAME_LENGTH];
        System.arraycopy(headerBuf, 3 * 100 - 8, deviceNameBuf, 0, deviceNameBuf.length);
        perfStatHisFileInfo.setDeviceName(new String(deviceNameBuf, "UTF-8").trim());

        byte[] dataLengthBuf = new byte[DATA_LENGTH_LENGTH];
        System.arraycopy(headerBuf, 333, dataLengthBuf, 0, dataLengthBuf.length);
        int dataLength = GFCommon.bytes2int(dataLengthBuf, false);
        perfStatHisFileInfo.setDataLength(dataLength);
        return dataLength;
    }

    private static List<String> getResIds(JSONObject objectTypeMap) throws JSONException {
        JSONArray jsonArray;
        try {
            jsonArray = objectTypeMap.getJSONArray("IDs");
        } catch (JSONException e) {
            jsonArray = objectTypeMap.getJSONArray("Names");
        }
        List<String> names = new LinkedList<String>();
        for (int i = 0; i < jsonArray.length(); i++) {
            names.add(jsonArray.getString(i));
        }
        return names;
    }

    private static List<String> getNames(JSONObject objectTypeMap) throws JSONException {
        JSONArray jsonArray = objectTypeMap.getJSONArray("Names");
        List<String> names = new LinkedList<String>();
        for (int i = 0; i < jsonArray.length(); i++) {
            names.add(jsonArray.getString(i));
        }
        return names;
    }

    private static void getDataSource(int offset, byte[] periodDataBuf, int dataLength,
        PerfStatHisObject perfStatHisObject) throws IOException, JSONException {
        List<Integer> dataTypes = perfStatHisObject.getDataTypes();
        int[] values = new int[dataTypes.size()];
        int valueindex = 0, curindex = 0;
        for (Integer d : dataTypes) {
            if (d != null) {
                byte[] valueBuf = new byte[dataLength];
                System.arraycopy(periodDataBuf, offset + curindex * dataLength, valueBuf, 0, valueBuf.length);
                values[valueindex] = GFCommon.bytes2int(valueBuf, false);
                valueindex++;
            }
            curindex++;
        }
        perfStatHisObject.getData().add(values);
    }

    private static Map<Integer, List<Integer>> mergeObjectTypeToDataTypesMapping(
            Map<Integer, List<Integer>> destMapping, Map<Integer, List<Integer>> srcMapping) {
        Iterator<Map.Entry<Integer, List<Integer>>> it = srcMapping.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<Integer, List<Integer>> entry = it.next();
            Integer objectType = entry.getKey();
            if (destMapping.keySet().contains(objectType)) {
                List<Integer> destDataTypes = destMapping.get(objectType);
                List<Integer> srcDataTypes = srcMapping.get(objectType);
                addDataType2destMap(destMapping, objectType, destDataTypes, srcDataTypes);
            } else {
                destMapping.put(objectType, srcMapping.get(objectType));
            }
        }
        return destMapping;
    }

    private static void addDataType2destMap(Map<Integer, List<Integer>> destMapping, Integer objectType,
                                            List<Integer> destDataTypes, List<Integer> srcDataTypes) {
        for (Integer srcDataType : srcDataTypes) {
            if (!destDataTypes.contains(srcDataType)) {
                destMapping.get(objectType).add(srcDataType);
            }
        }
    }

    private static Map<Integer, List<String>> mergeObjectTypeToMapping(Map<Integer, List<String>> destMapping,
        Map<Integer, List<String>> srcMapping) {
        Iterator<Map.Entry<Integer, List<String>>> it = srcMapping.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<Integer, List<String>> entry = it.next();
            Integer objectType = entry.getKey();
            if (destMapping.keySet().contains(objectType)) {
                List<String> destNames = destMapping.get(objectType);
                List<String> srcNames = srcMapping.get(objectType);

                addName2Map(destMapping, objectType, destNames, srcNames);
            } else {
                destMapping.put(objectType, srcMapping.get(objectType));
            }
        }
        return destMapping;
    }

    private static void addName2Map(Map<Integer, List<String>> destMapping, Integer objectType, List<String> destNames,
                                    List<String> srcNames) {
        for (String srcName : srcNames) {
            if (!destNames.contains(srcName)) {
                destMapping.get(objectType).add(srcName);
            }
        }
    }

    private static List<Integer> mergeObjectTypes(List<Integer> destObjectTypes, List<Integer> srcObjectTypes) {
        for (Integer objectType : srcObjectTypes) {
            if (!destObjectTypes.contains(objectType)) {
                destObjectTypes.add(objectType);
            }
        }
        return destObjectTypes;
    }

    private String decompress(String fileName) throws Exception {
        if (logger.isInfoEnabled()) {
            logger.info("begin decompress. fileName : " + fileName);
        }
        if (fileName.endsWith("tgz") || fileName.endsWith("bz2")) {
            File file = new File(fileName);
            String folder = fileName.substring(0, fileName.lastIndexOf(File.separator));
            if (logger.isInfoEnabled()) {
                logger.info("TarUtils.extract(file, folder) :" + TarUtils.extract(file, folder));
            }
            return TarUtils.extract(file, folder);
        } else {
            return fileName;
        }
    }
}