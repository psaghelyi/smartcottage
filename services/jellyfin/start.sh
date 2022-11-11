#!/bin/bash

docker run -d \
    --name jellyfin \
    --user $(id -u) \
    --net=host \
    --volume jellyfin-config:/config \
    --volume jellyfin-cache:/cache \
    --volume /mnt/sda1/Media:/media \
    --restart=unless-stopped \
    jellyfin/jellyfin
