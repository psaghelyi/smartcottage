#!/bin/bash

docker stop --time=30 prometheus
docker rm -f prometheus
