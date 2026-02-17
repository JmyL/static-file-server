#!/bin/bash
apt-get update
apt-get install -y docker.io
systemctl enable docker
systemctl start docker

# Pull and run the server container (listening on port 80)
docker pull ${SERVER_IMAGE}
docker run -d --name test-server -p 80:80 ${SERVER_IMAGE}