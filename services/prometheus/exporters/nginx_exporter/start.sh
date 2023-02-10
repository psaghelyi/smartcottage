#!/bin/bash

docker run -d \
  --restart unless-stopped \
  --name=nginx-exporter \
  --publish=9113:9113 \
  --add-host host.docker.internal:host-gateway \
  nginx/nginx-prometheus-exporter:0.10.0 \
  '-nginx.scrape-uri=http://host.docker.internal/stub_status'
