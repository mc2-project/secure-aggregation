#include <stdio.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <map>
#include <string>

#include "../host/host.cpp"

#include "encryption/encrypt.h"
#include "encryption/serialization.h"
#include "utils.h"
#include "../common/flatbuffers/model_generated.h"

using namespace std;

int main(int argc, char* argv[]) 
{  
    std::cout << "hello world" << std::endl;
    size_t accumulator_length = 3;
    uint8_t*** encrypted_accumulator = new uint8_t**[accumulator_length * sizeof(uint8_t**)];
    size_t* accumulator_lengths = new size_t[accumulator_length * sizeof(size_t)];

    for (int i = 0; i < accumulator_length; i++) {
        map<string, vector<double>> accumulator = {{"w1", {i, i + 1, i + 2, i + 3}}, 
                                                    {"w2", {i + 1, i + 2, i + 3, i + 4}},
                                                    {"w3", {i + 2, i + 3, i + 4, i + 5}},
                                                    {"_contribution", {1}}};
        uint8_t serialized_params[10000];
        int serialized_buffer_size = 0;
        std::cout << "Created accumulator map" << std::endl;

        serialize(accumulator, (uint8_t*) serialized_params, &serialized_buffer_size);
        std::cout << "serialized accumulator" << std::endl;

        encrypted_accumulator[i] = new uint8_t*[3 * sizeof(uint8_t*)];
        encrypted_accumulator[i][0] = new uint8_t[serialized_buffer_size];
        encrypted_accumulator[i][1] = new uint8_t[CIPHER_IV_SIZE];
        encrypted_accumulator[i][2] = new uint8_t[CIPHER_TAG_SIZE];

        encrypt_bytes(serialized_params, serialized_buffer_size, encrypted_accumulator[i]);
        std::cout << "encrypted accumulator" << std::endl;
        accumulator_lengths[i] = serialized_buffer_size;
    }

    map<string, vector<double>> old_params = {{"w1", {-3, -6, -9, -12}}, 
                                                {"w2", {-6, -9, -12, -15}},
                                                {"w3", {-9, -12, -15, -18}}};
    uint8_t serialized_old_params[10000];
    int serialized_old_params_buffer_size = 0;
    std::cout << "serializing old params" << std::endl;
    serialize(old_params, (uint8_t*) serialized_old_params, &serialized_old_params_buffer_size);
    std::cout << "in host test Model kv pai size" << secagg::GetModel(serialized_old_params)->kv()->size() << std::endl;


    uint8_t** encrypted_old_params = new uint8_t*[3 * sizeof(uint8_t*)];

    // Allocate memory for old params
    encrypted_old_params[0] = new uint8_t[serialized_old_params_buffer_size];
    encrypted_old_params[1] = new uint8_t[CIPHER_IV_SIZE];
    encrypted_old_params[2] = new uint8_t[CIPHER_TAG_SIZE];

    std::cout << "encrypting old params" << std::endl;
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
    std::cout << "host model aggregator" << std::endl;
    int error = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            serialized_old_params_buffer_size,
            encrypted_new_params_ptr,
            new_params_length);

    std::cout << "freeing memory" << std::endl;
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

    map<string, vector<double>> new_params = deserialize(serialized_new_params);

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
