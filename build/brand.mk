SHELL := /bin/sh

ENGINE_ROOT ?=
BUILDER_ROOT ?=
BUILD_ROOT ?= build-output
OUTPUT_ROOT ?= dist
CC ?= cc

GAME_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
override ENGINE_ROOT := $(abspath $(ENGINE_ROOT))
override BUILDER_ROOT := $(abspath $(BUILDER_ROOT))
override BUILD_ROOT := $(abspath $(BUILD_ROOT))
override OUTPUT_ROOT := $(abspath $(OUTPUT_ROOT))
ATARI_PROGRAM := $(OUTPUT_ROOT)/mixedres.prg
ATARI_BUILD_HELPER := $(BUILDER_ROOT)/toolchains/atari-ste/build-prg
GAME_CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -Werror

include $(ENGINE_ROOT)/make/brand2-target-sources.mk

.PHONY: atari check check-portable clean

check-portable:
	! grep -E 'brand_engine/internal|brand_engine/targets' \
		$(GAME_ROOT)/src/mixed_resolution_demo.c \
		$(GAME_ROOT)/src/mixed_resolution_demo.h
	$(CC) $(GAME_CFLAGS) -I$(ENGINE_ROOT)/include \
		-I$(GAME_ROOT)/src -fsyntax-only \
		$(GAME_ROOT)/src/mixed_resolution_demo.c

check: check-portable

atari: check-portable
	test -x $(ATARI_BUILD_HELPER)
	mkdir -p $(OUTPUT_ROOT)
	BRAND_ENGINE_ROOT=$(ENGINE_ROOT) $(ATARI_BUILD_HELPER) \
		$(ATARI_PROGRAM) -I$(GAME_ROOT)/src \
		$(GAME_ROOT)/platform/atari-ste/main.c \
		$(GAME_ROOT)/src/mixed_resolution_demo.c \
		$(ENGINE_ATARI_STE_SOURCES)

clean:
	rm -rf $(BUILD_ROOT) $(OUTPUT_ROOT)

