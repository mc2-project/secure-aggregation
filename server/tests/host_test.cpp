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
                                                    {"w2", {i + 1, i + 2, i + 3, i + 4}},
                                                    {"w3", {i + 2, i + 3, i + 4, i + 5}},
                                                    {"_contribution", {1}}};
        string accumulator_s = serialize(accumulator);

        encrypted_accumulator[i] = new unsigned char*[3 * sizeof(unsigned char*)];
        encrypt_bytes((unsigned char*) accumulator_s.c_str(), accumulator_s.size(), encrypted_accumulator[i]);
        accumulator_lengths[i] = accumulator_s.size();
    }

    map<string, vector<double>> old_params = {{"w1", {-3, -6, -9, -12}}, 
                                                {"w2", {-6, -9, -12, -15}},
                                                {"w3", {-9, -12, -15, -18}}};
    string serialized_old_params = serialize(old_params);
    unsigned char** encrypted_old_params = new unsigned char*[3 * sizeof(unsigned char*)];
    size_t old_params_length = serialized_old_params.size();

    encrypt_bytes((unsigned char*) serialized_old_params.c_str(), old_params_length, encrypted_old_params);


    cout << "Calling into enclave_modelaggregator" << endl;
    unsigned char*** encrypted_new_params_ptr = new unsigned char**[3 * sizeof(unsigned char**)];
    size_t* new_params_length = new size_t;
    int error = host_modelaggregator(encrypted_accumulator, 
            accumulator_lengths, 
            accumulator_length, 
            encrypted_old_params, 
            old_params_length,
            encrypted_new_params_ptr,
            new_params_length);

    if (error > 0) {
        return error;
    }

    unsigned char** encrypted_new_params = *encrypted_new_params_ptr;
    unsigned char serialized_new_params[*new_params_length];
    decrypt_bytes(encrypted_new_params[0], 
            encrypted_new_params[1], 
            encrypted_new_params[2], 
            *new_params_length,
            (unsigned char**) &serialized_new_params);

    cout << "New Params: " << serialized_new_params << endl;
    map<string, vector<double>> params = deserialize(string((const char*) serialized_new_params));

    cout << "Right before throwing on purpose" << endl;
    return 1;
}
