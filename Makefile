CC = gcc
CFLAGS = -std=gnu99 -Wextra -Wall #-Wno-unused-parameter -Werror -fsanitize=address

TARGET = transport

SRCS = check.c network.c
OBJS = check.o network.o

.PHONY: all clean distclean

all = $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
		$(RM) $(OBJS) 

distclean:
		$(RM) $(OBJS) $(TARGET)