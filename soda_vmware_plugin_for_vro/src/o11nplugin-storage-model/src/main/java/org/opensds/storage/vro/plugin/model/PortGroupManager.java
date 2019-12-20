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

import org.opensds.storage.conection.rest.common.OceanStorOperation;
import org.opensds.storage.conection.rest.common.OperationNamesEnum;
import org.opensds.storage.conection.rest.common.OperationUtil;
import org.opensds.storage.conection.rest.exception.RestException;
import org.apache.log4j.Logger;

import java.util.List;
import java.util.Map;

public class PortGroupManager {
    private static final Logger log = Logger.getLogger(PortGroupManager.class);

    /**
     * Create Port group
     * @param storageArrayInfo OceanStor info
     * @param portName Port name
     * @param Description Port deciption
     * @throws RestException
     */
    public void createPortGroup(StorageArrayInfo storageArrayInfo, String portName, String Description)
        throws RestException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_PORT_GROUP);
        operation.putOperationData(RestContains.NAME, portName);
        operation.putOperationData(RestContains.DESCRIPTION, Description);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete port group
     * @param storageArrayInfo OceanStor info
     * @param portGroupName Port group name
     * @throws StorageCommonException
     * @throws RestException
     */
    public void deletePortGroupById(StorageArrayInfo storageArrayInfo, String portGroupName)
        throws StorageCommonException, RestException {
        PortGroup portGroup = getPortGroupIdByName(storageArrayInfo, portGroupName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_PORT_GROUP);
        operation.putOperationData(RestContains.ID, portGroup.getReal_id());
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Create port and port group association
     * @param storageArrayInfo OceanStor info
     * @param portGroupName Port group name
     * @param portMAC Port MAC address
     * @throws StorageCommonException
     * @throws RestException
     */
    public void createPortGroupAssociation(StorageArrayInfo storageArrayInfo, String portGroupName, String portMAC)
        throws StorageCommonException, RestException {
        Port port = getPortByMAC(storageArrayInfo, portMAC);
        PortGroup portGroup = getPortGroupIdByName(storageArrayInfo, portGroupName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.CREATE_PORT_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, portGroup.getReal_id());
        operation.putOperationData(RestContains.ASSOCIATEOBJID, port.getReal_id());
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_ETH_PORT);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    /**
     * Delete port and port group association
     * @param storageArrayInfo OceanStor info
     * @param portGroupName Port group name
     * @param portMAC Port MAC address
     * @throws StorageCommonException
     * @throws RestException
     */
    public void deletePortGroupAssociation(StorageArrayInfo storageArrayInfo, String portGroupName, String portMAC)
        throws StorageCommonException, RestException {
        Port port = getPortByMAC(storageArrayInfo, portMAC);
        PortGroup portGroup = getPortGroupIdByName(storageArrayInfo, portGroupName);
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.DELETE_PORT_GROUP_ASSOCIATE);
        operation.putOperationData(RestContains.ID, portGroup.getReal_id());
        operation.putOperationData(RestContains.ASSOCIATEOBJID, port.getReal_id());
        operation.putOperationData(RestContains.ASSOCIATEOBJTYPE, RestContains.ASSOCIATEOBJTYPE_ETH_PORT);
        OperationUtil.performAction(storageArrayInfo, operation);
    }

    private PortGroup getPortGroupIdByName(StorageArrayInfo storageArrayInfo, String name) throws RestException,
        StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_PORT_GROUP_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find Port Group:" + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one Port Group for Name: " + name);
        }
        PortGroup portGroup = (PortGroup) ObjectsBuilder.INSTANCE.buildObject(list.get(0), PortGroup.class,
                storageArrayInfo);
        return portGroup;
    }

    private Port getPortIdByName(StorageArrayInfo storageArrayInfo, String name) throws RestException,
        StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_PORT_BY_NAME);
        operation.putOperationData(RestContains.FILTER_KEY, RestContains.FILTER_NAME_PARAMETER + name);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find Port:" + name);
        }
        if (list.size() > 1) {
            throw new StorageCommonException("there is not only one Port for Name: " + name);
        }
        Port port = (Port) ObjectsBuilder.INSTANCE.buildObject(list.get(0), Port.class,
                storageArrayInfo);
        return port;
    }

    private Port getPortByMAC(StorageArrayInfo storageArrayInfo, String portMac) throws RestException,
        StorageCommonException {
        OceanStorOperation operation = new OceanStorOperation();
        operation.setOperationName(OperationNamesEnum.GET_PORT_BY_NAME);
        List<Map<String, String>> list = OperationUtil.performAction(storageArrayInfo, operation);
        if (list.size() == 0) {
            throw new StorageCommonException("can not find Port:" + portMac);
        }
        Port port = null;
        for (Map<String, String> map : list) {
            Port port1 = (Port) ObjectsBuilder.INSTANCE.buildObject(map, Port.class,
                    storageArrayInfo);
            if (portMac.trim().equals(port1.getMacaddress().trim())) {
                port = port1;
                break;
            }
        }
        if (null == port) {
            throw new StorageCommonException("The MAC Address Error,please Enter a correct MAC Address");
        }
        return port;
    }

}
