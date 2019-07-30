project(PeakworkDemon CXX)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb -O0")

set(DAEMON_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path to the daemon project sources" FORCE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Build project in DEBUG mode")
    add_definitions(-DLOG_LEVEL=5)
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -ggdb -O0")
else()
    message(STATUS "Build project in Release mode")
    add_definitions(-DLOG_LEVEL=2)
    add_definitions(-DNDEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

add_definitions(-DDAEMON_VERSION="1.0.0")

# Boost
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(common_utils)
add_subdirectory(config)
add_subdirectory(common_data)
add_subdirectory(db)
add_subdirectory(rpc)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${COMMON_UTILS_INCLUDE_DIR}
    ${COMMON_DATA_INCLUDE_DIR}
    ${CONFIG_INCLUDE_DIR}
    ${PLOG_INCLUDE_DIR}
    ${RPC_INCLUDE_DIR}
    ${GRPC_SERVICE_GEN_DIR}
    )

file(GLOB DAEMON_SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB DAEMON_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )

add_executable(${PROJECT_NAME}
    ${DAEMON_SRC_LIST}
    )

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    pthread
    PeakworkCommonUtils
    PeakworkConfig
    PeakworkCommonData
    PeakworkRPC
    PeakworkDB
    )

add_dependencies(${PROJECT_NAME} PeakworkRPC)
add_dependencies(${PROJECT_NAME} generate_grpc)

add_subdirectory(tests)
