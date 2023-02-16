#!/bin/bash

docker run -d \
  --name=nginx-reverse-proxy \
  --network=host \
  -v $PWD/nginx.conf:/etc/nginx/nginx.conf:ro \
  -v $PWD/.certbot/www:/var/www/certbot/:ro \
  -v $PWD/.certbot/conf/:/etc/nginx/ssl/:ro \
  -v $PWD/.certbot/dhparam.pem:/etc/nginx/dhparam.pem:ro \
  --restart unless-stopped \
  nginx:latest
