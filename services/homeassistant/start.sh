#!/bin/bash

docker run -d \
    --name=homeassistant \
    -e PUID=$(id -u) \
    -e PGID=$(id -g) \
    -e TZ=Europe/Budapest \
    -p 8123:8123 \
    -v $PWD/.config:/config \
    --restart unless-stopped \
    lscr.io/linuxserver/homeassistant:latest
