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

public class GFCommon {
    private static final int OXFF = 0xff;

    /**
     * Bytes to int
     * @param b bytes
     * @param isHighFirst Check high first
     * @return int
     */
    public static int bytes2int(byte[] b, boolean isHighFirst) {
        return (int) bytes2long(b, isHighFirst);
    }

    /**
     * Bytes to long
     * @param b bytes
     * @return long
     */
    public static long bytes2long(byte[] b) {
        return bytes2long(b, true);
    }

    /**
     * Bytes to int
     * @param b Bytes
     * @return int
     */
    public static int bytes2int(byte[] b) {
        return (int) bytes2long(b);
    }

    /**
     * Bytes to long
     * @param b Bytes
     * @param isHighFirst Check high first
     * @return long
     */
    public static long bytes2long(byte[] b, boolean isHighFirst) {
        long result = 0;

        if (b != null && b.length <= 8) {
            long value;

            if (isHighFirst) {
                for (int i = b.length - 1, j = 0; i >= 0; i--, j++) {
                    value = b[i] & OXFF;
                    result += value << (j << 3);
                }
            } else {
                for (int i = 0, j = 0; i <= b.length - 1; i++, j++) {
                    value = b[i] & OXFF;
                    result += value << (j << 3);
                }
            }
        }
        return result;
    }
}