from client_methods import cy_serialize, cy_deserialize, cy_encrypt_bytes, cy_decrypt_bytes
import model_pb2

def dict_to_protobuf_enc(model):
    serialized = cy_serialize(model)
    out, iv, tag = cy_encrypt_bytes(serialized, len(serialized))
    new_proto = model_pb2.data()
    new_proto.enc_data = out
    return new_proto, iv, tag


def protobuf_to_dict_dec(proto, iv, tag):
    enc_data = proto.enc_data
    dec_data = cy_decrypt_bytes(enc_data, iv, tag, len(enc_data))
    deserialized = cy_deserialize(dec_data)
    return deserialized


test = {b'w1': [0.2, 1.9999], b'w2': [1.2, 1.2222, 1.76393033]}
proto_test, iv, tag = dict_to_protobuf_enc(test)
test_out = protobuf_to_dict_dec(proto_test, iv, tag)
print(test_out)




