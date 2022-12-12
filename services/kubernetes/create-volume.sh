#!/bin/bash

cat <<EOF | kubectl apply -f -
  apiVersion: v1
  kind: PersistentVolume
  metadata:
    name: k3s-pv
    labels:
      type: local
  spec:
    storageClassName: manual
    capacity:
      storage: 50Gi
    accessModes:
      - ReadWriteOnce
    hostPath:
      path: "/mnt/sda1/k3s-pv"
EOF

