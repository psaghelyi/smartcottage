#!/bin/bash

docker run -d \
    --name jellyfin \
    --user $(id -u) \
    --publish=8096:8096 \
    --publish=8920:8920 \
    --publish=1900:1900/udp \
    --publish=7359:7359/udp \
    --volume $PWD/.config:/config \
    --volume $PWD/.cache:/cache \
    --mount type=bind,source=/mnt/sda1/Media,target=/media \
    --restart=unless-stopped \
    jellyfin/jellyfin
