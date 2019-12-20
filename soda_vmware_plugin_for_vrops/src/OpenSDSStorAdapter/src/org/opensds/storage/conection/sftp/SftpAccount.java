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

package org.opensds.storage.conection.sftp;

public class SftpAccount {
    private static org.apache.log4j.Logger logger = org.apache.log4j.Logger.getLogger(SftpAccount.class);

    private String userName = "";

    private String password;

    private String ipAddress = "";

    private int port;

    private String identity;

    private String passphrase;

    private Boolean publicKeyAuth;

    public SftpAccount() {
    }

    public SftpAccount(String ip, String name, String priKey, String passphrase, int port) {
        if (null != ip) {
            this.ipAddress = ip;
        }

        if (null != passphrase) {
            this.passphrase = passphrase;
        }
        if (null != name) {
            this.userName = name;
        }
        this.identity = priKey;
        this.port = port;
    }

    public SftpAccount(String ip, String name, String password, int port) {
        if (null != ip) {
            this.ipAddress = ip;
        }

        if (null != password) {
            this.password = password;
        }
        if (null != name) {
            this.userName = name;
        }

        this.port = port;
    }

    public String getIpAddress() {
        return this.ipAddress;
    }

    public String getPassword() {
        return this.password;
    }

    public int getPort() {
        return this.port;
    }

    public String getUserName() {
        return this.userName;
    }

    public String getPassphrase() {
        return passphrase;
    }

    public void setPassphrase(String passphrase) {
        this.passphrase = passphrase;
    }

    public Boolean getPublicKeyAuth() {
        return publicKeyAuth;
    }

    public void setPublicKeyAuth(Boolean publicKeyAuth) {
        this.publicKeyAuth = publicKeyAuth;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public String getIdentity() {
        return identity;
    }

    public void setIdentity(String identity) {
        this.identity = identity;
    }

    /**
     * Check sftp account
     * @param account Sftp account
     * @return boolean
     */
    public boolean validateSftpAccount(SftpAccount account) {
        if (null == account) {
            logger.error("the sftp account can not be null ");
            return false;
        }
        if (null == account.getIpAddress() || null == account.getUserName()) {
            logger.error("the sftp account ip and username can not be null ");
            return false;
        }
        if (null != account.getPublicKeyAuth() && account.getPublicKeyAuth()) {
            if (account.getIdentity() == null) {
                logger.error("the sftp account Identity can not be null ");
                return false;
            }
        } else if (null == account.getPassword()) {
            logger.error("the sftp account password can not be null ");
            return false;
        } else {
            if (logger.isInfoEnabled()) {
                logger.info("Check sftp account success");
            }
        }
        return true;
    }

    @Override
    public String toString() {
        return new StringBuffer().append(super.toString()).append("ip:" + this.ipAddress)
                .append("user:" + this.userName).append("password:" + "<null>").append("port:" + this.port)
                .toString();
    }

}
