import numpy as np
import re
import logging

from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.protos.federated_pb2 import NDArray
from fed_learn.server.model_aggregator import Aggregator
from fed_learn.model_meta import FLContext
from berkeley_encrypt import protobuf_to_dict, dict_to_protobuf, encryption, decryption
from server_methods import cy_host_modelaggregator
import time

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

        print('---Beginning aggregation---')
        start = time.time()

        # Below is original NVIDIA code -------------------------------------
        # print("Running original aggregation code")
        # model = fl_ctx.get_model()
        # acc_vars = [set(item.get_model().params) for item in accumulator]
        # acc_vars = set.union(*acc_vars)
        # vars_to_aggregate = [
        #     g_var for g_var in acc_vars if not self.exclude_vars.search(g_var)
        # ] if self.exclude_vars else acc_vars
        
        # for v_name in vars_to_aggregate:
        #     n_local_iters, np_vars = [], []
        #     for item in accumulator:
        #         data = item.get_model()
        #         if v_name not in data.params:
        #             continue  # this item doesn't have the variable from client
        
        #         # contribution is a protobuf msg
        #         #   it has `n_iter` which represents number of local iterations 
        #         #   used to compute this contribution 
        #         acc = item.get_prop('_contribution')
        #         float_n_iter = np.float(acc.n_iter)
        #         n_local_iters.append(float_n_iter)
        
        #         # it also has `client` and client has `uid`
        #         self.logger.info(f'Get contribution from client {acc.client.uid}')
        
        #         # weighted using local iterations
        #         weighted_value = proto_to_ndarray(data.params[v_name]) * float_n_iter
        #         np_vars.append(weighted_value)
        #     if not n_local_iters:
        #         continue  # didn't receive this variable from any clients
        #     new_val = np.sum(np_vars, axis=0) / np.sum(n_local_iters)
        #     new_val += proto_to_ndarray(model.params[v_name])
        
        #     # Update the params in model using CopyFrom because it is a ProtoBuf structure
        #     model.params[v_name].CopyFrom(ndarray_to_proto(new_val))

            # ---------------------------------------------------------------------------------

        # Below is RISE Secure Aggregation code
        model = fl_ctx.get_model()
        model_dict = protobuf_to_dict(model)
        if 'enc_values' in model_dict.keys():
            enc_model = model_dict['enc_values']
        else:
            # This only happens if the initial host model is unencrypted
            enc_model = encryption(model_dict)['enc_values']
        
        enc_models = [protobuf_to_dict(client.get_model())['enc_values'] for client in accumulator]
        accumulator_lengths = [len(model[0]) for model in enc_models]

        contributions = [np.float(item.get_prop('_contribution').n_iter) for item in accumulator]
        print('---Contributions: ', contributions)
        
        # Encrypted Training in Secure Enclave
        enc_out, iv, tag = cy_host_modelaggregator(
            encrypted_accumulator = enc_models,
            accumulator_lengths = accumulator_lengths,
            accumulator_length = len(accumulator_lengths),
            encrypted_old_params = enc_model,
            old_params_length = len(enc_model[0]),
            contributions = contributions
        )
        print('---Successful aggregation---')
        
        new_model_enc = {'enc_values': [enc_out, iv, tag]}
        enc_dict = dict_to_protobuf(new_model_enc)
        fl_ctx.model = enc_dict

        #     # # -------------------------------------------------------------------------------------
        end = time.time()
        print("---Elapsed time: ", end - start, " seconds ---")

        return False
