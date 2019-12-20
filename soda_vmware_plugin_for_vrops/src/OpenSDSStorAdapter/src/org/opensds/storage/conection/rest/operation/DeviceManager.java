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

package org.opensds.storage.conection.rest.operation;

import java.io.File;
import java.util.Map;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.LoggerContext;

import com.google.gson.JsonObject;
import org.opensds.storage.conection.rest.connection.RestManager;
import org.opensds.storage.conection.rest.constants.HttpMethodEnum;
import org.opensds.storage.conection.rest.constants.OperationConstants;
import org.opensds.storage.conection.rest.constants.OperationError;
import org.opensds.storage.conection.rest.domain.ConnectionData;
import org.opensds.storage.conection.rest.domain.RestResponse;
import org.opensds.storage.conection.rest.exception.RestException;
import org.opensds.storage.conection.rest.utils.ComposeUtils;
import org.opensds.storage.extracdata.util.LogUtil;

public class DeviceManager {
	private static final Logger LOG;

	private RestManager restManager;
	// = new RestManager();
	private ConnectionData connectionData = new ConnectionData();
	private boolean isLogin = false;

	static {
    	LoggerContext context = (org.apache.logging.log4j.core.LoggerContext) LogManager.getContext(false);
    	try {
    		String filePath =  LogUtil.LOG_XML_PATH;
    		File file = new File(filePath);
    		context.setConfigLocation(file.toURI());
		} catch (Exception e) {
			e.getMessage();
		}
    	LOG = context.getLogger(DeviceManager.class.getName());
    }
	/**
	 * Instantiates a new Device manager. use connection data to contain all
	 * connection info.
	 * 
	 * @param connectionData
	 *            the connection data
	 */
	public DeviceManager(ConnectionData connectionData) {
		this.connectionData = connectionData;
		if (connectionData.getKeyStoreFile() != null && connectionData.getKeyStoreFilePass() != null) {
			restManager = new RestManager(connectionData.getKeyStoreFile(), connectionData.getKeyStoreFilePass(),
					connectionData.isStrictCheckHostName());
		}
		if (connectionData.getCertificateFilePath() != null && connectionData.getCertificateFilePath().length() > 0) {
			restManager = new RestManager(connectionData.getCertificateFilePath(),
					connectionData.isStrictCheckHostName());
		}
		restManager.setHostURL(connectionData.getHostURL());
	}

	/**
	 * Instantiates a new Device manager.
	 *
	 * @param hostURL
	 *            the host url
	 * @param username
	 *            the username
	 * @param password
	 *            the password
	 * @param scope
	 *            the scope 0 local user 1 ldap user
	 */
	public DeviceManager(String hostURL, String username, String password, String scope) {
		connectionData.setHostURL(hostURL);
		connectionData.setUsername(username);
		connectionData.setPassword(password);
		connectionData.setScope(scope);
		restManager.setHostURL(hostURL);
	}

	/**
	 * Instantiates a new Device manager.
	 *
	 * @param hostURL
	 *            the host url
	 * @param username
	 *            the username
	 * @param password
	 *            the password
	 */
	public DeviceManager(String hostURL, String username, String password) {
		connectionData.setHostURL(hostURL);
		connectionData.setUsername(username);
		connectionData.setPassword(password);
		restManager.setHostURL(hostURL);
	}

	/**
	 * Login into the device , get the login ibaseToken and cookie and put them
	 * into the following request header
	 * 
	 * @throws RestException
	 */
	public void login() throws RestException {
		if (LOG.isInfoEnabled()) {
			LOG.info("start login... login data is :" + connectionData);
		}
		JsonObject requestBody = new JsonObject();
		requestBody.addProperty("username", connectionData.getUsername());
		requestBody.addProperty("password", connectionData.getPassword());
		requestBody.addProperty("scope", connectionData.getScope());
		if (LOG.isDebugEnabled()) {
			LOG.debug("request body is " + "{'username':" + connectionData.getUsername() + ",'scope:'"
					+ connectionData.getScope() + "}");
		}
		restManager.doLogin(OperationConstants.RestRequestConstant.REST_LOGIN_URI, requestBody.toString());
		isLogin = true;
		if (LOG.isInfoEnabled()) {
			LOG.info("login success.");
		}
	}

	/**
	 * Logout the device
	 */
	public void logout() {
		if (null != restManager) {
			try {
				LOG.info("start logout..");
				restManager.doLogout();
			} catch (RestException e) {
				LOG.warn("exception happened when logout:" + e);
			}
		}
		isLogin = false;
		LOG.info("logout success..");
	}

	/**
	 * Perform action to restful webservice.
	 *
	 * @param operation
	 *            the operation contains name and data
	 * @return the operation result
	 * @throws RestException
	 *             the rest exception
	 */
	public OperationResult performAction(OceanStorOperation operation) throws RestException {
		LOG.info("start perform action ," + operation);
		checkOperation(operation);
		if (!isLogin) {
			throw OperationError.REQUEST.PERFORM_ACTION_NOT_LOGIN_ERROR.newException("");
		}
		OperationResult operationResult = doRestRequest(operation.getOperationName(), operation.getOperationData());
		if (LOG.isInfoEnabled()) {
			LOG.info("after process , the operation result is " + operationResult);
		}
		return operationResult;
	}

	/**
	 * check if the request data is valid
	 * 
	 * @param operationData
	 * @throws RestException
	 */
	private void checkOperation(OceanStorOperation operationData) throws RestException {
		LOG.debug("start check operation name");
		if (!RestInterfaceConfs.isValidOperation(operationData.getOperationName())) {
			throw OperationError.REQUEST.REQUEST_OPERATION_NAME_INVALID.newException(operationData.
					getOperationName());
		}
		// TODO check mandatory field here, check filed valid here
	}

	/**
	 * do real Http request
	 * 
	 * @param operationName
	 * @param operationData
	 * @return the operation Result
	 */
	private OperationResult doRestRequest(String operationName, Map<String, String> operationData)
			throws RestException {
		/*
		 * 1.Get operationMethod,operationURL from configuration by
		 * operationName 1.Set operationURL ,Set requestJSONBody 2.Get result
		 * data errorCode errorDesc
		 */
		if (LOG.isInfoEnabled()) {
			LOG.info("start do operation request,operation " + operationName + "," + operationData);
		}
		String restMethod = RestInterfaceConfs.getRestMethod(operationName);
		String restRelativeUri = ComposeUtils.composeRelativeUri(operationName, operationData);
		String requestBody = ComposeUtils.composeJsonFromMap(operationData);
		if (LOG.isInfoEnabled()) {
			LOG.info("after compose url,the request is " + restMethod + ","
					+ restRelativeUri + "," + requestBody);
		}
		// do Http request
		RestResponse restResult = restManager.doRequest(HttpMethodEnum.valueOf(restMethod), restRelativeUri,
				requestBody);
		// make JsonObject to a return result Object
		return ComposeUtils.composeResultFromRest(restResult);
	}

	public void setSocketTimeout(int socketTimeout) {
		this.restManager.setSocketTimeout(socketTimeout);
	}

	public void setConnectTimeout(int connectTimeout) {
		this.restManager.setConnectTimeout(connectTimeout);
	}
}
