project(GRPCDaemonService CXX)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Proto file
set(PEAKWORK_SERVER_PROTO peakwork_server)

# Generated sources
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-gen)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
set(GRPC_SERVICE_GEN_DIR "${PROTO_GEN_DIR}" CACHE STRING "Path to the generated files of grpcservice project" FORCE)
set(GRPC_GEN_HEADERS
    ${PROTO_GEN_DIR}/${PEAKWORK_SERVER_PROTO}.pb.h
    ${PROTO_GEN_DIR}/${PEAKWORK_SERVER_PROTO}.grpc.pb.h
    )
set(GRPC_GEN_SRCS
    ${PROTO_GEN_DIR}/${PEAKWORK_SERVER_PROTO}.pb.cc
    ${PROTO_GEN_DIR}/${PEAKWORK_SERVER_PROTO}.grpc.pb.cc
    )
set(GRPC_GEN_FILES ${GRPC_GEN_HEADERS} ${GRPC_GEN_SRCS})

set(GRPC_SERVICE_FILES "${GRPC_GEN_FILES}" CACHE STRING "Generated files of grpcservice project" FORCE)

execute_process(
    COMMAND which grpc_cpp_plugin
    OUTPUT_VARIABLE GRPC_CPP_PLUGIN
    )
string(STRIP ${GRPC_CPP_PLUGIN} GRPC_CPP_PLUGIN)

execute_process(
    COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}/protos --grpc_out=${PROTO_GEN_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PEAKWORK_SERVER_PROTO}.proto
    COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}/protos --cpp_out=${PROTO_GEN_DIR} ${PEAKWORK_SERVER_PROTO}.proto
    RESULT_VARIABLE RETURN_VALUE
    )

if (NOT RETURN_VALUE EQUAL 0)
    message(FATAL_ERROR "Failed to get the dependencies")
endif()

add_custom_command(OUTPUT ${GRPC_SERVICE_FILES}
    COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}/protos --grpc_out=${PROTO_GEN_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PEAKWORK_SERVER_PROTO}.proto
    COMMAND protoc -I ${CMAKE_CURRENT_SOURCE_DIR}/protos --cpp_out=${PROTO_GEN_DIR} ${PEAKWORK_SERVER_PROTO}.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/protos/${PEAKWORK_SERVER_PROTO}.proto
    )
add_custom_target(generate_grpc DEPENDS "${GRPC_SERVICE_FILES}")

SET_SOURCE_FILES_PROPERTIES(${GRPC_SERVICE_FILES}
    PROPERTIES GENERATED TRUE
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLOG_INCLUDE_DIR}
    ${GRPC_SERVICE_GEN_DIR}
    ${DAEMON_ROOT_DIR}
    ${CONFIG_INCLUDE_DIR}
    )

set(GRPC_CLIENT_SERVER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/peakwork_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/peakwork_server.cpp
    ${GRPC_SERVICE_FILES}
    )

add_library(${PROJECT_NAME} SHARED
    ${GRPC_CLIENT_SERVER_SRCS}
    )

target_link_libraries(${PROJECT_NAME}
    pthread
    protobuf
    grpc++
    )

add_dependencies(${PROJECT_NAME} generate_grpc)


