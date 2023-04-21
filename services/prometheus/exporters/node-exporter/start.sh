#!/bin/bash

docker run -d \
  --restart unless-stopped \
  --name=node-exporter \
  --privileged \
  --pid="host" \
  --network="host" \
  -v "/:/host:ro,rslave" \
  quay.io/prometheus/node-exporter:latest \
  --path.rootfs=/host
