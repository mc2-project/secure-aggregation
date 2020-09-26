/*
* Modifications Copyright (c) 2020 by Secure XGBoost Contributors
*/

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <cstring>
#include <stdio.h>
#include <string>
#include <stdint.h>

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

#define CIPHER_KEY_SIZE 16
#define CIPHER_IV_SIZE  12
#define CIPHER_TAG_SIZE 16
#define SHA_DIGEST_SIZE 32
#define CIPHER_PK_SIZE 512
#define SIG_ALLOC_SIZE 1024


// static int print_bytes(uint8_t* data, size_t len) {
//   for (int i = 0; i < len; i++)
//     std::cout << (int) data[i] << " ";
//   std::cout << std::endl;
// }

// static int generate_random(unsigned char* key, unsigned int key_len) {
//     mbedtls_ctr_drbg_context ctr_drbg;
//     mbedtls_entropy_context entropy;
//     const char pers[] = "Generate random key for MC^2";
//     int ret = 0;

//     mbedtls_entropy_init(&entropy);
//     mbedtls_ctr_drbg_init(&ctr_drbg);
//     memset(key, 0, key_len);

//     // mbedtls_ctr_drbg_seed seeds and sets up the CTR_DRBG entropy source for
//     // future reseeds.
//     ret = mbedtls_ctr_drbg_seed(
//         &ctr_drbg,
//         mbedtls_entropy_func,
//         &entropy,
//         (unsigned char*)pers,
//         sizeof(pers));
//     if (ret != 0) {
//         LOG(FATAL) << "mbedtls_ctr_drbg_init failed with " << -ret;
//     }

//     // mbedtls_ctr_drbg_random uses CTR_DRBG to generate random data
//     ret = mbedtls_ctr_drbg_random(&ctr_drbg, key, key_len);
//     if (ret != 0) {
//         LOG(FATAL) << "mbedtls_ctr_drbg_random failed with " << -ret;
//     }

//     mbedtls_ctr_drbg_free(&ctr_drbg);
//     mbedtls_entropy_free(&entropy);
//     return ret;
// }

static int cipher_init(mbedtls_gcm_context* gcm, unsigned char* key) {
  // Initialize GCM context (just makes references valid) - makes the context ready for mbedtls_gcm_setkey()
  mbedtls_gcm_init(gcm);
  int ret = mbedtls_gcm_setkey(
      gcm,                       // GCM context to be initialized
      MBEDTLS_CIPHER_ID_AES,     // cipher to use (a 128-bit block cipher)
      key,                       // encryption key
      CIPHER_KEY_SIZE * 8);      // key bits (must be 128, 192, or 256)
  if( ret != 0 ) {
    printf( "mbedtls_gcm_setkey failed to set the key for AES cipher - returned -0x%04x\n", -ret);
  }
  return ret;
}

static int encrypt_symm(unsigned char* key, const unsigned char* data, size_t data_len, unsigned char* aad, size_t aad_len, unsigned char* output, unsigned char* iv, unsigned char* tag) {
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;
  mbedtls_gcm_context gcm;

  // Initialize the entropy pool and the random source
  mbedtls_entropy_init( &entropy );
  mbedtls_ctr_drbg_init( &ctr_drbg );
  // The personalization string should be unique to your application in order to add some
  // personalized starting randomness to your random sources.
  std::string pers = "aes generate key for MC^2";
  // CTR_DRBG initial seeding Seed and setup entropy source for future reseeds
  int ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *)pers.c_str(), pers.length() );
  if( ret != 0 ) {
    printf( "mbedtls_ctr_drbg_seed() failed - returned --x%04x\n", -ret);
    return ret;
  }

  // Initialize the GCM context with our key and desired cipher
  ret = cipher_init(&gcm, key);
  if( ret != 0 ) {
    printf( "mbedtls_gcm_setkey failed to set the key for AES cipher - returned -0x%04x\n", -ret );
    return ret;
  }
  // Extract data for your IV, in this case we generate 12 bytes (96 bits) of random data
  ret = mbedtls_ctr_drbg_random( &ctr_drbg, iv, CIPHER_IV_SIZE );
  if( ret != 0 ) {
    printf( "mbedtls_ctr_drbg_random failed to extract IV - returned -0x%04x\n", -ret );
    return ret;
  }

  ret = mbedtls_gcm_crypt_and_tag( 
      &gcm,                                       // GCM context
      MBEDTLS_GCM_ENCRYPT,                        // mode
      data_len,                                   // length of input data
      iv,                                         // initialization vector
      CIPHER_IV_SIZE,                             // length of IV
      aad,                                        // additional data
      aad_len,                                    // length of additional data
      data,                                       // buffer holding the input data
      output,                                     // buffer for holding the output data
      CIPHER_TAG_SIZE,                            // length of the tag to generate
      tag);                                       // buffer for holding the tag
  if( ret != 0 ) {
    printf( "mbedtls_gcm_crypt_and_tag failed to encrypt the data - returned -0x%04x\n", -ret );
  }
  return ret;
}

