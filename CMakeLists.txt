cmake_minimum_required(VERSION 3.12)

project(pros)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES Off)
set(CMAKE_CXX_STANDARD 14)

# osdev on apple silicon can be quite painful
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Meta/gcc.sh")
    set(CMAKE_ASM_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Meta/yasm.sh")
else ()
    set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Toolchain/i686/bin/i686-elf-g++")
    set(CMAKE_ASM_COMPILER "${CMAKE_CURRENT_LIST_DIR}/Toolchain/i686/bin/yasm")
endif ()

set(CMAKE_CXX_FLAGS "-m32 -g -O2 -nostdlib -mno-red-zone -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -fstack-protector-all -fno-threadsafe-statics -fno-builtin -fno-rtti -fno-exceptions")
set(CMAKE_EXE_LINKER_FLAGS "-m32 -T${CMAKE_CURRENT_LIST_DIR}/Kernel/linker.ld")
set(CMAKE_ASM_FLAGS "-f elf32")

enable_language(ASM)

file(GLOB_RECURSE kernel_CPP_SOURCES "Kernel/src/*.cpp")
file(GLOB_RECURSE kernel_ASM_SOURCES "Kernel/src/*.asm")

add_executable(pros.bin
        ${kernel_CPP_SOURCES}
        ${kernel_ASM_SOURCES}
        )

target_include_directories(pros.bin
        PRIVATE
        Kernel/include
        )