#!/bin/bash

docker run -d \
    --name smartcottage \
    --restart=always \
    --add-host host.docker.internal:host-gateway \
    -p 8888:80 \
    --log-driver=gelf \
    --log-opt gelf-address=udp://localhost:12201 \
    --log-opt tag=smartcottage \
    nginx-unit-fastapi
