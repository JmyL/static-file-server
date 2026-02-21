#!/bin/bash
apt-get update
apt-get install -y docker.io
systemctl enable docker
systemctl start docker

snap install aws-cli --classic

apt-get install -y curl
curl -s https://dl.k6.io/key.gpg | apt-key add -
echo "deb https://dl.k6.io/deb stable main" | tee /etc/apt/sources.list.d/k6.list
apt-get update
apt-get install k6
