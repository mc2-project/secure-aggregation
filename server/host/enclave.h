#include "modelaggregator_u.h"

class Enclave {
    private:
        int num_clients;
        oe_enclave_t* enclave_ref;
        oe_result_t enclave_ret;

    public:

        Enclave(char* path, uint32_t flags)
        {
            enclave_ret = oe_create_modelaggregator_enclave(
                path, OE_ENCLAVE_TYPE_AUTO, flags, NULL, 0, &this->enclave_ref);
        }

        oe_enclave_t* getEnclave() 
        {
            return this->enclave_ref;
        }

        oe_enclave_t** getEnclaveRef() 
        {
            return &(this->enclave_ref);
        }

        oe_result_t getEnclaveRet() 
        {
            return this->enclave_ret;
        }


        int set_num_clients(int num) 
        {
          num_clients = num;
        }

        int get_num_clients() 
        {
          return num_clients;
        }
};;
//class Enclave {
//    private:
//        // Private constructor to prevent instancing
//        Enclave() {}
//        int num_clients;
//
//    public:
//        // Don't forget to declare these two. You want to make sure they
//        // are unacceptable otherwise you may accidentally get copies of
//        // your singleton appearing.
//        Enclave (Enclave const&) = delete;
//        void operator=(Enclave const&) = delete;
//
//        oe_enclave_t* enclave_ref;
//        int enclave_ret;
//
//        static Enclave& getInstance() {
//            static Enclave instance;
//            return instance;
//        }
//
//        oe_enclave_t* getEnclave() {
//            return this->enclave_ref;
//        }
//
//        oe_enclave_t** getEnclaveRef() {
//            return &(this->enclave_ref);
//        }
//
//        int set_num_clients(int num) {
//          num_clients = num;
//        }
//
//        int get_num_clients() {
//          return num_clients;
//        }
//
//};
