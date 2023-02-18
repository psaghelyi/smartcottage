#!/bin/bash

ID=$(id -u)

docker run -d \
    --name grafana \
    --pull=always \
    --restart unless-stopped \
    -p 3000:3000 \
    --add-host host.docker.internal:host-gateway \
    --user $ID \
    -v $PWD/.grafana:/var/lib/grafana \
    grafana/grafana-enterprise

#docker exec -it grafana grafana-cli admin reset-admin-password <pwd>
