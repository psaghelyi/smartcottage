#!/bin/bash

docker run -d \
    --name jellyfin \
    --privileged=true \
    --pull=always \
    --restart=always \
    --user $(id -u):$(id -g) \
    -e TZ=Europe/Budapest \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=jellyfin \
    --network=host \
    --device /dev/dri:/dev/dri \
    --volume $PWD/.jellyfin-config:/config \
    --volume $PWD/.jellyfin-cache:/cache \
    --volume /mnt/sda1/Media:/media \
    --volume /mnt/sda1/Downloads/Completed:/Completed \
    jellyfin/jellyfin:latest
