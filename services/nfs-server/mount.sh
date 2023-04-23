#!/bin/bash


docker run -it --rm \
    --name nfs-client \
    --privileged=true \
    --network=host \
    -v /Users/psaghelyi-remote/rpi4:/mnt/host \
    -e SERVER=192.168.1.40 \
    -e SHARE=nfsshare \
    -e MOUNT_OPTIONS="nfsvers=3,ro" \
    -e FSTYPE=nfs3 \
    -e MOUNTPOINT=/mnt/host \
    d3fk/nfs-client

# sudo mount -t nfs -o resvport,rw 192.168.1.40:/nfsshare ~/rpi4

