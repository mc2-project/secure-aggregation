include_directories (../)

set(TEST_SRCS host/ enclave/)

add_executable(Test host_test.cpp)

find_library(MBEDTLS mbedtls)
target_include_directories (Test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../host)
target_link_libraries(Test PRIVATE modelaggregator_host mbedcrypto mbedtls)
