# cython: language_level=3

from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.map cimport map as mapcpp
from libc.stdlib cimport malloc, free

cdef extern from "../common/encryption/serialization.h":
    string serialize(mapcpp[string, vector[double]] model)
    mapcpp[string, vector[double]] deserialize(string serialized_str)

cdef extern from "../common/encryption/encrypt.h":
    void encrypt_bytes(unsigned char* model_data, size_t data_len, unsigned char** ciphertext)
    void decrypt_bytes(unsigned char* model_data, unsigned char* iv, unsigned char* tag, size_t data_len, unsigned char* text)

def cy_serialize(model):
    model = serialize(model)
    return model

def cy_deserialize(serialized_str):
    model = deserialize(serialized_str)
    return model

def cy_encrypt_bytes(model_data, data_len):
    cdef unsigned char** ciphertext = <unsigned char**> malloc(3 * sizeof(unsigned char*))
    encrypt_bytes(model_data, data_len, ciphertext)
    return ciphertext[0], ciphertext[1], ciphertext[2]

def cy_decrypt_bytes(model_data, iv, tag, data_len):
    print(len(model_data), len(iv), len(tag), data_len)
    cdef unsigned char* text = <unsigned char*> malloc(data_len * sizeof(unsigned char))
    decrypt_bytes(model_data, iv, tag, data_len, text)
    return text

