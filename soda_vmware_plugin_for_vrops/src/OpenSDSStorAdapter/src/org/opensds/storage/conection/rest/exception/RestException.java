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

package org.opensds.storage.conection.rest.exception;

public class RestException extends Exception {
    private static final long serialVersionUID = -1041241007791942503L;

    private String errorCode;

    private String errorDescription;

    private String errorSuggestion;

    private Exception exception;

    public RestException(String errorCode, String errorDescription, String errorSuggestion) {
        this.errorCode = errorCode;
        this.errorDescription = errorDescription;
        this.errorSuggestion = errorSuggestion;
    }

    public void setException(Exception exception) {
        this.exception = exception;
    }

    public Exception getException() {
        return exception;
    }

    public String getErrorCode() {
        return errorCode;
    }

    public void setErrorCode(String errorCode) {
        this.errorCode = errorCode;
    }

    public String getErrorDescription() {
        return errorDescription;
    }

    public void setErrorDescription(String errorDescription) {
        this.errorDescription = errorDescription;
    }

    public String getErrorSuggestion() {
        return errorSuggestion;
    }

    public void setErrorSuggestion(String errorSuggestion) {
        this.errorSuggestion = errorSuggestion;
    }

    @Override
    public String toString() {
        return "Exception : [errorCode:" + errorCode + ",errorDescription:" + errorDescription + ",errorSuggestion:"
                + errorSuggestion + "]";
    }
}
