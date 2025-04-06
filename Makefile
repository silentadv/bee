CC = gcc
CFLAGS = -c
OUTDIR = ./build

OBJS = $(OUTDIR)/main.o $(OUTDIR)/lexer.o $(OUTDIR)/parser.o $(OUTDIR)/vector.o 
EXEC = $(OUTDIR)/bee

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

$(OUTDIR)/main.o: src/main.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/main.c -o $(OUTDIR)/main.o

$(OUTDIR)/lexer.o: src/frontend/lexer.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/frontend/lexer.c -o $(OUTDIR)/lexer.o

$(OUTDIR)/parser.o: src/frontend/parser.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/frontend/parser.c -o $(OUTDIR)/parser.o

$(OUTDIR)/vector.o: src/common/vector.c | $(OUTDIR)
	$(CC) $(CFLAGS) src/common/vector.c -o $(OUTDIR)/vector.o
	
$(OUTDIR):
	mkdir -p $(OUTDIR)

run: $(EXEC)
	build/bee

clean:
	rm -rf $(OUTDIR)/*.o $(EXEC)
