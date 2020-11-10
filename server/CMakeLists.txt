cmake_minimum_required(VERSION 3.11)
project(modelaggregator LANGUAGES CXX C VERSION 1.00)

include(CTest)

include_directories(../common /snap/flatbuffers/current/include)

if (LVI_MITIGATION MATCHES ControlFlow)
  # Configure the cmake to use customized compilation toolchain.
  # This package has to be added before `project()`.
  find_package(OpenEnclave-LVI-Mitigation CONFIG REQUIRED)
endif()

project("Model Aggregator" LANGUAGES C CXX)

# Currently the `OpenEnclave` package depends on `project()`.
find_package(OpenEnclave CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(enclave)
add_subdirectory(host)

if (BUILD_TESTING)
  add_subdirectory(tests)
  add_test(NAME HostTest COMMAND Test)
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                  DEPENDS Test modelaggregator_host sign)
endif()
