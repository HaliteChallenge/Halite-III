cmake_minimum_required(VERSION 2.8)

project(MyCBot)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Wextra -Wno-unused-function -pedantic -trigraphs")

include_directories(${CMAKE_SOURCE_DIR})
set(SOURCE_FILES "${SOURCE_FILES}" MyBot.c)

add_executable(MyBot ${SOURCE_FILES})

if(MINGW)
    target_link_libraries(MyBot -static)
endif()