#!/bin/bash

# Source the .env file if it exists
if [ -f "$PWD/.env" ]; then
  set -a  # automatically export all variables
  source "$PWD/.env"
  set +a
else
  echo "Warning: .env file not found. Make sure CF_API_TOKEN is set in your environment."
fi

# Explicitly export CF_API_TOKEN to ensure it's available to envsubst
export CF_API_TOKEN

# Debug - check if variable is set
echo "Using CF_API_TOKEN: ${CF_API_TOKEN:-(not set)}"

# Create a temporary configuration file with variables substituted
envsubst < "$PWD/ddclient.conf" > "$PWD/ddclient.conf.tmp"

# Debug - check the generated file
echo "Generated ddclient.conf.tmp with substituted values:"
grep -A 1 "password" "$PWD/ddclient.conf.tmp"

docker run -d \
  --name=ddclient \
  --pull=always \
  --restart=always \
  -e PUID=$(id -u) \
  -e PGID=$(id -g) \
  -e TZ=Europe/Budapest \
  -v "$PWD/ddclient.conf.tmp:/config/ddclient.conf" \
  --restart unless-stopped \
  lscr.io/linuxserver/ddclient:latest

# Clean up the temporary file
#rm "$PWD/ddclient.conf.tmp"