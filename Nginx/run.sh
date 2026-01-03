#! /usr/bin/env bash
podman run --rm -p 8080:8080 \
    --name static-server \
    -v "$PWD/../:/usr/share/nginx/html:ro,z" \
    -v "$PWD/nginx.conf:/etc/nginx/nginx.conf:ro,z" \
    nginx:alpine
