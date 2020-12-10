import numpy as np
import re
import logging

from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.protos.federated_pb2 import NDArray
from fed_learn.server.model_aggregator import Aggregator
from fed_learn.model_meta import FLContext
from berkeley_encrypt import protobuf_to_dict, dict_to_protobuf, encryption, decryption
from server_methods import cy_host_modelaggregator

class CustomModelAggregator(Aggregator):
    def __init__(self, exclude_vars=None):
        self.logger = logging.getLogger('CustomModelAggregator')
        self.exclude_vars = re.compile(exclude_vars) if exclude_vars else None

    # -- Working Client-Side Encryption but server re-encrypts -- 
    # def process(self, accumulator, fl_ctx):
    #         """Aggregate the contributions from all the submitted FL clients.
    #         This function is not thread-safe.
    #         :param accumulator: List of encrypted, serialized contributions.
    #         :param fl_ctx: Original central plaintext model
    #         :return:
    #         """
    #         enc_models = []
    #         for client in accumulator:
    #             acc = client.get_prop('_contribution')
    #             model_dict = protobuf_to_dict(client.get_model())
    #             model_dict['_contribution'] = np.array([np.float(acc.n_iter)])
    #             model_dict.pop('enc_values')
    #             enc_models.append(encryption(model_dict)['enc_values'])

    #         print("CLIENT MODEL ENCRYPTION SUCCESSFUL")

    #         # Serialize and Encrypt Central Model
    #         model = fl_ctx.get_model()
    #         enc_model = encryption(protobuf_to_dict(model))['enc_values']
    #         accumulator_lengths = [len(model[0]) for model in enc_models]

    #         # Encrypted Training in Secure Enclave
    #         enc_out, iv, tag = cy_host_modelaggregator(
    #             encrypted_accumulator = enc_models,
    #             accumulator_lengths = accumulator_lengths,
    #             accumulator_length = len(accumulator_lengths),
    #             encrypted_old_params = enc_model,
    #             old_params_length = len(enc_model[0])
    #         )
    #         print('AGGREGATION SUCCESSFUL')

    #         new_model_enc = {'enc_values': [enc_out, iv, tag]}

    #         params = new_model_enc['enc_values']
    #         enc_bytes = params[0] + b'boundary' + params[1] + b'boundary' + params[2]
    #         enc_array = NDArray()
    #         enc_array.ndarray = enc_bytes

    #         model.params['enc_values'].CopyFrom(enc_array)
    #         print('ENCRYPTED ARRAY COPIED TO MODEL')
    #         print('NUM KEYS ENCRYPTED: ', len(protobuf_to_dict(model).keys()))

    #         print('SENDING NEW MODEL TO CLIENTS')
    #         return False

    def process(self, accumulator, fl_ctx):
            """Aggregate the contributions from all the submitted FL clients.
            This function is not thread-safe.
            :param accumulator: List of encrypted, serialized contributions.
            :param fl_ctx: Original central plaintext model
            :return:
            """

            print('STARTING AGGREGATION')
            model = fl_ctx.get_model()
            model_dict = protobuf_to_dict(model)
            if 'enc_values' in model_dict.keys():
                enc_model = model_dict['enc_values']
            else:
                enc_model = encryption(model_dict)['enc_values']
                
            enc_models = [protobuf_to_dict(client.get_model())['enc_values'] for client in accumulator]
            accumulator_lengths = [len(model[0]) for model in enc_models]

            # Encrypted Training in Secure Enclave
            enc_out, iv, tag = cy_host_modelaggregator(
                encrypted_accumulator = enc_models,
                accumulator_lengths = accumulator_lengths,
                accumulator_length = len(accumulator_lengths),
                encrypted_old_params = enc_model,
                old_params_length = len(enc_model[0])
            )
            print('AGGREGATION SUCCESSFUL')

            new_model_enc = {'enc_values': [enc_out, iv, tag]}

            # params = new_model_enc['enc_values']
            # enc_bytes = params[0] + b'boundary' + params[1] + b'boundary' + params[2]
            # enc_array = NDArray()
            # enc_array.ndarray = enc_bytes
            # model.params['enc_values'].CopyFrom(enc_array)

            enc_dict = dict_to_protobuf(new_model_enc)
            fl_ctx.model = enc_dict

            print('ENCRYPTED ARRAY COPIED TO MODEL')
            print('NUM KEYS ENCRYPTED: ', len(protobuf_to_dict(model).keys()))

            print('SENDING NEW MODEL TO CLIENTS')
            return False