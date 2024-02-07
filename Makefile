
MY_USER := $(shell id -u)
MY_GROUP := $(shell id -g)
ME := $(MY_USER):$(MY_GROUP)

DOCKER_IMG := srt-cli
DOCKER_RUN_COMMON := --env-file ./docker.env -v .:/app $(DOCKER_IMG)
IN_DEV ?= docker run --user=$(ME) $(DOCKER_RUN_COMMON)
IN_IDEV ?= docker run -it $(DOCKER_RUN_COMMON)
BUILD_DIR ?= build
TEST_HARNESS_APP ?= $(BUILD_DIR)/test_harness

all: dev-env

dev-env: 
	docker build -t $(DOCKER_IMG) .

compile:
	$(IN_DEV) ninja

start: compile
	$(IN_DEV) $(TEST_HARNESS_APP)

fmt:
	$(IN_DEV) clang-format -i src/*.[ch]

check: compile
	$(IN_DEV) valgrind --tool=memcheck --leak-check=yes $(TEST_HARNESS_APP)

sh:
	$(IN_IDEV) /bin/bash

clean:
	rm -rf $(BUILD_DIR)

take-ownership:
	sudo chown -R $(ME) .

check-ownership:
	find . ! -user $(MY_USER) ! -group $(MY_GROUP)

.PHONY: dev-env fmt check clean \
	sh \
	take-ownership check-ownership \
	compile start
