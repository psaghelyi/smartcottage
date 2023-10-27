#!/bin/bash

docker run -it --rm \
  --name=certbot-renew \
  --network=host \
  --pull=always \
  -v $PWD/.config/certbot/www:/var/www/certbot:rw \
  -v $PWD/.config/certbot/conf:/etc/letsencrypt:rw \
  certbot/certbot:latest renew
