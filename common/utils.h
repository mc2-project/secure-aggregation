#include <stdio.h>
#include <vector>
#include <numeric>
#include <map>
#include <string>

using namespace std;

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

