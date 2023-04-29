cmake_minimum_required(VERSION 3.24)
project(tomography)

set(CMAKE_CXX_STANDARD 17)

add_executable(tomography
        src/main.cpp
        src/geom.cpp
        src/util.cpp
#        src/algo.cpp
        src/radon.cpp
        src/nummeth.cpp)

target_include_directories(tomography
        PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
        )
