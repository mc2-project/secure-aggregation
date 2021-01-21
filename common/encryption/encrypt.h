#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include "crypto.h"

void encrypt_bytes(uint8_t* model_data, size_t data_len, uint8_t** ciphertext) {
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    // FIXME: hardcoded key
    uint8_t key[] = "abcdefghijklmnop";

    uint8_t* output = (uint8_t*) malloc(data_len * sizeof(uint8_t));
    uint8_t* iv = (uint8_t*) malloc(CIPHER_IV_SIZE * sizeof(uint8_t));
    uint8_t* tag = (uint8_t*) malloc(CIPHER_TAG_SIZE * sizeof(uint8_t));

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

    memcpy(*ciphertext, output, data_len);
    memcpy(*ciphertext + data_len, iv, CIPHER_IV_SIZE);
    memcpy(*ciphertext + data_len + CIPHER_IV_SIZE, tag, CIPHER_TAG_SIZE);

    free(output);
    free(iv);
    free(tag);
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
