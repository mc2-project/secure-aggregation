#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
# docker run script for FL server
# local data directory
MY_DATA_DIR=/home/davidyi624/downloads/Task09_Spleen
# to use host network, use line below
NETARG="--net=host"
# or to expose specific ports, use line below
#NETARG="-p 8003:8003 -p 8002:8002"
DOCKER_IMAGE=clara-test
echo "Starting docker with $DOCKER_IMAGE"
TMPDIR=/mnt/ docker run --rm -it --name=flserver \
--mount type=bind,source=/snap/flatbuffers/current/include,target=/snap/flatbuffers/current/include \
--mount type=bind,source=/home/davidyi624/kvah,target=/workspace/kvah \
--mount type=bind,source=/home/davidyi624/kvah,target=/home/davidyi624/kvah \
-v $DIR/..:/workspace/ -v $MY_DATA_DIR:/data/Task09_Spleen -w /workspace/ --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864 $NETARG $DOCKER_IMAGE /bin/bash