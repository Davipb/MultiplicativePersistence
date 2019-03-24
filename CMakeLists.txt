cmake_minimum_required(VERSION 3.12)

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

