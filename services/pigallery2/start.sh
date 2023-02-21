#!/bin/bash

docker run -d \
    --name pigallery2 \
    --pull=always \
    --restart unless-stopped \
    -p 8880:80 \
    --user=$(id -u):$(id -g) \
    -e NODE_ENV=production \
    -v $PWD/.pigallery2-config:/app/data/config \
    -v $PWD/.pigallery2-data:/app/data/db \
    -v /mnt/sda1/Photos:/app/data/images \
    -v $PWD/.pigallery2-tmp:/app/data/tmp \
    bpatrik/pigallery2:latest

