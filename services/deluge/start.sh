#!/bin/bash

docker run -d \
    --name=deluge \
    --pull=always \
    --restart=always \
    -e PUID=$(id -u) \
    -e PGID=$(id -g) \
    --network=host \
    --privileged \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=deluge \
    -e TZ=Europe/Budapest \
    -v $PWD/.deluge-config:/config \
    -v /mnt/sda1/Downloads:/downloads \
    lscr.io/linuxserver/deluge:latest

