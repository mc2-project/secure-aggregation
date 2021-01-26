import numpy as np
import re
import logging

from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.protos.federated_pb2 import NDArray
from fed_learn.server.model_aggregator import Aggregator
from fed_learn.model_meta import FLContext
from berkeley_encrypt import protobuf_to_dict, dict_to_protobuf, encryption, decryption
#  from server_methods import cy_host_modelaggregator
from secagg import aggregate
import time

import sys

class CustomModelAggregator(Aggregator):
    def __init__(self, exclude_vars=None):
        self.logger = logging.getLogger('CustomModelAggregator')
        self.exclude_vars = re.compile(exclude_vars) if exclude_vars else None

    def process(self, accumulator, fl_ctx):
        """Aggregate the contributions from all the submitted FL clients.
        This function is not thread-safe.
        :param accumulator: List of encrypted, serialized contributions.
        :param fl_ctx: Original central plaintext model
        :return:
        """
        model = fl_ctx.get_model()
        model_dict = protobuf_to_dict(model)
        if 'enc_values' in model_dict.keys():
            enc_global_model = model_dict['enc_values']
        else:
            # This only happens if the initial host model is unencrypted
            enc_global_model = encryption(model_dict)['enc_values']
        
        enc_local_models = [protobuf_to_dict(client.get_model())['enc_values'] for client in accumulator]
        accumulator_lengths = [len(model[0]) for model in enc_local_models]
        
        contributions = [item.get_prop('_contribution').n_iter for item in accumulator]
        
        start = time.time()
        # Aggregation in enclave
        enc_out, iv, tag = aggregate(
            encrypted_accumulator = enc_local_models,
            accumulator_lengths = accumulator_lengths,
            accumulator_length = len(accumulator_lengths),
            encrypted_old_params = enc_global_model,
            old_params_length = len(enc_global_model[0]),
            contributions = contributions
        )
        end = time.time()

        new_model_enc = {'enc_values': [enc_out, iv, tag]}
        enc_dict = dict_to_protobuf(new_model_enc)
        fl_ctx.model = enc_dict
        return False
