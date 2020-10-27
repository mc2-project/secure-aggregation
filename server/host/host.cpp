#include <openenclave/host.h>
#include <stdio.h>
#include <iostream>

#include "enclave.h"

// Include the untrusted modelaggregator header that is generated
// during the build. This file is generated by calling the
// sdk tool oeedger8r against the modelaggregator.edl file.
#include "modelaggregator_u.h"

using namespace std;

char* path = "./enclave/enclave.signed";
uint32_t flags = OE_ENCLAVE_FLAG_DEBUG | OE_ENCLAVE_FLAG_SIMULATE;

// This is the function that the Python code will call into
// Returns NULL on failure, new encrypted model on success
unsigned char** host_modelaggregator(unsigned char*** encrypted_accumulator, 
        size_t* accumulator_lengths,
        size_t accumulator_length, 
        unsigned char** encrypted_old_params,
        size_t old_params_length)
{
    oe_result_t error;
    // Create the enclave
    Enclave enclave(path, flags);
    error = enclave.getEnclaveRet();
    if (error != OE_OK)
    {
        fprintf(
            stderr,
            "oe_create_modelaggregator_enclave(): result=%u (%s)\n",
            error,
            oe_result_str(error));
        return NULL;
    }

    unsigned char*** encrypted_new_params = new unsigned char**[sizeof(unsigned char**)];
    error = enclave_modelaggregator(enclave.getEnclave(), 
            encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            old_params_length, 
            encrypted_new_params);
    if (error != OE_OK)
    {
        fprintf(
            stderr,
            "calling into enclave_modelaggregator failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return NULL;
    }

    return *encrypted_new_params;
}

