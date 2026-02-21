FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y build-essential cmake git libboost-all-dev liburing-dev

WORKDIR /app

RUN git clone https://github.com/jmyl/static-file-server.git .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --target server callback_server awaitable_server

RUN echo '#!/bin/sh\n\
if [ "$1" = "--help" ] || [ -z "$1" ]; then\n\
  echo "Usage: podman run --rm -it --security-opt seccomp=unconfined -p 80:80 --entrypoint build/bin/server/awaitable_server <image> 80"\n\
  echo "Note: consider adding --security-opt label=disable on fedora-based systems"\n\
  exit 0\n\
fi\n\
exec "$@"' > /entrypoint.sh && chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]