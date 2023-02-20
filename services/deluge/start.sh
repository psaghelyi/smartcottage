#!/bin/bash

docker run -d \
  --name=deluge \
  --pull=always \
  --restart unless-stopped \
  -e PUID=$(id -u) \
  -e PGID=$(id -g) \
  -e TZ=Europe/Budapest \
  --net host \
  -v $PWD/.deluge-config:/config \
  -v /mnt/sda1/Downloads:/downloads \
  lscr.io/linuxserver/deluge:latest


#-p 8112:8112 \
#-p 6881:6881 \
#-p 6881:6881/udp \
  