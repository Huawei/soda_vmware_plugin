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

package org.opensds.ecosystem.adapter.model;

import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.log4j.Logger;

import org.opensds.ecosystem.adapter.model.meta.StorModel;
import org.opensds.ecosystem.adapter.model.meta.ObjectType;
import org.opensds.ecosystem.adapter.model.meta.ResourceModel;
import org.opensds.storage.extracdata.PerfStatHisFileInfo;
import org.opensds.storage.extracdata.PerfStatHisObject;

public final class LoadDataFactory {
    private static final Object LOCKOBJECT = new Object();
    private static final Logger LOGGER = Logger
            .getLogger(LoadDataFactory.class);

    private static LoadDataFactory instance;

    private LoadDataFactory() {
    }

    /**
     * 获取LoadDataFactory对象
     *
     * @return synchronized 返回值为该对象
     */
    public static LoadDataFactory getInstance() {
        synchronized (LOCKOBJECT) {
            if (instance == null) {
                instance = new LoadDataFactory();
            }
            return instance;
        }
    }

    /**
     * 添加数据
     *
     * @param data 存储数据
     * @param from 性能信息
     */
    public void add2MergeStorData(StorData data, PerfStatHisFileInfo from) {
        data.setDeviceName(from.getDeviceName());
        data.setDeviceSn(from.getDeviceSn());

        add2MergeStorData(from,
                data.getControllerData(),
                ObjectType.Controller);
        add2MergeStorData(from, data.getDiskData(), ObjectType.Disk);
        add2MergeStorData(from,
                data.getDiskDomainData(),
                ObjectType.DiskDomain);
        add2MergeStorData(from,
                data.getFileSystemData(),
                ObjectType.FileSystem);
        add2MergeStorData(from, data.getHostData(), ObjectType.Host);
        add2MergeStorData(from, data.getLunData(), ObjectType.Lun);
        add2MergeStorData(from, data.getPoolData(), ObjectType.Pool);
        add2MergeStorData(from, data.getFcPortData(), ObjectType.FCPort);
        add2MergeStorData(from,
                data.getIscsiPortData(),
                ObjectType.ISCSIPort);
        add2MergeStorData(from,
                data.getRemoteReplicationModel(),
                ObjectType.RemoteReplication);
        add2MergeStorData(from, data.getSnapShotModel(), ObjectType.SnapShot);
        add2MergeStorData(from,
                data.getSmartPartitionModel(),
                ObjectType.SmartPartition);
        add2MergeStorData(from, data.getSmartQosModel(), ObjectType.SmartQos);
    }

    /**
     * 通过id关联名字
     *
     * @param idList   id列表
     * @param nameList 名字列表
     * @param map      集合
     */
    public void setNameMapById(List<String> idList, List<String> nameList, Map<String, String> map) {
        if (LOGGER.isInfoEnabled()) {
            LOGGER.info("idList.size:" + idList.size() + "nameList.size:" + nameList.size());
        }
        for (int k = 0; k < idList.size(); k++) {
            if (nameList.size() < k - 1) {
                LOGGER.error("idList.size:" + k + "nameList.size:" + nameList.size());
                break;
            }
            map.put(idList.get(k), nameList.get(k));
        }
    }

    /**
     * 拷贝数据
     *
     * @param source       源
     * @param dest         目标
     * @param nameMap      NameMap
     * @param dataTypeSets DataTypeSets
     */
    public void copyData(List<PerfStatHisObject> source, Map<String, ResourceModel> dest, Map<String, String> nameMap,
        Set<Integer> dataTypeSets) {
        for (int i = 0; i < source.size(); i++) {
            Set<Integer> unUsed = new HashSet<Integer>();
            for (Integer k : dataTypeSets) {
                unUsed.add(k);
            }

            PerfStatHisObject p = source.get(i);
            ResourceModel res = new ResourceModel();
            res.setResourceId(p.getResId());
            res.setResourceName(nameMap.get(p.getResId()));
            List<Integer> keys = p.getDataTypes();
            List<int[]> vals = p.getData();
            int[] val = vals.get(0);
            Map<Integer, Integer> ds = new HashMap<Integer, Integer>();
            for (int j = 0; j < keys.size(); j++) {
                if (dataTypeSets.contains(keys.get(j))) {
                    ds.put(keys.get(j), val[j]);
                    unUsed.remove(keys.get(j));
                }
            }

            for (Integer kk : unUsed) {
                ds.put(kk, 0);
            }
            if (!unUsed.isEmpty()) {
                if (LOGGER.isInfoEnabled()) {
                    LOGGER.info("find unused metric :" + unUsed);
                }
            }

            res.setResourceData(ds);
            dest.put(res.getResourceId(), res);
        }
    }

    /**
     * 添加数据
     *
     * @param from       性能信息
     * @param model      模型
     * @param objectType 对象类型
     */
    public void add2MergeStorData(PerfStatHisFileInfo from, StorModel model, ObjectType objectType) {
        if (from.getObjectTypeToDatasMapping() == null) {
            LOGGER.error("the fileInfo is not data");
            return;
        }

        List<String> idList = from.getObjectTypeToIdsMapping().get(objectType.getId());
        List<String> nameList = from.getObjectTypeToNamesMapping().get(objectType.getId());

        if (idList == null || nameList == null) {
            LOGGER.error("the object type has no data:object id="
                    + objectType.toString());
            return;
        }
        Map<String, String> nameMap = new HashMap<String, String>();
        setNameMapById(idList, nameList, nameMap);
        List<PerfStatHisObject> resdata = from.getObjectTypeToDatasMapping()
                .get(objectType.getId());
        Map<String, ResourceModel> reses = new HashMap<String, ResourceModel>();

        if (resdata == null) {
            return;
        }
        copyData(resdata, reses, nameMap, model.getDataTypes());

        add2MergeIDList(model.getResourceIdList(), idList);
        add2MergeDataList(model.getDataList(), reses);

        if (LOGGER.isInfoEnabled()) {
            LOGGER.info(model);
        }
    }

    /**
     * 添加数据
     *
     * @param resc Resource
     * @param tar  Target
     */
    public void add2MergeIDList(List<String> resc, List<String> tar) {
        for (String val : tar) {
            if (resc.contains(val)) {
                if (LOGGER.isInfoEnabled()) {
                    LOGGER.info("resource id list already has id:" + val);
                }
            } else {
                resc.add(val);
                if (LOGGER.isInfoEnabled()) {
                    LOGGER.info("resource id list add a new id:" + val);
                }
            }
        }
    }

    /**
     * 添加数据
     *
     * @param resc Resource
     * @param tar  Target
     */
    public void add2MergeDataList(Map<String, ResourceModel> resc, Map<String, ResourceModel> tar) {
        int i = 0;
        for (Map.Entry<String, ResourceModel> entry : tar.entrySet()) {
            if (resc.containsKey(entry.getKey())) {
                LOGGER.error(
                        "---------------find a same id :" + entry.getKey());
                LOGGER.error(
                        "---------------had data is A,will fill data is B");
                LOGGER.error("[A] " + resc.get(entry.getKey()));
                LOGGER.error("[B] " + entry.getValue());
                LOGGER.error("                  ");
                i++;
            }
        }
        LOGGER.error("has data size :" + resc.size() + ",new data size :" + tar.size() + ",repeat count :" + i);
        resc.putAll(tar);
    }
}
