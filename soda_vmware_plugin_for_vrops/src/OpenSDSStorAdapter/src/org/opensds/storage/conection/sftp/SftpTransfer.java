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

import java.io.File;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.NoRouteToHostException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.jcraft.jsch.ChannelSftp;
import com.jcraft.jsch.ChannelSftp.LsEntry;
import com.jcraft.jsch.HostKey;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Logger;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.SftpException;

public class SftpTransfer {
    /**
     * Server spliter
     */
    public static final String SERVER_SPLITER = ":/";

    /**
     * Default port
     */
    public static final int DEAFLUAT_PORT = 22;

    private static final int CONNECTION_TIMEOUT = 30 * 1000;

    private static final Map<String, Session> ip2session = new ConcurrentHashMap<String, Session>();

    private static final Map<String, ChannelSftp> ip2channel = new ConcurrentHashMap<String, ChannelSftp>();

    private static final Map<String, Lock> ip2lock = new HashMap<String, Lock>();

    private static final Lock lockOfMap = new ReentrantLock();

    private static final ExecutorService es = Executors.newFixedThreadPool(3);

    private static org.apache.log4j.Logger logger = org.apache.log4j.Logger.getLogger(SftpTransfer.class);

    private SftpTransferProgress sftpTansferProgress = new SftpTransferProgress();

    private int ipRetryCount = 0;

    private String encoding;

    private int permission = -1;

    private static Lock getLock(SftpAccount account) {
        Lock t = null;
        lockOfMap.lock();
        try {
            t = ip2lock.get(account.getIpAddress());
            if (t == null) {
                t = new ReentrantLock();
                ip2lock.put(account.getIpAddress(), t);
            }
        } finally {
            lockOfMap.unlock();
        }
        return t;
    }

    static {
        JSch.setLogger(new SftpLogger());
    }

    public SftpTransfer() {
    }

    public SftpTransfer(String encoding) {
        this.encoding = encoding;
    }

    public SftpTransfer(String serverIp, SftpAccount account) throws Exception {
        createSession(account);
    }

    public int getPermission() {
        return permission;
    }

    public void setPermission(int permission) {
        this.permission = permission;
    }

    private static void closeChannel(String ip) {
        try {
            Set<String> keys = ip2channel.keySet();
            for (String key : keys) {
                if (key.startsWith(ip + "_")) {
                    ip2channel.get(key).disconnect();
                    ip2channel.get(key).exit();
                    ip2channel.remove(key);
                    logger.info("Close channel for: " + key);
                }
            }
        } catch (Exception e) {
            logger.error("Close Sftp Error!" + e);
        }
    }

    /**
     * Get current step
     * @return Current step
     */
    public int getCurrentStep() {
        long fileSize = sftpTansferProgress.getFileSize();
        long finishSize = sftpTansferProgress.getFinishSize();

        if (0 == fileSize) {
            return 0;
        }

        int currentStep = (int) (finishSize * 100 / fileSize);

        long tmp = (finishSize * 100) % fileSize;
        if (tmp != 0) {
            currentStep++;
        }
        if (currentStep > 100) {
            currentStep = 100;
        }
        return currentStep;
    }

