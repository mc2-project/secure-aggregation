from fed_learn.protos.federated_pb2 import ModelData
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from client_methods import cy_serialize, cy_deserialize, cy_encrypt_bytes, cy_decrypt_bytes
import enc_model_pb2


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
    # params: dictionary of model features:weights
    # Dictionary key strings may have to be represented as byte strings (i.e. b'string')
    # Worst case, new dictionaries will have to be made but i can probably fix this in client_methods.pyx

    print(f'Running Berkeley Encryption algorithm. # of model variables: {len(params.keys())}')

    params = {key.encode(): value for key, value in params.items()}
    serialized_params = cy_serialize(params)
    enc_out, iv, tag = cy_encrypt_bytes(serialized_params, len(serialized_params))

    while len(enc_out) != len(serialized) or len(iv) != 16 or len(tag) != 16:
        out, iv, tag = cy_encrypt_bytes(serialized_params, len(serialized_params))

    enc_proto = enc_proto_pb2.model()
    enc_proto.enc_data = enc_out
    enc_proto.iv = iv
    enc_proto.tag = tag

    return enc_proto

def decryption(encrypted_proto):
    print(f'Running Berkeley Decryption algorithm. # of model variables:')

    # encrypted_proto: enc_model_pb2 protobuf
    enc_out = encrypted_proto.enc_data
    iv = encrypted_proto.iv
    tag = encrypted_output.tag

    serialized_params = cy_decrypt_bytes(enc_out, iv, tag, len(enc_out))
    params = cy_deserialize(serialized_params)

    new_params = {key.decode(): value for key, value in params.items()}
    return new_params
