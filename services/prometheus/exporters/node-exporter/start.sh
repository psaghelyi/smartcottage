#!/bin/bash

docker run -d \
  --name=node-exporter \
  --pull=always \
  --restart=always \
  --privileged \
  --pid="host" \
  --network="host" \
  -v "/:/host:ro,rslave" \
  quay.io/prometheus/node-exporter:latest \
  --path.rootfs=/host
