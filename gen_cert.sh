#!/bin/bash


openssl req \
        -nodes \
        -new \
        -x509 \
        -keyout key.pem \
        -out cert.pem \
        -subj "/CN=psaghelyi.ddns.net" \
        -days 10000

