cmake_minimum_required(VERSION 3.30)

project(Project VERSION 1.0 LANGUAGES CXX)

add_subdirectory(dependencies)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(source)

add_executable(Project 
    ${SOURCE_FILES}
)

target_link_libraries(Project ${LINK_LIBRARIES})
target_include_directories(Project PRIVATE 
    ${INCLUDE_DIRECTORIES}
    ${SOURCE_DIRECTORIES}
)
