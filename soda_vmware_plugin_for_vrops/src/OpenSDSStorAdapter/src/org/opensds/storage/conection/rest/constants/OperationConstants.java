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

package org.opensds.storage.conection.rest.constants;

public class OperationConstants {
    /**
     * Error constant
     */
    public static class HttpErrorConstant {
        /**
         * rest
         */
        public static final int OK = 0;

        /**
         * authentication error
         */
        public static final int ERROR_401 = -401;
    }

    /**
     * Rest request constant
     */
    public static class RestRequestConstant {
        public static final String REST_LOGIN_URI = "/xxxx/sessions";
        public static final String REST_LOGOUT_URI = "/sessions";
    }
}
