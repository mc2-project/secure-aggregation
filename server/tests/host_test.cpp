#include <stdio.h>
#include <vector>
#include <numeric>
#include <map>
#include <string>

#include "../host/host.cpp"

#include "../../common/encryption/encrypt.h"
#include "../../common/encryption/serialization.h"

using namespace std;

int main(int argc, char* argv[]) 
{  
    size_t accumulator_length = 3;
    unsigned char*** encrypted_accumulator = new unsigned char**[accumulator_length * sizeof(unsigned char**)];
    size_t* accumulator_lengths = new size_t[accumulator_length * sizeof(size_t)];

    for (int i = 0; i < accumulator_length; i++) {
        map<string, vector<double>> accumulator = {{"w1", {i, i + 1, i + 2, i + 3}}, 
                                                    {"w2", {i + 1, i + 2, i + 3, i + 4}}};
        string accumulator_s = serialize(accumulator);

        *(encrypted_accumulator + i) = new unsigned char*[3 * sizeof(unsigned char*)];
        encrypt_bytes((unsigned char*) accumulator_s.c_str(), accumulator_s.size(), *(encrypted_accumulator + i));
        *(accumulator_lengths + i) = accumulator_s.size();
    }

    map<string, vector<double>> old_params = {{"w1", {8, 7, 6, 5}}, 
                                                {"w2", {4, 3, 2, 1}}};
    string serialized_old_params = serialize(old_params);
    unsigned char** encrypted_old_params = new unsigned char*[3 * sizeof(unsigned char*)];

    encrypt_bytes((unsigned char*) serialized_old_params.c_str(), serialized_old_params.size(), encrypted_old_params);
    size_t old_params_length = serialized_old_params.size();


    unsigned char** encrypted_new_params = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            old_params_length);

    if (encrypted_new_params == NULL) {
        return 1;
    }
    return 0;
}