#!/bin/bash

docker run -d \
    --name jellyfin \
    --user $(id -u) \
    --net=host \
    --device=/dev/video10:/dev/video10 \
    --device=/dev/video11:/dev/video11 \
    --device=/dev/video12:/dev/video12 \
    --volume jellyfin-config:/config \
    --volume jellyfin-cache:/cache \
    --volume /mnt/sda1/Media:/media \
    --restart=unless-stopped \
    jellyfin/jellyfin
