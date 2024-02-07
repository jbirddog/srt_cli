FROM debian:bookworm-slim

WORKDIR /app

RUN apt-get update \
    && apt-get install -y \
       clang-format \
       gcc \
       make \
       ninja-build \
       valgrind \
    && rm -rf /var/lib/apt/lists/*

COPY ./ ./

RUN ninja
