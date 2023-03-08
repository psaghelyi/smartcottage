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
<<<<<<< HEAD:kubernetes/install-ingress.sh
  service:
    ports:
      http: 8888
      https: 8443
  extraArgs:
    default-ssl-certificate: "default/${CERT_NAME}"
EOF
=======
  extraArgs:
    default-ssl-certificate: "default/${CERT_NAME}"
EOF

  kubectl wait --namespace ingress-nginx \
    --for=condition=ready pod \
    --selector=app.kubernetes.io/component=controller \
    --timeout=90s
>>>>>>> 7b398b9c2d743b51f26771bd0a1b1d4b98499a29:services/kubernetes/install-ingress.sh
}
