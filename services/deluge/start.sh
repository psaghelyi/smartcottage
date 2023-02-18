#!/bin/bash

docker run -d \
  --name=deluge \
  --pull=always \
  --restart unless-stopped \
  -e PUID=1000 \
  -e PGID=1000 \
  -e TZ=Europe/Budapest \
  --net host \
  -v deluge-config:/config \
  -v /mnt/sda1/Downloads:/downloads \
  lscr.io/linuxserver/deluge:latest


#-p 8112:8112 \
#-p 6881:6881 \
#-p 6881:6881/udp \
  