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

package org.opensds.storage.conection.rest.constants;

public enum LOG_TYPE {
    /**
     * info 日志
     */
    LOG_INFO("info"),
    /**
     * 错误日志
     */
    LOG_ERROR("error"),
    /**
     * denug日志
     */
    LOG_DEBUG("debug"),
    /**
     * 警告日志
     */
    LOG_WARN("warning");

    private String value;

    /**
     * 构造函数
     */
    private LOG_TYPE(String value) {
        this.value = value;
    }

    /**
     * 取得枚举的整型值
     *
     * @return int 返回结果
     */
    public String getValue() {
        return this.value;
    }

    /**
     * toString
     *
     * @return 返回結果
     */
    public String toString() {
        return this.getClass().getSimpleName() + "." + this.name();
    }
}