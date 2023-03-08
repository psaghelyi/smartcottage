#!/bin/bash

docker run -it --rm \
  --name=certbot-gencert \
  --network=host \
  --pull=always \
  -v $PWD/.config/www:/var/www/certbot/:rw \
  -v $PWD/.config/conf/:/etc/letsencrypt/:rw \
  certbot/certbot:arm64v8-latest certonly \
  --webroot --webroot-path /var/www/certbot/ \
  -d psaghelyi.ddns.net \
  --email psaghelyi@hotmail.com
