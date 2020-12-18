#ifndef _UTILS_H
#define _UTILS_H
#include <vector>
#include <numeric>
#include <map>
#include <string>
#include <iostream>

using namespace std;

// Print out a map<string, vector<float>>
void print_map(map<string, vector<float>> dict) {
    for (const auto& pair : dict) {
        cout << pair.first << ": ";
        for (float x : pair.second) {
            cout << x << ", ";
        }
        cout << endl;
    }
}

void print_map_keys(map<string, vector<float>> dict) {
  for (const auto& pair : dict) {
    if (pair.first.length() > 20) continue;
    cout << pair.first << endl;
  }
}


// Print integers instead of bytes for encryption debugging. 
int print_bytes(uint8_t* data, size_t len) {
    for (int i = 0; i < len; i++) {
    cout << (int) data[i] << " ";
    }
  cout << endl;
}

// Delete a float pointer.
void delete_double_ptr(unsigned char** src, size_t num) {
    for (int i = 0; i < num; i++) {
        delete src[i];
    }
    delete src;
}
#endif _UTILS_H
