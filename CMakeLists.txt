cmake_minimum_required(VERSION 3.24)
project(tomography)

set(CMAKE_CXX_STANDARD 17)

#add_executable(tomography conv_backproj.cpp)
add_executable(tomography radon.cpp)
#add_executable(tomography radon_inverse.cpp)