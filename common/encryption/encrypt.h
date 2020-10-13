#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#define CIPHER_KEY_SIZE 16
#define CIPHER_IV_SIZE  12
#define CIPHER_TAG_SIZE 16
#define SHA_DIGEST_SIZE 32
#define CIPHER_PK_SIZE 512
#define SIG_ALLOC_SIZE 1024

#include <iostream> 
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "serialization.h"
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

void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext) {

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    unsigned char key[] = "abcdefghijklmnop";

    unsigned char* output = new unsigned char[data_len * sizeof(unsigned char)];
    unsigned char* iv = new unsigned char[CIPHER_IV_SIZE * sizeof(unsigned char)];
    unsigned char* tag = new unsigned char[CIPHER_TAG_SIZE * sizeof(unsigned char)];

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

    *ciphertext = output;
    *(ciphertext + 1) = iv;
    *(ciphertext + 2) = tag;
}

void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char** text) {
  
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    unsigned char key[] = "abcdefghijklmnop";
    unsigned char* out = new unsigned char[data_len];

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
