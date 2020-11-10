from fed_learn.protos.federated_pb2 import ModelData
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto


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
    print(f'Running Berkeley Encryption algorithm. # of model variables: {len(params.keys())}')
    return params


def decryption(params):
    print(f'Running Berkeley Decryption algorithm. # of model variables: {len(params.keys())}')
    return params
