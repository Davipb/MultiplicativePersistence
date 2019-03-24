cmake_minimum_required(VERSION 3.10)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED "ON")

project(
    MultiplicativePersistence
    VERSION 1.0.0
    LANGUAGES C
)

include_directories(src)

add_executable(
    MultiplicativePersistence
    src/Main.c
    src/Number.c
)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(MultiplicativePersistence PRIVATE "/sdl" "/W4" "/WX")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(MultiplicativePersistence PRIVATE "-Wall" "-Werror")
endif()
