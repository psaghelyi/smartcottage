#!/bin/bash

sudo useradd -M node_exporter

sudo usermod -L node_exporter

# Create folder

sudo mkdir /var/lib/node_exporter
sudo mkdir /var/lib/node_exporter/textfile_collector
sudo chown -R node_exporter:node_exporter /var/lib/node_exporter

# Create config

sudo mkdir /etc/sysconfig
echo 'OPTIONS="--collector.textfile.directory /var/lib/node_exporter/textfile_collector"' | sudo tee -a /etc/sysconfig/node_exporter

# Copy executable

sudo cp node_exporter-1.4.0.linux-arm64/node_exporter /usr/sbin/node_exporter

# Create service

sudo cp node_exporter.service /etc/systemd/system
sudo systemctl enable node_exporter.service
sudo systemctl start node_exporter.service
