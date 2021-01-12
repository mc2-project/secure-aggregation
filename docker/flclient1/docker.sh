#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# docker run script for FL client

# TODO: modify this path to point to your Task09_Spleen directory
# local data directory
MY_DATA_DIR=/home/downloads/Task09_Spleen
# for all gpus use line below 
# GPU2USE=all 

# for 2 gpus use line below
# GPU2USE=2 

# for specific gpus as gpu#0 and gpu#2 use line below
# to use host network, use line below
NETARG="--net=host"

# FL clients do not need to open ports, so the following line is not needed.
#NETARG="-p 443:443 -p 8003:8003"

DOCKER_IMAGE="mc2project/clara-test:v1"
echo "Starting docker with $DOCKER_IMAGE"

TMPDIR=/mnt/ docker run --rm -it --name=flclient1 \
-v /snap/flatbuffers/current/include:/snap/flatbuffers/current/include \
-v /home/secure-aggregation:/workspace/secure-aggregation \
-v $DIR/..:/workspace/     -v $MY_DATA_DIR:/data/Task09_Spleen:ro     -w /workspace/     --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864     $NETARG     $DOCKER_IMAGE /bin/bash
