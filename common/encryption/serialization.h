#ifndef SERIALIZATION_H_
#define SERIALIZATION_H_

#include <iostream> 
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "flatbuffers/model_generated.h"

using namespace secagg;

std::string serialize(std::map<std::string, std::vector<double>> model) {
    // Serialize string:float[] map into SSSS:[VVVVVVVV]

    std::vector<unsigned char> serialized;

    for (const auto &[name, values]: model) {
        for (char const &c: name) {
            serialized.push_back(c);
        }
        serialized.push_back(':');
        serialized.push_back('[');
        for (const double &val: values) {
            std::string val_string = std::to_string(val);
            for (char const &c: val_string) {
                serialized.push_back(c);
            }
            serialized.push_back(',');
        }
        serialized.push_back(']');
        serialized.push_back('/');
    }

    std::string s(serialized.begin(), serialized.end());
    return s;

    flatbuffers::FlatBufferBuilder builder(1024);
}

std::map<std::string, std::vector<double>> deserialize(std::string serialized_str) {
    // Splits each entry on '/'
    std::vector<std::string> entries; 
    std::string delimitEntries = "/";
    size_t pos = 0;

    while ((pos = serialized_str.find(delimitEntries)) != std::string::npos) {
        std:: string token = serialized_str.substr(0, pos);
        entries.push_back(token);
        serialized_str.erase(0, pos+delimitEntries.length());
    }

    // Recreate the original map entries
    std::map<std::string, std::vector<double>> demodel;
    for (const std::string &entry: entries) {
        std::string delimiter = ":";
        std::string key = entry.substr(0, entry.find(delimiter));
        std::string values = entry.substr(entry.find(delimiter), entry.length());
        // values.length()-3 to remove trailing characters at the end (:[ and ])
        values = values.substr(2, values.length()-3);
        std::vector<double> newValues;
        pos = 0;
        std::string delimitValues = ",";
        while ((pos = values.find(",")) != std::string::npos) {
            std::string value = values.substr(0, pos);
            double valueDouble = ::atof(value.c_str());
            newValues.push_back(valueDouble);
            values.erase(0, pos+delimitValues.length());
        }
        demodel.insert({key, newValues});
    }

    return demodel;
    // END DESERIALIZATION

}

#endif 
