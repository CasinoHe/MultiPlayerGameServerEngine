# build protobuf locally, avoid using the system protobuf

# do not build tests and protoc
cmake_policy(SET CMP0077 NEW)
set(protobuf_BUILD_TESTS OFF)
set(protobuf_BUILD_PROTOC_BINARIES OFF)

# build static library
set(BUILD_SHARED_LIBS OFF)

# build protobuf
add_subdirectory(src/common/protobuf/cmake)