project(PeakworkDB CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DAEMON_ROOT_DIR}
    ${PLOG_INCLUDE_DIR}
    ${CONFIG_INCLUDE_DIR}
    )

set(SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/db_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/db_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/itable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/itable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table_companies.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table_companies.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table_prices.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table_prices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table_db_version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table_db_version.cpp
    )

set(DB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path to the common src files" FORCE)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
target_link_libraries(${PROJECT_NAME} sqlite3 SQLiteCpp pthread dl)
