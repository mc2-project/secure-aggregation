# The Custom Model Aggregator

## About
This folder contains all the necessary files to build and run a simple model aggregator in an enclave environment. The aggregator takes the element-wise sum of the variables received from multiple clients, weighs them by local iterations, then adds them to the existing model. All sensitive computation is done in an enclave environment, with encryption/decryption being performed inside the enclave.

## Building
Starting from within the `./server` folder:
1. Generate the necessary headers from the provided `.edl` file.
    * `oeedger8r --trusted-dir ./enclave --untrusted-dir ./host modelaggregator.edl`
2. Use CMake to build the makefiles in a corresponding `./build` folder.
    * `mkdir build && cd build && cmake ..`
3. Run tests:
    * `make check`
4. Build the enclave separately:
    * `make sign`

If building in simulate mode, set the environment variable `OE_SIMULATION` before step 2.
