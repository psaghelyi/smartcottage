#!/bin/bash

docker run -d \
  --restart unless-stopped \
  --name=node-exporter \
  --publish=9100:9100 \
  --privileged \
  --pid="host" \
  -v "/:/host:ro,rslave" \
  quay.io/prometheus/node-exporter:latest \
  --path.rootfs=/host
