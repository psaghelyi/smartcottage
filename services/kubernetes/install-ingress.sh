#/!/bin/bash

CERT_NAME=psaghelyi-ddns-net
CERT_FILE=../.cert/psaghelyi_ddns_net.pem-chain
KEY_FILE=../.cert/psaghelyi_ddns_net.key

installIngress ()
{
  header "install NGINX ingress"

  kubectl create secret tls ${CERT_NAME} --key ${KEY_FILE} --cert ${CERT_FILE}

  helm repo add ingress-nginx https://kubernetes.github.io/ingress-nginx
  helm repo update

  cat <<EOF | helm install --namespace ingress --create-namespace -f - ingress-nginx ingress-nginx/ingress-nginx
controller:
  service:
    ports:
      http: 8888
      https: 8443
  extraArgs:
    default-ssl-certificate: "default/${CERT_NAME}"
EOF
}
