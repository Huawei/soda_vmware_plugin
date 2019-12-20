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

package org.opensds.storage.vro.plugin.model;

import java.util.List;
import java.util.Objects;
import java.util.Vector;

public class StorageArrayInfo extends BaseArrayInfo {
    @Override
    public boolean equals(Object obj) {
        if (obj instanceof StorageArrayInfo) {
            StorageArrayInfo storageArrayInfo = (StorageArrayInfo) obj;
            if (storageArrayInfo.getId().equals(getId())) {
                return true;
            }
        }
        return false;
    }

    @Override
    public int hashCode() {
        return Objects.hash(super.hashCode(), productSN, checkCert, candidateIpList, needSwithIp, fsSnapshots,
                filesystems, nfsShares, cifsShares, fCInitiators, lunGroups, hostList, hostGroupList, lunList,
                lunsnapshots, iSCSIInitiators, replicationConsistentGroups);
    }

    private String productSN;

    private boolean checkCert = false;

    private String candidateIpList;

    private boolean needSwithIp = false;

    public boolean isNeedSwithIp() {
        return needSwithIp;
    }

    public void setNeedSwithIp(boolean needSwithIp) {
        this.needSwithIp = needSwithIp;
    }

    public String getCandidateIpList() {
        return candidateIpList;
    }

    public void setCandidateIpList(String candidateIpList) {
        this.candidateIpList = candidateIpList;
    }

    private List<FilesystemSnapshot> fsSnapshots = new Vector<FilesystemSnapshot>();

    private List<Filesystem> filesystems = new Vector<Filesystem>();

    private List<NFSShare> nfsShares = new Vector<NFSShare>();

    private List<CIFSShare> cifsShares = new Vector<CIFSShare>();

    private List<FCInitiator> fCInitiators = new Vector<FCInitiator>();

    private List<LUNGroup> lunGroups = new Vector<LUNGroup>();

    private List<Host> hostList = new Vector<Host>();

    private List<HostGroup> hostGroupList = new Vector<HostGroup>();

    private List<LUN> lunList = new Vector<LUN>();

    private List<LUNSnapshot> lunsnapshots = new Vector<LUNSnapshot>();

    private List<iSCSIInitiator> iSCSIInitiators = new Vector<iSCSIInitiator>();

    private List<RemoteReplicationConsistencyGroup> replicationConsistentGroups =
            new Vector<RemoteReplicationConsistencyGroup>();

    @Override
    public void clearCache() {
        lunList.clear();
        hostList.clear();
        hostGroupList.clear();
        lunsnapshots.clear();
        fsSnapshots.clear();
        getLuncopys().clear();
        filesystems.clear();
        nfsShares.clear();
        cifsShares.clear();
        fCInitiators.clear();
        lunGroups.clear();
        iSCSIInitiators.clear();
        replicationConsistentGroups.clear();
    }

    public String getProductSN() {
        return productSN;
    }

    public void setProductSN(String productSN) {
        this.productSN = productSN;
    }

    public List<iSCSIInitiator> getiSCSIInitiators() {
        return iSCSIInitiators;
    }

    public void setiSCSIInitiators(List<iSCSIInitiator> iSCSIInitiators) {
        this.iSCSIInitiators = iSCSIInitiators;
    }

    public List<RemoteReplicationConsistencyGroup> getReplicationConsistentGroups() {
        return replicationConsistentGroups;
    }

    public void setReplicationConsistentGroups(List<RemoteReplicationConsistencyGroup> groups) {
        this.replicationConsistentGroups = groups;
    }

    public boolean getCheckCert() {
        return checkCert;
    }

    public void setCheckCert(boolean checkCert) {
        this.checkCert = checkCert;
    }

    public List<LUNGroup> getLunGroups() {
        return lunGroups;
    }

    public void setLunGroups(List<LUNGroup> lunGroups) {
        this.lunGroups = lunGroups;
    }

    public List<FCInitiator> getfCInitiators() {
        return fCInitiators;
    }

    public void setfCInitiators(List<FCInitiator> fCInitiators) {
        this.fCInitiators = fCInitiators;
    }

    public List<CIFSShare> getCifsShares() {
        return cifsShares;
    }

    public void setCifsShares(List<CIFSShare> cifsShares) {
        this.cifsShares = cifsShares;
    }

    public List<NFSShare> getNfsShares() {
        return nfsShares;
    }

    public void setNfsShares(List<NFSShare> nfsShares) {
        this.nfsShares = nfsShares;
    }

