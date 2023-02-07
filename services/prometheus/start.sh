#!/bin/bash

ID=$(id -u)

docker run -d \
    --restart unless-stopped \
    -p 9090:9090 \
    --add-host host.docker.internal:host-gateway \
    --name prometheus \
    -v prometheus-data:/prometheus \
    -v $PWD/prometheus.yml:/etc/prometheus/prometheus.yml \
    prom/prometheus:latest

