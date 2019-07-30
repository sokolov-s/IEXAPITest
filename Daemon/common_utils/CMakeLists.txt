project(PeakworkCommonUtils CXX)
cmake_minimum_required(VERSION 2.8)

# Boost
find_package(Boost REQUIRED COMPONENTS system)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${PLOG_INCLUDE_DIR}
    )

file(GLOB COMMON_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB COMMON_HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(COMMON_UTILS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Path to the common src files" FORCE)

add_library(${PROJECT_NAME} SHARED ${COMMON_HEADER_LIST} ${COMMON_SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread curl ssl crypt)
