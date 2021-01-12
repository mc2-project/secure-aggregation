from fed_learn.protos.federated_pb2 import ModelData
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from client_methods import encrypt, cpp_encrypt_bytes, decrypt
from fed_learn.protos.federated_pb2 import NDArray

import numpy as np

# Below is the protobuf msg definition of model
#
# // A model consists of multiple tensors
# message ModelData {
#     map<string, NDArray> params = 1;
# }
#
# // NDArray data for protobuf
# message NDArray {
#   bytes ndarray = 1;
# }

def protobuf_to_dict(model_data):
    """Transform the model data protobuf structure to a dict of keys - numpy arrays."""
    result = {}
    for v in model_data.params:
        if v == 'enc_values':
            enc_params = model_data.params[v].ndarray
            enc_params = enc_params.split(b'boundary')
            result[v] = enc_params
        else:
            result[v] = proto_to_ndarray(model_data.params[v])
    return result


def dict_to_protobuf(params):
    """Transform a dict of keys - numpy arrays to the model data protobuf structure."""
    result = ModelData()
    for v in params:
        if v == 'enc_values':
            enc_values = params['enc_values']
            enc_bytes = enc_values[0] + "boundary".encode() + enc_values[1] + "boundary".encode() + enc_values[2]
            enc_array = NDArray(ndarray=enc_bytes)
            result.params['enc_values'].CopyFrom(enc_array)
        else:
            result.params[v].CopyFrom(ndarray_to_proto(params[v]))
    return result

def encryption(params):
    # Input: Dictionary of plaintext parameters
    # Output: Dictionary of encrypted parameters

    print(f'Running Berkeley Encryption algorithm. # of model variables: {len(params.keys())}')

    if 'enc_values' in params.keys():
        print("Dictionary already encrypted, removing encrypted values and re-encrypting")
        params.pop('enc_values')
    
    # Add shape parameter for each feature and flatten values
    for feature in list(params.keys()):
        if not feature.startswith('shape'):
            params['shape_' + feature] = list(params[feature].shape)
        params[feature] = params[feature].flatten().tolist()
    
    # Convert keys to byte strings for each feature
    # params_bytestr is a dictionary of key.bytes: NDArray
    params_bytestr = {key.encode(): value for key, value in params.items()}
    
    # Serialize and Encrypt params
    enc_out, iv, tag = encrypt(params_bytestr)
    enc_dict = {'enc_values': [enc_out, iv, tag]}
    
    return enc_dict


def decryption(params):
    # Input: Dictionary of encrypted parameters
    # Output: Dictionary of plaintext parameters

    print(f'Running Berkeley Decryption algorithm. # of model variables: {len(params.keys())}')

    # Decrypt and deserialize encrypted params
    enc_values = params['enc_values']
    params = decrypt(enc_values[0], enc_values[1], enc_values[2], len(enc_values[0]))
    
    # Convert byte strings to keys for each feature
    params = {key.decode(): value for key, value in params.items()}
    
    # Unflatten values
    for key, value in params.items():
        params[key] = np.array(value)
        if not key.startswith('shape_'):
            params[key] = np.array(value).reshape(np.array(params['shape_' + key]).astype(int))
            params[key] = params[key].astype(np.float32)
    
    for feature_name in list(params):
        if feature_name.startswith("shape_"):
            del params[feature_name]
    
    return params
