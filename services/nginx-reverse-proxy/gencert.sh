#!/bin/bash

docker run -it --rm \
  --network=host \
  -v $PWD/.certbot/www:/var/www/certbot/:rw \
  -v $PWD/.certbot/conf/:/etc/letsencrypt/:rw \
  certbot/certbot:arm64v8-latest certonly \
  --webroot --webroot-path /var/www/certbot/ \
  -d psaghelyi.ddns.net \
  --email psaghelyi@hotmail.com
