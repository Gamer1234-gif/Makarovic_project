cmake_minimum_required(VERSION 3.20)

project(CppTemplate VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
    
file(GLOB_RECURSE SOURCES
    ${SOURCE_DIR}/*.cpp
)

add_executable(CppTemplate ${SOURCES})

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_include_directories(CppTemplate PRIVATE "C:/msys64/ucrt64/include")
target_include_directories(CppTemplate PRIVATE ${SOURCE_DIR})

# Link SDL2
# SDL2 requires SDL2main when building on Windows to provide WinMain entry point.
target_link_libraries(CppTemplate PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf)
