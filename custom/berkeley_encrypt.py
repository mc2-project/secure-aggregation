from fed_learn.protos.federated_pb2 import ModelData
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from client_methods import encrypt, decrypt

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
        result[v] = proto_to_ndarray(model_data.params[v])
    return result


def dict_to_protobuf(params):
    """Transform a dict of keys - numpy arrays to the model data protobuf structure."""
    result = ModelData()
    for v in params:
        result.params[v].CopyFrom(ndarray_to_proto(params[v]))
    return result


def encryption(params):
    # Input: Dictionary of plaintext parameters
    # Output: Dictionary of encrypted parameters

    print(f'Running Berkeley Encryption algorithm. # of model variables: {len(params.keys())}')

    # Add shape parameter for each feature and flatten values
    for feature in list(params.keys()):
        if feature != '_contribution' and not feature.startswith('shape') and 'shape_'+feature not in params.keys():
            params['shape_' + feature] = list(params[feature].shape)
        params[feature] = params[feature].flatten().tolist()

    # Convert keys to byte strings for each feature
    params = {key.encode(): value for key, value in params.items()}

    # Serialize and Encrypt params
    enc_out, iv, tag = encrypt(params)

    enc_dict = {'enc_values': [enc_out, iv, tag]}

    return enc_dict

def decryption(input_params):
    print(f'Running Berkeley Decryption algorithm. # of model variables: {len(params.keys())}')

    # Input: Dictionary of encrypted parameters
    # Output: Dictionary of plaintext parameters

    # Decrypt and deserialize encrypted params
    enc_values = input_params['enc_values']
    params = decrypt(enc_values[0], enc_values[1], enc_values[2], len(enc_values[0]))

    # Convert byte strings to keys for each feature
    params = {key.decode(): value for key, value in params.items()}

    # Unflatten values
    for key, value in params.items():
        model[key] = np.array(value)
        if not key.startswith('shape_') and key != '_contribution':
            params[key] = np.array(value).reshape(np.array(params['shape_' + key]).astype(int))
    return params
