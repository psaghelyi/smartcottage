#!/bin/bash

docker run -it --rm \
  --name=certbot-renew \
  --network=host \
  --pull=always \
  --user $(id -u):$(id -g) \
  -v $PWD/.config/certbot/www:/var/www/certbot:rw \
  -v $PWD/.config/certbot/conf:/etc/letsencrypt:rw \
  -v $PWD/.config/certbot/log:/var/log/letsencrypt:rw \
  -v $PWD/.config/certbot/lib:/var/lib/letsencrypt:rw \
  certbot/certbot:latest certificates