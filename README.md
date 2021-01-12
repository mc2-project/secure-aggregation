# Secure Aggregation
This secure aggregation project plugs into NVIDIA Clara and performs federated learning server side aggregation using hardware enclaves, preventing the server from observing any local model updates. For testing, one can choose to run the code in hardware mode if they have access to machines with secure enclaves (i.e. Intel SGX), or in simulation mode. More on choosing execution mode in the [Installation](#installation) section.

## Installation
These instructions assume that you've cloned the [original NVIDIA aggregation code](https://drive.google.com/drive/u/0/folders/1Y2Hk9YCI12aF3MUA8Yxi6PMx5BEp8Pk2) into a directory called `nvidia` and the [RISE Secure Aggregation code](https://github.com/mc2-project/secure-aggregation/tree/develop) into a directory called `secure-aggregation`.

Navigate to the RISE Secure Aggregation repo.

1. Outside any container on the host machine, install Flatbuffers.

```
snap install flatbuffers
```

2. Before building the C++ code, if running in simulation mode, export the following environment variable.

```
export OE_SIMULATION=1
```

Otherwise, add the following line in `docker/server/docker.sh` below Line 21 to expose the SGX hardware to the Docker container.
```
--device /dev/sgx \
```

Note: the device may be at `/dev/isgx` -- check the directory to confirm.

3. Build the C++ code.

```
cd server
mkdir build
cd build
cmake ..
make -j4
```

4. Copy the 4 files in `custom/` to `admin/transfer/custom_ct_spleen/custom/`. 

```
cp custom/* ../nvidia/admin/transfer/custom_ct_spleen/custom/
```

5. Copy the Docker startup scripts to their respective directories. Once you copy the scripts, you'll likely have to change the paths to `Task09_Spleen` and to the `secure-aggregation` repo in each script. There are `TODO`s marked in each script where you'll have to change the paths.

```
cp docker/server/docker.sh ../nvidia/server/startup/
cp docker/flclient1/docker.sh ../nvidia/flclient1/startup/
cp docker/flclient2/docker.sh ../nvidia/flclient2/startup/
cp docker/admin/docker.sh ../nvidia/admin/startup/
```

6. Copy the setup scripts to their respective directories. These scripts will be run within each container in place of `startup.sh`.

```
cp docker/server/run.sh ../nvidia/server/
cp docker/flclient1/run.sh ../nvidia/flclient1/
cp docker/flclient2/run.sh ../nvidia/flclient2/
cp docker/admin/run.sh ../nvidia/admin/
```

7. Pull the updated Docker image from [Docker Hub](https://hub.docker.com/repository/docker/mc2project/clara-test). 

```
docker pull mc2project/clara-test:v1
```

8. Launch each container as specified in the original instructions.

9. In the server container, run the setup and start script that we provide.

```
./run.sh
```

10. Do the same in the flclient1 and flclient2 containers.

```
./run.sh
```

11. Lastly, run the setup script in the admin container and enter the same commands as specified in the original instructions.

```
./run.sh
upload_folder custom_ct_spleen
set_run_number 1
deploy custom_ct_spleen server
deploy custom_ct_spleen client
start server
start client

```
