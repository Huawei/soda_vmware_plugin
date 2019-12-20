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

package org.opensds.storage.conection.rest.connection;

import java.io.IOException;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import org.apache.http.HttpVersion;
import org.apache.http.client.HttpClient;
import org.apache.http.conn.params.ConnManagerParams;
import org.apache.http.conn.scheme.PlainSocketFactory;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.scheme.SchemeRegistry;
import org.apache.http.conn.ssl.SSLContexts;
import org.apache.http.conn.ssl.SSLSocketFactory;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.PoolingClientConnectionManager;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;
import org.apache.log4j.Logger;

import org.opensds.storage.vro.plugin.model.StorageCommonException;
import com.vmware.o11n.security.util.KeystoreHelper;

@SuppressWarnings("deprecation")
public class HttpClientFactory {
    private static final Logger LOG = Logger.getLogger(HttpClientFactory.class);

    private static final Object LOCK = new Object();

    /**
     * 同一个IP的最大连接数
     */
    private static final int MAX_TOTAL_CONNECTIONS = 100;

    /**
     * 所有IP的总的最大连接数
     */
    private static final int MAX_ROUTE_CONNECTIONS = 400;

    private static final int HTTP_PORT = 80;

    private static final int HTTPS_PORT = 443;

    /**
     * 等待数据超时时间
     */
    private static final int SOCKET_TIMEOUT = 5 * 60 * 1000;

    /**
     * 请求超时时间
     */
    private static final int CONNECT_TIMEOUT = 5 * 60 * 1000;

    /**
     * 连接池链接不够用的时候等待超时时间，不能太大
     **/
    private static final int CONN_MANAGER_TIMEOUT = 5 * 60 * 1000;

    private static HttpClient httpClientWithTrustStore;

    private static HttpClient httpClient;

    public final static String DEFAULT_CHARSET = "UTF-8";

    private static X509TrustManager manager = new X509TrustManager() {
        @Override
        public X509Certificate[] getAcceptedIssuers() {
            return new java.security.cert.X509Certificate[0];
        }

        @Override
        public void checkServerTrusted(X509Certificate[] arg0, String arg1)
                throws CertificateException {
            return;
        }

        @Override
        public void checkClientTrusted(X509Certificate[] arg0, String arg1)
                throws CertificateException {
            return;
        }
    };

    public static HttpClient getHttpClient() throws StorageCommonException {
        synchronized (LOCK) {
            if (httpClient == null) {
                httpClient = createHttpClient();
                return httpClient;
            } else {
                return httpClient;
            }
        }
    }

    public static HttpClient getHttpClientWithTrustStore()
            throws StorageCommonException {
        synchronized (LOCK) {
            if (httpClientWithTrustStore == null) {
                httpClientWithTrustStore = createHttpClientWithTrustStore();
                return httpClientWithTrustStore;
            } else {
                return httpClientWithTrustStore;
            }
        }

    }

    public static void reloadHttpClient() throws StorageCommonException {
        synchronized (LOCK) {
            releaseHttpClient();
            httpClientWithTrustStore = createHttpClientWithTrustStore();
            httpClient = createHttpClient();
        }
    }

    public static void releaseHttpClient() {
        synchronized (LOCK) {
            if (httpClientWithTrustStore != null) {
                httpClientWithTrustStore.getConnectionManager().shutdown();
                httpClientWithTrustStore = null;
            }
            if (httpClient != null) {
                httpClient.getConnectionManager().shutdown();
                httpClient = null;
            }
        }
    }

    private static KeyStore loadTrustStore() throws StorageCommonException {
        KeyStore keyStore = null;
        try {
            keyStore = KeystoreHelper.loadKeyStore();
        } catch (KeyStoreException e) {
            LOG.error("loadTrustStore-KeyStoreException::");
            throw new StorageCommonException(e);
        } catch (NoSuchAlgorithmException e) {
            LOG.error("loadTrustStore-NoSuchAlgorithmException::");
            throw new StorageCommonException(e);
        } catch (CertificateException e) {
            LOG.error("loadTrustStore-CertificateException::");
            throw new StorageCommonException(e);
        } catch (IOException e) {
            LOG.error("loadTrustStore-IOException::");
            throw new StorageCommonException(e);
        }
        return keyStore;
    }

