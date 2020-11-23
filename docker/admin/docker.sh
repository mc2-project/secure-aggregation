#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# docker run script for FL admin
# to use host network, use line below
NETARG="--net=host"
DOCKER_IMAGE=clara-test2
echo "Starting docker with $DOCKER_IMAGE"

docker run --privileged --rm -it --name=fladmin \
--mount type=bind,source=/snap/flatbuffers/current/include,target=/snap/flatbuffers/current/include -v $DIR/..:/workspace/ \
--mount type=bind,source=/home/ubuntu/test/secure-aggregation/server/,target=/home/ubuntu/test/secure-aggregation/server/  \
-w /workspace/ --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864 $NETARG $DOCKER_IMAGE /bin/bash
