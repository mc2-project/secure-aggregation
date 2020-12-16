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
#include <time.h>

using namespace std;

int main(int argc, char* argv[]) 
{  
    const size_t accumulator_length = 3;
    size_t num_weights = 6000;
    size_t weights_length = 100;

    uint8_t*** encrypted_accumulator = new uint8_t**[accumulator_length * sizeof(uint8_t**)];
    size_t* accumulator_lengths = new size_t[accumulator_length * sizeof(size_t)];

    for (int i = 0; i < accumulator_length; i++) {
        map<string, vector<float>> accumulator = {{"_contribution", {1}}};
        for (int j = 0; j < num_weights; j++) {
            vector<float> weights;
            for (int k = 0; k < weights_length; k++) {
                weights.push_back(i + j + k);
            }
            accumulator.insert(make_pair("w" + to_string(j), weights));
        }

        int serialized_buffer_size = 0;
        uint8_t* serialized_params = serialize(accumulator, &serialized_buffer_size);

        encrypted_accumulator[i] = new uint8_t*[3 * sizeof(uint8_t*)];
        encrypted_accumulator[i][0] = new uint8_t[serialized_buffer_size];
        encrypted_accumulator[i][1] = new uint8_t[CIPHER_IV_SIZE];
        encrypted_accumulator[i][2] = new uint8_t[CIPHER_TAG_SIZE];

        encrypt_bytes(serialized_params, serialized_buffer_size, encrypted_accumulator[i]);
        accumulator_lengths[i] = serialized_buffer_size;
    }

    map<string, vector<float>> old_params;
    for (int j = 0; j < num_weights; j++) {
        vector<float> weights;
        for (int k = 0; k < weights_length; k++) {
            weights.push_back(-(1 + j + k) * (int) accumulator_length);
        }
        old_params.insert(make_pair("w" + to_string(j), weights));
    }
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
    const clock_t begin_time = clock();
    int error = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            serialized_old_params_buffer_size,
            encrypted_new_params_ptr,
            new_params_length);
    cout << "Time for host_modelaggregator to run: " << double(clock() - begin_time) /  CLOCKS_PER_SEC << "s" << endl;

    /*
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
    */

    uint8_t** encrypted_new_params = *encrypted_new_params_ptr;
    uint8_t* serialized_new_params = new uint8_t[*new_params_length * sizeof(uint8_t)];
    decrypt_bytes(encrypted_new_params[0], 
            encrypted_new_params[1], 
            encrypted_new_params[2], 
            *new_params_length,
            &serialized_new_params);

    /*
    // Free memory
    for (int i = 0; i < accumulator_length; i++) {
        delete encrypted_new_params_ptr[i][0];
        delete encrypted_new_params_ptr[i][1];
        delete encrypted_new_params_ptr[i][2];
        delete encrypted_new_params_ptr[i];
    }
    */

    map<string, vector<float>> new_params = deserialize(serialized_new_params);

    for (const auto& pair : new_params) {
        if (pair.second.size() != weights_length) {
            return 1;
        }
        for (float x : pair.second) {
            if (x != 0) {
                return 1;
            }
        }
    }

    cout << "Before failing on purpose" << endl;
    return 1;
}