    /**
     * Transfer file from device
     * @param arrayId Storage array id
     * @param serverPath Server path
     * @param localPath Local path
     * @param account Sftp account
     * @param port port
     * @param brokenByException Broken by exception
     * @throws Exception
     */
    public void transferFileFromDevice(String arrayId, String serverPath, String localPath, SftpAccount account,
        int port, boolean brokenByException) throws Exception {
        String arrayIdTemp = arrayId;
        String serverPathTemp = serverPath;
        String localPathTemp = localPath;
        SftpAccount userTemp = account;
        logger.info("transferFromDevserverPath:" + serverPath + ";localPath" + localPath + " port " + port);

        int index = serverPath.indexOf(SERVER_SPLITER);

        String serverIp = serverPath.substring(0, index);
        String serverRelativePath = serverPath.substring(index + 1);
        String[] ips = serverIp.split(",");
        int length = ips.length;

        int unreachAbleCount = ipRetryCount;
        int usePort = (port > 0) ? port : DEAFLUAT_PORT;

        try {
            while (ipRetryCount < 3 && SftpUtils.isIpUnreachAble(ips[length - 1 - ipRetryCount], arrayId)) {
                ipRetryCount++;
                unreachAbleCount++;
                if (unreachAbleCount == 3) {
                    recheckIps(ips, usePort, arrayId);
                    logger.error("Sftp Fail.no serverIp to download!");
                    return;
                }
            }

            SftpAccount account2 = new SftpAccount(ips[length - 1 - ipRetryCount], account.getUserName(),
                    account.getPassword(), port > 0 ? port : DEAFLUAT_PORT);
            transferFile(account2, serverRelativePath, localPath, true, brokenByException);
            ipRetryCount = 0;
        } catch (Exception e) {
            ipRetryCount++;
            if (brokenByException && highArrayCannotConn(e, ips, usePort, arrayId)) {
                throw e;
            } else if (false || (!brokenByException && !isNoSuchFileError(e))) {
                if (ipRetryCount < length) {
                    transferFileFromDevice(arrayIdTemp, serverPathTemp, localPathTemp, userTemp, port,
                            brokenByException);
                }
                logger.error("Sftp Fail. serverIp:" + serverIp, e);
                if (ipRetryCount == length) {
                    ipRetryCount = 0;
                    throw e;
                }
            } else {
                ipRetryCount = 0;
                throw e;
            }
        }
        logger.debug("transferFileFromDevice over.");
    }

    private boolean isNoSuchFileError(Exception e) {
        Throwable t = e.getCause();
        if (t instanceof Exception) {
            Exception ee = (Exception) e.getCause();
            String errMsg = ee.toString();
            return errMsg != null && errMsg.contains("No such file");
        }
        return false;
    }

    @SuppressWarnings("unused")
    private boolean highArrayCannotConn(Exception e, String[] ips, int usePort, String arrayId) {
        if (false) {
            Throwable t = e.getCause();
            if (t instanceof NoRouteToHostException) {
                recheckIps(ips, usePort, arrayId);
                return true;
            }
        }
        return false;
    }

    private void recheckIps(String[] ips, int usePort, String deviceSn) {
        if (!false) {
            return;
        }
        @SuppressWarnings("unused")
        Set<String> unreachIps = SftpUtils.getUnReachableIps().get(deviceSn);
        if (unreachIps == null) {
            unreachIps = new HashSet<String>();
            SftpUtils.getUnReachableIps().put(deviceSn, unreachIps);
        }
        Set<String> checkedIps = new HashSet<String>();
        Set<String> unReachIpsTemp = new HashSet<String>();
        for (int i = 0; i < ips.length; i++) {
            unreachIps.remove(ips[i]);
            if (checkedIps.contains(ips[i])) {
                continue;
            }
            if (ips[i] != null && ips[i].length() > 0 && !SftpTransfer.isSftpCanConnected(ips[i], usePort)) {
                unReachIpsTemp.add(ips[i]);
            }
            checkedIps.add(ips[i]);
        }
        unreachIps.addAll(unReachIpsTemp);
    }

    /**
     * Transfer file to device
     * @param localPath Localpath
     * @param serverPath Server path
     * @param account Sftp account
     * @throws Exception
     */
    public void transferFileToDevice(String localPath, String serverPath, SftpAccount account) throws Exception {
        logger.info("transferFileToDevice,serverPath:" + serverPath + ";localPath:" + localPath);

        int index = serverPath.indexOf(SERVER_SPLITER);

        String serverIp = serverPath.substring(0, index);

        String serverRelativePath = serverPath.substring(index + 1);
        try {
            transferFile(account, serverRelativePath, localPath, false, Boolean.TRUE);
        } catch (Exception e) {
            logger.error("transferFileToDevice error:" + account);
            throw e;
        }
        logger.debug("transferFileToDevice over.");
    }

