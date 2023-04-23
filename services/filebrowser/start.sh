#!/bin/bash

docker run -d \
    --name filebrowser \
    --pull=always \
    --restart=always \
    --user=$(id -u):$(id -g) \
    -p 5555:80 \
    -v /mnt/sda1:/srv \
    -v $PWD/.filebrowser-data/filebrowser.db:/database/filebrowser.db \
    -v $PWD/.filebrowser-data/.filebrowser.json:/.filebrowser.json \
    filebrowser/filebrowser:latest


    