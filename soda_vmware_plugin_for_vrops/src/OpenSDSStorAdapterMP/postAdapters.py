#!/usr/bin/env python
# Copyright 2014-2015 VMware, Inc.  All rights reserved.

import sys
import subprocess
import getpass
import json
import time
import os
import re
import platform

from nagini import Nagini
from nagini import NaginiException
from os import chdir, getcwd


ADAPTERCONF_DIRECTORY = os.path.join(os.environ['VCOPS_BASE'], 'user', 'plugins', 'inbound', 'OpenSDSStorAdapter', 'conf')
OPSCLI_DIRECTORY = os.path.join(os.environ['VCOPS_BASE'], 'tools', 'vcopscli')

class AccessControlHelper:
    """
    Helper class for access control related action through RESTful API

    Attributes:
        client: Client object from python-binding of RESTful API
    """
    
    def __init__(self, client):
        """
        Init AccessControlHelper class with client.
        """
        self.client = client

    def get_traversal_specs(self, traversal_adapter_kind, traversal_resource_kind, traversal_name):
        """
        Retrieves the traversal specs described by this adapter installation
 
        Args:
            traversal_adapter_kind: The adapter kind of the traversal spec.
            traversal_resource_kind: The resource kind of the traversal spec.
            traversal_name: The name of the traversal spec.
        Returns:
            List of traversal specs for provided adapter kind and resource kind and traversal spec name.
        """
        traversalspecobjs = self.client.get_traversal_specs(adapterKind=traversal_adapter_kind, resourceKind=traversal_resource_kind, name=traversal_name)
        traversalspecs = json.dumps(traversalspecobjs, sort_keys=False,  indent=4, separators=(',', ': '))
        print("traversalspecs=" + traversalspecs)
        return traversalspecobjs

    def get_role_permissions(self, role_name):
        """
        Retrieves the list of privilege keys for an existing role.

        Args:
            role_name: The unique name of the role.
        Returns:
            List of permissions keys for the role.
        """
        roleobj = self.client.get_role_by_name(roleName=role_name)
        return roleobj['privilege-keys']


    def role_exists(self, role_name):
        """
        Returns true if a role with the role_name exists, False otherwise.

        Args:
            role_name: The unique name of the role.
        Returns:
            True or False based on existense of the role.
        """
        roleobjs = self.client.get_roles(roleName=role_name)
        for role in roleobjs['userRoles']:
            if (role['name'] == role_name):
                return True
        return False

    def create_user_role(self, role_name, role_description, role_display_name, privilege_keys):
        """
        Creates a user role using the role's name, description, display name and privilege keys

        Args:
            role_name: The name of the user role to be created.
            role_description: The descritpion of the user role to be created.
            role_display_name: The name of the user role displayed in UI.
            privilege_keys: a list of privilege keys.
        Returns:
            The unique name of created user role.
        Raises:
            Exception: Failed to create user role due to server error.
        """
        requestBody = {
            "name" : role_name,
            "description" : role_description,
            "displayName" : role_display_name,
            "links" : None,
            "privilege-keys" : privilege_keys,
            "system-created" : False
        }
        response = self.client.create_user_role(requestBody)
        return response['name']

    def get_user_group_id(self, user_group_name):
        """
        Retrieves the user group id for a user group name

        Args:
            user_group_name: The name of the user group.
        Returns:
            The id of the user group.
        """
        usergroupobjs = self.client.get_user_groups(name=user_group_name)
        for usergroup in usergroupobjs['userGroups']:
            if (usergroup['name'] == user_group_name):
                return usergroup['id']
        return None

    def create_user_group(self, usergroup_name, usergroup_desc, usergroup_role_permissions):
        """
        Creates a user group using the name and description and roles

        Args:
            usergroup_name: The name of the user group to be created.
            usergroup_desc: The descritpion of the user group to be created.
            usergroup_roles: The roles of the user group to be created.
        Returns:
            The unique id of created user group.
        Raises:
            Exception: Failed to create user group due to server error.
        """
        requestBody = {
            "name" : usergroup_name,
            "description" : usergroup_desc,
            "role-permissions" : usergroup_role_permissions
        }
        response = self.client.create_user_group(requestBody)
        return response['id']