    @SuppressWarnings("unchecked")
    public List<String> ls(String ip, String serverPath, SftpAccount account) throws Exception {
        List<String> fileNames = new ArrayList<String>();
        ChannelSftp channel = null;
        try {
            createSession(account);
            channel = getChannel(ip2session.get(ip), account, encoding, Boolean.FALSE);
            List<LsEntry> list = channel.ls(serverPath);
            if (list != null && !list.isEmpty()) {
                for (LsEntry ls : list) {
                    fileNames.add(ls.getFilename());
                }
            }
        } catch (SftpException e) {
            logger.error("transferFile error when call ls.");
        }
        return fileNames;
    }

    /**
     * Get new data file
     * @param ip device ip
     * @param serverPath Server path
     * @param account Sftp account
     * @return File name
     */
    public String getNewDataFile(String ip, String serverPath, SftpAccount account) {
        int first = 0;
        String filename = "";
        ChannelSftp channel = null;
        try {
            createSession(account);
            channel = getChannel(ip2session.get(ip), account, encoding, Boolean.FALSE);
            @SuppressWarnings("unchecked")
            List<LsEntry> list = channel.ls(serverPath);
            if (list != null && !list.isEmpty()) {
                for (LsEntry ls : list) {
                    if (ls.getFilename().length() >= 4) {
                        if (first == 0) {
                            first = ls.getAttrs().getATime();
                        }
                        if (first <= ls.getAttrs().getATime()) {
                            filename = ls.getFilename();
                            first = ls.getAttrs().getATime();
                        }
                    }
                }
            }
        } catch (Exception e) {
            logger.error("transferFile error when call ls." + e.getMessage());
        }
        return filename;
    }

    /*******************************
     * Private Function
     ***************************************/

    private void transferFile(SftpAccount account, String serverRelativePath, String localPath, boolean getOrPut,
        boolean brokenByException) throws Exception {
        ChannelSftp channelSftp = null;
        try {
            createSession(account);
            channelSftp = getChannel(ip2session.get(account.getIpAddress()), account, encoding, brokenByException);
            if (getOrPut) {
                GetSftpFile getsftpFile = new GetSftpFile(channelSftp, serverRelativePath, localPath);
                Future future = es.submit(getsftpFile);
                try {
                    future.get(1, TimeUnit.SECONDS);
                } catch (TimeoutException e) {
                    channelSftp.quit();
                    future.cancel(true);
                    logger.error("getSftpFile is timeOut.", e);
                } catch (Exception e) {
                    logger.error("getSftpFile is error.", e);
                }

            } else {
                channelSftp.put(localPath, serverRelativePath, sftpTansferProgress);
                if (permission >= 0) {
                    String sPath = serverRelativePath + "/"
                            + localPath.substring(localPath.lastIndexOf(File.separator));
                    channelSftp.chmod(permission, sPath);
                }
            }
            logger.info("SftpTransfer.transferFile over");
        } catch (Exception e) {
            logger.error("Sftp Fail :" + account, e);
            throw e;
        }
    }

    static class GetSftpFile implements Callable<Boolean> {
        public GetSftpFile(ChannelSftp channelSftp, String serverRelativePath, String localPath) {
            super();
            _channelSftp = channelSftp;
            this.serverRelativePath = serverRelativePath;
            this.localPath = localPath;
        }

        private ChannelSftp _channelSftp;

        private String serverRelativePath, localPath;

        @Override
        public Boolean call() throws Exception {
            try {
                _channelSftp.get(serverRelativePath, localPath);
                return true;
            } catch (SftpException e) {
                logger.error("getSftpFile is failed:" + e);
                return false;
            }
        }
    }

    private static void createSession(SftpAccount account) throws Exception {
        createSession0(account, 3);
    }

    private static void createSession0(SftpAccount account, int count) throws Exception {
        JSch jsch = new JSch();
        Lock lockOfSession = getLock(account);
        lockOfSession.lock();
        Session realSession = null;
        try {
            realSession = ip2session.get(account.getIpAddress());
            if (isConnected(account)) {
                logger.info("Not need to reconnect for ip:" + account.getIpAddress());
                return;
            }

            realSession = createSftpSession(account, jsch);
            ip2session.put(account.getIpAddress(), realSession);
            logger.info("new session created: ip:" + account.getIpAddress());
        } catch (JSchException e) {
            logger.error(
                    "try to connect to the sftp server failed" + account.getIpAddress() + ", the " + count + "th time.",
                    e);
            if (e.getMessage().equals("Auth fail") || e.getMessage().equals("Auth cancel")
                    || e.getMessage().startsWith("invalid privatekey") || e.getMessage().startsWith("USERAUTH fail")) {
                throw new Exception("", e);
            }

            if (count <= 1) {
                throw new Exception("SFTP_TRANSFER_FAIL", e);
            }
            createSession0(account, count - 1);

        } finally {
            lockOfSession.unlock();
        }
    }

