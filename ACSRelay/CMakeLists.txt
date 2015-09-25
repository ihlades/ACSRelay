cmake_minimum_required (VERSION 2.6)
project (ACSRelay)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(SOURCE_DIR "ACSRelay")

configure_file(
	"${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/cmake_config.h.in"
	"${PROJECT_BINARY_DIR}/${SOURCE_DIR}/cmake_config.h"
)

include_directories(${SOURCE_DIR})

set(project_SOURCES
	${SOURCE_DIR}/acsrelay.cpp
	${SOURCE_DIR}/cmdparams.cpp
	${SOURCE_DIR}/INIReader.cpp
	${SOURCE_DIR}/log.cpp
	${SOURCE_DIR}/main.cpp
	${SOURCE_DIR}/peerconnection.cpp
	${SOURCE_DIR}/tcpsocket.cpp
	${SOURCE_DIR}/udpsocket.cpp
)
list(SORT project_SOURCES)

add_executable(${PROJECT_NAME} ${project_SOURCES})
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++11 -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++11 -O3 -g")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O3")
