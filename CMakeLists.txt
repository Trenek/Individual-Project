cmake_minimum_required(VERSION 3.30)

project(Project VERSION 1.0 LANGUAGES CXX)

add_subdirectory(dependencies)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Project 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/simulation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/common.h"
)

target_link_libraries(Project ${LINK_LIBRARIES})
target_include_directories(Project PRIVATE 
    ${INCLUDE_DIRECTORIES}
)
