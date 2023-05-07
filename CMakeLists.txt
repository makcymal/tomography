cmake_minimum_required(VERSION 3.24)
project(tomography)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STACK_SIZE 10000000)

add_executable(tomography
        src/main.cpp
        src/geometry.cpp
        src/utilities.cpp
        src/radon.cpp
        src/quadratures.cpp)

target_include_directories(tomography
        PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
        )

