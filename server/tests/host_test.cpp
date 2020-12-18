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
    uint8_t*** encrypted_accumulator = new uint8_t**[accumulator_length * sizeof(uint8_t**)];
    size_t* accumulator_lengths = new size_t[accumulator_length * sizeof(size_t)];

    for (int i = 0; i < accumulator_length; i++) {
        map<string, vector<float>> accumulator = {{"w1", {i, i + 1, i + 2, i + 3}}, 
                                                    {"w2", {i + 1, i + 2, i + 3, i + 4}},
                                                    {"w3", {i + 2, i + 3, i + 4, i + 5}},
                                                    {"w4", {i + 3, i + 4, i + 5, i + 6}},
                                                    {"w5", {i + 4, i + 5, i + 6, i + 7}},
                                                    {"w6", {i + 5, i + 6, i + 7, i + 8}},
                                                    {"w7", {i + 6, i + 7, i + 8, i + 9}},
                                                    {"w8", {i + 7, i + 8, i + 9, i + 10}},
                                                    {"_contribution", {1}}};
        int serialized_buffer_size = 0;
        uint8_t* serialized_params = serialize(accumulator, &serialized_buffer_size);

        encrypted_accumulator[i] = new uint8_t*[3 * sizeof(uint8_t*)];
        encrypted_accumulator[i][0] = new uint8_t[serialized_buffer_size];
        encrypted_accumulator[i][1] = new uint8_t[CIPHER_IV_SIZE];
        encrypted_accumulator[i][2] = new uint8_t[CIPHER_TAG_SIZE];

        encrypt_bytes(serialized_params, serialized_buffer_size, encrypted_accumulator[i]);
        accumulator_lengths[i] = serialized_buffer_size;
    }

    map<string, vector<float>> old_params = {{"w1", {-3, -6, -9, -12}}, 
                                                {"w2", {-6, -9, -12, -15}},
                                                {"w3", {-9, -12, -15, -18}},
                                                {"w4", {-12, -15, -18, -21}},
                                                {"w5", {-15, -18, -21, -24}},
                                                {"w6", {-18, -21, -24, -27}},
                                                {"w7", {-21, -24, -27, -30}},
                                                {"w8", {-24, -27, -30, -33}}};
    int serialized_old_params_buffer_size = 0;
    uint8_t* serialized_old_params = serialize(old_params, &serialized_old_params_buffer_size);


    uint8_t** encrypted_old_params = new uint8_t*[3 * sizeof(uint8_t*)];

    // Allocate memory for old params
    encrypted_old_params[0] = new uint8_t[serialized_old_params_buffer_size];
    encrypted_old_params[1] = new uint8_t[CIPHER_IV_SIZE];
    encrypted_old_params[2] = new uint8_t[CIPHER_TAG_SIZE];

    encrypt_bytes(serialized_old_params, serialized_old_params_buffer_size, encrypted_old_params);

    // Allocate memory for encrypted new params
    uint8_t*** encrypted_new_params_ptr = new uint8_t**[3 * sizeof(uint8_t**)];
    for (int i = 0; i < accumulator_length; i++) {
        encrypted_new_params_ptr[i] = new uint8_t*[3 * sizeof(uint8_t*)];
        encrypted_new_params_ptr[i][0] = new uint8_t[serialized_old_params_buffer_size];
        encrypted_new_params_ptr[i][1] = new uint8_t[CIPHER_IV_SIZE];
        encrypted_new_params_ptr[i][2] = new uint8_t[CIPHER_TAG_SIZE];
    }

    size_t* new_params_length = new size_t;
    int error = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            serialized_old_params_buffer_size,
            encrypted_new_params_ptr,
            new_params_length);

    // Free memory
    for (int i = 0; i < accumulator_length; i++) {
        delete encrypted_accumulator[i][0];
        delete encrypted_accumulator[i][1];
        delete encrypted_accumulator[i][2];
        delete encrypted_accumulator[i];
    }
    delete encrypted_old_params[0];
    delete encrypted_old_params[1];
    delete encrypted_old_params[2];
    delete encrypted_old_params;

    if (error > 0) {
        return error;
    }

    uint8_t** encrypted_new_params = *encrypted_new_params_ptr;
    uint8_t* serialized_new_params = new uint8_t[*new_params_length * sizeof(uint8_t)];
    decrypt_bytes(encrypted_new_params[0], 
            encrypted_new_params[1], 
            encrypted_new_params[2], 
            *new_params_length,
            &serialized_new_params);

    // Free memory
    for (int i = 0; i < accumulator_length; i++) {
        delete encrypted_new_params_ptr[i][0];
        delete encrypted_new_params_ptr[i][1];
        delete encrypted_new_params_ptr[i][2];
        delete encrypted_new_params_ptr[i];
    }

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
