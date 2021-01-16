#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include "crypto.h"

#include "mbedtls/config.h"
#include "mbedtls/gcm.h"
#include "mbedtls/entropy.h"    // mbedtls_entropy_context
#include "mbedtls/ctr_drbg.h"   // mbedtls_ctr_drbg_context
#include "mbedtls/cipher.h"     // MBEDTLS_CIPHER_ID_AES
#include "mbedtls/gcm.h"        // mbedtls_gcm_context
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"

extern "C" void encrypt_bytes(uint8_t* model_data, size_t data_len, uint8_t** ciphertext) {
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

extern "C" void decrypt_bytes(uint8_t* model_data, uint8_t* iv, uint8_t* tag, size_t data_len, uint8_t** text) {
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
