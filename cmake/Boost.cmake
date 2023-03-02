# build boost from source to avoid different boost versions on different platforms
# and because boost is built using bjam, so it is hard to build it using cmake, and hard to maintain the build tree using cmake
# so we build boost using official build script and then use it as a static library
message(STATUS "Building external project boost from source")

# boost version variable
set(BOOST_MAJOR_VERSION 1)
set(BOOST_MINOR_VERSION 81)
set(BOOST_PATCH_VERSION 0)

# set boost build command
if (WIN32)
    set(Boost_DOWNLOAD_URL "https://boostorg.jfrog.io/artifactory/main/release/${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}.${BOOST_PATCH_VERSION}/source/boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_${BOOST_PATCH_VERSION}.zip")
    set(Boost_SHA256 "6e689b266b27d4db57f648b1e5c905c8acd6716b46716a12f6fc73fc80af842e")
    set(Boost_BOOTSTRAP_COMMAND "bootstrap.bat")
    set(Boost_BUILD_COMMAND b2.exe)
else ()
    set(Boost_DOWNLOAD_URL "https://boostorg.jfrog.io/artifactory/main/release/${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}.${BOOST_PATCH_VERSION}/source/boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_${BOOST_PATCH_VERSION}.tar.gz")
    set(Boost_SHA256 "205666dea9f6a7cfed87c7a6dfbeb52a2c1b9de55712c9c1a87735d7181452b6")
    set(Boost_BOOTSTRAP_COMMAND "./bootstrap.sh")
    set(Boost_BUILD_COMMAND ./b2)
endif()

# check architecture of current platform
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(Boost_ARCH 64)
else ()
    set(Boost_ARCH 32)
endif()

# set build libraries
set(Boost_BUILD_LIBRARIES 
    --with-system
    --with-program_options
)
# build static/shared libraries
set(Boost_BUILD_LIBS_TYPE static)
set(Boost_RUNTIME_LINK_TYPE shared)

# build debug/release libraries
if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set(Boost_BUILD_VARIANT "debug")
else ()
    set(Boost_BUILD_VARIANT "release")
endif()

# determine toolset
if (WIN32)
    set(Boost_TOOLSET "msvc-${MSVC_TOOLSET_VERSION}")
    message(STATUS "Build external boost using toolset ${Boost_TOOLSET}")
endif()

message(STATUS "build External boost, variant ${Boost_BUILD_VARIANT}, architecture ${Boost_ARCH}, cmake build type is ${CMAKE_BUILD_TYPE}")
# external build boost
ExternalProject_Add(Boost
    PREFIX ${THIRD_PARTIES_BUILD_DIR}/boost
    URL ${Boost_DOWNLOAD_URL}
    URL_HASH SHA256=${Boost_SHA256}
    DOWNLOAD_DIR ${THIRD_PARTIES_DOWNLOAD_DIR}/Boost
    CONFIGURE_COMMAND ${Boost_BOOTSTRAP_COMMAND}
    BUILD_COMMAND ${Boost_BUILD_COMMAND} --layout=system ${Boost_BUILD_LIBRARIES} address-model=${Boost_ARCH} link=${Boost_BUILD_LIBS_TYPE} runtime-link=${Boost_RUNTIME_LINK_TYPE} variant=${Boost_BUILD_VARIANT} toolset=${Boost_TOOLSET} threading=multi stage
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""
)

# set boost library and include directories
set(Boost_FOUND TRUE)
set(Boost_INCLUDE_DIRS ${THIRD_PARTIES_BUILD_DIR}/boost/src/boost)
set(Boost_LIBRARY_DIRS ${THIRD_PARTIES_BUILD_DIR}/boost/src/boost/stage/lib)

# if (WIN32)
#     set(Boost_LIBRARIES
#         ${Boost_LIBRARY_DIRS}/libboost_system-vc${MSVC_TOOLSET_VERSION}-mt-${Boost_LINK_GD_FLAG}x${Boost_ARCH}-${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.lib
#         ${Boost_LIBRARY_DIRS}/libboost_program_options-vc${MSVC_TOOLSET_VERSION}-mt-${Boost_LINK_GD_FLAG}x${Boost_ARCH}-${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}.lib
#     )
# else ()
message(STATUS "suffix is ${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(Boost_LIBRARIES
    ${Boost_LIBRARY_DIRS}/libboost_system${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${Boost_LIBRARY_DIRS}/libboost_program_options${CMAKE_STATIC_LIBRARY_SUFFIX}
)
# endif()