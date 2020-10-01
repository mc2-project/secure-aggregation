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

unsigned char** encrypt_bytes(std::string model_data) {

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    size_t data_len = model_data.length();
    unsigned char key[] = "abcdefghijklmnop";

    unsigned char* output = (unsigned char*) malloc(data_len * sizeof(unsigned char));
    unsigned char* iv = (unsigned char*) malloc(CIPHER_IV_SIZE * sizeof(unsigned char));
    unsigned char* tag = (unsigned char*) malloc(CIPHER_TAG_SIZE * sizeof(unsigned char));

    int ret = encrypt_symm(
        key,
        reinterpret_cast<const unsigned char*> (model_data.c_str()),
        data_len,
        NULL,
        0,
        output,
        iv,
        tag
    );

    unsigned char** out_iv_tag = (unsigned char**) malloc(3 * sizeof(unsigned char*));
    *out_iv_tag = output;
    *(out_iv_tag + 1) = iv;
    *(out_iv_tag + 2) = tag;

    return out_iv_tag;
}

unsigned char* decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len) {
  
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
        out
    );

    return out;
}

#endif
