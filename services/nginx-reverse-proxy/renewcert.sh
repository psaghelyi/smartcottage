#!/bin/bash

docker run \
  --name=certbot \
  --network=host \
  --pull=always \
  -v $PWD/.certbot/www/:/var/www/certbot/:rw \
  -v $PWD/.certbot/conf/:/etc/letsencrypt/:rw \
  certbot/certbot:arm64v8-latest renew
