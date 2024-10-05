#!/bin/bash

docker run \
  --name=sirway -d \
  --pull=always \
  --restart=always \
  -p 8888:80 \
  --log-driver=gelf \
  --log-opt gelf-address=udp://localhost:12201 \
  --log-opt tag=sirway \
  kennethreitz/httpbin
