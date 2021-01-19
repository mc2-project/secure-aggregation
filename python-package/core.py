import ctypes
import numpy as np
#  from numpy.ctypeslib import ndpointer

_LIB = ctypes.CDLL("../server/build/host/libmodelaggregator_host.so")

IV_LENGTH = 12
TAG_LENGTH = 16

_LIB.host_modelaggregator.argtypes = (
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.POINTER(ctypes.c_size_t),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.POINTER(ctypes.c_uint8)),
        ctypes.POINTER(ctypes.c_size_t),
        ctypes.POINTER(ctypes.c_float)
        )

_LIB.api_encrypt_bytes.argtypes = (
    ctypes.POINTER(ctypes.c_uint8),
    ctypes.c_size_t,
    ctypes.POINTER(ctypes.POINTER(ctypes.c_uint8))
    )

_LIB.api_encrypt_bytes.argtypes = (
    ctypes.POINTER(ctypes.c_uint8),
    ctypes.c_size_t,
    ctypes.POINTER(ctypes.POINTER(ctypes.c_uint8))
    )

_LIB.api_serialize.argtypes = (
        ctypes.POINTER(ctypes.c_char_p),
        ctypes.POINTER(ctypes.POINTER(ctypes.c_float)),
        ctypes.POINTER(ctypes.c_int),
        ctypes.c_int,
        ctypes.POINTER(ctypes.c_int)
        )

_LIB.api_deserialize_keys.argtypes = (
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.POINTER(ctypes.c_int)
        )

_LIB.api_deserialize_values.argtypes = (
        ctypes.POINTER(ctypes.c_uint8),
        ctypes.POINTER(ctypes.POINTER(ctypes.c_int)),
        ctypes.POINTER(ctypes.c_int)
        )

_LIB.api_serialize.restype = ctypes.POINTER(ctypes.c_uint8)
_LIB.api_deserialize_keys.restype = ctypes.POINTER(ctypes.c_char_p)
_LIB.api_deserialize_values.restype = ctypes.POINTER(ctypes.POINTER(ctypes.c_float))


def c_array(ctype, values):
    """Convert a python list to c array."""
    #  if isinstance(values, np.ndarray) and values.dtype.itemsize == ctypes.sizeof(ctype):
    #      return (ctype * len(values)).from_buffer_copy(values)
    return (ctype * len(values))(*values)

#  def c_model_update(local_model_update, local_model_update_len):
#      enc_data, iv, tag = split_ciphertext(local_model_update, local_model_update_len)
#  
#      c_enc_data = (ctypes.c_uint8 * local_model_update_len)(*enc_data)
#      c_iv = (ctypes.c_uint8 * IV_LENGTH)(*iv)
#      c_tag = (ctypes.c_uint8 * TAG_LENGTH)(*tag)
#  
#      c_model_update = ctypes.POINTER(ctypes.c_uint8 * (local_model_update_len + IV_LENGTH + TAG_LENGTH))
#  
#      return c_model_update

def malloc_model_update(model_len):
    c_new_model_update = ctypes.POINTER(ctypes.c_uint8 * (model_len + IV_LENGTH + TAG_LENGTH))()
    return c_new_model_update

def split_ciphertext(data, data_len):
    """
    data: enc_data || IV || TAG
    data_len: Length of data - IV_LENGTH - TAG_LENGTH, i.e. length of ciphertext without IV and TAG
    """
    output = data[:data_len]
    iv = data[data_len:data_len + IV_LENGTH]
    tag = data[data_len + IV_LENGTH:data_len + IV_LENGTH + TAG_LENGTH]
    return output, iv, tag

