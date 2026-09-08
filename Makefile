TARGET := hasm

CC := clang

CFLAGS := -Wall -Wextra -Werror -Wpedantic -g -O0
DFLAGS := -MMD -MP

INCS := -Iinclude/

SRCS := $(shell find src -type f -name "*.c")
OBJS := $(SRCS:src/%.c=bin-int/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all build clean dirs test

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

test:
	./bin/$(TARGET)

-include $(DEPS)
