#include <vector>
#include "flatbuffers/model_generated.h"
#include "encryption/encrypt.h"
#include <iostream>
#include "utils.h"
#include "host.h"

extern "C" void api_aggregate(uint8_t** encrypted_accumulator, size_t* accumulator_lengths,
        size_t accumulator_length, uint8_t* encrypted_old_params, size_t old_params_length,
        uint8_t** encrypted_new_params_ptr, size_t* new_params_length, float* contributions) {

    host_modelaggregator(encrypted_accumulator,
            accumulator_lengths,
            accumulator_length,
            encrypted_old_params,
            old_params_length,
            encrypted_new_params_ptr,
            new_params_length,
            contributions);
}

// Called from client code only
extern "C" void api_serialize(char* keys[], float* values[], int* num_floats_per_feature, int num_kvpairs, uint8_t** serialized_buffer, int* serialized_buffer_size) {
    // keys / values make up the map in the above serialize() function
    // num_kvpairs is the number of items in the map
    // feature_lens is the number of floats in each vector (the value of each kv pair)
    
    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<secagg::KVPair>> features;

    int num_floats_seen = 0;

    for (int i = 0; i < num_kvpairs; i++) {
        std::string name = keys[i];
        auto key = builder.CreateString(name);

        int this_feature_len = num_floats_per_feature[i];
        std::vector<float> feature_values(values[i], values[i] + this_feature_len);
        auto value = builder.CreateVector(feature_values);

        auto kvpair = secagg::CreateKVPair(builder, key, value);
        features.push_back(kvpair);
    }
    auto model_features = builder.CreateVector(features);
    auto model_offset = secagg::CreateModel(builder, model_features);
    builder.Finish(model_offset);

    uint8_t* model_buffer = builder.GetBufferPointer();
    int model_buffer_size = builder.GetSize();

    // TODO: do we even need to copy this over?
    uint8_t* ret_buffer = (uint8_t*) malloc(model_buffer_size * sizeof(uint8_t));
    memcpy(ret_buffer, model_buffer, sizeof(uint8_t) * model_buffer_size);
    *serialized_buffer = ret_buffer;
    *serialized_buffer_size = model_buffer_size;
}

// Deserialize and return keys of map
extern "C" void api_deserialize_keys(uint8_t* serialized_buffer, char*** ret_keys, int* ret_num_kvs ) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    // char** names = new char*[num_kvs];
    char** names = (char**) malloc(num_kvs * sizeof(char*));
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        // Key is a string
        auto key = pair->key()->str();
        size_t key_length = key.length();

        names[i] = (char*) malloc((key_length + 1) * sizeof(char));
        memcpy(names[i], key.c_str(), key_length + 1);
    }
    *ret_keys = names;
    *ret_num_kvs = num_kvs;
}

// Deserialize and return values of map
extern "C" void api_deserialize_values(uint8_t* serialized_buffer, float*** ret_values, int** ret_num_floats_per_value, int* ret_num_kvs) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    float** features_vals = (float**) malloc(num_kvs * sizeof(float*));
    int* num_floats_per_feature = (int*) malloc(num_kvs * sizeof(int));
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        auto value = pair->value();
        int num_values = value->size();
        for (int j = 0; j < num_values; j++) {
            auto feature_value = value->Get(j);
            feature_values.push_back(feature_value);
        }
        features_vals[i] = (float*) malloc(num_values * sizeof(float));
        memcpy(features_vals[i], feature_values.data(), num_values * sizeof(float));
        num_floats_per_feature[i] = num_values;
    }
    *ret_values = features_vals;
    *ret_num_floats_per_value = num_floats_per_feature;
    *ret_num_kvs = num_kvs;
}

extern "C" void api_free_ptr(void* ptr) {
    free(ptr);
}

extern "C" void api_free_double_ptr(void** ptr, int num_ptrs) {
    for (int i = 0; i < num_ptrs; i++) {
        free(ptr[i]);
    }
    free(ptr);
}

extern "C" void api_encrypt_bytes(uint8_t* model_data, size_t data_len, uint8_t** ciphertext) {
    uint8_t* ctext = (uint8_t*) malloc((data_len + CIPHER_IV_SIZE + CIPHER_TAG_SIZE) * sizeof(uint8_t));
    encrypt_bytes(model_data, data_len, &ctext);
    *ciphertext = ctext;
}

extern "C" void api_decrypt_bytes(uint8_t* model_data, uint8_t* iv, uint8_t* tag, size_t data_len, uint8_t** text) {
    uint8_t* plaintext = (uint8_t*) malloc(data_len * sizeof(uint8_t));
    decrypt_bytes(model_data, iv, tag, data_len, &plaintext);
    *text = plaintext;
}
