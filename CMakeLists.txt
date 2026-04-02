cmake_minimum_required(VERSION 3.20)

project(CppTemplate VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

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
target_link_libraries(CppTemplate PRIVATE 
    SDL2::SDL2main
    C:/msys64/ucrt64/lib/libSDL2_ttf.a
    #C:/msys64/ucrt64/lib/libSDL2_image.a
    C:/msys64/ucrt64/lib/libSDL2.a
    freetype harfbuzz png16 z bz2 brotlidec brotlicommon graphite2
    glib-2.0 intl pcre2-8 dwrite rpcrt4 usp10
    user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32
    setupapi shell32 dinput8 ws2_32 shlwapi atomic m kernel32
)
