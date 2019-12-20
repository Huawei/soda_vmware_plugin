// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.storage.vro.plugin.model;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;

import org.apache.log4j.Logger;

public enum ObjectsBuilder {
    INSTANCE;

    private Properties prop = null;

    private static final Logger LOG = Logger.getLogger(ObjectsBuilder.class);

    private ObjectsBuilder() {
        Logger log = Logger.getLogger(ObjectsBuilder.class);
        prop = new Properties();
        InputStream in = null;
        try {
            ClassLoader cl = ObjectsBuilder.class.getClassLoader();
            if (cl != null) {
                in = cl.getResourceAsStream("bean.properties");
            } 
            if (in != null) {
                prop.load(in);
            } else {
                log.error("Bean.properties is null !");
            }
        } catch (IOException e) {
            log.error("ObjectsBuilder::" + e.getMessage(), e);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    log.error("ObjectsBuilder::" + e.getMessage(), e);
                }
            }
        }
    }

    /**
     * Build object and object id
     * @param map Object param map
     * @param clazz Object class
     * @param storageArrayInfo OceanStor info
     * @return Object
     * @throws StorageCommonException
     */
    public Object buildObject(Map<String, String> map, Class clazz,
        StorageArrayInfo storageArrayInfo) throws StorageCommonException {
        Object object = null;
        try {
            object = clazz.newInstance();
            String propName = clazz.getName();
            if(clazz.getName().contains(".")) {
                String[] strings = clazz.getName().split("\\.");
                propName = strings[strings.length -1];
            }
            String clazzProp = prop.getProperty(propName);
            if (clazzProp != null) {
                String[] clazzPropStrs = clazzProp.split(",");
                for (String str : clazzPropStrs) {
                    String value = map.get(str.toUpperCase(Locale.US));
                    if (value == null) {
                        value = map.get(str.toLowerCase(Locale.US));
                    }
                    if (value == null) {
                        continue;
                    }
                    String methodStr = "set"
                            + toUpperCaseFirstOne(str.toLowerCase(Locale.US));
                    Method method = clazz.getDeclaredMethod(methodStr,
                            String.class);
                    method.invoke(object, value);
                }
                reBuildObjectID(object, storageArrayInfo.getId());
            }
        } catch (InstantiationException ie) {
            LOG.error("buildObject::" + ie.getMessage());
            throw new StorageCommonException(ie);
        } catch (IllegalAccessException ille) {
            LOG.error("buildObject::" + ille.getMessage());
            throw new StorageCommonException(ille);
        } catch (NoSuchMethodException ne) {
            LOG.error("buildObject::" + ne.getMessage());
            throw new StorageCommonException(ne);
        } catch (SecurityException se) {
            LOG.error("buildObject::" + se.getMessage());
            throw new StorageCommonException(se);
        } catch (IllegalArgumentException ile) {
            LOG.error("buildObject::" + ile.getMessage());
            throw new StorageCommonException(ile);
        } catch (InvocationTargetException ine) {
            LOG.error("buildObject::" + ine.getMessage());
            throw new StorageCommonException(ine);
        }
        return object;
    }

    private void reBuildObjectID(Object object, String id) throws NoSuchMethodException, SecurityException,
        IllegalAccessException, IllegalArgumentException, InvocationTargetException {
        Class clazz = object.getClass();
        Method methodGetId = clazz.getDeclaredMethod("getId");
        String oldId = (String) methodGetId.invoke(object);
        String newId = id + "," + oldId;

        Method setRealId = clazz.getDeclaredMethod("setReal_id", String.class);
        setRealId.invoke(object, oldId);

        Method setSetId = clazz.getDeclaredMethod("setId", String.class);
        setSetId.invoke(object, newId);
    }

    private String toUpperCaseFirstOne(String str) {
        if (Character.isUpperCase(str.charAt(0))) {
            return str;
        } else {
            return (new StringBuilder())
                    .append(Character.toUpperCase(str.charAt(0)))
                    .append(str.substring(1))
                    .toString();
        }
    }
}
