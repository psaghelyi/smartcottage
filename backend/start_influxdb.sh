#!/bin/bash


#influx backup /mnt/sda1/influxdb_backup_$(date '+%Y-%m-%d_%H-%M') -t iHi9f5K-BXUS0-fJBuGM7O6W5Irc0kmG97bKLXnRYc1zjnLugTeDqNNFz9zaI6GAKeTE_glSABH_U2h-WeWKpw==
#influx restore /mnt/sda1/influxdb_backup_2022-10-23_07-50 --full

docker run -d \
    --restart unless-stopped \
    -p 8086:8086 \
    --name influxdb2 \
    -v $PWD/.influxdb:/var/lib/influxdb2 \
    influxdb:2.4
