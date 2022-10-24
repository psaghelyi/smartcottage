#!/bin/bash

docker run  -d \
    --restart unless-stopped \
    --name=telegraf \
    --add-host host.docker.internal:host-gateway \
    --user telegraf:$(stat -c '%g' /var/run/docker.sock) \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v $PWD/telegraf.conf:/etc/telegraf/telegraf.conf:ro \
    -p 8125:8125 \
    -p 8092:8092/udp \
    -p 8094:8094 \
    telegraf
