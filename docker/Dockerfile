FROM ubuntu:24.04

ARG TARGET=awaitable_server

RUN apt-get update && \
    apt-get install -y build-essential cmake git libboost-all-dev liburing-dev

WORKDIR /app

RUN git clone https://github.com/jmyl/static-file-server.git .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --target $TARGET

EXPOSE 8080

ENTRYPOINT ["sh", "-c", "./build/$TARGET 8080"]