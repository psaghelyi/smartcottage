#!/bin/bash

docker run \
  --name registry -d \
  --pull=always \
  --restart=always \
  --log-driver=gelf \
  --log-opt gelf-address=udp://localhost:12201 \
  --log-opt tag=registry \
  --user=$(id -u):$(id -g) \
  -p 5001:5000 \
  -e REGISTRY_STORAGE_FILESYSTEM_ROOTDIRECTORY=/data \
  -e REGISTRY_HTTP_HOST=https://psaghelyi.ddns.net:5000 \
  -v $PWD/.registry-data:/data \
  registry:2
