#!/bin/bash
apt-get update
apt-get install -y docker.io
systemctl enable docker
systemctl start docker

docker pull ${SERVER_IMAGE}
docker run --security-opt seccomp=unconfined -d --name test-server -p 80:80 ${SERVER_IMAGE}