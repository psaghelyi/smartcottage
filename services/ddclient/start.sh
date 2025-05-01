#!/usr/bin/env bash
set -euo pipefail  # exit on error

# Load env vars from .env file
set -a
source .env
set +a

# 1. Create stopped container
CID=$(docker create \
        --name ddclient \
        --pull always \
        --restart unless-stopped \
        -e PUID=$(id -u) \
        -e PGID=$(id -g) \
        -e TZ=Europe/Budapest \
        lscr.io/linuxserver/ddclient:latest)

# 2. Generate config and copy it in
tmp=$(mktemp)
envsubst < ddclient.conf > "$tmp"
docker cp "$tmp" "$CID:/config/ddclient.conf"
rm "$tmp"  # clean up temp file

# 3. Start the container
docker start "$CID"
