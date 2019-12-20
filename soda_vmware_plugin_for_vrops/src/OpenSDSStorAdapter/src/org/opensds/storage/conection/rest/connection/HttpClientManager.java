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

package org.opensds.storage.conection.rest.connection;

import org.opensds.storage.conection.rest.exception.RestException;
import org.apache.http.client.HttpClient;

public class HttpClientManager {
    private HttpClientFactory httpClientFactory = new HttpClientFactory();
    private HttpClient httpClient;

    /**
     * Get http client
     * @param keyPath Key path
     * @param keyPass Key password
     * @param strictCheckHostName 是否精确校验主机名称
     * @return Httpclient
     * @throws RestException
     */
    public synchronized HttpClient getHttpClient(String keyPath, String keyPass, boolean strictCheckHostName)
            throws RestException {
        if (httpClient == null) {
            httpClient = httpClientFactory.createHttpClient(keyPath, keyPass, strictCheckHostName);
        }
        return httpClient;
    }

    public synchronized HttpClient getHttpClient() throws RestException {
        if (httpClient == null) {
            httpClient = httpClientFactory.createHttpClient();
        }
        return httpClient;
    }

    /**
     * Release http client
     */
    public void releaseHttpClient() {
        httpClientFactory.releaseHttpClient();
        httpClient = null;
    }

    /**
     * Set socket timeout
     * @param socTimeout Socket timeout
     */
    public void setSocTimeout(int socTimeout) {
        httpClientFactory.setSocTimeout(socTimeout);
    }

    public HttpClient getHttpClient(String certificateFilePath, boolean strictCheckHostName) throws RestException {
        if (httpClient == null) {
            httpClient = httpClientFactory.createHttpClient(certificateFilePath, strictCheckHostName);
        }
        return httpClient;
    }
}
