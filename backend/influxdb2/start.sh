#!/bin/bash

docker run -d \
    --restart unless-stopped \
    -p 8086:8086 \
    --user $(id -u) \
    --name influxdb2 \
    -v $PWD/.influxdb:/var/lib/influxdb2 \
    influxdb:latest
