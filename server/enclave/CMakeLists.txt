# Use the edger8r to generate C bindings from the EDL file.
add_custom_command(
  OUTPUT modelaggregator_t.h modelaggregator_t.c modelaggregator_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/modelaggregator.edl
  COMMAND
    openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/modelaggregator.edl
    --search-path ${OE_INCLUDEDIR} --search-path
    ${OE_INCLUDEDIR}/openenclave/edl/sgx)

add_executable(enclave ecalls.cpp synch.cpp ${CMAKE_CURRENT_BINARY_DIR}/modelaggregator_t.c)

# Need for the generated file modelaggregator_t.h
target_include_directories(enclave PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_compile_options(enclave
  PRIVATE
  -U_FORTIFY_SOURCE
  -fno-strict-aliasing
  -D_GLIBCXX_USE_CXX11_ABI=0
  -ftls-model=local-exec
  )

set_target_properties(enclave PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)

target_compile_definitions(enclave
  PRIVATE
  OE_API_VERSION=2)
if($ENV{OE_SIMULATION})
  target_compile_definitions(enclave PUBLIC -D__ENCLAVE_SIMULATION__)
endif()

if (LVI_MITIGATION MATCHES ControlFlow)
  # Helper to enable compiler options for LVI mitigation.
  apply_lvi_mitigation(enclave)
  # Link against LVI-mitigated libraries.
  target_link_libraries(enclave openenclave::oeenclave-lvi-cfg openenclave::oelibcxx-lvi-cfg openenclave::oehostfs-lvi-cfg openenclave::oehostsock-lvi-cfg openenclave::oehostresolver-lvi-cfg)
else()
  target_link_libraries(enclave openenclave::oeenclave openenclave::oelibcxx openenclave::oehostfs openenclave::oehostsock openenclave::oehostresolver)
endif()

# Generate key
add_custom_command(OUTPUT private.pem public.pem
  COMMAND openssl genrsa -out private.pem -3 3072
  COMMAND openssl rsa -in private.pem -pubout -out public.pem)

# Sign enclave
add_custom_command(OUTPUT enclave.signed
  DEPENDS enclave modelaggregator.conf private.pem
  COMMAND openenclave::oesign sign -e $<TARGET_FILE:enclave> -c ${CMAKE_SOURCE_DIR}/enclave/modelaggregator.conf -k private.pem)

add_custom_target(sign ALL DEPENDS enclave.signed)

