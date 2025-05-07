#!/bin/bash

docker run -it \
-v "/mnt/sda1:/mnt/sda1:ro" \
-e IMMICH_INSTANCE_URL=http://192.168.1.100:2283/api \
-e IMMICH_API_KEY=3aT0njWHLRBc5UxKXbSIAIhvV82T2xIdWZ7l6klb4 \
ghcr.io/immich-app/immich-cli:latest upload --album --recursive /mnt/sda1/Photos/iCloud-peter
