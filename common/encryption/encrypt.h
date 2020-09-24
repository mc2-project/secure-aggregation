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

unsigned char* encrypt_bytes(std::string model_data) {

    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    size_t data_len = model_data.length();
    char key[] = "abcdefghijklmnop";
    char iv[] = "123456789012";
    char tag[] = "ABCDEFGHIJKLMNOP";

    unsigned char* output = new unsigned char[data_len];

    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, (const unsigned char*) key, strlen(key)*8);
    mbedtls_gcm_starts(&gcm, MBEDTLS_GCM_ENCRYPT, (const unsigned char*)iv, strlen(iv), NULL, 0);
    mbedtls_gcm_update(&gcm, data_len, reinterpret_cast<const unsigned char*> (model_data.c_str()), output);
    mbedtls_gcm_free(&gcm);

    // cout << "ENCRYPTED OUTPUT: " << output << endl;
    return output;
}

unsigned char* decrypt_bytes(unsigned char* model_data) {
  
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    size_t data_len = strlen((char *) model_data);
    char key[] = "abcdefghijklmnop";
    char iv[] = "123456789012";
    char tag[] = "ABCDEFGHIJKLMNOP";

    unsigned char* output = new unsigned char[data_len];
    
    mbedtls_gcm_init(&gcm);
    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, (const unsigned char*) key, strlen(key)*8);
    mbedtls_gcm_starts(&gcm, MBEDTLS_GCM_DECRYPT, (const unsigned char*)iv, strlen(iv), NULL, 0);
    mbedtls_gcm_update(&gcm, data_len ,(const unsigned char*)model_data, output);
    mbedtls_gcm_free(&gcm);

    // cout << "DECRYPTED OUTPUT: " << output << endl;
    return output;

}

#endif
