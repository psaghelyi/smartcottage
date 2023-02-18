#!/bin/bash

ID=$(id -u)

docker run -d \
    --name prometheus \
    --pull=always \
    --restart unless-stopped \
    -p 9090:9090 \
    --add-host host.docker.internal:host-gateway \
    -v prometheus-data:/prometheus \
    -v $PWD/prometheus.yml:/etc/prometheus/prometheus.yml \
    prom/prometheus:latest