    public List<Filesystem> getFilesystems() {
        return filesystems;
    }

    public void setFilesystems(List<Filesystem> filesystems) {
        this.filesystems = filesystems;
    }

    public List<FilesystemSnapshot> getFsSnapshots() {
        return fsSnapshots;
    }

    public void setFsSnapshots(List<FilesystemSnapshot> fsSnapshots) {
        this.fsSnapshots = fsSnapshots;
    }

    public List<HostGroup> getHostGroupList() {
        return hostGroupList;
    }

    public void setHostGroupList(List<HostGroup> hostGroupList) {
        this.hostGroupList = hostGroupList;
    }

    public List<Host> getHostList() {
        return hostList;
    }

    public void setHostList(List<Host> hostList) {
        this.hostList = hostList;
    }

    public void clearAll() {
        lunList.clear();
    }

    public List<LUN> getLunList() {
        return lunList;
    }

    public void setLunList(List<LUN> lunList) {
        this.lunList = lunList;
    }

    public List<LUNSnapshot> getLunsnapshots() {
        return lunsnapshots;
    }

    public void setLunsnapshots(List<LUNSnapshot> lunsnapshots) {
        this.lunsnapshots = lunsnapshots;
    }

    public iSCSIInitiator getiSCSIInitiatorById(String id) {
        for (iSCSIInitiator iSCSIInitiator : iSCSIInitiators) {
            if (iSCSIInitiator.getId().equals(id)) {
                return iSCSIInitiator;
            }
        }
        return null;
    }

    public LUNSnapshot getLUNSnapshotById(String id) {
        for (LUNSnapshot lunSnapshot : lunsnapshots) {
            if (lunSnapshot.getId().equals(id)) {
                return lunSnapshot;
            }
        }
        return null;
    }

    public LUN getLUNById(String id) {
        for (LUN lun : lunList) {
            if (lun.getId().equals(id)) {
                return lun;
            }
        }
        return null;
    }

    public Host getHostById(String id) {
        for (Host host : hostList) {
            if (host.getId().equals(id)) {
                return host;
            }
        }
        return null;
    }

    public HostGroup getHostGroupById(String id) {
        for (HostGroup hostgroup : hostGroupList) {
            if (hostgroup.getId().equals(id)) {
                return hostgroup;
            }
        }
        return null;
    }

    public FilesystemSnapshot getFilesystemSnapshotById(String id) {
        for (FilesystemSnapshot filesystemSnapshot : fsSnapshots) {
            if (filesystemSnapshot.getId().equals(id)) {
                return filesystemSnapshot;
            }
        }
        return null;
    }

    public Filesystem getFilesystemById(String id) {
        for (Filesystem filesystem : filesystems) {
            if (filesystem.getId().equals(id)) {
                return filesystem;
            }
        }
        return null;
    }

    public NFSShare getNFSShareById(String id) {
        for (NFSShare nfsShare : nfsShares) {
            if (nfsShare.getId().equals(id)) {
                return nfsShare;
            }
        }
        return null;
    }

    public CIFSShare getCIFSShareById(String id) {
        for (CIFSShare cifsShare : cifsShares) {
            if (cifsShare.getId().equals(id)) {
                return cifsShare;
            }
        }
        return null;
    }

    public FCInitiator getFCInitiatorById(String id) {
        for (FCInitiator fcInitiator : fCInitiators) {
            if (fcInitiator.getId().equals(id)) {
                return fcInitiator;
            }
        }
        return null;
    }

    public LUNGroup getLUNGroupById(String id) {
        for (LUNGroup lunGroup : lunGroups) {
            if (lunGroup.getId().equals(id)) {
                return lunGroup;
            }
        }
        return null;
    }

    @Override
    public String getURL() {
        return "https://" + getHostName() + ":" + getPort() + "/deviceManager/rest";
    }

    @Override
    public String toString() {
        return "StorageArrayInfo [id=" + getId() + ", arrayName=" + getArrayName()
                + ", hostName=" + getHostName() + ", port=" + getPort()
                + ", candidateIpList=" + candidateIpList + ", checkCert="
                + getCheckCert() + ", needSwithIp=" + needSwithIp + ", productName="
                + getProductName() + ", productVersion=" + getProductVersion() + ", productModel="
                + getProductModel() + "]";
    }

    public StorageArrayInfo() {
        super();
    }

    public StorageArrayInfo(String id, String hostName, String port) {
        super();
        setId(id);
        setHostName(hostName);
        setPort(port);
    }
}
