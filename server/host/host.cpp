#include <omp.h>
#include "enclave.h"

// Include the untrusted modelaggregator header that is generated
// during the build. This file is generated by calling the
// sdk tool oeedger8r against the modelaggregator.edl file.
#include "modelaggregator_u.h"

using namespace std;

// FIXME: remove this hardcoded path
// static char* g_path = "./enclave/enclave.signed";
// static char* g_path = "/home/davidyi624/kvah/server/build/enclave/enclave.signed";
static char* g_path = "/workspace/secure-aggregation/server/build/enclave/enclave.signed";

// Cannot be larger than NumTCS in modelaggregator.conf
static const int NUM_THREADS = 1;

// FIXME: this should only be in encrypt.h
#define CIPHER_KEY_SIZE 16
#define CIPHER_IV_SIZE  12
#define CIPHER_TAG_SIZE 16

// This is the function that the Python code will call into.
// Returns 0 on success.
int host_modelaggregator(uint8_t** encrypted_accumulator, 
        size_t* accumulator_lengths,
        size_t accumulator_length, 
        uint8_t* encrypted_old_params,
        size_t old_params_length,
        uint8_t** encrypted_new_params_ptr,
        size_t* new_params_length,
        float* contributions) {
    if (!Enclave::getInstance().getEnclave()) {
        oe_result_t result;

        uint32_t flags = 0;
#ifdef __ENCLAVE_DEBUG__
        flags |= OE_ENCLAVE_FLAG_DEBUG;
#endif
#ifdef __ENCLAVE_SIMULATION__
        flags |= OE_ENCLAVE_FLAG_SIMULATE;
#endif
        oe_enclave_t** enclave = Enclave::getInstance().getEnclaveRef();

        // Create the enclave
        result = oe_create_modelaggregator_enclave(
            g_path, OE_ENCLAVE_TYPE_AUTO, flags, NULL, 0, enclave);
        if (result != OE_OK) {
          fprintf(
              stderr,
              "oe_create_enclave(): result=%u (%s)\n",
              result,
              oe_result_str(result));
          oe_terminate_enclave(Enclave::getInstance().getEnclave());
          return Enclave::getInstance().enclave_ret;
        }
    }
    oe_result_t error;

    error = enclave_store_globals(Enclave::getInstance().getEnclave(),
            encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            old_params_length + CIPHER_IV_SIZE + CIPHER_TAG_SIZE,
            contributions);

    if (error != OE_OK) {
        fprintf(
            stderr,
            "calling into enclave_store_globals failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return 1;
    }

    bool success;
    error = enclave_set_num_threads(Enclave::getInstance().getEnclave(), &success, NUM_THREADS);
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
        error = enclave_modelaggregator(Enclave::getInstance().getEnclave(), tid);
        if (error != OE_OK) {
            fprintf(
                stderr,
                "calling into enclave_modelaggregator failed: result=%u (%s)\n",
                error,
                oe_result_str(error));
            exit(1);
        }
    }

    error = enclave_transfer_model_out(Enclave::getInstance().getEnclave(),
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
