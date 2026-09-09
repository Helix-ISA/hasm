TARGET := hasm

CC := clang

CFLAGS := -Wall -Wextra -Werror -Wpedantic -g -O0
DFLAGS := -MMD -MP

INCS := -Iinclude/

SRCS := $(shell find src -type f -name "*.c")
OBJS := $(SRCS:src/%.c=bin-int/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all build clean dirs lexer parser symbol symbol-pic

all: build

build: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o bin/$@ $(OBJS)

bin-int/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DFLAGS) $(INCS) -c -o $@ $<

clean:
	$(RM) -r bin bin-int

dirs:
	mkdir -p bin bin-int

lexer:
	./bin/$(TARGET) test/000.hxs --lexer-debug | less

parser:
	./bin/$(TARGET) test/000.hxs --parser-debug | less

symbol:
	./bin/$(TARGET) test/000.hxs --symbol-debug | less

symbol-pic:
	./bin/$(TARGET) test/000.hxs --symbol-debug -pic | less

-include $(DEPS)
