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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.zip.GZIPInputStream;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

public final class TarUtils {
    private static final Logger logger;

    private static final int TOOBIG = 5 * 100 * 1024 * 1024; // 100MB

    private static final int TOOMANY = 1024; // max number of files

    private TarUtils() {
    }

    static {
        LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
        try {
            String filePath = LogUtil.LOG_XML_PATH;
            File file = new File(filePath);
            context.setConfigLocation(file.toURI());
        } catch (Exception e) {
            e.getMessage();
        }
        logger = context.getLogger(TarUtils.class.getName());
    }

    /**
     * Extract file
     * @param file File
     * @param outputDir Output directory
     * @param gen File name generater
     * @return content
     * @throws Exception
     */
    public static String extract(File file, String outputDir, FileNameGenerater gen) throws Exception {
        logger.info("begin extract.");
        String retName = "";
        TarArchiveInputStream in = null;
        BufferedInputStream bis = null;
        List<BufferedOutputStream> boses = new ArrayList<BufferedOutputStream>();
        FileInputStream fileInputStream = null;
        FileOutputStream outputStream = null;
        GZIPInputStream inputStream = null;

        try {
            fileInputStream = new FileInputStream(file);
            inputStream = new GZIPInputStream(fileInputStream);
            InputStream is = file.getName().toLowerCase(Locale.getDefault()).endsWith(".tgz") ? inputStream
                    : new BZip2CompressorInputStream(fileInputStream);
            in = new TarArchiveInputStream(is);
            // FIX FOR PERFORMANCE BY G
            byte[] tmpArray = new byte[1024 * 8];
            TarArchiveEntry entry;
            int entries = 0;
            while ((entry = in.getNextTarEntry()) != null) {
                if (logger.isDebugEnabled()) {
                    logger.debug("Extracting: " + entry);
                }
                if (ReportUtil.validateDir(outputDir)) {
                    throw new Exception("dir is invalid");
                }
                File tempFileFolder = new File(outputDir);
                boolean result = true;
                if (!tempFileFolder.exists()) {
                    result = tempFileFolder.mkdirs();
                }
                if (!result) {
                    throw new Exception("create file dir fail.");
                }
                if (ReportUtil.validateDir(outputDir + File.separator + file.getName() + System.currentTimeMillis())) {
                    throw new Exception("dir is invalid");
                }
                File tempFile = new File(outputDir + File.separator + gen.getUnzippedFileName(entry.getName()));
                retName = checkFile(tempFile);

                bis = new BufferedInputStream(in);
                outputStream = new FileOutputStream(tempFile);
                BufferedOutputStream bos = new BufferedOutputStream(outputStream);
                boses.add(bos);
                entries++;
                exceedMaxSize(bis, tmpArray, bos);
                exceedMaxNumber(entries);
                bos.flush();
            }
            return retName;
        } catch (Exception e) {
            throw new Exception(e);
        } finally {
            IOUtils.closeQuietly(bis, in, inputStream, fileInputStream);
            for (BufferedOutputStream b : boses) {
                IOUtils.closeQuietly(b);
            }
            IOUtils.closeQuietly(outputStream);
        }
    }

    /**
     * Exceed max number
     * @param entries Entries
     */
    public static void exceedMaxNumber(int entries) {
        if (entries > TOOMANY) {
            throw new IllegalStateException("Too many files to unzip.");
        }
    }

    /**
     * Extract file
     * @param file File
     * @param outputDir Output directory
     * @return content
     * @throws Exception
     */
    public static String extract(File file, String outputDir) throws Exception {
        return extract(file, outputDir, new AppendTimeAfterOrginalName());
    }

    private static void exceedMaxSize(BufferedInputStream bis, byte[] tmpArray, BufferedOutputStream bos)
            throws IOException {
        int count;
        /*
         * Write the files to the disk, but ensure that the file is not insanely
         * big
         */
        int total = 0;
        while ((count = bis.read(tmpArray)) != -1) {
            bos.write(tmpArray, 0, count);
            total += count;
            if (total > TOOBIG) {
                throw new IllegalStateException("File being extracted is huge.");
            }
        }
    }

    /**
     * Check file
     * @param tempFile Temp file
     * @return name
     */
    private static String checkFile(File tempFile) {
        String retName = tempFile.getPath();
        if (tempFile.exists()) {
            boolean b = tempFile.delete();
            if (!b) {
                throw new SecurityException();
            }
        }
        return retName;
    }

    /**
     * Zip
     * @param inputFilePath Input file path
     * @param zipFilePath Zip file path
     * @param baseName Base name
     */
    public static void zip(String inputFilePath, String zipFilePath, String baseName) {
        if (!ZipUtil.zip(inputFilePath, zipFilePath, baseName)) {
            logger.error("zip failed." + inputFilePath + " zip:" + zipFilePath + " base:" + baseName);
        }
    }

}