class ContentInstaller:
    """
    Helper class for installing contents (e.g. dashboards, views, reports)

    Attributes:
        None
    """

    @staticmethod
    def isLinux():
        return re.search('Linux', platform.system(), re.IGNORECASE)

    @staticmethod
    def isWindows():
        return re.search('Windows', platform.system(), re.IGNORECASE)
    
    @staticmethod
    def check_force(argv):
        force = ""
        for i in range(0,len(argv)):
             if (argv[i] == "--force_content_update"):
                 if(argv[i+1].upper() == "TRUE"):
                     force = " --force"
                     break
        return force

    @staticmethod
    def construct_script_command(arguments):
        if ContentInstaller.isLinux():
            script_command = "$VMWARE_PYTHON_BIN vcops-cli.py "
        if ContentInstaller.isWindows():
            script_command = "%VMWARE_PYTHON_BIN% vcops-cli.py "
        script_with_args = script_command + arguments
        print('Constructed script command: "{0}"'.format(script_with_args))
        return script_with_args

    @staticmethod
    def run_opscli_command(script_with_args):
        # OPSCLI must be run from the OPSCLI directory.
        starting_dir = os.getcwd()
        os.chdir(OPSCLI_DIRECTORY)
        result = subprocess.call(script_with_args, shell=True)
        # Return to the original directory.
        os.chdir(starting_dir)
        if result != 0:
            print('Script command: "{0}" failed with exit code: {1}'.format(script_with_args, str(result)))
            return False
        return True

    @staticmethod
    def install_content(argv, content_type, content_name):
        # Install content.
        force = ContentInstaller.check_force(argv)
        content_file_path = os.path.join(ADAPTERCONF_DIRECTORY, content_type + "s", content_name)
        if(content_type == "dashboard"):
            script_command = ContentInstaller.construct_script_command(content_type + " import admin " + content_file_path + " --share all --set 0 --create" + force);
        elif(content_type == "report" or content_type == "view"):
            script_command = ContentInstaller.construct_script_command(content_type + " import " + content_file_path + force);
        else:
            print("Undefined content_type: " + content_type)
        return ContentInstaller.run_opscli_command(script_command)

    @staticmethod
    def set_defsummary(content_name, adapter_kind, resource_kind):
        # set default summary page
        content_file_path = os.path.join(ADAPTERCONF_DIRECTORY, "dashboards", content_name)
        script_command = ContentInstaller.construct_script_command("dashboard defsummary " + content_file_path +
            " --adapterType " + adapter_kind + " --objectType " + resource_kind);
        return ContentInstaller.run_opscli_command(script_command)

