#!/bin/bash

docker run -d \
    --restart unless-stopped \
    --name smartcottage \
    --add-host host.docker.internal:host-gateway \
    -p 12345:443 \
    nginx-unit-fastapi
