#!/bin/bash

cd "$(dirname "$0")"
docker compose run --rm certbot certonly \
  --webroot --webroot-path /var/www/certbot/ \
  -d psaghelyi.ddns.net \
  --email psaghelyi@hotmail.com

