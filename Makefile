CC = gcc
CFLAGS = -c -Wall
OUTDIR = build

SRCS = $(shell find src -name '*.c')
OBJS = $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRCS))
EXEC = $(OUTDIR)/bee

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

compile: $(EXEC)
	nasm -f elf64 ./build/out.asm -o ./build/out.o
	ld ./build/out.o -o ./build/a.out

clean:
	rm -rf $(OUTDIR) *.out

