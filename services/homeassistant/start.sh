#!/bin/bash

docker run -d \
    --name=homeassistant \
    --pull=always \
    --restart unless-stopped \
    -e PUID=$(id -u) \
    -e PGID=$(id -g) \
    -e TZ=Europe/Budapest \
    -p 8123:8123 \
    -v $PWD/.config:/config \
    lscr.io/linuxserver/homeassistant:latest
