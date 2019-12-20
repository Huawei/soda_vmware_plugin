CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(TLVCom)

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
	MESSAGE("---- Using flag -std=c++0x ----")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	
	ADD_DEFINITIONS(-DCOMPILEDWITHC0X)
ELSEIF(COMPILER_SUPPORTS_CXX11)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ADD_DEFINITIONS(-DCOMPILEDWITHC11)

	MESSAGE("---- Using flag -std=c++11 ----")
ELSE()
	MESSAGE(FATAL_ERROR "---- The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler. ----")
ENDIF()



MESSAGE("---- ----------------------------------------------")
MESSAGE(${PROJECT_SOURCE_DIR})
MESSAGE("---- ----------------------------------------------")

# ----------------------------------------------------------------------------
# Variable CMAKE_LIBRARY_OUTPUT_DIRECTORY
# ----------------------------------------------------------------------------
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# --------------------------------------------------------------------------------
# Add macro definition
# --------------------------------------------------------------------------------
ADD_DEFINITIONS(-DACE_NEEDS_FUNC_DEFINITIONS)

# --------------------------------------------------------------------------------
# Set folder for short
# --------------------------------------------------------------------------------

SET(THIRD_PARTY_INCLUDE 
    ${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/jsoncpp-0.10.5/include
	${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/openssl-1.0.2j/include
	${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/ACE-6.2.4/include
	${PROJECT_SOURCE_DIR}/../common/include
	${PROJECT_SOURCE_DIR}/../../../../../platform/huaweisecurec/include)


# --------------------------------------------------------------------------------
# add the header file
# --------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
${THIRD_PARTY_INCLUDE}
${PROJECT_SOURCE_DIR})


# --------------------------------------------------------------------------------
# Target link directories
# --------------------------------------------------------------------------------
LINK_DIRECTORIES(
${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/jsoncpp-0.10.5/lib
${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/openssl-1.0.2j/lib
${PROJECT_SOURCE_DIR}/../../../../../third_party_groupware/build_scripts/build/ACE-6.2.4/lib
${PROJECT_SOURCE_DIR}/../../../../../platform/huaweisecurec/lib)

# --------------------------------------------------------------------------------
# Link libraries 
# --------------------------------------------------------------------------------
SET(LINK_LIBRAYIES
    json
    ssl
	crypto
	ACE
	ACE_SSL
	huaweisecurec
)

# --------------------------------------------------------------------------------
# set Source files used 
# --------------------------------------------------------------------------------
SET(SRC_LIST  
    ${PROJECT_SOURCE_DIR}/om_connecter.cpp
    ${PROJECT_SOURCE_DIR}/msg_package.cpp)

# --------------------------------------------------------------------------------
# Target library
# --------------------------------------------------------------------------------
#生成动态库
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_LIST})
 
#生成静态库
#ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRC_LIST})
 
# --------------------------------------------------------------------------------
# Target link libraries
# --------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LINK_LIBRAYIES})