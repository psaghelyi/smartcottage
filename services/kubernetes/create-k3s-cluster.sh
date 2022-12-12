#!/bin/bash

export K3S_KUBECONFIG_MODE="600"
export INSTALL_K3S_EXEC=" --no-deploy traefik"
curl -sfL https://get.k3s.io | sh -

