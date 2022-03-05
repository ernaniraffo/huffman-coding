SOURCES = $(wildcard *.c)
OBJ = $(SOURCES:%.c=%.o)
OBJ_ENC = io.o code.o encode.o node.o pq.o huffman.o stack.o
OBJ_DEC = io.o code.o decode.o node.o pq.o huffman.o stack.o

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror

.PHONY = all clean format debug tidy

all: encode decode

debug: CFLAGS += -g
debug: clean all

encode: $(OBJ_ENC)
	$(CC) $(CFLAGS) -o $@ $^

decode: $(OBJ_DEC)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

tidy:
	rm -f $(OBJ)

clean: tidy
	rm -f encode decode

format:
	clang-format -i -style=file *.[ch]
