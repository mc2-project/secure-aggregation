from client_methods import cy_serialize, cy_deserialize, cy_encrypt_bytes, cy_decrypt_bytes
import model_pb2

def dict_to_protobuf_enc(model):
    serialized = cy_serialize(model)
    out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    while len(out) != len(serialized) or len(iv) != 16 or len(tag) != 16:
        out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    new_proto = model_pb2.data()
    new_proto.enc_data = out
    return new_proto, iv, tag


def protobuf_to_dict_dec(proto, iv, tag):
    enc_data = proto.enc_data
    dec_data = cy_decrypt_bytes(enc_data, iv, tag, len(enc_data))
    deserialized = cy_deserialize(dec_data)
    return deserialized

test = {'w1': [-3, -6, -9, -12], 
        'w2': [-6, -9, -12, -15], 
        'w3': [-9, -12, -15, -18]}

new_test = {key.encode(): val for key, val in test.items()}

for i in range(1000):
    proto_test, iv, tag = dict_to_protobuf_enc(new_test)
    test_out = protobuf_to_dict_dec(proto_test, iv, tag)
    print(test_out)

test_out = {key.decode(): val for key, val in new_test.items()}
print(test_out)




