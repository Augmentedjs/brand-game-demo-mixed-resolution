ENGINE_ROOT ?= ../../../brand-engine
BUILDER_ROOT ?= ../../brand-builder
BUILD_ROOT ?= build-output
OUTPUT_ROOT ?= dist

.PHONY: atari check clean

atari check:
	$(MAKE) -f build/brand.mk $@ ENGINE_ROOT=$(ENGINE_ROOT) \
		BUILDER_ROOT=$(BUILDER_ROOT) BUILD_ROOT=$(BUILD_ROOT) \
		OUTPUT_ROOT=$(OUTPUT_ROOT)

clean:
	$(MAKE) -f build/brand.mk clean BUILD_ROOT=$(BUILD_ROOT) \
		OUTPUT_ROOT=$(OUTPUT_ROOT)

