#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# docker run script for FL admin
# to use host network, use line below
NETARG="--net=host"

# Admin clients do not need to open ports, so the following line is not needed.
#NETARG="-p 8003:8003"

DOCKER_IMAGE=mc2project/clara-test:v1
echo "Starting docker with $DOCKER_IMAGE"
docker run --rm -it --name=fladmin -v $DIR/..:/workspace/ -w /workspace/ --shm-size=1g --ulimit memlock=-1 --ulimit stack=67108864 $NETARG $DOCKER_IMAGE /bin/bash
