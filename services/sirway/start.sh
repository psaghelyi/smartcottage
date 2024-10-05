#!/bin/bash

docker run \
  --name=sirway -d \
  --pull=always \
  --restart=always \
  --log-driver=gelf \
  --log-opt gelf-address=udp://localhost:12201 \
  --log-opt tag=sirway \
  -p 8888:5000 \
  psaghelyi.ddns.net:5000/sirway
