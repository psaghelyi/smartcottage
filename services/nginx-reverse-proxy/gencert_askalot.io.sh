#!/bin/bash

docker run -it --rm \
  --name=certbot-gencert \
  --network=host \
  --pull=always \
  --user $(id -u):$(id -g) \
  -v $PWD/.config/certbot/conf:/etc/letsencrypt:rw \
  -v $PWD/.config/certbot/log:/var/log/letsencrypt:rw \
  -v $PWD/.config/certbot/lib:/var/lib/letsencrypt:rw \
  -v $PWD/.config/cloudflare/cloudflare.ini:/etc/letsencrypt/cloudflare.ini:ro \
  certbot/dns-cloudflare:latest certonly \
  --dns-cloudflare \
  --dns-cloudflare-credentials /etc/letsencrypt/cloudflare.ini \
  --agree-tos --non-interactive \
  -d askalot.io \
  -d '*.askalot.io' \
  --email psaghelyi@gmail.com
