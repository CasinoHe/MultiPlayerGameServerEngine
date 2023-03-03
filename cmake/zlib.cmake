# cmake for zlib library

# use cmake for zlib
add_subdirectory(src/common/zlib)
include_directories(${CMAKE_BINARY_DIR}/src/common/zlib)

set(ZLIB_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/src/common/zlib/${CMAKE_BUILD_TYPE})
message(STATUS "ZLIB_LIBRARY_DIRS: ${ZLIB_LIBRARY_DIRS}")
set(ZLIB_FOUND TRUE)
set(ZLIB_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/src/common/zlib ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)
set(ZLIB_LIBRARIES 
	${ZLIB_LIBRARY_DIRS}/zlibd${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/src/common/zlib ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)
set(ZLIB_LIBRARY 
	${ZLIB_LIBRARY_DIRS}/zlibd${CMAKE_STATIC_LIBRARY_SUFFIX}
)