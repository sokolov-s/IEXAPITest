project(PeakworkIEXClient CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${DAEMON_ROOT_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLOG_INCLUDE_DIR}
)

file(GLOB RPC_SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB RPC_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(${PROJECT_NAME} SHARED
    ${RPC_SRC_LIST}
    ${RPC_HEADER_LIST}
    )

target_link_libraries(${PROJECT_NAME}
    pthread
    )

set(CURL_CLIENT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR} ${GRPC_SERVICE_GEN_DIR}" CACHE STRING "Path to the common files" FORCE)

add_subdirectory(utests)
