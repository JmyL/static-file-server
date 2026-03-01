#! /usr/bin/env bash
# docker run --rm -p 80:80 \
#     --name static-server \
#     -v "$PWD/../:/usr/share/nginx/html:ro,z" \
#     -v "$PWD/nginx.conf:/etc/nginx/nginx.conf:ro,z" \
#     nginx:alpine

docker rm -f static-server 2>/dev/null

docker run -d --name static-server \
    -p 80:80 \
    -v "$PWD/nginx.conf:/etc/nginx/nginx.conf:ro,z" \
    nginx:alpine

docker cp ../. static-server:/usr/share/nginx/html

docker exec static-server nginx -s reload