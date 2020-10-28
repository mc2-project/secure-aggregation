// Include the trusted modelaggregator header that is generated
// during the build. This file is generated by calling the
// sdk tool oeedger8r against the modelaggregator.edl file.
#include "modelaggregator_t.h"

#include <stdio.h>
#include <vector>
#include <numeric>
#include <map>
#include <set>
#include <string>

// Include encryption/decryption and serialization/deserialization headers
#include "encryption/encrypt.h"
#include "encryption/serialization.h"

using namespace std;

#define check_host_buffer(ptr, size) {                    \
if (!oe_is_outside_enclave((ptr), size)) {                \
    fprintf(stderr,                                       \
            "%s:%d: Buffer bounds check failed\n",        \
            __FILE__, __LINE__);                          \
    exit(1);                                              \
}                                                         \
}

// Helper function to print out a map<string, vector<float>>
void print_map(map<string, vector<double>> dict) {
    for (const auto& pair : dict) {
        cout << pair.first << ": ";
        for (float x : pair.second) {
            cout << x << ", ";
        }
        cout << endl;
    }
}

// Helper function used to copy double pointers from untrusted memory to enclave memory
void copy_arr_to_enclave(char* dst[], size_t num, char* src[], size_t lengths[]) {
  for (int i = 0; i < num; i++) {
    size_t nlen = lengths[i];
    check_host_buffer(src[i], nlen);
    dst[i] = strndup(src[i], nlen);
    dst[i][nlen] = '\0';
  }
}

// This is the function that the host calls. It performs
// the aggregation and encrypts the new model to pass back.
void enclave_modelaggregator(unsigned char*** encrypted_accumulator,
            size_t* accumulator_lengths,
            size_t accumulator_length, 
            unsigned char** encrypted_old_params, 
            size_t old_params_length, 
            unsigned char*** encrypted_new_params_ptr,
            size_t* new_params_length)
{
    size_t encryption_metadata_length = 3;

    unsigned char* encrypted_old_params_cpy[encryption_metadata_length];
    size_t lengths[] = {old_params_length * sizeof(unsigned char), CIPHER_IV_SIZE + 1, CIPHER_TAG_SIZE};
    copy_arr_to_enclave((char**) encrypted_old_params_cpy,
            encryption_metadata_length, 
            (char**) encrypted_old_params,
            lengths);

    unsigned char* serialized_old_params = new unsigned char[old_params_length * sizeof(unsigned char)];
    decrypt_bytes(encrypted_old_params[0],
            encrypted_old_params[1],
            encrypted_old_params[2],
            old_params_length,
            &serialized_old_params);

    map<string, vector<double>> params = deserialize(string((const char*) serialized_old_params));

    
    vector<map<string, vector<double>>> accumulator;
    set<string> vars_to_aggregate;

    for (int i = 0; i < accumulator_length; i++) {
        unsigned char* decrypted_accumulator = new unsigned char[accumulator_lengths[i] * sizeof(unsigned char)];
        decrypt_bytes(encrypted_accumulator[i][0],
                encrypted_accumulator[i][1],
                encrypted_accumulator[i][2],
                accumulator_lengths[i],
                &decrypted_accumulator);

        map<string, vector<double>> params = deserialize(string((const char*) decrypted_accumulator));

        for (const auto& pair : params) {
            if (pair.first != "_contribution") {
                vars_to_aggregate.insert(pair.first);
            }
        }

        accumulator.push_back(params);
        delete decrypted_accumulator;
    }

    for (string v_name : vars_to_aggregate) {
        vector<double> n_local_iters;
        vector<vector<double>> vars;

        for (map<string, vector<double>> acc_params : accumulator) {
            if (acc_params.find(v_name) == acc_params.end()) { // these params don't have the variable from client
                continue;
            }

            // Each params map will have an additional key "_contribution" to hold the number of local iterations
            double n_iter = acc_params["_contribution"][0];
            n_local_iters.push_back(n_iter);

            // Weighted using local iterations
            vector<double>& weights = acc_params[v_name];
            for_each(weights.begin(), weights.end(), [&n_iter](double& d) { d *= n_iter; });
            vars.push_back(weights);
        }

        if (n_local_iters.empty()) {
            continue; // Didn't receive this variable from any clients
        }

        vector<double> new_val(vars.size(), 0.0);
        double iters_sum = accumulate(n_local_iters.begin(), n_local_iters.end(), 0);
        for (int i = 0; i < new_val.size(); i++) {
            for (vector<double> weights : vars) {
                new_val[i] += weights[i];
            }
            new_val[i] /= iters_sum;
        }
        params[v_name] = new_val;
    }

    string serialized_new_params = serialize(params);
    cout << serialized_new_params << endl;

    unsigned char** encrypted_new_params = new unsigned char*[encryption_metadata_length * sizeof(unsigned char*)];
    encrypt_bytes((unsigned char*) serialized_new_params.c_str(), serialized_new_params.size(), encrypted_new_params);

    // Need to copy over the encrypted model, IV, and tag over to untrusted memory
    *encrypted_new_params_ptr = (unsigned char**) oe_host_malloc(encryption_metadata_length * sizeof(unsigned char*));
    *new_params_length = serialized_new_params.size();
    for (int i = 0; i < encryption_metadata_length; i++) {
        size_t item_length = i == 0 ? *new_params_length : strlen((const char *) encrypted_new_params[i]);
        (*encrypted_new_params_ptr)[i] = (unsigned char*) oe_host_malloc((item_length + 1) * sizeof(unsigned char));
        memcpy((void *) (*encrypted_new_params_ptr)[i], (const void*) encrypted_new_params[i], item_length * sizeof(unsigned char));
    }

    cout << "End of enclave function" << endl;
}
