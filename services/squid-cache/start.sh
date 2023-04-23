#!/bin/bash

docker run -d \
    --name squid-cache \
    --pull=always \
    --restart=always \
    -p 3128:3128 \
    --user=$(id -u):$(id -g) \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=squid-cache \
    -e TZ=Europe/Budapest \
    -v $PWD/squid.conf:/etc/squid/squid.conf \
    -v $PWD/.squid-data:/var/spool/squid \
    -v $PWD/.squid-logs:/var/log/squid \
    ubuntu/squid:latest \
   