#!/bin/bash

docker run -d \
    --name bitwarden \
    --pull=always \
    --restart=unless-stopped \
    --env-file $PWD/.env \
    -v bitwarden-data:/data/ \
    -p 127.0.0.1:8880:80 \
    -p 127.0.0.1:3012:3012 \
    vaultwarden/server:latest
