from berkeley_encrypt import encryption, decryption, protobuf_to_dict, dict_to_protobuf
from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.components.data_processor import DataProcessor
from fed_learn.protos.federated_pb2 import NDArray
from fed_learn.protos.federated_pb2 import ModelData

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
        model_dict = protobuf_to_dict(data_ctx.model)
        model_dict['_contribution'] = np.array([1.0])

        encrypted_params = encryption(model_dict)
        # params = encrypted_params['enc_values']
        # enc_bytes = params[0] + b'boundary' + params[1] + b'boundary' + params[2]
        # enc_array = NDArray()
        # enc_array.ndarray = enc_bytes
        # data_ctx.model.params['enc_values'].CopyFrom(enc_array)

        enc_dict = dict_to_protobuf(encrypted_params)
        data_ctx.model = enc_dict

        print('POST MODEL ENCRYPTED')
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
        model_dict = protobuf_to_dict(data_ctx.model)
        print('ENCRYPTED ARRAY IN DICT: ', 'enc_values' in model_dict.keys())
        if 'enc_values' not in model_dict.keys():
            print('Decrypting Unencrypted Model')
            return data_ctx
        else:
            print('DECRYPT SERVER MODEL')
            enc_list = model_dict['enc_values']
            decrypted_dict = decryption({'enc_values': enc_list})
            for feat in decrypted_dict.copy().keys():
                if feat.startswith('shape'):
                    decrypted_dict.pop(feat)
            print('NUM KEYS DECRYPTED: ', len(decrypted_dict.keys()))
            data_ctx.model = dict_to_protobuf(decrypted_dict)
        return data_ctx
