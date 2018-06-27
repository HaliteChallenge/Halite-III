cmake_minimum_required(VERSION 3.8)

project(environment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall -Wextra -pedantic -Wno-unused-variable")

# versions of cmake before 3.4 always link with -rdynamic on linux, which breaks static linkage with clang
# unfortunately travis right now only has cmake 3.2, so have to do this workaround for now
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

include_directories(${CMAKE_SOURCE_DIR}/config)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/error)
include_directories(${CMAKE_SOURCE_DIR}/logging)
include_directories(${CMAKE_SOURCE_DIR}/mapgen)
include_directories(${CMAKE_SOURCE_DIR}/model)
include_directories(${CMAKE_SOURCE_DIR}/replay)
include_directories(${CMAKE_SOURCE_DIR}/networking/common)
include_directories(${CMAKE_SOURCE_DIR}/util)

if(WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/networking/win32)
else()
    include_directories(${CMAKE_SOURCE_DIR}/networking/unix)
endif()

add_custom_target(VERSION_CHECK
    ./set_version.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

set(dirs
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/error
    ${CMAKE_SOURCE_DIR}/logging
    ${CMAKE_SOURCE_DIR}/mapgen
    ${CMAKE_SOURCE_DIR}/model
    ${CMAKE_SOURCE_DIR}/replay
    ${CMAKE_SOURCE_DIR}/networking/common
    ${CMAKE_SOURCE_DIR}/util
)

if(WIN32)
    list(APPEND dirs ${CMAKE_SOURCE_DIR}/networking/win32)
else()
    list(APPEND dirs ${CMAKE_SOURCE_DIR}/networking/unix)
endif()

foreach(dir ${dirs})
    file(GLOB_RECURSE SOURCE ${dir}/*.[ch]*)
    set(SOURCE_FILES "${SOURCE_FILES}" ${SOURCE})
endforeach()

include_directories(${CMAKE_SOURCE_DIR})
set(SOURCE_FILES "${SOURCE_FILES}" main.cpp)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/external)
include(ExternalProject)

ExternalProject_Add(nlohmann
    URL https://github.com/nlohmann/json/releases/download/v3.1.2/include.zip
    URL_HASH SHA256=495362ee1b9d03d9526ba9ccf1b4a9c37691abe3a642ddbced13e5778c16660c
    PREFIX ${EXTERNAL_INSTALL_LOCATION}
    SOURCE_DIR ${EXTERNAL_INSTALL_LOCATION}/nlohmann
    CMAKE_COMMAND ""
    BUILD_COMMAND ""
    CONFIGURE_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add(tclap
    URL https://netix.dl.sourceforge.net/project/tclap/tclap-1.2.2.tar.gz
    URL_HASH SHA256=f5013be7fcaafc69ba0ce2d1710f693f61e9c336b6292ae4f57554f59fde5837
    PREFIX ${EXTERNAL_INSTALL_LOCATION}
    SOURCE_DIR ${EXTERNAL_INSTALL_LOCATION}/tclap
    CMAKE_COMMAND ""
    BUILD_COMMAND ""
    CONFIGURE_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Add(libzstd
    URL https://github.com/facebook/zstd/archive/v1.3.4.zip
    URL_HASH SHA256=76f3e6cadd9e1714cad7b550e53b82d4a5a5f9e08fbc0b3eb54abd0d0ebeb42d
    PREFIX ${EXTERNAL_INSTALL_LOCATION}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX:PATH=${EXTERNAL_INSTALL_LOCATION} ${EXTERNAL_INSTALL_LOCATION}/src/libzstd/build/cmake/
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include ${EXTERNAL_INSTALL_LOCATION}/nlohmann ${EXTERNAL_INSTALL_LOCATION}/tclap/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

add_executable(halite ${SOURCE_FILES})

if (APPLE)
add_custom_target(
    remove_dylib
    ${CMAKE_COMMAND} -E remove ${EXTERNAL_INSTALL_LOCATION}/lib/libzstd.1.3.4.dylib ${EXTERNAL_INSTALL_LOCATION}/lib/libzstd.dylib
)
add_dependencies(remove_dylib libzstd)
add_dependencies(halite remove_dylib)
endif()

add_dependencies(halite VERSION_CHECK nlohmann tclap libzstd)

if (APPLE)
    # No static linkage here - https://stackoverflow.com/questions/5259249/creating-static-mac-os-x-c-build
    target_link_libraries(halite pthread zstd)
else()
    # On Linux, make sure to link using the whole-archive option
    # See: https://stackoverflow.com/questions/35116327
    target_link_libraries(halite -Wl,--whole-archive pthread -Wl,--no-whole-archive -static zstd -Wl,--no-whole-archive)
endif()
