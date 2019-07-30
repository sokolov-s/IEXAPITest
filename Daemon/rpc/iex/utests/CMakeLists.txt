project(IEX_Unit_Tests CXX)
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

set(UTESTS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/utests.cpp
    )

add_executable(iex_utests
    ${UTESTS_SRC}
    )

target_link_libraries(iex_utests
    gtest gtest_main gmock gmock_main
    PeakworkIEXClient
    PeakworkCommonUtils
    PeakworkConfig
    PeakworkCommonData
    curl
    ssl
    crypto
    )
