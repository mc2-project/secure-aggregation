include_directories (../)

set(TEST_SRCS host/ enclave/)

add_executable(Test host_test.cpp)

target_link_libraries(Test modelaggregator_host)
