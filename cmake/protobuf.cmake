# build protobuf locally, avoid using the system protobuf

# do not build tests and protoc
set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build protobuf tests")
set(protobuf_BUILD_PROTOC_BINARIES OFF CACHE BOOL "Build protoc binaries")
set(protobuf_BUILD_CONFORMANCE OFF CACHE BOOL "Build protobuf conformance tests")
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "Build protobuf examples")

# build static library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build protobuf shared libraries")

# build protobuf
add_subdirectory(src/common/protobuf/cmake)

# set(PRO_FOUND TRUE)
# set(ZLIB_INCLUDE_DIRS ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)
# set(ZLIB_LIBRARIES zlibstatic)
# set(ZLIB_INCLUDE_DIR ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)
# set(ZLIB_LIBRARY zlibstatic)