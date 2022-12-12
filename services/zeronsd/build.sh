#!/bin/bash

ID=$(id -u)

docker build -t psaghelyi/zeronsd --build-arg VERSION=0.5.2 -f Dockerfile.rpi .

