#!/bin/bash

docker run -d \
  --name=code-server \
  --pull=always \
  --restart=always \
  -e PUID=1000 \
  -e PGID=1000 \
  -e TZ=Europe/Budapest \
  -p 8443:8443 \
  -v code-server:/config \
  -v /mnt/sda1/Project:/Project \
  lscr.io/linuxserver/code-server:latest
