CC = gcc
CFLAGS = -c -Wall
OUTDIR = build

SRCS = $(shell find src -name '*.c')
OBJS = $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRCS))

RUNSRCS = $(shell find src/runtime -name '*.asm')
RUNOBJS = $(patsubst src/runtime/%.asm, $(OUTDIR)/runtime/%.o, $(RUNSRCS))

EXEC = $(OUTDIR)/bee

all: $(EXEC)

$(EXEC): $(OBJS) 
	$(CC) $(OBJS) -o $@

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(OUTDIR)/runtime/%.o: src/runtime/%.asm
	@mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

compile: $(EXEC) $(RUNOBJS)
	nasm -f elf64 ./build/out.asm -o ./build/out.o
	ld ./build/out.o $(RUNOBJS) -o ./build/a.out

clean:
	rm -rf $(OUTDIR) *.out

