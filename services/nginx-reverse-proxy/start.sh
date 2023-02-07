#!/bin/bash

docker run -d \
  --name=nginx-reverse-proxy \
  --network=host \
  -v $PWD/nginx.conf:/etc/nginx/nginx.conf \
  -v $PWD/.cert/bundle.pem:/etc/nginx/bundle.pem \
  --restart unless-stopped \
  nginx:latest
