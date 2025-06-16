#!/bin/bash

docker run -d \
  --name=squid-exporter \
  --pull=always \
  --restart=always \
  -p 9301:9301 \
  --add-host host.docker.internal:host-gateway \
  -e SQUID_HOSTNAME="host.docker.internal" \
  squid-exporter
