CC := cc
TARGET := engine
SRCS := main.c render.c vector_math.c
OBJS := $(SRCS:.c=.o)
HEADERS := render.h vector_math.h

DEBUG ?= 1

CFLAGS := -std=c11 -Wall -Wextra -Werror $(shell pkg-config --cflags sdl3)
LDFLAGS :=
LDLIBS := $(shell pkg-config --libs sdl3) -lm

ifeq ($(DEBUG),1)
  CFLAGS += -g -O0 -fsanitize=address,undefined
  LDFLAGS += -fsanitize=address,undefined
else
  CFLAGS += -O2
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) frame_*.ppm snapshot.ppm test.ppm out.ppm
