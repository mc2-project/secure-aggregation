import numpy as np

from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.server.model_aggregator import Aggregator
from berkeley_encrypt import protobuf_to_dict, dict_to_protobuf, encryption, decryption
from server_methods import cy_host_modelaggregator


class CustomModelAggregator(Aggregator):
    def process(self, accumulator, fl_ctx):
        """Aggregate the contributions from all the submitted FL clients.

        This function is not thread-safe.

        :param accumulator: List of encrypted, serialized contributions.
        :param fl_ctx: Original central plaintext model
        :return:
        """
        # The model data is in model.params as a dict.

        # Serialize and Encrypt Central Model
        print('BEGINNING AGGREGATION')
        model = fl_ctx.get_model()
        model_params = model.params
        print(protobuf_to_dict(model_params))
        enc_model = encryption(protobuf_to_dict(model_params))

        accumulator = [client['enc_values'] for client in accumulator]
        accumulator_lengths = [len(acc) for acc in accumulator]

        print('HOST MODEL ENCRYPTION SUCCESSFUL')

        # Encrypted Training in Secure Enclave
        enc_out, iv, tag = cy_host_modelaggregator(
            encrypted_accumulator = accumulator,
            accumulator_lengths = accumulator_lengths,
            accumulator_length = len(accumulator),
            encrypted_old_params = enc_model,
            old_params_length = len(enc_model)
        )

        print('AGGREGATION SUCCESSFUL')

        new_model_enc = {'enc_values': [enc_out, iv, tag]}

        # Update central model parameters
        new_model = decryption(new_model_enc)
        model.params = dict_to_protobuf(new_model)

        # model = fl_ctx.get_model()

        # print(protobuf_to_dict(model.params))

        # vars_to_aggregate = [set(item.get_model().params) for item in accumulator]
        # vars_to_aggregate = set.union(*vars_to_aggregate)

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

        #         print(proto_to_ndarray(data.params[v_name]), proto_to_ndarray(data.params[v_name]).shape)
        #         # weighted using local iterations
        #         shape = proto_to_ndarray(data.params[v_name]).shape
        #         weighted_value = proto_to_ndarray(data.params[v_name]).flatten() * float_n_iter
        #         np_vars.append(weighted_value)
        #     if not n_local_iters:
        #         continue  # didn't receive this variable from any clients
        #     new_val = np.sum(np_vars, axis=0) / np.sum(n_local_iters)
        #     print(v_name, '\n')
        #     print(f'Original Shape: {shape}, Flattened Shape: {new_val.shape}', '\n')
        #     new_val = new_val.reshape(shape)
        #     new_val += proto_to_ndarray(model.params[v_name])
        #     model.params[v_name].CopyFrom(ndarray_to_proto(new_val))
        # return False
