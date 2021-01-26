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
        
        encrypted_params = encryption(model_dict)
        enc_dict = dict_to_protobuf(encrypted_params)
        data_ctx.model = enc_dict
        
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
        if 'enc_values' not in model_dict.keys():
            print('Decrypting unencrypted model')
            return data_ctx
        else:
            enc_list = model_dict['enc_values']
            decrypted_dict = decryption({'enc_values': enc_list})
            data_ctx.model = dict_to_protobuf(decrypted_dict)
        
        return data_ctx

