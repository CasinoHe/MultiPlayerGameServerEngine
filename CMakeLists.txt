cmake_minimum_required(VERSION 3.19)

project(MultiPlayerServer)

# export all compile commands for intellisense hinting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# default build type is debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# all options
option(USE_PROTOBUF "Use protobuf for network communication" OFF)
option(USE_SPDLOG "Use spdlog for logging" ON)
option(USE_FMT "Use fmt for log formatting" ON)
option(USE_RAPIDJSON "Use rapidjson for json parsing" OFF)
option(USE_BOOST_JSON_PARSER "Use boost json parser for json parsing" ON)

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
# add_subdirectory(src/common/zlib)
# include_directories(${ZLIB_INCLUDE_DIRS})
# message(STATUS "include zlib directories: ${ZLIB_INCLUDE_DIRS}")

# project settings
include_directories(${MULTIPLAYER_SERVER_ROOT_DIR})
set(MULTIPLAYER_SERVER_MAJOR_SRCS 
	${MULTIPLAYER_SERVER_ROOT_DIR}/main.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/config/arg_parser.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/config/json_config_parser.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/log/logger_manager.cpp
)
set(MULTIPLAYER_SERVER_NETWORK_SRC
	${MULTIPLAYER_SERVER_ROOT_DIR}/network/asio_server.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/network/asio_tcp_connection.cpp 
)
set(MULTIPLAYER_SERVER_GAME_SRC
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/entity.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/component.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/basic/entity_factory.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/service/login_service.cpp
	${MULTIPLAYER_SERVER_ROOT_DIR}/game/game_main.cpp
)

if (USE_PROTOBUF)
	# protobuf is required
	# include(protobuf)
	# include_directories(${Protobuf_INCLUDE_DIRS})
	# message(STATUS "include Protobuf directories: ${Protobuf_INCLUDE_DIRS}")
endif()

if (USE_FMT)
	# fmt is required
	add_definitions(-DUSE_FMT)
	add_subdirectory(src/log/fmt)
	include_directories(src/log/fmt/include)
endif()

if (USE_SPDLOG)
	# spdlog is required
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
	target_link_directories(${PROJECT_NAME} PRIVATE ${Boost_LIBRARY_DIRS})
else()
	# gcc and clang treat warning as error
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
endif()
add_dependencies(${PROJECT_NAME} Boost)

if (USE_PROTOBUF)
	# if protobuf is used, link it
	# target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf protobuf::libprotobuf-lite) # protobuf::libprotoc
endif()

if(USE_FMT)
	# if fmt is used, link it
	target_link_libraries(${PROJECT_NAME} PRIVATE fmt)
endif()