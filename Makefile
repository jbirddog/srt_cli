
MY_USER := $(shell id -u)
MY_GROUP := $(shell id -g)
ME := $(MY_USER):$(MY_GROUP)

DOCKER_IMG := srt-cli
DOCKER_RUN_COMMON := --env-file ./docker.env -v .:/app $(DOCKER_IMG)
IN_DEV ?= docker run $(DOCKER_RUN_COMMON)
IN_IDEV ?= docker run -it $(DOCKER_RUN_COMMON)

all: dev-env

dev-env: 
	docker build -t $(DOCKER_IMG) .

compile:
	$(IN_DEV) cargo build --color=never

tests:
	$(IN_DEV) cargo test --color=never

fmt:
	$(IN_DEV) cargo fmt

clippy:
	$(IN_DEV) cargo clippy

fmt-check:
	$(IN_DEV) cargo fmt --check

clippy-check:
	$(IN_DEV) cargo clippy -- -D warnings

check: fmt-check clippy-check
	@/bin/true

sh:
	$(IN_IDEV) /bin/bash

take-ownership:
	sudo chown -R $(ME) .

check-ownership:
	find . ! -user $(MY_USER) ! -group $(MY_GROUP)

.PHONY: dev-env tests fmt fmt-check clippy clippy-check check \
	sh \
	take-ownership check-ownership \
	compile
