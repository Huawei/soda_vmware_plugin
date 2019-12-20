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

import org.opensds.storage.conection.rest.exception.RestException;

public class OperationError {
    public enum REQUEST {
        MISSING_REQUIRED_PARAM("Request_error_00000001", "execute failed,the required parameter {0} " +
                "does not provided", "please full fill all mandatory parameter,and try again"),

        REQUEST_OPERATION_NAME_INVALID("Request_error_00000002", "the request operation {0} " +
                "does not supported", "please check the operation name you provided"),

        REQUEST_PARAMETER_CAN_NOT_ENCODE_TO_UTF8("Request_error_00000003", "the request JSONBody {0} " +
                "can not encode to utf-8", "please check the parameters you provided"),

        PERFORM_ACTION_NOT_LOGIN_ERROR("Request_error_00000004", "login is a must step before " +
                "perform the action", "please do the login first"
        );

        private String errorCode;
        private String errorDesc;
        private String errorSuggestion;

        REQUEST(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        /**
         * Create Exception
         * @param strings Exception message
         * @return RestException
         */
        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }
    }

    public enum JSON {
        MAKE_LOGIN_JSON_ERROR("JSON_error_00000001",
                "the login param you provided {0} can not convert to a json string",
                "please check the parameter you provided"),

        MAKE_REQUEST_BODY_JSON_ERROR("JSON_error_00000002",
                "the request parameter you provided {0} can not convert to a json string",
                "please check the parameter you provided"),

        MAKE_RESPONSE_BODY_TO_JSON_ERROR("JSON_error_00000003",
                "the http response body {0} can not convert to a jsonObject",
                "please check the storage devicemanger rest interface");

        private String errorCode;
        private String errorDesc;
        private String errorSuggestion;

        JSON(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        /**
         * Create Exception
         * @param strings Exception message
         * @return RestException
         */
        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }
    }

    public enum INTERNAL {
        OPERATION_CONFIG_FILE_LOADING_ERROR("Internal_error_00000001",
                "load the operation config file error,config file is {0}",
                "please contact the technical support to solve this"),

        REST_RESPONSE_ERROR_CODE_MISSING("Internal_error_00000002",
                "the rest response missing the error code, response is {0}",
                "please check the device manager rest interface");

        private String errorCode;
        private String errorDesc;
        private String errorSuggestion;

        INTERNAL(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        /**
         * Create Exception
         * @param strings Exception message
         * @return RestException
         */
        public RestException newException(String... strings) {
            for (int i = 0; i < strings.length; i++) {
                errorDesc = this.errorDesc.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errorDesc, errorSuggestion);
        }

        public String getErrorCode() {
            return errorCode;
        }

        public String getErrorDesc() {
            return errorDesc;
        }

        public String getErrorSuggestion() {
            return errorSuggestion;
        }
    }

    public enum HTTP {
        HTTP_RESPONSE_ERROR_STATUS_CODE("Http_error_00000001",
                "http operation response status code is not 200 , status code  {0}",
                "please check the network and storage device deviceManager rest interface"),

        HTTP_RESPONSE_BODY_ERROR("Http_error_00000002", "http operation response body is empty or not a " +
                "stream", "please check the network and storage device deviceManager rest interface"),

        HTTP_RESPONSE_BODY_READ_ERROR("Http_error_00000003",
                "http operation response body read error ,the exception is {0}",
                "please check the network connectivity and storage device deviceManager rest interface"),

        HTTP_IO_EXCEPTION_ERROR("Http_error_00000004",
                "http operation failed because of an IOException,method is {0} and url is {1},the " +
                        "exception is {2}", "please check the network connectivity, request restUrl and " +
                        "storage evice deviceManager rest interface"),

        HTTP_EXCEPTION_ERROR("Http_error_00000005",
                "http operation failed because of an Exception,method is {0} and url is {1} , exception " +
                    "is {2}", "please check the network connectivity, request restUrl and storage " +
                    "device deviceManager rest interface"),

        HTTP_RESPONSE_NULL_ERROR("Http_error_00000006", "http operation failed the http response is null",
                "please check the network connectivity, request restUrl and storage device " +
                "deviceManager rest interface"),

        HTTP_RESPONSE_NULL_STATUS_LINE("Http_error_00000007", "http operation response status line is null",
                "please check the network connectivity and storage device deviceManager rest interface"),

        HTTP_UNSUPPORTED_HTTP_METHOD("Http_error_00000008", "http method {0} you provided does not " +
                "supported", "please check the method you provided"),

        HTTP_LOGIN_CREDENTIAL_ERROR("Http_error_00000009", "the credential {0} you provided is not correct",
                "please check the credential you provided"),

        HTTP_DEVICEMANAGER_LOGIN_ERROR("Http_error_00000010", "error happened when login to the " +
                "devicemanager", "please check the network or contact the devicemanager administrator"),

        HTTP_SSL_NO_SUCH_ALGORITHM_ERROR("Http_error_00000011",
                "error happened when start https connection the encrypt algorithm is {0} the exception is " +
                        "{1}", "please check weather the ssl encrypt algorithm is correct"),

        HTTP_SSL_KEY_MANAGEMENT_ERROR("Http_error_00000012",
                "error happened when start https connection,https key management error ,the exception is " +
                        "{0}", "please check if the devicemanager can accept ssl connection without key or"
                        + "check if the key file you provided is valid"),

        HTTP_SSL_KEY_STORE_ERROR("Http_error_00000013",
                "error happened when start https connection,https key {0} you provide may not a valid " +
                "type , the exception is {1}", "please check if the key file is a valid keystore type"),

        HTTP_SSL_KEY_CERTIFICATE_ERROR("Http_error_00000014",
                "when create https connection,key certificate failed,key file {0} or password {1} " +
                        "you provided may not correct , the exception is {2}", "please check if the key " +
                        "file and the key password are valid"),

        HTTP_SSL_KEY_LOAD_ERROR("Http_error_00000015",
                "when create https connection,key certificate failed,key file {0} or password  you" +
                "provided may not correct,the exception is {1}", "please check if the key file and the key" +
                "password are valid"),

        HTTP_SSL_KEY_FILE_DOES_NOT_EXIST("Http_error_00000016",
                "when create https connection,key file {0} you provided does not exist , the exception " +
                        "is {1}", "please check if the key file path is correct"),

        HTTP_SSL_KEY_NO_SUCH_ALGORITHM_ERROR("Http_error_00000017",
                "when create https connection,the algorithm used to check "
                        + "the integrity of the keystore {0} cannot be found,the exception is {1}",
                "please check if the key file is valid "),

        HTTP_GET_X509_CERTIFICATE_INSTANCE_ERROR("Http_error_00000019", "get X.509 certificate instance " +
                "error", "please check the certificate file {0} you provider,exception is : {1}"),

        HTTP_SSL_KEY_READ_ERROR("Http_error_00000020", "certification file read error",
                "please check the certificate file {0} you provider,exception is : {1}"),

        HTTP_SSL_CERTIFICATE_LOAD_ERROR("Http_error_00000021",
                "when create https connection load certificate failed , certificate file {0} you " +
                "provided may not correct,the exception is {1}",
                "please check if the certificate file is valid"),

        HTTP_SSL_CERTIFICATE_NO_SUCH_ALGORITHM_ERROR("Http_error_00000022",
                "when create https connection,the algorithm used to check "
                        + "the integrity of the certificate cannot be found ",
                "please check if the certificate file is valid");

        private String errorCode;
        private String errorDesc;
        private String errorSuggestion;

        HTTP(String errorCode, String errorDescription, String errorSuggestion) {
            this.errorCode = errorCode;
            this.errorDesc = errorDescription;
            this.errorSuggestion = errorSuggestion;
        }

        public String getErrorCode() {
            return errorCode;
        }

        public String getErrorDesc() {
            return errorDesc;
        }

        public String getErrorSuggestion() {
            return errorSuggestion;
        }

        /**
         * Create Exception
         * @param strings Exception message
         * @return RestException
         */
        public RestException newException(String... strings) {
            String errortest = this.errorDesc;
            for (int i = 0; i < strings.length; i++) {
                errortest = errortest.replace("{" + i + "}", strings[i]);
            }
            return new RestException(errorCode, errortest, errorSuggestion);
        }
    }

    /**
     * Create Exception
     * @param strings Exception message
     * @return RestException
     */
    public static RestException newNullObjectionException(String... strings) {
        String errorDesc = "variable {0} cannot be a null value";
        for (int i = 0; i < strings.length; i++) {
            errorDesc = errorDesc.replace("{" + i + "}", strings[i]);
        }
        return new RestException("Common_error_00000001", errorDesc, "please check the value");
    }
}
