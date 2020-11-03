#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#define CIPHER_KEY_SIZE 16
#define CIPHER_IV_SIZE  16
#define CIPHER_TAG_SIZE 16
#define SHA_DIGEST_SIZE 32
#define CIPHER_PK_SIZE 512
#define SIG_ALLOC_SIZE 1024

#include <iostream> 

void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext);
void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char** text);

#endif
