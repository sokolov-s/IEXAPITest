project(PeakworkCommonData CXX)
cmake_minimum_required(VERSION 2.8)

file(GLOB COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB COMMON_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DAEMON_ROOT_DIR}
    ${PLOG_INCLUDE_DIR}
    )

set(COMMON_SRC_LIST
    ${COMMON_SRC}
    ${COMMON_HEADER}
)

set(COMMON_DATA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Path to the common data header files" FORCE)
set(COMMON_DATA_SRC "${COMMON_SRC_LIST}" CACHE STRING "Path to the common data src files" FORCE)

add_library(${PROJECT_NAME} SHARED ${COMMON_HEADER_LIST} ${COMMON_SRC_LIST})
target_link_libraries(${PROJECT_NAME})
