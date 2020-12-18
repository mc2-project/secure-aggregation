#include <openenclave/host.h>
#include <omp.h>

#include "enclave.h"

// Include the untrusted modelaggregator header that is generated
// during the build. This file is generated by calling the
// sdk tool oeedger8r against the modelaggregator.edl file.
#include "modelaggregator_u.h"

using namespace std;

static char* g_path = "./enclave/enclave.signed";
static uint32_t g_flags = 0;

// Cannot be larger than NumTCS in modelaggregator.conf
static const int NUM_THREADS = 3;

// This is the function that the Python code will call into.
// Returns 0 on success.
int host_modelaggregator(uint8_t*** encrypted_accumulator, 
        size_t* accumulator_lengths,
        size_t accumulator_length, 
        uint8_t** encrypted_old_params,
        size_t old_params_length,
        uint8_t*** encrypted_new_params_ptr,
        size_t* new_params_length)
{
    oe_result_t error;

#ifdef __ENCLAVE_SIMULATION__
    g_flags |= OE_ENCLAVE_FLAG_SIMULATE;
#endif
#ifdef __ENCLAVE_DEBUG__
    g_flags |= OE_ENCLAVE_FLAG_DEBUG;
#endif

    // Create the enclave
    Enclave enclave(g_path, g_flags);
    error = enclave.getEnclaveRet();
    if (error != OE_OK) {
        fprintf(
            stderr,
            "oe_create_modelaggregator_enclave(): result=%u (%s)\n",
            error,
            oe_result_str(error));
        return NULL;
    }

    error = enclave_store_globals(enclave.getEnclave(),
            encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            old_params_length);
    if (error != OE_OK) {
        fprintf(
            stderr,
            "calling into enclave_store_globals failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return 1;
    }

    bool success;
    error = enclave_set_num_threads(enclave.getEnclave(), &success, NUM_THREADS);
    if (error != OE_OK || !success) {
        fprintf(
            stderr,
            "calling into enclave_set_num_threads failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return 1;
    }

    #pragma omp parallel for
    for (int _ = 0; _ < NUM_THREADS; _ ++) {
        int tid = omp_get_thread_num();
        error = enclave_modelaggregator(enclave.getEnclave(), tid);
        if (error != OE_OK) {
            fprintf(
                stderr,
                "calling into enclave_modelaggregator failed: result=%u (%s)\n",
                error,
                oe_result_str(error));
            exit(1);
        }
    }

    error = enclave_transfer_model_out(enclave.getEnclave(),
            encrypted_new_params_ptr,
            new_params_length);
    if (error != OE_OK) {
        fprintf(
            stderr,
            "calling into enclave_transfer_model_out failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return 1;
    }

    return 0;
}

