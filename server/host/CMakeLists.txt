# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.
add_custom_command(OUTPUT modelaggregator_u.h modelaggregator_u.c modelaggregator_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/modelaggregator.edl
  COMMAND openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/modelaggregator.edl)

add_executable(modelaggregator_host host.cpp ${CMAKE_CURRENT_BINARY_DIR}/modelaggregator_u.c)

if(WIN32)
  copy_oedebugrt_target(modelaggregator_host_oedebugrt)
  add_dependencies(modelaggregator_host modelaggregator_host_oedebugrt)
endif()

target_include_directories(modelaggregator_host PRIVATE
  # Needed for the generated file modelaggregator_u.h
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(modelaggregator_host openenclave::oehostapp)
