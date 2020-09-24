cmake_minimum_required(VERSION 3.11)
project(aggregation LANGUAGES CXX C VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

add_executable(encryption main.cpp)

find_library(MBEDTLS mbedtls)
target_link_libraries(encryption PRIVATE mbedcrypto mbedtls)
