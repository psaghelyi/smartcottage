#!/bin/bash

docker run  -d \
    --name=telegraf \
    --pull=always \
    --restart=always \
    --add-host host.docker.internal:host-gateway \
    --user telegraf:$(stat -c '%g' /var/run/docker.sock) \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v $PWD/telegraf.conf:/etc/telegraf/telegraf.conf:ro \
    -p 8125:8125/udp \
    telegraf
