import numpy as np

from fed_learn.numproto import proto_to_ndarray, ndarray_to_proto
from fed_learn.server.model_aggregator import Aggregator


class CustomModelAggregator(Aggregator):
    def process(self, accumulator, fl_ctx):
        """Aggregate the contributions from all the submitted FL clients.

        This function is not thread-safe.

        :param accumulator: List of encrypted, serialized contributions.
        :param fl_ctx: Original central plaintext model
        :return:
        """
        model = fl_ctx.get_model()
        vars_to_aggregate = [set(item.get_model().params) for item in accumulator]
        vars_to_aggregate = set.union(*vars_to_aggregate)

        for v_name in vars_to_aggregate:
            n_local_iters, np_vars = [], []
            for item in accumulator:
                data = item.get_model()
                if v_name not in data.params:
                    continue  # this item doesn't have the variable from client

                # contribution is a protobuf msg
                #   it has `n_iter` which represents number of local iterations 
                #   used to compute this contribution 
                acc = item.get_prop('_contribution')
                float_n_iter = np.float(acc.n_iter)
                n_local_iters.append(float_n_iter)

                print(proto_to_ndarray(data.params[v_name]), proto_to_ndarray(data.params[v_name]).shape)
                # weighted using local iterations
                shape = proto_to_ndarray(data.params[v_name]).shape
                weighted_value = proto_to_ndarray(data.params[v_name]).flatten() * float_n_iter
                np_vars.append(weighted_value)
            if not n_local_iters:
                continue  # didn't receive this variable from any clients
            new_val = np.sum(np_vars, axis=0) / np.sum(n_local_iters)
            new_val = new_val.reshape(shape)
            new_val += proto_to_ndarray(model.params[v_name])
            model.params[v_name].CopyFrom(ndarray_to_proto(new_val))
        return False