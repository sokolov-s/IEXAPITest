project(Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_UTILS_INCLUDE_DIR}
    ${COMMON_DATA_INCLUDE_DIR}
    ${CONFIG_INCLUDE_DIR}
    ${RPC_INCLUDE_DIR}
    ${GRPC_SERVICE_GEN_DIR}
    )

set(FTESTS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/ftests.cpp
    )

add_executable(ftests
    ${FTESTS_SRC}
    )

target_link_libraries(ftests
    pthread
    gtest gtest_main gmock gmock_main
    PeakworkRPC
    PeakworkCommonUtils
    PeakworkConfig
    PeakworkCommonData
    PeakworkDB
    )
