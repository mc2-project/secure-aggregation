#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# docker run script for FL client
# local data directory
MY_DATA_DIR=/home/ubuntu/test/downloads/Task09_Spleen
# for all gpus use line below 
# GPU2USE=all 
# for specific gpus as gpu#0 use line below
#GPU2USE='"device=0"'
# to use host network, use line below
NETARG="--net=host"
# FL clients do not need to open ports, so the following line is not needed.
DOCKER_IMAGE=clara-test2
echo "Starting docker with $DOCKER_IMAGE"
TMPDIR=/mnt/ docker run --rm -it --name=flclient1   \
--mount type=bind,source=/snap/flatbuffers/current/include,target=/snap/flatbuffers/current/include \
--mount type=bind,source=/home/ubuntu/test/secure-aggregation/server/,target=/home/ubuntu/test/secure-aggregation/server/ \
-v $DIR/../../kvah:/workspace/kvah   -v $DIR/..:/workspace/     -v $MY_DATA_DIR:/data/Task09_Spleen:ro     -w /workspace/     --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864     $NETARG     $DOCKER_IMAGE /bin/bash
    #-u $(id -u):$(id -g) -v /etc/passwd:/etc/passwd -v /etc/group:/etc/group  