def main(argv):

    print("Post adapter install script started")
    print("len(argv)=" + str(len(argv)))
    sys.stdout.flush()
    if (len(argv) >= 2):
        token = argv[1]
    else:
        sys.exit('Usage: $VMWARE_PYTHON_BIN postAdapters.py [token]')

    arg=sys.argv
    token=argv[1].replace("--authToken=", "")

    # install dashboards/views/reports
    ContentInstaller.install_content(argv, "dashboard", "OpenSDS_Dashboards.json")
    ContentInstaller.install_content(argv, "report", "OpenSDS_Reports.xml")
    ContentInstaller.install_content(argv, "view", "OpenSDS_Views.xml")

    client = None
    try:
        user_group_storage_admin = 'StorageUserGroup-Admin'
        user_group_storage_readonly = 'StorageUserGroup-ReadOnly'
        user_group_storage_readonly_plus_actions = 'StorageUserGroup-ReadOnlyPlusActions'
        role_readonly_plus_actions = 'ReadOnlyPlusActions'

        # create client object and connect using token
        client = Nagini(host='localhost')
        client.set_auth_token(token)
        print("client connected to server.")

        # create an instance of the AccessControlHelper class
        access_control_helper = AccessControlHelper(client)

        # get the traversal specs
        traversal_adapter_kind = 'OpenSDSStorAdapter'
        traversal_resource_kind= 'Storage'
        traversal_name=' Storage and VM Filter'
        traversalspecobjs = access_control_helper.get_traversal_specs(traversal_adapter_kind, traversal_resource_kind, traversal_name)
        if (traversalspecobjs == None or len(traversalspecobjs) == 0):
            raise Exception("No travsersal spec found")
 
         # create a custom user role = ReadOnly plus permissions to run MyTestAdapter actions
         # first check if a role with the same name already exists

        if (access_control_helper.role_exists(role_readonly_plus_actions) == False):
             privilege_keys = access_control_helper.get_role_permissions("ReadOnly")
             action_permissions = ['cleanupTheVolume-MyTestAdapter-StorageSystem',
               'cleanupTheVolume-MyTestAdapter-Volume',
               'increaseTheVolumeCapacity-MyTestAdapter-StorageSystem',
               'increaseTheVolumeCapacity-MyTestAdapter-Volume',
               'decreaseTheVolumeCapacity-MyTestAdapter-StorageSystem',
               'decreaseTheVolumeCapacity-MyTestAdapter-Volume']
             privilege_keys.extend(action_permissions)
             access_control_helper.create_user_role(role_readonly_plus_actions, "ReadOnly plus Actions", "ReadOnly plus Actions", privilege_keys)
             print("successfully created role = %s ." %role_readonly_plus_actions)
        else:
             print("role = %s already exists." %role_readonly_plus_actions)
  
        # create 3 user groups, one for each user role-permission:
        # Role: The two system user roles "Administrator", "ReadOnly" and the one custom user role "ReadOnly plus Actions"
         # TraversalSpec Instance: Use an instance of "Storage and Volume Filter" traversal spec as part of role-permission
         # This would demonstrate access control both based on role permissions and TraversalSpec object access.
        ts_spec_instances = [
             {
                 "adapterKind": traversal_adapter_kind,
                 "resourceKind": traversal_resource_kind,
                 "name": traversal_name,
                 "resourceSelection": None,
                 "selectAllResources": True
             }
         ]
        # first check if user group with the same name already exists
        user_group_storage_admin_id = access_control_helper.get_user_group_id(user_group_storage_admin)
        if (user_group_storage_admin_id == None):
            usergroup_role_permissions = [
                {
                    "roleName": "Administrator",
                    "traversal-spec-instances": ts_spec_instances,
                    "allowAllObjects": False
                }
            ]
            access_control_helper.create_user_group(user_group_storage_admin, "Administrator users group", usergroup_role_permissions)
            print("successfully created usergroup = %s ." %user_group_storage_admin)
            user_group_storage_admin_id = access_control_helper.get_user_group_id(user_group_storage_admin)
        else:
            print("usergroup = %s already exists." %user_group_storage_admin)

        user_group_storage_readonly_id = access_control_helper.get_user_group_id(user_group_storage_readonly)
        if (user_group_storage_readonly_id == None):
            usergroup_role_permissions = [
                {
                    "roleName": "ReadOnly",
                    "traversal-spec-instances": ts_spec_instances,
                    "allowAllObjects": False
                }
            ]
            access_control_helper.create_user_group(user_group_storage_readonly, "Readonly users group", usergroup_role_permissions)
            print("successfully created usergroup = %s ." %user_group_storage_readonly)
            user_group_storage_readonly_id = access_control_helper.get_user_group_id(user_group_storage_readonly)
        else:
            print("usergroup = %s already exists." %user_group_storage_readonly)

        user_group_storage_readonly_plus_actions_id = access_control_helper.get_user_group_id(user_group_storage_readonly_plus_actions)
        if (user_group_storage_readonly_plus_actions_id == None):
            usergroup_role_permissions = [
                {
                    "roleName": role_readonly_plus_actions,
                    "traversal-spec-instances": ts_spec_instances,
                    "allowAllObjects": False
                }
            ]
            access_control_helper.create_user_group(user_group_storage_readonly_plus_actions, "Readonly plus actions users group", usergroup_role_permissions)
            print("successfully created usergroup = %s ." %user_group_storage_readonly_plus_actions)
            user_group_storage_readonly_plus_actions_id = access_control_helper.get_user_group_id(user_group_storage_readonly_plus_actions)
        else:
            print("usergroup = %s already exists." %user_group_storage_readonly_plus_actions)

        client.close()
        print("client closed.")
    except Exception as e:
        print("The post install script failed.")
        print(e)
        sys.exit(1)
    finally:
        if client:
            client.close()
    print("The post install script finished successfully.")

if __name__ == '__main__':
    main(sys.argv)