def c_arr_to_list(cptr, length, dtype=np.uint8):
    """Convert a ctypes pointer array to a Python list.
    """
    NUMPY_TO_CTYPES_MAPPING = {
        np.float32: ctypes.c_float,
        np.uint32: ctypes.c_uint,
        np.uint8: ctypes.c_uint8,
        np.intc: ctypes.c_int
    }
    if dtype not in NUMPY_TO_CTYPES_MAPPING:
        raise RuntimeError('Supported types: {}'.format(NUMPY_TO_CTYPES_MAPPING.keys()))
    ctype = NUMPY_TO_CTYPES_MAPPING[dtype]
    if not isinstance(cptr, ctypes.POINTER(ctype)):
        raise RuntimeError('expected {} pointer, got {}'.format(ctype, type(cptr)))
    res = np.zeros(length, dtype=dtype)
    if not ctypes.memmove(res.ctypes.data, cptr, length * res.strides[0]):
        raise RuntimeError('memmove failed')
    return res.tolist()

def from_pystr_to_cstr(data):
    """Convert a list of Python str to C pointer
    Parameters
    ----------
    data : list
        list of str
    """

    if not isinstance(data, list):
        raise NotImplementedError
    pointers = (ctypes.c_char_p * len(data))()
    data = [bytes(d, 'utf-8') for d in data]
    pointers[:] = data
    return pointers

def from_cstr_to_pystr(data, length):
    """Revert C pointer to list of Python str
    Parameters
    ----------
    data : ctypes pointer
        pointer to data
    length : ctypes pointer
        pointer to length of data
    """
    res = []
    for i in range(length.value):
        res.append(str(data[i].decode('utf-8')))
    return res

def from_pyfloat_to_cfloat(data):
    """Convert a list of a list of Python floats to C float double pointer
    Parameters
    ----------
    data : list
        list of list of floats
    """

    if not isinstance(data, list):
        raise NotImplementedError

    pointers = (ctypes.POINTER(ctypes.c_float) * len(data))()
    num_floats_per_pointer = (ctypes.c_int * len(data))()
    for i in range(len(data)):
        float_lst = data[i]
        pointers[i] = c_array(ctypes.c_float, float_lst)
        num_floats_per_pointer[i] = ctypes.c_int(len(float_lst))

    return pointers, num_floats_per_pointer

def from_cfloat_to_pyfloat(data, num_floats, length):
    """Convert a C float double pointer to a list of a list of Python floats
    Parameters
    ----------
    data : list
        C float double pointer
    num_floats : list 
        Num floats per list in data
    """
    num_floats_per_list = c_arr_to_list(num_floats, length.value, np.intc)
    res = []
    for i in range(length.value):
        float_lst = c_arr_to_list(data[i], num_floats_per_list[i], np.float32)
        res.append(float_lst)

    return res

def flatten_encrypted_model(model):
    flattened_model = [byte for element in model for byte in element]
    return flattened_model

def aggregate(encrypted_accumulator, accumulator_lengths, accumulator_length,
        encrypted_old_params, old_params_length, contributions):

    num_model_updates = accumulator_length

    # Convert everything to ctypes
    c_encrypted_accumulator = ctypes.POINTER(ctypes.POINTER(c_uint8)) * num_model_updates

    for i in range(num_model_updates):
        local_model_update = encrypted_accumulator[i]
        local_model_update_len = accumulator_lengths[i]
        flattened_model_update = flatten_encrypted_model(local_model_update)

        # FIXME: perhaps a bug here
        c_model_update_arr = c_array(ctypes.c_uint8, flattened_model_update) 
        c_encrypted_accumulator[i] = ctypes.cast(c_model_update_arr, ctypes.POINTER(ctypes.c_uint8))
        print(type(c_encrypted_accumulator[i]))

    c_accumulator_lengths = c_array(ctypes.c_size_t, accumulator_lengths)
    c_accumulator_length = ctypes.c_size_t(accumulator_length)

    flattened_encrypted_old_params = flatten_encrypted_model(encrypted_old_params)
    c_encrypted_old_params = c_array(ctypes.c_uint8, flattened_encrypted_old_params)
    c_old_params_length = ctypes.c_size_t(old_params_length)

    c_new_model_update = malloc_model_update(old_params_length)
    c_new_params_length = ctypes.c_size_t(0)

    c_contributions = c_array(ctypes.c_float, contributions)

    _LIB.host_modelaggregator(
        c_encrypted_accumulator,
        c_accumulator_lengths,
        c_accumulator_length,
        c_encrypted_old_params,
        c_old_params_length,
        c_new_model_update,
        c_new_params_length,
        c_contributions
            )

    py_ciphertext = c_arr_to_list(c_new_model_update, c_new_params_length.value + IV_LENGTH + TAG_LENGTH)
    py_update, py_iv, py_tag = split_ciphertext(py_ciphertext, c_new_params_length.value)

    return py_update, py_iv, py_tag

