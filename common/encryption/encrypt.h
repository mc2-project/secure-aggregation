#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <iostream> 

void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext);
void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char* text);

#endif
