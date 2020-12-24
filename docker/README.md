
## Installation

  

The following instructions are specifically for adding enclave support to NVIDIA's Clara Train federated learning v3.1 SDK.

  
  
  

1. Install Pip3:

	`sudo apt install python3-pip`

2. Install CMake (version 11+)

	`pip3 install cmake`

3. Install FlatBuffers. Snap installations do not work inside of Docker containers, so follow the steps above outside the container and mount the FlatBuffers installation to the same location inside the container when starting the container, i.e. add the following flag to your `docker exec` command:

	`-v /snap/flatbuffers/current/include:/snap/flatbuffers/current/include`

	```
	sudo apt install snapd
	sudo snap install flatbuffers
	```

4. Install Mbed TLS:

	`sudo apt install libmbedtls-dev`

5. Install Open Enclave by following the instructions [here](https://github.com/openenclave/openenclave/blob/master/docs/GettingStartedDocs/install_oe_sdk-Ubuntu_18.04.md). In the final step, you need to install an older verison of the Open Enclave SDK (0.12.0) by specifying the version as done below:.

	```
	sudo apt -y install clang-7 libssl-dev gdb libsgx-enclave-common libsgx-enclave-common-dev libprotobuf10 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave=0.12.0
	```

6. Install OpenMP:

	`sudo apt install libomp-dev`

  

## Setting up Secure Aggregation within the NVIDIA FL environment

  

After installing the dependencies above, you will need to build and install the secure aggregation libraries from [secure-aggregation](https://github.com/mc2-project/secure-aggregation). Follow steps 1-5 below **outside of the docker containers**.

  

1. Clone Secure Aggregation

	`git clone https://github.com/mc2-project/secure-aggregation.git`

2. Navigate to `secure-aggregation/server` and build the encrypted model aggregation library:

	```
	mkdir build && cd build
	cmake ..
	make
	```
3. Add the following [start scripts](https://github.com/kvah/secure-aggregation/tree/master/docker) to the corresponding container directories. 
4. In the server startup script (`server/startup/docker.sh`), mount the following directories in the docker run command:
	```
	-v /snap/flatbuffers/current/include:/snap/flatbuffers/current/include
	-v /path/to/secure-aggregation:/path/to/secure-aggregation
	-v /path/to/secure-aggregation:/workspace/secure-aggregation
	```
5. In the client startup scripts(`flclientx/startup/docker.sh`), mount the following directores in the docker run command:
	```
	-v/snap/flatbuffers/current/include:/snap/flatbuffers/current/include
	-v /path/to/secure-aggregation:/workspace/secure-aggregation
	```
6. Start each relevant container as you normally would and run the corresponding bash run scripts from step 3.
7. Execute the following sequence from the admin container as done before:
	```
	upload_folder custom_ct_spleen
	set_run_number 1
	deploy custom_ct_spleen server
	deploy custom_ct_spleen client
	start server
	start client
	```