    private static Session createSftpSession(SftpAccount account, JSch jsch) throws JSchException {
        Session realSession;
        realSession = jsch.getSession(account.getUserName(), account.getIpAddress(), account.getPort());
        if (null != account.getPublicKeyAuth() && account.getPublicKeyAuth()) {
            logger.info("use the public key to get sftp session");
            jsch.addIdentity(null, account.getIdentity().getBytes(), null, account
                    .getPassphrase().getBytes());
        } else if (account.getPassword() != null) {
            logger.info("use the default method to get sftp session");
            realSession.setPassword(account.getPassword());
        } else {
            logger.error("The sftp account error.");
        }
        Properties sshConfig = new Properties();
        sshConfig.put("StrictHostKeyChecking", "no");
        realSession.setConfig(sshConfig);
        realSession.connect(CONNECTION_TIMEOUT);

        return realSession;
    }

    private ChannelSftp getChannel(Session session, SftpAccount account, String encoding, int count,
        boolean brokenByException) throws Exception {
        if (session == null || !session.isConnected()) {
            createSession(account);
        }
        if (session == null) {
            throw new Exception("SFTP_TRANSFER_FAIL");
        }
        ChannelSftp channelSftp = null;
        try {
            channelSftp = (ChannelSftp) session.openChannel("sftp");
            channelSftp.connect(CONNECTION_TIMEOUT);
            ip2channel.put(account.getIpAddress() + "_" + brokenByException, channelSftp);
            logger.info(
                    "New channel sftp connect for device: " + account.getIpAddress() + ", param: " + brokenByException);
        } catch (JSchException e) {
            logger.error("Get sftp channel failed." + account.getIpAddress(), e);
            if (count <= 1) {
                throw new Exception("SFTP_TRANSFER_FAIL");
            }
            return getChannel(session, account, encoding, count - 1, brokenByException);
        }

        try {
            if (encoding != null && !"".equals(encoding)) {
                channelSftp.setFilenameEncoding(encoding);
            } else {
                channelSftp.setFilenameEncoding("GBK");
            }
        } catch (SftpException e) {
            try {
                channelSftp.setFilenameEncoding("UTF-8");
            } catch (SftpException e1) {
                logger.error("file name error.", e1);
            }
        }
        return channelSftp;
    }

    private ChannelSftp getChannel(Session session, SftpAccount account, String encoding, boolean brokenByException)
            throws Exception {
        ChannelSftp channel = ip2channel.get(account.getIpAddress() + "_" + brokenByException);
        if (channel != null && channel.isConnected()) {
            return channel;
        }
        return getChannel(session, account, encoding, 3, brokenByException);
    }

    private static boolean isConnected(SftpAccount account) {
        Session session = ip2session.get(account.getIpAddress());

        return session != null && session.isConnected();
    }

    /**
     * Check sftp connected
     * @param ip Device ip
     * @param port Device port
     * @return boolean
     */
    public static boolean isSftpCanConnected(String ip, int port) {
        boolean result = false;
        if (null == ip || "".equals(ip)) {
            logger.error("the param is empty");
            return result;
        }
        Socket socket = null;
        try {
            InetSocketAddress endPoint = new InetSocketAddress(ip, port > 0 ? port : SftpTransfer.DEAFLUAT_PORT);
            socket = new Socket();
            socket.connect(endPoint, 10000);
            result = true;
            logger.info("the sftp ip " + ip + " is connected ");
        } catch (IOException e) {
            logger.error("the sftp ip " + ip + " can not be connected ", e);
            result = false;
        } finally {
            if (socket != null) {
                try {
                    socket.close();
                } catch (IOException e) {
                    logger.warn("close socket failed." + ip + ":" + port, e);
                }
            }
        }

        return result;
    }

