#include <stdio.h>
#include <vector>
#include <numeric>
#include <map>
#include <string>
#include "../host/host.cpp"

#include "encryption/encrypt.h"
#include "encryption/serialization.h"
#include "utils.h"
#include "flatbuffers/model_generated.h"

using namespace std;

int main(int argc, char* argv[]) 
{  
    size_t accumulator_length = 3;
    uint8_t** encrypted_accumulator = new uint8_t*[accumulator_length * sizeof(uint8_t*)];
    size_t* accumulator_lengths = new size_t[accumulator_length * sizeof(size_t)];

    for (int i = 0; i < accumulator_length; i++) {
        map<string, vector<float>> accumulator = {{"w1", {i, i + 1, i + 2, i + 3}},
                                                    {"w2", {i + 1, i + 2, i + 3, i + 4}},
                                                    {"w3", {i + 2, i + 3, i + 4, i + 5}},
                                                    {"_contribution", {1}}};
        int serialized_buffer_size = 0;
        uint8_t* serialized_params = serialize(accumulator, &serialized_buffer_size);

        encrypted_accumulator[i] = new uint8_t[(serialized_buffer_size + CIPHER_IV_SIZE + CIPHER_TAG_SIZE) * sizeof(uint8_t)];

        encrypt_bytes(serialized_params, serialized_buffer_size, &encrypted_accumulator[i]);
        accumulator_lengths[i] = serialized_buffer_size;
    }

    map<string, vector<float>> old_params = {{"w1", {-3, -6, -9, -12}},
                                                {"w2", {-6, -9, -12, -15}},
                                                {"w3", {-9, -12, -15, -18}}};
    int serialized_old_params_buffer_size = 0;
    uint8_t* serialized_old_params = serialize(old_params, &serialized_old_params_buffer_size);
    uint8_t* encrypted_old_params = new uint8_t[(serialized_old_params_buffer_size + CIPHER_IV_SIZE + CIPHER_TAG_SIZE) * sizeof(uint8_t)];
    encrypt_bytes(serialized_old_params, serialized_old_params_buffer_size, &encrypted_old_params);

    // Allocate memory for encrypted new params
    uint8_t* encrypted_new_params_ptr;
    float contributions[] = {1, 1, 1};
    size_t new_params_length = 0;

    int error = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            serialized_old_params_buffer_size,
            &encrypted_new_params_ptr,
            &new_params_length,
            contributions);

    // Free memory
    for (int i = 0; i < accumulator_length; i++) {
        delete encrypted_accumulator[i];
    }
    delete encrypted_old_params;

    if (error > 0) {
        return error;
    }

    uint8_t* iv = encrypted_new_params_ptr + new_params_length;
    uint8_t* tag = encrypted_new_params_ptr + new_params_length + CIPHER_IV_SIZE;
    uint8_t* serialized_new_params = new uint8_t[new_params_length * sizeof(uint8_t)];

    decrypt_bytes(encrypted_new_params_ptr, 
            iv, 
            tag, 
            new_params_length,
            &serialized_new_params);

    // Memory allocated inside enclave using oe_host_malloc()
    free(encrypted_new_params_ptr);

    map<string, vector<float>> new_params = deserialize(serialized_new_params);

    for (const auto& pair : new_params) {
        if (pair.second.size() != 4) {
            return 1;
        }
        for (float x : pair.second) {
            if (x != 0) {
                return 1;
            }
        }
    }

    return 0;
}