    private static HttpClient createHttpClient() throws StorageCommonException {

        SSLContext sslcontext = null;
        try {
            sslcontext = SSLContext.getInstance("TLS");
            sslcontext.init(null, new TrustManager[]{manager}, null);
        } catch (KeyManagementException e) {
            LOG.error("createHttpClient-KeyManagementException::");
            throw new StorageCommonException(e);
        } catch (NoSuchAlgorithmException e) {
            LOG.error("createHttpClient-NoSuchAlgorithmException::");
            throw new StorageCommonException(e);
        }

        SchemeRegistry schemeRegistry = new SchemeRegistry();
        schemeRegistry.register(new Scheme("http", HTTP_PORT,
                PlainSocketFactory.getSocketFactory()));
        schemeRegistry.register(
                new Scheme("https", HTTPS_PORT, new SSLSocketFactory(sslcontext,
                        SSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER)));

        PoolingClientConnectionManager connManager = new PoolingClientConnectionManager(
                schemeRegistry);
        connManager.setMaxTotal(MAX_TOTAL_CONNECTIONS);
        connManager.setDefaultMaxPerRoute(MAX_ROUTE_CONNECTIONS);

        HttpParams params = new BasicHttpParams();
        HttpProtocolParams.setVersion(params, HttpVersion.HTTP_1_1);
        HttpProtocolParams.setContentCharset(params, DEFAULT_CHARSET);
        HttpProtocolParams.setUseExpectContinue(params, true);

        HttpConnectionParams.setConnectionTimeout(params, CONNECT_TIMEOUT);
        HttpConnectionParams.setSoTimeout(params, SOCKET_TIMEOUT);
        ConnManagerParams.setTimeout(params, CONN_MANAGER_TIMEOUT);

        return new DefaultHttpClient(connManager, params);
    }

    private static HttpClient createHttpClientWithTrustStore()
            throws StorageCommonException {

        KeyStore trustStore = loadTrustStore();
        SSLContext sslcontext = null;
        try {
            sslcontext = SSLContexts.custom()
                    .loadTrustMaterial(trustStore, new TrustNoneStrategy())
                    .build();
        } catch (KeyManagementException e) {
            LOG.error(
                    "createHttpClientWithTrustStore-KeyManagementException::");
            throw new StorageCommonException(e);
        } catch (NoSuchAlgorithmException e) {
            LOG.error(
                    "createHttpClientWithTrustStore-NoSuchAlgorithmException::");
            throw new StorageCommonException(e);
        } catch (KeyStoreException e) {
            LOG.error("createHttpClientWithTrustStore-KeyStoreException::");
            throw new StorageCommonException(e);
        }

        SSLSocketFactory factory = new SSLSocketFactory(sslcontext,
                new String[]{"TLSv1.2"}, null,
                SSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER);
        factory.setHostnameVerifier(
                SSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER);

        SchemeRegistry schemeRegistry = new SchemeRegistry();
        schemeRegistry.register(new Scheme("https", factory, HTTP_PORT));
        schemeRegistry.register(new Scheme("http",
                PlainSocketFactory.getSocketFactory(), HTTPS_PORT));
        PoolingClientConnectionManager connManager = new PoolingClientConnectionManager(
                schemeRegistry);
        connManager.setMaxTotal(MAX_TOTAL_CONNECTIONS);
        connManager.setDefaultMaxPerRoute(MAX_ROUTE_CONNECTIONS);

        HttpParams params = new BasicHttpParams();
        HttpProtocolParams.setVersion(params, HttpVersion.HTTP_1_1);
        HttpProtocolParams.setContentCharset(params, DEFAULT_CHARSET);
        HttpProtocolParams.setUseExpectContinue(params, true);

        HttpConnectionParams.setConnectionTimeout(params, CONNECT_TIMEOUT);
        HttpConnectionParams.setSoTimeout(params, SOCKET_TIMEOUT);
        ConnManagerParams.setTimeout(params, CONN_MANAGER_TIMEOUT);

        return new DefaultHttpClient(connManager, params);

    }
}