// static int encrypt_symm(mbedtls_gcm_context* gcm, mbedtls_ctr_drbg_context* ctr_drbg, const unsigned char* data, size_t data_len, unsigned char* aad, size_t aad_len, unsigned char* output, unsigned char* iv, unsigned char* tag) {

//     // Extract data for your IV, in this case we generate 12 bytes (96 bits) of random data
//     int ret = mbedtls_ctr_drbg_random(ctr_drbg, iv, CIPHER_IV_SIZE);
//     if( ret != 0 ) {
//         //printf( "mbedtls_ctr_drbg_random failed to extract IV - returned -0x%04x\n", -ret );
//         LOG(FATAL) << "mbedtls_ctr_drbg_random failed to extract IV - returned " << -ret;
//     }

//     ret = mbedtls_gcm_crypt_and_tag( 
//             gcm,                                        // GCM context
//             MBEDTLS_GCM_ENCRYPT,                        // mode
//             data_len,                                   // length of input data
//             iv,                                         // initialization vector
//             CIPHER_IV_SIZE,                             // length of IV
//             aad,                                        // additional data
//             aad_len,                                    // length of additional data
//             data,                                       // buffer holding the input data
//             output,                                     // buffer for holding the output data
//             CIPHER_TAG_SIZE,                            // length of the tag to generate
//             tag);                                       // buffer for holding the tag
//     if( ret != 0 ) {
//         //printf( "mbedtls_gcm_crypt_and_tag failed to encrypt the data - returned -0x%04x\n", -ret );
//         LOG(FATAL) << "mbedtls_gcm_crypt_and_tag failed to encrypt the data - returned " << -ret;
//     }
//     return ret;
// }

static int decrypt_symm(unsigned char* key, const unsigned char* data, size_t data_len, unsigned char* iv, unsigned char* tag, unsigned char* aad, size_t aad_len, unsigned char* output) {
  mbedtls_gcm_context gcm;

  // Initialize the GCM context with our key and desired cipher
  int ret = cipher_init(&gcm, key);
  if( ret != 0 ) {
    printf("failed to set key for AES Cipher");
    return ret;
  }

  ret = mbedtls_gcm_auth_decrypt(
      &gcm,                                     // GCM context
      data_len,                                 // length of the input ciphertext data (always same as plain)
      iv,                                       // initialization vector
      CIPHER_IV_SIZE,                           // length of IV
      aad,                                      // additional data
      aad_len,                                  // length of additional data
      tag,                                      // buffer holding the tag
      CIPHER_TAG_SIZE,                          // length of the tag
      data,                                     // buffer holding the input ciphertext data
      output);                                  // buffer for holding the output decrypted data
  if (ret != 0) {
    printf( "mbedtls_gcm_auth_decrypt failed with error -0x%04x\n", -ret);
  }
  return ret;
}

