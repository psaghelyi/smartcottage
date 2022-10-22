#!/bin/bash

docker container rm -f smartcottage
docker run -d \
    --restart unless-stopped \
    --name smartcottage \
    --add-host host.docker.internal:host-gateway \
    -p 8080:80 \
    -p 12345:443 \
    nginx-unit-fastapi
