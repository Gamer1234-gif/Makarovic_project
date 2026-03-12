cmake_minimum_required(VERSION 3.20)

project(CppTemplate VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
    
file(GLOB_RECURSE SOURCES
    ${SOURCE_DIR}/*.cpp
)

add_executable(CppTemplate ${SOURCES})

target_include_directories(CppTemplate PRIVATE "C:/msys64/ucrt64/include")
target_include_directories(CppTemplate PRIVATE ${SOURCE_DIR})