    /**
     * Check sftp connected
     * @param account Sfto account
     * @return boolean
     * @throws Exception
     */
    public static boolean isSftpCanConnected(SftpAccount account) throws Exception {
        logger.info("try use the new connection to ip: " + account.getIpAddress());
        boolean result = false;
        JSch jsch = new JSch();
        Session sshSession = null;
        if (!account.validateSftpAccount(account)) {
            logger.error("failed to validateSftpAccount");
            return result;
        }
        try {
            sshSession = createSftpSession(account, jsch);
            result = true;
            logger.info("the sftp ip " + account.getIpAddress() + " connected ");
        } catch (JSchException e) {
            logger.error("the sftp Auth failed for ip " + account.getIpAddress() + " can not connected ", e);
            throw new Exception("SFTP_USER_OR_PASSWORD_ERROR", e);
        } finally {
            closeSession(sshSession);
        }
        return result;
    }

    private static void closeSession(Session sshSession) {
        try {
            if (sshSession != null) {
                sshSession.disconnect();
            }
        } catch (Exception e) {
            logger.error("Close Sftp Error!" + e);
        }
    }

    /**
     * Get host key
     * @param account Sftp account
     * @return Host key
     * @throws Exception
     */
    public static String getHostKey(SftpAccount account) throws Exception {
        logger.info("try to get hostKey");
        JSch jsch = new JSch();
        Session sshSession = null;
        String fingerPrint = null;
        if (!account.validateSftpAccount(account)) {
            return fingerPrint;
        } else {
            createSession(account);
            sshSession = ip2session.get(account.getIpAddress());
            HostKey hk = sshSession.getHostKey();
            fingerPrint = hk.getFingerPrint(jsch);
            logger.info("success to get hostKey");
            return fingerPrint;
        }
    }

    /**
     * Get sftp session
     * @param ip Device ip
     * @return Sftp session
     */
    public static Session getSftpSession(String ip) {
        Session session = ip2session.get(ip);
        if (session != null && session.isConnected()) {
            return session;
        }
        return null;
    }

    /**
     * Close sftp session
     * @param ip Device ip
     */
    public static void closeSftpSession(String ip) {
        Session session = ip2session.get(ip);
        closeSession(session);
        ip2session.remove(ip);
        logger.info("closeSftpSession ip :" + ip);

        closeChannel(ip);
    }
}

class SftpLogger implements Logger {
    private static org.apache.log4j.Logger apachelogger = org.apache.log4j.Logger.getLogger(SftpLogger.class);

    /**
     * check enabled
     * @param i i
     * @return boolean
     */
    public boolean isEnabled(int i) {
        return true;
    }

    /**
     * log
     * @param i i
     * @param s s
     */
    public void log(int i, String s) {
        StackTraceElement[] stacks = Thread.currentThread().getStackTrace();
        int number = stacks.length;
        int index = 0;

        for (; index < number; index++) {
            StackTraceElement element = stacks[index];
            if (element.getClassName().equals(this.getClass().getName()) && element.getMethodName().equals("log")) {
                index++;
                break;
            }
        }

        String str = builderLogs(s, stacks, number, index);
        switch (i) {
            case Logger.WARN:
                apachelogger.warn(str);
                break;
            case Logger.ERROR:
                apachelogger.error(str);
                break;
            case Logger.FATAL:
                apachelogger.fatal(str);
                break;
            default:
                break;
        }
    }

    private String builderLogs(String s, StackTraceElement[] stacks, int number, int index) {
        StringBuffer str = new StringBuffer("jsch>>>");
        if (index < number) {
            StackTraceElement tmp = stacks[index];
            String className = tmp.getClassName();
            int lineNum = tmp.getLineNumber();
            String methodName = tmp.getMethodName();

            str.append('[' + className + "]-[" + methodName + "]- [" + lineNum + ']').append("/r/n");
        }
        str.append(s);
        return str.toString();
    }

}
