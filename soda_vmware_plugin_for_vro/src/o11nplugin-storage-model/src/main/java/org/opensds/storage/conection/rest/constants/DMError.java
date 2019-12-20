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

import org.opensds.storage.conection.rest.exception.RestException;

public class DMError {
    public static String IO_ERROR = "99999";

    public static enum REQUEST {
        MISSING_REQUIRED_PARAM("Request_error_00000001",
                "execute failed,the required paramter {0} does not provided",
                "please fullfill all mandatory paramter,and try again"),

        REQUEST_OPERATION_NAME_INVALID("Request_error_00000002",
                "the request operation {0} does not supported",
                "please check the operation name you provided"),

        REQUEST_PARAMETER_CAN_NOT_ENCODE_TO_UTF8("Request_error_00000003",
                "the request JSONBody {0} can not encode to utf-8",
                "please check the parameters you provided");

        private String errorCode;

        private String errorDesc;

        private String errorSuggestion;

        REQUEST(String errorCode, String errorDescription,
                String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }

    }

    public static enum JSON {
        MAKE_LOGIN_JSON_ERROR("JSON_error_00000001",
                "the login param you provided {0} can not convert to a json string",
                "please check the parameter you provided"),

        MAKE_REQUEST_BODY_JSON_ERROR("JSON_error_00000002",
                "the request parameter you provided {0} can not convert to a json string",
                "please check the parameter you provided"),

        MAKE_RESPONSE_BODY_TO_JSON_ERROR("JSON_error_00000003",
                "the http response body {0} can not convert to a jsonObject",
                "please check the storage devicemanger rest interface"),

        RESPONSE_BODY_JSON_PARSE_ERROR("JSON_error_00000004",
                "the http response body parse error :{0}",
                "please check the storage devicemanger rest interface");

        private String errorCode;

        private String errorDesc;

        private String errorSuggestion;

        JSON(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }

    }

    public static enum INTERNAL {
        OPERATION_CONFIG_FILE_LOADING_ERROR("Internal_error_00000001",
                "load the operation config file error,config file is {0}",
                "please contact the technical support to solve this");

        private String errorCode;

        private String errorDesc;

        private String errorSuggestion;

        INTERNAL(String errorCode, String errorDescription,
                 String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }

    }

    public static enum HTTP {
        HTTP_RESPONSE_ERROR_STATUS_CODE("Http_error_00000001",
                "http operaton response status code is not 200 , status code  {0}",
                "please check the network and storage device deviceManager rest interface"),

        HTTP_RESPONSE_BODY_ERROR("Http_error_00000002",
                "http operaton response body is empty or not a stream",
                "please check the network and storage device deviceManager rest interface"),

        HTTP_RESPONSE_BODY_READ_ERROR("Http_error_00000003",
                "http operaton response body read error",
                "please check the network and storage device deviceManager rest interface"),

        HTTP_IO_EXCEPTION_ERROR("Http_error_00000004",
                "http operation failed because of an IOException,method is {0}",
                "please check the network, request restUrl and storage device deviceManager rest interface");

        private String errorCode;

        private String errorDesc;

        private String errorSuggestion;

        HTTP(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }

    }

}
