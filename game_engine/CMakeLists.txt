cmake_minimum_required(VERSION 3.8)

project(environment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(MSVC)
    # define NOMINMAX so we can use std::min/std::max normally
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /EHsc /DNOMINMAX")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /EHsc /DNOMINMAX /Z7")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -pedantic -Wextra -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -Wextra -Wno-unused-variable -g -O0 -fno-inline")
    if(NOT MINGW)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined,address")
    endif()
endif()

# versions of cmake before 3.4 always link with -rdynamic on linux, which breaks static linkage with clang
# unfortunately travis right now only has cmake 3.2, so have to do this workaround for now
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

include_directories(${CMAKE_SOURCE_DIR}/config)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/core/command)
include_directories(${CMAKE_SOURCE_DIR}/error)
include_directories(${CMAKE_SOURCE_DIR}/logging)
include_directories(${CMAKE_SOURCE_DIR}/mapgen)
include_directories(${CMAKE_SOURCE_DIR}/model)
include_directories(${CMAKE_SOURCE_DIR}/networking/common)
include_directories(${CMAKE_SOURCE_DIR}/replay)
include_directories(${CMAKE_SOURCE_DIR}/util)

if(WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/networking/win32)
else()
    include_directories(${CMAKE_SOURCE_DIR}/networking/unix)
endif()

if(NOT WIN32)
add_custom_target(VERSION_CHECK
    ./set_version.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(dirs
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/core/command
    ${CMAKE_SOURCE_DIR}/error
    ${CMAKE_SOURCE_DIR}/logging
    ${CMAKE_SOURCE_DIR}/mapgen
    ${CMAKE_SOURCE_DIR}/model
    ${CMAKE_SOURCE_DIR}/networking/common
    ${CMAKE_SOURCE_DIR}/replay
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
set(HALITE_SOURCE_FILES "${SOURCE_FILES}" main.cpp)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)
include(ExternalProject)

configure_file(CMakeLists.txt.in deps-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -Wno-dev .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/deps-download)
if(result)
    message(FATAL_ERROR "CMake step for dependencies failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/deps-download)
if(result)
    message(FATAL_ERROR "Build step for dependencies failed: ${result}")
endif()

add_subdirectory(${EXTERNAL_INSTALL_LOCATION}/zstd/build/cmake EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_INSTALL_LOCATION}/catch)

include_directories(${EXTERNAL_INSTALL_LOCATION}/zstd/lib ${EXTERNAL_INSTALL_LOCATION}/nlohmann ${EXTERNAL_INSTALL_LOCATION}/tclap/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/zstd/build/cmake/lib)

add_library(halite_core OBJECT ${SOURCE_FILES})

add_executable(halite $<TARGET_OBJECTS:halite_core> main.cpp)
if(NOT WIN32)
    add_dependencies(halite VERSION_CHECK libzstd_static)
endif()

file(GLOB_RECURSE SOURCE ${CMAKE_SOURCE_DIR}/test/*.[ch]*)
set(TEST_FILES "${TEST_FILES}" ${SOURCE})

if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT WIN32)
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

    file(GLOB_RECURSE TEST_FILES ${CMAKE_SOURCE_DIR}/test/*.[ch]*)
    add_executable(halite_test $<TARGET_OBJECTS:halite_core> ${TEST_FILES} ${CMAKE_SOURCE_DIR}/test/TestMain.cpp)
else()
    add_executable(halite_test $<TARGET_OBJECTS:halite_core> ${TEST_FILES} ${CMAKE_SOURCE_DIR}/test/TestMain.cpp)
endif()

add_dependencies(halite_test libzstd_static)
if(APPLE)
    # No static linkage here - https://stackoverflow.com/questions/5259249/creating-static-mac-os-x-c-build
    target_link_libraries(halite pthread libzstd_static)
    target_link_libraries(halite_test pthread libzstd_static Catch2::Catch)
elseif(MSVC)
    target_link_libraries(halite libzstd_static)
    target_link_libraries(halite_test libzstd_static Catch2::Catch)
else()
    if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT MINGW)
        # Link dynamically on Linux since AddressSanitizer doesn't work with static linkage
        # But link statically on MinGW, since it doesn't have address sanitizer and keeping MinGW dlls around is a pain
        target_link_libraries(halite pthread libzstd_static)
        target_link_libraries(halite_test pthread libzstd_static Catch2::Catch)
    else()
        # On Linux, make sure to link using the whole-archive option
        # See: https://stackoverflow.com/questions/35116327
        target_link_libraries(halite -Wl,--whole-archive pthread -Wl,--no-whole-archive -static libzstd_static)
        target_link_libraries(halite_test -Wl,--whole-archive pthread -Wl,--no-whole-archive -static libzstd_static Catch2::Catch)
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${EXTERNAL_INSTALL_LOCATION}/catch/contrib)
include(Catch)
enable_testing()
catch_discover_tests(halite_test)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_dependencies(check halite_test)
