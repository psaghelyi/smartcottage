#!/bin/bash

docker run -d \
  --name=nginx-reverse-proxy \
  -p 12345:12345 \
  -v $PWD/nginx.conf:/etc/nginx/nginx.conf \
  -v $PWD/.cert/bundle.pem:/etc/nginx/bundle.pem \
  --restart unless-stopped \
  --add-host host.docker.internal:host-gateway \
  nginx:latest
