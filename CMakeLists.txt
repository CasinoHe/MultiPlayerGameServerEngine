cmake_minimum_required(VERSION 3.8)

project(MultiPlayerServer)

# export all compile commands for intellisense hinting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# all options
option(USE_PROTOBUF "Use protobuf for network communication" ON)
option(USE_SPDLOG "Use spdlog for logging" ON)
option(USE_FMT "Use fmt for log formatting" OFF)

# boost library is required
find_package(Boost COMPONENTS system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "include Boost directories: ${Boost_INCLUDE_DIRS}")

# project settings
set(MULTIPLAYER_SERVER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
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

if (USE_PROTOBUF)
		# protobuf is required
		find_package(Protobuf REQUIRED)
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
		add_definitions(-DUSE_SPDLOG)
		add_subdirectory(src/log/spdlog)
		include_directories(src/log/spdlog/include)
		list(APPEND MULTIPLAYER_SERVER_MAJOR_SRCS ${MULTIPLAYER_SERVER_ROOT_DIR}/log/spdlog_logger_imp.cpp)
else()
endif()

add_executable(${PROJECT_NAME} ${MULTIPLAYER_SERVER_MAJOR_SRCS} ${MULTIPLAYER_SERVER_NETWORK_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

if (USE_PROTOBUF)
		# if protobuf is used, link it
		target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf protobuf::libprotobuf-lite) # protobuf::libprotoc
endif()

if(USE_FMT)
	# if fmt is used, link it
	target_link_libraries(${PROJECT_NAME} PRIVATE fmt)
endif()