project(Peakwork)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1 -DSQLITE_THREADSAFE=1)

set(PLOG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include CACHE PATH "Path to the plog project sources" FORCE)

add_subdirectory(Daemon)
