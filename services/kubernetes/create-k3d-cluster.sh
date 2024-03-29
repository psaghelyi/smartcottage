#/bin/bash

<<<<<<< HEAD:kubernetes/create-k3d-cluster.sh
source helpers.sh
source install-ingress.sh
source install-dashboard.sh
=======
. ./helper.sh
. ./install-ingress.sh
. ./install-dashboard.sh
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/create-k3d-cluster.sh


# Installation variables
CLUSTER_IP=192.168.1.40
CLUSTER_DOMAIN=psaghelyi.ddns.net
CLUSTER_NAME=cluster-1
HOST_IP=$(dig +short host.docker.internal)
API_PORT=6443
HTTP_PORT=8080
HTTPS_PORT=8443
SERVERS=1
AGENTS=2
<<<<<<< HEAD:kubernetes/create-k3d-cluster.sh
=======
VOLUME_PATH=/mnt/sda1/k3s-pv
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/create-k3d-cluster.sh


header "cleanup previous run"
k3d cluster delete -a
footer


header "create cluster"
cat <<EOF  > tmp-${CLUSTER_NAME}.yaml
<<<<<<< HEAD:kubernetes/create-k3d-cluster.sh
  apiVersion: k3d.io/v1alpha3
  kind: Simple
  name: ${CLUSTER_NAME}
=======
  apiVersion: k3d.io/v1alpha4
  kind: Simple
  metadata:
    name: ${CLUSTER_NAME}
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/create-k3d-cluster.sh
  servers: ${SERVERS}
  agents: ${AGENTS}
  kubeAPI:
    hostIP: "0.0.0.0"
    hostPort: "${API_PORT}" # kubernetes api port 6443:6443
  image: rancher/k3s:latest
  #image: rancher/k3s:v1.22.6-k3s1
  volumes:
<<<<<<< HEAD:kubernetes/create-k3d-cluster.sh
    - volume: $(pwd)/.storage:/tmp/k3dvol # volume in host:container
=======
    - volume: ${VOLUME_PATH}:/tmp/k3dvol # volume in host:container
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/create-k3d-cluster.sh
      nodeFilters:
        - all
  ports:
    - port: 0.0.0.0:${HTTP_PORT}:80 # http port host:container
      nodeFilters:
        - loadbalancer
    - port: 0.0.0.0:${HTTPS_PORT}:443 # https port host:container
      nodeFilters:
        - loadbalancer
  env:
    - envVar: secret=token
      nodeFilters:
        - all
  #registries:
  #  create:
  #    name: "k3d-registry.${HOST_IP}.nip.io"
  #    host: "0.0.0.0"
  #    hostPort: "5000"
  options:
    k3d:
      wait: true
      timeout: "60s" # avoid an start/stop cicle when start fails
      disableLoadbalancer: false
      disableImageVolume: false
    k3s:
      extraArgs:
        - arg: --tls-san=127.0.0.1  # Add additional hostname or IP as a Subject Alternative Name in the TLS cert
          nodeFilters:
            - server:*
        - arg: --disable=traefik
          nodeFilters:
            - server:*
    kubeconfig:
      updateDefaultKubeconfig: true # update kubeconfig when cluster starts
      switchCurrentContext: true # change this cluster context when cluster starts
EOF

<<<<<<< HEAD:kubernetes/create-k3d-cluster.sh
K3D_FIX_DNS=1 k3d cluster create --config tmp-${CLUSTER_NAME}.yaml
=======
K3D_FIX_DNS=1 k3d cluster create --config tmp-${CLUSTER_NAME}.yaml --api-port ${API_PORT}
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/create-k3d-cluster.sh
rm tmp-${CLUSTER_NAME}.yaml

kubectl wait --for=condition=Ready nodes --all --timeout=120s
kubectl cluster-info
footer

header "provision persistent volume"
cat <<EOF | kubectl apply -f -
  apiVersion: v1
  kind: PersistentVolume
  metadata:
    name: k3d-pv
    labels:
      type: local
  spec:
    storageClassName: manual
    capacity:
      storage: 50Gi
    accessModes:
      - ReadWriteOnce
    hostPath:
      path: "/tmp/k3dvol"
EOF

kubectl describe pv k3d-pv

installIngress

installDashboard
