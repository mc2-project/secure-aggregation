#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# docker run script for FL server

# TODO: modify this path to point to you Task09_Spleen directory
# local data directory
MY_DATA_DIR=/home/davidyi624/downloads/Task09_Spleen

# to use host network, use line below
NETARG="--net=host"

# or to expose specific ports, use line below
#NETARG="-p 8003:8003 -p 8002:8002"

DOCKER_IMAGE="mc2project/clara-test:v1"
echo "Starting docker with $DOCKER_IMAGE"

# TODO: Modify the src mount path of the secure-aggregation repo
TMPDIR=/mnt/ docker run --rm -it --name=flserver \
-v /snap/flatbuffers/current/include:/snap/flatbuffers/current/include \
-v /home/davidyi624/kvah:/workspace/kvah \
--device /dev/sgx \
-v $DIR/..:/workspace/ -v $MY_DATA_DIR:/data/Task09_Spleen -w /workspace/ --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864 $NETARG $DOCKER_IMAGE /bin/bash
