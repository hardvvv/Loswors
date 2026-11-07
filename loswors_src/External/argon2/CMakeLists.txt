cmake_minimum_required(VERSION 3.15)
project(argon2 LANGUAGES C)

set(ARGON2_SOURCES
    src/argon2.c
    src/core.c
    src/encoding.c
    src/ref.c
    src/thread.c
    src/blake2/blake2b.c
)

add_library(argon2 STATIC ${ARGON2_SOURCES})

target_include_directories(argon2 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)