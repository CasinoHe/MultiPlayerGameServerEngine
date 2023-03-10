# cmake for zlib library

# use cmake for zlib
add_subdirectory(src/common/zlib)
include_directories(${CMAKE_BINARY_DIR}/src/common/zlib)

# msvc will add build_type directory to the library path
if (MSVC)
	set(ZLIB_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/src/common/zlib/${CMAKE_BUILD_TYPE})
else()
	set(ZLIB_LIBRARY_DIRS ${CMAKE_BINARY_DIR}/src/common/zlib/)
endif()

message(STATUS "ZLIB_LIBRARY_DIRS: ${ZLIB_LIBRARY_DIRS}")

set(ZLIB_FOUND TRUE)
set(ZLIB_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/src/common/zlib ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)
set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/src/common/zlib ${MULTIPLAYER_SERVER_ROOT_DIR}/common/zlib)

# protobuf need ZLIB_LIBRARY macros
# there are different names for the library on different platforms
if (MSVC)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set(ZLIB_LIBRARY
			${ZLIB_LIBRARY_DIRS}/zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}
		)
	else()
		set(ZLIB_LIBRARY
			${ZLIB_LIBRARY_DIRS}/zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}
		)
	endif()
else()
	set(ZLIB_LIBRARY
		${ZLIB_LIBRARY_DIRS}/libz${CMAKE_STATIC_LIBRARY_SUFFIX}
	)
endif()