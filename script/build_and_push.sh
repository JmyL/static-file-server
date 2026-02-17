#!/bin/bash
docker build -f docker/Dockerfile --build-arg TARGET=${SERVER_IMAGE} -t ghcr.io/${{ github.repository }}/${SERVER_IMAGE}:latest .
docker push ghcr.io/jmyl/${{ github.repository }}/${SERVER_IMAGE}:latest