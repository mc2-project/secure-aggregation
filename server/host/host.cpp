#include <openenclave/host.h>
// #include "encryption/encrypt.h"
#include <omp.h>
#include "enclave.h"

// Include the untrusted modelaggregator header that is generated
// during the build. This file is generated by calling the
// sdk tool oeedger8r against the modelaggregator.edl file.
#include "modelaggregator_u.h"

using namespace std;

// static char* g_path = "/workspace/secure-aggregation/server/build/enclave/enclave.signed"; 
static char* g_path = "/home/davidyi624/kvah/server/build/enclave/enclave.signed"; 

static uint32_t g_flags = 0;

// Cannot be larger than NumTCS in modelaggregator.conf
static const int NUM_THREADS = 1;

// FIXME: this shoudl only be in encrypt.h
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
        float* contributions)
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

    // int index = 0;
    int num_local_updates = accumulator_length;
    uint8_t*** new_encrypted_accumulator = (uint8_t***) malloc(num_local_updates * sizeof(uint8_t**));
    for (int i = 0; i < num_local_updates; i++) {
        new_encrypted_accumulator[i] = (uint8_t**) malloc(3 * sizeof(uint8_t*));

        // Copy over encrypted local model update
        new_encrypted_accumulator[i][0] = (uint8_t*) malloc(accumulator_lengths[i] * sizeof(uint8_t));
        memcpy(new_encrypted_accumulator[i][0], encrypted_accumulator[i], accumulator_lengths[i]);
        // index += accumulator_lengths[i];

        // Copy over IV
        new_encrypted_accumulator[i][1] = (uint8_t*) malloc(CIPHER_IV_SIZE * sizeof(uint8_t));
        memcpy(new_encrypted_accumulator[i][1], encrypted_accumulator[i] + accumulator_lengths[i], CIPHER_IV_SIZE);
        // index += CIPHER_IV_SIZE;

        // Copy over tag
        new_encrypted_accumulator[i][2] = (uint8_t*) malloc(CIPHER_TAG_SIZE * sizeof(uint8_t));
        memcpy(new_encrypted_accumulator[i][2], encrypted_accumulator[i] + accumulator_lengths[i] + CIPHER_IV_SIZE, CIPHER_TAG_SIZE);
        // index += CIPHER_TAG_SIZE;
    }

    int index = 0;
    uint8_t** new_encrypted_old_params = (uint8_t**) malloc(3 * sizeof(uint8_t*));

    // Copy over old encrypted params
    new_encrypted_old_params[0] = (uint8_t*) malloc(old_params_length * sizeof(uint8_t));
    memcpy(new_encrypted_old_params[0], encrypted_old_params + index, old_params_length);
    index += old_params_length;

    // Copy over IV
    new_encrypted_old_params[1] = (uint8_t*) malloc(CIPHER_IV_SIZE * sizeof(uint8_t));
    memcpy(new_encrypted_old_params[1], encrypted_old_params + index, CIPHER_IV_SIZE);
    index += CIPHER_IV_SIZE;

    // Copy over tag
    new_encrypted_old_params[2] = (uint8_t*) malloc(CIPHER_TAG_SIZE * sizeof(uint8_t));
    memcpy(new_encrypted_old_params[2], encrypted_old_params + index, CIPHER_TAG_SIZE);
    index += CIPHER_TAG_SIZE;

    error = enclave_store_globals(enclave.getEnclave(),
            new_encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            new_encrypted_old_params, 
            old_params_length,
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

    // uint8_t** new_encrypted_new_params_ptr = (uint8_t**) malloc(3 * sizeof(uint8_t*)); 
    // new_encrypted_new_params_ptr[0] = (uint8_t*) malloc(old_params_length * sizeof(uint8_t));
    // new_encrypted_new_params_ptr[1] = (uint8_t*) malloc(CIPHER_IV_SIZE * sizeof(uint8_t));
    // new_encrypted_new_params_ptr[2] = (uint8_t*) malloc(CIPHER_TAG_SIZE * sizeof(uint8_t));

    error = enclave_transfer_model_out(enclave.getEnclave(),
            encrypted_new_params_ptr,
            new_params_length);

    // index = 0;
    // 
    // memcpy(*encrypted_new_params_ptr + index, new_encrypted_new_params_ptr[0], *new_params_length);
    // index += *new_params_length;
    // 
    // memcpy(*encrypted_new_params_ptr + index, new_encrypted_new_params_ptr[1], cipher_iv_size);
    // index += cipher_iv_size;
    // 
    // memcpy(*encrypted_new_params_ptr + index, new_encrypted_new_params_ptr[2], cipher_tag_size);
    // index += CIPHER_TAG_SIZE;

    // Free everything
    for (int i = 0; i < num_local_updates; i++) {
        for (int j = 0; j < 3; j++) {
            free(new_encrypted_accumulator[i][j]);
        }
        free(new_encrypted_accumulator[i]);
    }
    free(new_encrypted_accumulator);

    for (int i = 0; i < 3; i++) {
        free(new_encrypted_old_params[i]);
    }
    free(new_encrypted_old_params);

    // free(new_encrypted_new_params_ptr);

    if (error != OE_OK) {
        fprintf(
            stderr,
            "calling into enclave_transfer_model_out failed: result=%u (%s)\n",
            error,
            oe_result_str(error));
        return 1;
    }
    enclave.terminate();

    return 0;
}
