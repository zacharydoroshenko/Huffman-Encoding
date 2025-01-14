CC=clang
CFLAGS=-Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic

SRCFILES=$(wildcard *.c)
OBJFILES=$(patsubst %.c, %.o, $(SRCFILES))
EXEFILES= brtest bwtest nodetest pqtest huff dehuff

.PHONY: all huff dehuff brtest bwtest nodetest pqtest format clean

all: $(EXEFILES)

huff: huff

dehuff: dehuff

brtest: brtest

bwtest: bwtest

nodetest: nodetest

pqtest: pqtest

format:
	clang-format -i *.c *.h

clean:
	rm -f $(OBJFILES) $(EXEFILES)

brtest: brtest.o bitreader.o 
	$(CC) $^ -o $@

bwtest: bwtest.o bitwriter.o 
	$(CC) $^ -o $@

nodetest: nodetest.o node.o
	$(CC) $^ -o $@

pqtest: pqtest.o pq.o node.o
	$(CC) $^ -o $@

huff: huff.o bitreader.o bitwriter.o pq.o node.o
	$(CC) $^ -o $@

dehuff: dehuff.o bitreader.o bitwriter.o pq.o node.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
