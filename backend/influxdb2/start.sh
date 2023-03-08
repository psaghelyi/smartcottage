#!/bin/bash

docker run -d \
    --name influxdb2 \
    --pull=always \
    --restart unless-stopped \
    --user $(id -u):$(id -g) \
    -p 8086:8086 \
    -v $PWD/.influxdb-data:/var/lib/influxdb2 \
    influxdb:latest
