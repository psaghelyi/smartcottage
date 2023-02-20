#!/bin/bash

docker run -d \
    --name prometheus \
    --pull=always \
    --restart unless-stopped \
    -p 9090:9090 \
    --user=$(id -u):$(id -g) \
    --add-host host.docker.internal:host-gateway \
    -v $PWD/.prometheus-data:/prometheus \
    -v $PWD/prometheus.yml:/etc/prometheus/prometheus.yml \
    prom/prometheus:latest

