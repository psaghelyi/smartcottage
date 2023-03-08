#!/bin/bash

docker run -d \
    --restart unless-stopped \
    --name smartcottage \
    --add-host host.docker.internal:host-gateway \
    -p 8888:80 \
    nginx-unit-fastapi
