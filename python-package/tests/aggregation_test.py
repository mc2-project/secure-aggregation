import numpy as np
from secagg import aggregate, encrypt, decrypt

def encrypt_model(model):
    for feature in list(model.keys()):
        if feature != '_contribution' and not feature.startswith('shape') and 'shape_'+feature not in model.keys():
            model['shape_' + feature] = list(model[feature].shape)
        model[feature] = model[feature].flatten().tolist()
    #  model_with_shape = {key.encode(): value for key, value in model.items()}
    print(model)
    #  enc_out, iv, tag = encrypt(model_with_shape)
    enc_out, iv, tag = encrypt(model)

    return enc_out, iv, tag

def decrypt_model(enc_out, iv, tag, model_len):
    #  model_map = decrypt(enc_out, iv, tag, model_len)
    model = decrypt(enc_out, iv, tag, model_len)
    #  model = {key.decode(): value for key, value in model_map.items()}
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
    contributions = [1.0, 1.0]
    enc_out, iv, tag = aggregate(
        encrypted_accumulator=encrypted_accumulator,
        accumulator_lengths=accumulator_lengths,
        accumulator_length=len(accumulator_lengths),
        encrypted_old_params=enc_host,
        old_params_length=len(enc_host[0]),
        contributions=contributions
    )

    host_model = decrypt_model(enc_out, iv, tag, len(enc_out))
    client1_model = decrypt_model(enc_client1[0], enc_client1[1], enc_client1[2], accumulator_lengths[0])
    client2_model = decrypt_model(enc_client2[0], enc_client2[1], enc_client2[2], accumulator_lengths[1])
    for feature in client1_model.keys():
        if feature.startswith('feature'):
            client1_model[feature] += np.random.randint(-3, 3, size=(client1_model[feature].shape))*0.01
            client2_model[feature] += np.random.randint(-3, 3, size=(client2_model[feature].shape))*0.01
            
    print('MODEL: ', host_model)

