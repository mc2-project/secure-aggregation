# The Custom Model Aggregator

## About
This folder contains all the necessary files to build and run a simple model aggregator in an enclave environment. The aggregator takes the element-wise sum of the variables received from multiple clients, weighs them by local iterations, then adds them to the existing model. All sensitive computation is done in an enclave environment, with encryption/decryption being performed inside the enclave.

## Building
