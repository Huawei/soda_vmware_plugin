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

import java.util.Collection;
import java.util.Map;

public final class VerifyUtil {
    private VerifyUtil() {
    }

    /**
     * Check empty
     * @param collection Collection
     * @return boolean
     */
    public static boolean isEmpty(Collection<?> collection) {
        return (null == collection) || (collection.isEmpty());
    }

    /**
     * Check empty
     * @param obj object
     * @return boolean
     */
    public static boolean isEmpty(Object obj) {
        return null == obj;
    }

    /**
     * Check empty
     * @param map map
     * @return boolean
     */
    public static boolean isEmpty(Map<?, ?> map) {
        return (null == map) || (map.isEmpty());
    }

    /**
     * Check empty
     * @param map map
     * @return boolean
     */
    public static boolean isEmpty(Map<?, ?> map, Object key) {
        // added by z90003445 2011-2-22
        if (isEmpty(map)) {
            return true;
        }
        // end
        return !map.containsKey(key) || null == map.get(key) || "".equals(map.get(key));
    }

    /**
     * Check empty
     * @param string string
     * @return boolean
     */
    public static boolean isEmpty(String string) {
        return (null == string) || (string.isEmpty());
    }

    /**
     * Check empty
     * @param objects objects
     * @return boolean
     */
    public static boolean isEmpty(Object[] objects) {
        return objects == null || objects.length == 0;
    }

    /**
     * Check empty
     * @param objects objects
     * @return boolean
     */
    public static boolean isEmpty(Object[][] objects) {
        if (objects == null || objects.length == 0) {
            return true;
        }

        for (Object[] objectArray : objects) {
            if (objectArray.length != 0) {
                return false;
            }
        }
        return true;
    }

    /**
     * Check empty
     * @param obj object
     * @return boolean
     */
    public static boolean isMultiEmpty(Object... obj) {
        if (obj == null) {
            return true;
        }
        for (int i = 0; i < obj.length; i++) {
            if (null == obj[i]) {
                return true;
            }
        }
        return false;
    }
}
