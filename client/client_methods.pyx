# cython: language_level=3

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map as mapcpp
from libc.stdlib cimport malloc, free
from cpython.bytes cimport PyBytes_FromStringAndSize


cdef extern from "../common/encryption/serialization.h":
    unsigned char* serialize(mapcpp[string, vector[double]] model, int* serialized_buffer_size)
    mapcpp[string, vector[double]] deserialize(unsigned char* serialized_buffer)

cdef extern from "../common/encryption/encrypt.h":
    void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext)
    void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char** text)

def cy_serialize(model):
    cdef int buffer_len = 0
    serialized_model = serialize(model, &buffer_len)
    cdef bytes serialized_buffer = serialized_model[:buffer_len]
    print("deserializing in cy serialize")
    #  model = cy_deserialize(serialized_buffer)
    #  print("finished deserializing in cy serialize")
    ciphertext, iv, tag = cpp_encrypt_bytes(serialized_buffer, buffer_len)
    return ciphertext, iv, tag

def cy_deserialize(serialized_model):
    model = deserialize(serialized_model)
    return model

def cpp_encrypt_bytes(model_data, data_len):
    cdef unsigned char** ciphertext = <unsigned char**> malloc(3 * sizeof(unsigned char*))
    ciphertext[0] = <unsigned char*> malloc(data_len * sizeof(unsigned char))
    ciphertext[1] = <unsigned char*> malloc(12 * sizeof(unsigned char))
    ciphertext[2] = <unsigned char*> malloc(16 * sizeof(unsigned char))
    #  print("Encrypting a buffer of len: ", data_len)
    encrypt_bytes(model_data, data_len, ciphertext)
    cdef bytes output = ciphertext[0][:data_len]
    cdef bytes iv = ciphertext[1][:12]
    cdef bytes tag = ciphertext[2][:16]
    free(ciphertext[0])
    free(ciphertext[1])
    free(ciphertext[2])
    free(ciphertext)
    return output, iv, tag

def cy_decrypt_bytes(model_data, iv, tag, data_len):
    cdef unsigned char* text = <unsigned char*> malloc(data_len * sizeof(unsigned char))
    decrypt_bytes(model_data, iv, tag, data_len, &text)
    model = cy_deserialize(text)
    free(text)
    return model
    

