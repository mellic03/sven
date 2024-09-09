cmake_minimum_required(VERSION 3.9)
project(sven)

cmake_policy(SET CMP0048 NEW)

set(CMAKE_BUILD_PARALLEL_LEVEL 24)
set(CMAKE_INSTALL_MESSAGE LAZY)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# ---------------------------------------------------
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_image_INCLUDE_DIRS})

include_directories(external/)
# ---------------------------------------------------


file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    SDL2
    SDL2_image
)
