cmake_minimum_required(VERSION 3.10)
project(BME280_Report)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-O2 -li2c")

file(GLOB_RECURSE native_srcs src/*.cpp)

add_executable(${PROJECT_NAME} ${native_srcs})
target_link_libraries(${PROJECT_NAME} i2c)