#!/bin/bash
apt-get update
apt-get install -y docker.io
systemctl enable docker
systemctl start docker

docker pull ghcr.io/jmyl/static-file-server/server:latest
docker pull nginx:alpine

snap install aws-cli --classic
