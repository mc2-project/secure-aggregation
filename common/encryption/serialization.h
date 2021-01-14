#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <map>
#include <vector>
#include "flatbuffers/model_generated.h"

void get_str_lengths(char** arr, size_t size, size_t* lengths) {
  for (int i = 0; i < size; i++) {
    lengths[i] = strlen(arr[i]);
  }
}

uint8_t* serialize(std::map<std::string, std::vector<float>> model,
                            int* serialized_buffer_size) {
    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<secagg::KVPair>> features;

    for (const auto &[name, values]: model) {
        auto key = builder.CreateString(name);
        auto value = builder.CreateVector(values);
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

// TODO: create serialization function that takes in two arrays instead of a map
// Called from client code only
uint8_t* serialize(char* keys[], float* values[], int num_kvpairs, int* serialized_buffer_size) {
    // keys / values make up the map in the above serialize() function
    // num_kvpairs is the number of items in the map
    // feature_lens is the number of floats in each vector (the value of each kv pair)
    
    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<secagg::KVPair>> features;

    int num_floats_seen = 0;

    // for (const auto &[name, values]: model) {
    for (int i = 0; i < num_kvpairs; i++) {
        std::string name = keys[i].c_str();
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

std::map<std::string, std::vector<float>> deserialize(uint8_t* serialized_buffer) {
    std::map<std::string, std::vector<float>> demodel;

    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        // Key is a string
        auto key = pair->key()->str();
        auto value = pair->value();
        for (int j = 0; j < value->size(); j++) {
            auto feature_value = value->Get(j);
            feature_values.push_back(feature_value);
        }
        demodel.insert({key, feature_values});
    }
    return demodel;

}

// TODO create deserialization function that returns two arrays instead of a map
// Deserialize and return keys of map
char** deserialize(uint8_t* serialized_buffer) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    char* names[num_kvs];
    for (int i = 0; i < num_kvs; i++) {
        std::vector<float> feature_values;
        auto pair = kvpairs->Get(i);

        // Key is a string
        auto key = pair->key()->str();
        size_t key_length = key.length();
        names[i] = new char[key_length];
        strcpy(names[i], key.c_str());
    }
    return names;

}

// Deserialize and return values of map
float** deserialize(uint8_t* serialized_buffer) {
    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    auto num_kvs = kvpairs->size();
    
    float* features_vals[num_kvs];
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
    return features_vals;

}
#endif 
