CC = gcc
CFLAGS = -c
OUTDIR = ./build

OBJS = $(OUTDIR)/main.o $(OUTDIR)/lexer.o $(OUTDIR)/vector.o
EXEC = $(OUTDIR)/bee

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

$(OUTDIR)/main.o: src/main.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/main.c -o $(OUTDIR)/main.o

$(OUTDIR)/lexer.o: src/lexer.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/lexer.c -o $(OUTDIR)/lexer.o

$(OUTDIR)/vector.o: src/vector.o | $(OUTDIR)
	$(CC) $(CFLAGS) src/vector.c -o $(OUTDIR)/vector.o
	
$(OUTDIR):
	mkdir -p $(OUTDIR)

run: $(EXEC)
	build/bee

clean:
	rm -rf $(OUTDIR)/*.o $(EXEC)
