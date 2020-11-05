# The Custom Model Aggregator

## About
This folder contains all the necessary files to build and run a simple model aggregator in an enclave environment. The aggregator takes the element-wise sum of the variables received from multiple clients, weighs them by local iterations, then adds them to the existing model. All sensitive computation is done in an enclave environment, with encryption/decryption being performed inside.

## Building
1. Use CMake to build the makefiles in a corresponding `./build` folder.
    * `mkdir build && cd build`
    * `cmake ..`
2. Build all targets:
    * `make`
3. Alternatively, you can chose to only build and run tests:
    * `make check`
4. Or build and sign the enclave separately:
    * `make sign`

If building in simulate mode, set the environment variable `OE_SIMULATION=1` before step 1.
