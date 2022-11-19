#!/bin/bash

docker run -d \
    --name=homeassistant \
    --net=host \
    -e PUID=1000 \
    -e PGID=1000 \
    -e TZ=Europe/Budapest \
    -p 8123:8123 \
    -v homeassistant-config:/config \
    --restart unless-stopped \
    lscr.io/linuxserver/homeassistant:latest
