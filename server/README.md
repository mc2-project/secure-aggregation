# The Custom Model Aggregator

## About
This folder contains all the necessary files to build and run a simple model aggregator in an enclave environment. The aggregator takes the element-wise sum of the variables received from multiple clients, weighs them by local iterations, then adds them to the existing model. All sensitive computation is done in an enclave environment, with encryption/decryption being performed inside.

## Dependencies
Building the server code requires the following dependencies that can be installed on Ubuntu as follows:
1. Pip3:
    * `sudo apt install python3-pip`
2. CMake:
    * `pip3 install cmake`
1. FlatBuffers:
    * `sudo apt install snapd`
    * `sudo snap install flatbuffers`
2. Mbed TLS:
    * `sudo apt install libmbedtls-dev`
3. Open Enclave:
    * Follow the instructions [here](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/install_oe_sdk-Ubuntu_18.04.md)
4. OpenMP:
    * `sudo apt install libomp-dev`

## Additional Flags
If you would like to build in debugging mode (extra logs of enclave behavior available), set the `OE_DEBUG` environment variable: `export OE_DEBUG=1`.

If building on a machine with hardware that does not support Intel SGX, you can still emulate the behavior of an enclave by building in simulation mode. To do this, set the `OE_SIMULATION` environment variable: `export OE_SIMULATION=1`.

## Building
1. Use CMake to build the makefiles in a corresponding `./build` folder:
    * `mkdir build && cd build`
    * `cmake ..`
2. Build all targets:
    * `make`
3. Alternatively, you can chose to only build and run tests:
    * `make check`
4. Or build and sign the enclave separately:
    * `make sign`
