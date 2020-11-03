from server_methods import cy_host_modelaggregator
from client_methods import cy_encrypt_bytes, cy_decrypt_bytes, cy_serialize, cy_deserialize
import numpy as np

def encrypt_model(model):
    for feature in list(model.keys()):
        # model['shape_' + feature] = model[feature].shape
        model[feature] = model[feature].flatten().tolist()
    model = {key.encode(): value for key, value in model.items()}
    serialized = cy_serialize(model)
    enc_out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    while len(enc_out) != len(serialized) or len(iv) != 16 or len(tag) != 16:
        enc_out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    return [enc_out, iv, tag]

def decrypt_model(enc_model):
    serialized_model = cy_decrypt_bytes(enc_model[0], enc_model[1], enc_model[2], len(enc_model[0]))
    model = cy_deserialize(serialized_model)
    model = {key.decode(): value for key, value in model.items()}
    for key, value in model.items():
        model[key] = np.array(value)
    return model
            
host_model = {
    'feature1': np.ones((1, 5)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10))
}

client1_model = {
    'feature1': np.ones((1, 5)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10)),
    '_contribution': np.array([1])
}

client2_model = {
    'feature1': np.ones((1, 5)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10)),
    '_contribution': np.array([1])
}

enc_host = encrypt_model(host_model)
enc_client1 = encrypt_model(client1_model)
enc_client2 = encrypt_model(client2_model)

encrypted_accumulator = [enc_client1, enc_client2]
accumulator_lengths = [len(model[0]) for model in encrypted_accumulator]

cy_host_modelaggregator(
    encrypted_accumulator = encrypted_accumulator,
    accumulator_lengths = accumulator_lengths,
    accumulator_length = len(accumulator_lengths),
    encrypted_old_params = enc_host,
    old_params_length = len(enc_host[0])
)


