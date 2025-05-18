#!/bin/bash

docker run -d \
    --name uptime-kuma \
    --pull=always \
    --restart=always \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=uptime-kuma \
    -v ./.kuma-data:/app/data \
    -p 3001:3001  \
    louislam/uptime-kuma:latest


    