# cython: language_level=3

cdef extern from "host.h":
    unsigned char** host_modelaggregator(unsigned char*** encrypted_accumulator, 
            size_t* accumulator_lengths,
            size_t accumulator_length, 
            unsigned char** encrypted_old_params,
            size_t old_params_length)

def cy_host_modelaggregator(encrypted_accumulator, accumulator_lengths, accumulator_length, encrypted_old_params, old_params_length):
    # encrypted_accumulator: List of ENCRYPTED SERIALIZED models
    # accumulator_lengths: # list of ENCRYPTED SERIALIED model lengths 
    # accumulator_length: # of models
    # encrypted_old_params: ENCRYPTED SERIALIZED original central model 
    # old_params_length: length of ENCRYPTED SERIALIZED central model 
    
    return host_model_aggregator(encrypted_accumulator,
                                 accumulator_lengths,
                                 accumulator_length,
                                 encrypted_old_params,
                                 old_params_length)




