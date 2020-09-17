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

ifeq ($(LVI_MITIGATION), ControlFlow)
        ifeq ($(LVI_MITIGATION_BINDIR),)
            $(error LVI_MITIGATION_BINDIR is not set)
        endif
        CC := $(LVI_MITIGATION_BINDIR)/$(CC)
        COMPILER := $(COMPILER)-lvi-cfg
endif

CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --libs)

all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../modelaggregator.edl --trusted
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 enc.c -o enc.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 modelaggregator_t.c -o modelaggregator_t.o
	$(CC) -o modelaggregatorenc modelaggregator_t.o enc.o $(LDFLAGS)

sign:
	oesign sign -e modelaggregatorenc -c modelaggregator.conf -k private.pem

clean:
	rm -f enc.o modelaggregatorenc modelaggregatorenc.signed private.pem public.pem modelaggregator_t.o modelaggregator_t.h modelaggregator_t.c modelaggregator_args.h

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem
