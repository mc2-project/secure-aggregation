# cython: language_level=3

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map as mapcpp
from libc.stdlib cimport malloc, free
from ctypes import c_ubyte

cdef extern from "../common/encryption/serialization.h":
    unsigned char* serialize(mapcpp[string, vector[double]] model, int* serialized_buffer_size)
    mapcpp[string, vector[double]] deserialize(unsigned char* serialized_buffer)

cdef extern from "../common/encryption/encrypt.h":
    void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext)
    void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char** text)

def encrypt(model):
    cdef int buffer_len = 0
    print('STARTING CYTHON ENCRYPT')
    serialized_model = serialize(model, &buffer_len)
    print('SERIALIZED MODEL LENGTH: ', len(serialized_model))
    print('SERIALIZED BUFFER LENGTH: ', buffer_len)
    cdef bytes serialized_buffer = serialized_model[:buffer_len]
    print('serialized byte array length: ', len(serialized_buffer))
    ciphertext, iv, tag = cpp_encrypt_bytes(serialized_buffer, buffer_len)
    print('model encrypted')
    return ciphertext, iv, tag

def cpp_encrypt_bytes(model_data, data_len):
    print('Initializing buffers')
    cdef unsigned char** ciphertext = <unsigned char**> malloc(3 * sizeof(unsigned char*))
    ciphertext[0] = <unsigned char*> malloc(data_len * sizeof(unsigned char))
    ciphertext[1] = <unsigned char*> malloc(12 * sizeof(unsigned char))
    ciphertext[2] = <unsigned char*> malloc(16 * sizeof(unsigned char))

    print('Starting Encryption')
    encrypt_bytes(model_data, data_len, ciphertext)

    print('Filling buffers')
    cdef bytes output = ciphertext[0][:data_len]
    cdef bytes iv = ciphertext[1][:12]
    cdef bytes tag = ciphertext[2][:16]
    
    print('Freeing Memory')
    free(ciphertext[0])
    free(ciphertext[1])
    free(ciphertext[2])
    free(ciphertext)
    return output, iv, tag

def decrypt(model_data, iv, tag, data_len):
    cdef unsigned char* plaintext = <unsigned char*> malloc(data_len * sizeof(unsigned char))
    decrypt_bytes(model_data, iv, tag, data_len, &plaintext)
    model = deserialize(plaintext)
    free(plaintext)
    return model
    