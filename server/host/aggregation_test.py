from server_methods import cy_host_modelaggregator
from client_methods import cy_encrypt_bytes, cy_decrypt_bytes, cy_serialize, cy_deserialize
import numpy as np

def encrypt_model(model):
    for feature in list(model.keys()):
        if feature != '_contribution' and not feature.startswith('shape') and 'shape_'+feature not in model.keys():
            model['shape_' + feature] = list(model[feature].shape)
        model[feature] = model[feature].flatten().tolist()
    model = {key.encode(): value for key, value in model.items()}
    print("about to serialize")
    #  serialized, buffer_len = cy_serialize(model)
    #  print("Buffer len: ", buffer_len)
    #  serialized = serialized[:buffer_len]
    #  print("serialized!")
    #  cy_deserialize(serialized)
    #  enc_out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    enc_out, iv, tag = cy_serialize(model)

    return [enc_out, iv, tag]

def decrypt_model(enc_out, iv, tag, model_len):
    print("decrypting model")
    model = cy_decrypt_bytes(enc_out, iv, tag, model_len)
    print("decrypted bytes")
    #  print("decrypted bytes")
    #  serialized_model = serialized_model[:model_len]
    #  model = cy_deserialize(serialized_model)
    #  print("deserializing")
    model = {key.decode(): value for key, value in model.items()}
    for key, value in model.items():
        model[key] = np.array(value)
        if not key.startswith('shape_') and key != '_contribution':
            model[key] = np.array(value).reshape(np.array(model['shape_'+key]).astype(int))
    return model
            
host_model = {
    'feature1': np.ones((3, 3, 2)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10))
}

client1_model = {
    'feature1': np.ones((3, 3, 2)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10)),
    '_contribution': np.array([1])
}

client2_model = {
    'feature1': np.ones((3, 3, 2)),
    'feature2': np.ones((1, 10)),
    'feature3': np.ones((1, 10)),
    '_contribution': np.array([1])
}

for i in range(10):

    print(f'RUN {i}')

    enc_host = encrypt_model(host_model)
    enc_client1 = encrypt_model(client1_model)
    enc_client2 = encrypt_model(client2_model)

    encrypted_accumulator = [enc_client1, enc_client2]
    accumulator_lengths = [len(model[0]) for model in encrypted_accumulator]

    print("host model aggregator")
    enc_out, iv, tag = cy_host_modelaggregator(
        encrypted_accumulator=encrypted_accumulator,
        accumulator_lengths=accumulator_lengths,
        accumulator_length=len(accumulator_lengths),
        encrypted_old_params=enc_host,
        old_params_length=len(enc_host[0])
    )

    print("agregated!")
    print("length end out: ", len(enc_out))
    host_model = decrypt_model(enc_out, iv, tag, len(enc_out))
    print("host MODEL")
    client1_model = decrypt_model(enc_client1[0], enc_client1[1], enc_client1[2], accumulator_lengths[0])
    print("clinet1 model")
    client2_model = decrypt_model(enc_client2[0], enc_client2[1], enc_client2[2], accumulator_lengths[1])
    print("decrypted")
    for feature in client1_model.keys():
        if feature.startswith('feature'):
            client1_model[feature] += np.random.randint(-3, 3, size=(client1_model[feature].shape))*0.01
            client2_model[feature] += np.random.randint(-3, 3, size=(client2_model[feature].shape))*0.01
            
    print('MODEL: ', host_model)



