
#include <iostream> 
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "serialization.h"
#include "encrypt.h"

int main() {

    // TODO: FUNCTION TO CONVERT PYTHON MAP TO C++ (PROTOBUF)

    // CREATE MAP
    std::map<std::string, std::vector<double>> model {
        {"feats1", {}},
        {"feats2", {0.9, 0.1, 0.2, 1.1}},
        {"feats3", {-1.1, 1.9, 1.5, 1.3, 1.5, 1.66195999}},
        {"feats4", {1.1, 1.24921, 1.3}}
    };
    
    // SERIALIZATION
    std::string serialized = serialize(model);
    size_t data_len = serialized.length();
    // std::cout << "SERIALIZZED STRING: " << serialized << std::endl;

    // ENCRYPTION
    std::vector<unsigned char*> encrypted = encrypt_bytes(serialized);

    // DECRYPTION
    unsigned char* decrypted = decrypt_bytes(encrypted[0], encrypted[1], encrypted[2], data_len);
    // std::cout << "OUTPUT POST-DECRYPTION: " << decrypted << std::endl;

    // DESERIALIZATION
    std::map<std::string, std::vector<double>> deserialized = deserialize(serialized);

    // PRINT MAP VALUES
    for (const auto &[name, values]: deserialized) {
        std::cout << name << ": ";
        for (const double &value: values) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}