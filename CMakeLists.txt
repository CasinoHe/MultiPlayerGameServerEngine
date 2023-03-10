cmake_minimum_required(VERSION 3.19)

project(MultiPlayerServer)

# export all compile commands for intellisense hinting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# default build type is debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# macos set rpath
if (APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	# set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
	# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# all options
option(USE_PROTOBUF "Use protobuf for network communication" ON)
option(USE_SPDLOG "Use spdlog for logging" ON)
option(USE_FMT "Use fmt for log formatting" ON)
option(USE_RAPIDJSON "Use rapidjson for json parsing" ON)
option(USE_BOOST_JSON_PARSER "Use boost json parser for json parsing" OFF)

# set project root directory
set(MULTIPLAYER_SERVER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# boost library is required
# build boost library locally, avoid version conflicts and macro conflicts, so static link it to avoid dll conflicts
# boost is built using bjam, so use externalproject_add to build it
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# first, set external build environment for all external build projects
set(THIRD_PARTIES_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/thirdparties/downloads)
set(THIRD_PARTIES_BUILD_DIR ${CMAKE_BINARY_DIR}/thirdparties/build)
set(THIRD_PARTIES_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparties/install)
# build boost library using externalproject_add
include(ExternalProject)

include(Boost)
# add boost include directories
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "include Boost directories: ${Boost_INCLUDE_DIRS}")

# zlib library is required
# build zlib library locally, avoid version conflicts
include(zlib)
include_directories(${ZLIB_INCLUDE_DIRS})
message(STATUS "include zlib directories: ${ZLIB_INCLUDE_DIRS}")

# project settings
include_directories(${MULTIPLAYER_SERVER_ROOT_DIR})
set(MULTIPLAYER_SERVER_MAJOR_SRCS 
	${MULTIPLAYER_SERVER_ROOT_DIR}/main.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/config/arg_parser.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/config/game_config.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/log/logger_manager.cpp
)
set(MULTIPLAYER_SERVER_NETWORK_SRC
	${MULTIPLAYER_SERVER_ROOT_DIR}/network/asio_server.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/network/asio_tcp_connection.cpp 
)
set(MULTIPLAYER_SERVER_GAME_SRC
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/entity.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/server_entity.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/client_entity.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/component.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/entity_factory.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/service/login_service.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/game_main.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/component/network_component.cpp
)

if (USE_PROTOBUF)
	# protobuf is required
	include(protobuf)
	include_directories(${Protobuf_INCLUDE_DIRS})
	message(STATUS "include Protobuf directories: ${Protobuf_INCLUDE_DIRS}")
endif()

if (USE_FMT)
	# fmt is required
	add_definitions(-DUSE_FMT)
	add_subdirectory(src/log/fmt)
	include_directories(src/log/fmt/include)
endif()

if (USE_SPDLOG)
	# spdlog is required
	if (USE_FMT)
	set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)
	endif()
	add_definitions(-DUSE_SPDLOG)
	add_subdirectory(src/log/spdlog)
	include_directories(src/log/spdlog/include)
	list(APPEND MULTIPLAYER_SERVER_MAJOR_SRCS ${MULTIPLAYER_SERVER_ROOT_DIR}/log/spdlog_logger_imp.cpp)
else()
endif()

if (USE_RAPIDJSON)
	# rapidjson is required
	add_definitions(-DUSE_RAPIDJSON)
	include_directories(src/common/rapidjson/include)
elseif(USE_BOOST_JSON_PARSER)
	# boost json parser is required
	add_definitions(-DUSE_BOOST_JSON_PARSER)
else ()
	message(FATAL_ERROR "No json parser is used")
endif()

add_executable(${PROJECT_NAME} ${MULTIPLAYER_SERVER_MAJOR_SRCS} ${MULTIPLAYER_SERVER_NETWORK_SRC} ${MULTIPLAYER_SERVER_GAME_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
if (MSVC)
	# msvc will link library automated
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
	target_link_directories(${PROJECT_NAME} PRIVATE ${Boost_LIBRARY_DIRS} ${ZLIB_INCLUDE_DIRS})
elseif(APPLE)
	# gcc and clang treat warning as error
	# because of boost lexical stream(use by program_options), so disable treat warning as error
	# error message boost 1.81.0:
	#  boost/lexical_cast/detail/converter_lexical_streams.hpp:297:21: error: 'sprintf' is deprecated: This function is provided for compatibility reasons only.  
	#  Due to security concerns inherent in the design of sprintf(3), it is highly recommended that you use snprintf(3) instead. [-Werror,-Wdeprecated-declarations]
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
else()
	# gcc and clang treat warning as error
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
endif()

# link neccessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE zlibstatic)
add_dependencies(${PROJECT_NAME} Boost zlibstatic zlib)

if (USE_PROTOBUF)
	# if protobuf is used, link it
	target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf protobuf::libprotobuf-lite) # protobuf::libprotoc
endif()

if(USE_FMT)
	# if fmt is used, link it
	target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

if (USE_SPDLOG)
	target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
endif()