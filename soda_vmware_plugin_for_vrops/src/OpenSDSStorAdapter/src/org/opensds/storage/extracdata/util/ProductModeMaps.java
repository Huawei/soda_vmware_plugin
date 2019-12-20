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

import java.util.HashMap;
import java.util.Map;

public class ProductModeMaps {
    private static Map<String, String> map = new HashMap<String, String>();

    static {
        map.put("61", "6800 V3");
        map.put("62", "6900 V3");
        map.put("63", "5600 V3");
        map.put("64", "5800 V3");
        map.put("68", "5500 V3");
        map.put("69", "2600 V3");
        map.put("70", "5300 V3");
        map.put("71", "2800 V3");
        map.put("72", "18500 V3");
        map.put("73", "18800 V3");
        map.put("74", "2200 V3");
        map.put("84", "2600F V3");
        map.put("85", "5500F V3");
        map.put("86", "5600F V3");
        map.put("87", "5800F V3");
        map.put("88", "6800F V3");
        map.put("89", "18500F V3");
        map.put("90", "18800F V3");
        map.put("92", "2800 V5");
        map.put("93", "5300 V5");
        map.put("94", "5300F V5");
        map.put("95", "5500 V5");
        map.put("96", "5500F V5");
        map.put("97", "5600 V5");
        map.put("98", "5600F V5");
        map.put("99", "5800 V5");
        map.put("100", "5800F V5");
        map.put("101", "6800 V5");
        map.put("102", "6800F V5");
        map.put("103", "18500 V5");
        map.put("104", "18500F V5");
        map.put("105", "18800 V5");
        map.put("106", "18800F V5");
        map.put("107", "5500 V5 Elite");
        map.put("108", "2100 V3");
        map.put("805", "Dorado5000 V3");
        map.put("806", "Dorado6000 V3");
        map.put("807", "Dorado18000 V3");
        map.put("808", "DoradoNAS");
        map.put("809", "DoradoNAS");
        map.put("810", "Dorado3000 V3");
        map.put("811", "PRODUCT_MODE_D5000V6");
        map.put("812", "PRODUCT_MODE_D5000V6_N");
        map.put("821", "PRODUCT_MODE_D5000V6_I");
        map.put("813", "PRODUCT_MODE_D6000V6");
        map.put("814", "PRODUCT_MODE_D6000V6_NPRODUCT_MODE_D6000V6_N");
        map.put("815", "PRODUCT_MODE_D8000V6");
        map.put("816", "PRODUCT_MODE_D8000V6_N");
        map.put("817", "PRODUCT_MODE_D18000V6");
        map.put("818", "PRODUCT_MODE_D18000V6_N");
        map.put("819", "PRODUCT_MODE_D3000V6");
        map.put("822", "PRODUCT_MODE_D6000V6_I");
        map.put("823", "PRODUCT_MODE_D8000V6_");
        map.put("824", "PRODUCT_MODE_D18000V6_I");
        map.put("825", "Dorado5300 V6");
        map.put("826", "Dorado5500 V6");
        map.put("827", "Dorado5600 V6");
        map.put("828", "Dorado5800 V6");
        map.put("829", "Dorado6800 V6");
        map.put("830", "Dorado18500 V6");
        map.put("831", "Dorado18800 V6");
        // Enhanced
        map.put("112", "2200 V3");
        map.put("113", "2600 V3");
        map.put("114", "2600F V3");
        map.put("115", "5300 V5");
        map.put("116", "5110 V5");
        map.put("117", "5110F V5");
        map.put("118", "5210 V5");
        map.put("119", "5210F V5");
    }

    /**
     * Get value
     * @param key key
     * @return value
     */
    public static String getValue(String key) {
        return map.get(key);
    }
}
