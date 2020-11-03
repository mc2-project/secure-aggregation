# cython: language_level=3
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.list cimport list as cpplist
from libcpp.map cimport map as mapcpp
from libc.stdlib cimport malloc, free
from cpython.string cimport PyString_AsString
from cpython.bytes cimport PyBytes_FromStringAndSize

cdef extern from "host.h":
    int host_modelaggregator(unsigned char*** encrypted_accumulator, 
            size_t* accumulator_lengths,
            size_t accumulator_length, 
            unsigned char** encrypted_old_params,
            size_t old_params_length,
            unsigned char*** encrypted_new_params_ptr,
            size_t* new_params_length)

cdef unsigned char** to_cstring_array(list_str):
    cdef unsigned char** ret = <unsigned char **>malloc(len(list_str) * sizeof(unsigned char *))
    for i in range(len(list_str)):
        ret[i] = list_str[i]
    return ret

cdef unsigned char*** to_cstringarray_array(list_strarray):
    cdef unsigned char*** ret = <unsigned char ***>malloc(len(list_strarray) * sizeof(unsigned char **))
    for i in range(len(list_strarray)):
        ret[i] = to_cstring_array(list_strarray[i])
    return ret

cdef size_t* to_sizet_array(list_int):
    cdef size_t* ret = <size_t *>malloc(len(list_int) * sizeof(size_t))
    for i in range(len(list_int)):
        ret[i] = list_int[i]
    return ret

def cy_host_modelaggregator(encrypted_accumulator, accumulator_lengths, accumulator_length, encrypted_old_params, old_params_length):
    # encrypted_accumulator: List of ENCRYPTED SERIALIZED models
    # accumulator_lengths: # list of ENCRYPTED SERIALIED model lengths 
    # accumulator_length: # of models
    # encrypted_old_params: ENCRYPTED SERIALIZED original central model 
    # old_params_length: length of ENCRYPTED SERIALIZED central model 

    cdef unsigned char*** c_encrypted_accumulator = to_cstringarray_array(encrypted_accumulator)
    cdef size_t* c_accumulator_lengths = to_sizet_array(accumulator_lengths)
    cdef unsigned char** c_encrypted_old_params = to_cstring_array(encrypted_old_params)

    cdef unsigned char*** new_params_ptr = <unsigned char ***>malloc(3 * sizeof(unsigned char**))
    cdef size_t* new_params_length = <size_t *>malloc(1 * sizeof(size_t))
    
    err = host_modelaggregator(c_encrypted_accumulator,
                                 c_accumulator_lengths,
                                 accumulator_length,
                                 c_encrypted_old_params,
                                 old_params_length,
                                 new_params_ptr,
                                 new_params_length)
    
    if (err):
        print('calling into enclave_modelaggregator failed')
        return
                                 
    cdef bytes output = new_params_ptr[0][0][:new_params_length[0]]
    cdef bytes iv = new_params_ptr[0][1][:12]
    cdef bytes tag = new_params_ptr[0][2][:16]
    return output, iv, tag





