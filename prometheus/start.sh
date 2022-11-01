#!/bin/bash

ID=$(id -u)

docker run -d \
    --restart unless-stopped \
    -p 9090:9090 \
    --name prometheus \
    -v $PWD/.prometheus:/prometheus \
    -v $PWD/prometheus.yml:/etc/prometheus/prometheus.yml \
    prom/prometheus:latest

