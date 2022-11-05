#!/bin/bash

docker run -d \
 --name jellyfin \
 --user $(id -u) \
 --publish=8096:8096 \
 --volume $PWD/.config:/config \
 --volume $PWD/.cache:/cache \
 --mount type=bind,source=/mnt/sda1/Media,target=/media \
 --restart=unless-stopped \
 jellyfin/jellyfin