static int decrypt_symm(mbedtls_gcm_context* gcm, const unsigned char* data, size_t data_len, unsigned char* iv, unsigned char* tag, unsigned char* aad, size_t aad_len, unsigned char* output) {

  int ret = mbedtls_gcm_auth_decrypt(
      gcm,                                      // GCM context
      data_len,                                 // length of the input ciphertext data (always same as plain)
      iv,                                       // initialization vector
      CIPHER_IV_SIZE,                           // length of IV
      aad,                                      // additional data
      aad_len,                                  // length of additional data
      tag,                                      // buffer holding the tag
      CIPHER_TAG_SIZE,                          // length of the tag
      data,                                     // buffer holding the input ciphertext data
      output);                                  // buffer for holding the output decrypted data
  if (ret != 0) {
    printf( "mbedtls_gcm_auth_decrypt failed with error -0x%04x\n ", -ret);
  }
  return ret;
}

static int compute_sha256(const uint8_t* data, size_t data_size, uint8_t sha256[SHA_DIGEST_SIZE]) {
  int ret = 0;
  mbedtls_sha256_context ctx;

#define safe_sha(call) {                  \
  int ret = (call);                       \
  if (ret) {                              \
    mbedtls_sha256_free(&ctx);            \
    return -1;                            \
  }                                       \
}
  mbedtls_sha256_init(&ctx);
  safe_sha(mbedtls_sha256_starts_ret(&ctx, 0));
  safe_sha(mbedtls_sha256_update_ret(&ctx, data, data_size));
  safe_sha(mbedtls_sha256_finish_ret(&ctx, sha256));

  mbedtls_sha256_free(&ctx);
  return ret;
}

static int verifySignature(mbedtls_pk_context pk, uint8_t* data, size_t data_len, uint8_t* signature, size_t sig_len) {
  unsigned char hash[SHA_DIGEST_SIZE];
  int ret = 0;

  if(!mbedtls_pk_can_do(&pk, MBEDTLS_PK_RSA)) {
    printf("Verification failed - Key is not an RSA Key\n");
    return ret;
  }

  mbedtls_rsa_set_padding( mbedtls_pk_rsa( pk ), MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256 );

  if((ret = compute_sha256(data, data_len, hash)) != 0) {
    printf("Verification failed -- Could not hash returned --x%04x\n ", -ret);
    return ret;
  }

  if((ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, 0, signature, sig_len)) != 0 ) {
    printf("Verification failed - mbedtls_pk_verify returned --x%04x\n", -ret);
    return ret;
  }

  return ret;
}

static int sign_data(mbedtls_pk_context pk, uint8_t* data, size_t data_size, uint8_t* signature, size_t* sig_len) {
  mbedtls_entropy_context m_entropy_context;
  mbedtls_ctr_drbg_context m_ctr_drbg_context;

  mbedtls_entropy_init( &m_entropy_context );
  mbedtls_ctr_drbg_init( &m_ctr_drbg_context );

  unsigned char hash[32];
  int ret = 1;

  ret = mbedtls_ctr_drbg_seed(&m_ctr_drbg_context, mbedtls_entropy_func, &m_entropy_context, NULL, 0);

  if(!mbedtls_pk_can_do(&pk, MBEDTLS_PK_RSA)) {
    printf("Signing failed -- Key is not an RSA Key\n");
    return ret;
  }

  mbedtls_rsa_set_padding(mbedtls_pk_rsa(pk), MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256 );

  if((ret = compute_sha256(data, data_size, hash)) != 0) {
    printf("Signing failed - could not hash returned --x%04x\n ", -ret);
    return ret;
  }

  if((ret = mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, hash, 0, signature, sig_len, mbedtls_ctr_drbg_random, &m_ctr_drbg_context)) != 0) {
    printf("Signing failed -- mbedtls_pk_sign returned returned --x%04x\n ", -ret);
    return ret;
  }

  return 0;
}
#endif // CRYPTO_H_