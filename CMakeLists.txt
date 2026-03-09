cmake_minimum_required(VERSION 4.2)
project(CHIP8Emu)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 REQUIRED)

add_library(CPU STATIC
        cpu/RAM.h
        cpu/RAM.cpp
        cpu/CPU.h
        cpu/CPU.cpp
        cpu/IFramebuffer.h
        cpu/Keyboard.h
        cpu/ISoundDevice.h
)
target_include_directories(CPU PUBLIC cpu)

add_executable(CHIP8Emu src/main.cpp
        src/SDLFramebuffer.cpp
        src/SDLFramebuffer.h)
target_link_libraries(CHIP8Emu PRIVATE CPU SDL3::SDL3)

install(TARGETS CHIP8Emu RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(CHIP8Emu PRIVATE -O3)
    if (WIN32)
        set_target_properties(CHIP8Emu PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()
