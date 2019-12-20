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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import org.apache.commons.io.FileUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import org.opensds.storage.extracdata.constant.IsmConstant;
import org.opensds.storage.extracdata.constant.IsmNumberConstant;
import org.opensds.storage.extracdata.constant.LegoNumberConstant;

public class FileUtil {
    static final int BUFFER = 8192;
    private static final Logger logger;

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        logger = context.getLogger(FileUtil.class.getName());
    }

    /**
     * Compress file
     * @param srcPathName Src path name
     * @param destZipFile Dest zip file
     * @return boolean
     */
    public static boolean compressFile(String srcPathName, String destZipFile) {
        if (destZipFile == null || srcPathName == null) {
            return false;
        }
        File file = new File(srcPathName);
        if (!file.exists()) {
            logger.warn(srcPathName + "not exits");
            return false;
        }
        try {
            return ZipUtil.zip(srcPathName, destZipFile, "");
        } catch (Exception e) {
            logger.error("zip compress file error", e);
            return false;
        }
    }

    /**
     * @param sourcefile sourcefile
     * @param targetFile targetFile
     * @return boolean suc or failed
     */
    public static boolean copyFile(File sourcefile, File targetFile) {
        if (logger.isDebugEnabled()) {
            logger.debug("copy src:" + sourcefile + "to:" + targetFile);
        }
        FileInputStream input = null;
        FileOutputStream out = null;
        try {
            input = new FileInputStream(sourcefile.getCanonicalPath());
            out = new FileOutputStream(targetFile.getCanonicalPath());
            byte[] b = new byte[IsmNumberConstant.NUM_1024 * IsmNumberConstant.FIVE];
            int len = 0;
            while ((len = input.read(b)) != -1) {
                out.write(b, 0, len);
            }
            out.flush();
            return true;
        } catch (FileNotFoundException e) {
            logger.error("copy file error，not exists", e);
            return false;
        } catch (IOException e) {
            logger.error("copy file error", e);
            return false;
        } finally {
            IOUtils.closeQuietly(out);
            IOUtils.closeQuietly(input);
        }
    }

    /**
     * @param temporyFilePath temporyFilePath
     * @param permission      permission
     */
    public static void chmodFile(String temporyFilePath, String permission) {
        if (ReportUtil.isLinux()) {
            try {
                Process p = Runtime.getRuntime().exec("chmod -R " + permission + temporyFilePath);
                Thread.sleep(LegoNumberConstant.ONE_HUNDRED_TWENTY_EIGHT);
                if (logger.isDebugEnabled()) {
                    logger.debug("Process checked." + p);
                }
            } catch (Exception e) {
                logger.error("exec chmod faild:" + temporyFilePath, e);
            }
        }
    }

    /**
     * @param temporyFilePath temporyFilePath
     */
    public static void chmodFile(String temporyFilePath) {
        chmodFile(temporyFilePath, "700");
    }

    /**
     * Get perfix
     * @param fileName File name
     * @return perfix
     */
    public static String getPrefix(String fileName) {
        if (VerifyUtil.isEmpty(fileName)) {
            throw new IllegalArgumentException("Invalid file:" + fileName);
        }
        int index = fileName.lastIndexOf(".");
        if (index >= 0) {
            return fileName.substring(0, index);
        }
        throw new IllegalArgumentException("Invalid file:" + fileName);
    }

    /**
     * Get suffix
     * @param fileName File name
     * @return suffix
     */
    public static String getSuffix(String fileName) {
        if (VerifyUtil.isEmpty(fileName)) {
            throw new IllegalArgumentException("Invalid file:" + fileName);
        }
        int index = fileName.lastIndexOf(".");
        if (index >= 0) {
            return fileName.substring(index + 1, fileName.length());
        }
        throw new IllegalArgumentException("Invalid file:" + fileName);
    }

    /**
     * Clear floder
     * @param folder Folder
     */
    public static void clearFloder(String folder) {
        clearFloder(folder, true);
    }

    /**
     * Clear floder
     * @param folder Folder
     * @param deleteSelf Delete self
     */
    public static void clearFloder(String folder, boolean deleteSelf) {
        if (folder == null) {
            return;
        }
        try {
            File dir = new File(folder);
            File[] fs = dir.listFiles();
            if (fs != null) {
                processNotEmpty(fs);
            }
            fs = dir.listFiles();
            if (VerifyUtil.isEmpty(fs) && deleteSelf) {
                if (logger.isInfoEnabled()) {
                    logger.info("delete dir:" + dir.getAbsolutePath() + " result: " + dir.delete());
                }
            }
        } catch (Exception e) {
            logger.error("DELETE FAILED." + folder, e);
        }
    }

    private static void processNotEmpty(File[] fs) {
        for (File f : fs) {
            if (f.isDirectory()) {
                clearFloder(f.getAbsolutePath(), true);
            } else {
                if (!f.delete()) {
                    if (logger.isInfoEnabled()) {
                        logger.info("DELETE failed." + f.getAbsolutePath());
                    }
                }
            }
        }
    }

    /**
     * Delete file
     * @param f file
     */
    public static void deleteFile(String f) {
        if (f != null) {
            try {
                File file = new File(f);
                boolean suc = false;
                if (file.exists()) {
                    suc = FileUtils.deleteQuietly(file);
                }
                if (!suc) {
                    logger.warn("delete failed." + f);
                }
            } catch (Exception e) {
                logger.error("error when delete " + f, e);
            }
        }
    }

    /**
     * Create folder
     * @param path path
     * @return boolean
     */
    public static boolean mkdirs(String path) {
        if (path == null) {
            return false;
        }
        try {
            File f = new File(path);
            if (f.exists()) {
                return f.isDirectory();
            } else {
                return f.mkdirs();
            }
        } catch (Exception e) {
            logger.error("error when create dir " + path, e);
            return false;
        }
    }

    /**
     * Delete public key file
     * @param name File name
     * @return boolean
     */
    public static boolean deletePublicKeyFile(String name) {
        boolean flag = false;
        if (name == null) {
            return false;
        }
        File file = FileUtils.getFile(name);
        if (null == file) {
            if (logger.isInfoEnabled()) {
                logger.info("fileUtil getFile return null, file=" + name);
            }
            return false;
        }
        if (file.isFile() && file.exists()) {
            boolean result = file.delete();
            if (result) {
                if (logger.isInfoEnabled()) {
                    logger.info("this public key file is deleted :" + name);
                }
            } else {
                if (logger.isInfoEnabled()) {
                    logger.info("this public key file delete fail :" + name);
                }
            }
            flag = true;
        } else {
            if (logger.isInfoEnabled()) {
                logger.info("this public key file delete fail:" + name);
            }
        }
        return flag;
    }

    /**
     * Delete file by ip
     * @param ip ip
     */
    public static void deleteFileByIp(String ip) {
        String ipStr = "";
        if (AddressUtil.isValidIPV4(ip)) {
            ipStr = ip.replaceAll("\\.", "_");
        }
        if (AddressUtil.isValidIPV6(ip)) {
            ipStr = ip.replaceAll(":", "_");
        }
        String publicDir = System.getProperty("user.dir") + File.separator + IsmConstant.PUBLIC_KEY;
        try {
            File file = FileUtils.getFile(publicDir);
            if (null == file) {
                if (logger.isInfoEnabled()) {
                    logger.info("fileUtil getFile return null, file=" + publicDir);
                }
                return;
            }
            File[] files = file.listFiles();
            if (null == files) {
                if (logger.isInfoEnabled()) {
                    logger.info("this public key dir not exit :" + publicDir);
                }
                return;
            }
            for (File f : files) {
                String fn = f.getName();
                int index = fn.lastIndexOf("_");
                if (index < 0) {
                    continue;
                }
                String ipString = fn.substring(0, index);
                if (ipString.equals(ipStr) && f.isFile() && f.exists()) {
                    FileUtil.deleteFile(f.getAbsolutePath());
                }
            }

        } catch (Exception e) {
            if (logger.isInfoEnabled()) {
                logger.info("this public key file delete fail:");
            }
        }
    }

    /**
     * Read file
     * @param fileName File name
     * @return File name
     */
    public static String readFile(String fileName) {
        String str = "";
        if (null == fileName || fileName.isEmpty()) {
            logger.error("Param error: null or empty");
            return str;
        }

        BufferedReader bf = null;
        try {
            bf = new BufferedReader(new FileReader(fileName));

            String content = "";
            StringBuilder sb = new StringBuilder();
            while (null != content) {
                content = bf.readLine();
                if (content == null) {
                    break;
                }
                sb.append(content).append('\n');
            }

            str = sb.toString();
        } catch (FileNotFoundException e) {
            logger.error("readFile error. file not found", e);
        } catch (IOException e) {
            logger.error("IOException:", e);
        } finally {
            IOUtils.closeQuietly(bf);
        }
        return str;
    }

    /**
     * Write file
     * @param fileName File name
     * @param content Content
     */
    public static void writeFile(String fileName, String content) {
        if (null == fileName || fileName.isEmpty() || null == content) {
            logger.error("Param error: null or empty");
            return;
        }

        BufferedWriter bw = null;
        try {
            bw = new BufferedWriter(new FileWriter(fileName));
            bw.write(content);
        } catch (FileNotFoundException e) {
            logger.error("writeFile error. file not found", e);
        } catch (IOException e) {
            logger.error("IOException:", e);
        } finally {
            closeBw(bw);
        }
    }

    private static void closeBw(BufferedWriter bw) {
        if (null != bw) {
            try {
                bw.close();
            } catch (IOException e) {
                logger.error("IOException: while close the buffer write", e);
            }
        }
    }
}
