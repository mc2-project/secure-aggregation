# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

# Detect C and C++ compiler options
# if not gcc, default to clang-7

COMPILER=$(notdir $(CC))
ifeq ($(COMPILER), gcc)
        USE_GCC = true
endif

ifeq ($(USE_GCC),)
        CC = clang-7
        COMPILER=clang
endif

CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../modelaggregator.edl --untrusted
	$(CC) -g -c $(CFLAGS) host.c
	$(CC) -g -c $(CFLAGS) modelaggregator_u.c
	$(CC) -o modelaggregatorhost modelaggregator_u.o host.o $(LDFLAGS)

clean:
	rm -f modelaggregatorhost host.o modelaggregator_u.o modelaggregator_u.c modelaggregator_u.h modelaggregator_args.h
