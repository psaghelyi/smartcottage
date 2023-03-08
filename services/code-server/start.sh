#!/bin/bash

docker run -d \
  --name=code-server \
  -e PUID=1000 \
  -e PGID=1000 \
  -e TZ=Europe/Budapest \
  -p 8443:8443 \
  -v code-server:/config \
  -v /mnt/sda1/Project:/Project \
  --restart unless-stopped \
  lscr.io/linuxserver/code-server:latest
