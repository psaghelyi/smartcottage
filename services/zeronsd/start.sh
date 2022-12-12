#!/bin/bash

ID=$(id -u)

docker run -d \
    --restart unless-stopped \
    --name zeronsd \
    --net host \
    -v /var/lib/zerotier-one/authtoken.secret:/authtoken.secret \
    -v $HOME/.zeronsd/token.txt:/token.txt \
    zerotier/zeronsd start -s /authtoken.secret -t /token.txt d3ecf5726d92fd9c
