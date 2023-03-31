#!/bin/bash

docker run -d \
    --name jellyfin \
    --privileged=true \
    --pull=always \
    --restart=unless-stopped \
    --user $(id -u):$(id -g) \
    -e TZ=Europe/Budapest \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=jellyfin \
    --net=host \
    --device=/dev/video10:/dev/video10 \
    --device=/dev/video11:/dev/video11 \
    --device=/dev/video12:/dev/video12 \
    --device=/dev/video13:/dev/video13 \
    --device=/dev/video14:/dev/video14 \
    --device=/dev/video15:/dev/video15 \
    --device=/dev/video16:/dev/video16 \
    --device=/dev/video18:/dev/video18 \
    --device=/dev/video19:/dev/video19 \
    --device=/dev/video20:/dev/video20 \
    --device=/dev/video21:/dev/video21 \
    --device=/dev/video22:/dev/video22 \
    --device=/dev/video23:/dev/video23 \
    --device=/dev/video31:/dev/video31 \
    --volume $PWD/.jellyfin-config:/config \
    --volume $PWD/.jellyfin-cache:/cache \
    --volume /mnt/sda1/Media:/media \
    --volume /mnt/sda1/Downloads/Completed:/Completed \
    jellyfin/jellyfin:latest
