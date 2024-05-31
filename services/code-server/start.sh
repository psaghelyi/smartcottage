#!/bin/bash

docker run -d \
  --name=code-server \
  --pull=always \
  --restart unless-stopped \
  -e PUID=1000 \
  -e PGID=1000 \
  -e TZ=Europe/Budapest \
  -e PASSWORD=targa456 \
  -e DEFAULT_WORKSPACE=/Project \
  -p 8443:8443 \
  -v $PWD/.config:/config \
  -v /mnt/sda1/Project:/Project \
  lscr.io/linuxserver/code-server:latest


