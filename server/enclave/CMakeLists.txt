# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

# Use the edger8r to generate C bindings from the EDL file.
add_custom_command(OUTPUT modelaggregator_t.h modelaggregator_t.c modelaggregator_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/modelaggregator.edl
  COMMAND openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/modelaggregator.edl)

add_executable(enclave enc.cpp ${CMAKE_CURRENT_BINARY_DIR}/modelaggregator_t.c)

if (WIN32)
  maybe_build_using_clangw(enclave)
endif ()

# Need for the generated file modelaggregator_t.h
target_include_directories(enclave PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_options(enclave
  PRIVATE
  -U_FORTIFY_SOURCE
  -fno-strict-aliasing
  -D_GLIBCXX_USE_CXX11_ABI=0
  -ftls-model=local-exec
  )
if (WIN32 AND MINGW)
  target_compile_options(enclave PUBLIC -static-libstdc++)
endif (WIN32 AND MINGW)

if (USE_AVX2)
  target_compile_options(enclave PRIVATE -mavx2)
  target_compile_definitions(enclave PRIVATE -DUSE_AVX2)
endif (USE_AVX2)

set_target_properties(enclave PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)

target_compile_definitions(enclave
  PRIVATE
  -D__ENCLAVE__
  -DDMLC_CORE_USE_CMAKE
  OE_API_VERSION=2)

if (LVI_MITIGATION MATCHES ControlFlow)
  # Helper to enable compiler options for LVI mitigation.
  apply_lvi_mitigation(enclave)
  # Link against LVI-mitigated libraries.
  target_link_libraries(enclave openenclave::oeenclave-lvi-cfg openenclave::oelibc-lvi-cfg)
else()
  target_link_libraries(enclave openenclave::oeenclave openenclave::oelibc)
endif()
