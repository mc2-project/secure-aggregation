#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include "crypto.h"

void encrypt_bytes(uint8_t* model_data, size_t data_len, uint8_t** ciphertext) {
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    // FIXME: hardcoded key
    uint8_t key[] = "abcdefghijklmnop";

    uint8_t* output = ciphertext[0];
    uint8_t* iv = ciphertext[1];
    uint8_t* tag = ciphertext[2];

    int ret = encrypt_symm(
        key,
        model_data,
        data_len,
        NULL,
        0,
        output,
        iv,
        tag
    );
}

void decrypt_bytes(uint8_t* model_data, uint8_t* iv, uint8_t* tag, size_t data_len, uint8_t** text) {
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    // FIXME: hardcoded key
    uint8_t key[] = "abcdefghijklmnop";

    decrypt_symm(
        key,
        model_data,
        data_len,
        iv,
        tag,
        NULL,
        0,
        *text
    );
}

#endif
