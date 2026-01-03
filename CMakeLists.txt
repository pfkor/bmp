cmake_minimum_required(VERSION 4.0)
project(bmp_main C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

file(GLOB SOURCES "*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
