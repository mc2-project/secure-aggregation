#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <iostream> 
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "flatbuffers/model_generated.h"

uint8_t* serialize(std::map<std::string, std::vector<double>> model,
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

    *serialized_buffer_size = model_buffer_size;
    std::cout << "buffer size: " << model_buffer_size << std::endl;
    return model_buffer;
}

std::map<std::string, std::vector<double>> deserialize(uint8_t* serialized_buffer) {
    std::map<std::string, std::vector<double>> demodel;

    auto model = secagg::GetModel(serialized_buffer);
    auto kvpairs = model->kv();
    std::cout << "Deserialize: Getting num kvs" << std::endl;
    auto num_kvs = kvpairs->size();
    std::cout << "About to start for loop" << std::endl;
    for (int i = 0; i < num_kvs; i++) {
        std::cout << "Got size: " << num_kvs << std::endl;
        std::vector<double> feature_values;
        auto pair = kvpairs->Get(i);

        // Key is a string
        auto key = pair->key()->str();
        std::cout << "Feature: " << key << std::endl;
        auto value = pair->value();
        for (int j = 0; j < value->size(); j++) {
            auto feature_value = value->Get(j);
            feature_values.push_back(feature_value);
        }
        demodel.insert({key, feature_values});
    }
    return demodel;

}

#endif 
