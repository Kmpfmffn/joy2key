
cmake_minimum_required(VERSION 3.10.0)
project(joy2key VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(joy2key)
target_sources(joy2key PRIVATE
    src/joy2key.cpp)