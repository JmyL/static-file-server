#!/bin/bash
apt-get update
apt-get install -y docker.io
systemctl enable docker
systemctl start docker

snap install aws-cli --classic
