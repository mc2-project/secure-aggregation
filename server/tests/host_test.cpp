#include <stdio.h>

#include "../host/host.cpp"

using namespace std;

int main(int argc, char* argv[]) 
{
    unsigned char** encrypted_new_params = host_modelaggregator(NULL, NULL, NULL, NULL, NULL);
    if (encrypted_new_params == NULL) {
        return 1;
    }

    return 0;
}
