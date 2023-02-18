#!/bin/bash

docker volume create portainer_data

docker run -d \
    --name portainer \
    --pull=always \
    --restart=unless-stopped \
    -p 9000:9000 \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -v portainer_data:/data \
    portainer/portainer-ce:latest
