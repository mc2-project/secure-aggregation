from berkeley_encrypt import encryption, decryption, protobuf_to_dict, dict_to_protobuf
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.protos.federated_pb2 import NDArray
from fed_learn.components.data_processor import DataProcessor
import numpy as np
import sys

class CustomModelEncryptor(DataProcessor):

    def process(self, data_ctx, app_ctx):
        """Encrypts the data_ctx.

        :param data_ctx:
        :param app_ctx:
        :return:
        """
        # The model data is hold in data_ctx.model.params as a dict. The keys are the model variables, and
        # the values are model weights in ndarray.
        # The encryption can encrypt the individual variable name and value to keep the same amount of key values
        # pairs in the data_ctx.model.params, or encrypt the whole dict into a single key value pair.
        print('ORIGINAL SIZE OF PROTOBUF: ', sys.getsizeof(data_ctx.model.SerializeToString()))
        print('NUM KEYS: ', len(model_dict.keys()))

        model_dict = protobuf_to_dict(data_ctx.model)
        model_dict['_contribution'] = np.array([1.0])

        encrypted_params = encryption(model_dict)
        params = encrypted_params['enc_values']
        enc_bytes = params[0] + b'boundary' + params[1] + b'boundary' + params[2]
        enc_array = NDArray()
        enc_array.ndarray = enc_bytes

        print('SERIALIZED SIZE OF ENC_BYTES: ', sys.getsizeof(enc_array.SerializeToString()))

        data_ctx.model.params['enc_values'].CopyFrom(enc_array)

        print('POST MODEL ENCRYPTED')
        print('TOTAL SERIALIZED BYTE SIZE: ', sys.getsizeof(data_ctx.model.SerializeToString()))
        return data_ctx


class CustomModelDecryptor(DataProcessor):

    def process(self, data_ctx, app_ctx):
        """Decrypts the data_ctx.

        :param data_ctx:
        :param app_ctx:
        :return:
        """
        # Add the data de_encryption code here.
        # Based the encrypt algorithm process, decrypt the key value pair, or key value pairs into the original
        # model variable names and model weights.
        print('DECRYPTING GLOBAL MODEL')
        print('SIZE OF PROTOBUF (PRE DECRYPTION)', sys.getsizeof(data_ctx.model.SerializeToString()))
        model_dict = protobuf_to_dict(data_ctx.model)
        if 'enc_values' not in model_dict.keys():
            print('TRYING TO DECRYPT UNENCRYPTED MODEL!')
            model = dict_to_protobuf(model_dict)
            data_ctx.model = model
        else:
            print('PROTOBUF KEYS')
            print(len(model_dict.keys()))
            decrypted_params = decryption(model_dict)
            # for key in decrypted_params.keys():
            #     if not key.startswith('shape_'):
            #         data_ctx.model.params[key].CopyFrom(ndarray_to_proto(decrypted_params[key]))
            data_ctx.model.params.pop('enc_values')
            print("CLEANED DECRYPTED PROTOBUF KEYS")
            print(len(data_ctx.model.params.keys()))
            print('SIZE OF PROTOBUF (POST DECRYPTION)', sys.getsizeof(data_ctx.model.SerializeToString()))
        print('GLOBAL MODEL DECRYPTED')
        return data_ctx
