# cython: language_level=3

cdef extern from "host.h":
    void host_modelaggregator(unsigned char*** encrypted_accumulator, 
            size_t* accumulator_lengths,
            size_t accumulator_length, 
            unsigned char** encrypted_old_params,
            size_t old_params_length)
