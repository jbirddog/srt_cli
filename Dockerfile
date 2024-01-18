FROM rust:slim

WORKDIR /app

COPY ./ ./

RUN rustup component add rustfmt clippy

RUN \
    --mount=type=cache,target=/var/cache/cargo \
    cargo build
