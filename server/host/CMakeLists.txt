find_package(OpenEnclave CONFIG REQUIRED)

# Use the edger8r to generate C bindings from the EDL file.
add_custom_command(
  OUTPUT modelaggregator_u.h modelaggregator_u.c modelaggregator_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/modelaggregator.edl
  COMMAND
    openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/modelaggregator.edl
    --search-path ${OE_INCLUDEDIR} --search-path
    ${OE_INCLUDEDIR}/openenclave/edl/sgx)

add_library(modelaggregator_host host.cpp ${CMAKE_CURRENT_BINARY_DIR}/modelaggregator_u.c)

target_include_directories(modelaggregator_host PRIVATE
  # Needed for the generated file modelaggregator_u.h
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(modelaggregator_host openenclave::oehostapp)