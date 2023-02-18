#!/bin/bash

docker run -d \
    --name influxdb2 \
    --pull=always \
    --restart unless-stopped \
    -p 8086:8086 \
    --user $(id -u) \
    -v $PWD/.influxdb:/var/lib/influxdb2 \
    influxdb:latest