def encrypt(model):
    # Convert model to format inputtable to C++
    keys = []
    values = []
    num_kvpairs = 0
    for feature_name, feature_value in model.items():
        keys.append(feature_name)
        values.append(feature_value)
        num_kvpairs += 1

    c_feature_names = from_pystr_to_cstr(keys)
    c_feature_values, c_num_floats_per_feature = from_pyfloat_to_cfloat(values)
    c_num_kvpairs = ctypes.c_int(num_kvpairs)
    c_serialized_buffer_size = ctypes.c_int()

    # Call C++ serialize() function
    serialized_model_pointer = _LIB.api_serialize(c_feature_names, c_feature_values, c_num_floats_per_feature, c_num_kvpairs, ctypes.byref(c_serialized_buffer_size))
    data_len = c_serialized_buffer_size.value

    # Allocate memory for ciphertext
    c_ciphertext_arr = (ctypes.c_uint8 * (data_len + IV_LENGTH + TAG_LENGTH))() 
    c_ciphertext = ctypes.cast(c_ciphertext_arr, ctypes.POINTER(ctypes.c_uint8))

    # Call C++ encrypt_bytes() function
    buffer_size_t = ctypes.c_size_t(data_len)
    _LIB.api_encrypt_bytes(serialized_model_pointer, buffer_size_t, ctypes.byref(c_ciphertext))

    # Convert ciphertext to Python format
    py_ciphertext = c_arr_to_list(c_ciphertext, data_len + IV_LENGTH + TAG_LENGTH)
    output, iv, tag = split_ciphertext(py_ciphertext, data_len)

    return output, iv, tag, data_len

def decrypt(model_data, iv, tag, data_len):
    # Get arguments for C++ decrypt_bytes() function
    c_model_data = c_array(ctypes.c_uint8, model_data)
    c_iv = c_array(ctypes.c_uint8, iv)
    c_tag = c_array(ctypes.c_uint8, tag)
    c_data_len = ctypes.c_size_t(data_len)

    # Allocate memory for C++ to store decrypted data
    c_serialized_plaintext_arr = (ctypes.c_uint8 * data_len)()
    c_serialized_plaintext = ctypes.cast(c_serialized_plaintext_arr, ctypes.POINTER(ctypes.c_uint8))

    # Call decrypt_bytes()
    _LIB.api_decrypt_bytes(c_model_data, c_iv, c_tag, c_data_len, ctypes.byref(c_serialized_plaintext))
    # Call deserialize()
    num_keys = ctypes.c_int()
    keys = _LIB.api_deserialize_keys(c_serialized_plaintext, ctypes.byref(num_keys))

    num_values = ctypes.c_int()
    c_num_floats_per_value_arr = (ctypes.c_int * num_keys.value)()
    c_num_floats_per_value = ctypes.cast(c_num_floats_per_value_arr, ctypes.POINTER(ctypes.c_int))
    values = _LIB.api_deserialize_values(c_serialized_plaintext, ctypes.byref(c_num_floats_per_value), ctypes.byref(num_values))
    
    assert(num_keys.value == num_values.value)

    # Convert from C++ keys/values to Python dictionary
    py_keys = from_cstr_to_pystr(keys, num_keys)
    py_values = from_cfloat_to_pyfloat(values, c_num_floats_per_value, num_values)

    model = {}

    for i in range(num_keys.value):
        model[py_keys[i]] = py_values[i]

    return model

output, iv, tag, data_len = encrypt({"feature1": [1.0, 3.0, 4.0], "feature2": [2.0]})
print(decrypt(output, iv, tag, data_len))

