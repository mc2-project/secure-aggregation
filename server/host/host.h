#ifndef _HOST_H
#define _HOST_H

int host_modelaggregator(uint8_t** encrypted_accumulator, 
        size_t* accumulator_lengths,
        size_t accumulator_length, 
        uint8_t* encrypted_old_params,
        size_t old_params_length,
        uint8_t** encrypted_new_params_ptr,
        size_t* new_params_length,
        float* contributions);

#endif
