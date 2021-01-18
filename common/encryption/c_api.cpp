#include <map>
#include <vector>
#include "flatbuffers/model_generated.h"
#include "encrypt.h"
#include <iostream>
#include "utils.h"

// TODO: create serialization function that takes in two arrays instead of a map
// Called from client code only
extern "C" uint8_t* api_serialize(char* keys[], float* values[], int num_kvpairs, int* serialized_buffer_size) {
    // keys / values make up the map in the above serialize() function
    // num_kvpairs is the number of items in the map
    // feature_lens is the number of floats in each vector (the value of each kv pair)
    //
    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<secagg::KVPair>> features;

    int num_floats_seen = 0;

    // for (const auto &[name, values]: model) {
    for (int i = 0; i < num_kvpairs; i++) {
        std::string name = keys[i];
        auto key = builder.CreateString(name);

        int this_feature_len = sizeof(values[i]) / sizeof(float);
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

    // FIXME: memory leak
    uint8_t* ret_buffer = new uint8_t[model_buffer_size];
    memcpy(ret_buffer, model_buffer, sizeof(uint8_t) * model_buffer_size);
    *serialized_buffer_size = model_buffer_size;
    return ret_buffer;
}

// TODO create deserialization function that returns two arrays instead of a map
// Deserialize and return keys of map
extern "C" char** api_deserialize_keys(uint8_t* serialized_buffer, int* ret_num_kvs ) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    char** names = new char*[num_kvs];
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        // Key is a string
        auto key = pair->key()->str();
        size_t key_length = key.length();

        // FIXME: memory leak
        names[i] = new char[key_length];
        strcpy(names[i], key.c_str());
    }
    *ret_num_kvs = num_kvs;
    for (int i = 0; i < num_kvs; i++) {
        std::cout << names[i] << std::endl;
    }
    return names;

}

// Deserialize and return values of map
extern "C" float** api_deserialize_values(uint8_t* serialized_buffer, int* ret_num_kvs) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    float** features_vals = new float*[num_kvs];
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        auto value = pair->value();
        auto num_values = value->size();
        for (int j = 0; j < value->size(); j++) {
            auto feature_value = value->Get(j);
            feature_values.push_back(feature_value);
        }
        features_vals[i] = new float[num_values];
        memcpy(features_vals[i], feature_values.data(), num_values * sizeof(float));
    }
    *ret_num_kvs = num_kvs;
    return features_vals;

}

extern "C" void api_encrypt_bytes(uint8_t* model_data, size_t data_len, uint8_t** ciphertext) {
    encrypt_bytes(model_data, data_len, ciphertext);
}

extern "C" void api_decrypt_bytes(uint8_t* model_data, uint8_t* iv, uint8_t* tag, size_t data_len, uint8_t** text) {
    decrypt_bytes(model_data, iv, tag, data_len, text);
}
