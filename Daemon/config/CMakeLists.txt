project(PeakworkConfig CXX)
cmake_minimum_required(VERSION 2.8)

# Boost
find_package(Boost REQUIRED COMPONENTS system)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DAEMON_ROOT_DIR}
    ${PLOG_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    )

set(SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h  
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/config_daemon.h  
    ${CMAKE_CURRENT_SOURCE_DIR}/config_daemon.cpp  
    )

set(CONFIG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Path to the config src files" FORCE)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} PeakworkCommonUtils ${Boost_LIBRARIES})
