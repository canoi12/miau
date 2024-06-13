cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(miau VERSION 0.1.0 LANGUAGES C)

include_directories(.)
add_library(${PROJECT_NAME} STATIC miau.c)

find_package(SDL2 REQUIRED)
add_executable(example main.c)
target_link_libraries(example miau SDL2 m)