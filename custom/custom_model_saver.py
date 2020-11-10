from berkeley_encrypt import encryption, protobuf_to_dict, dict_to_protobuf

from fed_learn.components.tf_model_saver import TFModelSaver


class CustomModelSaver(TFModelSaver):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def initialize(self, builder=None):
        super().initialize(builder)
        encrypted_params = encryption(protobuf_to_dict(self.model))
        encrypted_model = dict_to_protobuf(encrypted_params)
        print('BYTE SIZE: ', encrypted_model.ByteSize())
        self.model = encrypted_model
