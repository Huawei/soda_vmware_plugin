CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(command)

# --------------------------------------------------------------------------------
# Build type
# --------------------------------------------------------------------------------
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall -g")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g")

    MESSAGE("---- Debug build ----")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

    MESSAGE("---- Release build ----")
ELSE()
    MESSAGE("---- Other build type  ----")
ENDIF()

# --------------------------------------------------------------------------------
# Compiler setting
# --------------------------------------------------------------------------------
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -march=native -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -march=native -g")
SET(ARCH_TYPE x64) # architecture type is x64

# Check C++11 or C++0x support
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX0X)	
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ADD_DEFINITIONS(-DCOMPILEDWITHC11)

	MESSAGE("---- Using flag -std=c++11 ----")
	
ELSEIF(COMPILER_SUPPORTS_CXX11)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	ADD_DEFINITIONS(-DCOMPILEDWITHC0X)

	MESSAGE("---- Using flag -std=c++0x ----")
ELSE()
	MESSAGE(FATAL_ERROR "---- The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler. ----")
ENDIF()

MESSAGE("---- ----------------------------------------------")
MESSAGE(${PROJECT_SOURCE_DIR})
MESSAGE("---- ----------------------------------------------")

# ----------------------------------------------------------------------------
# Variable CMAKE_LIBRARY_OUTPUT_DIRECTORY
# ----------------------------------------------------------------------------
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
#set(CMAKE_EXE_LINKER_FLAGS "-static")

# --------------------------------------------------------------------------------
# Add macro definition
# --------------------------------------------------------------------------------
ADD_DEFINITIONS(-DACE_NEEDS_FUNC_DEFINITIONS)
ADD_DEFINITIONS(-DLINUX)

