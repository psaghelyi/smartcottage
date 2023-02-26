#!/bin/bash

docker run -d \
    --name jellyfin \
    --pull=always \
    --restart=unless-stopped \
    --user $(id -u):$(id -g) \
    --net=host \
    --device=/dev/video10:/dev/video10 \
    --device=/dev/video11:/dev/video11 \
    --device=/dev/video12:/dev/video12 \
    --volume $PWD/.jellyfin-config:/config \
    --volume $PWD/.jellyfin-cache:/cache \
    --volume /mnt/sda1/Media:/media \
    --volume /mnt/sda1/Downloads/Completed:/Completed \
    jellyfin/jellyfin:latest
