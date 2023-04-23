#!/bin/bash

docker run -d \
    --name grafana \
    --pull=always \
    --restart=always \
    --user=$(id -u):$(id -g) \
    -p 3000:3000 \
    --add-host host.docker.internal:host-gateway \
    -v $PWD/.grafana-data:/var/lib/grafana \
    grafana/grafana-enterprise

#docker exec -it grafana grafana-cli admin reset-admin-password <pwd>