# --------------------------------------------------------------------------------
# Set folder for short
# --------------------------------------------------------------------------------
SET(THIRD_PARTY_INCLUDE 
    ${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/curl-7.52.1/include
	${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/ACE-6.2.4/include
    ${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/jsoncpp-0.10.5/include
	${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/openssl-1.0.2j/include)
	
	
# --------------------------------------------------------------------------------
# add the header file
# --------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${THIRD_PARTY_INCLUDE}
	${PROJECT_SOURCE_DIR}/sra
	${PROJECT_SOURCE_DIR}/common
	${PROJECT_SOURCE_DIR}/common/xml_node
	${PROJECT_SOURCE_DIR}/common/cli
	${PROJECT_SOURCE_DIR}/common/os
	${PROJECT_SOURCE_DIR}/TlvCom
	${PROJECT_SOURCE_DIR}/CmdAdapter
    ${PROJECT_SOURCE_DIR}/securec)


# --------------------------------------------------------------------------------
# Target link directories
# --------------------------------------------------------------------------------
LINK_DIRECTORIES(
${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/jsoncpp-0.10.5/lib
${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/openssl-1.0.2j/lib
${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/curl-7.52.1/lib
${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/ACE-6.2.4/lib
${PROJECT_SOURCE_DIR}/../../../../third_party_groupware/build_scripts/build/libssh2-1.7.0/lib
${PROJECT_SOURCE_DIR}/securec/lib
${PROJECT_SOURCE_DIR}/TlvCom/lib)



# --------------------------------------------------------------------------------
# Link libraries find
# --------------------------------------------------------------------------------
SET(LINK_LIBRAYIES_INFO  
	TLVCom
	securec
	json
	ACE
	ACE_SSL
	curl
	ssh2
	ssl
	crypto)
	


# --------------------------------------------------------------------------------
# set Source files used 
# --------------------------------------------------------------------------------
SET(SRC_CMDADAPTER_LIST  
    ${PROJECT_SOURCE_DIR}/CmdAdapter/CmdAdapter.cpp
	${PROJECT_SOURCE_DIR}/CmdAdapter/CmdOperate.cpp
	${PROJECT_SOURCE_DIR}/CmdAdapter/CmdRESTFusionStorage.cpp
	${PROJECT_SOURCE_DIR}/CmdAdapter/CmdRESTAdapter.cpp
	${PROJECT_SOURCE_DIR}/CmdAdapter/RESTConn.cpp
	${PROJECT_SOURCE_DIR}/CmdAdapter/RESTPackage.cpp)

SET(SRC_COMMON_XMLSERIAL	
    ${PROJECT_SOURCE_DIR}/common/xmlserial/tinyxml.cpp
	${PROJECT_SOURCE_DIR}/common/xmlserial/tinyxmlerror.cpp
	${PROJECT_SOURCE_DIR}/common/xmlserial/tinyxmlparser.cpp
	${PROJECT_SOURCE_DIR}/common/xmlserial/xmlreader.cpp
	${PROJECT_SOURCE_DIR}/common/xmlserial/xmlserializable.cpp
	${PROJECT_SOURCE_DIR}/common/xmlserial/xmlwriter.cpp)	
	
SET(SRC_COMMON_XMLNODE 
	${PROJECT_SOURCE_DIR}/common/xml_node/array.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/arrays.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/cg_snapshot.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/commond_info.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/consistency_groups.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/device.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/device_sync.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/failover_results.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/identity.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/recovery_point.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/replicated_devices.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/replication_setting.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/snapshot.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/source_device.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/source_devices.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/target_device.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/target_devices.cpp
	${PROJECT_SOURCE_DIR}/common/xml_node/target_groups.cpp)
	
SET(SRC_COMMON_OS
	${PROJECT_SOURCE_DIR}/common/os/Commf.cpp
	${PROJECT_SOURCE_DIR}/common/os/Log.cpp
    )

SET(SRC_COMMON_CLI
	${PROJECT_SOURCE_DIR}/common/cli/rslt_parser.cpp)
	
SET(SRC_SRA_SOURCE
	${PROJECT_SOURCE_DIR}/sra/discover_arrays.cpp
	${PROJECT_SOURCE_DIR}/sra/discover_devices.cpp
	${PROJECT_SOURCE_DIR}/sra/failover.cpp
	${PROJECT_SOURCE_DIR}/sra/prepare_failover.cpp
	${PROJECT_SOURCE_DIR}/sra/query_capacity.cpp
	${PROJECT_SOURCE_DIR}/sra/query_connection.cpp
	${PROJECT_SOURCE_DIR}/sra/query_error.cpp
	${PROJECT_SOURCE_DIR}/sra/query_info.cpp
	${PROJECT_SOURCE_DIR}/sra/query_strings.cpp
	${PROJECT_SOURCE_DIR}/sra/query_syncstatus.cpp
	${PROJECT_SOURCE_DIR}/sra/restore.cpp
	${PROJECT_SOURCE_DIR}/sra/prepare_reverse.cpp
	${PROJECT_SOURCE_DIR}/sra/reverse_replication.cpp
	${PROJECT_SOURCE_DIR}/sra/ThreeDCLun.cpp
	${PROJECT_SOURCE_DIR}/sra/sra.cpp
	${PROJECT_SOURCE_DIR}/sra/sra_basic.cpp
	${PROJECT_SOURCE_DIR}/sra/sync_once.cpp
	${PROJECT_SOURCE_DIR}/sra/test_failover_start.cpp
	${PROJECT_SOURCE_DIR}/sra/test_failover_stop.cpp)
 
# --------------------------------------------------------------------------------
# EXECUTE FILE
# --------------------------------------------------------------------------------
ADD_EXECUTABLE(${PROJECT_NAME}   ${SRC_SRA_SOURCE}  
                                 ${SRC_COMMON_CLI}  
								 ${SRC_COMMON_OS} 
								 ${SRC_COMMON_XMLNODE} 
								 ${SRC_COMMON_XMLSERIAL}
								 ${SRC_CMDADAPTER_LIST})						 
 
# --------------------------------------------------------------------------------
# Target link libraries
# --------------------------------------------------------------------------------
 TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LINK_LIBRAYIES_INFO})