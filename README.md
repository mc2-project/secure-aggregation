These instructions assume that you've cloned the [original NVIDIA aggregation code](https://drive.google.com/drive/u/0/folders/1Y2Hk9YCI12aF3MUA8Yxi6PMx5BEp8Pk2) into a directory called `nvidia` and the RISE Secure Aggregation code into a directory called `secure-aggregation`.

Navigate to the RISE Secure Aggregation repo.

1. Outside any container on the host machine, install Flatbuffers.

```
snap install flatbuffers
```

2. Build the C++ code

```
cd server
mkdir build
cd build
cmake ..
make -j4
```

3. Copy the 4 files in `custom/` to `admin/transfer/custom_ct_spleen/custom/`. 

```
cp custom/* ../nvidia/admin/transfer/custom_ct_spleen/custom/
```

4. Copy the Docker startup scripts to their respective directories. Once you copy the scripts, you'll likely have to change the paths to `Task09_Spleen` and to the `secure-aggregation` repo in each script. There are `TODO`s marked in each script where you'll have to change the paths.

```
cp docker/server/docker.sh ../nvidia/server/startup/
cp docker/flclient1/docker.sh ../nvidia/flclient1/startup/
cp docker/flclient2/docker.sh ../nvidia/flclient2/startup/
cp docker/admin/docker.sh ../nvidia/admin/startup/
```

5. Copy the setup scripts to their respective directories. These scripts will be run within each container in place of `startup.sh`.

```
cp docker/server/run.sh ../nvidia/server/
cp docker/flclient1/run.sh ../nvidia/flclient1/
cp docker/flclient2/run.sh ../nvidia/flclient2/
cp docker/admin/run.sh ../nvidia/admin/
```

6. Pull the updated Docker image from [Docker Hub](https://hub.docker.com/repository/docker/mc2project/clara-test). 

```
docker pull mc2project/clara-test:v1
```
