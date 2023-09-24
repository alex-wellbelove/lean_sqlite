
.PHONY: all build run

build:
	lake build

run:
	./build/bin/lean_sqlite


all: build run
	@echo "Nothing to do for $@"
