#!/bin/bash

docker run -d \
    --name zeronsd \
    --restart=always \
    --network=host \
    -v /var/lib/zerotier-one/authtoken.secret:/authtoken.secret \
    -v /home/pi/.token:/token.txt \
    psaghelyi/zeronsd start -s /authtoken.secret -t /token.txt -d home.arpa d3ecf5726d92fd9c
