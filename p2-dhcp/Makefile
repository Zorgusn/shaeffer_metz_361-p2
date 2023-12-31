#
# Simple Makefile
# Mike Lam, James Madison University, August 2016
#
# This makefile builds a simple application that contains a main module
# (specified by the EXE variable) and a predefined list of additional modules
# (specified by the MODS variable). If there are any external library
# dependencies (e.g., the math library, "-lm"), list them in the LIBS variable.
# If there are any precompiled object files, list them in the OBJS variable.
#
# By default, this makefile will build the project with debugging symbols and
# without optimization. To change this, edit or remove the "-g" and "-O0"
# options in CFLAGS and LDFLAGS accordingly.
#
# By default, this makefile build the application using the GNU C compiler,
# adhering to the C99 standard with all warnings enabled.


# application-specific settings and run target

EXE=interp client server
MODS=dhcp.o format.o
OBJS=port_utils.o
LIBS=-lm

default: build $(EXE)

build:
	mkdir build

int: build interp
	make -C tests inttest

cli: build client
	make -C tests itest-cl

pro: build client
	make -C tests itest-proto

srv: build server
	make -C tests itest-serv

# compiler/linker settings

CC=gcc
CFLAGS=-g -O0 -Wall --std=gnu99 -pedantic
LDFLAGS=-g -O0 -pthread


# build targets

BUILD=$(addprefix build/, $(MODS))

build/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

interp: $(BUILD) build/interp.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

client: $(BUILD) build/client.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	cp client tests

server: $(BUILD) build/server.o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	cp server tests

clean:
	rm -rf $(EXE) $(MODS) build
	make -C tests clean

.PHONY: default clean

