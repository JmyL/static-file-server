#! /usr/bin/env bash
# podman run --rm -p 8080:8080 \
#     --name static-server \
#     -v "$PWD/../:/usr/share/nginx/html:ro,z" \
#     -v "$PWD/nginx.conf:/etc/nginx/nginx.conf:ro,z" \
#     nginx:alpine

sudo podman rm -f static-server 2>/dev/null

sudo podman run -d --name static-server \
    -p 8080:8080 \
    -v "$PWD/nginx.conf:/etc/nginx/nginx.conf:ro,z" \
    nginx:alpine

sudo podman cp ../. static-server:/usr/share/nginx/html

sudo podman exec static-server nginx -s reload
