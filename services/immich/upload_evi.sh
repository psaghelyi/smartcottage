#!/bin/bash

docker run -it \
-v "/mnt/sda1:/mnt/sda1:ro" \
-e IMMICH_INSTANCE_URL=http://192.168.1.100:2283/api \
-e IMMICH_API_KEY=56VwJKqfSFaAviIFZOVmJhgkdOuWOUlNZmwfCLRWc \
ghcr.io/immich-app/immich-cli:latest upload --album --recursive /mnt/sda1/Photos/iCloud-evi
