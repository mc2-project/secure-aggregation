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

        # BERKELEY ------------
        model_dict = protobuf_to_dict(data_ctx.model)

        # Below to check accuracy
        #  print(model_dict['stage5/_identity_block/bn5c_branch2b/batch_norm/beta:0'].dtype)

        encrypted_params = encryption(model_dict)
        enc_dict = dict_to_protobuf(encrypted_params)
        data_ctx.model = enc_dict

        print("Client side encryption, local model update protobuf size: ", data_ctx.model.ByteSize())

        return data_ctx
        # -----------

        ## ORIGINAL -------------
        # encrypted_params = protobuf_to_dict(data_ctx.model)
        # print('---WEIGHTS POST MODEL TRAINING')
        # print(encrypted_params['stage5/_identity_block/bn5c_branch2b/batch_norm/beta:0'])
        # encrypted_model = dict_to_protobuf(encrypted_params)
        # data_ctx.model = encrypted_model
        # return data_ctx
        # -----------

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

        print(("Client side decryption, global model protobuf size: ", data_ctx.model.ByteSize()))
        model_dict = protobuf_to_dict(data_ctx.model)
        if 'enc_values' not in model_dict.keys():
            print('Decrypting Unencrypted Model')
            return data_ctx
        else:
            enc_list = model_dict['enc_values']
            decrypted_dict = decryption({'enc_values': enc_list})

            # Moved to decryption() in berkeley_encrypt.py
            #  for feat in decrypted_dict.copy().keys():
            #      if feat.startswith('shape'):
            #          decrypted_dict.pop(feat)

            # Below to check accuracy
            #  print(decrypted_dict['stage5/_identity_block/bn5c_branch2b/batch_norm/beta:0'][:10])

            data_ctx.model = dict_to_protobuf(decrypted_dict)
        return data_ctx
        
        ## ORIGINAL -------------
        # decrypted_params = protobuf_to_dict(data_ctx.model)
        # print('---WEIGHTS (RECEIVED BY CLIENT)')
        # print(decrypted_params['stage5/_identity_block/bn5c_branch2b/batch_norm/beta:0'][:10])
        # decrypted_model = dict_to_protobuf(decrypted_params)
        # data_ctx.model = decrypted_model
        # return data_ctx
        # -----------

