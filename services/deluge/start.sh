#!/bin/bash

docker run -d \
  --name=deluge \
  -e PUID=1000 \
  -e PGID=1000 \
  -e TZ=Europe/Budapest \
  --net host \
  -v deluge-config:/config \
  -v /mnt/sda1/Downloads:/downloads \
  --restart unless-stopped \
  lscr.io/linuxserver/deluge:latest


#-p 8112:8112 \
#-p 6881:6881 \
#-p 6881:6881/udp \
  