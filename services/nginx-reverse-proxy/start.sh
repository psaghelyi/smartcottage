#!/bin/bash

docker run -d \
  --name=nginx-reverse-proxy \
  --pull=always \
  --restart=always \
  --network=host \
  --log-driver=gelf \
  --log-opt gelf-address=udp://localhost:12201 \
  --log-opt tag=nginx \
  -v $PWD/nginx.conf:/etc/nginx/nginx.conf:ro \
  -v $PWD/nginx.conf.d:/etc/nginx/conf.d:ro \
  -v $PWD/.config/dhparam.pem:/etc/nginx/dhparam.pem:ro \
  -v $PWD/.config/certbot/conf:/etc/nginx/ssl:ro \
  -v $PWD/.config/certbot/www:/var/www/certbot:rw \
  nginx:latest
