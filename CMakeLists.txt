cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(miau VERSION 0.1.0 LANGUAGES C)

include_directories(.)
add_library(${PROJECT_NAME} STATIC miau.c)

if (MSVC)
    set(SDL2_DIR SDL2/cmake)
endif ()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(example main.c)
target_link_libraries(example miau ${SDL2_LIBRARIES})

if (MSVC)
    file(GLOB SDL_LIB SDL2/lib/x64/SDL2.dll)
    add_custom_command(TARGET example POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_LIB} "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif ()

add_executable(keyboard keyboard.c)
target_link_libraries(keyboard miau ${SDL2_LIBRARIES})

if (MSVC)
    file(GLOB SDL_LIB SDL2/lib/x64/SDL2.dll)
    add_custom_command(TARGET keyboard POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_LIB} "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif ()