#!/bin/bash

docker run \
    --name mailserver \
    --pull=always \
    --restart=always \
    --env-file ./.mailserver-config/mailserver.env \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=mailserver \
    -p 25:25  \
    -p 143:143 \
    -p 465:465 \
    -p 587:587 \
    -p 993:993 \
    -v ./.mailserver-data/mail-data/:/var/mail/ \
    -v ./.mailserver-data/mail-state/:/var/mail-state/ \
    -v ./.mailserver-data/mail-logs/:/var/log/mail/ \
    -v ./.mailserver-data/config/:/tmp/docker-mailserver/ \
    -v /etc/localtime:/etc/localtime:ro \
    -v ./../nginx-reverse-proxy/.config/certbot/conf/live/askalot.io/fullchain.pem:/etc/letsencrypt/live/mail.askalot.io/fullchain.pem:ro \
    -v ./../nginx-reverse-proxy/.config/certbot/conf/live/askalot.io/privkey.pem:/etc/letsencrypt/live/mail.askalot.io/privkey.pem:ro \
    ghcr.io/docker-mailserver/docker-mailserver:latest


    