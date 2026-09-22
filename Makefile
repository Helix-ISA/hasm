TARGET := hasm

CC := clang

CFLAGS := -Wall -Wextra -Werror -Wpedantic -g -O0
DFLAGS := -MMD -MP
LIBS := -lflagparser

INCS := -Iinclude/

SRCS := $(shell find src -type f -name "*.c")
OBJS := $(SRCS:src/%.c=bin-int/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all build clean dirs lexer parser symbol symbol-pic

all: build

build: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o bin/$@ $(OBJS) $(LIBS)

bin-int/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DFLAGS) $(INCS) -c -o $@ $<

clean:
	$(RM) -r bin bin-int

dirs:
	mkdir -p bin bin-int

lexer:
	./bin/$(TARGET) test/000.hxs --lexer-debug

parser:
	./bin/$(TARGET) test/000.hxs --parser-debug

symbol:
	./bin/$(TARGET) test/000.hxs --symbol-debug

symbol-pic:
	./bin/$(TARGET) test/000.hxs --symbol-debug -pic

-include $(DEPS)
