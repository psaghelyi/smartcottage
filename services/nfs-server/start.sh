#!/bin/bash

docker run -d \
    --name=nfs-server \
    --restart unless-stopped \
    -v /mnt/sda1:/nfsshare \
    -e SHARED_DIRECTORY=/nfsshare \
    -e FILEPERMISSIONS_UID=$(id -u) \
    -e FILEPERMISSIONS_GID=$(id -g) \
    -e FILEPERMISSIONS_MODE=0744 \
    --cap-add SYS_ADMIN \
    -p 2049:2049 \
    openebs/nfs-server-alpine
